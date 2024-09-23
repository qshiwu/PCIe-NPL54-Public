/*
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#ifndef __LSC_VDMA_IF_H__
#define __LSC_VDMA_IF_H__

#include <iostream>
#include <syslog.h>
#include <unistd.h>

using namespace std;

#ifndef MAX_PCI_BARS
#define MAX_PCI_BARS 1
#endif

#define MAX_DEV_FILENAME_SIZE 1024 // maximum chars in a device driver filename
#define MAX_FUNCTION_NAME_SIZE 64  // maximum chars in a driver function string

#define MAX_BOARD_ID_SIZE 32
#define MAX_DEMO_ID_SIZE 32

// Log an error message and print to stdout
#define ERRORSTR(a)                                                            \
  syslog(LOG_ERR, "%s\n", a);                                                  \
  cout << a

#define MAX_DRIVER_VERSION_LEN 128

#define LSCVDMA_MAGIC 'L'

#define IOCTL_LSCVDMA_START _IOW(LSCVDMA_MAGIC, 0, struct vdma_ubuf_ioctl_arg)
#define IOCTL_LSCVDMA_GET_FRAME _IOWR(LSCVDMA_MAGIC, 1, int)
#define IOCTL_LSCVDMA_STOP _IO(LSCVDMA_MAGIC, 2)
#define IOCTL_LSCVDMA_GET_VERSION_INFO _IOR(LSCVDMA_MAGIC, 3, DriverVerStr_t)
#define IOCTL_LSCVDMA_READ_8BIT _IOR(LSCVDMA_MAGIC, 4, int)
#define IOCTL_LSCVDMA_READ_16BIT _IOR(LSCVDMA_MAGIC, 5, int)
#define IOCTL_LSCVDMA_READ_32BIT _IOR(LSCVDMA_MAGIC, 6, int)
#define IOCTL_LSCVDMA_WRITE_8BIT _IOW(LSCVDMA_MAGIC, 7, int)
#define IOCTL_LSCVDMA_WRITE_16BIT _IOW(LSCVDMA_MAGIC, 8, int)
#define IOCTL_LSCVDMA_WRITE_32BIT _IOW(LSCVDMA_MAGIC, 9, int)
#define IOCTL_LSCVDMA_GET_RESOURCES                                            \
  _IOR(LSCVDMA_MAGIC, 10, struct PCIResourceInfo)
#define IOCTL_LSCVDMA_TX_FRAME _IOWR(LSCVDMA_MAGIC, 11, int)

typedef int HANDLE;

typedef char DriverVerStr_t[MAX_DRIVER_VERSION_LEN];

typedef struct vdma_ubuf_ioctl_arg {
  void **addr;
  size_t len; /* per buf must has the same size*/
  int buf_cnt;
  uint32_t dir;

} vdma_ubuf_ioctl_t;

typedef struct vdma_rw_ioctl_arg {
  unsigned int reg;
  unsigned int value;

} vdma_rw_ioctl_t;

/**
 * Information about a device's BAR.
 */
typedef struct {
  unsigned long nBAR;
  unsigned long physStartAddr;
  unsigned long size;
  bool memMapped;
  unsigned short flags;
  uint8_t type;

} PCI_BAR_t;

typedef struct PCIResourceInfo {
  // Device Memory Access info
  unsigned long numBARs;
  PCI_BAR_t BAR[MAX_PCI_BARS];

  uint8_t PCICfgReg[256];

  // Device Interrupt Info
  bool hasInterrupt;
  unsigned long intrVector;

} PCIResourceInfo_t;

// added by zhang yue on 2023-12-04 start
typedef struct {
  uint16_t vendorID;          // 00
  uint16_t deviceID;          // 02
  uint16_t command;           // 04
  uint16_t status;            // 06
  uint8_t revID;              // 08
  uint8_t classCode[3];       // 09
  uint8_t cacheLineSize;      // 0c
  uint8_t latencyTimer;       // 0d
  uint8_t headerType;         // 0e
  uint8_t BIST;               // 0f
  uint32_t BAR0;              // 10
  uint32_t BAR1;              // 14
  uint32_t BAR2;              // 18
  uint32_t BAR3;              // 1c
  uint32_t BAR4;              // 20
  uint32_t BAR5;              // 24
  uint32_t cardBusPtr;        // 28
  uint16_t subsystemVendorID; // 2c
  uint16_t subsystemID;       // 2e
  uint32_t expROM_BAR;        // 30
  uint8_t capabilitiesPtr;    // 34
  uint8_t reserved[7];
  uint8_t interruptLine; // 3c
  uint8_t interruptPin;  // 3d
  uint8_t minGrant;      // 3e
  uint8_t maxLatency;    // 3f
} PCICfgRegs_t;
// added by zhang yue on 2023-12-04 end

/**
 * The LSCVDMA_IF class provides the actual basic read/write access to hardware
 * registers. This class implements the only the common methods used by all PCIe
 * driver interfaces.
 */
class LSCVDMA_IF {

public:
  static const char *PCIE_SC_BRD;
  static const char *PCIEDMA_DEMO;

  LSCVDMA_IF(const char *pBoardID, const char *pDemoID, uint32_t devNum = 1,
             const char *pFriendlyName = NULL,
             const char *pFunctionName = NULL);

  ~LSCVDMA_IF();

  bool getPCIConfigRegs(uint8_t *pCfg);
  bool getDriverVersionStr(string &outs);
  // Added by Zhang Yue on 2023-11-29 start
  bool getPCIResourcesStr(string &outs);
  bool getPCICapabiltiesStr(string &outs);
  // Added by Zhang Yue on 2023-11-29 end
  bool getIPVersionStr(string &ostr);
  int vdmaStartCtrl(uint8_t on, struct vdma_ubuf_ioctl_arg *arg);
  int vdmaGetVideoFrame(uint32_t *p_idx, uint32_t timeout_ms);
  int vdmaTxVideoFrame(uint32_t *p_idx, uint32_t timeout_ms);

  uint8_t read8(uint32_t addr);
  void write8(uint32_t addr, uint8_t val);
  uint16_t read16(uint32_t addr);
  void write16(uint32_t addr, uint16_t val);
  uint32_t read32(uint32_t addr);
  void write32(uint32_t addr, uint32_t val);

  /* Block Access Methods */
  bool read8(uint32_t addr, uint8_t *val, size_t len, bool incAddr = true);
  bool write8(uint32_t addr, uint8_t *val, size_t len, bool incAddr = true);
  bool read16(uint32_t addr, uint16_t *val, size_t len, bool incAddr = true);
  bool write16(uint32_t addr, uint16_t *val, size_t len, bool incAddr = true);
  bool read32(uint32_t addr, uint32_t *val, size_t len, bool incAddr = true);
  bool write32(uint32_t addr, uint32_t *val, size_t len, bool incAddr = true);

protected:
  HANDLE hDev;
  int OpenDriver(HANDLE &h, const char *pBoardID, const char *pDemoID,
                 uint32_t instance = 1, const char *pFriendlyName = NULL,
                 const char *pFunctionName = NULL);
  PCIResourceInfo_t PCIinfo;

private:
};

/**
 * Exception class for objects in the Lattice PCIe driver module.
 * This type of exception is thrown if an occurs deep in the
 * driver code and execution can not continue.  The top-level
 * code can determine the exception cause by displaying the
 * char string returned by the what() method.
 */
class LSCVDMA_IF_Error : public exception {
public:
  LSCVDMA_IF_Error(const char *s) { msg = s; }
  virtual const char *what(void) const throw() { return (msg); }

private:
  const char *msg;
};

#endif
