/**
 ************ https://github.com/sin1111yi ************
 ******************************************************
 *        .__       ____ ____ ____ ____        .__
 *   _____|__| ____/_   /_   /_   /_   |___.__.|__|
 *  /  ___/  |/    \|   ||   ||   ||   <   |  ||  |
 *  \___ \|  |   |  \   ||   ||   ||   |\___  ||  |
 *  /___  >__|___|  /___||___||___||___|/ ____||__|
 *      \/        \/                    \/
 ******************************************************
 * @filename  bmp280.h
 * @brief     this file is about basic BMP280 driving
 * @author    sin1111yi
 * @date      2022/3/15
 * @version   1.0.0
 *
 * */
#ifndef __BMP280_H
#define __BMP280_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "spi.h"

#define BMP280_SPI &hspi2
#define BMP280_CS_PIN BMP280_CSB_Pin
#define BMP280_CS_GPIO BMP280_CSB_GPIO_Port

#define BMP280_ADDRESS (uint8_t)0x76
#define BMP280_RESET_VALUE (uint8_t)0xB6 // reset reg reset value

#define BMP280_CHIPID_REG (uint8_t)0xD0	  // Chip ID reg
#define BMP280_RESET_REG (uint8_t)0xE0	  // Softreset reg
#define BMP280_STATUS_REG (uint8_t)0xF3	  // Status reg
#define BMP280_CTRLMEAS_REG (uint8_t)0xF4 // Ctrl measure reg
#define BMP280_CONFIG_REG (uint8_t)0xF5	  // Configuration reg

#define BMP280_PRESSURE_MSB_REG (uint8_t)0xF7  // Pressure MSB reg
#define BMP280_PRESSURE_LSB_REG (uint8_t)0xF8  // Pressure LSB reg
#define BMP280_PRESSURE_XLSB_REG (uint8_t)0xF9 // Pressure XLSB reg

#define BMP280_TEMPERATURE_MSB_REG (uint8_t)0xFA  // Temperature MSB reg
#define BMP280_TEMPERATURE_LSB_REG (uint8_t)0xFB  // Temperature LSB reg
#define BMP280_TEMPERATURE_XLSB_REG (uint8_t)0xFC // Temperature XLSB reg
#define BMP280_MEASURING (uint8_t)0x01
#define BMP280_IM_UPDATE (uint8_t)0x08

/* how could I know that */
#define BMP280_TRANSFER (uint8_t)0x74		 // https://blog.csdn.net/little_grapes/article/details/121445119
#define BMP280_TRANSFER_ENABLE (uint8_t)0xff // check the above link to understand this
/* unbelievable */

// calibration parameters
#define BMP280_DIG_T1_LSB_REG (uint8_t)0x88
#define BMP280_DIG_T1_MSB_REG (uint8_t)0x89
#define BMP280_DIG_T2_LSB_REG (uint8_t)0x8A
#define BMP280_DIG_T2_MSB_REG (uint8_t)0x8B
#define BMP280_DIG_T3_LSB_REG (uint8_t)0x8C
#define BMP280_DIG_T3_MSB_REG (uint8_t)0x8D

#define BMP280_DIG_P1_LSB_REG (uint8_t)0x8E
#define BMP280_DIG_P1_MSB_REG (uint8_t)0x8F
#define BMP280_DIG_P2_LSB_REG (uint8_t)0x90
#define BMP280_DIG_P2_MSB_REG (uint8_t)0x91
#define BMP280_DIG_P3_LSB_REG (uint8_t)0x92
#define BMP280_DIG_P3_MSB_REG (uint8_t)0x93
#define BMP280_DIG_P4_LSB_REG (uint8_t)0x94
#define BMP280_DIG_P4_MSB_REG (uint8_t)0x95
#define BMP280_DIG_P5_LSB_REG (uint8_t)0x96
#define BMP280_DIG_P5_MSB_REG (uint8_t)0x97
#define BMP280_DIG_P6_LSB_REG (uint8_t)0x98
#define BMP280_DIG_P6_MSB_REG (uint8_t)0x99
#define BMP280_DIG_P7_LSB_REG (uint8_t)0x9A
#define BMP280_DIG_P7_MSB_REG (uint8_t)0x9B
#define BMP280_DIG_P8_LSB_REG (uint8_t)0x9C
#define BMP280_DIG_P8_MSB_REG (uint8_t)0x9D
#define BMP280_DIG_P9_LSB_REG (uint8_t)0x9E
#define BMP280_DIG_P9_MSB_REG (uint8_t)0x9F

	/* following enums based on official manual */
	/* set "ctrl_meas" register Bit[7:5] or Bit[4:2]*/
	typedef enum __BMP280_Oversampling
	{
		BMP280_OS_Skip = 0b000, // skipped
		BMP280_OS_x1,			// 16 bit - 5e-3 cel - 2.62 Pa
		BMP280_OS_x2,			// 17 bit - 2.5e-3 cel - 1.31 Pa
		BMP280_OS_x4,			// 18 bit - 1.2e-3 cel - 0.66 Pa
		BMP280_OS_x8,			// 19 bit - 6e-4 cel - 0.33 Pa
		BMP280_OS_x16 = 0b101,	// 20 bit - 3e-4 cel - 0.16 Pa
								// 110 & 111 are the same with 101
	} BMP280_Oversampling_T,
		BMP280_Oversampling_P;

	/* set "ctrl_meas" register Bit[1:0] */
	typedef enum __BMP280_PowerMode
	{
		BMP280_SleepMode = 0b00,  // sleep mode
		BMP280_ForcedMode = 0b01, // forced mode, 10 is the same with 01
		BMP280_NormalMode = 0b11, // normal mode
	} BMP280_PowerMode;

	/* set "config" register Bit[7:5] */
	typedef enum __BMP280_ODR
	{
		BMP280_ODR_0_5_MS = 0b000, // 0.5ms
		BMP280_ODR_62_5_MS,		   // 62.5ms
		BMP280_ODR_125_MS,		   // 125ms
		BMP280_ODR_250_MS,		   // 250ms
		BMP280_ODR_500_MS,		   // 500ms
		BMP280_ODR_1000_MS,		   // 1000ms
		BMP280_ODR_2000_MS,		   // 2000ms
		BMP280_ODR_4000_MS,		   // 4000ms
	} BMP280_ODR;

	/* set "config" register Bit[4:2] */
	typedef enum __BMP280_IIR_Filter
	{
		BMP280_Filter_OFF = 0b000,
		BMP280_Filter_Coeff_2,
		BMP280_Filter_Coeff_4,
		BMP280_Filter_Coeff_8,
		BMP280_Filter_Coeff_16
	} BMP280_IIR_Filter;

	/* set "config" register Bit[0] */
	typedef enum __BMP280_SPI3w_EN
	{
		BMP280_SPI3w_Disable,
		BMP280_SPI3w_Enbale
	} BMP280_SPI3w_EN;

	/* following structs based on official lib */
	/* Calibration parameters' structure */
	typedef struct __BMP280_CalibParam
	{
		uint16_t dig_t1;
		int16_t dig_t2;
		int16_t dig_t3;
		uint16_t dig_p1;
		int16_t dig_p2;
		int16_t dig_p3;
		int16_t dig_p4;
		int16_t dig_p5;
		int16_t dig_p6;
		int16_t dig_p7;
		int16_t dig_p8;
		int16_t dig_p9;
		int32_t t_fine;
	} BMP280_CalibParam;
	/* Sensor configuration structure */
	typedef struct __BMP280_ConfigOption
	{
		uint8_t os_temp;
		uint8_t os_pres;
		uint8_t odr;
		uint8_t filter;
		uint8_t spi3w_en;
		uint8_t power_mode;
	} BMP280_ConfigOption;
	/* Sensor status structure */
	typedef struct __BMP280_Status
	{
		uint8_t measuring;
		uint8_t im_update;
	} BMP280_Status;
	/* Uncompensated data structure */
	typedef struct __BMP280_UncompData
	{
		int32_t uncomp_temp;
		uint32_t uncomp_press;
	} BMP280_UncompData;
	/* Compensated data structure */
	typedef struct __BMP280_CompData
	{
		float temp;
		float press;
	} BMP280_CompData;
	/* device structure */
	typedef struct __BMP280
	{
		BMP280_CalibParam calib_param;
		BMP280_ConfigOption conf;
		BMP280_UncompData uncomp_data;
		BMP280_CompData comp_data;
	} BMP280;

	extern BMP280 bmp280;

	uint8_t bmp280_r_ChipId();
	void BMP280_Set_RegCtrlMeas();
	void BMP280_Set_RegConfig();
	void BMP280_Config();
	void BMP280_Init();
	uint8_t BMP280_ReadStatus();
	int32_t BMP280_ReadPressure_Row();
	int32_t BMP280_ReadTemperature_Row();

#define _COMPENSATION_FORMULA_ 1
/* 64bit fixing point */
#if (_COMPENSATION_FORMULA_ == 0)
#define bmp280_calc_t_fine() bmp280_calc_t_fine_int64()
#define BMP280_Compensate_T() BMP280_Compensate_T_int32()
#define BMP280_Compensate_P() BMP280_Compensate_P_int64()

	float bmp280_calc_t_fine_int64();
	float BMP280_Compensate_T_int32();
	float BMP280_Compensate_P_int64();

/* 32bit floating point */
#elif (_COMPENSATION_FORMULA_ == 1)
#define bmp280_calc_t_fine() bmp280_calc_t_fine_double()
#define BMP280_Compensate_T() BMP280_Compensate_T_double()
#define BMP280_Compensate_P() BMP280_Compensate_P_double()

double bmp280_calc_t_fine_double();
double BMP280_Compensate_T_double();
double BMP280_Compensate_P_double();

/* 32bit fixing point */
#elif (_COMPENSATION_FORMULA_ == 2)
#define bmp280_calc_t_fine() bmp280_calc_t_fine_int32()
#define BMP280_Compensate_T() BMP280_Compensate_T_int32()
#define BMP280_Compensate_P() BMP280_Compensate_P_int32()

float bmp280_calc_t_fine_int32();
float BMP280_Compensate_T_int32();
float BMP280_Compensate_P_int32();
#endif

	void BMP280_ReadData();

#ifdef __cplusplus
}
#endif

#endif
