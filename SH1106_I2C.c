/****************************************************************
* SH1106 BASED OLED MODULE (128 x 64)
* I2C MODE
*
* NOTE: THE OLED CONTROLLER SUPPORTS BOTH I2C AND SPI, BUT THE
* 		MODULES BOUGHT USUALLY HARDWIRE TO SELECT A PARTICULAR
* 		MODE
*
* 		THE ORIGINAL ADAFRUIT ONES USE THE SSD1306 CONTROLLER,
* 		BUT THE CHEAPER ONES BOUGHT FROM ALIEXPRESS N ALL USE
* 		THE CHEAPER SH1306 CONTROLLER, WHICH IS SIMILAR BUT
* 		NOT EXACTLY THE SAME TO THIS ONE !
* 		SH1106
* 			(1) DISPLAY BUFFER = 132x64 (THIS IS MAPPED TO 128x64 OF THE OLED)
* 			(2) ONLY SUPPORTS PAGE ADDRESSING MODE (SSD1306 PROVIDES VARIOUS MODES)
	* 			1 BYTE WRITES ALL THE ROWS IN A PAGE (8) AND THEN THE CURSOR MOVES
* 				ON TO THE NEXT COLUMN. COLUMN INCREAMENTS ACUTOMATICALLY. HOWEVER
* 				WHEN REACHED THE END OF PAGE, NEED TO SET THE NEXT PAGE MANUALLY
* 			(3) DOES NOT SUPPORT CUSTOM WINDOW WITHIN THE DISPLAY RAM
*
* MAY 29 2017
*
* ANKIT BHATNAGAR
* ANKIT.BHATNAGARINDIA@GMAIL.COM
*
* REFERENCES
* ------------
*   (1) http://www.picbasic.co.uk/forum/archive/index.php/t-21213.html
*   (2)	https://arduino.stackexchange.com/questions/13975/porting-sh1106-oled-driver-128-x-64-128-x-32-screen-is-garbled-but-partially
*   (3) http://forum.arduino.cc/index.php?topic=265557.0
****************************************************************/

#include "SH1106_I2C.h"

//LOCAL LIBRARY VARIABLES////////////////////////////////
//DEBUG RELATED
static uint8_t _sh1106_i2c_debug;

//DEVICE RELATED
static uint8_t _sh1106_i2c_slave_address;
static uint8_t* _sh1106_frambuffer_pointer;
//END LOCAL LIBRARY VARIABLES/////////////////////////////

void PUT_FUNCTION_IN_FLASH SH1106_I2C_SetDebug(uint8_t debug_on)
{
	//SET DEBUG PRINTF ON(1) OR OFF(0)

	_sh1106_i2c_debug = debug_on;
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_SetDeviceAddress(uint8_t address)
{
	//SET THE I2C MODULE SLAVE ADDRESS
	//NOTE THIS IS THE 7 BIT ADDRESS (WITHOUT THE R/W BIT)

	_sh1106_i2c_slave_address = address;

	//INITIALIZE BACKEND I2C
	_sh1106_i2c_backend_init(_sh1106_i2c_slave_address);

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : I2C : Address set to %d\n", _sh1106_i2c_slave_address);
		debug_printf("SH1106 : I2C : Initialized backend I2C\n", _sh1106_i2c_slave_address);
	}
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_Init(void)
{
	//INITIALIZE THE OLED MODULE AS PER THE DEFAULT PARAMETERS

	//INITIALIZE DISPLAY FRAMBUFFER
	_sh1106_frambuffer_pointer = (uint8_t*)os_zalloc((SH1106_I2C_OLED_MAX_COLUMN + 1) * (SH1106_I2C_OLED_MAX_PAGE + 1));

	//INITIALIZE THE DISPLAY
	_sh1106_i2c_send_start_function();

	//SET I2C SLAVE WRITE ADDRESS
	_sh1106_i2c_send_byte_function((_sh1106_i2c_slave_address << 1));

	//SET TYPE TO COMMAND STREAM
	_sh1106_i2c_send_byte_function(SH1106_I2C_CONTROL_BYTE_CMD_STREAM);

	//DISPLAY OFF
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_OFF);

	//SET COLUMN ADDRESS LOWER (2)
	//BECAUSE THIS CONTROLLER HAS RAM SIZE 132 X 64 WHEREAS OUR DISPLAY
	//IS 128 X 4. SO THE OLED IS MAPPED FROM RAM COLUMN 2 - COLUMN 130
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_COLUMN_LOWER_4 | 2);
	_sh1106_i2c_send_byte_function(0x10);

	//SET START PAGE ADDRESS = 0
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_PAGE_ADDRESS | 0);

	//SET COMMON OUTPUT SCAN DIRECTION = TOP -> BOTTOM
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_COMMON_SCAN_DIRECTION | 0);

	_sh1106_i2c_send_byte_function(0x00);
	_sh1106_i2c_send_byte_function(0x10);

	//SET DISPLAY START LINE = 0
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_START_LINE | 0);

	//SET CONTRAST
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_CONTRAST_CONTROL_MODE);
	_sh1106_i2c_send_byte_function(0x7F);

	//SET SEGMENT REMAP
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_SEGMENT_REMAP | 1);

	//SET DISPLAY = NORMAL
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_NORMAL);

	//SET MULTIPLEX RATIO = 63 (ALL ROWS)
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_MULTIPLEX_RATIO);
	_sh1106_i2c_send_byte_function(0x3F);

	//SET ENTIRE DISPLAY = ON
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_ENTIRE_DISPLAY_ON);

	//SET DISPLAY OFFSET = 0
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_OFFSET_MODE);
	_sh1106_i2c_send_byte_function(0x00);

	//SET DISPLAY OSCILLATOR FREQUENCY
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_OSCILLATOR_FREQUENCY);
	_sh1106_i2c_send_byte_function(0xF0);

	//SET DISCHARGE-PRECHARGE PERIOD
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISCHARGE_PRECHARGE);
	_sh1106_i2c_send_byte_function(0x22);

	//SET COMMON PADS HARDWARE CONFIG
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_COMMON_PADS_HARDWARE_CONFIG);
	_sh1106_i2c_send_byte_function(0x12);

	//SET COMMON PAD OUTPUT VOLTAGE
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_COMMON_PADS_OUTPUT_VOLTAGE);
	_sh1106_i2c_send_byte_function(0x20);

	_sh1106_i2c_send_byte_function(0x8D);
	_sh1106_i2c_send_byte_function(0x14);

	//SET DISPLAY ON
	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_ON);

	_sh1106_i2c_send_stop_function();

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Frame buffer allocated\n");
		debug_printf("SH1106 : Display initialized\n");
	}
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_SetDisplayOnOff(uint8_t on)
{
	//TURN THE DISPLAY ON/OFF DEPENDING ON INPUT ARGUENT VALUE

	_sh1106_i2c_send_start_function();

	//SET I2C SLAVE WRITE ADDRESS
	_sh1106_i2c_send_byte_function((_sh1106_i2c_slave_address << 1));

	//SET TYPE TO COMMAND STREAM
	_sh1106_i2c_send_byte_function(SH1106_I2C_CONTROL_BYTE_CMD_STREAM);

	if(on)
	{
		_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_ON);
		if(_sh1106_i2c_debug)
		{
			debug_printf("SH1106 : Display turned ON\n");
		}
	}
	else
	{
		_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_OFF);
		if(_sh1106_i2c_debug)
		{
			debug_printf("SH1106 : Display turned OFF\n");
		}
	}
	_sh1106_i2c_send_stop_function();
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_SetDisplayContrast(uint8_t contrast_val)
{
	//SET THE CONTRAST OF THE DISPLAY (0 - 255)
	//HIGHER THE CONTRAST, HIGHER THE DISPLAY CURRENT CONSUMPTION

	_sh1106_i2c_send_start_function();

	//SET I2C SLAVE WRITE ADDRESS
	_sh1106_i2c_send_byte_function((_sh1106_i2c_slave_address << 1));

	//SET TYPE TO COMMAND STREAM
	_sh1106_i2c_send_byte_function(SH1106_I2C_CONTROL_BYTE_CMD_STREAM);

	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_CONTRAST_CONTROL_MODE);
	_sh1106_i2c_send_byte_function(contrast_val);

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Contrast set to : %u\n", contrast_val);
	}
	_sh1106_i2c_send_stop_function();
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_SetDisplayNormal(void)
{
	//SET DISPLAY TO NORMAL MODE

	_sh1106_i2c_send_start_function();

	//SET I2C SLAVE WRITE ADDRESS
	_sh1106_i2c_send_byte_function((_sh1106_i2c_slave_address << 1));

	//SET TYPE TO COMMAND STREAM
	_sh1106_i2c_send_byte_function(SH1106_I2C_CONTROL_BYTE_CMD_STREAM);

	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_NORMAL);
	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Display = Normal\n");
	}

	_sh1106_i2c_send_stop_function();
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_SetDisplayInverted(void)
{
	//SET DISPLAY TO INVERTED MODE

	_sh1106_i2c_send_start_function();

	//SET I2C SLAVE WRITE ADDRESS
	_sh1106_i2c_send_byte_function((_sh1106_i2c_slave_address << 1));

	//SET TYPE TO COMMAND STREAM
	_sh1106_i2c_send_byte_function(SH1106_I2C_CONTROL_BYTE_CMD_STREAM);

	_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_DISPLAY_REVERSED);
	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Display = Inverted\n");
	}

	_sh1106_i2c_send_stop_function();
}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_ResetAndClearScreen(uint8_t fill_pattern)
{
	//RESET THE CURSOR TO COLUMN 0, PAGE 0
	//CLEAR THE SCREEN AND FILL WITH THE SPECIFIED PATTERN

	uint16_t counter = 0;

	for(counter = 0; counter < (SH1106_I2C_OLED_MAX_COLUMN + 1) * (SH1106_I2C_OLED_MAX_PAGE + 1); counter++)
	{
		_sh1106_frambuffer_pointer[counter] = fill_pattern;
	}

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Display filled with pattern = 0x%x\n", fill_pattern);
	}

}

void PUT_FUNCTION_IN_FLASH SH1106_I2C_UpdateDisplay(void)
{
	//TRANSFER THE FRAMEBUFFER TO THE DISPLAY IN BULK

	uint16_t counter = 0;
	uint16_t x = 0;
	uint16_t y = 0;

	for(y = 0; y < (SH1106_I2C_OLED_MAX_PAGE + 1); y++)
	{
		//SET CURSOR TO THE BEGINNING OF THE CURRENT PAGE
		_sh1106_i2c_send_start_function();
		_sh1106_i2c_send_byte_function((_sh1106_i2c_slave_address << 1));
		_sh1106_i2c_send_byte_function(SH1106_I2C_CONTROL_BYTE_CMD_STREAM);

		//SET COLUMN TO 0
		_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_COLUMN_LOWER_4 | 0);
		_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_COLUMN_UPPER_4 | 0);

		//SET PAGE
		_sh1106_i2c_send_byte_function(SH1106_I2C_CMD_SET_PAGE_ADDRESS | y);
		_sh1106_i2c_send_stop_function();

		//SEND PAGE DATA
		_sh1106_i2c_send_start_function();
		_sh1106_i2c_send_byte_function((_sh1106_i2c_slave_address << 1));
		_sh1106_i2c_send_byte_function(SH1106_I2C_CONTROL_BYTE_DATA_STREAM);

		for(x = 0; x < (SH1106_I2C_OLED_MAX_COLUMN + 1); x++)
		{
			_sh1106_i2c_send_byte_function(_sh1106_frambuffer_pointer[counter]);
			counter++;
		}
		_sh1106_i2c_send_stop_function();
	}

	if(_sh1106_i2c_debug)
	{
		debug_printf("SH1106 : Display updated with frame buffer\n");
	}
}
