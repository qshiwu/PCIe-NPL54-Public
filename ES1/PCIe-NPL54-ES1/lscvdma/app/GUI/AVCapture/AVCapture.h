/*
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
#ifndef __AV_CAPTURE__
#define __AV_CAPTURE__
#include "AvCapView.h"
#include "LSCVDMA_IF.h"
#include "LscVdmaController.h"
#include "MemBuf.h"
#include <fstream>

#define DEBUG_DUMP_FRAMES (1)

class AvCapView;

class YUV422Writer {  
private:  
    std::ofstream outputFile;  
  
public:  
    YUV422Writer(const std::string& filename) : outputFile(filename, std::ios::binary) {  
        if (!outputFile.is_open()) {  
            throw std::runtime_error("Failed to open file for writing.");  
        }  
    }  
  
    ~YUV422Writer() {  
        outputFile.close();  
    }  
  
    void writeFrame(const uint8_t *frameData, int width, int height) {  
        if (!frameData) {  
            throw std::runtime_error("Frame data pointer is null.");  
        }  
  
        size_t frameSize = width * height * 2;  
        outputFile.write(reinterpret_cast<const char*>(frameData), frameSize);  
  
        if (!outputFile.good()) {  
            throw std::runtime_error("Failed to write frame data to file.");  
        }  
    }  
};

class AppVdma {
  static bool running;
  const static uint8_t buf_nums;
  bool dma_stop_flag;
  uint8_t bd_num;
  uint8_t ch_id;
  AvCapView *pAvCapViewInst;
  LscVdmaController *pVdmaCtlInst;
  MemBufs *pMem;
  string _ipVer;
  string _swVer;
  uint64_t _baseTime;
  void GetFuncName(char *pFuncName);
#if (DEBUG_DUMP_FRAMES)
  int DumpFrame(const void *buf, uint8_t cpld_idx, uint64_t size);
#endif
public:
  AppVdma(uint8_t bdNum = 1, uint8_t ch = 0, bool stop_flag = true);
  ~AppVdma();
  bool GetAppRunning(void);
  static void signal_handler(int sig);
  bool GetAppVdmaStopFlag(void);
  int AppMain(void);
};
#endif
