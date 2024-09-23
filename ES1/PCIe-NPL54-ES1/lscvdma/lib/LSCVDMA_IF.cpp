/*
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <exception>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>

#include <errno.h>
#include <sys/mman.h>

using namespace std;

#include "LSCVDMA_IF.h"

const char *LSCVDMA_IF::PCIE_SC_BRD = "LSC";
const char *LSCVDMA_IF::PCIEDMA_DEMO = "VDMA";

#define NO_SUCH_GUID -1
#define NO_SUCH_BOARD_ID -2
#define NO_SUCH_DEMO_ID -3
#define NO_MORE_BOARDS -4
#define PARAM_ERROR -5
#define OPEN_FILE_FAILED -6

#define MaxBlockSize 256

/**
 * Create an interface to a Lattice PCIe Driver and eval board.
 * @param pBoardID the PCI Vendor and Device register pair indicating the board
 * to open
 * @param pDemoID the PCI SubSystem regsiter value indicating the Demo IP to
 * open
 * @param devNum the instance of the board (i.e. locate and open the 2nd SC80 x4
 * board)
 * @param pFriendlyName (NOT USED) text string naming the board, set in
 * registry,
 * @param pFunctionName text string naming a specific driver function to open
 * (i.e. use DMA chan #1)
 */
LSCVDMA_IF::LSCVDMA_IF(const char *pBoardID, const char *pDemoID,
                       uint32_t devNum, const char *pFriendlyName,
                       const char *pFunctionName) {
  int ret = 0;
  hDev = -1;

  if ((pBoardID == NULL) || (pDemoID == NULL))
    return;

  cout << "BoardID: " << pBoardID << " DemoID: " << pDemoID
       << " devNum: " << devNum << endl;

  /* Open the kernel driver using the driver ID, Board, Demo and instance number
   * passed in */
  ret =
      OpenDriver(hDev, pBoardID, pDemoID, devNum, pFriendlyName, pFunctionName);
  if (ret == NO_SUCH_GUID)
    throw(LSCVDMA_IF_Error("NO_SUCH_GUID \n"));
  else if (ret == NO_SUCH_BOARD_ID)
    throw(LSCVDMA_IF_Error("NO_SUCH_BOARD_ID \n"));
  else if (ret == NO_MORE_BOARDS)
    throw(LSCVDMA_IF_Error("NO_MORE_BOARDS \n"));
  else if (ret == PARAM_ERROR)
    throw(LSCVDMA_IF_Error("PARAM_ERROR \n"));
  else if (ret == OPEN_FILE_FAILED)
    throw(LSCVDMA_IF_Error("OPEN_FILE_FAILED \n"));

  cout << "LSCVDMA_IF created." << endl;

  // Use driver specific IOCTL call to get PCI info from the driver
  if (ioctl(hDev, IOCTL_LSCVDMA_GET_RESOURCES, &PCIinfo) != 0) {
    close(hDev);
    hDev = -1;
    throw(LSCVDMA_IF_Error("LSCVDMA_IF: ioctl GET PCI RESOURCES Failed! \n"));
  }

  // cout<<"LSCVDMA_IF Get_RESOURCES: "<<endl;

  cout << "hasInterrupt: " << PCIinfo.hasInterrupt;
  cout << "  Vect: " << PCIinfo.intrVector;
  cout << "  Num BARs: " << PCIinfo.numBARs << endl;
  for (uint32_t i = 0; i < PCIinfo.numBARs; i++) {
    cout << "BAR" << PCIinfo.BAR[i].nBAR;
    cout << "  Addr: " << hex << PCIinfo.BAR[i].physStartAddr;
    cout << "  Size: " << dec << PCIinfo.BAR[i].size;
    cout << "  Mapped: " << PCIinfo.BAR[i].memMapped << endl;
  }
}

/**
 * Delete an instance of a register access object.
 */
LSCVDMA_IF::~LSCVDMA_IF() {
  if (hDev != -1) {
    close(hDev);
    hDev = -1; // don't use anymore
  }

  // cout<<"LSCVDMA_IF: driver closed. Bye."<<endl;
}

/**
 * Return the 256 bytes of the device's PCI configuration space registers.
 * These registers must be present on any PCI/PCIExpress device.
 * They have a standard format.
 * @param pCfg user's location to store 256 bytes
 * @return true if read byte OK, false if driver reports error
 */
bool LSCVDMA_IF::getPCIConfigRegs(uint8_t *pCfg) {
  PCIResourceInfo_t PCIinfo;

  if (ioctl(hDev, IOCTL_LSCVDMA_GET_RESOURCES, &PCIinfo) != 0)
    return (false);

  memcpy(pCfg, PCIinfo.PCICfgReg, sizeof(PCIinfo.PCICfgReg));

  return (true);
}

/**
 * Return the string containing the driver version and build date.
 * The drivers implement an IOCTL that returns the build number, date
 * and description of the driver.  This method gets that info from
 * the kernel driver and returns it to the caller.
 * @return true if info returned from driver or false if the driver
 * doesn't implement the IOCTL code or something else failed.
 */
bool LSCVDMA_IF::getDriverVersionStr(string &outs) {
  DriverVerStr_t str;

  if (ioctl(hDev, IOCTL_LSCVDMA_GET_VERSION_INFO, str) != 0) {
    outs.append("ERROR");
    return (false);
  }

  outs.append(str);

  return (true);
}

bool LSCVDMA_IF::getIPVersionStr(string &ostr) {
  uint8_t major, minor, minor_update;
  uint16_t lsc_internal;
  char buf[64];
  vdma_rw_ioctl_t rw;

  rw.reg = 0x000;
  rw.value = 0;

  if (ioctl(hDev, IOCTL_LSCVDMA_READ_32BIT, &rw) != 0) {
    throw(LSCVDMA_IF_Error("LSCVDMA_IF: read32 version failed! \n"));
  }

  major = (rw.value & 0xF0000000) >> 28;
  minor = (rw.value & 0x0FF00000) >> 20;
  minor_update = (rw.value & 0x000F0000) >> 16;
  lsc_internal = rw.value & 0x0000FFFF;

  sprintf(buf, "%01x.%02x.%01x.%04x\n", major, minor, minor_update,
          lsc_internal);
  ostr.append("Lattice PCIe VDMA IP v");
  ostr.append(buf);

  return (true);
}

// Added by Zhang Yue on 2023-11-29 start
bool LSCVDMA_IF::getPCIResourcesStr(string &outs) {
  std::ostringstream oss;
  oss << "Number of BARs: " << PCIinfo.numBARs << "\n";
  for (uint32_t i = 0; i < PCIinfo.numBARs; i++) {
    oss << "BAR" << i << ":";
    oss << "  Addr: " << hex << PCIinfo.BAR[i].physStartAddr;
    oss << "  Size: " << dec << PCIinfo.BAR[i].size;
    oss << "  Mapped:" << PCIinfo.BAR[i].memMapped << "\n";
  }
  if (PCIinfo.hasInterrupt)
    oss << "Interrupt Vector: " << PCIinfo.intrVector << "\n";
  else
    oss << "NO INTERRUPTS\n";

  outs = oss.str();

  return (true);
}
bool LSCVDMA_IF::getPCICapabiltiesStr(string &outs) {
  int i, id, next, index;
  // uint8_t buf[256];
  uint8_t *buf;
  PCICfgRegs_t *pPCI;
  uint8_t *p8;
  uint16_t *p16;
  uint32_t *p32;
  std::ostringstream oss;

  // Element "No limit" added by zhang yue on 2023-11-01 for memory cross the
  // board issue
  static const char *L0sLat[8] = {"64ns", "128ns", "256ns", "1us",
                                  "2us",  "4us",   ">4us",  "\"No limit\""};
  static const char *L1Lat[8] = {"1us",  "2us",  "4us",  "8us",
                                 "16us", "32us", "64us", ">64us"};

  pPCI = (PCICfgRegs_t *)PCIinfo.PCICfgReg;
  buf = PCIinfo.PCICfgReg;
  oss << std::setbase(16);

  // Now parse the capabilities structures.  The first structure in the list
  // is pointed to by the Capabilities Ptr at location 0x34.  If this is 0
  // or the capabilities bit in the status is 0 then there are none.
  if (((pPCI->status & 0x10) == 0) || (pPCI->capabilitiesPtr == 0)) {
    oss << "No Capabilities Structures.";
    outs = oss.str();
    return (true);
  }

  i = 0;
  next = (int)pPCI->capabilitiesPtr;
  while ((next >= 0x40) && (i < 16)) {
    ++i; // loop counter to prevent circular loop
    index = next;
    id = buf[next];
    p8 = (uint8_t *)&buf[next];
    p16 = (uint16_t *)&buf[next];
    p32 = (uint32_t *)&buf[next];
    next = (int)buf[next + 1];
    switch (id) {
    case 1: // Power Management
      oss << "\nPower Management Capability Structure @ 0x" << hex << index
          << "\n";

      oss << "\tPwrCap: 0x" << hex << p16[1] << dec << " = [";
      oss << "PME:";
      if (p16[1] & 0x8000)
        oss << "D3c-";
      if (p16[1] & 0x4000)
        oss << "D3h-";
      if (p16[1] & 0x2000)
        oss << "D2-";
      if (p16[1] & 0x1000)
        oss << "D1-";
      if (p16[1] & 0x0800)
        oss << "D0";
      if (p16[1] & 0x0400)
        oss << " D2_support";
      if (p16[1] & 0x0200)
        oss << " D1_support";
      // bunch of other bits that aren't really applicable
      oss << " ver=" << (p16[1] & 0x007);
      oss << "]\n";

      oss << "\tPMCSR: 0x" << hex << p16[2] << dec << " = [";
      if (p16[2] & 0x8000)
        oss << "PME_Event";
      // Data_scale is bits 14:13
      // Data_select is bits 12:9
      if (p16[2] & 0x0100)
        oss << " PME_en";
      oss << " state=D" << (p16[2] & 0x003);
      oss << "]\n";

      oss << "\tData: 0x" << hex << (int)p8[7] << "\n";
      break;

    case 2: // AGP Capability
      oss << "\nAGP Capability Structure @ 0x" << hex << index << "\n";
      break;

    case 3: // VPD (Vital Product Data) Capability
      oss << "\nVPD (Vital Product Data) Capability Structure @ 0x" << hex
          << index << "\n";
      break;

    case 4: // Slot ID Capability
      oss << "\nSlot ID Capability Structure @ 0x" << hex << index << "\n";
      break;

    case 5: // MSI
      oss << "\nMSI Capability Structure @ 0x" << hex << index << "\n";

      oss << "\tMsgCtrlReg: 0x" << hex << p16[1] << dec << " = [";
      if (p16[1] & 0x0080)
        oss << "64bitAddr ";
      oss << "numMsgs=" << (1 << ((p16[1] & 0x0070) >> 4)) << " ";
      oss << "reqMsgs=" << (1 << ((p16[1] & 0x000e) >> 1)) << " ";
      if (p16[1] & 0x0001)
        oss << "MSI_Enable ";
      oss << "]\n";

      oss << "\tMsgAddr: 0x" << hex << p32[2] << p32[1]
          << "\n"; // display the 64 bit address
      oss << "\tMsgData: 0x" << hex << p16[6] << "\n";
      break;

    case 6: // CompactPCI Hot Swap
      oss << "\nCompactPCI Hot Swap Capability Structure @ 0x" << hex << index
          << "\n";
      break;

    case 7: // PCI-X
      oss << "\nPCI-X Capability Structure @ 0x" << hex << index << "\n";
      break;

    case 8: // AMD
      oss << "\nAMD Capability Structure @ 0x" << hex << index << "\n";
      break;

    case 9: // Vendor Specific
      oss << "\nVendor Specific Capability Structure @ 0x" << hex << index
          << "\n";
      break;

    case 0x0a: // Debug Port
      oss << "\nDebug Port @ 0x" << hex << index << "\n";
      break;

    case 0x0b: // CompactPCI central resource control
      oss << "\nCompactPCI central resource control @ 0x" << hex << index
          << "\n";
      break;

    case 0x0c: // PCI Hot Plug
      oss << "\nPCI Hot-Plug Compatable @ 0x" << hex << index << "\n";
      break;

    case 0x10: // PCI Express
      oss << "\nPCI Express Capability Structure @ 0x" << hex << index << "\n";
      oss << "\tPCIe_Cap: 0x" << hex << p16[1] << dec << " = [";
      oss << " IntMsg#=" << ((p16[1] & 0x3e00) >> 9);
      if (p16[1] & 0x0100)
        oss << " SlotImp";
      oss << " type=" << ((p16[1] & 0x0f0) >> 4); // 0=PCIeEndPt, 1= Legacy
      oss << " ver=" << (p16[1] & 0x00f);
      oss << "]\n";

      oss << "\tDev_Cap: 0x" << hex << p32[1] << dec << " = [";
      if (p32[1] & 0x4000)
        oss << " PwrInd";
      if (p32[1] & 0x2000)
        oss << " AttInd";
      if (p32[1] & 0x1000)
        oss << " AttBtn";
      oss << " L1Lat=" << L1Lat[((p32[1] & 0x0e00) >> 9)];
      oss << " L0sLat=" << L0sLat[((p32[1] & 0x01c0) >> 6)];
      if (p32[1] & 0x20)
        oss << " 8bit_tag";
      if (p32[1] & 0x18)
        oss << " Phantom";
      oss << " MaxTLPSize=" << (128 << (p32[1] & 0x07));
      oss << "]\n";

      oss << "\tDev_Ctrl: 0x" << hex << p16[4] << dec << " = [";
      oss << " MaxRdSize=" << (128 << ((p16[4] & 0x7000) >> 12));
      if (p16[4] & 0x0800)
        oss << " NoSnoop";
      if (p16[4] & 0x0400)
        oss << " AuxPwr";
      if (p16[4] & 0x0200)
        oss << " Phantom";
      if (p16[4] & 0x0100)
        oss << " ExtTag";
      // oss<<" MaxTLPSize="<<(128<<((p16[4] & 0x0e)>>5));
      oss << " MaxTLPSize=" << (128 << ((p16[4] & 0xe0) >> 5));
      if (p16[4] & 0x0010)
        oss << " RlxOrd";
      if (p16[4] & 0x0008)
        oss << " UR";
      if (p16[4] & 0x0004)
        oss << " ERR_FATAL";
      if (p16[4] & 0x0002)
        oss << " ERR_NONFATAL";
      if (p16[4] & 0x0001)
        oss << " ERR_COR";
      oss << "]\n";

      oss << "\tDev_Stat: 0x" << hex << p16[5] << dec << " = [";
      if (p16[5] & 0x0020)
        oss << " ReqPend";
      if (p16[5] & 0x0010)
        oss << " AuxPwr";
      if (p16[5] & 0x0008)
        oss << " UR";
      if (p16[5] & 0x0004)
        oss << " ERR_FATAL";
      if (p16[5] & 0x0002)
        oss << " ERR_NONFATAL";
      if (p16[5] & 0x0001)
        oss << " ERR_COR";
      oss << "]\n";

      oss << "\tLink_Cap: 0x" << hex << p32[3] << dec << " = [";
      oss << " Port#=" << ((p32[3] & 0xff000000) >> 24);
      oss << " L1Lat=" << L1Lat[((p32[3] & 0x38000) >> 15)];
      oss << " L0sLat=" << L0sLat[((p32[3] & 0x7000) >> 12)];
      if ((p32[3] & 0x0c00) == 0x0c00)
        oss << " ASPM:L1+L0s";
      if ((p32[3] & 0x0c00) == 0x0400)
        oss << " ASPM:L0s";
      oss << " Width=x" << ((p32[3] & 0x03f0) >> 4);
      switch (p32[3] & 0x000F) {
      case 1:
        oss << " 2.5 G";
        break;
      case 2:
        oss << " 5.0 G";
        break;
      case 3:
        oss << " 8.0 G";
        break;
      default:
        oss << " 2.5 G";
        break;
      }

      oss << "]\n";

      oss << "\tLink_Ctrl: 0x" << hex << p16[6] << dec << " = [";
      if (p16[8] & 0x0080)
        oss << " ExtSync";
      if (p16[8] & 0x0040)
        oss << " CommonClk";
      if (p16[8] & 0x0008)
        oss << " RCB=128";
      else
        oss << " RCB=64";
      oss << " ASPM:";
      switch (p16[8] & 0x0003) {
      case 0:
        oss << "Disabled";
        break;
      case 1:
        oss << "L0s";
        break;
      case 2:
        oss << "L1";
        break;
      case 3:
        oss << "L0s+L1";
        break;
      }
      oss << "]\n";

      oss << "\tLink_Stat: 0x" << hex << p16[7] << dec << " = [";
      if (p16[9] & 0x1000)
        oss << " SlotRefClk";
      if (p16[9] & 0x0400)
        oss << " TRAIN_ERR";
      oss << " Width=x" << ((p16[9] & 0x03f0) >> 4);
      switch (p16[9] & 0x000F) {
      case 1:
        oss << " 2.5 G";
        break;
      case 2:
        oss << " 5.0 G";
        break;
      case 3:
        oss << " 8.0 G";
        break;
      default:
        oss << " 2.5 G";
        break;
      }
      oss << "]\n";
      // Slot Registers and Root Registers not implemented by our EndPoint core

      break;

    default:
      oss << "Unknown Capability: " << dec << id << hex << "\n";
      next = 0; // abort
      break;
    }
  }
  outs = oss.str();

  return true;
}
// Added by Zhang Yue on 2023-11-29 end

int LSCVDMA_IF::vdmaStartCtrl(uint8_t on, struct vdma_ubuf_ioctl_arg *arg) {

  if (on == 1) // start
  {
    if (ioctl(hDev, IOCTL_LSCVDMA_START, arg) < 0)
      throw(LSCVDMA_IF_Error("LSCVDMA_IF: vdma start failed! \n"));
  } else // stop
  {
    if (ioctl(hDev, IOCTL_LSCVDMA_STOP, 0) < 0)
      throw(LSCVDMA_IF_Error("LSCVDMA_IF: vdma stop failed! \n"));
  }

  return 0;
}

int LSCVDMA_IF::vdmaGetVideoFrame(uint32_t *p_idx, uint32_t timeout_ms) {
  int status = 0;

  if (ioctl(hDev, IOCTL_LSCVDMA_GET_FRAME, &timeout_ms) < 0)
    status = (errno);
  else
    *p_idx = timeout_ms;

  return status;
}

int LSCVDMA_IF::vdmaTxVideoFrame(uint32_t *p_idx, uint32_t timeout_ms) {
  int status = 0;

  if (ioctl(hDev, IOCTL_LSCVDMA_TX_FRAME, &timeout_ms) < 0)
    status = (errno);
  else
    *p_idx = timeout_ms;

  return status;
}

/**
 * Read 8 bits from an LSC FPGA register via PCIe bus.
 * @param offset BAR + address of device register to read from
 * @return the 8bit value read
 */
uint8_t LSCVDMA_IF::read8(uint32_t offset) {
  vdma_rw_ioctl_t rw;
  uint8_t val;

  rw.reg = offset;
  rw.value = 0;

  if (ioctl(hDev, IOCTL_LSCVDMA_READ_8BIT, &rw) != 0) {
    throw(LSCVDMA_IF_Error("LSCVDMA_IF: read8 failed! \n"));
  }

  val = (uint8_t)rw.value;

  return (val);
}

/**
 * Write 8 bits to an LSC hardware register via PCIe bus.
 * @param offset BAR + address of device register to write to
 * @param val value to write into the register
 */
void LSCVDMA_IF::write8(uint32_t offset, uint8_t val) {
  vdma_rw_ioctl_t rw;

  rw.reg = offset;
  rw.value = val;

  if (ioctl(hDev, IOCTL_LSCVDMA_WRITE_8BIT, &rw) != 0) {
    throw(LSCVDMA_IF_Error("LSCVDMA_IF: write8 failed! \n"));
  }
}

/**
 * Read 16 bits from an FPGA register via PCIe bus.
 * @param offset BAR + address of device register to read from
 * @param val location of storage for data read
 * @return uint16 value read
 */
uint16_t LSCVDMA_IF::read16(uint32_t offset) {
  vdma_rw_ioctl_t rw;
  uint16_t val;

  if (offset & 0x01) {
    throw(
        LSCVDMA_IF_Error("LSCVDMA_IF: read16 Addr must be multiple of 2. \n"));
  }

  rw.reg = offset;
  rw.value = 0;

  if (ioctl(hDev, IOCTL_LSCVDMA_READ_16BIT, &rw) != 0) {
    throw(LSCVDMA_IF_Error("LSCVDMA_IF: read16 failed! \n"));
  }

  val = (uint16_t)rw.value;

  return (val);
}

/**
 * Write 16 bits to an SC hardware register via PCIe bus.
 * @param offset address of device register to write to
 * @param val value to write into the register
 * @return true; error in writing will cause hardware exception
 */
void LSCVDMA_IF::write16(uint32_t offset, uint16_t val) {
  vdma_rw_ioctl_t rw;

  if (offset & 0x01) {
    throw(
        LSCVDMA_IF_Error("LSCVDMA_IF: write16 Addr must be multiple of 2. \n"));
  }

  rw.reg = offset;
  rw.value = val;

  if (ioctl(hDev, IOCTL_LSCVDMA_WRITE_16BIT, &rw) != 0) {
    throw(LSCVDMA_IF_Error("LSCVDMA_IF: write16 failed! \n"));
  }
}

/**
 * Read 32 bits from an SC hardware register via PCIe bus.
 * This is done with 2 16 bit reads because the SC900 only has a 16 bit wide
 * data bus and will not allow accesses larger than 16 bits.
 * @param offset address of device register to read from
 * @param val location of storage for data read
 * @return true; false if address not multiple of 4
 * @note error in reading will cause hardware exception
 */
uint32_t LSCVDMA_IF::read32(uint32_t offset) {
  vdma_rw_ioctl_t rw;
  uint32_t val;

  if (offset & 0x03) {
    throw(
        LSCVDMA_IF_Error("LSCVDMA_IF: read32 Addr must be multiple of 4. \n"));
  }

  rw.reg = offset;
  rw.value = 0;

  if (ioctl(hDev, IOCTL_LSCVDMA_READ_32BIT, &rw) != 0) {
    throw(LSCVDMA_IF_Error("LSCVDMA_IF: read32 failed! \n"));
  }

  val = (uint32_t)rw.value;

  return (val);
}

/**
 * Write 32 bits to an SC hardware register via PCIe bus.
 * @param offset address of device register to write to
 * @param val value to write into the register
 * @note error in reading will cause hardware exception
 */
void LSCVDMA_IF::write32(uint32_t offset, uint32_t val) {
  vdma_rw_ioctl_t rw;

  if (offset & 0x03) {
    throw(
        LSCVDMA_IF_Error("LSCVDMA_IF: write32 Addr must be multiple of 4. \n"));
  }

  rw.reg = offset;
  rw.value = val;

  if (ioctl(hDev, IOCTL_LSCVDMA_WRITE_32BIT, &rw) != 0) {
    throw(LSCVDMA_IF_Error("LSCVDMA_IF: write32 failed! \n"));
  }
}

/**
 * Read a block of 8 bit registers from FPGA hardware via PCIe bus.
 * @param offset BAR + address of device register to read from
 * @param val location of storage for data read
 * @param len number of bytes to read
 * @param incAddr true if address increments for each operation, false to
 *        access the same address each time (FIFO access)
 * @return true; error if driver fails
 */
bool LSCVDMA_IF::read8(uint32_t offset, uint8_t *val, size_t len,
                       bool incAddr) {
  vdma_rw_ioctl_t rw;
  uint8_t *buf8;

  size_t i;

  buf8 = new uint8_t[MaxBlockSize];
  if (buf8 == NULL)
    return (false);

  if (incAddr) {
    // loop to copy len bytes from BAR addr to user's array
    for (i = 0; i < len; i++) {
      rw.reg = offset;
      rw.value = 0;

      if (ioctl(hDev, IOCTL_LSCVDMA_READ_8BIT, &rw) != 0) {
        throw(LSCVDMA_IF_Error("LSCVDMA_IF: block read8 failed! \n"));
      }

      *val = (uint8_t)rw.value;
      // printf("multi read addr:0x%x, value: 0x%x\n", temp_addr,buf8[i]);
      offset += 1;
      ++val;
    }
  }

  return (true);
}

/**
 * Write a block of 8 bit registers into FPGA hardware via PCIe bus.
 * @param offset address of device register to write to
 * @param val location of bytes to write
 * @param len number of bytes to write
 * @param incAddr true if address increments for each operation, false to
 *        access the same address each time (FIFO access)
 * @return true; error in writing will cause hardware exception
 */
bool LSCVDMA_IF::write8(uint32_t offset, uint8_t *val, size_t len,
                        bool incAddr) {
  vdma_rw_ioctl_t rw;
  size_t i;

  if (incAddr) {
    // loop to copy len bytes from  user's array to BAR addr
    for (i = 0; i < len; i++) {
      rw.reg = offset;
      rw.value = *val;

      if (ioctl(hDev, IOCTL_LSCVDMA_WRITE_8BIT, &rw) != 0) {
        throw(LSCVDMA_IF_Error("LSCVDMA_IF: block write8 failed! \n"));
      }
      // printf("multi write addr:0x%x, value: 0x%x\n", wr_addr,buf8[i]);
      offset += 1;
      ++val;
    }
  }

  return (true);
}

/**
 * Read a block of 16 bit registers from SC hardware via PCIe bus.
 * @param offset address of device registers to read from
 * @param val location of storage for data read
 * @param len number of 16 bit words to read (not byte count)
 * @param incAddr true if address increments for each operation, false to
 *        access the same address each time (FIFO access)
 * @return true;  false if address not multiple of 2
 * @note error in reading will cause hardware exception
 */
bool LSCVDMA_IF::read16(uint32_t offset, uint16_t *val, size_t len,
                        bool incAddr) {
  vdma_rw_ioctl_t rw;
  size_t i;

  if (offset & 0x01) {
    return false; // error! must be multiple of 2
  }

  if (incAddr) {
    // loop to copy len bytes from BAR addr to user's array
    for (i = 0; i < len; i++) {
      rw.reg = offset;
      rw.value = 0;

      if (ioctl(hDev, IOCTL_LSCVDMA_READ_16BIT, &rw) != 0) {
        throw(LSCVDMA_IF_Error("LSCVDMA_IF: block read16 failed! \n"));
      }

      *val = (uint16_t)rw.value;
      // printf("multi read addr:0x%x, value: 0x%x\n", temp_addr,buf8[i]);
      offset += 2;
      ++val;
    }
  }

  return (true);
}

/**
 * Write a block of 16 bit registers into FPGA hardware via PCIe bus.
 * @param offset BAR + address of device registers to write to
 * @param val location of 16 bit words to write
 * @param len number of 16 bit words to write
 * @param incAddr true if address increments for each operation, false to
 *        access the same address each time (FIFO access)
 * @return true;  false if address not multiple of 2
 */
bool LSCVDMA_IF::write16(uint32_t offset, uint16_t *val, size_t len,
                         bool incAddr) {
  vdma_rw_ioctl_t rw;
  size_t i;

  if (offset & 0x01) {
    return false; // error! must be multiple of 2
  }

  if (incAddr) {
    // loop to copy len bytes from  user's array to BAR addr
    for (i = 0; i < len; i++) {
      rw.reg = offset;
      rw.value = *val;

      if (ioctl(hDev, IOCTL_LSCVDMA_WRITE_16BIT, &rw) != 0) {
        throw(LSCVDMA_IF_Error("LSCVDMA_IF: block write16 failed! \n"));
      }
      offset += 2;
      ++val;
    }
  }

  return (true);
}

/**
 * Read a block of 32 bit registers from FPGA hardware via PCIe bus.
 * @param offset BAR + address of device registers to read from
 * @param val location of storage for data read
 * @param len number of 32 bit words to read (not byte count)
 * @param incAddr true if address increments for each operation, false to
 *        access the same address each time (FIFO access)
 * @return true;  false if address not multiple of 4 or read failed
 */
bool LSCVDMA_IF::read32(uint32_t offset, uint32_t *val, size_t len,
                        bool incAddr) {
  vdma_rw_ioctl_t rw;
  size_t i;

  if (offset & 0x03) {
    return false; // error! must be multiple of 4
  }

  if (incAddr) {
    // loop to copy len bytes from BAR addr to user's array
    for (i = 0; i < len; i++) {
      rw.reg = offset;
      rw.value = 0;

      if (ioctl(hDev, IOCTL_LSCVDMA_READ_32BIT, &rw) != 0) {
        throw(LSCVDMA_IF_Error("LSCVDMA_IF: block read32 failed! \n"));
      }

      *val = (uint32_t)rw.value;
      // printf("multi read addr:0x%x, value: 0x%x\n", temp_addr,buf8[i]);
      offset += 4;
      ++val;
    }
  }

  return (true);
}

/**
 * Write a block of 32 bit registers into SC hardware via PCIe bus.
 * @param offset address of device registers to write to
 * @param val location of 32 bit words to write into SC
 * @param len number of 32 bit words to write
 * @param incAddr true if address increments for each operation, false to
 *        access the same address each time (FIFO access)
 * @return true;  false if address not multiple of 4
 */
bool LSCVDMA_IF::write32(uint32_t offset, uint32_t *val, size_t len,
                         bool incAddr) {
  vdma_rw_ioctl_t rw;
  size_t i;

  if (offset & 0x03) {
    return false; // error! must be multiple of 4
  }

  if (incAddr) {
    // loop to copy len bytes from  user's array to BAR addr
    for (i = 0; i < len; i++) {
      rw.reg = offset;
      rw.value = *val;

      if (ioctl(hDev, IOCTL_LSCVDMA_WRITE_32BIT, &rw) != 0) {
        throw(LSCVDMA_IF_Error("LSCVDMA_IF: block write32 failed! \n"));
      }
      offset += 4;
      ++val;
    }
  }

  return (true);
}

/**
 * Open a device driver via a device interface and return the Linux file handle.
 * The device to open is specified using the /dev filename (pointed to by the
 * GUID) The instance number of the device can also be specified, such as open
 * the 2nd board of this type.  If the driver supports opening various
 * functions, such as DMA using a specific channel, then specify this function
 * as a string in the pFunctionName parameter.
 * @param hnd reference to caller supplied HANDLE to return open driver
 * @param pBoardID pointer a string identifying the PCI Vendor_Device pair in
 * board's CFG0 space
 * @param pDemoID pointer a string identifying the SubSys register values used
 * to identify a Demo
 * @param instance which instance of that board, if multiple in system
 * @param pFriendlyName string specifying the Window registery friendly name
 * associated with device (NULL for now)
 * @param pFunctionName string specifying driver specific function to open
 * (appended to the filename at open time)
 */
int LSCVDMA_IF::OpenDriver(HANDLE &hnd, const char *pBoardID,
                           const char *pDemoID, uint32_t instance,
                           const char *pFriendlyName,
                           const char *pFunctionName) {
  int ret = 0;
  int fd;
  char drivername[MAX_DEV_FILENAME_SIZE];

  pFriendlyName = pFriendlyName;

  if ((pFunctionName != NULL) &&
      (strlen(pFunctionName) >= MAX_FUNCTION_NAME_SIZE)) {
    ERRORSTR("ERROR: FunctionName string too large\n");
    return (PARAM_ERROR);
  }

  if ((pBoardID == NULL) || (strlen(pBoardID) > MAX_BOARD_ID_SIZE)) {
    ERRORSTR("ERROR: invalid BoardID string!\n");
    return (PARAM_ERROR);
  }

  if ((pDemoID == NULL) || (strlen(pDemoID) > MAX_DEMO_ID_SIZE)) {
    ERRORSTR("ERROR: invalid DemoID string!\n");
    return (PARAM_ERROR);
  }

  sprintf(drivername, "/dev/%s_%s_%d", pBoardID, pDemoID, instance);
  cout << "Opening " << drivername << endl;

  if (pFunctionName != NULL) {
    strcat(drivername, pFunctionName);
  }

  fd = open(drivername, O_RDWR, 0666);
  if (fd == -1) {
    perror("OpenDriver: open");
    ERRORSTR("ERROR: Device file not found!\n");
    return (OPEN_FILE_FAILED);
  }

  hnd = fd; // return the handle to the open device

  return (ret);
}
