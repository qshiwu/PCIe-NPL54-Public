/*
 * This file is part of the Lattice Video DMA IP Core driver for Linux
 *
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#ifndef __LSC_VDMA_REGS_H__
#define __LSC_VDMA_REGS_H__
#include <linux/kernel.h>
//VDMA_VERSION Register 
#define REG_VDMA_VER                                  0x000
#define VDMA_MAJOR_VER_MASK                           0xF0000000 //bit31~28
#define VDMA_MINOR_VER_MASK                           0x0FF00000 //bit27~20
#define VDMA_MINOR_UPDATE_MASK                        0x000F0000 //bit19~16
#define VDMA_LSC_INTERNAL_MASK                        0x0000FFFF //bit15~0


//VDMA_CAP Register 
#define REG_VDMA_CAP                                  0x004
//RESERVED                                                       //bit31~28

#define VDMA_CAP_H2C_MUL_DMA_CHAN_MASK                0x0F000000 //bit27~24
#define BIT_CAP_H2C_MUL_DMA_CHAN_NOT_SUPPORT          0x00000000
#define BIT_CAP_H2C_MUL_DMA_CHAN_MAX_2                0x01000000
#define BIT_CAP_H2C_MUL_DMA_CHAN_MAX_3                0x02000000
#define BIT_CAP_H2C_MUL_DMA_CHAN_MAX_4                0x03000000

#define VDMA_CAP_C2H_MUL_DMA_CHAN_MASK                0x00F00000 //bit23~20
#define BIT_CAP_C2H_MUL_DMA_CHAN_NOT_SUPPORT          0x00000000
#define BIT_CAP_C2H_MUL_DMA_CHAN_MAX_2                0x00100000
#define BIT_CAP_C2H_MUL_DMA_CHAN_MAX_3                0x00200000
#define BIT_CAP_C2H_MUL_DMA_CHAN_MAX_4                0x00300000

//RESERVED                                                       //bit19~12

#define VDMA_CAP_LRAM_CAP_MASK                        0x00000F00 //bit11~8
#define BIT_CAP_LRAM_CAP_NONE                         0x00000000
#define BIT_CAP_LRAM_CAP_512kb                        0x00000100
#define BIT_CAP_LRAM_CAP_1024kb                       0x00000200
#define BIT_CAP_LRAM_CAP_1536kb                       0x00000300
#define BIT_CAP_LRAM_CAP_2048kb                       0x00000400
#define BIT_CAP_LRAM_CAP_2560kb                       0x00000500
#define BIT_CAP_LRAM_CAP_3072kb                       0x00000600
#define BIT_CAP_LRAM_CAP_3584kb                       0x00000700

#define VDMA_CAP_C2H_AUD_MASK                         0x000000C0 //bit7~6
#define BIT_CAP_C2H_AUD_NOT_SUPPORT                   0x00000000
#define BIT_CAP_C2H_AUD_SUPPORT                       0x00000010

#define VDMA_CAP_DMA_DIR_MASK                         0x00000030 //bit5~4
#define BIT_CAP_DMA_DIR_C2H                           0x00000000
#define BIT_CAP_DMA_DIR_H2C                           0x00000010
#define BIT_CAP_DMA_DIR_BOTH                          0x00000020

#define VDMA_CAP_PCIE_IP_WIDTH_MASK                   0x0000000C //bit3~2
#define BIT_CAP_PCIE_IP_WIDTH_X1                      0x00000000
#define BIT_CAP_PCIE_IP_WIDTH_X2                      0x00000004
#define BIT_CAP_PCIE_IP_WIDTH_X4                      0x00000008

#define VDMA_CAP_PCIE_IP_VER_MASK                     0x00000003 //bit1~0
#define BIT_CAP_PCIE_IP_VER_GEN1                      0x00000000
#define BIT_CAP_PCIE_IP_VER_GEN2                      0x00000001
#define BIT_CAP_PCIE_IP_VER_GEN3                      0x00000002


//VDMA_C2H_CR Register 
#define REG_VDMA_C2H_CR                               0x008
#define VDMA_C2H_CR_FB_INS_NUM_MASK                   0xF8000000  //bit31~27

#define VDMA_C2H_CR_RO_EN_MASK                        0x00400000  //bit22
#define BIT_C2H_CR_RO_DIS                             0x00000000
#define BIT_C2H_CR_RO_EN                              0x00400000

#define VDMA_C2H_CR_MRD_CPLD_TIMEOUT_MASK             0x00380000  //bit21~19
#define BIT_C2H_CR_MRD_NOT_RESEND                     0x00000000
#define BIT_C2H_CR_MRD_AUDDEC_RESEND                  0x00080000
#define BIT_C2H_CR_MRD_AUDDEC_256MS_RESEND            0x00100000
#define BIT_C2H_CR_MRD_AUDDEC_512MS_RESEND            0x00180000
#define BIT_C2H_CR_MRD_AUDDEC_1S_RESEND               0x00200000
#define BIT_C2H_CR_MRD_AUDDEC_2S_RESEND               0x00280000
#define BIT_C2H_CR_MRD_AUDDEC_4S_RESEND               0x00300000
#define BIT_C2H_CR_MRD_AUDDEC_16S_RESEND              0x00380000

#define VDMA_C2H_CR_IDO_EN_MASK                       0x00040000  //bit18
#define BIT_C2H_CR_IDO_EN                             0x00040000
#define BIT_C2H_CR_IDO_DIS                            0x00000000

#define VDMA_C2H_CR_INT_METHOD_MASK                   0x00020000  //bit17
#define BIT_C2H_CR_INT_METHOD_MSI                     0x00000000
#define BIT_C2H_CR_INT_METHOD_POLL                    0x00020000

#define VDMA_C2H_CR_HW_FRME_DROP_MASK                 0x00010000  //bit16
#define BIT_C2H_CR_HW_FRME_DROP_EN                    0x00010000
#define BIT_C2H_CR_HW_FRME_DROP_DIS                   0x00000000

#define VDMA_C2H_CR_ALL_CH_BUF_LOCK_MASK              0x0000F000  //bit15-bit12
#define BIT_C2H_CR_ALL_CH_BUF_LOCK_DIS                0x00000000
#define BIT_C2H_CR_ALL_CH_BUF_LOCK_EN                 0x0000F000  

#define VDMA_C2H_CR_CH3_BUF_LOCK_MASK			0x00008000  //bit15
#define BIT_C2H_CR_CH3_BUF_LOCK_DIS                   0x00000000
#define BIT_C2H_CR_CH3_BUF_LOCK_EN                    0x00008000 

#define VDMA_C2H_CR_CH2_BUF_LOCK_MASK			0x00004000  //bit14
#define BIT_C2H_CR_CH2_BUF_LOCK_DIS                   0x00000000
#define BIT_C2H_CR_CH2_BUF_LOCK_EN                    0x00004000 

#define VDMA_C2H_CR_CH1_BUF_LOCK_MASK			0x00002000  //bit13
#define BIT_C2H_CR_CH1_BUF_LOCK_DIS                   0x00000000
#define BIT_C2H_CR_CH1_BUF_LOCK_EN                    0x00002000  

#define VDMA_C2H_CR_CH0_BUF_LOCK_MASK			0x00001000  //bit12
#define BIT_C2H_CR_CH0_BUF_LOCK_DIS                   0x00000000
#define BIT_C2H_CR_CH0_BUF_LOCK_EN                    0x00001000  

#define VDMA_C2H_CR_DMA_CHANS_MASK                    0x000000F0  //bit7~4

#define VDMA_C2H_CR_DMA_CHAN0_MASK                    0x00000010 
#define BIT_C2H_CR_DMA_CHAN0_RUN                      0x00000010
#define BIT_C2H_CR_DMA_CHAN0_STOP                     0x00000000

#define VDMA_C2H_CR_DMA_CHAN1_MASK                    0x00000020
#define BIT_C2H_CR_DMA_CHAN1_RUN                      0x00000020
#define BIT_C2H_CR_DMA_CHAN1_STOP                     0x00000000

#define VDMA_C2H_CR_DMA_CHAN2_MASK                    0x00000040
#define BIT_C2H_CR_DMA_CHAN2_RUN                      0x00000040
#define BIT_C2H_CR_DMA_CHAN2_STOP                     0x00000000

#define VDMA_C2H_CR_DMA_CHAN3_MASK                    0x00000080
#define BIT_C2H_CR_DMA_CHAN3_RUN                      0x00000080
#define BIT_C2H_CR_DMA_CHAN3_STOP                     0x00000000

#define VDMA_C2H_CR_GLOBAL_RST_MASK                   0x00000008  //bit3
#define BIT_C2H_CR_GLOBAL_RST                         0x00000008
#define BIT_C2H_CR_GLOBAL_RST_DEASSERT                0x00000000

#define VDMA_C2H_CR_SG_STORE_MODE_MASK                0x00000002  //bit1
#define BIT_C2H_CR_SG_STORE_MODE1                     0x00000002
#define BIT_C2H_CR_SG_STORE_MODE0                     0x00000000

#define VDMA_C2H_CR_DMA_EN_MASK                       0x00000001  //bit0
#define BIT_C2H_CR_DMA_EN                             0x00000001
#define BIT_C2H_CR_DMA_DIS                            0x00000000


//VDMA_C2H_SR Register 
#define REG_VDMA_C2H_SR                               0x00C
#define VDMA_C2H_SR_CH3_TRANS_DONE_FRME_INX_MASK      0xF8000000  //bit31~27

#define VDMA_C2H_SR_CH2_TRANS_DONE_FRME_INX_MASK      0x07C00000  //bit26~22

#define VDMA_C2H_SR_CH1_TRANS_DONE_FRME_INX_MASK      0x003E0000  //bit21~17

#define VDMA_C2H_SR_CH0_TRANS_DONE_FRME_INX_MASK      0x0001F000  //bit16~12

#define VDMA_C2H_SR_CH3_SG_BUF_RDY_MASK               0x00000080  //bit7
#define VDMA_C2H_SR_CH2_SG_BUF_RDY_MASK               0x00000040  //bit6
#define VDMA_C2H_SR_CH1_SG_BUF_RDY_MASK               0x00000020  //bit5
#define VDMA_C2H_SR_CH0_SG_BUF_RDY_MASK               0x00000010  //bit4

#define VDMA_C2H_SR_CH3_DMA_RUN_STATE_MASK            0x00000008  //bit3
#define BIT_C2H_SR_CH3_DMA_RUN_STATE_NORMAL           0x00000000
#define BIT_C2H_SR_CH3_DMA_RUN_STATE_HALTED           0x00000008  

#define VDMA_C2H_SR_CH2_DMA_RUN_STATE_MASK            0x00000004  //bit2
#define BIT_C2H_SR_CH2_DMA_RUN_STATE_NORMAL           0x00000000
#define BIT_C2H_SR_CH2_DMA_RUN_STATE_HALTED           0x00000004  

#define VDMA_C2H_SR_CH1_DMA_RUN_STATE_MASK            0x00000002  //bit1
#define BIT_C2H_SR_CH1_DMA_RUN_STATE_NORMAL           0x00000000
#define BIT_C2H_SR_CH1_DMA_RUN_STATE_HALTED           0x00000002  

#define VDMA_C2H_SR_CH0_DMA_RUN_STATE_MASK            0x00000001  //bit0
#define BIT_C2H_SR_CH0_DMA_RUN_STATE_NORMAL           0x00000000
#define BIT_C2H_SR_CH0_DMA_RUN_STATE_HALTED           0x00000001   


//VDMA_C2H_CH0_BUF_LOCK Register 
#define REG_VDMA_C2H_CH0_BUFLOC                       0x010
#define VDMA_C2H_CH0_BUFLOC15_MASK                    0x00008000  //bit15
#define BIT_C2H_CH0_BUFLOC15_CLR                      0x00008000

#define VDMA_C2H_CH0_BUFLOC14_MASK                    0x00004000  //bit14
#define BIT_C2H_CH0_BUFLOC14_CLR                      0x00004000

#define VDMA_C2H_CH0_BUFLOC13_MASK                    0x00002000  //bit13
#define BIT_C2H_CH0_BUFLOC13_CLR                      0x00002000

#define VDMA_C2H_CH0_BUFLOC12_MASK                    0x00001000  //bit12
#define BIT_C2H_CH0_BUFLOC12_CLR                      0x00001000

#define VDMA_C2H_CH0_BUFLOC11_MASK                    0x00000800  //bit11
#define BIT_C2H_CH0_BUFLOC11_CLR                      0x00000800

#define VDMA_C2H_CH0_BUFLOC10_MASK                    0x00000400  //bit10
#define BIT_C2H_CH0_BUFLOC10_CLR                      0x00000400
 
#define VDMA_C2H_CH0_BUFLOC9_MASK                     0x00000200  //bit9
#define BIT_C2H_CH0_BUFLOC9_CLR                       0x00000200

#define VDMA_C2H_CH0_BUFLOC8_MASK                     0x00000100  //bit8
#define BIT_C2H_CH0_BUFLOC8_CLR                       0x00000100

#define VDMA_C2H_CH0_BUFLOC7_MASK                     0x00000080  //bit7
#define BIT_C2H_CH0_BUFLOC7_CLR                       0x00000080

#define VDMA_C2H_CH0_BUFLOC6_MASK                     0x00000040  //bit6
#define BIT_C2H_CH0_BUFLOC6_CLR                       0x00000040

#define VDMA_C2H_CH0_BUFLOC5_MASK                     0x00000020  //bit5
#define BIT_C2H_CH0_BUFLOC5_CLR                       0x00000020

#define VDMA_C2H_CH0_BUFLOC4_MASK                     0x00000010  //bit4
#define BIT_C2H_CH0_BUFLOC4_CLR                       0x00000010

#define VDMA_C2H_CH0_BUFLOC3_MASK                     0x00000008  //bit3
#define BIT_C2H_CH0_BUFLOC3_CLR                       0x00000008

#define VDMA_C2H_CH0_BUFLOC2_MASK                     0x00000004  //bit2
#define BIT_C2H_CH0_BUFLOC2_CLR                       0x00000004

#define VDMA_C2H_CH0_BUFLOC1_MASK                     0x00000002  //bit1
#define BIT_C2H_CH0_BUFLOC1_CLR                       0x00000002

#define VDMA_C2H_CH0_BUFLOC0_MASK                     0x00000001  //bit0
#define BIT_C2H_CH0_BUFLOC0_CLR                       0x00000001


//VDMA_C2H_CH0_IRQ_COUNTER Register
#define REG_VDMA_C2H_CH0_IRQ_CNT                      0x014
#define VDMA_C2H_CH0_IRQ_CNT_VIDEO_IN_HALTED_MASK     0x000000F0  //bit7~4
#define BIT_C2H_CH0_VIDEO_IN_HALTED_TIME_NONE         0x00000000
#define BIT_C2H_CH0_VIDEO_IN_HALTED_TIME_100MS        0x00000010
#define BIT_C2H_CH0_VIDEO_IN_HALTED_TIME_200MS        0x00000020
#define BIT_C2H_CH0_VIDEO_IN_HALTED_TIME_500MS        0x00000030          
#define BIT_C2H_CH0_VIDEO_IN_HALTED_TIME_1S           0x00000040
#define BIT_C2H_CH0_VIDEO_IN_HALTED_TIME_2S           0x00000050
#define BIT_C2H_CH0_VIDEO_IN_HALTED_TIME_5S           0x00000060

#define VDMA_C2H_CH0_IRQ_CNT_FRME_TRANS_DONE_MASK     0x0000000F  //bit3~0
#define BIT_C2H_CH0_IRQ_CNT_FRME_TRANS_DONE_NONE      0x00000000
#define BIT_C2H_CH0_IRQ_CNT_FRME_TRANS_DONE_1         0x00000001
#define BIT_C2H_CH0_IRQ_CNT_FRME_TRANS_DONE_2         0x00000002
#define BIT_C2H_CH0_IRQ_CNT_FRME_TRANS_DONE_4         0x00000004


//VDMA_C2H_IRQ_MASK Register 
#define REG_VDMA_C2H_IRQ_MASK                         0x018
#define VDMA_C2H_IRQ_UNSUPPORT_MRdLk_REQ_MASK         0x80000000  //bit31
#define BIT_C2H_IRQ_UNSUPPORT_MRdLk_REQ_EN            0x00000000
#define BIT_C2H_IRQ_UNSUPPORT_MRdLk_REQ_DIS           0x80000000

#define VDMA_C2H_IRQ_UNSUPPORT_IO_REQ_MASK            0x40000000  //bit30
#define BIT_C2H_IRQ_UNSUPPORT_IO_REQ_EN               0x00000000
#define BIT_C2H_IRQ_UNSUPPORT_IO_REQ_DIS              0x40000000

#define VDMA_C2H_IRQ_UNSUPPORT_MES_REQ_MASK           0x20000000  //bit29
#define BIT_C2H_IRQ_UNSUPPORT_MES_REQ_EN              0x00000000
#define BIT_C2H_IRQ_UNSUPPORT_MES_REQ_DIS             0x20000000

#define VDMA_C2H_IRQ_UNSUPPORT_BAR_REQ_MASK           0x10000000  //bit28
#define BIT_C2H_IRQ_UNSUPPORT_BAR_REQ_EN              0x00000000
#define BIT_C2H_IRQ_UNSUPPORT_BAR_REQ_DIS             0x10000000

#define VDMA_C2H_IRQ_INCOMP_ELEMENT_MASK              0x08000000  //bit27
#define BIT_C2H_IRQ_INCOMP_ELEMENT_EN                 0x00000000
#define BIT_C2H_IRQ_INCOMP_ELEMENT_DIS                0x08000000

#define VDMA_C2H_IRQ_USR_DEF_INT4_MASK                0x00008000  //bit15
#define BIT_C2H_IRQ_USR_DEF_INT4_EN                   0x00000000
#define BIT_C2H_IRQ_USR_DEF_INT4_DIS                  0x00008000

#define VDMA_C2H_IRQ_USR_DEF_INT3_MASK                0x00004000  //bit14
#define BIT_C2H_IRQ_USR_DEF_INT3_EN                   0x00000000
#define BIT_C2H_IRQ_USR_DEF_INT3_DIS                  0x00004000

#define VDMA_C2H_IRQ_USR_DEF_INT2_MASK                0x08002000  //bit13
#define BIT_C2H_IRQ_USR_DEF_INT2_EN                   0x00000000
#define BIT_C2H_IRQ_USR_DEF_INT2_DIS                  0x00002000

#define VDMA_C2H_IRQ_USR_DEF_INT1_MASK                0x00001000  //bit12
#define BIT_C2H_IRQ_USR_DEF_INT1_EN                   0x00000000
#define BIT_C2H_IRQ_USR_DEF_INT1_DIS                  0x00001000

#define VDMA_C2H_IRQ_USR_DEF_INT0_MASK                0x00000800  //bit11
#define BIT_C2H_IRQ_USR_DEF_INT0_EN                   0x00000000
#define BIT_C2H_IRQ_USR_DEF_INT0_DIS                  0x00000800

#define VDMA_C2H_IRQ_FRAME_BROKEN_DET_MASK            0x00000400  //bit10
#define BIT_C2H_IRQ_FRAME_BROKEN_DET_EN               0x00000000
#define BIT_C2H_IRQ_FRAME_BROKEN_DET_DIS              0x00000400

#define VDMA_C2H_IRQ_DESC_TERM_INT_MASK	        0x00000010  //bit4
#define BIT_C2H_IRQ_DESC_TERM_INT_EN                  0x00000000
#define BIT_C2H_IRQ_DESC_TERM_INT_DIS                 0x00000010

#define VDMA_C2H_IRQ_SGB_ADDR_LEN_ERR_MASK            0x00000008  //bit3
#define BIT_C2H_IRQ_SGB_ADDR_LEN_ERR_EN               0x00000000
#define BIT_C2H_IRQ_SGB_ADDR_LEN_ERR_DIS              0x00000008

#define VDMA_C2H_IRQ_VIDEO_IN_HALTED_MASK             0x00000004  //bit2
#define BIT_C2H_IRQ_VIDEO_IN_HALTED_EN                0x00000000
#define BIT_C2H_IRQ_VIDEO_IN_HALTED_DIS               0x00000004

#define VDMA_C2H_IRQ_FRME_TRANS_DONE_DAT_LOS_MASK     0x00000002  //bit1
#define BIT_C2H_IRQ_FRME_TRANS_DONE_DAT_LOS_EN        0x00000000
#define BIT_C2H_IRQ_FRME_TRANS_DONE_DAT_LOS_DIS       0x00000002

#define VDMA_C2H_IRQ_FRME_TRANS_DONE_MASK             0x00000001  //bit0
#define BIT_C2H_IRQ_FRME_TRANS_DONE_EN                0x00000000
#define BIT_C2H_IRQ_FRME_TRANS_DONE_DIS               0x00000001


//VDMA_C2H_IRQ_SRC Register 
#define REG_VDMA_C2H_IRQ_SRC                          0x01C
#define VDMA_C2H_IRQ_SRC_UNSUPPORT_MRdLk_REQ_MASK     0x80000000  //bit31
#define VDMA_C2H_IRQ_SRC_UNSUPPORT_IO_REQ_MASK        0x40000000  //bit30
#define VDMA_C2H_IRQ_SRC_UNSUPPORT_MES_REQ_MASK       0x20000000  //bit29
#define VDMA_C2H_IRQ_SRC_UNSUPPORT_BAR_REQ_MASK       0x10000000  //bit28
#define VDMA_C2H_IRQ_SRC_INCOMP_ELEMENT_MASK          0x08000000  //bit27

#define VDMA_C2H_IRQ_SRC_USR_DEF_INT4_MASK            0x00008000  //bit15
#define BIT_C2H_IRQ_SRC_USR_DEF_INT4_CLR              0x00008000

#define VDMA_C2H_IRQ_SRC_USR_DEF_INT3_MASK            0x00004000  //bit14
#define BIT_C2H_IRQ_SRC_USR_DEF_INT3_CLR              0x00004000

#define VDMA_C2H_IRQ_SRC_USR_DEF_INT2_MASK            0x00002000  //bit13
#define BIT_C2H_IRQ_SRC_USR_DEF_INT2_CLR              0x00002000

#define VDMA_C2H_IRQ_SRC_USR_DEF_INT1_MASK            0x00001000  //bit12
#define BIT_C2H_IRQ_SRC_USR_DEF_INT1_CLR              0x00001000

#define VDMA_C2H_IRQ_SRC_USR_DEF_INT0_MASK            0x00000800  //bit11
#define BIT_C2H_IRQ_SRC_USR_DEF_INT0_CLR              0x00000800

#define VDMA_C2H_IRQ_SRC_DESC_TERM_INT_MASK	        0x00000010  //bit4
#define BIT_C2H_IRQ_SRC_DESC_TERM_INT_CLR             0x00000010

#define VDMA_C2H_IRQ_SRC_SGB_ADDR_LEN_ERR_MASK        0x00000008  //bit3
#define BIT_C2H_IRQ_SRC_SGB_ADDR_LEN_ERR_NORMAL       0x00000000
#define BIT_C2H_IRQ_SRC_SGB_ADDR_LEN_ERR_ZERO         0x00000008

#define VDMA_C2H_IRQ_SRC_VIDEO_IN_HALTED_MASK         0x00000004  //bit2
#define BIT_C2H_IRQ_SRC_VIDEO_IN_HALTED_CLR           0x00000004

#define VDMA_C2H_IRQ_SRC_FRME_TRANS_DONE_DAT_LOS_MASK 0x00000002  //bit1
#define BIT_C2H_IRQ_SRC_FRME_TRANS_DONE_DAT_LOS_CLR   0x00000002

#define VDMA_C2H_IRQ_SRC_FRME_TRANS_DONE_MASK         0x00000001  //bit0
#define BIT_C2H_IRQ_SRC_FRME_TRANS_DONE_CLR           0x00000001
#define BIT_C2H_IRQ_SRC_FRME_TRANS_INPROGRESS         0x00000000
 
 
//VDMA_C2H_CH0_FRAME_SIZE Register
#define REG_VDMA_C2H_CH0_FRAME_SIZE                   0x020
#define VDMA_C2H_CH0_FRME_SIZE_WIDTH_MASK             0xFFFF0000  //bit31~16
#define VDMA_C2H_CH0_FRME_SIZE_HEIGHT_MASK            0x0000FFFF  //bit15~0


//VDMA_C2H_CH0_AVI_INFO Register
#define REG_VDMA_C2H_CH0_AVI_INFO                     0x024
#define VDMA_C2H_CH0_AUD_SAMPLE_RATE_MASK             0x0F000000  //bit27~24
#define BIT_C2H_CH0_AUD_SAMPLE_RATE_32kHz             0x00000000
#define BIT_C2H_CH0_AUD_SAMPLE_RATE_32kHz             0x00000000
#define BIT_C2H_CH0_AUD_SAMPLE_RATE_44100Hz           0x01000000
#define BIT_C2H_CH0_AUD_SAMPLE_RATE_48kHz             0x02000000
#define BIT_C2H_CH0_AUD_SAMPLE_RATE_96kHz             0x03000000
#define BIT_C2H_CH0_AUD_SAMPLE_RATE_192kHz            0x04000000

#define VDMA_C2H_CH0_VIDEO_COL_DEPTH_MASK             0x000000F0  //bit7~4
#define BIT_C2H_CH0_VIDEO_COL_DEPTH_4BITS             0x00000000
#define BIT_C2H_CH0_VIDEO_COL_DEPTH_5BITS             0x00000010
#define BIT_C2H_CH0_VIDEO_COL_DEPTH_6BITS             0x00000020
#define BIT_C2H_CH0_VIDEO_COL_DEPTH_7BITS             0x00000030
#define BIT_C2H_CH0_VIDEO_COL_DEPTH_8BITS             0x00000040
#define BIT_C2H_CH0_VIDEO_COL_DEPTH_9BITS             0x00000050
#define BIT_C2H_CH0_VIDEO_COL_DEPTH_10BITS            0x00000060
#define BIT_C2H_CH0_VIDEO_COL_DEPTH_11BITS            0x00000070
#define BIT_C2H_CH0_VIDEO_COL_DEPTH_12BITS            0x00000080
#define BIT_C2H_CH0_VIDEO_COL_DEPTH_13BITS            0x00000090
#define BIT_C2H_CH0_VIDEO_COL_DEPTH_14BITS            0x000000A0
#define BIT_C2H_CH0_VIDEO_COL_DEPTH_16BITS            0x000000B0
#define BIT_C2H_CH0_VIDEO_COL_DEPTH_18BITS            0x000000C0
#define BIT_C2H_CH0_VIDEO_COL_DEPTH_20BITS            0x000000D0

#define VDMA_C2H_CH0_VIDEO_FORMAT_MASK                0x0000000F  //bit3~0
#define BIT_C2H_CH0_VIDEO_FORMAT_RGB                  0x00000000
#define BIT_C2H_CH0_VIDEO_FORMAT_YUV422               0x00000001
#define BIT_C2H_CH0_VIDEO_FORMAT_YUV444               0x00000002
#define BIT_C2H_CH0_VIDEO_FORMAT_YUV420               0x00000003
#define BIT_C2H_CH0_VIDEO_FORMAT_Y                    0x00000004
#define BIT_C2H_CH0_VIDEO_FORMAT_RAW                  0x00000005
#define BIT_C2H_CH0_VIDEO_FORMAT_RGB565               0x00000006


//VDMA PCIE MSI VECTOR Register
#define REG_VDMA_PCIE_MSI_VECTOR                      0x028
#define BIT_MSI_VECTOR_PCIE_INTERFACE_ERR		0x80000000 //bit31
#define BIT_MSI_VECTOR_VDMA_IP_ERR                    0x40000000 //bit30
#define BIT_MSI_VECTOR_USER_DEFINED_INT4		0x10000000 //bit28
#define BIT_MSI_VECTOR_USER_DEFINED_INT3		0x08000000 //bit27
#define BIT_MSI_VECTOR_USER_DEFINED_INT2		0x04000000 //bit26
#define BIT_MSI_VECTOR_USER_DEFINED_INT1		0x02000000 //bit25
#define BIT_MSI_VECTOR_USER_DEFINED_INT0		0x01000000 //bit24
#define BIT_C2H_MSI_VECTOR_CH3_VIDEO_INPUT_HALT       0x00800000 //bit23 
#define BIT_C2H_MSI_VECTOR_CH2_VIDEO_INPUT_HALT       0x00400000 //bit22
#define BIT_C2H_MSI_VECTOR_CH1_VIDEO_INPUT_HALT	0x00200000 //bit21
#define BIT_C2H_MSI_VECTOR_CH0_VIDEO_INPUT_HALT	0x00100000 //bit20
#define BIT_C2H_MSI_VECTOR_CH3_FRAME_DATA_LOSS	0x00080000 //bit19
#define BIT_C2H_MSI_VECTOR_CH2_FRAME_DATA_LOSS	0x00040000 //bit18
#define BIT_C2H_MSI_VECTOR_CH1_FRAME_DATA_LOSS	0x00020000 //bit17
#define BIT_C2H_MSI_VECTOR_CH0_FRAME_DATA_LOSS	0x00010000 //bit16
#define BIT_H2C_MSI_VECTOR_CH3_DESC_TERM_INT		0x00008000 //bit15
#define BIT_H2C_MSI_VECTOR_CH2_DESC_TERM_INT		0x00004000 //bit14
#define BIT_H2C_MSI_VECTOR_CH1_DESC_TERM_INT		0x00002000 //bit13
#define BIT_H2C_MSI_VECTOR_CH0_DESC_TERM_INT		0x00001000 //bit12
#define BIT_C2H_MSI_VECTOR_CH3_DESC_TERM_INT		0x00000800 //bit11
#define BIT_C2H_MSI_VECTOR_CH2_DESC_TERM_INT		0x00000400 //bit10
#define BIT_C2H_MSI_VECTOR_CH1_DESC_TERM_INT		0x00000200 //bit9
#define BIT_C2H_MSI_VECTOR_CH0_DESC_TERM_INT		0x00000100 //bit8
#define BIT_H2C_MSI_VECTOR_CH3_FRAME_TRANS_DONE	0x00000080 //bit7 
#define BIT_H2C_MSI_VECTOR_CH2_FRAME_TRANS_DONE	0x00000040 //bit6 
#define BIT_H2C_MSI_VECTOR_CH1_FRAME_TRANS_DONE	0x00000020 //bit5
#define BIT_H2C_MSI_VECTOR_CH0_FRAME_TRANS_DONE	0x00000010 //bit4 
#define BIT_C2H_MSI_VECTOR_CH3_FRAME_TRANS_DONE	0x00000008 //bit3 
#define BIT_C2H_MSI_VECTOR_CH2_FRAME_TRANS_DONE	0x00000004 //bit2 
#define BIT_C2H_MSI_VECTOR_CH1_FRAME_TRANS_DONE	0x00000002 //bit1
#define BIT_C2H_MSI_VECTOR_CH0_FRAME_TRANS_DONE	0x00000001 //bit0   

#define BIT_C2H_MSI_VECTOR_MASK_FRAME_TRANS_DONE	(BIT_C2H_MSI_VECTOR_CH0_FRAME_TRANS_DONE | BIT_C2H_MSI_VECTOR_CH1_FRAME_TRANS_DONE | BIT_C2H_MSI_VECTOR_CH2_FRAME_TRANS_DONE | BIT_C2H_MSI_VECTOR_CH3_FRAME_TRANS_DONE)
#define BIT_C2H_MSI_VECTOR_MASK_DESC_TERM_INT         (BIT_C2H_MSI_VECTOR_CH0_DESC_TERM_INT | BIT_C2H_MSI_VECTOR_CH1_DESC_TERM_INT | BIT_C2H_MSI_VECTOR_CH2_DESC_TERM_INT | BIT_C2H_MSI_VECTOR_CH3_DESC_TERM_INT)

#define BIT_C2H_MSI_VECTOR_MASK_FRAME_DATA_LOSS	(BIT_C2H_MSI_VECTOR_CH0_FRAME_DATA_LOSS | BIT_C2H_MSI_VECTOR_CH1_FRAME_DATA_LOSS | BIT_C2H_MSI_VECTOR_CH2_FRAME_DATA_LOSS | BIT_C2H_MSI_VECTOR_CH3_FRAME_DATA_LOSS )
#define BIT_C2H_MSI_VECTOR_MASK_VIDEO_INPUT_HALT	(BIT_C2H_MSI_VECTOR_CH0_VIDEO_INPUT_HALT | BIT_C2H_MSI_VECTOR_CH1_VIDEO_INPUT_HALT | BIT_C2H_MSI_VECTOR_CH2_VIDEO_INPUT_HALT | BIT_C2H_MSI_VECTOR_CH3_VIDEO_INPUT_HALT)
#define BIT_MSI_VECTOR_MASK_USR_DEFINED_INT	        (BIT_MSI_VECTOR_USER_DEFINED_INT0 | BIT_MSI_VECTOR_USER_DEFINED_INT1 | BIT_MSI_VECTOR_USER_DEFINED_INT2 | BIT_MSI_VECTOR_USER_DEFINED_INT3 | BIT_MSI_VECTOR_USER_DEFINED_INT4)
#define BIT_H2C_MSI_VECTOR_MASK_FRAME_TRANS_DONE	(BIT_H2C_MSI_VECTOR_CH0_FRAME_TRANS_DONE | BIT_H2C_MSI_VECTOR_CH1_FRAME_TRANS_DONE | BIT_H2C_MSI_VECTOR_CH2_FRAME_TRANS_DONE | BIT_H2C_MSI_VECTOR_CH3_FRAME_TRANS_DONE)
#define BIT_H2C_MSI_VECTOR_MASK_DESC_TERM_INT	        (BIT_H2C_MSI_VECTOR_CH0_DESC_TERM_INT | BIT_H2C_MSI_VECTOR_CH1_DESC_TERM_INT | BIT_H2C_MSI_VECTOR_CH2_DESC_TERM_INT | BIT_H2C_MSI_VECTOR_CH3_DESC_TERM_INT)


//VDMA_C2H_CH0_AVI_INFO Register
#define REG_VDMA_C2H_DMA_CH_CR                        0x02C
#define VDMA_C2H_DMA_CH3_CR_MASK                      0xF0000000  //bit31~28
#define VDMA_C2H_DMA_CH2_CR_MASK                      0x0F000000  //bit27~24
#define VDMA_C2H_DMA_CH1_CR_MASK                      0x00F00000  //bit23~20
#define VDMA_C2H_DMA_CH0_CR_MASK                      0x000F0000  //bit19~16

#define VDMA_C2H_DMA_PRI_SETTING_MASK                 0x000000F0  //bit7~4
#define BIT_C2H_DMA_PRI_CH0                           0x00000010  // Priority is Ch0 highest > Ch1 > Ch2 > Ch3 lowest
#define BIT_C2H_DMA_PRI_CH1                           0x00000020  // Priority is Ch1 highest > Ch2 > Ch3 > Ch0 lowest
#define BIT_C2H_DMA_PRI_CH2                           0x00000040  // Priority is Ch2 highest > Ch3 > Ch0 > Ch1 lowest
#define BIT_C2H_DMA_PRI_CH3                           0x00000080  // Priority is Ch3 highest > Ch0 > Ch1 > Ch2 lowest

#define VDMA_C2H_DMA_CH_ARB_MASK                      0x00000003  //bit1~0
#define BIT_C2H_DMA_CH_ARB_FIXED_PRI                  0x00000000  //Fixed Priority for each DMA channel
#define BIT_C2H_DMA_CH_ARB_SROUND_ROBIN               0x00000001  //Simple round robin
#define BIT_C2H_DMA_CH_ARB_WROUND_ROBIN               0x00000002  //Weighted round robin


//H2C Mode Control Register
#define REG_VDMA_H2C_CR                               0x030
#define VDMA_H2C_CR_FB_INS_NUM_MASK                   0xF8000000  //bit31~27

#define VDMA_H2C_CR_MRD_CPLD_TIMEOUT_MASK             0x00380000  //bit21~19
#define BIT_H2C_CR_MRD_NOT_RESEND                     0x00000000
#define BIT_H2C_CR_MRD_AUDDEC_RESEND                  0x00080000
#define BIT_H2C_CR_MRD_AUDDEC_256MS_RESEND            0x00100000
#define BIT_H2C_CR_MRD_AUDDEC_512MS_RESEND            0x00180000
#define BIT_H2C_CR_MRD_AUDDEC_1S_RESEND               0x00200000
#define BIT_H2C_CR_MRD_AUDDEC_2S_RESEND               0x00280000
#define BIT_H2C_CR_MRD_AUDDEC_4S_RESEND               0x00300000
#define BIT_H2C_CR_MRD_AUDDEC_16S_RESEND              0x00380000

#define VDMA_H2C_CR_IDO_EN_MASK                       0x00040000  //bit18
#define BIT_H2C_CR_IDO_EN                             0x00040000
#define BIT_H2C_CR_IDO_DIS                            0x00000000

#define VDMA_H2C_CR_INT_METHOD_MASK                   0x00020000  //bit17
#define BIT_H2C_CR_INT_METHOD_MSI                     0x00000000
#define BIT_H2C_CR_INT_METHOD_POLL                    0x00020000

#define VDMA_H2C_CR_ALL_CH_BUF_LOCK_MASK              0x0000F000  //bit15-bit12
#define BIT_H2C_CR_ALL_CH_BUF_LOCK_DIS                0x00000000
#define BIT_H2C_CR_ALL_CH_BUF_LOCK_EN                 0x0000F000  

#define VDMA_H2C_CR_CH3_BUF_LOCK_MASK			0x00008000  //bit15
#define BIT_H2C_CR_CH3_BUF_LOCK_DIS                   0x00000000
#define BIT_H2C_CR_CH3_BUF_LOCK_EN                    0x00008000 

#define VDMA_H2C_CR_CH2_BUF_LOCK_MASK			0x00004000  //bit14
#define BIT_H2C_CR_CH2_BUF_LOCK_DIS                   0x00000000
#define BIT_H2C_CR_CH2_BUF_LOCK_EN                    0x00004000 

#define VDMA_H2C_CR_CH1_BUF_LOCK_MASK			0x00002000  //bit13
#define BIT_H2C_CR_CH1_BUF_LOCK_DIS                   0x00000000
#define BIT_H2C_CR_CH1_BUF_LOCK_EN                    0x00002000  

#define VDMA_H2C_CR_CH0_BUF_LOCK_MASK			0x00001000  //bit12
#define BIT_H2C_CR_CH0_BUF_LOCK_DIS                   0x00000000
#define BIT_H2C_CR_CH0_BUF_LOCK_EN                    0x00001000  

#define VDMA_H2C_CR_DMA_CHANS_MASK                    0x000000F0  //bit7~4

#define VDMA_H2C_CR_DMA_CHAN0_MASK                    0x00000010 
#define BIT_H2C_CR_DMA_CHAN0_RUN                      0x00000010
#define BIT_H2C_CR_DMA_CHAN0_STOP                     0x00000000

#define VDMA_H2C_CR_DMA_CHAN1_MASK                    0x00000020
#define BIT_H2C_CR_DMA_CHAN1_RUN                      0x00000020
#define BIT_H2C_CR_DMA_CHAN1_STOP                     0x00000000

#define VDMA_H2C_CR_DMA_CHAN2_MASK                    0x00000040
#define BIT_H2C_CR_DMA_CHAN2_RUN                      0x00000040
#define BIT_H2C_CR_DMA_CHAN2_STOP                     0x00000000

#define VDMA_H2C_CR_DMA_CHAN3_MASK                    0x00000080
#define BIT_H2C_CR_DMA_CHAN3_RUN                      0x00000080
#define BIT_H2C_CR_DMA_CHAN3_STOP                     0x00000000

#define VDMA_H2C_CR_GLOBAL_RST_MASK                   0x00000008  //bit3
#define BIT_H2C_CR_GLOBAL_RST                         0x00000008
#define BIT_H2C_CR_GLOBAL_RST_DEASSERT                0x00000000

#define VDMA_H2C_CR_SG_STORE_MODE_MASK                0x00000002  //bit1
#define BIT_H2C_CR_SG_STORE_MODE1                     0x00000002
#define BIT_H2C_CR_SG_STORE_MODE0                     0x00000000

#define VDMA_H2C_CR_DMA_EN_MASK                       0x00000001  //bit0
#define BIT_H2C_CR_DMA_EN                             0x00000001
#define BIT_H2C_CR_DMA_DIS                            0x00000000


//H2C Mode Status Register
#define REG_VDMA_H2C_SR                               0x034
#define VDMA_H2C_SR_CH3_TRANS_DONE_FRME_INX_MASK      0xF8000000  //bit31~27

#define VDMA_H2C_SR_CH2_TRANS_DONE_FRME_INX_MASK      0x07C00000  //bit26~22

#define VDMA_H2C_SR_CH1_TRANS_DONE_FRME_INX_MASK      0x003E0000  //bit21~17

#define VDMA_H2C_SR_CH0_TRANS_DONE_FRME_INX_MASK      0x0001F000  //bit16~12

#define VDMA_H2C_SR_CH3_SG_BUF_RDY_MASK               0x00000080  //bit7
#define VDMA_H2C_SR_CH2_SG_BUF_RDY_MASK               0x00000040  //bit6
#define VDMA_H2C_SR_CH1_SG_BUF_RDY_MASK               0x00000020  //bit5
#define VDMA_H2C_SR_CH0_SG_BUF_RDY_MASK               0x00000010  //bit4

#define VDMA_H2C_SR_CH3_DMA_RUN_STATE_MASK            0x00000008  //bit3
#define BIT_H2C_SR_CH3_DMA_RUN_STATE_NORMAL           0x00000000
#define BIT_H2C_SR_CH3_DMA_RUN_STATE_HALTED           0x00000008  

#define VDMA_H2C_SR_CH2_DMA_RUN_STATE_MASK            0x00000004  //bit2
#define BIT_H2C_SR_CH2_DMA_RUN_STATE_NORMAL           0x00000000
#define BIT_H2C_SR_CH2_DMA_RUN_STATE_HALTED           0x00000004  

#define VDMA_H2C_SR_CH1_DMA_RUN_STATE_MASK            0x00000002  //bit1
#define BIT_H2C_SR_CH1_DMA_RUN_STATE_NORMAL           0x00000000
#define BIT_H2C_SR_CH1_DMA_RUN_STATE_HALTED           0x00000002  

#define VDMA_H2C_SR_CH0_DMA_RUN_STATE_MASK            0x00000001  //bit0
#define BIT_H2C_SR_CH0_DMA_RUN_STATE_NORMAL           0x00000000
#define BIT_H2C_SR_CH0_DMA_RUN_STATE_HALTED           0x00000001   


//H2C Mode Host Frame Buffer Lock Register for DMA Channel 0,1,2,3
#define REG_VDMA_H2C_BUFLOC(ch_id)                    (0x038 + ch_id * 0x400) //0x038, 0x438, 0x838, 0xC38
#define VDMA_H2C_CH0_BUFLOC15_MASK                    0x00008000  //bit15
#define BIT_H2C_CH0_BUFLOC15_CLR                      0x00008000

#define VDMA_H2C_CHX_BUFLOC14_MASK                    0x00004000  //bit14
#define BIT_H2C_CHX_BUFLOC14_CLR                      0x00004000

#define VDMA_H2C_CHX_BUFLOC13_MASK                    0x00002000  //bit13
#define BIT_H2C_CHX_BUFLOC13_CLR                      0x00002000

#define VDMA_H2C_CHX_BUFLOC12_MASK                    0x00001000  //bit12
#define BIT_H2C_CHX_BUFLOC12_CLR                      0x00001000

#define VDMA_H2C_CHX_BUFLOC11_MASK                    0x00000800  //bit11
#define BIT_H2C_CHX_BUFLOC11_CLR                      0x00000800

#define VDMA_H2C_CHX_BUFLOC10_MASK                    0x00000400  //bit10
#define BIT_H2C_CHX_BUFLOC10_CLR                      0x00000400
 
#define VDMA_H2C_CHX_BUFLOC9_MASK                     0x00000200  //bit9
#define BIT_H2C_CHX_BUFLOC9_CLR                       0x00000200

#define VDMA_H2C_CHX_BUFLOC8_MASK                     0x00000100  //bit8
#define BIT_H2C_CHX_BUFLOC8_CLR                       0x00000100

#define VDMA_H2C_CHX_BUFLOC7_MASK                     0x00000080  //bit7
#define BIT_H2C_CHX_BUFLOC7_CLR                       0x00000080

#define VDMA_H2C_CHX_BUFLOC6_MASK                     0x00000040  //bit6
#define BIT_H2C_CHX_BUFLOC6_CLR                       0x00000040

#define VDMA_H2C_CHX_BUFLOC5_MASK                     0x00000020  //bit5
#define BIT_H2C_CHX_BUFLOC5_CLR                       0x00000020

#define VDMA_H2C_CHX_BUFLOC4_MASK                     0x00000010  //bit4
#define BIT_H2C_CHX_BUFLOC4_CLR                       0x00000010

#define VDMA_H2C_CHX_BUFLOC3_MASK                     0x00000008  //bit3
#define BIT_H2C_CHX_BUFLOC3_CLR                       0x00000008

#define VDMA_H2C_CHX_BUFLOC2_MASK                     0x00000004  //bit2
#define BIT_H2C_CHX_BUFLOC2_CLR                       0x00000004

#define VDMA_H2C_CHX_BUFLOC1_MASK                     0x00000002  //bit1
#define BIT_H2C_CHX_BUFLOC1_CLR                       0x00000002

#define VDMA_H2C_CHX_BUFLOC0_MASK                     0x00000001  //bit0
#define BIT_H2C_CHX_BUFLOC0_CLR                       0x00000001


//H2C Mode Interrupt Counter Register for DMA Channel 0,1,2,3
#define REG_VDMA_H2C_IRQ_CNT(ch_id)                   (0x03C + ch_id * 0x400) //0x03C, 0x43C, 0x83C, 0xC3C
#define VDMA_H2C_CHX_IRQ_CNT_FRME_TRANS_DONE_MASK     0x0000000F  //bit3~0
#define BIT_H2C_CHX_IRQ_CNT_FRME_TRANS_DONE_NONE      0x00000000
#define BIT_H2C_CHX_IRQ_CNT_FRME_TRANS_DONE_1         0x00000001
#define BIT_H2C_CHX_IRQ_CNT_FRME_TRANS_DONE_2         0x00000002
#define BIT_H2C_CHX_IRQ_CNT_FRME_TRANS_DONE_4         0x00000004


//H2C Mode Interrupt Mask Register
#define REG_VDMA_H2C_IRQ_MASK                         0x040
#define VDMA_H2C_IRQ_DESC_TERM_INT_MASK               0x00000010  //bit4
#define BIT_H2C_IRQ_DESC_TERM_INT_EN                  0x00000000
#define BIT_H2C_IRQ_DESC_TERM_INT_DIS                 0x00000010

#define VDMA_H2C_IRQ_FRME_TRANS_DONE_MASK             0x00000001  //bit0
#define BIT_H2C_IRQ_FRME_TRANS_DONE_EN                0x00000000
#define BIT_H2C_IRQ_FRME_TRANS_DONE_DIS               0x00000001


//H2C Mode Interrupt Source Register
#define REG_VDMA_H2C_IRQ_SRC                          0x044
#define VDMA_H2C_IRQ_SRC_DESC_TERM_INT_MASK           0x00000010  //bit4
#define BIT_H2C_IRQ_SRC_DESC_TERM_INT_INPROGRESS      0x00000000
#define BIT_H2C_IRQ_SRC_DESC_TERM_INT_CLR             0x00000010

#define VDMA_H2C_IRQ_SRC_FRME_TRANS_DONE_MASK         0x00000001  //bit0
#define BIT_H2C_IRQ_SRC_FRME_TRANS_DONE_CLR           0x00000001
#define BIT_H2C_IRQ_SRC_FRME_TRANS_INPROGRESS         0x00000000


//H2C Mode Initial Frame Size Register for DMA Channel 0,1,2,3
#define REG_VDMA_H2C_FRAME_SIZE_INIT(ch_id)           (0x048 + ch_id * 0x400) //0x048, 0x448, 0x848, 0xC48
#define VDMA_H2C_FRME_SIZE_WIDTH_MASK                 0xFFFF0000  //bit31~16
#define VDMA_H2C_FRME_SIZE_HEIGHT_MASK                0x0000FFFF  //bit15~0


//H2C Mode Initial Video Information Register for DMA Channel 0,1,2,3
#define REG_VDMA_H2C_VID_INFO_INIT(ch_id)             (0x04C + ch_id * 0x400) //0x04C, 0x44C, 0x84C, 0xC4C
//RESERVED bit31~8

#define VDMA_H2C_VID_INFO_INIT_COL_DEPTH_MASK         0x000000F0  //bit7~4
#define BIT_H2C_VID_INFO_INIT_COL_DEPTH_4BITS         0x00000000
#define BIT_H2C_VID_INFO_INIT_COL_DEPTH_5BITS         0x00000010
#define BIT_H2C_VID_INFO_INIT_COL_DEPTH_6BITS         0x00000020
#define BIT_H2C_VID_INFO_INIT_COL_DEPTH_7BITS         0x00000030
#define BIT_H2C_VID_INFO_INIT_COL_DEPTH_8BITS         0x00000040
#define BIT_H2C_VID_INFO_INIT_COL_DEPTH_9BITS         0x00000050
#define BIT_H2C_VID_INFO_INIT_COL_DEPTH_10BITS        0x00000060
#define BIT_H2C_VID_INFO_INIT_COL_DEPTH_11BITS        0x00000070
#define BIT_H2C_VID_INFO_INIT_COL_DEPTH_12BITS        0x00000080
#define BIT_H2C_VID_INFO_INIT_COL_DEPTH_13BITS        0x00000090
#define BIT_H2C_VID_INFO_INIT_COL_DEPTH_14BITS        0x000000A0
#define BIT_H2C_VID_INFO_INIT_COL_DEPTH_16BITS        0x000000B0
#define BIT_H2C_VID_INFO_INIT_COL_DEPTH_18BITS        0x000000C0
#define BIT_H2C_VID_INFO_INIT_COL_DEPTH_20BITS        0x000000D0

#define VDMA_H2C_VID_INFO_INIT_FORMAT_MASK            0x0000000F  //bit3~0
#define BIT_H2C_VID_INFO_INIT_FORMAT_RGB              0x00000000
#define BIT_H2C_VID_INFO_INIT_FORMAT_YUV422           0x00000001
#define BIT_H2C_VID_INFO_INIT_FORMAT_YUV444           0x00000002
#define BIT_H2C_VID_INFO_INIT_FORMAT_YUV420           0x00000003
#define BIT_H2C_VID_INFO_INIT_FORMAT_Y                0x00000004
#define BIT_H2C_VID_INFO_INIT_FORMAT_RAW              0x00000005
#define BIT_H2C_VID_INFO_INIT_FORMAT_RGB565           0x00000006


//H2C Mode Multiple DMA Channel Control Register
#define REG_VDMA_H2C_DMA_CH_CR                        0x050
#define VDMA_H2C_DMA_PRI_SETTING_MASK                 0x000000F0  //bit7~4
#define BIT_H2C_DMA_PRI_CH0                           0x00000010  // Priority is Ch0 highest > Ch1 > Ch2 > Ch3 lowest
#define BIT_H2C_DMA_PRI_CH1                           0x00000020  // Priority is Ch1 highest > Ch2 > Ch3 > Ch0 lowest
#define BIT_H2C_DMA_PRI_CH2                           0x00000040  // Priority is Ch2 highest > Ch3 > Ch0 > Ch1 lowest
#define BIT_H2C_DMA_PRI_CH3                           0x00000080  // Priority is Ch3 highest > Ch0 > Ch1 > Ch2 lowest

#define VDMA_H2C_DMA_CH_ARB_MASK                      0x00000001  //bit0
#define BIT_H2C_DMA_CH_ARB_FIXED_PRI                  0x00000000  //Fixed Priority for each DMA channel
#define BIT_H2C_DMA_CH_ARB_SROUND_ROBIN               0x00000001  //Simple round robin


//#define Reserved                                    0x054


//VDMA_C2H_CH0_AUDIO_BUF_SIZE Register
#define REG_VDMA_C2H_CH0_AUDIO_BUF_SIZE               0x058
#define VDMA_C2H_CH0_AUD_BUF_SIZE_MASK                0x00FFFFFF  //bit23~0


//#define Reserved                     0x05C


//VDMA_C2H_CH0_SGB_ADDR_HIGH Register 
#define REG_VDMA_C2H_CH0_SGB_ADDR_HIGH(i)             (0x060 + i * 12)     //0x060+(0xC*i) [i=0~15]

//VDMA_C2H_CH0_SGB_ADDR_LOW Register
#define REG_VDMA_C2H_CH0_SGB_ADDR_LOW(i)              (0x064 + i * 12)     //0x064+(0xC*i) [i=0~15]

//VDMA_C2H_CH0_SGB_SGLIST_LEN Register 
#define REG_VDMA_C2H_CH0_SGB_SGLIST_LEN(i)            (0x068 + i * 12)     //0x068+(0xC*i) [i=0~15]

//H2C Mode SG-List Buffer Start Address High Register (1 to 16) for DMA Channel 0
#define REG_VDMA_H2C_CH0_SGB_ADDR_HIGH(i)             (0x120 + i * 12)     //0x120+(0xC*i) [i=0~15]

//H2C Mode SG-List Buffer Start Address Low Registers (1 to 16) for DMA Channel 0
#define REG_VDMA_H2C_CH0_SGB_ADDR_LOW(i)              (0x124 + i * 12)     //0x124+(0xC*i) [i=0~15]

//Defines the number of SG-Element in a Frame Buffer SG-List. (H2C Mode) (1 to 16) for DMA Channel 0
#define REG_VDMA_H2C_CH0_SGB_SGLIST_LEN(i)            (0x128 + i * 12)     //0x128+(0xC*i) [i=0~15]


//VDMA_C2H_SGB_ADDR_LOCAL Register 
#define REG_VDMA_C2H_SGB_ADDR_LOCAL                    0x1E0    //0x1E0~0x21C
#define REG_VDMA_H2C_SGB_ADDR_LOCAL                    0x220    //0x220~0x25C


//VDMA_C2H_NONINT_ADDR_HIGH Register 
#define REG_VDMA_C2H_NONINT_ADDR_HIGH(ch_id)           (0x260 + ch_id * 0x400)  //NONINT_ADDR_HI:CH0-0x260, CH1-0x660, CH2-0xA60, CH3-0xE60   


//VDMA_C2H_NONINT_ADDR_LOW Register
#define REG_VDMA_C2H_NONINT_ADDR_LOW(ch_id)            (0x264 + ch_id * 0x400)  //NONINT_ADDR_LO:CH0-0x264, CH1-0x664, CH2-0xA64， CH3-0xE64


//VDMA_C2H_FRAME_SIZE_CURR
#define REG_VDMA_C2H_FRAME_SIZE_CURR(ch_id)            (0x268 + ch_id * 0x400)  //FRAME_SIZE_CURR:CH0-0x268, CH1-0x668, CH2-0xA68, CH3-0xE68   


//VDMA_C2H_VIDEO_AUDIO_INFO_CURR
#define REG_VDMA_C2H_AVI_INFO_CURR(ch_id)              (0x26C + ch_id * 0x400)  //AVI_INFO_CURR:CH0-0x26C, CH1-0x66C, CH2-0xA6C, CH3-0xE6C 


//VDMA_H2C_NONINT_ADDR_HIGH Register 
#define REG_VDMA_H2C_NONINT_ADDR_HIGH(ch_id)           (0x270 + ch_id * 0x400)  //NONINT_ADDR_HI:CH0-0x270, CH1-0x670, CH2-0xA70, CH3-0xE70   


//VDMA_H2C_NONINT_ADDR_LOW Register
#define REG_VDMA_H2C_NONINT_ADDR_LOW(ch_id)            (0x274 + ch_id * 0x400)  //NONINT_ADDR_LO:CH0-0x274, CH1-0x674, CH2-0xA74, CH3-0xE74 


//H2C Mode Current Frame Size Register for DMA Channel 0
#define REG_VDMA_H2C_FRAME_SIZE_CURR(ch_id)            (0x278 + ch_id * 0x400)  //FRAME_SIZE_CURR:CH0-0x278, CH1-0x678, CH2-0xA78, CH3-0xE78
#define VDMA_H2C_FRME_SIZE_WIDTH_CURR_MASK            0xFFFF0000  //bit31~16
#define VDMA_H2C_FRME_SIZE_HEIGHT_CURR_MASK           0x0000FFFF  //bit15~0


//H2C Mode Current Video Information Register for DMA Channel 0
#define REG_VDMA_H2C_VID_INFO_CURR(ch_id)              (0x27C + ch_id * 0x400)  //FRAME_SIZE_CURR:CH0-0x27C, CH1-0x67C, CH2-0xA7C, CH3-0xE7C
#define VDMA_H2C_VID_INFO_CURR_COL_DEPTH_MASK         0x000000F0  //bit7~4
#define BIT_H2C_VID_INFO_CURR_COL_DEPTH_4BITS         0x00000000
#define BIT_H2C_VID_INFO_CURR_COL_DEPTH_5BITS         0x00000010
#define BIT_H2C_VID_INFO_CURR_COL_DEPTH_6BITS         0x00000020
#define BIT_H2C_VID_INFO_CURR_COL_DEPTH_7BITS         0x00000030
#define BIT_H2C_VID_INFO_CURR_COL_DEPTH_8BITS         0x00000040
#define BIT_H2C_VID_INFO_CURR_COL_DEPTH_9BITS         0x00000050
#define BIT_H2C_VID_INFO_CURR_COL_DEPTH_10BITS        0x00000060
#define BIT_H2C_VID_INFO_CURR_COL_DEPTH_11BITS        0x00000070
#define BIT_H2C_VID_INFO_CURR_COL_DEPTH_12BITS        0x00000080
#define BIT_H2C_VID_INFO_CURR_COL_DEPTH_13BITS        0x00000090
#define BIT_H2C_VID_INFO_CURR_COL_DEPTH_14BITS        0x000000A0
#define BIT_H2C_VID_INFO_CURR_COL_DEPTH_16BITS        0x000000B0
#define BIT_H2C_VID_INFO_CURR_COL_DEPTH_18BITS        0x000000C0
#define BIT_H2C_VID_INFO_CURR_COL_DEPTH_20BITS        0x000000D0

#define VDMA_H2C_VID_INFO_CURR_FORMAT_MASK            0x0000000F  //bit3~0
#define BIT_H2C_VID_INFO_CURR_FORMAT_RGB              0x00000000
#define BIT_H2C_VID_INFO_CURR_FORMAT_YUV422           0x00000001
#define BIT_H2C_VID_INFO_CURR_FORMAT_YUV444           0x00000002
#define BIT_H2C_VID_INFO_CURR_FORMAT_YUV420           0x00000003
#define BIT_H2C_VID_INFO_CURR_FORMAT_Y                0x00000004
#define BIT_H2C_VID_INFO_CURR_FORMAT_RAW              0x00000005
#define BIT_H2C_VID_INFO_CURR_FORMAT_RGB565           0x00000006


//C2H DMA Transmission Counter for Descriptor Termination for DMA channel 0,1,2,3
#define REG_VDMA_C2H_DESC_TERM_CNT(ch_id)              (0x280 + ch_id * 0x400)  // 0x280, 0x680, 0xA80, 0xE80


//H2C DMA Transmission Counter for Descriptor Termination for DMA channel 0,1,2,3
#define REG_VDMA_H2C_DESC_TERM_CNT(ch_id)              (0x284 + ch_id * 0x400)  // 0x284, 0x684, 0xA84, 0xE84


//Reserved 0x288 ~ 0x3E7


//Valid MWr Cycle Counter (per second) Register used for Throughput Calculation
#define REG_VDMA_H2C_TP_MWR_VALID                     0x3E8


//Valid MWr Cycle Counter (per second) Register used for Throughput Calculation
#define REG_VDMA_H2C_TP_MWR_INVALID                   0x3EC


//System information Register for Throughput Calculation
#define REG_VDMA_TP_SYS_INFO                          0x3F0
#define VDMA_TP_SYS_INFO_IP_BUS_WIDTH_MASK            0x0F000000  //bit27~24
#define BIT_TP_SYS_INFO_IP_BUS_WIDTH_32               0x00000000
#define BIT_TP_SYS_INFO_IP_BUS_WIDTH_64               0x01000000
#define BIT_TP_SYS_INFO_IP_BUS_WIDTH_128              0x02000000
#define BIT_TP_SYS_INFO_IP_BUS_WIDTH_256              0x03000000

#define VDMA_TP_SYS_INFO_IP_BUS_CLK_MASK              0x00FFFFFF //bit23~0


//Valid MWr Cycle Counter (per second) Register used for Throughput Calculation
#define REG_VDMA_C2H_TP_MWR_VALID                     0x3F4


//Invalid MWr Cycle Counter (per second) Register used for Throughput Calculation
#define REG_VDMA_C2H_TP_MWR_INVALID                   0x3F8


//RESERVED 0x3FC
//Reserved for 0x400 ~ 0x40F


//VDMA_C2H_CH1_BUF_LOCK Register 
#define REG_VDMA_C2H_CH1_BUFLOC                       0x410
#define VDMA_C2H_CH1_BUFLOC15_MASK                    0x00008000  //bit15
#define BIT_C2H_CH1_BUFLOC15_CLR                      0x00008000

#define VDMA_C2H_CH1_BUFLOC14_MASK                    0x00004000  //bit14
#define BIT_C2H_CH1_BUFLOC14_CLR                      0x00004000

#define VDMA_C2H_CH1_BUFLOC13_MASK                    0x00002000  //bit13
#define BIT_C2H_CH1_BUFLOC13_CLR                      0x00002000

#define VDMA_C2H_CH1_BUFLOC12_MASK                    0x00001000  //bit12
#define BIT_C2H_CH1_BUFLOC12_CLR                      0x00001000

#define VDMA_C2H_CH1_BUFLOC11_MASK                    0x00000800  //bit11
#define BIT_C2H_CH1_BUFLOC11_CLR                      0x00000800

#define VDMA_C2H_CH1_BUFLOC10_MASK                    0x00000400  //bit10
#define BIT_C2H_CH1_BUFLOC10_CLR                      0x00000400
 
#define VDMA_C2H_CH1_BUFLOC9_MASK                     0x00000200  //bit9
#define BIT_C2H_CH1_BUFLOC9_CLR                       0x00000200

#define VDMA_C2H_CH1_BUFLOC8_MASK                     0x00000100  //bit8
#define BIT_C2H_CH1_BUFLOC8_CLR                       0x00000100

#define VDMA_C2H_CH1_BUFLOC7_MASK                     0x00000080  //bit7
#define BIT_C2H_CH1_BUFLOC7_CLR                       0x00000080

#define VDMA_C2H_CH1_BUFLOC6_MASK                     0x00000040  //bit6
#define BIT_C2H_CH1_BUFLOC6_CLR                       0x00000040

#define VDMA_C2H_CH1_BUFLOC5_MASK                     0x00000020  //bit5
#define BIT_C2H_CH1_BUFLOC5_CLR                       0x00000020

#define VDMA_C2H_CH1_BUFLOC4_MASK                     0x00000010  //bit4
#define BIT_C2H_CH1_BUFLOC4_CLR                       0x00000010

#define VDMA_C2H_CH1_BUFLOC3_MASK                     0x00000008  //bit3
#define BIT_C2H_CH1_BUFLOC3_CLR                       0x00000008

#define VDMA_C2H_CH1_BUFLOC2_MASK                     0x00000004  //bit2
#define BIT_C2H_CH1_BUFLOC2_CLR                       0x00000004

#define VDMA_C2H_CH1_BUFLOC1_MASK                     0x00000002  //bit1
#define BIT_C2H_CH1_BUFLOC1_CLR                       0x00000002

#define VDMA_C2H_CH1_BUFLOC0_MASK                     0x00000001  //bit0
#define BIT_C2H_CH1_BUFLOC0_CLR                       0x00000001


//VDMA_C2H_CH1_IRQ_COUNTER Register
#define REG_VDMA_C2H_CH1_IRQ_CNT                      0x414
#define VDMA_C2H_CH1_IRQ_CNT_VIDEO_IN_HALTED_MASK     0x000000F0  //bit7~4
#define BIT_C2H_CH1_VIDEO_IN_HALTED_TIME_NONE         0x00000000
#define BIT_C2H_CH1_VIDEO_IN_HALTED_TIME_100MS        0x00000010
#define BIT_C2H_CH1_VIDEO_IN_HALTED_TIME_200MS        0x00000020
#define BIT_C2H_CH1_VIDEO_IN_HALTED_TIME_500MS        0x00000030          
#define BIT_C2H_CH1_VIDEO_IN_HALTED_TIME_1S           0x00000040
#define BIT_C2H_CH1_VIDEO_IN_HALTED_TIME_2S           0x00000050
#define BIT_C2H_CH1_VIDEO_IN_HALTED_TIME_5S           0x00000060

#define VDMA_C2H_CH1_IRQ_CNT_FRME_TRANS_DONE_MASK     0x0000000F  //bit3~0
#define BIT_C2H_CH1_IRQ_CNT_FRME_TRANS_DONE_NONE      0x00000000
#define BIT_C2H_CH1_IRQ_CNT_FRME_TRANS_DONE_1         0x00000001
#define BIT_C2H_CH1_IRQ_CNT_FRME_TRANS_DONE_2         0x00000002
#define BIT_C2H_CH1_IRQ_CNT_FRME_TRANS_DONE_4         0x00000004


//RESERVED for 0x418 ~ 0x41F


//VDMA_C2H_CH1_FRAME_SIZE Register
#define REG_VDMA_C2H_CH1_FRAME_SIZE                   0x420
#define VDMA_C2H_CH1_FRME_SIZE_WIDTH_MASK             0xFFFF0000  //bit31~16
#define VDMA_C2H_CH1_FRME_SIZE_HEIGHT_MASK            0x0000FFFF  //bit15~0


//VDMA_C2H_CH1_AVI_INFO Register
#define REG_VDMA_C2H_CH1_AVI_INFO                     0x424
#define VDMA_C2H_CH1_AUD_SAMPLE_RATE_MASK             0x0F000000  //bit27~24
#define BIT_C2H_CH1_AUD_SAMPLE_RATE_32kHz             0x00000000
#define BIT_C2H_CH1_AUD_SAMPLE_RATE_32kHz             0x00000000
#define BIT_C2H_CH1_AUD_SAMPLE_RATE_44100Hz           0x01000000
#define BIT_C2H_CH1_AUD_SAMPLE_RATE_48kHz             0x02000000
#define BIT_C2H_CH1_AUD_SAMPLE_RATE_96kHz             0x03000000
#define BIT_C2H_CH1_AUD_SAMPLE_RATE_192kHz            0x04000000

#define VDMA_C2H_CH1_VIDEO_COL_DEPTH_MASK             0x000000F0  //bit7~4
#define BIT_C2H_CH1_VIDEO_COL_DEPTH_4BITS             0x00000000
#define BIT_C2H_CH1_VIDEO_COL_DEPTH_5BITS             0x00000010
#define BIT_C2H_CH1_VIDEO_COL_DEPTH_6BITS             0x00000020
#define BIT_C2H_CH1_VIDEO_COL_DEPTH_7BITS             0x00000030
#define BIT_C2H_CH1_VIDEO_COL_DEPTH_8BITS             0x00000040
#define BIT_C2H_CH1_VIDEO_COL_DEPTH_9BITS             0x00000050
#define BIT_C2H_CH1_VIDEO_COL_DEPTH_10BITS            0x00000060
#define BIT_C2H_CH1_VIDEO_COL_DEPTH_11BITS            0x00000070
#define BIT_C2H_CH1_VIDEO_COL_DEPTH_12BITS            0x00000080
#define BIT_C2H_CH1_VIDEO_COL_DEPTH_13BITS            0x00000090
#define BIT_C2H_CH1_VIDEO_COL_DEPTH_14BITS            0x000000A0
#define BIT_C2H_CH1_VIDEO_COL_DEPTH_16BITS            0x000000B0
#define BIT_C2H_CH1_VIDEO_COL_DEPTH_18BITS            0x000000C0
#define BIT_C2H_CH1_VIDEO_COL_DEPTH_20BITS            0x000000D0

#define VDMA_C2H_CH1_VIDEO_FORMAT_MASK                0x0000000F  //bit3~0
#define BIT_C2H_CH1_VIDEO_FORMAT_RGB                  0x00000000
#define BIT_C2H_CH1_VIDEO_FORMAT_YUV422               0x00000001
#define BIT_C2H_CH1_VIDEO_FORMAT_YUV444               0x00000002
#define BIT_C2H_CH1_VIDEO_FORMAT_YUV420               0x00000003
#define BIT_C2H_CH1_VIDEO_FORMAT_Y                    0x00000004
#define BIT_C2H_CH1_VIDEO_FORMAT_RAW                  0x00000005
#define BIT_C2H_CH1_VIDEO_FORMAT_RGB565               0x00000006


//Reserved for 0x428 ~ 0x437


//Reserved for 0x440 ~ 0x447


//RESERVED for 0x44D ~ 0x457


//VDMA_C2H_CH1_AUDIO_BUF_SIZE Register
#define REG_VDMA_C2H_CH1_AUDIO_BUF_SIZE               0x458
#define VDMA_C2H_CH1_AUD_BUF_SIZE_MASK                0x00FFFFFF  //bit23~0


//RESERVED for 0x45C


//VDMA_C2H_CH1_SGB_ADDR_HIGH Register 
#define REG_VDMA_C2H_CH1_SGB_ADDR_HIGH(i)             (0x460 + i * 12)     //0x060+(0xC*i) [i=0~15]


//VDMA_C2H_CH1_SGB_ADDR_LOW Register
#define REG_VDMA_C2H_CH1_SGB_ADDR_LOW(i)              (0x464 + i * 12)     //0x064+(0xC*i) [i=0~15]


//VDMA_C2H_CH1_SGB_SGLIST_LEN Register 
#define REG_VDMA_C2H_CH1_SGB_SGLIST_LEN(i)            (0x468 + i * 12)     //0x068+(0xC*i) [i=0~15]


//H2C Mode SG-List Buffer Start Address High Register (1 to 16) for DMA Channel 1
#define REG_VDMA_H2C_CH1_SGB_ADDR_HIGH(i)             (0x520 + i * 12)     //0x520+(0xC*i) [i=0~15]


//H2C Mode SG-List Buffer Start Address Low Registers (1 to 16) for DMA Channel 1
#define REG_VDMA_H2C_CH1_SGB_ADDR_LOW(i)              (0x524 + i * 12)     //0x524+(0xC*i) [i=0~15]


//Defines the number of SG-Element in a Frame Buffer SG-List. (H2C Mode) (1 to 16) for DMA Channel 1
#define REG_VDMA_H2C_CH1_SGB_SGLIST_LEN(i)            (0x528 + i * 12)     //0x528+(0xC*i) [i=0~15]


//RESERVED for 0x5E0 ~ 0x65F
//RESERVED for 0x688 ~ 0x7FF


//VDMA_C2H_CH2_BUF_LOCK Register 
#define REG_VDMA_C2H_CH2_BUFLOC                       0x810
#define VDMA_C2H_CH2_BUFLOC15_MASK                    0x00008000  //bit15
#define BIT_C2H_CH2_BUFLOC15_CLR                      0x00008000

#define VDMA_C2H_CH2_BUFLOC14_MASK                    0x00004000  //bit14
#define BIT_C2H_CH2_BUFLOC14_CLR                      0x00004000

#define VDMA_C2H_CH2_BUFLOC13_MASK                    0x00002000  //bit13
#define BIT_C2H_CH2_BUFLOC13_CLR                      0x00002000

#define VDMA_C2H_CH2_BUFLOC12_MASK                    0x00001000  //bit12
#define BIT_C2H_CH2_BUFLOC12_CLR                      0x00001000

#define VDMA_C2H_CH2_BUFLOC11_MASK                    0x00000800  //bit11
#define BIT_C2H_CH2_BUFLOC11_CLR                      0x00000800

#define VDMA_C2H_CH2_BUFLOC10_MASK                    0x00000400  //bit10
#define BIT_C2H_CH2_BUFLOC10_CLR                      0x00000400
 
#define VDMA_C2H_CH2_BUFLOC9_MASK                     0x00000200  //bit9
#define BIT_C2H_CH2_BUFLOC9_CLR                       0x00000200

#define VDMA_C2H_CH2_BUFLOC8_MASK                     0x00000100  //bit8
#define BIT_C2H_CH2_BUFLOC8_CLR                       0x00000100

#define VDMA_C2H_CH2_BUFLOC7_MASK                     0x00000080  //bit7
#define BIT_C2H_CH2_BUFLOC7_CLR                       0x00000080

#define VDMA_C2H_CH2_BUFLOC6_MASK                     0x00000040  //bit6
#define BIT_C2H_CH2_BUFLOC6_CLR                       0x00000040

#define VDMA_C2H_CH2_BUFLOC5_MASK                     0x00000020  //bit5
#define BIT_C2H_CH2_BUFLOC5_CLR                       0x00000020

#define VDMA_C2H_CH2_BUFLOC4_MASK                     0x00000010  //bit4
#define BIT_C2H_CH2_BUFLOC4_CLR                       0x00000010

#define VDMA_C2H_CH2_BUFLOC3_MASK                     0x00000008  //bit3
#define BIT_C2H_CH2_BUFLOC3_CLR                       0x00000008

#define VDMA_C2H_CH2_BUFLOC2_MASK                     0x00000004  //bit2
#define BIT_C2H_CH2_BUFLOC2_CLR                       0x00000004

#define VDMA_C2H_CH2_BUFLOC1_MASK                     0x00000002  //bit1
#define BIT_C2H_CH2_BUFLOC1_CLR                       0x00000002

#define VDMA_C2H_CH2_BUFLOC0_MASK                     0x00000001  //bit0
#define BIT_C2H_CH2_BUFLOC0_CLR                       0x00000001


//VDMA_C2H_CH2_IRQ_COUNTER Register
#define REG_VDMA_C2H_CH2_IRQ_CNT                      0x814
#define VDMA_C2H_CH2_IRQ_CNT_VIDEO_IN_HALTED_MASK     0x000000F0  //bit7~4
#define BIT_C2H_CH2_VIDEO_IN_HALTED_TIME_NONE         0x00000000
#define BIT_C2H_CH2_VIDEO_IN_HALTED_TIME_100MS        0x00000010
#define BIT_C2H_CH2_VIDEO_IN_HALTED_TIME_200MS        0x00000020
#define BIT_C2H_CH2_VIDEO_IN_HALTED_TIME_500MS        0x00000030          
#define BIT_C2H_CH2_VIDEO_IN_HALTED_TIME_1S           0x00000040
#define BIT_C2H_CH2_VIDEO_IN_HALTED_TIME_2S           0x00000050
#define BIT_C2H_CH2_VIDEO_IN_HALTED_TIME_5S           0x00000060

#define VDMA_C2H_CH2_IRQ_CNT_FRME_TRANS_DONE_MASK     0x0000000F  //bit3~0
#define BIT_C2H_CH2_IRQ_CNT_FRME_TRANS_DONE_NONE      0x00000000
#define BIT_C2H_CH2_IRQ_CNT_FRME_TRANS_DONE_1         0x00000001
#define BIT_C2H_CH2_IRQ_CNT_FRME_TRANS_DONE_2         0x00000002
#define BIT_C2H_CH2_IRQ_CNT_FRME_TRANS_DONE_4         0x00000004


//VDMA_C2H_CH2_FRAME_SIZE Register
#define REG_VDMA_C2H_CH2_FRAME_SIZE                   0x820
#define VDMA_C2H_CH2_FRME_SIZE_WIDTH_MASK             0xFFFF0000  //bit31~16
#define VDMA_C2H_CH2_FRME_SIZE_HEIGHT_MASK            0x0000FFFF  //bit15~0


//VDMA_C2H_CH2_AVI_INFO Register
#define REG_VDMA_C2H_CH2_AVI_INFO                     0x824
#define VDMA_C2H_CH2_AUD_SAMPLE_RATE_MASK             0x0F000000  //bit27~24
#define BIT_C2H_CH2_AUD_SAMPLE_RATE_32kHz             0x00000000
#define BIT_C2H_CH2_AUD_SAMPLE_RATE_32kHz             0x00000000
#define BIT_C2H_CH2_AUD_SAMPLE_RATE_44100Hz           0x01000000
#define BIT_C2H_CH2_AUD_SAMPLE_RATE_48kHz             0x02000000
#define BIT_C2H_CH2_AUD_SAMPLE_RATE_96kHz             0x03000000
#define BIT_C2H_CH2_AUD_SAMPLE_RATE_192kHz            0x04000000

#define VDMA_C2H_CH2_VIDEO_COL_DEPTH_MASK             0x000000F0  //bit7~4
#define BIT_C2H_CH2_VIDEO_COL_DEPTH_4BITS             0x00000000
#define BIT_C2H_CH2_VIDEO_COL_DEPTH_5BITS             0x00000010
#define BIT_C2H_CH2_VIDEO_COL_DEPTH_6BITS             0x00000020
#define BIT_C2H_CH2_VIDEO_COL_DEPTH_7BITS             0x00000030
#define BIT_C2H_CH2_VIDEO_COL_DEPTH_8BITS             0x00000040
#define BIT_C2H_CH2_VIDEO_COL_DEPTH_9BITS             0x00000050
#define BIT_C2H_CH2_VIDEO_COL_DEPTH_10BITS            0x00000060
#define BIT_C2H_CH2_VIDEO_COL_DEPTH_11BITS            0x00000070
#define BIT_C2H_CH2_VIDEO_COL_DEPTH_12BITS            0x00000080
#define BIT_C2H_CH2_VIDEO_COL_DEPTH_13BITS            0x00000090
#define BIT_C2H_CH2_VIDEO_COL_DEPTH_14BITS            0x000000A0
#define BIT_C2H_CH2_VIDEO_COL_DEPTH_16BITS            0x000000B0
#define BIT_C2H_CH2_VIDEO_COL_DEPTH_18BITS            0x000000C0
#define BIT_C2H_CH2_VIDEO_COL_DEPTH_20BITS            0x000000D0

#define VDMA_C2H_CH2_VIDEO_FORMAT_MASK                0x0000000F  //bit3~0
#define BIT_C2H_CH2_VIDEO_FORMAT_RGB                  0x00000000
#define BIT_C2H_CH2_VIDEO_FORMAT_YUV422               0x00000001
#define BIT_C2H_CH2_VIDEO_FORMAT_YUV444               0x00000002
#define BIT_C2H_CH2_VIDEO_FORMAT_YUV420               0x00000003
#define BIT_C2H_CH2_VIDEO_FORMAT_Y                    0x00000004
#define BIT_C2H_CH2_VIDEO_FORMAT_RAW                  0x00000005
#define BIT_C2H_CH2_VIDEO_FORMAT_RGB565               0x00000006


//RESERVED for 0x828 ~ 0x837
//RESERVED for 0x840 ~ 0x847
//RESERVED for 0x850 ~ 0x857


//VDMA_C2H_CH2_AUDIO_BUF_SIZE Register
#define REG_VDMA_C2H_CH2_AUDIO_BUF_SIZE               0x858
#define VDMA_C2H_CH2_AUD_BUF_SIZE_MASK                0x00FFFFFF  //bit23~0


//VDMA_C2H_CH2_SGB_ADDR_HIGH Register 
#define REG_VDMA_C2H_CH2_SGB_ADDR_HIGH(i)             (0x860 + i * 12)     //0x060+(0xC*i) [i=0~15]


//VDMA_C2H_CH2_SGB_ADDR_LOW Register
#define REG_VDMA_C2H_CH2_SGB_ADDR_LOW(i)              (0x864 + i * 12)     //0x064+(0xC*i) [i=0~15]


//VDMA_C2H_CH2_SGB_SGLIST_LEN Register 
#define REG_VDMA_C2H_CH2_SGB_SGLIST_LEN(i)            (0x868 + i * 12)     //0x068+(0xC*i) [i=0~15]


//H2C Mode SG-List Buffer Start Address High Register (1 to 16) for DMA Channel 2
#define REG_VDMA_H2C_CH2_SGB_ADDR_HIGH(i)             (0x920 + i * 12)     //0x920+(0xC*i) [i=0~15]


//H2C Mode SG-List Buffer Start Address Low Registers (1 to 16) for DMA Channel 2
#define REG_VDMA_H2C_CH2_SGB_ADDR_LOW(i)              (0x924 + i * 12)     //0x924+(0xC*i) [i=0~15]


//Defines the number of SG-Element in a Frame Buffer SG-List. (H2C Mode) (1 to 16) for DMA Channel 2
#define REG_VDMA_H2C_CH2_SGB_SGLIST_LEN(i)            (0x928 + i * 12)     //0x928+(0xC*i) [i=0~15]


//RERSERVED for 0x9E0 ~ 0xA5F
//RESERVED for 0xA88 ~ 0xBFF
//RESERVED for 0xC00 ~ 0xC0F


//VDMA_C2H_CH3_BUF_LOCK Register 
#define REG_VDMA_C2H_CH3_BUFLOC                       0xC10
#define VDMA_C2H_CH3_BUFLOC15_MASK                    0x00008000  //bit15
#define BIT_C2H_CH3_BUFLOC15_CLR                      0x00008000

#define VDMA_C2H_CH3_BUFLOC14_MASK                    0x00004000  //bit14
#define BIT_C2H_CH3_BUFLOC14_CLR                      0x00004000

#define VDMA_C2H_CH3_BUFLOC13_MASK                    0x00002000  //bit13
#define BIT_C2H_CH3_BUFLOC13_CLR                      0x00002000

#define VDMA_C2H_CH3_BUFLOC12_MASK                    0x00001000  //bit12
#define BIT_C2H_CH3_BUFLOC12_CLR                      0x00001000

#define VDMA_C2H_CH3_BUFLOC11_MASK                    0x00000800  //bit11
#define BIT_C2H_CH3_BUFLOC11_CLR                      0x00000800

#define VDMA_C2H_CH3_BUFLOC10_MASK                    0x00000400  //bit10
#define BIT_C2H_CH3_BUFLOC10_CLR                      0x00000400
 
#define VDMA_C2H_CH3_BUFLOC9_MASK                     0x00000200  //bit9
#define BIT_C2H_CH3_BUFLOC9_CLR                       0x00000200

#define VDMA_C2H_CH3_BUFLOC8_MASK                     0x00000100  //bit8
#define BIT_C2H_CH3_BUFLOC8_CLR                       0x00000100

#define VDMA_C2H_CH3_BUFLOC7_MASK                     0x00000080  //bit7
#define BIT_C2H_CH3_BUFLOC7_CLR                       0x00000080

#define VDMA_C2H_CH3_BUFLOC6_MASK                     0x00000040  //bit6
#define BIT_C2H_CH3_BUFLOC6_CLR                       0x00000040

#define VDMA_C2H_CH3_BUFLOC5_MASK                     0x00000020  //bit5
#define BIT_C2H_CH3_BUFLOC5_CLR                       0x00000020

#define VDMA_C2H_CH3_BUFLOC4_MASK                     0x00000010  //bit4
#define BIT_C2H_CH3_BUFLOC4_CLR                       0x00000010

#define VDMA_C2H_CH3_BUFLOC3_MASK                     0x00000008  //bit3
#define BIT_C2H_CH3_BUFLOC3_CLR                       0x00000008

#define VDMA_C2H_CH3_BUFLOC2_MASK                     0x00000004  //bit2
#define BIT_C2H_CH3_BUFLOC2_CLR                       0x00000004

#define VDMA_C2H_CH3_BUFLOC1_MASK                     0x00000002  //bit1
#define BIT_C2H_CH3_BUFLOC1_CLR                       0x00000002

#define VDMA_C2H_CH3_BUFLOC0_MASK                     0x00000001  //bit0
#define BIT_C2H_CH3_BUFLOC0_CLR                       0x00000001


//VDMA_C2H_CH3_IRQ_COUNTER Register
#define REG_VDMA_C2H_CH3_IRQ_CNT                      0xC14
#define VDMA_C2H_CH3_IRQ_CNT_VIDEO_IN_HALTED_MASK     0x000000F0  //bit7~4
#define BIT_C2H_CH3_VIDEO_IN_HALTED_TIME_NONE         0x00000000
#define BIT_C2H_CH3_VIDEO_IN_HALTED_TIME_100MS        0x00000010
#define BIT_C2H_CH3_VIDEO_IN_HALTED_TIME_200MS        0x00000020
#define BIT_C2H_CH3_VIDEO_IN_HALTED_TIME_500MS        0x00000030          
#define BIT_C2H_CH3_VIDEO_IN_HALTED_TIME_1S           0x00000040
#define BIT_C2H_CH3_VIDEO_IN_HALTED_TIME_2S           0x00000050
#define BIT_C2H_CH3_VIDEO_IN_HALTED_TIME_5S           0x00000060

#define VDMA_C2H_CH3_IRQ_CNT_FRME_TRANS_DONE_MASK     0x0000000F  //bit3~0
#define BIT_C2H_CH3_IRQ_CNT_FRME_TRANS_DONE_NONE      0x00000000
#define BIT_C2H_CH3_IRQ_CNT_FRME_TRANS_DONE_1         0x00000001
#define BIT_C2H_CH3_IRQ_CNT_FRME_TRANS_DONE_2         0x00000002
#define BIT_C2H_CH3_IRQ_CNT_FRME_TRANS_DONE_4         0x00000004


//VDMA_C2H_CH3_FRAME_SIZE Register
#define REG_VDMA_C2H_CH3_FRAME_SIZE                   0xC20
#define VDMA_C2H_CH3_FRME_SIZE_WIDTH_MASK             0xFFFF0000  //bit31~16
#define VDMA_C2H_CH3_FRME_SIZE_HEIGHT_MASK            0x0000FFFF  //bit15~0


//VDMA_C2H_CH3_AVI_INFO Register
#define REG_VDMA_C2H_CH3_AVI_INFO                     0xC24
#define VDMA_C2H_CH3_AUD_SAMPLE_RATE_MASK             0x0F000000  //bit27~24
#define BIT_C2H_CH3_AUD_SAMPLE_RATE_32kHz             0x00000000
#define BIT_C2H_CH3_AUD_SAMPLE_RATE_32kHz             0x00000000
#define BIT_C2H_CH3_AUD_SAMPLE_RATE_44100Hz           0x01000000
#define BIT_C2H_CH3_AUD_SAMPLE_RATE_48kHz             0x02000000
#define BIT_C2H_CH3_AUD_SAMPLE_RATE_96kHz             0x03000000
#define BIT_C2H_CH3_AUD_SAMPLE_RATE_192kHz            0x04000000

#define VDMA_C2H_CH3_VIDEO_COL_DEPTH_MASK             0x000000F0  //bit7~4
#define BIT_C2H_CH3_VIDEO_COL_DEPTH_4BITS             0x00000000
#define BIT_C2H_CH3_VIDEO_COL_DEPTH_5BITS             0x00000010
#define BIT_C2H_CH3_VIDEO_COL_DEPTH_6BITS             0x00000020
#define BIT_C2H_CH3_VIDEO_COL_DEPTH_7BITS             0x00000030
#define BIT_C2H_CH3_VIDEO_COL_DEPTH_8BITS             0x00000040
#define BIT_C2H_CH3_VIDEO_COL_DEPTH_9BITS             0x00000050
#define BIT_C2H_CH3_VIDEO_COL_DEPTH_10BITS            0x00000060
#define BIT_C2H_CH3_VIDEO_COL_DEPTH_11BITS            0x00000070
#define BIT_C2H_CH3_VIDEO_COL_DEPTH_12BITS            0x00000080
#define BIT_C2H_CH3_VIDEO_COL_DEPTH_13BITS            0x00000090
#define BIT_C2H_CH3_VIDEO_COL_DEPTH_14BITS            0x000000A0
#define BIT_C2H_CH3_VIDEO_COL_DEPTH_16BITS            0x000000B0
#define BIT_C2H_CH3_VIDEO_COL_DEPTH_18BITS            0x000000C0
#define BIT_C2H_CH3_VIDEO_COL_DEPTH_20BITS            0x000000D0

#define VDMA_C2H_CH3_VIDEO_FORMAT_MASK                0x0000000F  //bit3~0
#define BIT_C2H_CH3_VIDEO_FORMAT_RGB                  0x00000000
#define BIT_C2H_CH3_VIDEO_FORMAT_YUV422               0x00000001
#define BIT_C2H_CH3_VIDEO_FORMAT_YUV444               0x00000002
#define BIT_C2H_CH3_VIDEO_FORMAT_YUV420               0x00000003
#define BIT_C2H_CH3_VIDEO_FORMAT_Y                    0x00000004
#define BIT_C2H_CH3_VIDEO_FORMAT_RAW                  0x00000005
#define BIT_C2H_CH3_VIDEO_FORMAT_RGB565               0x00000006


//RESERVED for 0xC28 ~ 0xC37
//RESERVED for 0xC40 ~ 0xC47
//RESERVED for 0xC50 ~ 0xC57


//VDMA_C2H_CH3_AUDIO_BUF_SIZE Register
#define REG_VDMA_C2H_CH3_AUDIO_BUF_SIZE               0xC58
#define VDMA_C2H_CH3_AUD_BUF_SIZE_MASK                0x00FFFFFF  //bit23~0


//VDMA_C2H_CH3_SGB_ADDR_HIGH Register 
#define REG_VDMA_C2H_CH3_SGB_ADDR_HIGH(i)             (0xC60 + i * 12)     //0x060+(0xC*i) [i=0~15]


//VDMA_C2H_CH3_SGB_ADDR_LOW Register
#define REG_VDMA_C2H_CH3_SGB_ADDR_LOW(i)              (0xC64 + i * 12)     //0x064+(0xC*i) [i=0~15]


//VDMA_C2H_CH3_SGB_SGLIST_LEN Register 
#define REG_VDMA_C2H_CH3_SGB_SGLIST_LEN(i)            (0xC68 + i * 12)     //0x068+(0xC*i) [i=0~15]


//H2C Mode SG-List Buffer Start Address High Register (1 to 16) for DMA Channel 3
#define REG_VDMA_H2C_CH3_SGB_ADDR_HIGH(i)             (0xD20 + i * 12)     //0xD20+(0xC*i) [i=0~15]


//H2C Mode SG-List Buffer Start Address Low Registers (1 to 16) for DMA Channel 3
#define REG_VDMA_H2C_CH3_SGB_ADDR_LOW(i)              (0xD24 + i * 12)     //0xD24+(0xC*i) [i=0~15]


//Defines the number of SG-Element in a Frame Buffer SG-List. (H2C Mode) (1 to 16) for DMA Channel 2
#define REG_VDMA_H2C_CH3_SGB_SGLIST_LEN(i)            (0xD28 + i * 12)     //0xD28+(0xC*i) [i=0~15]


//RESERVED for 0xDE0 ~ 0xE5F
//RESERVED for 0xE88 ~ 0xFFF


#define __REG_VDMA_C2H_CHX_SGB_ADDR_HI_SELECT(id, i)             \
({                  \
    unsigned long int __ret; \
    if(id == 0)                                      \
      __ret = REG_VDMA_C2H_CH0_SGB_ADDR_HIGH(i);             \
    else if(id == 1)                                 \
      __ret = REG_VDMA_C2H_CH1_SGB_ADDR_HIGH(i);             \
    else if(id == 2)                                 \
      __ret = REG_VDMA_C2H_CH2_SGB_ADDR_HIGH(i);             \
    else if(id == 3)                                 \
      __ret = REG_VDMA_C2H_CH3_SGB_ADDR_HIGH(i);             \
    __ret; \
 })     
		
#define __REG_VDMA_C2H_CHX_SGB_ADDR_LO_SELECT(id, i)             \
({                  \
    unsigned long int __ret; \
    if(id == 0){                                       \
       __ret = REG_VDMA_C2H_CH0_SGB_ADDR_LOW(i);             \
    }                                                  \
	else if(id == 1){                                  \
       __ret = REG_VDMA_C2H_CH1_SGB_ADDR_LOW(i);             \
    }                                                  \
    else if(id == 2){                                  \
       __ret = REG_VDMA_C2H_CH2_SGB_ADDR_LOW(i);             \
    }                                                  \
    else if(id == 3){                                  \
       __ret = REG_VDMA_C2H_CH3_SGB_ADDR_LOW(i);             \
    }                                                  \
    __ret; \
 })   
     
 #define __REG_VDMA_C2H_CHX_SGB_SGLIST_LEN_SELECT(id, i)             \
 ({                  \
    unsigned long int __ret; \
    if(id == 0){                                       \
      __ret = REG_VDMA_C2H_CH0_SGB_SGLIST_LEN(i);             \
	}                                                  \
	else if(id == 1){                                  \
      __ret = REG_VDMA_C2H_CH1_SGB_SGLIST_LEN(i);             \
	}                                                  \
    else if(id == 2){                                  \
      __ret = REG_VDMA_C2H_CH2_SGB_SGLIST_LEN(i);             \
    }                                                  \
    else if(id == 3){                                  \
      __ret = REG_VDMA_C2H_CH3_SGB_SGLIST_LEN(i);             \
    }                                                  \
    __ret; \
 })


#define __REG_VDMA_H2C_CHX_SGB_ADDR_HI_SELECT(id, i)             \
({                  \
    unsigned long int __ret; \
    if(id == 0)                                      \
      __ret = REG_VDMA_H2C_CH0_SGB_ADDR_HIGH(i);             \
    else if(id == 1)                                 \
      __ret = REG_VDMA_H2C_CH1_SGB_ADDR_HIGH(i);             \
    else if(id == 2)                                 \
      __ret = REG_VDMA_H2C_CH2_SGB_ADDR_HIGH(i);             \
    else if(id == 3)                                 \
      __ret = REG_VDMA_H2C_CH3_SGB_ADDR_HIGH(i);             \
    __ret; \
 })     
		
#define __REG_VDMA_H2C_CHX_SGB_ADDR_LO_SELECT(id, i)             \
({                  \
    unsigned long int __ret; \
    if(id == 0){                                       \
      __ret = REG_VDMA_H2C_CH0_SGB_ADDR_LOW(i);             \
    }                                                  \
	else if(id == 1){                                  \
      __ret = REG_VDMA_H2C_CH1_SGB_ADDR_LOW(i);             \
    }                                                  \
    else if(id == 2){                                  \
      __ret = REG_VDMA_H2C_CH2_SGB_ADDR_LOW(i);             \
    }                                                  \
    else if(id == 3){                                  \
      __ret = REG_VDMA_H2C_CH3_SGB_ADDR_LOW(i);             \
    }                                                  \
    __ret; \
 })   
     
 #define __REG_VDMA_H2C_CHX_SGB_SGLIST_LEN_SELECT(id, i)             \
 ({                  \
     unsigned long int __ret; \
     if(id == 0){                                       \
         __ret = REG_VDMA_H2C_CH0_SGB_SGLIST_LEN(i);             \
	 }                                                  \
	 else if(id == 1){                                  \
         __ret = REG_VDMA_H2C_CH1_SGB_SGLIST_LEN(i);             \
	 }                                                  \
     else if(id == 2){                                  \
         __ret = REG_VDMA_H2C_CH2_SGB_SGLIST_LEN(i);             \
     }                                                  \
     else if(id == 3){                                  \
         __ret = REG_VDMA_H2C_CH3_SGB_SGLIST_LEN(i);             \
     }                                                  \
     __ret; \
 })

			
//This below is used to test the reliability of the PCI driver function 
/* vdma_ctrl_regs_t struct tell the layout of the registers of pcie IP core.
 *
 * @control: start or stop the IP function @0x00
 * @status: label the status, such as busy/irq @0x04
 * @addr_low: the lower 32-bit of physical address of a desc array @0x08
 * @addr_high: the high 32-bit of physical address of a desc array @0x0C
 * @count: how many items in the desc array @0x10
 * @cpld_tag: indicate which buffer is completed, the tag is copy from pcie_dma_desc_t item which
 *				have DMA_DESC_FLAGS_TAG_VALID set. @0x14
 * */
typedef struct vdma_ctrl_regs
{
    u32 control;
    u32 status;
    u32 addr_lo;
    u32 addr_hi;
    u32 count;
    u32 cpld_tag;
} __attribute__((packed)) vdma_ctrl_regs_t;

#define DMA_SGE_FLAGS_LAST                (1 << 31)      
#define DMA_SGE_FRME_BUF_INX(i)           (i << 24)       

#endif   // #ifndef __LSC_VDMA_REGS_H__

