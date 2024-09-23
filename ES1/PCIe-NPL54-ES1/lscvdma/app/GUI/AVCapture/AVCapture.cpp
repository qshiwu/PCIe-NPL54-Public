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
#include <fcntl.h>
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
#include <string>
#include <unistd.h>

#include <linux/videodev2.h>
#define VIDEO_OUTPUT0 "/dev/video0"
#define VIDEO_OUTPUT1 "/dev/video1"
#define VIDEO_OUTPUT2 "/dev/video2"
#define VIDEO_OUTPUT3 "/dev/video3"

#define DUMP_CH3_FRAMES (0)
using namespace std;
bool AppVdma::running = true;
const uint8_t AppVdma::buf_nums = 16;
uint8_t gChannel = 0;
uint32_t gWidth = 0, gHeight = 0;


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

bool AppVdma::GetAppRunning(void) { return running; }

bool AppVdma::GetAppVdmaStopFlag(void) { return dma_stop_flag; }

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
  int fd;

  if (!pVdmaCtlInst->BindingVdmaFrameBuffers(
          (void **)(pMem->GetBufs()), buf_nums, pMem->GetBufSize(), dir)) {
    ERRORSTR("Can't Binding Vdma Frame buffers!\n");
    ret = -1;
    return ret;
  }

    printf("V4L2 Open Video Node \n");

    if(gChannel == 0)
    	fd = open(VIDEO_OUTPUT0, O_RDWR);
    else if(gChannel == 1)
    	fd = open(VIDEO_OUTPUT1, O_RDWR);
    else if(gChannel == 2)
    	fd = open(VIDEO_OUTPUT2, O_RDWR);
    else if(gChannel == 3)
    	fd = open(VIDEO_OUTPUT3, O_RDWR);

    if(fd < 0){
    	perror("Failed to open device, OPEN");
    	return 1;
    }
    
    printf("V4L2 Image Format \n");
        
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(struct v4l2_format));
    fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    
    if(ioctl(fd, VIDIOC_G_FMT, &fmt)< 0){
        perror("Device could not set format, VIDIOC_G_FMT");
        return 1;
    }
    
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY; //V4L2_PIX_FMT_RGB24 V4L2_PIX_FMT_YUYV
    fmt.fmt.pix.width = gWidth;
    fmt.fmt.pix.height = gHeight;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    fmt.fmt.pix.sizeimage = 8 * gWidth * gHeight / 4;  //1920*1080*8/4=4147200
 
    if(ioctl(fd, VIDIOC_S_FMT, &fmt)< 0){
        perror("Device could not set format, VIDIOC_S_FMT");
        return 1;
    }    
    
    printf("V4L2 Image Format Complete \n");

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
      //pAvCapViewInst->RenderFrame(0, 0, 0, NULL);
#ifdef USE_BUF_LOCK
      // next_cpld_idx = (next_cpld_idx + 1) % buf_nums;
      next_cpld_idx = 0;
      new_next_cpld_idx = 0;
      cpld_idx = 0;

      pVdmaCtlInst->RunCtrl(true);
      dma_stop_flag = false;
#endif
      continue;
    } else {
      status = pVdmaCtlInst->GetVdmaUpdatedIdx(&cpld_idx, 5);
    }

#if 1
    if (status && !_baseTime) { // 10
      _baseTime = dwTime;
    } else if (status && _baseTime && (dwTime > _baseTime + 200)) { // 11
      //pAvCapViewInst->RenderFrame(0, 0, 0, NULL);
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
      write(fd, (const char*)pMem->GetBuf(next_cpld_idx), (8 * gWidth * gHeight / 4));  //1920*1080*8/4=4147200

      //pAvCapViewInst->RenderFrame(width, height, stride,
      //                            pMem->GetBuf(next_cpld_idx));
      if (pVdmaCtlInst->CheckFrameBufLockStatus(next_cpld_idx)) {
        pVdmaCtlInst->UnlockFrameBuf(next_cpld_idx);
      }
      pAvCapViewInst->UpdateFrameStatiBuflock(cpld_idx, &next_cpld_idx);
#else // bufloc is dsiabled
      //pAvCapViewInst->RenderFrame(width, height, stride,
      //                            pMem->GetBuf(next_cpld_idx));
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

  close(fd);
  printf("close(fd); \n");
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
  writer.writeFrame(buf, gWidth, gHeight);
#endif
  return ret;
}
#endif

uint32_t getopt_integer(char *optarg) {
  uint32_t val;

  sscanf(optarg, "%d", &val);  //hex -> dec
  return val;
}

static int sGetBoardNumAndChannel(int argc, char *argv[], uint8_t *pBoardNum,
                                  uint8_t *pChannel) {
  int ch = 0;

  *pBoardNum = 0;
  *pChannel = 0;
  while ((ch = getopt(argc, argv, "b:c:w:h:")) != -1) {
    switch (ch) {
    case 'b':
      *pBoardNum = getopt_integer(optarg);
      break;
    case 'c':
      *pChannel = getopt_integer(optarg);
      break;
    case 'w':
      gWidth = getopt_integer(optarg);
      break;
    case 'h':
      gHeight = getopt_integer(optarg);
      break;

    default:
      break;
    }
  }

  if (*pBoardNum < 1) {
    *pBoardNum = 1;
  }

  if (*pChannel > 3) {
    *pChannel = 0;
  }

  if (gWidth < 640 || gWidth > 3840) {
    gWidth = 1920;
  }

  if (gHeight < 480 || gHeight > 2160) {
    gHeight = 1080;
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
  gChannel = channel;

  try {
    AppVdma appInst(boardNum, channel);
    appInst.AppMain();
  } catch (std::exception &e) {
    ERRORSTR(e.what());
  }
  // new architecture end
}
