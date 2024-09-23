/*
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
#ifndef __MEM_BUF_H
#define __MEM_BUF_H
#include <exception>
#include <stdint.h>

using namespace std;

class MemBufErrors : public exception {
public:
  MemBufErrors(const char *s) { msg = s; }
  virtual const char *what(void) const throw() { return (msg); }

private:
  const char *msg;
};

class MemBufs {
public:
  MemBufs(void){};
  virtual ~MemBufs(){};
  virtual const char *GetBuf(uint8_t index) const = 0;
  virtual char **GetBufs() const = 0;
  virtual uint8_t GetBufNum(void) const = 0;
  virtual uint32_t GetBufSize(void) const = 0;
};

class VidBufs : public MemBufs {
public:
  VidBufs(uint8_t bufNum = 16, uint16_t width = 1920, uint16_t height = 1080,
          uint8_t bytesPerPixel = 2);
  VidBufs(const VidBufs &other);
  ~VidBufs();
  virtual const char *GetBuf(uint8_t index) const override;
  virtual char **GetBufs(void) const override;
  virtual uint8_t GetBufNum(void) const override;
  virtual uint32_t GetBufSize(void) const override;

private:
  char **data;
  int width;
  int height;
  int bytesPerPixel;
  uint8_t bufNum;
};

class AudBufs : public MemBufs {
public:
  AudBufs(uint8_t AudFsMax = 48, uint8_t bufNum = 16);
  ~AudBufs();
  virtual const char *GetBuf(uint8_t index) const override;
  virtual char **GetBufs(void) const override;
  virtual uint8_t GetBufNum(void) const override;
  virtual uint32_t GetBufSize(void) const override;

private:
  char **data;
  uint8_t AudFsMax;
  uint8_t bufNum;
  uint16_t bufSize;
};

class AvBufs : public MemBufs {
public:
  AvBufs(uint8_t AudFsMax = 48, uint8_t bufNum = 16, uint16_t width = 1920,
         uint16_t height = 1080, uint8_t bytesPerPixel = 2);
  ~AvBufs();
  virtual const char *GetBuf(uint8_t index) const override;
  virtual char **GetBufs(void) const override;
  virtual uint8_t GetBufNum(void) const override;
  virtual uint32_t GetBufSize(void) const override;

private:
  char **data;
  int width;
  int height;
  int bytesPerPixel;
  uint8_t AudFsMax;
  uint8_t bufNum;
  uint16_t bufSize;
};
#endif
