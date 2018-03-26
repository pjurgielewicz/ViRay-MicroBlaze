/*
 * vdma.h
 *
 *  Created on: Jan 23, 2018
 *      Author: pjurgiel
 */

#ifndef SRC_VDMA_H_
#define SRC_VDMA_H_

/******************************************************************************
 *
 * (c) Copyright 2012-2013 Xilinx, Inc. All rights reserved.
 *
 * This file contains confidential and proprietary information of Xilinx, Inc.
 * and is protected under U.S. and international copyright and other
 * intellectual property laws.
 *
 * DISCLAIMER
 * This disclaimer is not a license and does not grant any rights to the
 * materials distributed herewith. Except as otherwise provided in a valid
 * license issued to you by Xilinx, and to the maximum extent permitted by
 * applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
 * FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
 * IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
 * and (2) Xilinx shall not be liable (whether in contract or tort, including
 * negligence, or under any other theory of liability) for any loss or damage
 * of any kind or nature related to, arising under or in connection with these
 * materials, including for any direct, or any indirect, special, incidental,
 * or consequential loss or damage (including loss of data, profits, goodwill,
 * or any type of loss or damage suffered as a result of any action brought by
 * a third party) even if such damage or loss was reasonably foreseeable or
 * Xilinx had been advised of the possibility of the same.
 *
 * CRITICAL APPLICATIONS
 * Xilinx products are not designed or intended to be fail-safe, or for use in
 * any application requiring fail-safe performance, such as life-support or
 * safety devices or systems, Class III medical devices, nuclear facilities,
 * applications related to the deployment of airbags, or any other applications
 * that could lead to death, personal injury, or severe property or
 * environmental damage (individually and collectively, "Critical
 * Applications"). Customer assumes the sole risk and liability of any use of
 * Xilinx products in Critical Applications, subject only to applicable laws
 * and regulations governing limitations on product liability.
 *
 * THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
 * AT ALL TIMES.
 *
 ******************************************************************************/
/*****************************************************************************/
/**
 *
 * @file xaxivdma_example_intr.c
 *
 * This example demonstrates how to use the AXI Video DMA with other video IPs
 * to do video frame transfers. This example does not work by itself. It needs
 * two other Video IPs, one for writing video frames to the memory and one for
 * reading video frames from the memory.
 *
 * To see the debug print, you need a Uart16550 or uartlite in your system,
 * and please set "-DDEBUG" in your compiler options. You need to rebuild your
 * software executable.
 *
 * @note
 * The values of DDR_BASE_ADDR and DDR_HIGH_ADDR should be as per the HW system.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- -------------------------------------------------------
 * 1.00a jz   07/26/10 First release
 * 1.01a jz   09/26/10 Updated callback function signature
 * 2.00a jz   12/10/10 Added support for direct register access mode, v3 core
 * 2.01a rvp  01/22/11 Renamed the example file to be consistent
 * 		       Added support to the example to use SCU GIC interrupt
 *		       controller for ARM, some functions in this example have
 *		       changed.
 *       rkv  03/28/11 Updated to support for frame store register.
 * 3.00a srt  08/26/11 Added support for Flush on Frame Sync Feature.
 * 4.00a srt  03/06/12 Modified interrupt support for Zynq.
 * 4.02a srt  09/25/12 Fixed CR 677704
 *		       Description - Arguments misused in function
 *                     XAxiVdma_IntrEnable().
 * 4.03a srt  03/01/13 Updated DDR base address for IPI designs (CR 703656).
 * </pre>
 *
 * ***************************************************************************
 */

#include "xaxivdma.h"
#include "xparameters.h"
#include "xil_exception.h"

#include "../include/worldDescription.h"

/******************** Constant Definitions **********************************/

/*
 * Device related constants. These need to defined as per the HW system.
 */
#define DMA_DEVICE_ID		XPAR_AXIVDMA_0_DEVICE_ID

#define DDR_BASE_ADDR		XPAR_MIG_0_BASEADDR
#define DDR_HIGH_ADDR	 	XPAR_MIG_0_HIGHADDR

/* Memory space for the frame buffers
 *
 * This example only needs one set of frame buffers, because one video IP is
 * to write to the frame buffers, and the other video IP is to read from the
 * frame buffers.
 *
 * For 16 frames of 1080p, it needs 0x07E90000 memory for frame buffers
 */
#define MEM_BASE_ADDR		((unsigned)OUT_COLOR_ADDR)
#define MEM_HIGH_ADDR		DDR_HIGH_ADDR
#define MEM_SPACE		    (MEM_HIGH_ADDR - MEM_BASE_ADDR)

/* Read channel and write channel start from the same place
 *
 * One video IP write to the memory region, the other video IP read from it
 */
#define READ_ADDRESS_BASE	MEM_BASE_ADDR
#define WRITE_ADDRESS_BASE	MEM_BASE_ADDR

/* Frame size related constants
 */
#define FRAME_HORIZONTAL_LEN  (4 * WIDTH)   /* 1920 pixels, each pixel 4 bytes */
#define FRAME_VERTICAL_LEN    (HEIGHT)    /* 1080 pixels */

/* Subframe to be transferred by Video DMA
 *
 *|<----------------- FRAME_HORIZONTAL_LEN ---------------------->|
 * --------------------------------------------------------------------
 *|                                                                | ^
 *|                                                                | |
 *|               |<-SUBFRAME_HORIZONTAL_SIZE ->|                  | FRAME_
 *|               -----------------------------------              | VERTICAL_
 *|               |/////////////////////////////|  ^               | LEN
 *|               |/////////////////////////////|  |               | |
 *|               |/////////////////////////////|  |               | |
 *|               |/////////////////////////////| SUBFRAME_        | |
 *|               |/////////////////////////////| VERTICAL_        | |
 *|               |/////////////////////////////| SIZE             | |
 *|               |/////////////////////////////|  |               | |
 *|               |/////////////////////////////|  v               | |
 *|                ----------------------------------              | |
 *|                                                                | v
 *--------------------------------------------------------------------
 *
 * Note that SUBFRAME_HORIZONTAL_SIZE and SUBFRAME_VERTICAL_SIZE must ensure
 * to be inside the frame.
 */
#define SUBFRAME_START_OFFSET    0
#define SUBFRAME_HORIZONTAL_SIZE FRAME_HORIZONTAL_LEN
#define SUBFRAME_VERTICAL_SIZE   FRAME_VERTICAL_LEN

/* Number of frames to work on, this is to set the frame count threshold
 *
 * We multiply 15 to the num stores is to increase the intervals between
 * interrupts. If you are using fsync, then it is not necessary.
 */
#define NUMBER_OF_READ_FRAMES	2
#define NUMBER_OF_WRITE_FRAMES	2


/******************* Function Prototypes ************************************/

class VDMA{
public:
	VDMA() : 	ReadFrameAddr(READ_ADDRESS_BASE),
				BlockStartOffset(SUBFRAME_START_OFFSET),
				BlockHoriz(SUBFRAME_HORIZONTAL_SIZE),
				BlockVert(SUBFRAME_VERTICAL_SIZE){}

	/*****************************************************************************/
	/**
	 *
	 * Main function
	 *
	 * This function is the main entry point of the example on DMA core. It sets up
	 * DMA engine to be ready to receive and send frames, and start the transfers.
	 * It waits for the transfer of the specified number of frame sets, and check
	 * for transfer errors.
	 *
	 * @return
	 *		- XST_SUCCESS if example finishes successfully
	 *		- XST_FAILURE if example fails.
	 *
	 * @note		None.
	 *
	 ******************************************************************************/
	int Init();
	int Start();

private:

	/*****************************************************************************/
	/**
	 *
	 * This function sets up the read channel
	 *
	 * @param	InstancePtr is the instance pointer to the DMA engine.
	 *
	 * @return	XST_SUCCESS if the setup is successful, XST_FAILURE otherwise.
	 *
	 * @note		None.
	 *
	 ******************************************************************************/
	int FrameSetup(XAxiVdma *InstancePtr);

	/*****************************************************************************/
	/**
	 *
	 * This function starts the DMA transfers. Since the DMA engine is operating
	 * in circular buffer mode, video frames will be transferred continuously.
	 ******************************************************************************/
	int StartTransfer(XAxiVdma *InstancePtr);

	/*
	 * Device instance definitions
	 */
	XAxiVdma AxiVdma;

	/* DMA channel setup
	 */
	XAxiVdma_DmaSetup ReadCfg;
	XAxiVdma_DmaSetup WriteCfg;

	/* Data address
	 *
	 * Read and write sub-frame use the same settings
	 */
	const u32 ReadFrameAddr;
	const u32 BlockStartOffset;
	const u32 BlockHoriz;
	const u32 BlockVert;
};


#endif /* SRC_VDMA_H_ */
