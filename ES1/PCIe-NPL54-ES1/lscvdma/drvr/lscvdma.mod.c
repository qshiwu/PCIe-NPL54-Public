#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x32e21920, "module_layout" },
	{ 0x4a165127, "kobject_put" },
	{ 0x2d3385d3, "system_wq" },
	{ 0x19d09ac9, "dma_map_sg_attrs" },
	{ 0x9fb74541, "kernel_write" },
	{ 0xc7466e35, "cdev_del" },
	{ 0x26087692, "kmalloc_caches" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0x320c3a3f, "cdev_init" },
	{ 0xf9a482f9, "msleep" },
	{ 0x27012063, "put_devmap_managed_page" },
	{ 0x822531c7, "pci_free_irq_vectors" },
	{ 0x59d2a97, "param_ops_int" },
	{ 0x754d539c, "strlen" },
	{ 0x54b1fac6, "__ubsan_handle_load_invalid_value" },
	{ 0x78a31f11, "pci_read_config_byte" },
	{ 0xbc3c631b, "dma_unmap_sg_attrs" },
	{ 0x7eae295f, "dma_set_mask" },
	{ 0xfb14ff3c, "pci_disable_device" },
	{ 0x56470118, "__warn_printk" },
	{ 0x8ffcddf7, "device_destroy" },
	{ 0x15a66331, "kobject_set_name" },
	{ 0x9e2fb826, "filp_close" },
	{ 0x87b8798d, "sg_next" },
	{ 0xe51ce86c, "get_user_pages_unlocked" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0xf288fcdc, "pci_release_regions" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x87cec3d1, "dma_free_attrs" },
	{ 0xb3378a7b, "pv_ops" },
	{ 0x3c32d274, "set_page_dirty" },
	{ 0x2da00799, "kthread_create_on_node" },
	{ 0xd9a5ea54, "__init_waitqueue_head" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x1e323c29, "pci_set_master" },
	{ 0x5f49d55d, "pci_alloc_irq_vectors_affinity" },
	{ 0xc18f69f3, "pci_enable_pcie_error_reporting" },
	{ 0x8304bd67, "sg_alloc_table_from_pages_segment" },
	{ 0x22a9e612, "pci_restore_state" },
	{ 0x85c71d85, "kthread_stop" },
	{ 0xde80cd09, "ioremap" },
	{ 0x29a90398, "pci_read_config_word" },
	{ 0x1d0107ac, "dma_alloc_attrs" },
	{ 0x1e6d26a8, "strstr" },
	{ 0x3e4bbd69, "device_create" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x6a0a71c7, "cdev_add" },
	{ 0x800473f, "__cond_resched" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0xa916b694, "strnlen" },
	{ 0xd0da656b, "__stack_chk_fail" },
	{ 0x8ddd8aad, "schedule_timeout" },
	{ 0x1000e51, "schedule" },
	{ 0x8427cc7b, "_raw_spin_lock_irq" },
	{ 0x92997ed8, "_printk" },
	{ 0x65487097, "__x86_indirect_thunk_rax" },
	{ 0x45937b6f, "wake_up_process" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0x233f929e, "pci_unregister_driver" },
	{ 0xf35141b2, "kmem_cache_alloc_trace" },
	{ 0xba8fbd64, "_raw_spin_lock" },
	{ 0x148653, "vsnprintf" },
	{ 0x696f32fb, "pci_irq_vector" },
	{ 0x3eeb2322, "__wake_up" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0xf5efb42f, "pci_disable_pcie_error_reporting" },
	{ 0x37a0cba, "kfree" },
	{ 0x6df1aaf1, "kernel_sigaction" },
	{ 0x95b52781, "pci_request_regions" },
	{ 0xedc03953, "iounmap" },
	{ 0xf2bb7d9, "__pci_register_driver" },
	{ 0x748154da, "class_destroy" },
	{ 0x92540fbf, "finish_wait" },
	{ 0x7f5b4fe4, "sg_free_table" },
	{ 0xc5b6f236, "queue_work_on" },
	{ 0x656e4a6e, "snprintf" },
	{ 0xa8528b64, "pci_enable_device_mem" },
	{ 0x7f02188f, "__msecs_to_jiffies" },
	{ 0x8d929026, "pci_enable_device" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x70a3a844, "param_ops_uint" },
	{ 0xaf919f03, "__class_create" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xb915ba86, "__put_page" },
	{ 0xc1514a3b, "free_irq" },
	{ 0x6e053cc4, "pci_save_state" },
	{ 0xa0c4c91b, "filp_open" },
	{ 0x587f22d7, "devmap_managed_key" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "01709A295A3EF8F957DB9C4");
