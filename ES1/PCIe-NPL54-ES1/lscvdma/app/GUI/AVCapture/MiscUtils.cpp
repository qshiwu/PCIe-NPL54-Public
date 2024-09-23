/*
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

/** @file DebugPrint.cpp */

#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "MiscUtils.h"
#include <stdarg.h>
#include <syslog.h>
#include <time.h>
#include <assert.h>

#define MAX_MSG_SIZE 256

#define DEBUG
using namespace std;

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

/**
 * Format a string, using printf rules, and submit the string
 * to the Windows debug queue.
 * @param fmtStr printf-like format string
 * @param ... variable list of params to place in the formatted string
 */
void DebugPrint(const char *fmtStr, ...) {
#ifndef RELEASE
  va_list args;
  char str[MAX_MSG_SIZE];

  // Format the args into a string
  va_start(args, fmtStr);
  vsnprintf(str, MAX_MSG_SIZE, fmtStr, args);
  va_end(args);

  // Send to Linux system log
  syslog(LOG_INFO, "%s", str);

#ifdef DEBUG
  cout << str << endl; // also print to the screen
#endif

#endif
}

void OutputDebugPrintf(const char *strOutputString, ...) {
  va_list args;
  char str[MAX_MSG_SIZE];

  // Format the args into a string
  va_start(args, strOutputString);
  vsnprintf(str, MAX_MSG_SIZE, strOutputString, args);
  va_end(args);

  // Send to Linux system log
  syslog(LOG_ERR, "%s", str);
  cout << str << endl;
}

uint64_t time_get_ms(void) {
  struct timespec tp;
  uint64_t time_val;
  int ret;

  ret = clock_gettime(CLOCK_MONOTONIC, &tp);
  assert(ret == 0);
  time_val = (uint64_t)tp.tv_sec * 1000 + tp.tv_nsec / 1000000;

  return time_val;
}
