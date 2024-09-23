/*
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
#include <errno.h>
#include <exception>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <syslog.h>
#include <time.h>

#include "LscVdmaController.h"

using namespace std;

static const char *vid_fmt_str[] = {
    "Normal RGB", "YUV 422", "YUV 444", "YUV 420", "Y Only", "RAW", "RGB565"};
static const char *aud_smp_rate_str[] = {"32KHz", "44.1KHz", "48KHz", "96KHz",
                                         "192KHz"};

#define IS_USE_OUTPUT_DEBUG_PRINT 1

#if (IS_USE_OUTPUT_DEBUG_PRINT)

#define OUTPUT_DEBUG_PRINTF(str) OutputDebugPrintf(str)
static void OutputDebugPrintf(const char *strOutputString, ...) {
#define PUT_PUT_DEBUG_BUF_LEN 1024
  char strBuffer[PUT_PUT_DEBUG_BUF_LEN] = {0};
  va_list vlArgs;
  va_start(vlArgs, strOutputString);
  vsnprintf(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
  // vsprintf(strBuffer,strOutputString,vlArgs);
  va_end(vlArgs);
  syslog(LOG_ERR, "%s\n", strBuffer);
  cout << strBuffer << endl;
}
#else
#define OUTPUT_DEBUG_PRINTF(str)
#endif

long LscVdmaController::GetVdmaInitFrameSize(void) {
  uint32_t frame_width = 0;
  uint32_t frame_height = 0;

  uint32_t reg_offset = 0x020;

  switch (ch_id) {
  case 0:
    reg_offset = 0x020;
    break;
  case 1:
    reg_offset = 0x420;
    break;
  case 2:
    reg_offset = 0x820;
    break;
  case 3:
    reg_offset = 0xC20;
    break;
  default:
    break;
  }

  frame_width = ((pDrvIf->read32(reg_offset)) & 0xFFFF0000) >> 16;
  frame_height = ((pDrvIf->read32(reg_offset)) & 0x0000FFFF);

  if (frame_width * frame_height % 16 != 0) {
    throw(LscVdmaControllerError(
        "The size of frame buffer must be a multiple of 16 bytes!"));
  }

  width_init = frame_width;
  height_init = frame_height;

  frame_size_init = width_init * height_init;

  return frame_size_init;
}

long LscVdmaController::GetVdmaCurrFrameSize(void) {
  uint32_t frame_width = 0;
  uint32_t frame_height = 0;

  uint32_t reg_offset = 0x020;

  switch (ch_id) {
  case 0:
    reg_offset = 0x268;
    break;
  case 1:
    reg_offset = 0x668;
    break;
  case 2:
    reg_offset = 0xA68;
    break;
  case 3:
    reg_offset = 0xE68;
    break;
  default:
    break;
  }
  frame_width = ((pDrvIf->read32(reg_offset)) & 0xFFFF0000) >> 16;
  frame_height = ((pDrvIf->read32(reg_offset)) & 0x0000FFFF);

  if (frame_width * frame_height % 16 != 0) {
    width = 0;
    height = 0;
    frame_size = 0;
    throw(LscVdmaControllerError(
        "The size of current frame must be a multiple of 16 bytes!"));
  }

  width = frame_width;
  height = frame_height;

  frame_size = width * height;
  return frame_size;
}

void LscVdmaController::GetVdmaInitAVInfo() {
  uint32_t reg_val = 0;

  uint32_t reg_av_info = 0x020;

  switch (ch_id) {
  case 0:
    reg_av_info = 0x024;
    break;
  case 1:
    reg_av_info = 0x424;
    break;
  case 2:
    reg_av_info = 0x824;
    break;
  case 3:
    reg_av_info = 0xC24;
    break;
  default:
    break;
  }

  reg_val = pDrvIf->read32(reg_av_info);

  if ((reg_val & BIT_MASK_VID_FORMAT) > 6) {
    OutputDebugPrintf("VDMA config wrong, it reports wrong color space!!\n");
    return;
  }
  color_space_init =
      (static_cast<vdma_color_space_t>(reg_val & BIT_MASK_VID_FORMAT));

  if (((reg_val & BIT_MASK_COLOR_DEPTH) >> 4) > 13) {
    OutputDebugPrintf("VDMA config wrong, it reports wrong color depth!!\n");
    return;
  }
  color_depth_init = ((reg_val & BIT_MASK_COLOR_DEPTH) >> 4) + 4;

  if (((reg_val & BIT_MASK_AUD_MAX_SMPL_RATE) >> 24) > 4) {
    OutputDebugPrintf(
        "VDMA config wrong, it reports wrong aud max sample rate!\n");
    return;
  }
  max_aud_fs_init = (static_cast<vdma_aud_samp_rate_t>(
      (reg_val & BIT_MASK_AUD_MAX_SMPL_RATE) >> 24));

  return;
}

void LscVdmaController::GetVdmaCurrAVInfo(void) {
  uint32_t reg_val = 0;

  uint32_t reg_av_info = 0x020;

  switch (ch_id) {
  case 0:
    reg_av_info = 0x26C;
    break;
  case 1:
    reg_av_info = 0x66C;
    break;
  case 2:
    reg_av_info = 0xA6C;
    break;
  case 3:
    reg_av_info = 0xE6C;
    break;
  default:
    break;
  }
  reg_val = pDrvIf->read32(reg_av_info);

  if ((reg_val & BIT_MASK_VID_FORMAT) > 6) {
    OutputDebugPrintf("VDMA config wrong, it reports wrong color space!\n");
    return;
    // assert(0);
  }

  color_space =
      (static_cast<vdma_color_space_t>(reg_val & BIT_MASK_VID_FORMAT));

  if (((reg_val & BIT_MASK_COLOR_DEPTH) >> 4) > 13) {
    OutputDebugPrintf("VDMA config wrong, it reports wrong color depth!\n");
    return;
  }
  color_depth = ((reg_val & BIT_MASK_COLOR_DEPTH) >> 4) + 4;

  if (((reg_val & BIT_MASK_AUD_MAX_SMPL_RATE) >> 24) > 4) {
    OutputDebugPrintf(
        "VDMA config wrong, it reports wrong aud max sample rate!\n");
    return;
  }

  max_aud_fs = (static_cast<vdma_aud_samp_rate_t>(
      (reg_val & BIT_MASK_AUD_MAX_SMPL_RATE) >> 24));

  return;
}

void LscVdmaController::UpDateCurrVideoInfo(void) {
  GetVdmaCurrFrameSize();
  GetVdmaCurrAVInfo();
}

uint8_t LscVdmaController::GetMaxSupportchannels(void) {
  uint8_t val = 0x00;
  // Bit7~6, 00-not support audio, 01-C2H audio, 10-H2C audio, 11-H2C and C2H
  // audio
  val = ((pDrvIf->read32(REG_C2H_CAP)) & REG_C2H_CAP_MASK_MAX_CH_SUPPORTED) >>
        REG_C2H_CAP_MASK_MAX_CH_SHIFT;

  switch (val) {
  case 0:
    max_supp_ch = 1;
    break;
  case 1:
    max_supp_ch = 2;
    break;
  case 2:
    max_supp_ch = 3;
    break;
  case 3:
    max_supp_ch = 4;
    break;
  default:
    max_supp_ch = 0;
  }

  return max_supp_ch;
}

// added by zhang yue on 2023-11-30 start
void LscVdmaController::GetVdmaCfgSpace(uint8_t *pPciCfg) {
  if (pPciCfg != NULL) {
    memcpy(pPciCfg, pci_cfg, 256);
  }
}
// added by zhang yue on 2023-11-30 end

LscVdmaController::LscVdmaController(const char *pFunctionName, uint8_t bdNum,
                                     uint8_t channel_id)
    : bd_num(bdNum), ch_id(channel_id) {
  char boardName[256], demoName[256], functionName[256];
  string verStr;
  uint32_t vMaj, vMin;

  max_buf_num = 16;
  buf_lock = false;

  snprintf(boardName, 256, "lscvdma%d", (bd_num - 1));
  strncpy(demoName, "c2h", 256);
  strncpy(functionName, pFunctionName, 256);

  try {
    pDrvIf = new LSCVDMA_IF(boardName, demoName, ch_id);
    if (pDrvIf->getDriverVersionStr(verStr)) {
      strncpy(sw_ver, verStr.c_str(), MAX_VER_STRING_SIZE);
    }
    verStr.clear();
    if (pDrvIf->getIPVersionStr(verStr)) {
      // OutputDebugPrintf("PCI IP Version: ");
      // OutputDebugPrintf("%s\n", verStr.c_str());
      strncpy(ip_ver, verStr.c_str(), MAX_VER_STRING_SIZE);
    }

    if ((ch_id + 1) > GetMaxSupportchannels()) {
      OutputDebugPrintf("this channel not supported by vdma IP bitstream\n");
      throw 1;
    }

    GetVdmaIpVer(&vMaj, &vMin);

    if (vMaj > 2 || (vMaj == 2 ? vMin > 0 : vMin > 11)) {

      OutputDebugPrintf("VDMA IP version not match with SW! Please click 'OK' "
                        "button to exit application.\n");
      throw 2;
    }

    GetVdmaInitFrameSize();
    GetVdmaInitAVInfo();
    GetVdmaCurrFrameSize();
    GetVdmaCurrAVInfo();

    if (IsVdmaSuppAud()) {
      GetVdmaAudBufSize();
    }

    poll_mode = GetVdmaFrameIdxUpdateMode();

    if (poll_mode) {
      SetGenIrqframeCnt(1);
    } else {
      SetGenIrqframeCnt(4);
    }

#ifdef USE_BUF_LOCK
    SetBufLockMode(true);
#endif

    // Added by zhang yue on 2023-11-29 start
    pDrvIf->getPCIConfigRegs(pci_cfg);
    // Added by zhang yue on 2023-11-29 end

    PrintInfo();

  } catch (LSCVDMA_IF_Error &e) {
    OutputDebugPrintf("%s \n", e.what());
    throw(LscVdmaControllerError("Create vdma driver interface failed"));
  } catch (int &e) {
    delete pDrvIf;
    pDrvIf = NULL;

    if (e == 1) {
      throw(LscVdmaControllerError("IP not implement this channel"));
    } else {
      throw(
          LscVdmaControllerError("ERROR VDMA IP version not match SW version"));
    }
  }
}

LscVdmaController::~LscVdmaController() {
  if (pDrvIf)
    delete pDrvIf;
}

void LscVdmaController::RunCtrl(bool on) {
  uint32_t val;

  val = pDrvIf->read32(REG_C2H_CR);
  if (on) {
    OutputDebugPrintf("Really Enable vdma transfer for channel: %d\n", ch_id);
    switch (ch_id) {
    case 0:
      pDrvIf->write32(REG_C2H_CR, val | 0x10);
      break;
    case 1:
      pDrvIf->write32(REG_C2H_CR, val | 0x20);
      break;
    case 2:
      pDrvIf->write32(REG_C2H_CR, val | 0x40);
      break;
    case 3:
      pDrvIf->write32(REG_C2H_CR, val | 0x80);
      break;
    default:
      pDrvIf->write32(REG_C2H_CR, val | 0x10);
      break;
    }
  } else {
    OutputDebugPrintf("Really Disable vdma transfer for channel: %d ", ch_id);
    switch (ch_id) {
    case 0:
      pDrvIf->write32(REG_C2H_CR, val & 0xFFFFFFEF);
      break;
    case 1:
      pDrvIf->write32(REG_C2H_CR, val & 0xFFFFFFDF);
      break;
    case 2:
      pDrvIf->write32(REG_C2H_CR, val & 0xFFFFFFBF);
      break;
    case 3:
      pDrvIf->write32(REG_C2H_CR, val & 0xFFFFFF7F);
      break;
    default:
      pDrvIf->write32(REG_C2H_CR, val & 0xFFFFFFEF);
      break;
    }
  }

  return;
}

void LscVdmaController::SetBufLockMode(bool on) {
  uint32_t val;

  if (on == 1) {
    val = pDrvIf->read32(REG_C2H_CR);
    // pDrvIf->write32(REG_C2H_CR, val | 0x00008000);
    pDrvIf->write32(REG_C2H_CR, val | (0x00001000 << ch_id));
    buf_lock = true;
    SetGenIrqframeCnt(1);
  } else {
    val = pDrvIf->read32(REG_C2H_CR);
    // pDrvIf->write32(REG_C2H_CR, val & (~0x00008000));
    pDrvIf->write32(REG_C2H_CR, val & (~(0x00001000 << ch_id)));
    buf_lock = false;
    SetGenIrqframeCnt(4); // default to 4
  }

  return;
}

bool LscVdmaController::CheckFrameBufLockStatus(uint8_t idx) {
  uint32_t val = 0;

  if (pDrvIf == NULL || idx > 15) {
    OutputDebugPrintf("idx =:%d > 15 or pDrvIf is NULL\n", idx);
    return false;
  }

  pDrvIf->read32(REG_C2H_BUF_LOCK(this->ch_id), &val, 1);
  if (val & (0x0001 << idx)) {
    // OutputDebugPrintf("idx =:%d  val = 0x%x\n", idx, val);
    return true;
  } else
    return false;
}

void LscVdmaController::UnlockFrameBuf(uint8_t idx) {
  if (idx > 15 || NULL == pDrvIf) {
    OutputDebugPrintf("idx =:%d > 15 or pDrvIf is NULL\n", idx);
    return;
  }

  return pDrvIf->write32(REG_C2H_BUF_LOCK(this->ch_id), 0x0001 << idx);
}

void LscVdmaController::SetGenIrqframeCnt(uint8_t number) {
  uint32_t val;

  val = pDrvIf->read32(REG_C2H_IRQ_COUNTER(this->ch_id));
  if (number == 1) {
    pDrvIf->write32(REG_C2H_IRQ_COUNTER(this->ch_id),
                    (val & (~0x0000000F)) | 0x01);
    gen_irq_frm_cnt = 1;
  } else if (number == 2) {
    pDrvIf->write32(REG_C2H_IRQ_COUNTER(this->ch_id),
                    (val & (~0x0000000F)) | 0x02);
    gen_irq_frm_cnt = 2;
  } else if (number == 4) {
    pDrvIf->write32(REG_C2H_IRQ_COUNTER(this->ch_id),
                    (val & (~0x0000000F)) | 0x04);
    gen_irq_frm_cnt = 4;
  } else {
    // use default value, trigger an interrupt when 4 frames transfer is done.
    gen_irq_frm_cnt = 4;
  }
  return;
}

uint8_t LscVdmaController::GetGenIrqframeCnt(void) const {
  return this->gen_irq_frm_cnt;
}

void LscVdmaController::GetVdmaIpVer(uint32_t *maj, uint32_t *min) {
  uint32_t val;

  val = pDrvIf->read32(REG_C2H_VER);
  *maj = (val & 0xF0000000) >> 28;
  *min = (val & 0x0FF00000) >> 20;

  return;
}

bool LscVdmaController::GetVdmaFrameIdxUpdateMode(void) const {
  uint32_t val;

  val = pDrvIf->read32(REG_C2H_CR);
  OutputDebugPrintf("get_vdma_xfer_mode REG_C2H_CR value is 0x%08x\n", val);
  return (val & VDMA_C2H_CR_INT_METHOD_MASK) ? true : false;
}

bool LscVdmaController::IsVdmaSuppAud(void) {
  uint32_t val;

  // Bit7~6, 00-not support audio, 01-C2H audio, 10-H2C audio, 11-H2C and C2H
  // audio
  val = ((pDrvIf->read32(REG_C2H_CAP)) & 0xC0) >> 6;

  audio_cap = val ? true : false;

  return audio_cap;
}

uint32_t LscVdmaController::GetVdmaAudBufSize() {
  uint32_t val;
  uint32_t REG_C2H_AVI_INFO = 0x024;
  uint32_t audio_buf_size = 1600;

  val = ((pDrvIf->read32(REG_C2H_AVI_INFO)) & 0x0F000000) >> 24;
  if (val == 4) {
    OutputDebugPrintf("Support max audio sample freq: 192kHz\n");
    max_aud_fs_init = vdma_aud_samp_rate::AUD_SMP_RATE_192K;
    audio_buf_size = 6560; // 192kHz
  } else if (val == 3) {
    OutputDebugPrintf("Support max audio sample freq: 96kHz\n");
    max_aud_fs_init = vdma_aud_samp_rate::AUD_SMP_RATE_96K;
    audio_buf_size = 3280; // 96kHz
  } else if (val == 2) {
    OutputDebugPrintf("Support max audio sample freq: 48kHz\n");
    max_aud_fs_init = vdma_aud_samp_rate::AUD_SMP_RATE_48K;
    audio_buf_size = 1600; // 48kHz, 44.1kHz, 32kHz-1640
  } else {                 // as other value should be defau to 48k
    OutputDebugPrintf("Support max audio sample freq: 48kHz\n");
    max_aud_fs_init = vdma_aud_samp_rate::AUD_SMP_RATE_48K;
    audio_buf_size = 1600; // 48kHz, 44.1kHz, 32kHz-1640
  }

  return audio_buf_size;
}

void LscVdmaController::PrintInfo(void) const {
  OutputDebugPrintf("PCI Driver Version: %s \n", sw_ver); //  ( endl ;
  OutputDebugPrintf("PCI IP Version: %s \n", ip_ver);

  OutputDebugPrintf("Video resolution: %d x %d\n", width_init,
                    height_init); //  endl;
  OutputDebugPrintf("Video colour space: %s\n",
                    vid_fmt_str[static_cast<int>(color_space_init)]); // endl;
  OutputDebugPrintf("Video colour depth: %d\n ",
                    static_cast<int>(color_depth_init));
  OutputDebugPrintf("Support max buffer number: %d\n", max_buf_num);
  OutputDebugPrintf("Trigger an interrupt per :%d frames\n", gen_irq_frm_cnt);

  if (poll_mode)
    OutputDebugPrintf("Transfer Method: Polling mode\n"); // endl;
  else
    OutputDebugPrintf("Transfer Method: MSI\n"); // endl;

  if (audio_cap) {
    OutputDebugPrintf("Audio feature support: Yes\n"); // endl;;
    OutputDebugPrintf("Audio supported max audio sample rate is %s\n",
                      aud_smp_rate_str[static_cast<int>(max_aud_fs_init)]);
  } else {
    OutputDebugPrintf("Audio feature: No\n"); // endl;;
  }
  if (buf_lock)
    OutputDebugPrintf("Buffer lock feature: on\n"); // endl;
  else
    OutputDebugPrintf("Buffer lock feature: off\n"); //  endl;

  return;
}

uint32_t LscVdmaController::GetFrameInitWidth(void) const { return width_init; }
uint32_t LscVdmaController::GetFrameInitHeight(void) const {
  return height_init;
}
long LscVdmaController::GetFrameInitSize(void) const { return frame_size_init; }
vdma_color_space_t LscVdmaController::GetFrameInitColorSpace(void) const {
  return color_space_init;
}
uint32_t LscVdmaController::GetFrameCurrWidth(void) const { return width; }
uint32_t LscVdmaController::GetFrameCurrHeight(void) const { return height; }
long LscVdmaController::GetFrameCurrSize(void) const { return frame_size; }
vdma_color_space_t LscVdmaController::GetFrameCurrColorSpace(void) const {
  return color_space;
}
vdma_aud_samp_rate_t LscVdmaController::GetAudioMaxFs(void) const {
  return max_aud_fs_init;
}
uint32_t LscVdmaController::GetAudioBufSize(void) const {
  return audio_buf_size;
}

void LscVdmaController::GetIpVerStr(char *pIpVer) const {
  size_t szIp = 0;
  if (NULL != pIpVer) {
    szIp = strnlen(ip_ver, MAX_VER_STRING_SIZE);
    strncpy(pIpVer, ip_ver, szIp + 1);
  }
}

void LscVdmaController::GetDrvVerStr(char *pSwVer) const {
  size_t szIp = 0;
  if (NULL != pSwVer) {
    szIp = strnlen(sw_ver, MAX_VER_STRING_SIZE);
    strncpy(pSwVer, sw_ver, szIp + 1);
  }
}

bool LscVdmaController::BindingVdmaFrameBuffers(void **bufs, size_t bufs_num,
                                                size_t buf_size, uint32_t dir) {
  try {
    struct vdma_ubuf_ioctl_arg userptr_arg;
    userptr_arg.addr = bufs;
    userptr_arg.len = buf_size;
    userptr_arg.buf_cnt = bufs_num;
    userptr_arg.dir = dir;
    return (0 == pDrvIf->vdmaStartCtrl(true, &userptr_arg));
  } catch (LSCVDMA_IF_Error &e) {
    OutputDebugPrintf("%s\n", e.what()); // endl;
    return false;
  }
}
bool LscVdmaController::UnBindingVdmaframBuffers(void) {
  try {
    return (0 == pDrvIf->vdmaStartCtrl(false, NULL));
  } catch (LSCVDMA_IF_Error &e) {
    OutputDebugPrintf("%s\n", e.what()); // endl;
    return false;
  }
}

int LscVdmaController::GetVdmaUpdatedIdx(uint32_t *p_idx, uint32_t timeout_ms) {
  return pDrvIf->vdmaGetVideoFrame(p_idx, timeout_ms);
}

bool LscVdmaController::read8(uint32_t addr, uint8_t *val, size_t len) {
  return pDrvIf->read8(addr, val, len);
}
bool LscVdmaController::write8(uint32_t addr, uint8_t *val, size_t len) {
  return pDrvIf->write8(addr, val, len);
}
bool LscVdmaController::read16(uint32_t addr, uint16_t *val, size_t len) {
  return pDrvIf->read16(addr, val, len);
}
bool LscVdmaController::write16(uint32_t addr, uint16_t *val, size_t len) {
  return pDrvIf->write16(addr, val, len);
}
bool LscVdmaController::read32(uint32_t addr, uint32_t *val, size_t len) {
  return pDrvIf->read32(addr, val, len);
}
bool LscVdmaController::write32(uint32_t addr, uint32_t *val, size_t len) {
  return pDrvIf->write32(addr, val, len);
}
