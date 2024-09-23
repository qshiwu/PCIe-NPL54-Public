/*
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
#include "MemBuf.h"
#include <cassert>
#include <cstring>
#include <iostream>

using namespace std;

VidBufs::VidBufs(uint8_t bufNum, uint16_t width, uint16_t height,
                 uint8_t bytesPerPixel)
    : width(width), height(height), bytesPerPixel(bytesPerPixel),
      bufNum(bufNum) {
  uint32_t size = 0;

  assert(bufNum);
  assert(width);
  assert(height);
  assert(bytesPerPixel);

  data = new char *[bufNum];
  size = width * height * bytesPerPixel;

  for (int i = 0; i < bufNum; i++) {
    try {
      data[i] = new char[size];
    } catch (const std::bad_alloc &e) {
      std::cerr << "Memory allocation failed: for VidBufs" << e.what() << endl;
      throw MemBufErrors("Vid Buffers allocate failed");
    }
  }
}

VidBufs::VidBufs(const VidBufs &other) {
  uint32_t size = 0;
  bufNum = other.bufNum;
  width = other.width;
  height = other.height;
  bytesPerPixel = other.bytesPerPixel;

  assert(bufNum);
  assert(width);
  assert(height);
  assert(bytesPerPixel);

  size = width * height * bytesPerPixel;
  data = new char *[bufNum];
  for (int i = 0; i < bufNum; i++) {
    try {
      data[i] = new char[size];
    } catch (const std::bad_alloc &e) {
      std::cerr << "Memory allocation failed for VidBufs " << e.what() << endl;
      throw MemBufErrors("Vid Buffers(copy) allocate failed");
    }
    memcpy(data[i], other.data[i], size);
  }
}

VidBufs::~VidBufs() {
  for (int i = 0; i < bufNum; i++) {
    delete[] data[i];
  }
  delete[] data;
}

const char *VidBufs::GetBuf(uint8_t idx) const {
  assert(data);
  assert(data[idx]);

  return data[idx];
}

char **VidBufs::GetBufs(void) const {
  assert(data);
  return data;
}

uint8_t VidBufs::GetBufNum(void) const { return bufNum; }

uint32_t VidBufs::GetBufSize(void) const {
  return width * height * bytesPerPixel;
}

AudBufs::AudBufs(uint8_t AudFsMax, uint8_t bufNum)
    : AudFsMax(AudFsMax), bufNum(bufNum) {
  assert(bufNum);
  switch (AudFsMax) {
  case 48:
    bufSize = 6400;
    break;
  case 96:
    bufSize = 3280 * 4;
    break;
  case 192:
    bufSize = 6560 * 4;
    break;
  default:
    bufSize = 6400;
  }

  data = new char *[bufNum];
  for (int i = 0; i < bufNum; i++) {
    try {
      data[i] = new char[bufSize];
    } catch (const std::bad_alloc &e) {
      std::cerr << "Memory allocation failed: for AudBufs" << e.what() << endl;
      throw MemBufErrors("Aud Buffers(copy) allocate failed");
    }
  }
}

AudBufs::~AudBufs() {
  for (int i = 0; i < bufNum; i++) {
    delete[] data[i];
  }
  delete[] data;
}

const char *AudBufs::GetBuf(uint8_t index) const {
  assert(data);
  assert(data[index]);

  return data[index];
}
char **AudBufs::GetBufs(void) const {
  assert(data);
  return data;
}

uint8_t AudBufs::GetBufNum(void) const { return bufNum; }

uint32_t AudBufs::GetBufSize(void) const { return bufSize; }
AvBufs::AvBufs(uint8_t AudFsMax, uint8_t bufNum, uint16_t width,
               uint16_t height, uint8_t bytesPerPixel)
    : width(width), height(height), bytesPerPixel(bytesPerPixel),
      AudFsMax(AudFsMax), bufNum(bufNum) {
  uint32_t size = 0;
  assert(bufNum);
  assert(width);
  assert(height);
  assert(bytesPerPixel);
  switch (AudFsMax) {
  case 48:
    bufSize = 6400;
    break;
  case 96:
    bufSize = 3280 * 4;
    break;
  case 192:
    bufSize = 6560 * 4;
    break;
  default:
    bufSize = 6400;
  }

  data = new char *[bufNum];

  size = width * height * bytesPerPixel + bufSize;

  for (int i = 0; i < bufNum; i++) {
    try {
      data[i] = new char[size];
    } catch (const std::bad_alloc &e) {
      std::cerr << "Memory allocation failed: for AvBufs" << e.what() << endl;
      throw MemBufErrors("Aud Buffers(copy) allocate failed");
    }
  }
}

AvBufs::~AvBufs() {
  for (int i = 0; i < bufNum; i++) {
    delete[] data[i];
  }
  delete[] data;
}

const char *AvBufs::GetBuf(uint8_t index) const {
  assert(data);
  assert(index < bufNum);
  assert(data[index]);

  return data[index];
}
char **AvBufs::GetBufs(void) const {
  assert(data);
  return data;
}
uint8_t AvBufs::GetBufNum(void) const { return bufNum; }

uint32_t AvBufs::GetBufSize(void) const {
  return bufSize + width * height * bytesPerPixel;
}
