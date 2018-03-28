/*
 * adv7511_setup.cc
 *
 *  Created on: Jan 23, 2018
 *      Author: pjurgiel
 */

#include "../include/adv7511_setup.h"


u8 hdmi_iic[][2] = {

	{ 0x01, 0x00 },
	{ 0xD6, 0xC0 },
	{ 0x02, 0x18 },
	{ 0x03, 0x00 },
	{ 0x15, /*0x00*/ 0x01 },	// 16, 20, 24 bit YCbCr 4:2:2 (separate syncs)
	{ 0x16, 0x3C + 0x01 },	// Output Format -> 0 = 4:4:4, Color Depth  -> 11 = 8 bit, Input Style -> 11 = style 3
	{	0x17, 0x02 + 0x04},		// 4:2:2 to 4:4:4 Up Conversion Method -> 1 = use first order interpolation, Aspect Ratio -> 1 = 16:9 Aspect Ratio

	/*
	* CSC
	*/
	{ 0x18, 0xA8 },
	{ 0x19, 0x00 },
	{ 0x1A, 0x00 },
	{ 0x1B, 0x00 },
	{ 0x1C, 0x00 },
	{ 0x1D, 0x00 },
	{ 0x1E, 0x00 },
	{ 0x1F, 0x00 },

	{ 0x20, 0x00 },
	{ 0x21, 0x00 },
	{ 0x22, 0x08 },
	{ 0x23, 0x00 },
	{ 0x24, 0x00 },
	{ 0x25, 0x00 },
	{ 0x26, 0x00 },
	{ 0x27, 0x00 },

	{ 0x28, 0x00 },
	{ 0x29, 0x00 },
	{ 0x2A, 0x00 },
	{ 0x2B, 0x00 },
	{ 0x2C, 0x08 },
	{ 0x2D, 0x00 },
	{ 0x2E, 0x00 },
	{ 0x2F, 0x00 },
	/*
	* !CSC
	*/

	{ 0x40, 0x00 },
	{ 0x41, 0x10 },
	{ 0x48, 0x08 /*+ 0x40*/},	// Video Input Justification -> 01 = right justified
	{ 0x49, 0xA8 },
	{ 0x4C, 0x00 },
	{ 0x55, 0x00 },
	{ 0x56, 0x28 },
	{ 0x96, 0x20 },
	{ 0x98, 0x03 },
	{ 0x99, 0x02 },
	{ 0x9A, 0xE0 },
	{ 0x9C, 0x30 },
	{ 0x9D, 0x61 },
	{ 0xA2, 0xA4 },
	{ 0xA3, 0xA4 },
	{ 0xA5, 0x44 },
	{ 0xAB, 0x40 },
	{ 0xAF, 0x04 },
	//{ 0x39, 0xBA, /*0xA0*/0x30},
	{ 0xD0, 0x3C },
	{ 0xD1, 0xFF },
	{ 0xDE, 0x9C },
	{ 0xE0, 0xD0 },
	{ 0xE4, 0x60 },
	{ 0xF9, 0x00 },
	{ 0x3C, 0x01 }
};

u8 hdmi_iic_kcu116_refDes[][2] = {
	{0x01, 0x00},
	{0x02, 0x18},
	{0x03, 0x00},
	{0x15, 0x01},
	{0x16, 0x39 + 0x04},		// (!) ORIGINALLY WAS : B9 (different input style & output not converted to 444 (422))

	/*
	 * CSC
	 */
	{0x18, 0x46},				// CSC disabled
	{0x19, 0x34},
	{0x1A, 0x04},
	{0x1B, 0xAD},
	{0x1C, 0x00},
	{0x1D, 0x00},
	{0x1E, 0x1C},
	{0x1F, 0x1B},

	{0x20, 0x1D},
	{0x21, 0xDC},
	{0x22, 0x04},
	{0x23, 0xAD},
	{0x24, 0x1F},
	{0x25, 0x24},
	{0x26, 0x01},
	{0x27, 0x35},

	{0x28, 0x00},
	{0x29, 0x00},
	{0x2A, 0x04},
	{0x2B, 0xAD},
	{0x2C, 0x08},
	{0x2D, 0x7C},
	{0x2E, 0x1B},
	{0x2F, 0x77},
	/*
	 * !CSC
	 */

	{0x40, 0x80},		// GC Packet enabled, to disable: 0x00
	{0x41, 0x10},
	{0x48, 0x08},
	{0x49, 0xA8},
	{0x4C, 0x00},
	{0x55, 0x20},		// 0x00 - RGB, 0x20 - 422, 0x40 - 444
	{0x56, 0x08 + 0x20},// ADDED: 0x20 (16:9)
	{0x96, 0x20},
	{0x98, 0x03},
	{0x99, 0x02},
	{0x9A, 0xE0},
	{0x9C, 0x30},
	{0x9D, 0x61},
	{0xA2, 0xA4},
	{0xA3, 0xA4},
	{0xA5, 0x44},		//???: 0xA5 R/W [7:1] 0000010* Fixed Must be set to Default Value
	{0xAB, 0x40},
	{0xAF, 0x06},
	{0xBA, 0xA0},
	{0xD0, 0x3C},
	{0xD1, 0xFF},
	{0xD6, 0xC0},		// WAS NOT HERE ORIGINALLY
	{0xDE, 0x9C},		// ??? : [7:4] 0001**** Fixed Must be set to Default for proper operation, [2:0] *****000 Fixed Must be set to Default for proper operation
	{0xE0, 0xD0},
	{0xE4, 0x60},
	{0xF9, 0x00}
};

/*
 * https://forums.xilinx.com/t5/Embedded-Processor-System-Design/Problem-with-ADV7511-video-out-display/td-p/429736
 */
u8 hdmi_iic_internet_tip[][2] = {
	{0x41, 0x10}, //Power Up the txr.
	{0x98, 0x03}, //ADI Recommended write_7511
	{0x9a, 0xe0}, //ADI Recommended write_7511
	{0x9c, 0x30}, //ADI Recommended write_7511
	{0x9d, 0x61}, //ADI Recommended write_7511
	{0xa2, 0xa4}, //ADI Recommended write_7511
	{0xa3, 0xa4}, //ADI Recommended write_7511
	{0xe0, 0xd0}, //ADI Recommended write_7511
	{0xf9, 0x00}, //ADI Recommended write_7511
	{0xfa, 0x00}, //Set to default 0x00
	{0xe4, 0x60}, //Set to default 0x60
	{0xf9, 0x00}, //I2C Address to recomended 0x00
	{0xde, 0x10}, //No TMDS Clk Inversion
	{0xd1, 0xff}, //Set to default 0x00
	{0xBA, 0x60}, //No clk delay
	{0x56, 0x28}, //16:9 Aspect ratio to out put
	{0x48, 0x08}, //Right Justified
	{0x15, 0x01}, //I/P Format 4:2:2 Seperate syncs
	{0x16, 0x38}, // O/P Format 4:4:4  RGB// For George Modified to 0x34
	{0x17, 0x02}, // Color Depth= 8
				  // Style 2
				  // For Style 1 = 0x38
				  // For Style 2 = 0x34
				  // For Style 3 = 0x3c
				  // I/P Aspect ratio 16:9
	////////////
	{0x18, 0xE7},
	{0x19, 0x34},
	{0x1A, 0x04},
	{0x1B, 0xAD},
	{0x1C, 0x00},
	{0x1D, 0x00},
	{0x1E, 0x1C},
	{0x1F, 0x1B},
	{0x20, 0x1D},
	{0x21, 0xDC},
	{0x22, 0x04},
	{0x23, 0xAD},
	{0x24, 0x1F},
	{0x25, 0x24},
	{0x26, 0x01},
	{0x27, 0x35},
	{0x28, 0x00},
	{0x29, 0x00},
	{0x2A, 0x04},
	{0x2B, 0xAD},
	{0x2C, 0x08},
	{0x2D, 0x7C},
	{0x2E, 0x1B},
	{0x2F, 0x77},

	{0x56, 0x08}, //16:9 Aspect ratio to out put
	{0xaf, 0x04}, //HDMI Mode - 6
	{0x40, 0x80}, //GC Packet Enabled
	{0x4c, 0x04}, //O/p Color depth  24
	{0x55, 0x00}  //o/p Format RGB format
};


/*
 * CONFIGURATION DATA FROM XIIC_HDMI_CONF
 */
u8 hdmi_iic_other[][2] = {
	{ 0xD6, 0xC0 },
	{ 0x41, 0x10 },
	{ 0x98, 0x03 },
	{ 0x9A, 0xE0 }, // 0xE0 ----- ***
	{ 0x9C, 0x30 },
	{ 0x9D, 0x61 },
	{ 0xA2, 0xA4 },
	{ 0xA3, 0xA4 },
	{ 0xE0, 0xD0 },
	{ 0xF9, 0x00 },
	{ 0x15, 0x00 }, //0x01
	{ 0x16, 0x30 }, //0xB9
	{ 0x18, 0x66 }, //0x66
	{ 0xAF, 0x06 }, // hdmi dvi mode (bit [1] 0= dvi, 1=hdmi)
	{ 0x48, 0x00 }, //08
	{ 0xDE, 0x18 }, //18
	{ 0xBA, 0x00 },
	{ 0x56, 0x10 },
	{ 0x55, 0x40 }
};

/*
 * https://ez.analog.com/thread/87252-settings-of-adv7612adv7511-and-output-of-adv7511-not-stable
 */

 u8 hdmi_iic_other2[][2] = {
    {0x01, 0x00}, //Set N Value(6144)
    {0x02, 0x18}, //Set N Value(6144)
    {0x03, 0x00}, //Set N Value(6144)
    {0x15, 0x20}, //Input 444(RGB or YCrCb) with Separate Syncs,48Khz FS
    {0x16, 0x70}, //Output format 444, 24-bit RGB input
    {0x18, 0x46}, //CSC disabled
    {0x40, 0x80}, //General Control Packet Enable
    {0x41, 0x10}, //Power Down control
    {0x49, 0x00}, //No truncation
    {0x4C, 0x04}, //8bit Output
    {0x55, 0x00}, //Set RGB 444 in AVinfo Frame
    {0x56, 0x08}, //Set active format Aspect
    {0x96, 0x20}, //HPD Interrupt clear
    {0xD6, 0xC0}, //HPD always high
    {0x98, 0x03}, //ADI reqired write
    {0x99, 0x02}, //ADI Required Write
    {0x9C, 0x30}, //PLL Filter R1 Value
    {0x9D, 0x61}, //Set clock divide
    {0xA2, 0xA4}, //ADI Recommended Write
    {0xA3, 0xA4}, //ADI Recommended Write
    {0xA5, 0x04}, //ADI Recommended Write
    {0xAB, 0x40}, //ADI Recommended Write
    {0xAF, 0x16}, //Set HDMI Mode
    //{0xAF, 0x04}, //Set DVI Mode
    {0xBA, 0x60}, //No clock delay
    {0xD1, 0xFF}, //ADI Recommended Write
    {0xDE, 0xD8}, //ADI Recommended Write
    {0xE4, 0x60}, //VCO Swing Reference Voltage
    {0xFA, 0x7D}, //Nbr of times to search for good phase
 };

#define ADV7511_INIT_SEQUENCE (hdmi_iic_kcu116_refDes)
#define NUMBER_OF_HDMI_REGS  (sizeof(ADV7511_INIT_SEQUENCE) / 3)

int I2C::Init()
{
//	u8 BytesRead;
//	u8 BytesWritten;
	u8 readReg;
	u32 StatusReg;
	int Status;

	/*
	 * Initialize the IIC Core.
	 */
	Status = XIic_DynInit(IIC_BASE_ADDRESS);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// FOR KINTEX-7 (KC705, Rev 1.0) only: generate GPIO Reset sequence
	XIic_WriteReg(IIC_BASE_ADDRESS, 0x124, 0xffffffff);	// bit0 = 1
	for (volatile unsigned i = 0; i < 100000; i++);	// delay
	XIic_WriteReg(IIC_BASE_ADDRESS, 0x124, 0xfffffffe);	// bit0 = 0
	for (volatile unsigned i = 0; i < 100000; i++);	// delay
	XIic_WriteReg(IIC_BASE_ADDRESS, 0x124, 0xffffffff);	// bit0 = 1

	/*
	 * Make sure all the Fifo's are cleared and Bus is Not busy.
	 */
	while (((StatusReg = XIic_ReadReg(IIC_BASE_ADDRESS, XIIC_SR_REG_OFFSET))
			& (XIIC_SR_RX_FIFO_EMPTY_MASK | XIIC_SR_TX_FIFO_EMPTY_MASK | XIIC_SR_BUS_BUSY_MASK))
			!= (XIIC_SR_RX_FIFO_EMPTY_MASK | XIIC_SR_TX_FIFO_EMPTY_MASK))
	{
//		xil_printf("\r\n while");
	}

	/*
	 * KCU116 DOES NOT HAVE ADV7511 CONNECTED TO THE MAIN I2C BUS
	 * SO SWITCH SETUP IS NOT NEEDED IN THIS CASE
	 */
//	/*
//	 * Initialize the data to written and the read buffer.
//	 */
//	for (u8 Index = 0; Index < PAGE_SIZE; Index++) {
//		WriteBuffer[Index] = Index;
//	}
//	/*
//	 * Write to the IIC SWITCH.
//	 */
//	EepromIicAddr = IIC_SWITCH_ADDRESS; // Alternate use of Write routine
//	WriteBuffer[0] = 0x20;
////	BytesWritten = EepromWriteByte(0x20, WriteBuffer, 0);

	/*
	 * Write Initialization Sequence to ADV7511.
	 */
	EepromIicAddr = IIC_ADV7511_ADDRESS;

	for (u8 Index = 0; Index < NUMBER_OF_HDMI_REGS; Index++)
	{
		/*BytesWritten = */EepromWriteByte(ADV7511_INIT_SEQUENCE[Index][0], &ADV7511_INIT_SEQUENCE[Index][1], 1);

//		xil_printf("Bytes %d\n\r", BytesWritten);

#ifdef READBACK_ENABLE
	    for (unsigned i = 0; i < 50000; i++);
//
//	    // TRY TO READ DATA FROM EEPROM
//
	    /*BytesRead = */EepromReadByte(ADV7511_INIT_SEQUENCE[Index][0], &readReg, 1);
	    xil_printf("Register : %X = %X (%X)\n\r",	ADV7511_INIT_SEQUENCE[Index][0], readReg, ADV7511_INIT_SEQUENCE[Index][1]);
	    if (readReg != ADV7511_INIT_SEQUENCE[Index][1])
	    {
	    	xil_printf("\t\tERROR !!!\n\r");
	    }
#endif
	}

	return XST_SUCCESS;
}

void I2C::Refresh()
{
	u8 readReg1, readReg2;
	EepromReadByte((AddressType)0x41, &readReg1, 1);
	EepromReadByte((AddressType)0x42, &readReg2, 1);

	/*
	 * Interrupt-based HPD detection would be more elegant but
	 * it still does not degrade efficiency of MB (much...)
	 * 'The best method to determine when the HPD is high is to use the interrupt system.' - from ADV7511 UG
	 */
	if (readReg1 != 0x10 && readReg2 == 0xF0)
	{
		xil_printf("HDMI cable connection!\n\rRefreshing...\n\r");
		for (u8 Index = 0; Index < NUMBER_OF_HDMI_REGS; Index++)
		{
			EepromWriteByte(ADV7511_INIT_SEQUENCE[Index][0], &ADV7511_INIT_SEQUENCE[Index][1], 1);
		}
	}
}

u8 I2C::EepromWriteByte(AddressType Address, u8 *BufferPtr, u8 ByteCount)
{
	u8 SentByteCount;
	u8 WriteBuffer[sizeof(Address) + PAGE_SIZE];
	u8 Index;

	/*
	 * A temporary write buffer must be used which contains both the address
	 * and the data to be written, put the address in first based upon the
	 * size of the address for the EEPROM
	 */
	if (sizeof(AddressType) == 2)
	{
		WriteBuffer[0] = (u8) (Address >> 8);
		WriteBuffer[1] = (u8) (Address);
	}
	else if (sizeof(AddressType) == 1)
	{
		WriteBuffer[0] = (u8) (Address);
		EepromIicAddr |= (EEPROM_TEST_START_ADDRESS >> 8) & 0x7;
	}

	/*
	 * Put the data in the write buffer following the address.
	 */
	for (Index = 0; Index < ByteCount; Index++)
	{
		WriteBuffer[sizeof(Address) + Index] = BufferPtr[Index];
	}

	/*
	 * Write a page of data at the specified address to the EEPROM.
	 */
	SentByteCount = XIic_DynSend(IIC_BASE_ADDRESS, EepromIicAddr, WriteBuffer,
								sizeof(Address) + ByteCount, XIIC_STOP);

	/*
	 * Return the number of bytes written to the EEPROM.
	 */
	return SentByteCount - sizeof(Address);
}

/******************************************************************************
/**
*
* This function reads a number of bytes from the IIC serial EEPROM into a
* specified buffer.
*
* @param	BufferPtr contains the address of the data buffer to be filled.
* @param	ByteCount contains the number of bytes in the buffer to be read.
*		This value is constrained by the page size of the device such
*		that up to 64K may be read in one call.
*
* @return	The number of bytes read. A value less than the specified input
*		value indicates an error.
*
* @note		None.
*
******************************************************************************/
u8 I2C::EepromReadByte(u8 *BufferPtr, u8 ByteCount)
{
	u8 ReceivedByteCount;
	u8 SentByteCount;
	u16 StatusReg;
	AddressType Address = EEPROM_TEST_START_ADDRESS;

	/*
	 * Position the Read pointer to specific location in the EEPROM.
	 */
	do
	{
		StatusReg = XIic_ReadReg(IIC_BASE_ADDRESS, XIIC_SR_REG_OFFSET);
		if (!(StatusReg & XIIC_SR_BUS_BUSY_MASK))
		{
			SentByteCount = XIic_DynSend(IIC_BASE_ADDRESS,
											EepromIicAddr,
											(u8 *) &Address,
											sizeof(Address),
											XIIC_STOP);
		}

	} while (SentByteCount != sizeof(Address));

	StatusReg = XIic_ReadReg(IIC_BASE_ADDRESS, XIIC_SR_REG_OFFSET);

	while ((StatusReg & XIIC_SR_BUS_BUSY_MASK))
	{
		StatusReg = XIic_ReadReg(IIC_BASE_ADDRESS, XIIC_SR_REG_OFFSET);
	}

	do
	{
		/*
		 * Receive the data.
		 */
		ReceivedByteCount = XIic_DynRecv(IIC_BASE_ADDRESS,
										 EepromIicAddr,
										 BufferPtr,
										 ByteCount);
	} while (ReceivedByteCount != ByteCount);

	/*
	 * Return the number of bytes received from the EEPROM.
	 */
	return ReceivedByteCount;
}

/******************************************************************************
 *
 * This function reads a number of bytes from the IIC serial EEPROM into a
 * specified buffer.
 *
 * @param	BufferPtr contains the address of the data buffer to be filled.
 * @param	ByteCount contains the number of bytes in the buffer to be read.
 *		This value is constrained by the page size of the device such
 *		that up to 64K may be read in one call.
 *
 * @return	The number of bytes read. A value less than the specified input
 *		value indicates an error.
 *
 * @note		None.
 *
 ******************************************************************************/
u8 I2C::EepromReadByte(AddressType Address, u8 *BufferPtr, u8 ByteCount)
{
	u8 ReceivedByteCount;
	u8 SentByteCount;
	u16 StatusReg;

	/*
	 * Position the Read pointer to specific location in the EEPROM.
	 */
   /*
  * Set the address register to the specified address by writing
  * the address to the device, this must be tried until it succeeds
  * because a previous write to the device could be pending and it
  * will not ack until that write is complete.
  */
	do
	{
		StatusReg = XIic_ReadReg(IIC_BASE_ADDRESS, XIIC_SR_REG_OFFSET);
		if (!(StatusReg & XIIC_SR_BUS_BUSY_MASK))
		{
			SentByteCount = XIic_DynSend(IIC_BASE_ADDRESS,
											EepromIicAddr,
											(u8 *) &Address,
											sizeof(Address),
											XIIC_REPEATED_START);
		}

	} while (SentByteCount != sizeof(Address));
	/*
	 * Receive the data.
	 */
  /*
	 * Read the number of bytes at the specified address from the EEPROM.
	 */
	ReceivedByteCount = XIic_DynRecv(IIC_BASE_ADDRESS, EepromIicAddr, BufferPtr, ByteCount);

	/*
	 * Return the number of bytes received from the EEPROM.
	 */
	return ReceivedByteCount;
}
