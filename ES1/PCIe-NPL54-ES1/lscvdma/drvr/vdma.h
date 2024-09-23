/*
 * This file is part of the Lattice Video DMA IP Core driver for Linux
 *
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#ifndef __LSC_VDMA_H__
#define __LSC_VDMA_H__

#include <linux/version.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/tty.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/cpuset.h>
#include "vdma_regs.h"
 


#define  NON_INT_METHOD			1

#define MAX_VDMA_CHANNEL_NUM 4

#ifndef MAX_PCI_BARS
#define MAX_PCI_BARS 1
#endif

#define NUM_BARS MAX_PCI_BARS
#define MAX_FB_NUM 16
#define MSI

#define NUM_BOARDS 4           /* 4 PCIe boards per system is a lot of PCIe slots and eval boards to have on hand */
#define MAX_BOARDS (NUM_BOARDS) 
#define MINORS_PER_BOARD 4     /* 3 minor number per discrete device */
#define MAX_MINORS (MAX_BOARDS * MINORS_PER_BOARD)  

#define C2H_CHANNEL_MAX 4
#define H2C_CHANNEL_MAX 4

// Address of VDMA IP Module
#define BAR0(a)  (0x00000000 | a)
#define SGDMA(i) ((BAR0(0x0000)) + i)

#define MAX_DRIVER_VERSION_LEN 128

typedef char DriverVerStr_t[MAX_DRIVER_VERSION_LEN];

#define LSCVDMA_MAGIC 'L'
#define IOCTL_LSCVDMA_START              _IOW(LSCVDMA_MAGIC, 0, struct vdma_ubuf_ioctl_arg)
#define IOCTL_LSCVDMA_GET_FRAME          _IOWR(LSCVDMA_MAGIC, 1, int)
#define IOCTL_LSCVDMA_STOP               _IO(LSCVDMA_MAGIC, 2)
#define IOCTL_LSCVDMA_GET_VERSION_INFO   _IOR(LSCVDMA_MAGIC, 3, DriverVerStr_t)
#define IOCTL_LSCVDMA_READ_8BIT          _IOR(LSCVDMA_MAGIC, 4, int) 
#define IOCTL_LSCVDMA_READ_16BIT         _IOR(LSCVDMA_MAGIC, 5, int) 
#define IOCTL_LSCVDMA_READ_32BIT         _IOR(LSCVDMA_MAGIC, 6, int) 
#define IOCTL_LSCVDMA_WRITE_8BIT         _IOW(LSCVDMA_MAGIC, 7, int) 
#define IOCTL_LSCVDMA_WRITE_16BIT        _IOW(LSCVDMA_MAGIC, 8, int) 
#define IOCTL_LSCVDMA_WRITE_32BIT        _IOW(LSCVDMA_MAGIC, 9, int)
#define IOCTL_LSCVDMA_GET_RESOURCES      _IOR(LSCVDMA_MAGIC, 10, struct PCIResourceInfo)
#define IOCTL_LSCVDMA_TX_FRAME           _IOWR(LSCVDMA_MAGIC, 11, int)


typedef struct PCI_Dev_BAR
{
    int bar;
    void *pci_addr;                     /* the physical bus address of the BAR (assigned by PCI system), used in mmap */
    void *kvm_addr;                     /* the virtual address of a BAR mapped into kernel space, used in ioremap */
    int memType;
    int dataSize;
    unsigned long len;
    unsigned long pci_start;            /* info gathered from pci_resource_*() */
    unsigned long pci_end;
    unsigned long pci_flags;
    
} pci_dev_bar_t;

//added by zhang yue on 2023-12-04 start
/**
 * Device Drvier specific information to return to user space.
 * This includes the BAR resources allocated to the device,
 * interrupts, etc.
 * NOTE: to keep backwards compatibility with older applications
 * that were built against v1.0.0.x drivers, do no add any new
 * fields to this structure or the driver will crash during
 * copying info back to user pages because the size will be different
 * and a fault will occur.  Add new fields to the extra info struct.
 */
/**
 * Information about a device's BAR.
 */
typedef struct
{
    unsigned long nBAR;
    unsigned long physStartAddr;
    unsigned long size;
    bool memMapped;
    unsigned short flags;
    uint8_t type;

} PCI_BAR_t;

typedef struct PCIResourceInfo
{
    // Device Memory Access info
    unsigned long numBARs;
    PCI_BAR_t BAR[MAX_PCI_BARS];

    unsigned char PCICfgReg[256];

    // Device Interrupt Info
    bool hasInterrupt;
    unsigned long intrVector;

} PCIResourceInfo_t;


//added by zhang yue on 2023-12-04 end

// IOCTL Operations.
typedef struct vdma_ubuf_ioctl_arg 
{
    void **addr;
    size_t len;   /* per buf must have the same size*/
    u8 buf_cnt;
    u32 dir;
}vdma_ubuf_ioctl_t;

typedef struct vdma_rw_ioctl_arg
{
    u32 reg;
    u32 value;
}vdma_rw_ioctl_t;

/**
 * the vdma_desc_hw struct is used to represent a piece of user buffer since the memory of
 * buffer is not continuous in phyical address. all memory in one piece must be continuous.
 * you need to allocate an array to store all the piece. it need to pay attention to that
 * the memory of the array should be coherent, or else the cache problem will mess you. how
 * many array depend on how many buffer you have ,like the following diagram.
 * _____________________________     ____________________________
 * |    |    |     |    |    	 |    |    |    |     |    |      |
 * | p1 | p2 | ... | pn |p(n+1) |    | p1 | p2 | ... | pn |p(n+1)|
 * |____|____|_____|____|_______|    |____|____|_____|____|______|
 *
 * |<--- buf0 desc array ----->|     |<---buf1 desc array  ---->|
 *
 * */
typedef struct vdma_desc_hw 
{
    u32 addr_hi;        /* the higher 32-bit of physical address of a piece memory */
    u32 addr_lo;        /* the lower 32-bit of physical address of a piece memory */
    u32 flags_len;      /* the length of the piece memory and flags to lable the properies */
    u32 rsvd;           /* reserved for use in future */
} __attribute__((packed)) vdma_desc_hw_t;

struct ubuf_info 
{
    int offset;
    size_t size;
    int nr_pages;
    struct page **pages;
};

typedef struct lscvdma_kthread 
{
    spinlock_t lock;
    char name[10];

    unsigned short id;
    unsigned short timeout;
	
    wait_queue_head_t waitq;
    unsigned int schedule;
    struct task_struct *task;
    unsigned int work_cnt;

    int (*fproc)(void * complthp);
	
    struct vdma_channel * v_channel;

    uint32_t cnt;
} lscvdma_kthread_t;

/**
 * SGDMA channel information for keeping track of what's being transferred
 */
typedef struct vdma_channel
{	
    struct vdma_dev *vdev;
    u8 id;                                          /* channel ID */
    spinlock_t lock;                                /* spinlock to provide mutex protection when modifying SGDMA and IntCtrl regs */
    wait_queue_head_t wq;                           /* park user's read() request here while DMA'ing */
    u32 fb_idx;
    u32 wb_idx;
    dma_addr_t non_int_bus;
    volatile uint32_t * non_int_vir; 	
	
    bool fb_idx_update;
	
    u32 wb_idx_update;
	
    u8 open_flag;                                   /* open flag to ensure only 1 user opens one channel at a time */
    u8 sge_mode;
    u32 frm_stats;
    
    struct ubuf_info *ubuf_info[MAX_FB_NUM];  
    struct sg_table *ubuf_sg_table[MAX_FB_NUM];
    u16 ubuf_cnt;
    int sge_cnts[MAX_FB_NUM];  						
    
    vdma_desc_hw_t *vdma_desc[MAX_FB_NUM];
    dma_addr_t vdma_desc_handle[MAX_FB_NUM];
    enum dma_data_direction dir;
   
} vdma_channel_t;

typedef struct vdma_dev
{
    // Driver and Device Information
    u32 ID;                             /* PCI device ID of the board */
    u32 demoID;                         /* PCI subsys device ID of the board */
    struct pci_dev *pdev;
    const char *name; 
    u8 c2h_channel_max;
    u8 h2c_channel_max;
    bool isDmaAddr64;                   /* set to true if asked for 64 bit address support */
    u8 c2h_channel_open_cnt;
    u8 h2c_channel_open_cnt;

    // BAR Assignments
    pci_dev_bar_t Dev_BARs[NUM_BARS];   /* database of valid, mapped BARs belonging to this board */
    u8 numBars;                         /* number of valid BARs this board has, used to limit access into Dev_BARs[] */
    u8 mmapBAR;                         /* which BAR is used for mmap into user space.  Can change with IOCTL call */
    u8 ctrlBAR;                         /* which BAR is used for control access */
    void *ctrlBARaddr;	                 /* above BAR memory ioremap'ed into driver space to access */
    
    // Interrupt Management
    u32 irq_line;                       /* -1 if no legacy interrupt support, otherwise the interrupt line/vector */
    bool msi;                   /* true if MSI interrupts have been enabled, false if regular INTx */
    u32 msi_irq_vec[32];                
    u32 num_msi_irqs;    

    bool irq_trigger_ch_c2h[MAX_VDMA_CHANNEL_NUM];
    bool irq_trigger_ch_h2c[MAX_VDMA_CHANNEL_NUM];

    vdma_channel_t vdma_chan_c2h[MAX_VDMA_CHANNEL_NUM];
    vdma_channel_t vdma_chan_h2c[MAX_VDMA_CHANNEL_NUM];
    struct work_struct c2h_work;
    struct work_struct h2c_work;

    //Added by zhang yue on 2024-04-07 start
    lscvdma_kthread_t *fetchthp[MAX_VDMA_CHANNEL_NUM];
    //Added by zhang yue on 2024-04-07 end
    lscvdma_kthread_t *cmplthp[MAX_VDMA_CHANNEL_NUM];

} vdma_dev_t;

enum cdev_type{
    CHAR_VDMA_H2C,
    CHAR_VDMA_C2H,
};

typedef struct vdma_cdev
{
    struct vdma_pci_dev *vpdev;
    struct vdma_dev *vdev;
    
    dev_t cdevno;                     /* starting [MAJOR][MINOR] device number for this driver */
    struct cdev cdev;
    
    vdma_channel_t *channel;
    struct device *sys_dev;
    spinlock_t lock;
    
} vdma_cdev_t;


struct vdma_pci_dev
{
    struct pci_dev *pdev;                               /* pointer to the PCI core representation of the board */
    struct vdma_dev *vdev;

    u32 majorNum;                                       /* copy of driver's Major number for use in places where only device exists */                        
    u32 minorNum;					  /* copy of driver's First Minor number for use in places where only device exists */ 					
    u8 user_irq_max;
    u8 c2h_channel_max;
    u8 h2c_channel_max;
    
    vdma_cdev_t vdma_c2h_dev[MAX_VDMA_CHANNEL_NUM];
    vdma_cdev_t vdma_h2c_dev[MAX_VDMA_CHANNEL_NUM];
  
}vdma_pci_dev_t;

#endif // #ifndef __LSC_VDMA_H__

