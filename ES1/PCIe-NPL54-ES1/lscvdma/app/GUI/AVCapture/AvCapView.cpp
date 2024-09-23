/*
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
#include "AvCapView.h"
#include "MiscUtils.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

using namespace std;

#define CAP_PTR_REG 13
#define PCI_CFG_REG_SIZE 256

// static const char* WindowTitle[] = { "LatticeVDMA ColorBars: channel 0 ",
// "LatticeVDMA ColorBars: channel 1", "LatticeVDMA ColorBars:  channel 2",
// "LatticeVDMA ColorBars:  channel 3" };

AvCapView::AvCapView(const string swVer, const string ipVer, int bd_id, int ch,
                     uint8_t buf_num, bool bAud)
    : swVerStr(swVer), ipVerStr(ipVer), bd_select(bd_id), ch_select(ch),
      buf_num(buf_num), b_audio_module(bAud) {

}

AvCapView::~AvCapView() {

}

void AvCapView::IncFrameTotal(uint32_t inc) {
  if (frame_total == 0x7fffffff) {
    frame_total = 0;
    DEBUGSTR("g_frame_total is overflow clear !!\n");
  }
  frame_total += inc;
}

uint32_t AvCapView::GetFrameTotal() { return frame_total; }

void AvCapView::IncFrameLoss(uint32_t inc) { frame_loss_cnt += inc; }

void AvCapView::UpdateFrameStati(uint8_t cpld_idx, uint32_t *next_cpld_idx) {

  if (((buf_num + cpld_idx - *next_cpld_idx) % buf_num) > 13) {
    *next_cpld_idx = (*next_cpld_idx + 13) % buf_num; // Skip 12 frames
    IncFrameTotal(13);
    IncFrameLoss(12);
  } else if (((buf_num + cpld_idx - *next_cpld_idx) % buf_num) > 11) {
    *next_cpld_idx = (*next_cpld_idx + 11) % buf_num; // Skip 8 frames
    IncFrameTotal(11);
    IncFrameLoss(10);

  } else if (((buf_num + cpld_idx - *next_cpld_idx) % buf_num) > 9) {
    *next_cpld_idx = (*next_cpld_idx + 9) % buf_num; // Skip 8 frames
    IncFrameTotal(9);
    IncFrameLoss(8);
  } else if (((buf_num + cpld_idx - *next_cpld_idx) % buf_num) > 7) {
    *next_cpld_idx = (*next_cpld_idx + 7) % buf_num; // Skip 6 frames
    IncFrameTotal(7);
    IncFrameLoss(6);
  } else if (((buf_num + cpld_idx - *next_cpld_idx) % buf_num) > 5) {
    *next_cpld_idx = (*next_cpld_idx + 5) % buf_num; // Skip 4 frames
    IncFrameTotal(5);
    IncFrameLoss(4);
  } else {
    *next_cpld_idx = (*next_cpld_idx + 1) % buf_num;
    IncFrameTotal(1);
  }
}

void AvCapView::UpdateFrameStatiBuflock(uint8_t cpld_idx,
                                        uint32_t *next_cpld_idx) {
  *next_cpld_idx = (*next_cpld_idx + 1) % buf_num;
  IncFrameTotal(1);
}

void AvCapView::SetAppModel(AppVdma *pApp) {
  assert(pApp != NULL);
  pAppVdma = pApp;
}
