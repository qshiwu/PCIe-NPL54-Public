/**
 * This file is part of the Lattice Video DMA IP Core driver for Linux
 *
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */


#include <linux/fs.h>  
#include <linux/uaccess.h>  
#include <linux/writeback.h> 
#include <linux/stringify.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/aer.h>
#include "version.h"
#include "vdma.h"

#define WRITE_SG_LIST_INFO_TO_FILE				(1)



#if (WRITE_SG_LIST_INFO_TO_FILE)

#define FILE_PATH "/var/log/vdma_sg_list_msg.txt" 

static struct file *log_file = NULL;  
void printk_file(const char *fmt, ...) ; 

#endif






#ifdef pr_fmt     
#undef pr_fmt
#define pr_fmt(fmt)     KBUILD_MODNAME " %s: " fmt, __func__
#endif

/* Variables that can be overriden from module command line */
static int vdma_debug = 1;
module_param(vdma_debug, int, 0);
MODULE_PARM_DESC(vdma_debug, "vdma enable debugging (0-1)");

/* Variables that used for polling mode */
static unsigned int poll_mode = 0;
module_param(poll_mode, uint, 0644);
MODULE_PARM_DESC(poll_mode, "Set 1 for polling mode, default is 0 for interrupt mode");

static struct class *glscvdma_class;

/* total number of boards controlled by driver */
static u8 g_boards_num = 0;                                       

/* List of boards we will attempt to find and associate with the driver */
static struct pci_device_id lscvdma_pci_id_tbl[] = 
{
    { 0x1204, 0x0001, 0x1204, 0x0001, },   /* LSC VDMA */
    { }			                 
};

#define __wait_event_lock_timeout(wq_head, condition, lock, timeout, state) \
        ___wait_event(wq_head, ___wait_cond_timeout(condition),             \
                      state, 0, timeout,                                    \
                      spin_unlock_irq(&lock);                               \
                      __ret = schedule_timeout(__ret);                      \
                      spin_lock_irq(&lock));

#define wait_event_lock_timeout(wq_head, condition, lock, timeout)          \
({                                                                          \
    long __ret = timeout;                                                   \
    if (!___wait_cond_timeout(condition))                                   \
        __ret = __wait_event_lock_timeout(                                  \
                    wq_head, condition, lock, timeout,                      \
                    TASK_UNINTERRUPTIBLE);                                  \
    __ret;                                                                  \
})

/**
 * Read a 8 bit hardware register and return the word.
 * 
 * @param vdev pointer to the Board to get hardware BAR address
 * @param offset in board's mapped control BAR to read from
 * @return 8 bit value read from the hardware register
 */
static u8 rdReg8(vdma_dev_t *vdev, unsigned long offset)
{
    return(readb(vdev->ctrlBARaddr + offset));
}

/**
 * Read a 16 bit hardware register and return the word.
 * 
 * @param vdev pointer to the Board to get hardware BAR address
 * @param offset in board's mapped control BAR to read from
 * @return 16 bit value read from the hardware register
 */
static u16 rdReg16(vdma_dev_t *vdev, unsigned long offset)
{
    return(readw(vdev->ctrlBARaddr + offset));
}

/**
 * Read a 32 bit hardware register and return the word.
 * 
 * @param vdev pointer to the Board to get hardware BAR address
 * @param offset in board's mapped control BAR to read from
 * @return 32 bit value read from the hardware register
 */
static u32 rdReg32(vdma_dev_t *vdev, unsigned long offset)
{
    return(readl(vdev->ctrlBARaddr + offset));
}

/**
 * Write a 8 bit value to a hardware register.
 * 
 * @param vdev pointer to Board to get hardware BAR address
 * @param offset in the board's mapped control BAR to write to
 * @param val the 8 bit value to write
 */
static void wrReg8(vdma_dev_t *vdev, unsigned long offset, u8 val)
{
    writeb(val, vdev->ctrlBARaddr + offset);
}

/**
 * Write a 16 bit value to a hardware register.
 * 
 * @param vdev pointer to Board to get hardware BAR address
 * @param offset in the board's mapped control BAR to write to
 * @param val the 16 bit value to write
 */
static void wrReg16(vdma_dev_t *vdev, unsigned long offset, u16 val)
{
    writew(val, vdev->ctrlBARaddr + offset);
}

/**
 * Write a 32 bit value to a hardware register.
 * 
 * @param vdev pointer to Board to get hardware BAR address
 * @param offset in the board's mapped control BAR to write to
 * @param val the long value to write
 */
static void wrReg32(vdma_dev_t *vdev, unsigned long offset, unsigned long val)
{
    writel(val, vdev->ctrlBARaddr + offset);
    wmb();
}

static void wrReg32Modify(vdma_dev_t *vdev, unsigned long offset, unsigned long mask,  unsigned long val)
{
    uint32_t result;
    
    result = rdReg32(vdev, offset);
    result &= (~mask);
    result |= (mask & val);
    mdelay(5);
    wrReg32(vdev, offset, result);
}

#if (WRITE_SG_LIST_INFO_TO_FILE)

void printk_file(const char *fmt, ...)  
{  
    va_list args;  
    char kernel_buf[356]; //Max length of log is 356. 
    char *user_buf = NULL;  
    int len;  
    //mm_segment_t old_fs;  
    static loff_t pos = 0;

    //log_file = filp_open(FILE_PATH, O_WRONLY | O_CREAT | O_APPEND, 0666);  
    if (!log_file || IS_ERR(log_file) || !(log_file->f_mode & FMODE_CAN_WRITE)) {  
        printk(KERN_ERR "Invalid file pointer or write operation not supported\n");  
        return;  
    } 

     
    //printk(KERN_ERR "log_file check OK, not NULL, no error\n");  
    va_start(args, fmt);  
    len = vsnprintf(kernel_buf, sizeof(kernel_buf), fmt, args);  
    va_end(args);  
  
    //check if format string contains user space pointer  
    if (strstr(fmt, "%s")) {  
        user_buf = va_arg(args, char *);  
  
        //temply change address space to access user space memory  
        //old_fs = get_fs();  
        //set_fs(KERNEL_DS);  
  
        //copy user space string to kernel space 
        if (user_buf && !copy_from_user(kernel_buf, user_buf, strlen(user_buf) + 1)) {  
            // write kernel space strings to file  
            //if(NULL != log_file->f_op && NULL != log_file->f_op->write)	
            //    log_file->f_op->write(log_file, kernel_buf, strlen(kernel_buf), &log_file->f_pos);
            //vfs_write(log_file, kernel_buf, strlen(kernel_buf), &log_file->f_pos); 
            kernel_write(log_file, kernel_buf, strlen(kernel_buf) + 1, &pos);			
        } else {  
            printk(KERN_ERR "Failed to copy string from user space\n");  
        }  
  
    } else {  
        // no user space pointer，write to file directly  
        kernel_write(log_file, kernel_buf, strlen(kernel_buf), &pos);
    }  

    #if 0    
     if (log_file != NULL && !IS_ERR(log_file)) {  
        filp_close(log_file, NULL);  
        log_file = NULL;  
     }
     #endif 
}
#endif


static void put_ubuf_pages(struct ubuf_info *info)
{
    int i;
    //down_read(&current->mm->mmap_sem);
    for (i = 0; i < info->nr_pages; i++){
        set_page_dirty(info->pages[i]);
        put_page(info->pages[i]);
    }
    kfree(info->pages);
    //up_read(&current->mm->mmap_sem);
}

static int get_ubuf_pages(struct ubuf_info *info, unsigned long uaddr, unsigned long size)
{
    unsigned long first = (uaddr & PAGE_MASK) >> PAGE_SHIFT;
    unsigned long last = ((uaddr+size-1) & PAGE_MASK) >> PAGE_SHIFT;
    unsigned int flags = FOLL_FORCE | FOLL_WRITE;
    int ret, i;

    info->offset = uaddr & ~PAGE_MASK;
    info->nr_pages = last-first+1;
    info->size = size;
    info->pages = kmalloc(info->nr_pages * sizeof(struct page *), GFP_KERNEL);
    if (!info->pages)
        return -ENOMEM;

    pr_info("%#lx + %ld => %d pages\n", uaddr, size, info->nr_pages);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
    ret = get_user_pages(current, current->mm, uaddr & PAGE_MASK, info->nr_pages, flags & FOLL_WRITE, flags & FOLL_FORCE, info->pages, NULL);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(4, 4, 168)
    ret = get_user_pages_unlocked(current, current->mm, uaddr & PAGE_MASK, info->nr_pages, flags & FOLL_WRITE, flags & FOLL_FORCE, info->pages);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
    ret = get_user_pages_unlocked(current, current->mm, uaddr & PAGE_MASK, info->nr_pages, info->pages, flags);
#else
    ret = get_user_pages_unlocked(uaddr & PAGE_MASK, info->nr_pages, info->pages, flags);
#endif
    if (ret != info->nr_pages)
        goto fail_map;

    return 0;

fail_map:
    pr_err("get_user_pages fail: ret=%d [%d]\n", ret, info->nr_pages);
    if (ret > 0) {
        for (i = 0; i < ret; i++)
            put_page(info->pages[i]);
        ret = -EINVAL;
    }
    
    kfree(info->pages);
	
    return ret;
}

static void ubuf_sg_free(vdma_channel_t *vchannel)
{
    int i;
    struct sg_table *table;
    struct ubuf_info *info;

    for (i = 0; i < MAX_FB_NUM; i++)
    {
        table = vchannel->ubuf_sg_table[i];
        info = vchannel->ubuf_info[i];
        if (table) {
            sg_free_table(table);
            kfree(table);
            vchannel->ubuf_sg_table[i] = NULL;
        }

        if (info) {
            put_ubuf_pages(info);
            kfree(info);
            vchannel->ubuf_info[i] = NULL;
        }
    }
}

static int vdma_ubuf_unmap(vdma_channel_t *vchannel)
{
    int i;
    struct sg_table *table;
    struct vdma_dev *vdev;
    
    vdev = vchannel->vdev;
    if(!vdev){
        pr_err("vdev NULL vdma_ubuf_unmap!\n");
        return -EINVAL;
    }
    
    pr_info("enter\n");
    for (i = 0; i < MAX_FB_NUM; i++) {         
        table = vchannel->ubuf_sg_table[i];
        if(vchannel->sge_cnts[i]){	
            dma_unmap_sg(&vdev->pdev->dev, table->sgl, table->nents, vchannel->dir);
        }
        else{
            break;
        }
    }

    ubuf_sg_free(vchannel);
    
    return 0;
}

static int ubuf_sg_alloc(vdma_channel_t *vchannel, int idx, void *addr, size_t len)
{
    unsigned long uaddr = (unsigned long)addr;
    struct ubuf_info *info;
    struct sg_table *table;
    int err;

    info = kmalloc(sizeof(*info), GFP_KERNEL);
    if (!info) {
        return -ENOMEM;
    }

    err = get_ubuf_pages(info, uaddr, len);
    if (err)
        goto fail_init_ubuf;

    table = kmalloc(sizeof(*table), GFP_KERNEL);
    if (!table) {
        err = -ENOMEM;
        goto fail_table;
    }

    err = sg_alloc_table_from_pages(table, info->pages, info->nr_pages, info->offset, info->size, GFP_KERNEL);
    if (err)
        goto fail_sg;

    vchannel->ubuf_sg_table[idx] = table;
    vchannel->ubuf_info[idx] = (void *)info;

    return 0;

fail_sg:
    kfree(table);
fail_table:
    put_ubuf_pages(info);
fail_init_ubuf:
    kfree(info);
  
    return err;
}

static int vdma_ubuf_map(vdma_channel_t *vchannel, unsigned int cmd, unsigned long arg)
{
    u8 i;
    //u32 dir;
    vdma_ubuf_ioctl_t data;
    void *uaddr[MAX_FB_NUM];
    struct sg_table *table;
    struct vdma_dev *vdev;
  
    if(!vchannel){
        pr_err("vdma_ubuf_map: vchannel NULL!\n");
        return -EINVAL;
    }
    vdev = vchannel->vdev;
    if(!vdev){
        pr_err("vdma_ubuf_map: vdev NULL!\n");
        return -EINVAL;
    }
    
    if (vchannel->ubuf_cnt > 0){ /* already set */
        pr_err("vdma_ubuf_map: channle buf already set!\n"); 
        return -EALREADY;
    }
    
    if (copy_from_user(&data, (void __user *)arg, _IOC_SIZE(cmd)))
        return -EFAULT;
        
    if (data.buf_cnt > MAX_FB_NUM) {
        pr_warn("Warning:too many bufs, only use %d bufs\n", MAX_FB_NUM);
        data.buf_cnt = MAX_FB_NUM;
    }
    
    if (copy_from_user(uaddr, (void __user *)data.addr, data.buf_cnt * sizeof(void*)))
        return -EFAULT;
        
    vchannel->dir = data.dir;
    pr_info("DMA channel dir: %d \n", data.dir);
    
    for (i = 0; i < data.buf_cnt; i++){
        if (ubuf_sg_alloc(vchannel, i, uaddr[i], data.len)) {
            ubuf_sg_free(vchannel);
            return -ENOMEM;
        }

        table = vchannel->ubuf_sg_table[i];

        vchannel->sge_cnts[i] = dma_map_sg(&vdev->pdev->dev, table->sgl, table->nents, vchannel->dir);
        if(!vchannel->sge_cnts[i])
        {
            vdma_ubuf_unmap(vchannel);
            return (-ENOMEM);
        }
    }
    pr_info("channel%d dir:%d user buffer map successful, ubuf_cnt:%d \n",vchannel->id, vchannel->dir, data.buf_cnt);
    vchannel->ubuf_cnt = data.buf_cnt;
    
    return 0;
}

static int vdma_desc_free(vdma_channel_t *vchannel)
{
    u8 i;
    vdma_desc_hw_t *vdma_desc;
    //struct sg_table *table;
    vdma_dev_t *vdev;
    
    vdev = vchannel->vdev;
    if(!vdev){
        pr_err("vdma_desc_free: vdev NULL!\n");
        return -EINVAL;
    }
    
    pr_info("free vdma desc \n");
    for (i = 0; i < MAX_FB_NUM; i++) {      
        vdma_desc = vchannel->vdma_desc[i];
		
        if(NULL != vdma_desc){
            pr_info("dma_free_coherent \n");
            dma_free_coherent(&vdev->pdev->dev, (vchannel->sge_cnts[i]) * sizeof(vdma_desc_hw_t), 
                vdma_desc, vchannel->vdma_desc_handle[i]);
        }
        vchannel->vdma_desc[i] = NULL;
        vchannel->sge_cnts[i] = 0;
    }
    
    return 0;
}

static int vdma_tx_frame(vdma_channel_t *vchannel, unsigned int cmd, unsigned long arg)
{
    int ret;
    int timeout;
    int cur_fb_idx = -1;

    vdma_dev_t * vdev = NULL;
    lscvdma_kthread_t *thp = NULL;
    
    if(!vchannel){
        pr_err("vdma_tx_frame: vchannel NULL!\n");
        return -EINVAL;
    }
    
    if (vchannel->ubuf_cnt == 0) /* not init */
        return -ENOBUFS;
        
    if (copy_from_user(&timeout, (void __user *)arg, _IOC_SIZE(cmd)))
        return -EFAULT;
      
    spin_lock_irq(&vchannel->lock);

    vdev = vchannel->vdev;

    //released by zhang yue on 2024-04-07 start
    thp = vdev->fetchthp[vchannel->id];
    if(poll_mode){
        thp->schedule = 1;
        //pr_err("trigger h2c poll thread!\n");
        wake_up(&(thp->waitq));
        ret = wait_event_lock_timeout(vchannel->wq ,  
            vchannel->wb_idx_update,
            vchannel->lock,
            msecs_to_jiffies(timeout));
        if (0 == ret) {
            spin_unlock_irq(&vchannel->lock);
            //pr_info("H2C wait_event_lock_timeout \n");  //remove unnessary debug message.
            return -ETIME;
        }
        spin_lock(&(vdev->fetchthp[vchannel->id]->lock));
        cur_fb_idx = vchannel->wb_idx;
        vchannel->wb_idx_update = 0;
        spin_unlock(&(vdev->fetchthp[vchannel->id]->lock));
    }else { //released by zhang yue on 2024-04-07 end
        ret = wait_event_lock_timeout(vchannel->wq,  
                vchannel->fb_idx_update,
                vchannel->lock,
                msecs_to_jiffies(timeout));
        if (0 == ret) {
            ret = rdReg32(vchannel->vdev, SGDMA(REG_VDMA_H2C_CR));
            pr_info("channel:%d get frame timeout, read H2C CR reg:0x%x.", vchannel->id, ret);
            
            ret = rdReg32(vchannel->vdev, SGDMA(REG_VDMA_H2C_SR));
            pr_info("read H2C SR reg:0x%x.", ret);
            
            ret = rdReg32(vchannel->vdev, SGDMA(REG_VDMA_H2C_IRQ_SRC));
            pr_info("read REG_VDMA_H2C_IRQ_SRC:0x%x.", ret);
            spin_unlock_irq(&vchannel->lock);
            return -ETIME;
        }
        vchannel->fb_idx_update = 0;
        cur_fb_idx = vchannel->fb_idx;   
        pr_info("h2c cur_fb_idx:%d \n", cur_fb_idx);
    }   
	
   spin_unlock_irq(&vchannel->lock);

   if (copy_to_user((void __user *)arg, &cur_fb_idx, _IOC_SIZE(cmd)))
      return -EFAULT;
      
   return 0;
}

static int vdma_recv_frame(vdma_channel_t *vchannel, unsigned int cmd, unsigned long arg)
{
    int ret;
    int timeout;
    int cur_fb_idx = -1;

    vdma_dev_t * vdev = NULL;
    lscvdma_kthread_t *thp = NULL;
    
    if(!vchannel){
        pr_err("vdma_recv_frame: vchannel NULL!\n");
        return -EINVAL;
    }
    
    if (vchannel->ubuf_cnt == 0) /* not init */
        return -ENOBUFS;
        
    if (copy_from_user(&timeout, (void __user *)arg, _IOC_SIZE(cmd)))
        return -EFAULT;
      
    spin_lock_irq(&vchannel->lock);

    vdev = vchannel->vdev;
    thp = vdev->cmplthp[vchannel->id];
    if(poll_mode){
        thp->schedule = 1;
        //pr_err("trigger poll thread!\n");
        wake_up(&(thp->waitq));
        ret = wait_event_lock_timeout(vchannel->wq ,  
            vchannel->wb_idx_update,
            vchannel->lock,
            msecs_to_jiffies(timeout));
        if (0 == ret) {
            spin_unlock_irq(&vchannel->lock);
            //pr_info("ret, wait_event_lock_timeout \n");  //remove unnessary debug message.
            return -ETIME;
        }
        spin_lock(&(vdev->cmplthp[vchannel->id]->lock));
        cur_fb_idx = vchannel->wb_idx;
        vchannel->wb_idx_update = 0;
        spin_unlock(&(vdev->cmplthp[vchannel->id]->lock));
    }else{
        ret = wait_event_lock_timeout(vchannel->wq,  
                vchannel->fb_idx_update,
                vchannel->lock,
                msecs_to_jiffies(timeout));
        if (0 == ret) {
            ret = rdReg32(vchannel->vdev, SGDMA(REG_VDMA_C2H_CR));
            //pr_info("channel:%d get frame timeout, read CR reg:0x%x.", vchannel->id, ret);
            
            ret = rdReg32(vchannel->vdev, SGDMA(REG_VDMA_C2H_SR));
            //pr_info("read SR reg:0x%x.", ret);
            
            ret = rdReg32(vchannel->vdev, SGDMA(REG_VDMA_C2H_IRQ_SRC));
            //pr_info("read REG_VDMA_C2H_IRQ_SRC:0x%x.", ret);
            spin_unlock_irq(&vchannel->lock);
            return -ETIME;
        }
        vchannel->fb_idx_update = 0;
        cur_fb_idx = vchannel->fb_idx;   
    }   
	
   spin_unlock_irq(&vchannel->lock);

   if (copy_to_user((void __user *)arg, &cur_fb_idx, _IOC_SIZE(cmd)))
      return -EFAULT;
      
   return 0;
}

//added by zhang yue on 2023-12-04 start
static int vdma_get_resource(vdma_channel_t *vchannel, unsigned int cmd, unsigned long arg)
{
    int i = 0;
    int status = 0;
    PCIResourceInfo_t *pInfo;
    vdma_dev_t * vdev = NULL;

    if(!vchannel){
        pr_err("vdma_recv_frame: vchannel NULL!\n");
        return -EINVAL;
    }

    //deleted by Zhang Yue on 2023-12-10 start	
    //if (vchannel->ubuf_cnt == 0) /* not init */
    //    return -ENOBUFS;
    //deleted by Zhang Yue on 2023-12-10 end
    	
    vdev = vchannel->vdev;
	
    #if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0))
    if (!access_ok((void *)arg, _IOC_SIZE(cmd)))
    #else
    if (!access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd)))
    #endif
    {
        status = -EFAULT;
        pr_err("the access right not correct\n");
	return status;
    }

     pInfo = kmalloc(sizeof(PCIResourceInfo_t), GFP_KERNEL);
     if (pInfo == NULL)
     {
         status = -EFAULT;
         pr_err("allocate memory for pInfo failed\n");
         return status;	// abort
     }

    if(vdev->num_msi_irqs > 0)
        pInfo->hasInterrupt = true;
    else
        pInfo->hasInterrupt = false;
    pInfo->intrVector =vdev->msi_irq_vec[0];
    pInfo->numBARs = vdev->numBars;
    for (i = 0; i < MAX_PCI_BARS; i++)
    {
        pInfo->BAR[i].nBAR = vdev->Dev_BARs[i].bar;    //pBrd->Dev_BARs[i].bar;
        pInfo->BAR[i].physStartAddr = (unsigned long) vdev->Dev_BARs[i].pci_addr    ;//pBrd->Dev_BARs[i].pci_addr;
        pInfo->BAR[i].size = vdev->Dev_BARs[i].len     ;//pBrd->Dev_BARs[i].len;
        pInfo->BAR[i].memMapped =  (vdev->Dev_BARs[i].kvm_addr) ? 1 : 0;  //(pBrd->Dev_BARs[i].kvm_addr) ? 1 : 0;
        pInfo->BAR[i].flags = (unsigned short) (vdev->Dev_BARs[i].pci_flags)    ;// (pBrd->Dev_BARs[i].pci_flags);
        pInfo->BAR[i].type = (unsigned char) ((vdev->Dev_BARs[i].memType)>>8)   ;//((pBrd->Dev_BARs[i].memType)>>8);    // get the bits that show IO or mem
    }

    spin_lock_irq(&vchannel->lock);
    for (i = 0; i < 0x100; ++i)
    {
        pci_read_config_byte(vdev->pdev, i, &(pInfo->PCICfgReg[i]));
    }
    
    spin_unlock_irq(&vchannel->lock);
	
    if (copy_to_user((void *)arg, (void *)pInfo, sizeof(PCIResourceInfo_t)) != 0){
        pr_err("copy memory to arg failed\n");
        status = -EFAULT; // Not all bytes were copied so this is an error
    }
    kfree(pInfo);  // release kernel temp buffer
          
    return status;
}
//added by zhang yue on 2023-12-04 end


static int vdma_stop_transfer(vdma_channel_t *vchannel)
{
    vdma_dev_t *vdev;
    
    pr_info("DMA stop \n");
    if(!vchannel){
        pr_err("vdma_stop_transfer: vchannel NULL!\n");
        return -EINVAL;
    }
    vdev = vchannel->vdev;
    if(!vdev){
        pr_err("vdma_stop_transfer: vdev NULL!\n");
        return -EINVAL;
    } 
    if(!vchannel->ubuf_cnt) {/* not init */
        return -ENOBUFS;
    }

    if(poll_mode){
        if(vchannel->dir == DMA_FROM_DEVICE){
            vdev->cmplthp[vchannel->id]->work_cnt = 0;  
        }else if( vchannel->dir == DMA_TO_DEVICE){
            vdev->fetchthp[vchannel->id]->work_cnt = 0;
        }
    }

    	
    vchannel->ubuf_cnt = 0;
    if (vchannel->dir == DMA_FROM_DEVICE){
        vdev->c2h_channel_open_cnt--;         
        pr_info("c2h_channel_open_cnt:%d \n", vdev->c2h_channel_open_cnt);   
	
        // Stop DMA channel
        if(vchannel->id == 0)
            wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_DMA_CHAN0_MASK, BIT_C2H_CR_DMA_CHAN0_STOP);
        else if(vchannel->id == 1)
            wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_DMA_CHAN1_MASK, BIT_C2H_CR_DMA_CHAN1_STOP);
        else if(vchannel->id == 2)
            wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_DMA_CHAN2_MASK, BIT_C2H_CR_DMA_CHAN2_STOP);
        else if(vchannel->id == 3)
            wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_DMA_CHAN3_MASK, BIT_C2H_CR_DMA_CHAN3_STOP);
    
        if (vdev->c2h_channel_open_cnt == 0){
            // Disable interrupt
            wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_IRQ_MASK), VDMA_C2H_IRQ_FRME_TRANS_DONE_MASK, BIT_C2H_IRQ_FRME_TRANS_DONE_DIS);

            // Stop DMA engine
            wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_DMA_EN_MASK, BIT_C2H_CR_DMA_DIS);
        }
        // Clear IRQ
        wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_IRQ_SRC), VDMA_C2H_IRQ_SRC_FRME_TRANS_DONE_MASK, BIT_C2H_IRQ_SRC_FRME_TRANS_DONE_CLR);
    }
    else if (vchannel->dir == DMA_TO_DEVICE){
        vdev->h2c_channel_open_cnt--;         
        pr_info("h2c_channel_open_cnt:%d \n", vdev->h2c_channel_open_cnt);   
	
        // Stop DMA channel
        if(vchannel->id == 0)
            wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_CR), VDMA_H2C_CR_DMA_CHAN0_MASK, BIT_H2C_CR_DMA_CHAN0_STOP);
        else if(vchannel->id == 1)
            wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_CR), VDMA_H2C_CR_DMA_CHAN1_MASK, BIT_H2C_CR_DMA_CHAN1_STOP);
        else if(vchannel->id == 2)
            wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_CR), VDMA_H2C_CR_DMA_CHAN2_MASK, BIT_H2C_CR_DMA_CHAN2_STOP);
        else if(vchannel->id == 3)
            wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_CR), VDMA_H2C_CR_DMA_CHAN3_MASK, BIT_H2C_CR_DMA_CHAN3_STOP);
    
        if (vdev->h2c_channel_open_cnt == 0){
            // Disable interrupt
            wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_IRQ_MASK), VDMA_H2C_IRQ_FRME_TRANS_DONE_MASK, BIT_H2C_IRQ_FRME_TRANS_DONE_DIS);

            // Stop DMA engine
            wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_CR), VDMA_H2C_CR_DMA_EN_MASK, BIT_H2C_CR_DMA_DIS);
        }
        // Clear IRQ
        wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_IRQ_SRC), VDMA_H2C_IRQ_SRC_FRME_TRANS_DONE_MASK, BIT_H2C_IRQ_SRC_FRME_TRANS_DONE_CLR);
    }


    //added by zhang yue on 2024-04-08 start  
    if(poll_mode){
         if(NULL != vchannel->non_int_vir)
            dma_free_coherent(&vdev->pdev->dev, sizeof(uint32_t), (void *)vchannel->non_int_vir, vchannel->non_int_bus);
    }
    //added by zhang yue on 2024-04-08 end 
    
    return 0;
}

static int sgb_rdy_check(vdma_channel_t *vchannel, bool *flag)
{
    u32 value;
    vdma_dev_t *vdev;
    
    if(!vchannel){
        pr_err("sgb_rdy_check: vchannel NULL!\n");
        return -EINVAL;
    }
    vdev = vchannel->vdev;
    if(!vdev){
        pr_err("sgb_rdy_check: vdev NULL!\n");
        return -EINVAL;
    } 
    
    *flag = 0;
    value = rdReg32(vdev, SGDMA(REG_VDMA_C2H_SR));
    if((vchannel->id == 0) && (value & VDMA_C2H_SR_CH0_SG_BUF_RDY_MASK)){
        pr_info("channel0 sglist buffer rdy. \n");
        *flag = 1;
    }
    else if((vchannel->id == 1) && (value & VDMA_C2H_SR_CH1_SG_BUF_RDY_MASK)){
        pr_info("channel1 sglist buffer rdy. \n");
        *flag = 1;
    }
    else if((vchannel->id == 2) && (value & VDMA_C2H_SR_CH2_SG_BUF_RDY_MASK)){
        pr_info("channel2 sglist buffer rdy. \n");
        *flag = 1;
    }
    else if((vchannel->id == 3) && (value & VDMA_C2H_SR_CH3_SG_BUF_RDY_MASK)){
        pr_info("channel3 sglist buffer rdy. \n");
        *flag = 1;
    }
    
    return 0;
}

static int vdma_start_transfer(vdma_channel_t *vchannel)
{
    u8 mode;
    bool flag;
    int err;
    vdma_dev_t *vdev;
    
    if(!vchannel){
        pr_err("vdma_start_transfer: vchannel NULL!\n");
        return -EINVAL;
    }
    vdev = vchannel->vdev;
    if(!vdev){
        pr_err("vdma_start_transfer: vdev NULL!\n");
        return -EINVAL;
    } 
                 
    mode = vchannel->sge_mode;
    if (mode == 1) {
        err = sgb_rdy_check(vchannel, &flag);
        if (!err && flag) {
            //Start DMA engine
            pr_info(" start DMA engine with mode 1. \n");
            wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_DMA_EN_MASK, BIT_C2H_CR_DMA_EN);
        }
        else{
            pr_err("not rdy or sgb_rdy_check failed! flag=%d \n",flag);
            return -EINVAL;
        } 
    }
    else {
        //Start DMA engine
        if (vchannel->dir == DMA_FROM_DEVICE){
            //Modified by zhang yue on 2023-11-28 start 
            if(vdev->c2h_channel_open_cnt == 0){
                wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_DMA_EN_MASK, BIT_C2H_CR_DMA_EN);
                wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_INT_METHOD_MASK, (poll_mode ? BIT_C2H_CR_INT_METHOD_POLL : BIT_C2H_CR_INT_METHOD_MSI ));
            }
            //Modified by zhang yue on 2023-11-28 end
        
            vdev->c2h_channel_open_cnt++;
            pr_info(" start DMA engine with mode 0, DMA engine cnt:%d \n", vdev->c2h_channel_open_cnt);
        }
        else if(vchannel->dir == DMA_TO_DEVICE){
            if(vdev->h2c_channel_open_cnt == 0){
                wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_CR), VDMA_H2C_CR_DMA_EN_MASK, BIT_H2C_CR_DMA_EN);
                wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_CR), VDMA_H2C_CR_INT_METHOD_MASK, (poll_mode ? BIT_H2C_CR_INT_METHOD_POLL : BIT_H2C_CR_INT_METHOD_MSI ));
            }

            vdev->h2c_channel_open_cnt++;
            pr_info(" start DMA engine with mode 0, DMA engine cnt:%d \n", vdev->h2c_channel_open_cnt);
        }
    }

    if(poll_mode){
		//Modified by zhang yue on 2024-04-08 start
    	if(vchannel->dir == DMA_FROM_DEVICE)
            vdev->cmplthp[vchannel->id]->work_cnt = 1;
        else if(vchannel->dir == DMA_TO_DEVICE)
            vdev->fetchthp[vchannel->id]->work_cnt = 1;		
	    //vdev->cmplthp[vchannel->id]->work_cnt = 1;
	    //Modified by zhang yue on 2024-04-08 end	
    }
	
    //added by zhang yue on 2023-12-22 start
    vchannel->fb_idx = 0x3C;  //first assign one invalid idx
    vchannel->wb_idx = 0x3C;  //first assign one invalid idx
    //added by zhang yue on 2023-12-22 end

    //Mask DMA Run, and control by 'RUN' menu in the GUI
    //pr_info(" DMA run. \n");
    //wrReg32Modify(pBrd, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_DMA_CHANS_MASK, BIT_C2H_CR_DMA_CHAN0_RUN);
    
    return 0;
}

static int vdma_gen_desc(vdma_channel_t *vchannel)
{
    int i, j;
    vdma_dev_t *vdev;
    struct sg_table *table;
    struct scatterlist *sg;
    vdma_desc_hw_t *desc;
    unsigned int num_sg, sum_sg = 0;
    unsigned long desc_phy_addr;
    size_t desc_size;
    
    if(!vchannel){
        pr_err("vchannel NULL vdma_gen_desc!\n");
        return -EINVAL;
    }
    vdev = vchannel->vdev;
    if(!vdev){
        pr_err("vdev NULL vdma_gen_desc!\n");
        return -EINVAL;
    }
    
    //Generate desc for buffer from user space
    for (i = 0; i < vchannel->ubuf_cnt; i++) {
        table = vchannel->ubuf_sg_table[i];
        //desc_size = (table->nents) * sizeof(vdma_desc_hw_t);
        desc_size = vchannel->sge_cnts[i] * sizeof(vdma_desc_hw_t);
        vchannel->vdma_desc[i] = dma_alloc_coherent(&vdev->pdev->dev, desc_size, 
                              &vchannel->vdma_desc_handle[i], GFP_KERNEL | __GFP_ZERO);
        if (!vchannel->vdma_desc[i]) {
            vdma_desc_free(vchannel);
            return -ENOMEM;
        }
#if( WRITE_SG_LIST_INFO_TO_FILE)
       printk_file("channel%d: dir:%d, desc[%d]: cpu_addr %px dma_addr: %llx, len = %zu\n", vchannel->id, vchannel->dir, i, 
                               vchannel->vdma_desc[i], vchannel->vdma_desc_handle[i], desc_size);
#else        
        pr_info("channel%d: dir:%d, desc[%d]: cpu_addr %px dma_addr: %llx, len = %zu\n", vchannel->id, vchannel->dir, i, 
                               vchannel->vdma_desc[i], vchannel->vdma_desc_handle[i], desc_size);
#endif
    }
    
    //Fill SG-Elements info
    for (i = 0; i < vchannel->ubuf_cnt; i++){
        table = vchannel->ubuf_sg_table[i];
        desc = vchannel->vdma_desc[i];
        num_sg = vchannel->sge_cnts[i];
        
        sum_sg += num_sg;
        if(vdma_debug){
#if( WRITE_SG_LIST_INFO_TO_FILE)
	        printk_file("=== buf[%d] has nents %d and has sg_num %d ===\n", i, table->nents, num_sg);
#else        			
            pr_info("=== buf[%d] has nents %d and has sg_num %d ===\n", i, table->nents, num_sg);
#endif
        }
        for_each_sg(table->sgl, sg, num_sg, j) {
            if(vdma_debug) {
                /*if(j < 2 || j > num_sg - 3) //comment by zhang yue on 2024-04-10 temply for Percy debug*/ { // Limit the print info
#if( WRITE_SG_LIST_INFO_TO_FILE)
#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
                    printk_file("[%02d]: dma_addr: %#llx, len = %u\n", j, sg_dma_address(sg), sg_dma_len(sg));
#else
                    printk_file("[%02d]: dma_addr: %#x, len = %u\n", j, sg_dma_address(sg), sg_dma_len(sg));
#endif
#else
#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
                    pr_info("[%02d]: dma_addr: %#llx, len = %u\n", j, sg_dma_address(sg), sg_dma_len(sg));
#else
                    pr_info("[%02d]: dma_addr: %#x, len = %u\n", j, sg_dma_address(sg), sg_dma_len(sg));
#endif
#endif
                }
            }
            vdev->isDmaAddr64 ? (desc[j].addr_hi = sg_dma_address(sg) >> 32): (desc[j].addr_hi = 0);
            //desc[j].addr_hi = sg_dma_address(sg) >> 32;
            desc[j].addr_lo = sg_dma_address(sg);
            desc[j].flags_len = DMA_SGE_FRME_BUF_INX(i) | sg_dma_len(sg);
        }

        //Overwrite the last item of SGE 
        desc[num_sg - 1].flags_len |= (DMA_SGE_FLAGS_LAST);
#if( WRITE_SG_LIST_INFO_TO_FILE)
        printk_file("---------------------");   
        printk_file("**buf[%d] SGE[0] -DW0:0x%x, DW1:0x%x, DW2:0x%x \n", i, desc[0].addr_hi, desc[0].addr_lo, desc[0].flags_len);
        printk_file("**        SGE[1] -DW0:0x%x, DW1:0x%x, DW2:0x%x \n", desc[1].addr_hi, desc[1].addr_lo, desc[1].flags_len);
        printk_file("**        SGE[%d]-DW0:0x%x, DW1:0x%x, DW2:0x%x \n", num_sg - 1, desc[num_sg - 1].addr_hi, desc[num_sg - 1].addr_lo, desc[num_sg - 1].flags_len);
#else
        pr_info("---------------------");   
        pr_info("**buf[%d] SGE[0] -DW0:0x%x, DW1:0x%x, DW2:0x%x \n", i, desc[0].addr_hi, desc[0].addr_lo, desc[0].flags_len);
        pr_info("**        SGE[1] -DW0:0x%x, DW1:0x%x, DW2:0x%x \n", desc[1].addr_hi, desc[1].addr_lo, desc[1].flags_len);
        pr_info("**        SGE[%d]-DW0:0x%x, DW1:0x%x, DW2:0x%x \n", num_sg - 1, desc[num_sg - 1].addr_hi, desc[num_sg - 1].addr_lo, desc[num_sg - 1].flags_len);
#endif

        //Store the SGE physical into SGB ADDR Regs
        desc_phy_addr = vchannel->vdma_desc_handle[i];
        if (vchannel->dir == DMA_FROM_DEVICE){
            wrReg32(vdev, SGDMA(__REG_VDMA_C2H_CHX_SGB_ADDR_HI_SELECT(vchannel->id, i)), (u32)(desc_phy_addr >> 32));
            mdelay(10);
            wrReg32(vdev, SGDMA(__REG_VDMA_C2H_CHX_SGB_ADDR_LO_SELECT(vchannel->id, i)), (u32)desc_phy_addr);
            mdelay(10);
            wrReg32(vdev, SGDMA(__REG_VDMA_C2H_CHX_SGB_SGLIST_LEN_SELECT(vchannel->id, i)), vchannel->sge_cnts[i]);
            mdelay(10);
            if(vdma_debug) {
#if( WRITE_SG_LIST_INFO_TO_FILE)
                printk_file("===buf[%d] cur_desc: %px,desc_phy_addr: 0x%lx ===\n", i, desc, desc_phy_addr);
                printk_file("Channel: %d, REG_SGB_ADDR_HI: 0x%lx  VAL: 0x%x    \n", vchannel->id, SGDMA(__REG_VDMA_C2H_CHX_SGB_ADDR_HI_SELECT(vchannel->id, i)), (u32)(desc_phy_addr >> 32));
                printk_file("Channel: %d, REG_SGB_ADDR_LO: 0x%lx  VAL: 0x%x    \n", vchannel->id, SGDMA(__REG_VDMA_C2H_CHX_SGB_ADDR_LO_SELECT(vchannel->id, i)), (u32)desc_phy_addr);
                printk_file("Channel: %d, REG_SGB_SGLIST_LEN: 0x%lx  VAL: 0x%x \n", vchannel->id, SGDMA(__REG_VDMA_C2H_CHX_SGB_SGLIST_LEN_SELECT(vchannel->id, i)), vchannel->sge_cnts[i]);
                printk_file("=================================================\n");


#else
                pr_info("===buf[%d] cur_desc: %px,desc_phy_addr: 0x%lx ===\n", i, desc, desc_phy_addr);
                pr_info("Channel: %d, REG_SGB_ADDR_HI: 0x%lx  VAL: 0x%x    \n", vchannel->id, SGDMA(__REG_VDMA_C2H_CHX_SGB_ADDR_HI_SELECT(vchannel->id, i)), (u32)(desc_phy_addr >> 32));
                pr_info("Channel: %d, REG_SGB_ADDR_LO: 0x%lx  VAL: 0x%x    \n", vchannel->id, SGDMA(__REG_VDMA_C2H_CHX_SGB_ADDR_LO_SELECT(vchannel->id, i)), (u32)desc_phy_addr);
                pr_info("Channel: %d, REG_SGB_SGLIST_LEN: 0x%lx  VAL: 0x%x \n", vchannel->id, SGDMA(__REG_VDMA_C2H_CHX_SGB_SGLIST_LEN_SELECT(vchannel->id, i)), vchannel->sge_cnts[i]);
                pr_info("=================================================\n");
#endif
            }
        }
        else if(vchannel->dir == DMA_TO_DEVICE){
            wrReg32(vdev, SGDMA(__REG_VDMA_H2C_CHX_SGB_ADDR_HI_SELECT(vchannel->id, i)), (u32)(desc_phy_addr >> 32));
            mdelay(10);
            wrReg32(vdev, SGDMA(__REG_VDMA_H2C_CHX_SGB_ADDR_LO_SELECT(vchannel->id, i)), (u32)desc_phy_addr);
            mdelay(10);  
            wrReg32(vdev, SGDMA(__REG_VDMA_H2C_CHX_SGB_SGLIST_LEN_SELECT(vchannel->id, i)), vchannel->sge_cnts[i]);
            mdelay(10);
            if(vdma_debug) {
#if( WRITE_SG_LIST_INFO_TO_FILE)
                printk_file("===buf[%d] cur_desc: %px,desc_phy_addr: 0x%lx ===\n", i, desc, desc_phy_addr);
                printk_file("Channel: %d, REG_SGB_ADDR_HI: 0x%lx  VAL: 0x%x	\n", vchannel->id, SGDMA(__REG_VDMA_H2C_CHX_SGB_ADDR_HI_SELECT(vchannel->id, i)), (u32)(desc_phy_addr >> 32));
                printk_file("Channel: %d, REG_SGB_ADDR_LO: 0x%lx  VAL: 0x%x	\n", vchannel->id, SGDMA(__REG_VDMA_H2C_CHX_SGB_ADDR_LO_SELECT(vchannel->id, i)), (u32)desc_phy_addr);
                printk_file("Channel: %d, REG_SGB_SGLIST_LEN: 0x%lx  VAL: 0x%x \n", vchannel->id, SGDMA(__REG_VDMA_H2C_CHX_SGB_SGLIST_LEN_SELECT(vchannel->id, i)), vchannel->sge_cnts[i]);
                printk_file("=================================================\n");
#else
                pr_info("===buf[%d] cur_desc: %px,desc_phy_addr: 0x%lx ===\n", i, desc, desc_phy_addr);
                pr_info("Channel: %d, REG_SGB_ADDR_HI: 0x%lx  VAL: 0x%x    \n", vchannel->id, SGDMA(__REG_VDMA_H2C_CHX_SGB_ADDR_HI_SELECT(vchannel->id, i)), (u32)(desc_phy_addr >> 32));
                pr_info("Channel: %d, REG_SGB_ADDR_LO: 0x%lx  VAL: 0x%x    \n", vchannel->id, SGDMA(__REG_VDMA_H2C_CHX_SGB_ADDR_LO_SELECT(vchannel->id, i)), (u32)desc_phy_addr);
                pr_info("Channel: %d, REG_SGB_SGLIST_LEN: 0x%lx  VAL: 0x%x \n", vchannel->id, SGDMA(__REG_VDMA_H2C_CHX_SGB_SGLIST_LEN_SELECT(vchannel->id, i)), vchannel->sge_cnts[i]);
                pr_info("=================================================\n");
#endif
            }
        }
    }

    if(poll_mode){
        vchannel->non_int_vir = dma_alloc_coherent(&(vdev->pdev->dev), sizeof(u32), 
                                         &vchannel->non_int_bus, GFP_KERNEL | __GFP_ZERO);
#if( WRITE_SG_LIST_INFO_TO_FILE)
        printk_file("Vchannel:%d dir:%d, non-INT buffer info:  cpu_addr %px, dma_addr: %llx, len = %zu\n", vchannel->id, vchannel->dir, vchannel->non_int_vir,
                                vchannel->non_int_bus , sizeof(u32));
#else
        pr_info("Vchannel:%d dir:%d, non-INT buffer info:  cpu_addr %px, dma_addr: %llx, len = %zu\n", vchannel->id, vchannel->dir, vchannel->non_int_vir,
                                vchannel->non_int_bus , sizeof(u32));
#endif
        if (vchannel->dir == DMA_FROM_DEVICE){
            wrReg32(vdev, SGDMA(REG_VDMA_C2H_NONINT_ADDR_HIGH(vchannel->id)), (u32)(vchannel->non_int_bus >> 32)); 
            mdelay(10);    
            wrReg32(vdev, SGDMA(REG_VDMA_C2H_NONINT_ADDR_LOW(vchannel->id)), (u32)( vchannel->non_int_bus));
            mdelay(10);  
            //wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_INT_METHOD_MASK,  BIT_C2H_CR_INT_METHOD_POLL);
        }
        else if(vchannel->dir == DMA_TO_DEVICE){
            wrReg32(vdev, SGDMA(REG_VDMA_H2C_NONINT_ADDR_HIGH(vchannel->id)), (u32)(vchannel->non_int_bus >> 32)); 
            mdelay(10);    
            wrReg32(vdev, SGDMA(REG_VDMA_H2C_NONINT_ADDR_LOW(vchannel->id)), (u32)( vchannel->non_int_bus));
            mdelay(10);  
        }
    }

    if (vchannel->dir == DMA_FROM_DEVICE){
        wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_FB_INS_NUM_MASK, ((vchannel->ubuf_cnt) << 27));
        // Enable interrupts
        wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_IRQ_MASK), 
                       VDMA_C2H_IRQ_FRME_TRANS_DONE_MASK | VDMA_C2H_IRQ_SGB_ADDR_LEN_ERR_MASK 
                       | VDMA_C2H_IRQ_FRME_TRANS_DONE_DAT_LOS_MASK | VDMA_C2H_IRQ_VIDEO_IN_HALTED_MASK, 
                       BIT_C2H_IRQ_FRME_TRANS_DONE_EN | BIT_C2H_IRQ_SGB_ADDR_LEN_ERR_DIS
                       | BIT_C2H_IRQ_FRME_TRANS_DONE_DAT_LOS_EN | BIT_C2H_IRQ_VIDEO_IN_HALTED_EN );
    }
    else if(vchannel->dir == DMA_TO_DEVICE){
        wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_CR), VDMA_H2C_CR_FB_INS_NUM_MASK, ((vchannel->ubuf_cnt) << 27));
        // Enable interrupts
        wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_IRQ_MASK), 
                       VDMA_H2C_IRQ_FRME_TRANS_DONE_MASK | VDMA_H2C_IRQ_DESC_TERM_INT_MASK, 
                       BIT_H2C_IRQ_FRME_TRANS_DONE_EN | BIT_H2C_IRQ_DESC_TERM_INT_EN);
    }

    return 0;
}

#define  FRAME_INDEX_OFFSET    (12)
static int lscvdma_thread_proc(void * complthp)
{
    lscvdma_kthread_t *thp = NULL;
    u32 desc_wb = 0;
    uint8_t idx_done = 0;
    vdma_channel_t * vchannel;
    uint8_t * pByte = NULL;
    uint32_t non_int_mem_cpy = 0;

    thp  = (lscvdma_kthread_t *)complthp;
    vchannel = thp->v_channel;
    //pr_info("enter Preidx = 0x%x \n", vchannel->wb_idx);

    non_int_mem_cpy = *vchannel->non_int_vir;
    idx_done = non_int_mem_cpy & 0x01;
    
    desc_wb = (non_int_mem_cpy & 0x1F000) >> FRAME_INDEX_OFFSET;

    
    if(!idx_done )
        return 0;

        	
    if(((vchannel->wb_idx + 1) % 16) != desc_wb){
        //pr_err("******%s  thread id =%d  non_int_vir= 0x%08x idx_done=0x%x idx = 0x%x desc_wb = 0x%x\n", thp->name, thp->id, non_int_mem_cpy, idx_done, vchannel->wb_idx, desc_wb);
    }
    
    thp->cnt++;

    //added by zhang yue on 2024-01-05 for debug start
    pByte = (uint8_t *) vchannel->non_int_vir;
    if(idx_done){
        *pByte &= (~0x01);
    }
    //added by zhang yue on 2024-01-05 for debug end
	
    spin_lock(&(thp->lock));
    //desc_wb = ((*vchannel->non_int_vir) & 0x1F000) >>  FRAME_INDEX_OFFSET;
    
	
    if(vchannel->wb_idx != desc_wb  || !vchannel->wb_idx){
        //pr_err("------%s  thread id =%d  non_int_vir= 0x%08x idx_done = 0x%x idx = 0x%x desc_wb = 0x%x\n",thp->name, thp->id, *(vchannel->non_int_vir), idx_done, vchannel->wb_idx, desc_wb);
        vchannel->wb_idx = desc_wb;
        vchannel->wb_idx_update = 1;
    }


    spin_unlock(&(thp->lock));
    wake_up(&vchannel->wq);

    return 0;
}

static int lscvdma_thread_main(void *data)
{
    lscvdma_kthread_t *thp = (lscvdma_kthread_t *)data;
    //vdma_channel_t * vchannel =thp->v_channel;
     
    
    pr_info("%s up. \n", thp->name);
    disallow_signal(SIGPIPE);
    
    while(!kthread_should_stop()){
        //struct list_head *work_item, *next;
        wait_event_interruptible_timeout(thp->waitq, thp->schedule, thp->timeout);
		
        thp->schedule = 0;
		
        if(thp->work_cnt){
            thp->fproc(thp);
        }
       
        schedule();
    }
    
    pr_info("%s exit. \n", thp->name);
    return 0;
}


static int lscvdma_kthread_start(lscvdma_kthread_t *thp, char *name, int id)
{
    int len;
    //int node;
    
    if(thp->task){
        pr_warn(" kthread %s already running \n", thp->name);
        return -EINVAL;
    }
    
    len =snprintf(thp->name, sizeof(thp->name),"%s_%d", name, id);
    if(len < 0){
        pr_err("thread %d, error in snprintf name %s. \n", id, name);
        return -EINVAL;
    }
    
    thp->id = id;
    spin_lock_init(&thp->lock);
    init_waitqueue_head(&thp->waitq);

    
    //thp->task = kthread_create_on_node(lscvdma_thread_main, (void *)thp, node, "%s", thp->name);
    thp->task = kthread_create(lscvdma_thread_main, (void *)thp, thp->name);    
    if(IS_ERR(thp->task)){
        pr_err("kthread %s, create task failed: 0x%lx \n", thp->name, (unsigned long)IS_ERR(thp->task));
        thp->task = NULL;
        return -EFAULT;
    }
    
    //kthread_bind(thp->task, thp->cpu);
    
    wake_up_process(thp->task);
    
    return 0;
}

static int lscvdma_kthread_stop(lscvdma_kthread_t *thp)
{
    int ret;
    
    if(!thp->task){
        pr_info("kthread %s already stopped. \n",thp->name);
        return 0;
    }
    
    thp->schedule = 0;
    ret = kthread_stop(thp->task);
    if(ret < 0){
        pr_warn("kthread %s, stop err %d \n", thp->name, ret);
        return ret;
    }
    
    pr_info("kthread %s, 0x%p, stopped \n", thp->name, thp->task);
    thp->task = NULL;
    
    return 0;
}

static int lscvdma_thread_create(vdma_dev_t * vdev, u32 thread_id, enum dma_data_direction dir)
{
    lscvdma_kthread_t *thp;
    int ret;
    //int cpu;
    
    if(vdev == NULL){
        pr_warn("invaild parameter vdev \n");
        return -1;
    }

    if(thread_id > 3 ){
        pr_warn("Invalid parameter, thread_id =%d, it should be less than 4\n", thread_id);
        return -2;
    }


    if(dir == DMA_FROM_DEVICE){
	    if(NULL != vdev->cmplthp[thread_id]){
		    pr_warn("DMA from device, this thread already exist\n");
		    return -3;
	    }
		
        vdev->cmplthp[thread_id] =kzalloc(sizeof(lscvdma_kthread_t), GFP_KERNEL);
	thp = vdev->cmplthp[thread_id];
    }else if(dir == DMA_TO_DEVICE){
	    if(NULL != vdev->fetchthp[thread_id]){
		    pr_warn("DMA to device, this thread already exist\n");
		    return -3;
	    }
	
	    vdev->fetchthp[thread_id] =kzalloc(sizeof(lscvdma_kthread_t), GFP_KERNEL);
	
	    if (!vdev->fetchthp[thread_id] ){
		    pr_err("alloc lscvdma_threads failed, thread %p \n", vdev->cmplthp[thread_id]);
		    return -ENOMEM;
	    }
	    thp = vdev->fetchthp[thread_id];
    }

	thp->timeout = 5;
	thp->fproc = lscvdma_thread_proc;

    if(dir == DMA_FROM_DEVICE){
		
		thp->v_channel = &(vdev->vdma_chan_c2h[thread_id]);
        ret = lscvdma_kthread_start(thp, "lscp", thread_id);
        if( ret < 0){
            kfree(vdev->cmplthp[thread_id]);
            vdev->cmplthp[thread_id] = NULL;
            return ret;
        }
    }else if(dir == DMA_TO_DEVICE){
        thp->v_channel = &(vdev->vdma_chan_h2c[thread_id]);

        ret = lscvdma_kthread_start(thp, "lscf", thread_id);
        if( ret < 0){
            kfree(vdev->cmplthp[thread_id]);
            vdev->cmplthp[thread_id] = NULL;
            return ret;
   	    }
    }
	
	return ret;
}

static void lscvdma_thread_destroy(vdma_dev_t *vdev, u32 thread_id, enum dma_data_direction dir)
{
    //int i;
    lscvdma_kthread_t *thp;
    
    pr_info("enter \n");
    if(thread_id > 3 || NULL == vdev)
        return;

    if(dir == DMA_FROM_DEVICE){
        thp = vdev->cmplthp[thread_id];
    }else{
        thp = vdev->fetchthp[thread_id];
    }

    if(NULL == thp){
        pr_info("Invalid parameter\n");
        return;
    }
	
    if(thp->fproc){
        lscvdma_kthread_stop(thp);
    }

    kfree(thp);
	
    if(dir == DMA_FROM_DEVICE){
        vdev->cmplthp[thread_id] = NULL;
    }else{
        vdev->fetchthp[thread_id] = NULL;
    }

}


static int lscvdma_open(struct inode *inode, struct file *filp)
{
    int ret = 0;
    struct vdma_cdev *vcdev;
    struct vdma_channel *vchannel; 
    
    struct vdma_dev * vdev; 
    
    vcdev = container_of(inode->i_cdev, struct vdma_cdev, cdev);
    if(!vcdev){
        pr_err("vcdev NULL lscvdma_open!\n");
        return -EINVAL;
    }
    
    filp->private_data = vcdev;
    vchannel = vcdev->channel;
    
    if(vchannel->open_flag){
        pr_err("channel# %d already open!\n", vchannel->id);
        return -EBUSY;
    }
    vchannel->open_flag = 1;

    vdev = vchannel->vdev;
    if(poll_mode){
        pr_info("prepare to create lscvdma thread \n");
        
        if (vchannel->dir == DMA_FROM_DEVICE){
            if(vdev->c2h_channel_open_cnt == 0){
                wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_INT_METHOD_MASK, (poll_mode ? BIT_C2H_CR_INT_METHOD_POLL : BIT_C2H_CR_INT_METHOD_MSI ));
	    }
        }
        else if(vchannel->dir == DMA_TO_DEVICE){
            if(vdev->h2c_channel_open_cnt == 0){
                wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_CR), VDMA_H2C_CR_INT_METHOD_MASK, (poll_mode ? BIT_H2C_CR_INT_METHOD_POLL : BIT_H2C_CR_INT_METHOD_MSI ));
            }
        }

        #if 1 //Modified by zhang yue on 2024-04-07 start
        ret = lscvdma_thread_create(vdev, vchannel->id, vchannel->dir);
        #else
        ret = lscvdma_thread_create(vdev, vchannel->id);
        #endif ////Modified by zhang yue on 2024-04-07 end
        if(ret < 0){
            pr_err("lscvdma_thread_create failed!\n");
            return ret;
        }
        
    }
	
    return 0;
}

/**
 * Close.
 * The complement to open().
 */
static int lscvdma_release(struct inode *inode, struct file *filp)
{
    struct vdma_dev *vdev;
    struct vdma_cdev *vcdev;
    struct vdma_channel *vchannel; 
    
    pr_info("enter!\n");
    
    vcdev = (struct vdma_cdev *)filp->private_data;
    if(!vcdev){
        pr_err("vcdev NULL lscvdma_release!\n");
        return -EINVAL;
    }
    
    vdev = vcdev->vdev;
    if(!vdev){
        pr_err("vdev NULL lscvdma_release!\n");

        return -EINVAL;
    }
    
    vchannel = vcdev->channel;
    if(!vchannel){
        pr_err("vchannel NULL lscvdma_release!\n");
        return -EINVAL;
    }

    if(poll_mode){
        #if 1
        lscvdma_thread_destroy(vdev, vchannel->id, vchannel->dir);
        #else
        lscvdma_thread_destroy(vdev, vchannel->id);
        #endif
    }
    if (vchannel->dir == DMA_FROM_DEVICE){
        if(vdev->c2h_channel_open_cnt == 0){
            wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_GLOBAL_RST_MASK, BIT_C2H_CR_GLOBAL_RST);
            mdelay(50);
            wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_GLOBAL_RST_MASK, BIT_C2H_CR_GLOBAL_RST_DEASSERT);  
        }
    }
    else if(vchannel->dir == DMA_TO_DEVICE){
        if(vdev->h2c_channel_open_cnt == 0){
            wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_CR), VDMA_H2C_CR_GLOBAL_RST_MASK, BIT_H2C_CR_GLOBAL_RST);
            mdelay(50);
            wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_CR), VDMA_H2C_CR_GLOBAL_RST_MASK, BIT_H2C_CR_GLOBAL_RST_DEASSERT);  
        }
    }

    vdma_desc_free(vchannel);
    vdma_ubuf_unmap(vchannel);
    
    vchannel->ubuf_cnt = 0;
    vchannel->sge_mode = 0;
    vchannel->open_flag = 0;
    vchannel->frm_stats = 0;
     
    return 0;
}

/**
 * ioctl.
 * Allow simple access to generic PCI control type things like enabling
 * device interrupts and such.
 * IOCTL works on a board object as a whole, not a BAR.
 */
static long lscvdma_ioctl( struct file *filp, unsigned int cmd, unsigned long arg)
{
    int err;
    struct vdma_dev *vdev;
    struct vdma_cdev *vcdev;
    struct vdma_channel *vchannel; 
    vdma_rw_ioctl_t rw;

    
    vcdev = (struct vdma_cdev *)filp->private_data;
    if(!vcdev){
        pr_err("vcdev NULL lscvdma_ioctl!\n");
        return -EINVAL;
    }
    
    vdev = vcdev->vdev;
    if(!vdev){
        pr_err("vdev NULL lscvdma_ioctl!\n");
        return -EINVAL;
    }
    
    vchannel = vcdev->channel;
    if(!vchannel){
        pr_err("vchannel NULL lscvdma_ioctl!\n");
        return -EINVAL;
    }
    
    switch(cmd)
    {
        case IOCTL_LSCVDMA_GET_VERSION_INFO:
            //strncpy((void *)arg, version, MAX_DRIVER_VERSION_LEN - 1);
            if (copy_to_user((void *)arg, (void *)version, sizeof(version)) != 0)
                return -EFAULT;
            break;
            
        case IOCTL_LSCVDMA_READ_8BIT:
            if (copy_from_user(&rw, (vdma_rw_ioctl_t *)arg, sizeof(vdma_rw_ioctl_t)))
                return -EACCES;
            //pr_info("reg:0x%x \n", rw.reg);
            rw.value = rdReg8(vdev, rw.reg);
            if (copy_to_user((void*)arg, &rw, sizeof(vdma_rw_ioctl_t))) {
                pr_err("r8 copy_to_user failed.\n");
                return -EACCES;
            }
            break;
      
        case IOCTL_LSCVDMA_READ_16BIT:
            if (copy_from_user(&rw, (vdma_rw_ioctl_t *)arg, sizeof(vdma_rw_ioctl_t)))
                return -EACCES;
            //pr_info("reg:0x%x \n", rw.reg);
            rw.value = rdReg16(vdev, rw.reg);
            if (copy_to_user((void*)arg, &rw, sizeof(vdma_rw_ioctl_t))) {
                pr_err("r16 copy_to_user failed.\n");
                return -EACCES;
            }
            break;
      
        case IOCTL_LSCVDMA_READ_32BIT:
            if (copy_from_user(&rw, (vdma_rw_ioctl_t *)arg, sizeof(vdma_rw_ioctl_t)))
                return -EACCES;
            //pr_info("reg:0x%x \n", rw.reg);
            rw.value = rdReg32(vdev, rw.reg);
            if (copy_to_user((void*)arg, &rw, sizeof(vdma_rw_ioctl_t))) {
                pr_err("r32 copy_to_user failed.\n");
                return -EACCES;
            }
            break;
      
        case IOCTL_LSCVDMA_WRITE_8BIT:
            if (copy_from_user(&rw, (vdma_rw_ioctl_t *)arg, sizeof(vdma_rw_ioctl_t))) {
                pr_err("w8 copy_to_user failed.\n");
                return -EACCES;
            }
            //pr_info("reg:0x%x val8: 0x%x\n", rw.reg, rw.value);
            wrReg8(vdev, rw.reg, rw.value);
            break;
      
        case IOCTL_LSCVDMA_WRITE_16BIT:
            if (copy_from_user(&rw, (vdma_rw_ioctl_t *)arg, sizeof(vdma_rw_ioctl_t))) {
                pr_err("w16 copy_to_user failed.\n");
                return -EACCES;
            }
            //pr_info("reg:0x%x val16: 0x%x\n", rw.reg, rw.value);
            wrReg16(vdev, rw.reg, rw.value);
            break;
      
        case IOCTL_LSCVDMA_WRITE_32BIT:
            if (copy_from_user(&rw, (vdma_rw_ioctl_t *)arg, sizeof(vdma_rw_ioctl_t))) {
                pr_err("w32 copy_to_user failed.\n");
                return -EACCES;
            }
            //pr_info("reg:0x%x val32: 0x%x\n", rw.reg, rw.value);
            wrReg32(vdev, rw.reg, rw.value);
            break;
            
        case IOCTL_LSCVDMA_START:
            pr_info("start enter: \n");
            if (vdma_ubuf_map(vchannel, cmd, arg))
                return -ENOMEM;
                
            if (vdma_gen_desc(vchannel)) {
                vdma_ubuf_unmap(vchannel);
                return -ENOMEM;
            }
            err = vdma_start_transfer(vchannel);
            if(err){
                pr_err("vdma_start_transfer failed.\n");
                return err;
            }
            break;
            
        case IOCTL_LSCVDMA_STOP:
            err = vdma_stop_transfer(vchannel);
            if(err){
                pr_err("vdma_stop_transfer failed.\n");
                return err;
            }
            break;
            
        case IOCTL_LSCVDMA_TX_FRAME:  
            err = vdma_tx_frame(vchannel, cmd, arg);
            if(err){
                //pr_err("vdma_tx_frame failed.\n");
                return err;
            }
            break;
            
        case IOCTL_LSCVDMA_GET_FRAME:  
            err = vdma_recv_frame(vchannel, cmd, arg);
            if(err){
                //pr_err("vdma_recv_frame failed.\n");
                return err;
            }
            break;
        //Added by Zhang Yue on 2023-12-04 start
        case IOCTL_LSCVDMA_GET_RESOURCES:
        //First make sure the pointer passed in arg is still valid user page
            err = vdma_get_resource(vchannel, cmd, arg);
            if(err){
                pr_err("vdma get resource failed.\n");
                return err;
            }
            break;
	    //Added by Zhang Yue on 2023-12-04 end
        default:
            return -EINVAL;
    }
    
    return 0;
    
}

/**
 * mmap.
 * This is the most important driver method.  This maps the device's PCI
 * address space (based on the select mmap BAR number) into the user's
 * address space, allowing direct memory access with standard pointers.
 */
static int lscvdma_mmap(struct file *filp, struct vm_area_struct *vma)
{
    return 0;
}

/**
 * read.
 * Read from system CommonBuffer DMA memory into users buffer.
 * User passes length (in bytes) like reading from a file.
 */
static ssize_t lscvdma_read(struct file *filp, char __user *userBuf, size_t len, loff_t *offp)
{
    return 0;
}

/**
 * write.
 * Write from users buffer into system CommonBuffer DMA memory.
 * User passes length (in bytes) like writing to a file.
 */
static ssize_t lscvdma_write(struct file *filp, const char __user *userBuf, size_t len, loff_t *offp)
{
    return 0;
}

/**
 * The file operations table for the device.
 * read/write/seek, etc. are not implemented because device access
 * is memory mapped based.
 */
static struct file_operations drvr_fops =
{
    owner:   THIS_MODULE,
    open:    lscvdma_open,
    release: lscvdma_release,
    unlocked_ioctl:   lscvdma_ioctl,
    mmap:    lscvdma_mmap,
    read:    lscvdma_read,
    write:   lscvdma_write,
};

static int vdma_cdev_destroy(struct vdma_cdev *vcdev)
{
    if(!vcdev){
        pr_err("vcdev NULL!\n");
        return -EINVAL;
    }
    
    if(!vcdev->vdev){
        pr_err("vdev NULL!\n");
        return -EINVAL;
    }
    
    if(vcdev->sys_dev){
        device_destroy(glscvdma_class, vcdev->cdevno);
        vcdev->sys_dev = NULL;
    }
    cdev_del(&(vcdev->cdev));
    
    return 0;
}

static int vdma_cdev_create(struct vdma_pci_dev *vpdev, struct vdma_cdev *vcdev, struct vdma_channel *vchannel, enum cdev_type type)
{
    int err;
    vdma_dev_t *vdev = vpdev->vdev;
    static dev_t devno;
    u32 minorNum;
    
    // Register device driver as a character device and get a dynamic Major number
    if(!(vpdev->majorNum)){
        err = alloc_chrdev_region(&devno, 0, MAX_MINORS, "lscvdma");
        if (err < 0) {
            pr_err("can't get major/minor numbers!\n");
            return(err);
        }
        vpdev->majorNum = MAJOR(devno);  
        vpdev->minorNum = MINOR(devno);
    }
    
    // Initialize the CharDev entry for this new found hw board device
    vcdev->cdev.owner = THIS_MODULE;
    vcdev->vpdev = vpdev;
    vcdev->vdev = vdev;
    vcdev->channel = vchannel;
    
    if (type == CHAR_VDMA_C2H)
        kobject_set_name(&(vcdev->cdev.kobj), "lscvdma%d_c2h_%d", g_boards_num, vchannel->id);
    
    if (type == CHAR_VDMA_H2C)
        kobject_set_name(&(vcdev->cdev.kobj), "lscvdma%d_h2c_%d", g_boards_num, vchannel->id);
        
    cdev_init(&(vcdev->cdev), &drvr_fops);
    if (type == CHAR_VDMA_C2H)
        minorNum = vpdev->minorNum + vchannel->id + 4 * g_boards_num;  
    if (type == CHAR_VDMA_H2C)
        minorNum = 40 + vpdev->minorNum + vchannel->id + 4 * g_boards_num;
    if (vdma_debug)
        pr_info("Major=%d Minor=%d \n", vpdev->majorNum, minorNum);
        
    vcdev->cdevno = MKDEV(vpdev->majorNum, minorNum);
    
    if (cdev_add(&(vcdev->cdev), vcdev->cdevno, MINORS_PER_BOARD)) {
        pr_err("error adding char device\n");
        kobject_put(&(vcdev->cdev.kobj));
        unregister_chrdev_region(vcdev->cdevno, MAX_MINORS);
        return(-1);	
    }

    if (glscvdma_class){
        if (type == CHAR_VDMA_C2H){
            vcdev->sys_dev = device_create(glscvdma_class, NULL, vcdev->cdevno, &vdev->pdev->dev,
		 "lscvdma%d_c2h_%d", g_boards_num, vchannel->id);
            pr_info("add device: lscvdma%d_c2h_%d \n", g_boards_num, vchannel->id);
		}
        else if(type == CHAR_VDMA_H2C){
            vcdev->sys_dev = device_create(glscvdma_class, NULL, vcdev->cdevno, &vdev->pdev->dev,
		 "lscvdma%d_h2c_%d", g_boards_num, vchannel->id);
	    pr_info("add device: lscvdma%d_h2c_%d \n", g_boards_num, vchannel->id);
		}
		 
        if(!vcdev->sys_dev){
            pr_err("device_create failed. \n");
            cdev_del(&(vcdev->cdev));
            unregister_chrdev_region(vcdev->cdevno, MAX_MINORS);
            return -EINVAL;
        }
    }
    
    return 0;
}

static void h2c_channel_service_work(struct work_struct *work)
{
    u8 ch;
    u32 cur_inx;
    struct vdma_dev *vdev;
	
    vdev = container_of(work, struct vdma_dev, h2c_work);
    if(!vdev){
        pr_err("Invalid vdev in channel_service_work. \n");
        return;
    }
	
    //get all channels index, used for judging if channels had index update at the moment
    cur_inx = (rdReg32(vdev, SGDMA(REG_VDMA_H2C_SR))) >> 12;
    pr_info("cur_inx:0x%x \n",cur_inx);
    for(ch = 0; ch < vdev->h2c_channel_max; ch++,  cur_inx >>= 5){
        if((vdev->vdma_chan_h2c[ch].fb_idx != (cur_inx & 0x1F)) || vdev->irq_trigger_ch_h2c[ch]){
            spin_lock(&vdev->vdma_chan_h2c[ch].lock);
            vdev->vdma_chan_h2c[ch].fb_idx = (cur_inx & 0x1F);
            vdev->vdma_chan_h2c[ch].fb_idx_update = 1;
            vdev->vdma_chan_h2c[ch].frm_stats++;
            vdev->irq_trigger_ch_h2c[ch] = false;
            spin_unlock(&vdev->vdma_chan_h2c[ch].lock);
            pr_info("ch:%d, fb_idx:0x%x \n", ch, vdev->vdma_chan_h2c[ch].fb_idx);
            wake_up(&vdev->vdma_chan_h2c[ch].wq);
        }
    } 

    wrReg32(vdev, SGDMA(REG_VDMA_H2C_IRQ_SRC), BIT_H2C_IRQ_SRC_FRME_TRANS_DONE_CLR);
    
    return;
}

#if 1
static void c2h_channel_service_work(struct work_struct *work)
{
    u8 ch;
    u32 cur_inx;
    struct vdma_dev *vdev;
	
    vdev = container_of(work, struct vdma_dev, c2h_work);
    if(!vdev){
        pr_err("Invalid vdev in channel_service_work. \n");
        return;
    }
	
    //get all channels index, used for judging if channels had index update at the moment
    cur_inx = (rdReg32(vdev, SGDMA(REG_VDMA_C2H_SR))) >> 12;

    for(ch = 0; ch < vdev->c2h_channel_max; ch++,  cur_inx >>= 5){

        if((vdev->vdma_chan_c2h[ch].fb_idx != (cur_inx & 0x1F)) || vdev->irq_trigger_ch_c2h[ch]){
            spin_lock(&vdev->vdma_chan_c2h[ch].lock);
            vdev->vdma_chan_c2h[ch].fb_idx = (cur_inx & 0x1F);
            vdev->vdma_chan_c2h[ch].fb_idx_update = 1;
            vdev->vdma_chan_c2h[ch].frm_stats++;
            vdev->irq_trigger_ch_c2h[ch] = false;
            spin_unlock(&vdev->vdma_chan_c2h[ch].lock);

            wake_up(&vdev->vdma_chan_c2h[ch].wq);
        }
    } 

    wrReg32(vdev, SGDMA(REG_VDMA_C2H_IRQ_SRC), BIT_C2H_IRQ_SRC_FRME_TRANS_DONE_CLR);
    
    return;
}


#else
static void channel_service_work(struct work_struct *work)
{
    u8 ch;
    u32 cur_inx, tmp;
    struct vdma_dev *vdev;
    static int pre_inx = -1;
    static int pre_fb_idx[C2H_CHANNEL_MAX]={-1};
    //static int cnt = 0;
    vdev = container_of(work, struct vdma_dev, work);
    if(!vdev){
        pr_err("Invalid vdev in channel_service_work. \n");
        return;
    }
	
    //cnt++;
    //get all channels index, used for judging if channels had index update at the moment
    cur_inx = (rdReg32(vdev, SGDMA(REG_VDMA_C2H_SR))) >> 12;
    //if(pre_inx != cur_inx){
    tmp = cur_inx;
        
    for(ch = 0; ch < vdev->c2h_channel_max; ch++, tmp >>= 5){
        
        vdev->vdma_chan_c2h[ch].fb_idx = tmp & 0x1F;
            
        //if(pre_fb_idx[ch] != vdev->vdma_chan_c2h[ch].fb_idx){
        if(vdev->msi_irq_vec[3*ch] == vdev->tri_irq){
            spin_lock(&vdev->vdma_chan_c2h[ch].lock);
            vdev->vdma_chan_c2h[ch].fb_idx_update = 1;
            vdev->vdma_chan_c2h[ch].frm_stats++;
            pre_fb_idx[ch] = vdev->vdma_chan_c2h[ch].fb_idx;
            spin_unlock(&vdev->vdma_chan_c2h[ch].lock);
                
            wake_up(&vdev->vdma_chan_c2h[ch].ReadWaitQ);
        }
        //if(cnt % 5 == 0)
            //pr_info("channel:%d , received frames index:%d ! \n",ch, vdev->vdma_chan_c2h[ch].fb_idx);
    } 
    pre_inx = cur_inx;
   //}
   //else{
        //pr_warn("Invalid IRQ, no frame index updates! \n");
        //pr_err("pre_inx:%d , cur_inx:%d ! \n",pre_inx,cur_inx);
        //return;
   //}

    wrReg32(vdev, SGDMA(REG_VDMA_C2H_IRQ_SRC), BIT_C2H_IRQ_SRC_FRME_TRANS_DONE_CLR);
    
    return;
}
#endif

static void c2h_irq_handler(vdma_dev_t *vdev, int irq)
{
    u32 value;
    u8 irq_ch = 0xFF;

    value = rdReg32(vdev, SGDMA(REG_VDMA_C2H_IRQ_SRC));
    if(value & VDMA_C2H_IRQ_SRC_FRME_TRANS_DONE_MASK){
    #if 1
        irq_ch = (irq <= vdev->msi_irq_vec[3]) ? (irq - vdev->msi_irq_vec[0]) : 0xFF;
			
        if(irq_ch != 0xFF && irq_ch < MAX_VDMA_CHANNEL_NUM){
            vdev->irq_trigger_ch_c2h[irq_ch]  = true;
        }
    #else
        vdev->tri_irq = irq;
    #endif
        schedule_work(&vdev->c2h_work);
    }
		
    if(value & VDMA_C2H_IRQ_SRC_FRME_TRANS_DONE_DAT_LOS_MASK){
        pr_warn("Frame transmit done with data loss.");
        wrReg32(vdev, SGDMA(REG_VDMA_C2H_IRQ_SRC), BIT_C2H_IRQ_SRC_FRME_TRANS_DONE_DAT_LOS_CLR);
    }
		
    if(value & VDMA_C2H_IRQ_SRC_VIDEO_IN_HALTED_MASK){
        pr_warn("Video input halted.");
        wrReg32(vdev, SGDMA(REG_VDMA_C2H_IRQ_SRC), BIT_C2H_IRQ_SRC_VIDEO_IN_HALTED_CLR);
    }

    if(value & VDMA_C2H_IRQ_SRC_DESC_TERM_INT_MASK){
        pr_warn("C2H Descriptor termination interrupt occurred.");
        wrReg32(vdev, SGDMA(REG_VDMA_C2H_IRQ_SRC), BIT_C2H_IRQ_SRC_DESC_TERM_INT_CLR);
    }
	
    return;
}

static void h2c_irq_handler(vdma_dev_t *vdev, int irq)
{
    u32 value;
    u8 irq_ch = 0xFF;

    value = rdReg32(vdev, SGDMA(REG_VDMA_H2C_IRQ_SRC));
    if(value & VDMA_H2C_IRQ_SRC_FRME_TRANS_DONE_MASK){
        pr_info("enter:");
    #if 1
        irq_ch = (irq <= vdev->msi_irq_vec[7]) ? (irq - vdev->msi_irq_vec[4]) : 0xFF;
			
        if(irq_ch != 0xFF && irq_ch < MAX_VDMA_CHANNEL_NUM){
            vdev->irq_trigger_ch_h2c[irq_ch]  = true;
        }
    #else
        vdev->tri_irq = irq;
    #endif
        schedule_work(&vdev->h2c_work);
    }

    if(value & VDMA_H2C_IRQ_SRC_DESC_TERM_INT_MASK){
        pr_warn("H2C Descriptor termination interruptc occurred.");
        wrReg32(vdev, SGDMA(REG_VDMA_H2C_IRQ_SRC), BIT_H2C_IRQ_SRC_DESC_TERM_INT_CLR);
    }
	
    return;
}

/* Interrupt handler. */
static irqreturn_t vdma_irq_handler(int irq, void *arg)
{
    vdma_dev_t *vdev;

    vdev = (vdma_dev_t *)arg;
    if(!vdev){
        pr_err("NULL, invalid vdev handle \n");
        return IRQ_NONE;
    }
    c2h_irq_handler(vdev, irq);
    h2c_irq_handler(vdev, irq);

    return IRQ_HANDLED;
}

/* User defined Interrupt handler. */
static irqreturn_t vdma_usr_irq_handler(int irq, void *arg)
{
    return IRQ_HANDLED;
}

static irqreturn_t vdma_one_msi_irq_handler(int irq, void *arg)
{
    u32 value;
    vdma_dev_t *vdev;
  
    vdev = (vdma_dev_t *)arg;
    if(!vdev){
        pr_err("NULL, invalid vdev handle \n");
        return IRQ_NONE;
    }
    value = rdReg32(vdev, SGDMA(REG_VDMA_PCIE_MSI_VECTOR));
	
    if(value & BIT_C2H_MSI_VECTOR_MASK_FRAME_TRANS_DONE){	
        if(value & BIT_C2H_MSI_VECTOR_CH0_FRAME_TRANS_DONE)
        {
            vdev->irq_trigger_ch_c2h[0] = true;
            //pr_warn("channel 0 frame transmit done normally");
        }
	
        if(value & BIT_C2H_MSI_VECTOR_CH1_FRAME_TRANS_DONE)
        {
            vdev->irq_trigger_ch_c2h[1] = true;
            //pr_warn("channel 1 frame transmit done normally");		
        }

        if(value & BIT_C2H_MSI_VECTOR_CH2_FRAME_TRANS_DONE)
        {
            vdev->irq_trigger_ch_c2h[2] = true;
            //pr_warn("channel 2 frame transmit done normally");		
        }

        if(value & BIT_C2H_MSI_VECTOR_CH3_FRAME_TRANS_DONE)
        {
            vdev->irq_trigger_ch_c2h[3] = true;
            //pr_warn("channel 3 frame transmit done normally");		
        }

        wrReg32(vdev, SGDMA(REG_VDMA_PCIE_MSI_VECTOR), (value & BIT_C2H_MSI_VECTOR_MASK_FRAME_TRANS_DONE));
        schedule_work(&vdev->c2h_work);
    }

    if(value & BIT_H2C_MSI_VECTOR_MASK_FRAME_TRANS_DONE){
			
        if(value & BIT_H2C_MSI_VECTOR_CH0_FRAME_TRANS_DONE)
        {
            vdev->irq_trigger_ch_h2c[0] = true;
            //pr_warn("channel 0 frame transmit done normally");
        }
		
        if(value & BIT_H2C_MSI_VECTOR_CH1_FRAME_TRANS_DONE)
        {
            vdev->irq_trigger_ch_h2c[1] = true;
            //pr_warn("channel 1 frame transmit done normally");		
        }
	
        if(value & BIT_H2C_MSI_VECTOR_CH2_FRAME_TRANS_DONE)
        {
            vdev->irq_trigger_ch_h2c[2] = true;
            //pr_warn("channel 2 frame transmit done normally");		
        }
	
        if(value & BIT_H2C_MSI_VECTOR_CH3_FRAME_TRANS_DONE)
        {
            vdev->irq_trigger_ch_h2c[3] = true;
            //pr_warn("channel 3 frame transmit done normally");		
        }
	
        wrReg32(vdev, SGDMA(REG_VDMA_PCIE_MSI_VECTOR), (value & BIT_H2C_MSI_VECTOR_MASK_FRAME_TRANS_DONE));
        schedule_work(&vdev->h2c_work);
    }
	
    if(value & BIT_H2C_MSI_VECTOR_MASK_DESC_TERM_INT ){	
        if(value & BIT_H2C_MSI_VECTOR_CH0_DESC_TERM_INT){
            pr_warn("H2C Channel 0 descriptor termination interrupt occurred.\n");
        }
			
        if(value & BIT_H2C_MSI_VECTOR_CH1_DESC_TERM_INT){
            pr_warn("H2C Channel 1 descriptor termination interrupt occurred.\n");
        }
		
        if(value & BIT_H2C_MSI_VECTOR_CH2_DESC_TERM_INT){
            pr_warn("H2C Channel 2 descriptor termination interrupt occurred.\n");
        }

        if(value & BIT_H2C_MSI_VECTOR_CH3_DESC_TERM_INT){
            pr_warn("H2C Channel 3 descriptor termination interrupt occurred.\n");
        }
        wrReg32(vdev, SGDMA(REG_VDMA_PCIE_MSI_VECTOR), (value & BIT_H2C_MSI_VECTOR_MASK_DESC_TERM_INT));	
        wrReg32(vdev, SGDMA(REG_VDMA_H2C_IRQ_SRC), BIT_H2C_IRQ_SRC_DESC_TERM_INT_CLR);
    }

    if(value & BIT_C2H_MSI_VECTOR_MASK_FRAME_DATA_LOSS ){	
        if(value & BIT_C2H_MSI_VECTOR_CH0_FRAME_DATA_LOSS)
            pr_warn("Channel 0 Frame transmit done with data loss.\n");
		
        if(value & BIT_C2H_MSI_VECTOR_CH1_FRAME_DATA_LOSS)
            pr_warn("Channel 1 Frame transmit done with data loss.\n");
	
        if(value & BIT_C2H_MSI_VECTOR_CH2_FRAME_DATA_LOSS)
            pr_warn("Channel 2 Frame transmit done with data loss.\n");

        if(value & BIT_C2H_MSI_VECTOR_CH3_FRAME_DATA_LOSS)
            pr_warn("Channel 3 Frame transmit done with data loss.\n");
        wrReg32(vdev, SGDMA(REG_VDMA_PCIE_MSI_VECTOR), (value & BIT_C2H_MSI_VECTOR_MASK_FRAME_DATA_LOSS));
        wrReg32(vdev, SGDMA(REG_VDMA_C2H_IRQ_SRC), BIT_C2H_IRQ_SRC_FRME_TRANS_DONE_DAT_LOS_CLR);
    }

    if(value & BIT_C2H_MSI_VECTOR_MASK_DESC_TERM_INT ){	
        if(value & BIT_C2H_MSI_VECTOR_CH0_DESC_TERM_INT)
            pr_warn("Channel 0 descriptor termination interrupt occurred.\n");
		
        if(value & BIT_C2H_MSI_VECTOR_CH1_DESC_TERM_INT)
            pr_warn("Channel 1 Frame transmit done with data loss.\n");
	
        if(value & BIT_C2H_MSI_VECTOR_CH2_DESC_TERM_INT)
            pr_warn("Channel 2 Frame transmit done with data loss.\n");

        if(value & BIT_C2H_MSI_VECTOR_CH3_DESC_TERM_INT)
            pr_warn("Channe 3 Frame transmit done with data loss.\n");
        wrReg32(vdev, SGDMA(REG_VDMA_PCIE_MSI_VECTOR), (value & BIT_C2H_MSI_VECTOR_MASK_DESC_TERM_INT));
        wrReg32(vdev, SGDMA(REG_VDMA_C2H_IRQ_SRC), BIT_C2H_IRQ_SRC_DESC_TERM_INT_CLR);
    }

    if(value & BIT_C2H_MSI_VECTOR_MASK_VIDEO_INPUT_HALT){
        if(value & BIT_C2H_MSI_VECTOR_CH0_VIDEO_INPUT_HALT)
            pr_warn("Channel 0 Video input halted. \n");
		
        if(value & BIT_C2H_MSI_VECTOR_CH1_VIDEO_INPUT_HALT)
            pr_warn("Channel 1 Frame transmit done with data loss. \n");
	
        if(value & BIT_C2H_MSI_VECTOR_CH2_VIDEO_INPUT_HALT)
            pr_warn("Channel 2 Frame transmit done with data loss. \n");

        if(value & BIT_C2H_MSI_VECTOR_CH3_VIDEO_INPUT_HALT)
            pr_warn("Channel 3 Frame transmit done with data loss. \n");
	
        wrReg32(vdev, SGDMA(REG_VDMA_PCIE_MSI_VECTOR), (value & BIT_C2H_MSI_VECTOR_MASK_FRAME_DATA_LOSS));
        wrReg32(vdev, SGDMA(REG_VDMA_C2H_IRQ_SRC), BIT_C2H_IRQ_SRC_VIDEO_IN_HALTED_CLR);
    }

    if(value & BIT_MSI_VECTOR_MASK_USR_DEFINED_INT){
        pr_warn("VDMA user defined interrupts. \n");
    }
 
    return IRQ_HANDLED;
}


static int vdma_msi_irq_setup(vdma_dev_t *vdev, struct pci_dev *pdev)
{
    int i, j, n, err;
    int msi_cnt;
    u32 vector;
    
#ifdef MSI
    // Use MSI Interrupt
    pr_info("Attach MSI interrupt\n");
    //ch0:0~2.  ch1:3~5.  ch2:6~8.  ch3:9~11.  user intr:24~28. IP ERR:30. PCIe Interface ERR:31    
    msi_cnt = pci_alloc_irq_vectors(vdev->pdev, 1, 32, PCI_IRQ_MSI);  
    if(msi_cnt < 0){
        pr_err("Failed to get MSI interrupts ret = %d\n", msi_cnt);	
        vdev->num_msi_irqs = 0;
        return -EINVAL;
    }
    pr_info("pci_alloc_irq_vectors get count = %d\n", msi_cnt);
    vdev->msi = true;
  
    if(msi_cnt < 8){
        vector = pci_irq_vector(vdev->pdev, 0);
        vdev->msi_irq_vec[0] = vector;
        
        err = request_irq(vector, vdma_one_msi_irq_handler, 0, "lscvdamOneMSI", vdev);
        if(err){
            pr_err("can't request MSI IRQ %d, channel: %d\n", vector, i);
            return err;
        }
        vdev->num_msi_irqs = msi_cnt;		
        return 0;
    }

    if(msi_cnt == 8){
        for (i = 0; i < 2; i++){
            for(j = 0; j < 4; j++){
                n = 4*i + j;
                vector = pci_irq_vector(vdev->pdev, n);
                vdev->msi_irq_vec[n] = vector;
                err = request_irq(vector, vdma_irq_handler, 0, "lscvdmaMSI", vdev);
                if (err){
                    pr_err("can't request MSI IRQ %d, channel: %d\n", vector, i);
                    return err;
                }
                else {
                    //vdev->msi = true;
                    if (vdma_debug)
                        pr_info("MSI IRQ=%d channel=%d vector cnt:%d \n", vector, i, n);
                }
            }			   
        }
        vdev->num_msi_irqs = msi_cnt;		
        return 0;
    }

    if (msi_cnt == 16){
        for (i = 0; i < 4; i++){
            for(j = 0; j < 4; j++){
                n = 4*i + j;
                vector = pci_irq_vector(vdev->pdev, n);
                vdev->msi_irq_vec[n] = vector;
                err = request_irq(vector, vdma_irq_handler, 0, "lscvdmaMSI", vdev);
                if (err){
                    pr_err("can't request MSI IRQ %d, channel: %d\n", vector, i);
                    return err;
                }
                else {
                    //vdev->msi = true;
                    if (vdma_debug)
                        pr_info("MSI IRQ=%d channel=%d vector cnt:%d \n", vector, i, n);
                }
            }			   
        }
        vdev->num_msi_irqs = msi_cnt;		
        return 0;
    }

    if (msi_cnt == 32){
        for (i = 0; i < 6; i++){
            for(j = 0; j < 4; j++){
                n = 4*i + j;
                vector = pci_irq_vector(vdev->pdev, n);
                vdev->msi_irq_vec[n] = vector;
                err = request_irq(vector, vdma_irq_handler, 0, "lscvdmaMSI", vdev);
                if (err){
                    pr_err("can't request MSI IRQ %d, channel: %d\n", vector, i);
                    return err;
                }
                else {
                    //vdev->msi = true;
                    if (vdma_debug)
                        pr_info("MSI IRQ=%d channel=%d vector cnt:%d \n", vector, i, n);
                }
            }				
        }
    }
    
    // Custom defined interrupts
    for(i = 24; i <= 28; i++){
        vector = pci_irq_vector(vdev->pdev, i);
        n++;
        vdev->msi_irq_vec[n] = vector;
        err = request_irq(vector, vdma_usr_irq_handler, 0, "lscvdmaMSI_usr", vdev);                          
        if (err) {
            pr_err("can't request usr MSI IRQ %d, id: %d\n", vector, i);
            return err;
        }
        else{
            pr_info("User defined MSI IRQ=%d id: %d vector cnt:%d\n", vector, i, n);
        }
    }
    
    // Abort interrupts
    for(i = 30; i <= 31; i++){
        vector = pci_irq_vector(vdev->pdev, i);
        n++;
        vdev->msi_irq_vec[n] = vector;
        err = request_irq(vector, vdma_irq_handler, 0, "lscvdmaMSI", vdev);                          
        if (err) {
            pr_err("can't request usr MSI IRQ %d, id: %d\n", vector, i);
            return err;
        }
        else{
            pr_info("MSI IRQ=%d id: %d vector cnt:%d\n", vector, i, n);
        }
    }
    vdev->num_msi_irqs = n + 1;
#else
    // Use INTx legacy interrupts
    pr_info("Attach INTx interrupt\n");
    err = request_irq(vdev->pdev->irq, vdma_irq_handler, IRQF_SHARED, "lscvdmaINTx", vdev);
    if (err) {
        pr_err("can't get INTx IRQ %d\n", vdev->irq_line);
        vdev->irq_line = -1;
        return err;
    }
    else {
        vdev->irq_line = vdev->pdev->irq;
        pr_info("INTx IRQ=%d\n", vdev->irq_line);
    }
#endif

    return 0;
}

static int vdma_channel_init(vdma_dev_t *vdev, enum dma_data_direction dir, u8 idx)
{
    struct vdma_channel *vchannel;
    
    if(!vdev){
        pr_err("NULL,inavlid vdev \n");
        return -EINVAL;
    }
	
    if(dir == DMA_FROM_DEVICE)
        vchannel = &vdev->vdma_chan_c2h[idx]; 
    else if (dir == DMA_TO_DEVICE)
        vchannel = &vdev->vdma_chan_h2c[idx];  

    vchannel->dir = dir;
    vchannel->id = idx;
    vchannel->vdev = vdev;
    vchannel->open_flag = 0;
    vchannel->frm_stats = 0;
    vchannel->sge_mode = 0;
    //added by zhang yue on 2023-12-22 start
    vchannel->fb_idx = 0x3C;  //first assign one invalid idx
    vchannel->wb_idx = 0x3C;  //first assign one invalid idx
    //added by zhang yue on 2023-12-22 end
	
    spin_lock_init(&vchannel->lock);             //initialize spin-lock for register access protection
    init_waitqueue_head(&vchannel->wq);
    
    return 0;
}

static int vdma_sgdma_init(vdma_dev_t *vdev)
{   
    int i;
    int err;
    u32 val = 0;
    u8 verMajor = 0x00;
    u8 verMinor = 0x00;

    u8 ip_ver = 0x00;
    u8 ip_width = 0x00;
    
    if (vdma_debug)
        pr_info("enter \n");
        
    if(!vdev){
        pr_err("NULL,inavlid vdev \n");
        return -EINVAL;
    }
    
    val = rdReg32(vdev, SGDMA(REG_VDMA_VER));
    verMajor = (val & VDMA_MAJOR_VER_MASK) >> 28;
    verMinor = (val & VDMA_MINOR_VER_MASK) >> 20;
    
    pr_info("VDMA IP Info Major Version: 0x%x  Minor Version: 0x%x \n", verMajor, verMinor);
    
    if(verMajor != 0x01 || verMinor != 0x06){
    	pr_err("Invalid VDMA IP version, please upgarde to correct version 1.06\n");    
    }
	
    val = rdReg32(vdev, SGDMA(REG_VDMA_CAP));

    ip_ver = val & VDMA_CAP_PCIE_IP_VER_MASK; 
    ip_width = (val & VDMA_CAP_PCIE_IP_WIDTH_MASK) >> 2 ;
    pr_info("VDMA IP actual IP version is Gen%d.\n", ip_ver);
    pr_info("VDMA IP actual IP width is x%d\n ", ip_width ? (ip_width << 1) : (ip_width + 1));

	pr_info("Reset VDMA C2H module \n");
	wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_GLOBAL_RST_MASK, BIT_C2H_CR_GLOBAL_RST);
    msleep(1);
	wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_GLOBAL_RST_MASK, BIT_C2H_CR_GLOBAL_RST_DEASSERT);

	pr_info("Reset VDMA H2C module \n");
	wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_CR), VDMA_H2C_CR_GLOBAL_RST_MASK, BIT_H2C_CR_GLOBAL_RST);
    msleep(1);
	wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_CR), VDMA_H2C_CR_GLOBAL_RST_MASK, BIT_H2C_CR_GLOBAL_RST_DEASSERT);
	
    // diable buffer lock
    wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_ALL_CH_BUF_LOCK_MASK, BIT_C2H_CR_ALL_CH_BUF_LOCK_DIS);
    wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_CR), VDMA_H2C_CR_ALL_CH_BUF_LOCK_MASK, BIT_H2C_CR_ALL_CH_BUF_LOCK_DIS);

    // enable hw drop frame feature
    wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_HW_FRME_DROP_MASK, BIT_C2H_CR_HW_FRME_DROP_EN);
    
    // enable simple round robin
    wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_DMA_CH_CR), VDMA_C2H_DMA_CH_ARB_MASK, BIT_C2H_DMA_CH_ARB_SROUND_ROBIN);
    wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_DMA_CH_CR), VDMA_H2C_DMA_CH_ARB_MASK, BIT_H2C_DMA_CH_ARB_SROUND_ROBIN);

    if(poll_mode)
    {
        wrReg32Modify(vdev, SGDMA(REG_VDMA_C2H_CR), VDMA_C2H_CR_INT_METHOD_MASK,  BIT_C2H_CR_INT_METHOD_POLL);	
        wrReg32Modify(vdev, SGDMA(REG_VDMA_H2C_CR), VDMA_H2C_CR_INT_METHOD_MASK,  BIT_H2C_CR_INT_METHOD_POLL);
    }
    
    for (i = 0; i < vdev->c2h_channel_max; i++){
       err = vdma_channel_init(vdev, DMA_FROM_DEVICE, i);
       if(err){
           return err;
       }
    }

    for (i = 0; i < vdev->h2c_channel_max; i++){
       err = vdma_channel_init(vdev, DMA_TO_DEVICE, i);
       if(err){
           return err;
       }
    }
    
    return 0;
}

static int vdma_dev_bars_map(vdma_dev_t *vdev, struct pci_dev *pdev)
{
    int i;
    pci_dev_bar_t *pBAR, *p;
    
    vdev->numBars = 0;
    for (i = 0; i < NUM_BARS; i++) {
        p = &(vdev->Dev_BARs[i]);
        p->pci_start = pci_resource_start(pdev, i);
        p->pci_end   = pci_resource_end(pdev, i);
        p->len       = pci_resource_len(pdev, i);
        p->pci_flags = pci_resource_flags(pdev, i);
	
        if ((p->pci_start > 0) && (p->pci_end > 0)) {
            ++(vdev->numBars);
            p->bar = i;
            p->pci_addr = (void *)p->pci_start;
            p->memType = p->pci_flags; 							
            pr_info("BAR=%d\n", i);
            pr_info("start=%lx\n", p->pci_start);
            pr_info("end=%lx\n", p->pci_end);
            pr_info("len=0x%lx\n", p->len);
            pr_info("flags=0x%lx\n", p->pci_flags);
        }
    }
	
    // Map the BAR into kernel space so the driver can access registers.
    pBAR = &(vdev->Dev_BARs[vdev->ctrlBAR]);
    if (pBAR->pci_start) {
        vdev->ctrlBARaddr = ioremap(pBAR->pci_start, pBAR->len);
        pBAR->kvm_addr = vdev->ctrlBARaddr;
        if (vdev->ctrlBARaddr) {
            pr_info("map the bar into kernel space success with ioremap\n");
        }
        else {
            pr_err("error with ioremap\n");
            return(-EINVAL);
        }
    }
    else {
        pr_err("error ctrlBAR %d not avail!\n", vdev->ctrlBAR);
        return(-EINVAL);
    }
    
    return 0;
}

static vdma_dev_t* vdma_dev_init(struct pci_dev *pdev, const char *name, u8 h2c_channel_max, u8 c2h_channel_max)
{
    int err;
    vdma_dev_t *vdev = NULL;
    u16 DeviceID, SubSystem;
    
    if (vdma_debug)
        pr_info("enter \n");
        
    if(!pdev){
        pr_err("pdev is null !\n");
        return NULL;
    }
    
    vdev = kzalloc(sizeof(vdma_dev_t), GFP_KERNEL);
    if (!vdev)
        return(NULL);
    
    if (pci_read_config_word(pdev, PCI_DEVICE_ID, &DeviceID)) {
        pr_err("board cfg access failed!\n");
        goto free_vdev;
    }
    if (pci_read_config_word(pdev, PCI_SUBSYSTEM_ID, &SubSystem)) {
        pr_err("board cfg access failed!\n");
        goto free_vdev;
    }
    
    // Start initializing data for new board
    vdev->name = name;
    vdev->ID = DeviceID;
    vdev->demoID = SubSystem;
    vdev->pdev = pdev;
    vdev->irq_line = -1;
    vdev->msi = false;
    vdev->c2h_channel_max = c2h_channel_max;
    vdev->h2c_channel_max = h2c_channel_max;
    vdev->ctrlBAR = 0;
    vdev->c2h_channel_open_cnt = 0;
    vdev->h2c_channel_open_cnt = 0;
    
    err = vdma_dev_bars_map(vdev, pdev);
    if(err){
        pr_err("vdma_dev_bars_map failed!\n");
        goto free_vdev;
    }
    
    err = pci_enable_pcie_error_reporting(pdev);
    if (err && err != -EINVAL)
        pr_err("PCIe AER not available %d \n", err);
    
    // It return zero if card can perofrm DMA properly on the machine given the address mask
    if (sizeof(dma_addr_t) == 8 && !pci_set_dma_mask(pdev, DMA_BIT_MASK(64))) {
        pr_info("set DMA_BIT_MASK(64) ok \n");
        vdev->isDmaAddr64 = true;
    }
    else if(!pci_set_dma_mask(pdev, DMA_BIT_MASK(32))) {
        pr_info("set DMA_BIT_MASK(32) ok \n");
        vdev->isDmaAddr64 = false;
    }
    else {
        pr_err("DMA not supported on this platform!\n");
        pci_disable_pcie_error_reporting(pdev);
        goto err_mask;
    }

    err = vdma_sgdma_init(vdev);
    if(err){
        goto err_sgdma_init;
    }
    
    err = vdma_msi_irq_setup(vdev, pdev);
    if(err){
        goto err_irq;
    }
    
    INIT_WORK(&vdev->c2h_work, c2h_channel_service_work);
    INIT_WORK(&vdev->h2c_work, h2c_channel_service_work);
    
    return vdev;

err_sgdma_init:
err_irq:    
err_mask:
    if (vdev->ctrlBARaddr)
        iounmap(vdev->ctrlBARaddr);
free_vdev:
    kfree(vdev);
    
    return NULL;  
}

static int lscvdma_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    int i, err;
    static char devNameStr[12] = "lscvdma__";
    struct vdma_pci_dev *vpdev;
    struct vdma_channel *vchannel;
    vdma_dev_t *vdev;
    //vdma_channel_t *vchannel;

    vpdev = kmalloc(sizeof(*vpdev), GFP_KERNEL);
    if(!vpdev)
        return -ENOMEM;
    memset(vpdev, 0, sizeof(*vpdev));
    
    vpdev->pdev = pdev;
    vpdev->c2h_channel_max = C2H_CHANNEL_MAX;
    vpdev->h2c_channel_max = H2C_CHANNEL_MAX;
    
    devNameStr[8] = '0' + g_boards_num;
    if (vdma_debug)
        pr_info("pci probe for: %s  pdev=%p  vpdev=%p ent=%p\n", devNameStr, pdev, vpdev, ent);
        
    err = pci_request_regions(pdev, devNameStr);
    if (err)
        return err;

    pci_set_master(pdev);

    err = pci_enable_device(pdev);
    if (err)
        return err;
        
    vdev = vdma_dev_init(pdev, DRV_MODULE_NAME, vpdev->h2c_channel_max, vpdev->c2h_channel_max);
    if (vdev == NULL) {
        pr_err("error initializing vdma device.\n");
        // Clean up any resources we acquired along the way
        pci_release_regions(pdev);
        pci_disable_device(pdev);
        return(-EINVAL);
    }
    vpdev->vdev = vdev;
    
    for (i = 0; i < vpdev->c2h_channel_max; i++){
        vchannel = &vdev->vdma_chan_c2h[i];
        vdma_cdev_create(vpdev, &vpdev->vdma_c2h_dev[i], vchannel, CHAR_VDMA_C2H);
    }
    
    for (i = 0; i < vpdev->h2c_channel_max; i++){
        vchannel = &vdev->vdma_chan_h2c[i];
        vdma_cdev_create(vpdev, &vpdev->vdma_h2c_dev[i], vchannel, CHAR_VDMA_H2C);
    }
    
    dev_set_drvdata(&pdev->dev, vpdev);
    g_boards_num++;
    
    return 0;
}

static void lscvdma_remove(struct pci_dev *pdev)
{
    int i;
    struct vdma_pci_dev *vpdev;
    vdma_dev_t *vdev;
    
    vpdev = dev_get_drvdata(&pdev->dev);
    if(!vpdev)
        return;
    pr_info("pci remove for device: pdev=%p vpdev=%p\n", pdev, vpdev);     
    vdev = vpdev->vdev;
    
    // Disable PCIe error reporting		
    pci_disable_pcie_error_reporting(pdev);
    
    for(i = 0; i < vpdev->c2h_channel_max; i++){
        vdma_cdev_destroy(&(vpdev->vdma_c2h_dev[i]));
    }
    
    for(i = 0; i < vpdev->h2c_channel_max; i++){
        vdma_cdev_destroy(&(vpdev->vdma_h2c_dev[i]));
    }
    
    if(vpdev->majorNum){
        unregister_chrdev_region(MKDEV(vpdev->majorNum, 0), MAX_MINORS);
    }
    
#ifdef MSI
    if (vdev->msi){
        for (i = 0; i < vdev->num_msi_irqs; i++) {
            free_irq(vdev->msi_irq_vec[i], vdev);
        }
        pci_free_irq_vectors(vdev->pdev);
        vdev->msi = false;
        vdev->num_msi_irqs = 0;
    }
#else
    if (vdev->irq_line != -1) {
        pr_info(" close - free_irq %d\n", vdev->irq_line);
        free_irq(vdev->irq_line, vdev);
    }
    vdev->irq_line = -1;
#endif

    // Free our internal access to the control BAR address space
    if (vdev->ctrlBARaddr)
        iounmap(vdev->ctrlBARaddr);
    vpdev->vdev = NULL;
 
    // No more access after this call
    pci_release_regions(pdev);
    
    kfree(vdev);
    kfree(vpdev);
    
    dev_set_drvdata(&pdev->dev, NULL);
    g_boards_num--;
}

static int lscvdma_suspend(struct pci_dev *pdev, pm_message_t state)
{
    pr_info("suspend\n");
    return 0;
}

static int lscvdma_resume(struct pci_dev *pdev)
{
    pr_info("resume \n");
    return 0;
}

static void lscvdma_shutdown(struct pci_dev *pdev)
{
    pr_info("shutdown \n");
}

static pci_ers_result_t lscvdma_error_detected (struct pci_dev *pdev, pci_channel_state_t state)
{
    pci_channel_state_t s;
  
    s = state;
    switch (s)
    {
        case pci_channel_io_normal:
            return PCI_ERS_RESULT_CAN_RECOVER;
      
        case pci_channel_io_frozen:
            pr_warn(" dev 0x%p, frozen state error, reset controller \n", pdev);
            pci_disable_device(pdev);
            return PCI_ERS_RESULT_NEED_RESET;
      
        case pci_channel_io_perm_failure:
            pr_warn("dev 0x%p, failure state error, request disconnect \n", pdev);
            return PCI_ERS_RESULT_DISCONNECT;
    }
  
    return PCI_ERS_RESULT_NEED_RESET;
}

static pci_ers_result_t lscvdma_slot_reset(struct pci_dev *pdev)
{
    pr_info("0x%p restart after slot reset \n", pdev);
    if(pci_enable_device_mem(pdev)) {
        pr_err(" failed to reenable after slot reset \n");
        return PCI_ERS_RESULT_DISCONNECT;
    }
    pci_set_master(pdev);
    pci_restore_state(pdev);
    pci_save_state(pdev);
  
    return PCI_ERS_RESULT_RECOVERED;
}

static const struct pci_error_handlers lscvdma_err_handler = {
    .error_detected = lscvdma_error_detected,
    .slot_reset = lscvdma_slot_reset,
    //.resume,
    //.reset_prepare,
    //.reset_done,
};

static struct pci_driver lscvdma_driver = {
    .name = "lscvdma",
    .id_table = lscvdma_pci_id_tbl,
    .probe = lscvdma_probe,
    .remove = lscvdma_remove,
    .suspend = lscvdma_suspend,
    .resume = lscvdma_resume,
    .shutdown = lscvdma_shutdown,
    .err_handler = &lscvdma_err_handler,
};

static int __init lscvdma_init(void)
{
    int err;
  
    pr_info("vdma_debug=%d, poll mode=%d\n", vdma_debug, poll_mode);
    pr_info("%s\n", version);

    /* Create the new sysfs Class entry that will hold the tree of detected PCIe board devices. */
    glscvdma_class = class_create(THIS_MODULE, "lscvdma");
    if (IS_ERR(glscvdma_class)) {
        pr_err("Error creating simple class interface\n");
        return(-1);	
    }
    
    if (vdma_debug)
        pr_info("registering driver with PCI\n");


	
    /* Register our PCI components and functions with the Kernel PCI core.*/
    err = pci_register_driver(&lscvdma_driver);
    if (err < 0)
        return(err);
	
    #if (WRITE_SG_LIST_INFO_TO_FILE)
    log_file = filp_open(FILE_PATH, O_WRONLY | O_CREAT | O_APPEND, 0666);  
    //log_file = filp_open(FILE_PATH, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (!log_file || IS_ERR(log_file) || !log_file->f_op || !log_file->f_op->write) { 
        if(!log_file) {
            printk(KERN_ERR "log file is NULL \n");  
            return 0; 
        }else if(IS_ERR(log_file)) {
            printk(KERN_ERR "log file ERROR \n");  
            return 0;         
        }else if(!log_file->f_op){
            printk(KERN_ERR "log_file->f_op is NULL \n");  
            return 0;       
        }else if(!(log_file->f_mode & FMODE_CAN_WRITE)){
            printk(KERN_ERR "log_file not writeable \n");  
            return 0;         
        }
    }  
    #endif	
    return 0;
}

module_init(lscvdma_init);

/**
 * Driver clean-up.
 * Called when module is unloaded by kernel or rmmod
 */
static void __exit lscvdma_exit(void)
{
    pr_info("%s.\n", __func__);

    pci_unregister_driver(&lscvdma_driver);
    if (glscvdma_class){
        class_destroy(glscvdma_class);
    }
	
#if (WRITE_SG_LIST_INFO_TO_FILE)
    if (log_file != NULL && !IS_ERR(log_file)) {  
        filp_close(log_file, NULL);  
        log_file = NULL;  
    }   
#endif	
    return;
}

module_exit(lscvdma_exit);

MODULE_AUTHOR("Lattice Semiconductor");
MODULE_DESCRIPTION("LSC PCIe VDMA Device Driver");
MODULE_VERSION(DRV_MODULE_VERSION);

/* License this so no annoying messages when loading module */
//MODULE_LICENSE("Dual BSD/GPL");
MODULE_LICENSE("GPL v2");

MODULE_ALIAS("platform:lscvdma");
