//////////////////////////////////////////////////////////////////////////
/*
	ESP32 | LVGL9.2.2 | Ep 0. GFX Setup (ft. LovyanGFX)
	Developing Embedded GUI with EEZ Studio
	Configuración de config.h
	Created by That Project
*/
//////////////////////////////////////////////////////////////////////////
/*
	Modificado: Ernesto José Guerrero González, Civil Engineering ud FJdC.
*/
//////////////////////////////////////////////////////////////////////////

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define VERSION "0.0.0.4" 				//version number
#define _DEBUG_           				//debug switch
#define DEV_TYPE "CrowPanel 4.3 inch"
#define LVGL_TICK_PERIOD 10 			//lvgl tick period
#define ROTATION 0          			//set ui rotation
#define TOUCHPAD 						//whether the UI support touch pad
#define PIN_BL GPIO_NUM_2				//	cfg.pin_bl = 2

//----------------------------tp configaction
// Pines TFT HMI ESP32 4827S043 - ESP32-S3 480x272
// Pines CrowPanel ESP32 HMI 4.3inch Display.
static const uint16_t TFT_WIDTH = 480;
static const uint16_t TFT_HEIGHT = 272;
//----------------------------end tp configaction

//----------------------------LCD configaction
#define LCD_d0 				GPIO_NUM_8;        	// B0
#define LCD_d1 				GPIO_NUM_3;        	// B1
#define LCD_d2 				GPIO_NUM_46;       	// B2
#define LCD_d3 				GPIO_NUM_9;        	// B3
#define LCD_d4 				GPIO_NUM_1;        	// B4

#define LCD_d5 				GPIO_NUM_5;        	// G0
#define LCD_d6 				GPIO_NUM_6;        	// G1
#define LCD_d7 				GPIO_NUM_7;        	// G2
#define LCD_d8 				GPIO_NUM_15;       	// G3
#define LCD_d9 				GPIO_NUM_16;       	// G4
#define LCD_d10				GPIO_NUM_4;       	// G5

#define LCD_d11				GPIO_NUM_45;      	// R0
#define LCD_d12				GPIO_NUM_48;      	// R1
#define LCD_d13				GPIO_NUM_47;      	// R2
#define LCD_d14				GPIO_NUM_21;      	// R3
#define LCD_d15				GPIO_NUM_14;      	// R4

#define LCD_henable  		GPIO_NUM_40;
#define LCD_vsync  			GPIO_NUM_41;
#define LCD_hsync  			GPIO_NUM_39;
#define LCD_pclk   			GPIO_NUM_42;

#define LCD_bl 				GPIO_NUM_2;
//----------------------------end LCD configaction

//----------------------------tp configaction
#define TP_int				GPIO_NUM_36;

#define TP_SCK 				GPIO_NUM_12	
#define TP_MOSI 			GPIO_NUM_11	
#define TP_MISO 			GPIO_NUM_13	
#define TP_SS0 				-1	
#define TP_SS1 				GPIO_NUM_0	
#define TP_SS2 				GPIO_NUM_10

#define TOUCH_XPT2046
#define TOUCH_XPT2046_SCK	GPIO_NUM_12
#define TOUCH_XPT2046_MISO	GPIO_NUM_13
#define TOUCH_XPT2046_MOSI	GPIO_NUM_11
#define TOUCH_XPT2046_CS	GPIO_NUM_0
#define TOUCH_XPT2046_INT	GPIO_NUM_36
//----------------------------end tp configaction

//----------------------------SD configaction
#define TF_CS				GPIO_NUM_10
#define TF_SPI_MOSI			GPIO_NUM_11
#define TF_CLK				GPIO_NUM_12
#define TF_SPI_MISO			GPIO_NUM_13

#define SD_D0				GPIO_NUM_2
#define SD_D1				GPIO_NUM_4
#define SD_D2				GPIO_NUM_12
#define SD_D3				GPIO_NUM_13
#define SD_CMD				GPIO_NUM_15
#define SD_CLK				GPIO_NUM_14
#define SD_DET				GPIO_NUM_34
//----------------------------end SD configaction

//----------------------------UART1 configaction
#define UART1_TXD1			GPIO_NUM_17
#define UART1_RXD1			GPIO_NUM_18
//----------------------------end UART1 configaction

//----------------------------CONNECTOR configaction
#define CONNECTOR_TXD1		GPIO_NUM_17
#define CONNECTOR_RXD1		GPIO_NUM_18
#define CONNECTOR_I2C_SCL	GPIO_NUM_38
#define CONNECTOR_I2C_SDA	GPIO_NUM_37
//----------------------------end CONNECTOR configaction

//----------------------------GPIO_D configaction
#define GPIO_D_I2C_SCL		GPIO_NUM_38
#define GPIO_D_I2C_SDA		GPIO_NUM_37
//----------------------------end GPIO_D configaction

//----------------------------Audio (ISS:NS4168) configaction
#define I2S_DOUT			GPIO_NUM_20
#define I2S_BCLK			GPIO_NUM_35
#define I2S_LRC 			GPIO_NUM_19
//----------------------------end Audio (ISS:NS4168) configaction

//------------------------start display select
#define BUF_SIZE 80
#define DISP_BUF_SIZE (LV_HOR_RES_MAX * BUF_SIZE)
#define BUF_NUM 1
//------------------------end display select

#endif // __CONFIG_H__