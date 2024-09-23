/*
 * This file is part of the Lattice Video DMA IP Core driver for Linux
 *
 *  COPYRIGHT (c) 2022 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#ifndef __VDMA_VERSION_H__
#define __VDMA_VERSION_H__

#define DRV_MODULE_NAME		"lscvdma"
#define DRV_MODULE_DESC		"Lattice PCIe VDMA Device Driver"

#define DRV_MOD_MAJOR		1
#define DRV_MOD_MINOR		06
#define DRV_MOD_MINOR_UPDTAE		0
#define DRV_MOD_PATCHLEVEL	0015

#define DRV_MODULE_VERSION      \
	__stringify(DRV_MOD_MAJOR) "." \
	__stringify(DRV_MOD_MINOR) "." \
	__stringify(DRV_MOD_MINOR_UPDTAE) "." \
	__stringify(DRV_MOD_PATCHLEVEL)

static char version[] =
	DRV_MODULE_DESC " " DRV_MODULE_NAME " v" DRV_MODULE_VERSION "\n";


#endif  /* ifndef __VDMA_VERSION_H__ */

