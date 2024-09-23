/*
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#ifndef __LSC_VDMA_CONTROLLER__H
#define __LSC_VDMA_CONTROLLER__H

#include "LSCVDMA_IF.h"

#define DLLIMPORT
//#define  USE_BUF_LOCK

#define MAX_VER_STRING_SIZE (256)
#define MAX_MSG_SIZE 256
#define C2H_CHANNEL_MAX 4
#define DEBUG

#define REG_C2H_VER 0x000
#define REG_C2H_CAP 0x004
#define REG_C2H_CAP_MASK_MAX_CH_SUPPORTED 0x00F00000
#define REG_C2H_CAP_MASK_MAX_CH_SHIFT (20)

#define REG_C2H_CR 0x008
#define VDMA_C2H_CR_INT_METHOD_MASK 0x00020000 // bit17
#define BIT_C2H_CR_INT_METHOD_MSI 0x00000000
#define BIT_C2H_CR_INT_METHOD_POLL 0x00020000

#define REG_C2H_BUF_LOCK(i) 0x010 + i * 1024
#define REG_C2H_IRQ_COUNTER(i) 0x014 + i * 1024
#define REG_C2H_CHX_FRME_SIZE(i) 0x020 + i * 1024
#define REG_C2H_AVI_INFO(i) 0x024 + i * 1024

#define BIT_MASK_VID_FORMAT (0x0000000F)
#define BIT_MASK_COLOR_DEPTH (0x000000F0)
#define BIT_MASK_AUD_MAX_SMPL_RATE (0x0F000000)

typedef enum class vdma_color_space {
  COLOR_SPACE_RGB_NORMAL = 0,
  COLOR_SPACE_YUV422,
  COLOR_SPACE_YUV444,
  COLOR_SPACE_YUV420,
  COLOR_SPACE_Y_ONLY,
  COLOR_SPACE_RAW,
  COLOR_SPACE_RGB_565,
} vdma_color_space_t;

typedef enum class vdma_aud_samp_rate {
  AUD_SMP_RATE_32K = 0,
  AUD_SMP_RATE_44_1K,
  AUD_SMP_RATE_48K,
  AUD_SMP_RATE_96K,
  AUD_SMP_RATE_192K,
} vdma_aud_samp_rate_t;

class DLLIMPORT LscVdmaController {
  uint32_t ipMaj;
  uint32_t ipMin;
  LSCVDMA_IF *pDrvIf;

  bool poll_mode;
  uint8_t max_buf_num;

  bool buf_lock;
  char ip_ver[MAX_VER_STRING_SIZE];
  char sw_ver[MAX_VER_STRING_SIZE];

  uint8_t bd_num;
  uint8_t ch_id;

  // Init video format information
  uint32_t width_init;
  uint32_t height_init;
  uint8_t color_depth_init;
  vdma_color_space_t color_space_init;
  long frame_size_init;
  uint32_t width;
  uint32_t height;
  uint8_t color_depth;
  vdma_color_space_t color_space;
  long frame_size;

  bool audio_cap;
  vdma_aud_samp_rate_t max_aud_fs_init;
  vdma_aud_samp_rate_t max_aud_fs; // currently  supported max audio fs
  uint32_t audio_buf_size;

  uint8_t max_supp_ch; // max number of support channels
  uint8_t gen_irq_frm_cnt;

  // Added by zhang yue on 2023-11-29 start
  uint8_t pci_cfg[256];
  // Added by zhang yue on 2023-11-29 end

  long GetVdmaInitFrameSize(void);
  void GetVdmaInitAVInfo(void);
  long GetVdmaCurrFrameSize(void);
  void GetVdmaCurrAVInfo(void);
  uint32_t GetVdmaAudBufSize(void);
  uint8_t GetMaxSupportchannels(void);
  // retval true: indicates as Poll mode, if false, indicates MSI INT mode
  bool GetVdmaFrameIdxUpdateMode(void) const;
  void SetGenIrqframeCnt(uint8_t num);
  uint8_t GetGenIrqframeCnt(void) const;
  void PrintInfo(void) const;

public:
  LscVdmaController(const char *pFunctionName, uint8_t bdNum = 1,
                    uint8_t channel_id = 0);
  ~LscVdmaController();

  // video information APIs like width, height, colorsapce, frame size
  uint32_t GetFrameInitWidth(void) const;
  uint32_t GetFrameInitHeight(void) const;
  long GetFrameInitSize(void) const;
  vdma_color_space_t GetFrameInitColorSpace(void) const;
  void UpDateCurrVideoInfo(void);
  uint32_t GetFrameCurrWidth(void) const;
  uint32_t GetFrameCurrHeight(void) const;
  long GetFrameCurrSize(void) const;
  vdma_color_space_t GetFrameCurrColorSpace(void) const;

  // audio related information APIs
  bool IsVdmaSuppAud(void);
  vdma_aud_samp_rate_t GetAudioMaxFs(void) const;
  uint32_t GetAudioBufSize(void) const;

  // Version realated APIs
  void GetIpVerStr(char *ipVer) const;
  void GetDrvVerStr(char *swVer) const;
  void GetVdmaIpVer(uint32_t *maj, uint32_t *min);
  // Added by Zhang Yue on 2023-11-29 start
  void GetVdmaCfgSpace(uint8_t *pPciCfg);
  // Added by Zhang Yue on 2023-11-29 end

  bool BindingVdmaFrameBuffers(void **bufs, size_t bufs_num, size_t buf_size,
                               uint32_t dir);
  bool UnBindingVdmaframBuffers(void);
  int GetVdmaUpdatedIdx(uint32_t *p_idx, uint32_t timeout_ms);

  // Control the behavivour of VDMA IP
  // Enable/Disable transfer video frames from VDMAIP to Host memory
  void RunCtrl(bool on);
  void SetBufLockMode(bool on);
  bool CheckFrameBufLockStatus(uint8_t idx);
  void UnlockFrameBuf(uint8_t idx);

  // read and write registers APIs
  bool read8(uint32_t addr, uint8_t *val, size_t len);
  bool write8(uint32_t addr, uint8_t *val, size_t len);
  bool read16(uint32_t addr, uint16_t *val, size_t len);
  bool write16(uint32_t addr, uint16_t *val, size_t len);
  bool read32(uint32_t addr, uint32_t *val, size_t len);
  bool write32(uint32_t addr, uint32_t *val, size_t len);
};

/**
 * Exception class for objects in the Lattice PCIe driver module.
 * This type of exception is thrown if an occurs deep in the
 * driver code and execution can not continue.  The top-level
 * code can determine the exception cause by displaying the
 * char string returned by the what() method.
 */
// class LscVdmaControllerError : public exception
class LscVdmaControllerError : public exception {
public:
  LscVdmaControllerError(const char *s) { msg = s; }
  virtual const char *what(void) const throw() { return (msg); }

private:
  const char *msg;
};
#endif
