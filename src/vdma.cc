/*
 * vdma.cc
 *
 *  Created on: Jan 23, 2018
 *      Author: pjurgiel
 */

#include "../include/vdma.h"

int VDMA::Init(){
	int Status;
	XAxiVdma_Config *Config;

	/* The information of the XAxiVdma_Config comes from hardware build.
	 * The user IP should pass this information to the AXI DMA core.
	 */
	Config = XAxiVdma_LookupConfig(DMA_DEVICE_ID);
	if (!Config)
	{
		xil_printf("No video DMA found for ID %d\r\n", DMA_DEVICE_ID);
		return XST_FAILURE;
	}

	/* Initialize DMA engine */
	Status = XAxiVdma_CfgInitialize(&AxiVdma, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Configuration Initialization failed %d\r\n", Status);
		return XST_FAILURE;
	}

	/* Setup the read channel
	 */
	Status = FrameSetup(&AxiVdma);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Read\\Write channel setup failed %d\r\n", Status);
		if (Status == XST_VDMA_MISMATCH_ERROR)
		{
			xil_printf("DMA Mismatch Error\r\n");
		}
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int VDMA::Start()
{
	int Status = StartTransfer(&AxiVdma);
	if (Status != XST_SUCCESS)
	{
		if (Status == XST_VDMA_MISMATCH_ERROR)
		{
			xil_printf("DMA Mismatch Error\r\n");
		}
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int VDMA::FrameSetup(XAxiVdma *InstancePtr) {
	int Index;
	u32 Addr;
	int Status;

	ReadCfg.VertSizeInput 			= SUBFRAME_VERTICAL_SIZE;
	ReadCfg.HoriSizeInput 			= SUBFRAME_HORIZONTAL_SIZE;

	ReadCfg.Stride 					= FRAME_HORIZONTAL_LEN;
	ReadCfg.FrameDelay 				= 0; /* This example does not test frame delay */

	ReadCfg.EnableCircularBuf 		= 1;
	ReadCfg.EnableSync 				= 0; /* No Gen-Lock */

	ReadCfg.PointNum 				= 0; /* No Gen-Lock */
	ReadCfg.EnableFrameCounter 		= 0; /* Endless transfers */

	ReadCfg.FixedFrameStoreAddr 	= 0; /* We are not doing parking */

	WriteCfg.VertSizeInput 			= SUBFRAME_VERTICAL_SIZE;
	WriteCfg.HoriSizeInput 			= SUBFRAME_HORIZONTAL_SIZE;

	WriteCfg.Stride 				= FRAME_HORIZONTAL_LEN;
	WriteCfg.FrameDelay 			= 0; /* This example does not test frame delay */

	WriteCfg.EnableCircularBuf 		= 1;
	WriteCfg.EnableSync 			= 0; /* No Gen-Lock */

	WriteCfg.PointNum 				= 0; /* No Gen-Lock */
	WriteCfg.EnableFrameCounter 	= 0; /* Endless transfers */

	WriteCfg.FixedFrameStoreAddr 	= 0; /* We are not doing parking */

	Status = XAxiVdma_DmaConfig(InstancePtr, XAXIVDMA_READ, &ReadCfg);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Read channel config failed %d\r\n", Status);
		return XST_FAILURE;
	}

	Status = XAxiVdma_DmaConfig(InstancePtr, XAXIVDMA_WRITE, &WriteCfg);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Write channel config failed %d\r\n", Status);
		return XST_FAILURE;
	}

	/* Initialize buffer addresses
	 *
	 * These addresses are physical addresses
	 */
	Addr = READ_ADDRESS_BASE + BlockStartOffset;
	for (Index = 0; Index < NUMBER_OF_READ_FRAMES; ++Index)
	{
		ReadCfg.FrameStoreStartAddr[Index] = Addr;
		//Addr += FRAME_HORIZONTAL_LEN * FRAME_VERTICAL_LEN;
	}

	Addr = WRITE_ADDRESS_BASE + BlockStartOffset;
	for (Index = 0; Index < NUMBER_OF_WRITE_FRAMES; ++Index)
	{
		WriteCfg.FrameStoreStartAddr[Index] = Addr;
		//Addr += FRAME_HORIZONTAL_LEN * FRAME_VERTICAL_LEN;
	}

	/* Set the buffer addresses for transfer in the DMA engine
	 * The buffer addresses are physical addresses
	 */
	Status = XAxiVdma_DmaSetBufferAddr(InstancePtr, XAXIVDMA_READ, ReadCfg.FrameStoreStartAddr);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Read channel set buffer address failed %d\r\n", Status);
		return XST_FAILURE;
	}

	Status = XAxiVdma_DmaSetBufferAddr(InstancePtr, XAXIVDMA_WRITE,	WriteCfg.FrameStoreStartAddr);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Write channel set buffer address failed %d\r\n", Status);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int VDMA::StartTransfer(XAxiVdma *InstancePtr) {
	int Status;

	Status = XAxiVdma_DmaStart(InstancePtr, XAXIVDMA_READ);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Start read transfer failed %d\r\n", Status);
		return XST_FAILURE;
	}

	Status = XAxiVdma_DmaStart(InstancePtr, XAXIVDMA_WRITE);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Start write transfer failed %d\r\n", Status);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
