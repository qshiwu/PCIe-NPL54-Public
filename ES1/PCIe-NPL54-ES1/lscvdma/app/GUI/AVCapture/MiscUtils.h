/*
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include "LscVdmaController.h"
#include <stdarg.h>
#include <syslog.h>

#define USE_BUF_LOCK
//#define _LOG_FILE
#define CAPTURE_VFR_NUM 8
#define CAPTURE_CHANNEL 2
#define CAPTURE_SAMPLE_RATE 32000
#define CAPTURE_BIT_PER_SAMPLE 16
#define CAPTURE_SAMPLE_PER_FRAME 267 * 100
//#define AUDIO_PAYLOAD_PER_FRAME 1067             //32kHz 1067, 44.1kHz 1470,
// 48kHz 1600
#define AUDIO_FRAME_NUM 1000

void ShowAppConsole(LscVdmaController *pDrvr, bool *p_open);

void DebugPrint(const char *fmtStr, ...);

#ifdef RELEASE

#define DEBUGPRINT(a)
#define DEBUGSTR(a)
#define DEBUGSTRNL(a)
#define TRACE()
#define ENTER()
#define LEAVE()

#else

// Use this macro like this: DEBUGPRINT(("BAD cnt=%d\n",n));
#define DEBUGPRINT(a) DebugPrint a

#define DEBUGSTR(a) syslog(LOG_INFO, a)   // print a string (new line not added)
#define DEBUGSTRNL(a) syslog(LOG_INFO, a) // print string, (same in linux)
#define TRACE() DebugPrint("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__)
#define ENTER() DebugPrint("ENTER: %s\n", __FUNCTION__)
#define LEAVE() DebugPrint("LEAVE: %s\n", __FUNCTION__)

#endif

// Log an error message and print to stdout
#define AVERRORSTR(a)                                                          \
  syslog(LOG_ERR, "%s\n" a);                                                   \
  cout << a

#define OUTPUT_DEBUG_PRINTF(str) OutputDebugPrintf(str)

void OutputDebugPrintf(const char *strOutputString, ...);

uint64_t time_get_ms(void);
