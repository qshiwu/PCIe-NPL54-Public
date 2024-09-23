/*
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include "AVCapture.h"
#include "AvCapView.h"
#include "MemBuf.h"
#include "MiscUtils.h"
#include "unistd.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fcntl.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string>
#include <unistd.h>

#define DUMP_CH3_FRAMES (0)
using namespace std;
bool AppVdma::running = true;
const uint8_t AppVdma::buf_nums = 16;

ALSAPlayerDevice audio_player;

static inline bool status_is_success(int status) { return (0 == status); }

void AppVdma::GetFuncName(char *pFuncName) {
  switch (ch_id) {
  case 0:
    strncpy(pFuncName, "\\c2h_0", 7);
    break;
  case 1:
    strncpy(pFuncName, "\\c2h_1", 7);
    break;
  case 2:
    strncpy(pFuncName, "\\c2h_2", 7);
    break;
  case 3:
    strncpy(pFuncName, "\\c2h_3", 7);
    break;
  default:
    strncpy(pFuncName, "\\c2h_0", 7);
    break;
  }
}

AppVdma::AppVdma(uint8_t bdNum, uint8_t ch, bool stop_flag)
    : dma_stop_flag(stop_flag), bd_num(bdNum), ch_id(ch) {
  char funcName[256];

  _baseTime = 0;

  GetFuncName(funcName);

  try {

    pVdmaCtlInst = new LscVdmaController(funcName, bdNum, ch_id);
    pMem = new VidBufs(buf_nums, pVdmaCtlInst->GetFrameInitWidth(),
                       pVdmaCtlInst->GetFrameInitHeight());

    pVdmaCtlInst->GetIpVerStr(funcName);
    _ipVer = funcName;
    memset(funcName, 0, sizeof(funcName));
    pVdmaCtlInst->GetDrvVerStr(funcName);
    _swVer = funcName;
    pAvCapViewInst = new AvCapView(_swVer, _ipVer, bd_num, ch_id, buf_nums,
                                   pVdmaCtlInst->IsVdmaSuppAud());
    pAvCapViewInst->SetAppModel(this);
  } catch (LscVdmaControllerError &e) {
    ERRORSTR(e.what());
    throw e;
  } catch (MemBufErrors &e) {
    ERRORSTR(e.what());
    delete pVdmaCtlInst;
    throw e;
  } catch (AvCapViewError &e) {
    ERRORSTR(e.what());
    delete pVdmaCtlInst;
    delete pMem;
    _ipVer.~basic_string();
    _swVer.~basic_string();
    throw e;
  }
}

AppVdma::~AppVdma() {
  if (NULL != pAvCapViewInst) {
    delete pAvCapViewInst;
    pAvCapViewInst = NULL;
  }
  if (NULL != pMem) {
    delete pMem;
    pMem = NULL;
  }

  if (NULL != pVdmaCtlInst) {
    delete pVdmaCtlInst;
    pVdmaCtlInst = NULL;
  }
}

void AppVdma::SetAppRunning(bool run) { running = run; }

bool AppVdma::GetAppRunning(void) { return running; }

void AppVdma::SetAppVdmaStart(bool start) {

  if (NULL == pVdmaCtlInst) {
    ERRORSTR("pVdmaCtlInst is NULL\n");
    return;
  }

  if (start) {
    ERRORSTR("Call pVdmaCtlInst->RunCtrl(true)\n");
    pVdmaCtlInst->RunCtrl(true);
    dma_stop_flag = false;
  } else {
    ERRORSTR("Call pVdmaCtlInst->RunCtrl(false)\n");
    pVdmaCtlInst->RunCtrl(false);
#ifdef USE_BUF_LOCK
    // Added by Zhang Yue on 2024-05-14 start
    uint32_t cpld_idx;
    usleep(10000);
    for (uint8_t idx = 0; idx < buf_nums; idx++) {
      if (pVdmaCtlInst->CheckFrameBufLockStatus(idx)) {
        OutputDebugPrintf("buf idx = %d locked, need to unlock\n", idx);
        // cout << "frame buffer idx" << idx << "locked" << endl;
        pVdmaCtlInst->UnlockFrameBuf(idx);
      }
      // Consume last updated cpld idx, otherwise this idx will be polled by
      // Appmain function after click run/start menu, and this will results no
      // video issue.
    }
    pVdmaCtlInst->GetVdmaUpdatedIdx(&cpld_idx, 5);
    // Added by Zhang Yue on 2024-05-14 end
#endif
    dma_stop_flag = true;
  }
}

bool AppVdma::GetAppVdmaStopFlag(void) { return dma_stop_flag; }

LscVdmaController *AppVdma::GetAppVdmaDrvIf(void) {
  if (NULL != pVdmaCtlInst) {
    return pVdmaCtlInst;
  } else {
    ERRORSTR("In func GetAppVdmaDrvIf  pVdmaCtlInst is NULL \n");
  }

  return NULL;
}

int AppVdma::AppMain(void) {
  int ret = 0;
  uint32_t cpld_idx = 0;
  uint32_t next_cpld_idx = 0;
  uint32_t new_next_cpld_idx = 0;
  uint32_t status = 0;

  uint64_t dwTime = 0;
  uint64_t dwTimePrv = 0;

  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t stride = 0;
  uint32_t dir = 2; // H2C,DMA_TO_DEVICE = 1, C2H,DMA_FROM_DEVICE = 2,

  if (!pVdmaCtlInst->BindingVdmaFrameBuffers(
          (void **)(pMem->GetBufs()), buf_nums, pMem->GetBufSize(), dir)) {
    ERRORSTR("Can't Binding Vdma Frame buffers!\n");
    ret = -1;
    return ret;
  }

  width = pVdmaCtlInst->GetFrameCurrWidth();
  height = pVdmaCtlInst->GetFrameCurrHeight();
  stride = 2 * width;

  dwTimePrv = time_get_ms();
  while (GetAppRunning()) {
    uint64_t dwTime = time_get_ms();
    // below code snippet is used to poll the frame size change, customer can
    // think the code snippet is just an example.
#if 0 
        if (dwTime > dwTimePrv + 4000) {
            pVdmaCtl->UpDateCurrVideoInfo();
            dwTimePrv = dwTime;
            width = pVdmaCtlInst->GetFrameCurrWidth();
            height = pVdmaCtlInst->GetFrameCurrHeight();
            stride = 2 * width;
        }
#endif

    if (GetAppVdmaStopFlag()) {
      sleep(0.5);
      pAvCapViewInst->RenderFrame(0, 0, 0, NULL);
#ifdef USE_BUF_LOCK
      // added by zhang yue on 2024-04-25 start
      // next_cpld_idx = (next_cpld_idx + 1) % buf_nums;
      next_cpld_idx = 0;
      new_next_cpld_idx = 0;
      cpld_idx = 0;
// added by zhang yue on 2024-04-25 end
#endif
      continue;
    } else {
      status = pVdmaCtlInst->GetVdmaUpdatedIdx(&cpld_idx, 5);
    }

#if 1
    if (status && !_baseTime) { // 10
      _baseTime = dwTime;
    } else if (status && _baseTime && (dwTime > _baseTime + 200)) { // 11
      pAvCapViewInst->RenderFrame(0, 0, 0, NULL);
      // OutputDebugPrintf("curr_time:%llu, baseTime:%llu", dwTime, _baseTime);
      // ERRORSTR("Time out render black screen one time\n");
      continue;
    } else if (!status && _baseTime) { // 01
      _baseTime = 0;
    } // 00 do nothing.
#endif

    if ((next_cpld_idx != new_next_cpld_idx) || status_is_success(status)) {

      new_next_cpld_idx = (cpld_idx + 1) % buf_nums;
#if (DEBUG_DUMP_FRAMES)
      DumpFrame(pMem->GetBuf(next_cpld_idx), next_cpld_idx, stride * height);
#endif

#ifdef USE_BUF_LOCK
      pAvCapViewInst->RenderFrame(width, height, stride,
                                  pMem->GetBuf(next_cpld_idx));
      if (pVdmaCtlInst->CheckFrameBufLockStatus(next_cpld_idx)) {
        pVdmaCtlInst->UnlockFrameBuf(next_cpld_idx);
      }
      pAvCapViewInst->UpdateFrameStatiBuflock(cpld_idx, &next_cpld_idx);
#else // bufloc is dsiabled
      pAvCapViewInst->RenderFrame(width, height, stride,
                                  pMem->GetBuf(next_cpld_idx));
      pAvCapViewInst->UpdateFrameStati(cpld_idx, &next_cpld_idx);
#endif
    } else {
#if 0
      if (pAvCapViewInst->GetFrameTotal() > 1) {
        pAvCapViewInst->RenderFrame(width, height, stride,
                                    pMem->GetBuf(next_cpld_idx));
      }
#endif      
    }
  }

  OUTPUT_DEBUG_PRINTF("Exit!\n");

  if (!pVdmaCtlInst->UnBindingVdmaframBuffers()) {
    ERRORSTR("Can't Binding Vdma Frame buffers!\n");
    OUTPUT_DEBUG_PRINTF("Can't Binding Vdma Frame buffers!!\n");
  } else {
    OUTPUT_DEBUG_PRINTF("UnBindingVdmaframBuffers success!\n");
  }

  return ret;
}

void AppVdma::signal_handler(int sig) {
  if (SIGINT == sig) {
    running = false;
  }
}

#if (DEBUG_DUMP_FRAMES)
int AppVdma::DumpFrame(const void *buf, uint8_t cpld_idx, uint64_t size) {
  int ret = 0;
  size_t wt_cnt = 0;
  FILE *file_hd = NULL;
  char file_name[100];

  if (ch_id > 3 || buf == NULL) {
    ret = -1;
    return ret;
  }

  if (NULL == pAvCapViewInst || pAvCapViewInst->GetFrameTotal() > 120) {
    ret = -2;
    return ret;
  }

  snprintf(file_name, 99, "ch%d_dmp_gTotal_%d_idx_%d", ch_id,
           pAvCapViewInst->GetFrameTotal(), cpld_idx);

#if 0
  file_hd = fopen(file_name, "wb+");

  if (file_hd == NULL) {
    ret = -3;
    cout << "Open file: " << file_name << " failed" << endl;
    return ret;
  }

  wt_cnt = fwrite((const void *)buf, sizeof(char), size, file_hd);
  cout << "write " << wt_cnt << "bytes to file: " << file_name << endl;

  ret = fclose(file_hd);
#else
  YUV422Writer writer(file_name);
  writer.writeFrame(buf, 1920, 1080);
#endif
  return ret;
}
#endif
// Added by Zhang Yue on 2023-11-29 start
bool AppVdma::GetPciCfgRegs(uint8_t *regs) {
  bool bRet = false;
  if (pVdmaCtlInst != NULL) {
    pVdmaCtlInst->GetVdmaCfgSpace(regs);
    bRet = true;

  } else {
    ERRORSTR("Vdma Lib Controller Instance pointer is NULL\n");
    bRet = false;
  }
  return bRet;
}
// Added by Zhang Yue on 2023-11-29 end

void *audio_player_func(void *arg) {
#if 0
    audio_info_t *info;
    
    info = (audio_info_t *)arg;
    do{
        usleep(1000);
    } while (!info->valid);
    audio_player.create_device("default");
    DebugPrint("Current audio sample freq: %d, audio data size: %d \n", info->cur_aud_fs, info->aud_size);
    
    audio_player.SetFormat(info->cur_aud_fs, CAPTURE_CHANNEL, CAPTURE_SAMPLE_PER_FRAME);
    audio_player.SetVolume(0, 90, 100);
    //DebugPrint("audio_player thread enter");
    while(g_running & g_audio_capture){
        if(g_afw_index == g_afr_index_play){
            usleep(10000);
            continue;
        }
        if(g_audio_switch) {
            audio_player.write_data(g_audio_frame[g_afr_index_play], CAPTURE_SAMPLE_PER_FRAME);
            g_afr_index_play++;         
        }
        else {
            //audio_player.set_pcm_silence(g_audio_frame[0], CAPTURE_SAMPLE_PER_FRAME);
        }

        if(AUDIO_FRAME_NUM == g_afr_index_play){
            g_afr_index_play = 0;
        }
    }
    DebugPrint("audio_player thread exit");
#endif

  return NULL;
}

#if 0
void audio_capture(void **buf, uint32_t idx, audio_info_t *info)
{
    uint32_t k;
    uint32_t *audio_buf;
    static uint32_t vfr_cnt = 0;
    static uint32_t aud_cnt = 0;
    
    audio_buf = (uint32_t *)buf[idx] + 1036800;
    if (vfr_cnt < CAPTURE_VFR_NUM - 1) {
        audio_play_buf = g_audio_frame[g_afw_index]; 
        for (k = 1; k <= AUDIO_PAYLOAD_PER_FRAME; k++){
            // Check audio data valid
            if (0x00 == (audio_buf[k-1] & 0x18000000) >> 27){
                DebugPrint("audio data");
                //fprintf(afp, "[%04d]Audio Data: %08x (invalid+)\n", k-1, audio_buf[k-1]);
                continue;
            }
            else {
                //fprintf(afp, "[%04d]Audio Data: %08x\n", k-1, audio_buf[k-1]);
                audio_play_buf[aud_cnt++] = (audio_buf[k-1] & 0x0000FF00) >> 8;
                audio_play_buf[aud_cnt++] = (audio_buf[k-1] & 0x000000FF);
            }
        }
        vfr_cnt++;
    }
    else {
        for (k = 1; k <= AUDIO_PAYLOAD_PER_FRAME; k++) {
            if (0x00 == (audio_buf[k-1] & 0x18000000) >> 27){
                //DebugPrint("Invalid audio data-");
                //fprintf(afp, "[%04d]Audio Data: %08x (invalid-)\n", k-1, audio_buf[k-1]);
                continue;
            }
            else {
                //fprintf(afp, "[%04d]Audio Data: %08x\n", k-1, audio_buf[k-1]);
                audio_play_buf[aud_cnt++] = (audio_buf[k-1] & 0x0000FF00) >> 8;
                audio_play_buf[aud_cnt++] = (audio_buf[k-1] & 0x000000FF); 
            }
        }
        audio_info_parse(audio_buf[k-2], info);
        g_afw_index++;
        vfr_cnt = 0;
        aud_cnt = 0;      
        if(AUDIO_FRAME_NUM == g_afw_index) {
            g_afw_index = 0;
        }
    }
}
#endif

uint32_t getopt_integer(char *optarg) {
  uint32_t val;

  sscanf(optarg, "%x", &val);
  return val;
}

static int sGetBoardNumAndChannel(int argc, char *argv[], uint8_t *pBoardNum,
                                  uint8_t *pChannel) {
  int ch = 0;

  *pBoardNum = 0;
  *pChannel = 0;
  while ((ch = getopt(argc, argv, "b:c:")) != -1) {
    switch (ch) {
    case 'b':
      *pBoardNum = getopt_integer(optarg);
      break;
    case 'c':
      *pChannel = getopt_integer(optarg);
      break;
    default:
      break;
    }
  }

  if (*pBoardNum < 1) {
    *pBoardNum = 1;
  }

  if (*pChannel > 3) {
    *pBoardNum = 3;
  }

  return 0;
}
/**
 * Main Entry Point
 * Creates the OpenGL window, opens the VDMA driver, starts reads from
 * IP module and displays the pixel data in the OpenGL window.
 */

int main(int argc, char *argv[]) {

  int ret = 0;
  char functionName[25];
  uint8_t boardNum = 1;
  uint8_t channel = 0;
  uint64_t dwTime = 0;
  uint64_t dwTimePrv = 0;

  signal(SIGINT, AppVdma::signal_handler);
  strncpy(functionName, "\\c2h_0", 7);

  sGetBoardNumAndChannel(argc, argv, &boardNum, &channel);

  OutputDebugPrintf("boardNum = %d channel = %d \n", boardNum, channel);

  try {
    AppVdma appInst(boardNum, channel);
    appInst.AppMain();
  } catch (std::exception &e) {
    ERRORSTR(e.what());
  }
  // new architecture end
}
