/*
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#ifndef __AVCAP_VIEW_H
#define __AVCAP_VIEW_H

#include "AVCapture.h"
#include <string>

using namespace std;

class AppVdma;

class AvCapView {
  bool b_show_app_about;
  bool b_show_app_console;
  bool b_show_metrics;
  // Added by Zhang Yue on 2023-11-29 start
  bool b_show_dev_info;
  // Added by Zhang Yue on 2023-11-29 end
  bool b_buflock_switch;
  bool b_audio_switch;
  bool b_vsync;

  string swVerStr;
  string ipVerStr;

  uint8_t bd_select;
  uint8_t ch_select;
  uint8_t buf_num;
  bool b_audio_module;

  uint32_t frame_loss_cnt;
  uint32_t frame_total;
  float metric_factor;
  uint32_t width;
  uint32_t height;
  uint32_t view_width;
  uint32_t view_height;

  AppVdma *pAppVdma;

  void IncFrameTotal(uint32_t inc);
  void IncFrameLoss(uint32_t inc);

public:
  uint32_t GetFrameTotal(void);
  AvCapView(const string swVer, const string ipVer, int bd_id, int ch = 0,
            uint8_t buf_num = 16, bool bAud = false);
  ~AvCapView();
  void UpdateFrameStati(uint8_t cpld_idx, uint32_t *next_cpld_idx);
  void UpdateFrameStatiBuflock(uint8_t cpld_idx, uint32_t *next_cpld_idx);
  void SetAppModel(AppVdma *pApp);
};

/**
 * Exception class for objects in the Lattice PCIe driver module.
 * This type of exception is thrown if an occurs deep in the
 * driver code and execution can not continue.  The top-level
 * code can determine the exception cause by displaying the
 * char string returned by the what() method.
 */
// class DLLIMPORT LSCVDMA_IF_Error : public exception
class AvCapViewError : public exception {
public:
  AvCapViewError(const char *s) { msg = s; }
  virtual const char *what(void) const throw() { return (msg); }

private:
  const char *msg;
};
#endif
