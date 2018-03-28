/*
 * adv7511_setup.h
 *
 *  Created on: Jan 23, 2018
 *      Author: pjurgiel
 */

#ifndef SRC_ADV7511_SETUP_H_
#define SRC_ADV7511_SETUP_H_

#include "xiic.h"
#include "../include/worldDescription.h"

#define IIC_SWITCH_ADDRESS 	0x74
#define IIC_ADV7511_ADDRESS 0x39

#define IIC_BASE_ADDRESS	XPAR_AXI_IIC_0_BASEADDR

#define IIC_DEVICE_ID		XPAR_IIC_0_DEVICE_ID

/************************** Variable Definitions *****************************/
typedef u8 AddressType;
#define PAGE_SIZE					16
#define EEPROM_TEST_START_ADDRESS	0x80

#define READBACK_ENABLE

class I2C{
public:
	I2C(){}
	int Init();

#ifdef READBACK_ENABLE
	//SEE:
	// https://github.com/Xilinx/embeddedsw/blob/master/XilinxProcessorIPLib/drivers/iic/examples/xiic_low_level_dynamic_eeprom_example.c
	u8 EepromReadByte(u8 *BufferPtr, u8 ByteCount);
	u8 EepromReadByte(AddressType Address, u8 *BufferPtr, u8 ByteCount);
#endif

	void Refresh();

private:
	/*****************************************************************************/
	/**
	 * This function writes a buffer of bytes to the IIC serial EEPROM.
	 *
	 * @param	BufferPtr contains the address of the data to write.
	 * @param	ByteCount contains the number of bytes in the buffer to be
	 *		written. Note that this should not exceed the page size of the
	 *		EEPROM as noted by the constant PAGE_SIZE.
	 *
	 * @return	The number of bytes written, a value less than that which was
	 *		specified as an input indicates an error.
	 *
	 * @note		one.
	 *
	 ******************************************************************************/
	u8 EepromWriteByte(AddressType Address, u8 *BufferPtr, u8 ByteCount);

	u8 EepromIicAddr; 									/* Variable for storing Eeprom IIC address */
	u8 WriteBuffer[sizeof(AddressType) + PAGE_SIZE]; 	/* Write buffer for writing a page. */
};


#endif /* SRC_ADV7511_SETUP_H_ */
