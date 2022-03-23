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
 * @filename  bmp280.c
 * @brief     this file is about basic BMP280 driving
 * @author    sin1111yi
 * @date      2022/3/15
 * @version   1.0.0
 *
 * */

#include "bmp280.h"

BMP280 bmp280;
static ncs_io bmp280_ncs;

/*** basic bmp280 operate ***/
/*
 * @brief   write bmp280 reg through SPI
 * @param   address: address of reg to be written
 * @param   byte: one byte data to be written
 * */
static void bmp280_w_reg(uint8_t reg_addr, uint8_t byte)
{
	spi_w_byte(BMP280_SPI, reg_addr, byte, bmp280_ncs);
}

/*
 * @brief   read bmp280 regs through spi
 * @param   address: address of reg to be read
 * @param   num: number of byte to be read
 * */
static void bmp280_r_regs(uint8_t reg_addr, uint8_t num)
{
	spi_r_bytes(BMP280_SPI, reg_addr, num, bmp280_ncs);
}
/*/
 * @brief   init bmp280
 * */
uint8_t bmp280_r_ChipId()
{
	bmp280_r_regs(BMP280_CHIPID_REG, 1);
	return spiDataBuf[0];
}
/*
 * @brief   read bmp280 status reg
 *          register address: 0xF3, name: status
 *          Bit[3] name: measuring[0] '1' whenever a conversion is running
 *                 '0' when the results have been transferred to the data registers
 *          Bit[0] name: im_update[0] '1' when the NVM data are being copied to image registers
 *                 '0' when copying is done
 *          check page25 for more details
 * */
uint8_t BMP280_ReadStatus()
{
	bmp280_r_regs(BMP280_STATUS_REG, 1);
	if (spiDataBuf[0] == BMP280_IM_UPDATE)
		return BMP280_IM_UPDATE;
	else if (spiDataBuf[0] == BMP280_MEASURING)
		return BMP280_MEASURING;
	else
		return 0xFF;
}
/*
 * @brief   set the data acquisition options bmp280
 *          register address: 0xF4, register name: ctrl_meas
 *          Bit[7:5]: osrs_t[2:0] oversampling of temperature data
 *          Bit[4:2]: osrs_p[2:0] oversampling of pressure data
 *          Bit[1:0]: mode[1:0] power mode of device
 * */
void BMP280_Set_RegCtrlMeas()
{
	uint8_t ctrlMeasSet;
	ctrlMeasSet = (bmp280.conf.os_temp << 5) | (bmp280.conf.os_pres << 2) | (bmp280.conf.power_mode);
	bmp280_w_reg(BMP280_CTRLMEAS_REG, ctrlMeasSet);
}
/*
 * @brief   set stand by time and filter factor
 *          register address: 0xF5, register name: config
 *          Bit[7:5]: t_sb[2:0] inactive duration in normal mode
 *          Bit[4:2]: filter[2:0] time constant of the IIR filter
 *          Bit[0]: spi3w_en[1:0] enable 3-wire SPI interface when set to '1'
 * */
void BMP280_Set_RegConfig()
{
	uint8_t configSet;
	configSet = (bmp280.conf.odr << 5) | (bmp280.conf.filter << 2) | (bmp280.conf.spi3w_en);
	bmp280_w_reg(BMP280_CONFIG_REG, configSet);
}
/*
 * @brief   config ctrl_meas and config two registers
 * */
void BMP280_Config()
{
	// Always set the power mode after setting the configuration
	BMP280_Set_RegConfig();
	BMP280_Set_RegCtrlMeas();
}
/*
 * @brief   get correction parameters
 * */
void BMP280_GetCalibParam()
{
	bmp280_r_regs(BMP280_DIG_T1_LSB_REG, 6);
	bmp280.calib_param.dig_t1 = ((uint16_t)spiDataBuf[1] << 8) | spiDataBuf[0];
	bmp280.calib_param.dig_t2 = ((int16_t)spiDataBuf[3] << 8) | spiDataBuf[2];
	bmp280.calib_param.dig_t3 = ((int16_t)spiDataBuf[5] << 8) | spiDataBuf[4];

	bmp280_r_regs(BMP280_DIG_P1_LSB_REG, 18);
	bmp280.calib_param.dig_p1 = ((uint16_t)spiDataBuf[1] << 8) | spiDataBuf[0];
	bmp280.calib_param.dig_p2 = ((int16_t)spiDataBuf[3] << 8) | spiDataBuf[2];
	bmp280.calib_param.dig_p3 = ((int16_t)spiDataBuf[5] << 8) | spiDataBuf[4];
	bmp280.calib_param.dig_p4 = ((int16_t)spiDataBuf[7] << 8) | spiDataBuf[6];
	bmp280.calib_param.dig_p5 = ((int16_t)spiDataBuf[9] << 8) | spiDataBuf[8];
	bmp280.calib_param.dig_p6 = ((int16_t)spiDataBuf[11] << 8) | spiDataBuf[10];
	bmp280.calib_param.dig_p7 = ((int16_t)spiDataBuf[13] << 8) | spiDataBuf[12];
	bmp280.calib_param.dig_p8 = ((int16_t)spiDataBuf[15] << 8) | spiDataBuf[14];
	bmp280.calib_param.dig_p9 = ((int16_t)spiDataBuf[17] << 8) | spiDataBuf[16];

	bmp280_w_reg(BMP280_TRANSFER, BMP280_TRANSFER_ENABLE);
}
/*
 * @brief   init bmp280
 *          step1. read correction parameters
 *          step2. reset bmp280
 *          step3. config work mode
 * */
void BMP280_Init()
{
	/* set CSB port and pin */
	bmp280_ncs.port = BMP280_CSB_GPIO_Port;
	bmp280_ncs.pin = BMP280_CSB_Pin;

	bmp280_w_reg(BMP280_RESET_REG, BMP280_RESET_VALUE);
	BMP280_GetCalibParam();

	bmp280.conf.os_temp = BMP280_OS_x16;
	bmp280.conf.os_pres = BMP280_OS_x16;
	bmp280.conf.odr = BMP280_ODR_62_5_MS;
	bmp280.conf.filter = BMP280_Filter_Coeff_16;
	bmp280.conf.spi3w_en = BMP280_SPI3w_Disable;
	bmp280.conf.power_mode = BMP280_NormalMode;
	BMP280_Config();
}

/*
 * @brief   register 0xF7...0xF9
 *          0xF7 name: press_msb[7:0]
 *          0xF8 name: press_lsb[7:0]
 *          0xF9(Bit[7:4]) name: press_xlsb[3:0]
 * */
int32_t BMP280_ReadPressure_Row()
{

	bmp280_r_regs(BMP280_PRESSURE_MSB_REG, 3);

	bmp280.uncomp_data.uncomp_press = ((uint32_t)spiDataBuf[0] << 12) | ((uint32_t)spiDataBuf[1] << 4) | ((uint32_t)spiDataBuf[2] >> 4);

	return bmp280.uncomp_data.uncomp_press;
}
/*
 * @brief   register 0xFA...0xFC
 *          0xFA name: temp_msb[7:0]
 *          0xFB name: temp_lsb[7:0]
 *          0xFC(Bit[7:4]) name: temp_xlsb[3:0]
 * */
int32_t BMP280_ReadTemperature_Row()
{

	bmp280_r_regs(BMP280_TEMPERATURE_MSB_REG, 3);

	bmp280.uncomp_data.uncomp_temp = ((uint32_t)spiDataBuf[0] << 12) | ((uint32_t)spiDataBuf[1] << 4) | ((uint32_t)spiDataBuf[2] >> 4);

	return bmp280.uncomp_data.uncomp_temp;
}
/**** compensation formula functions ****/
#if (_COMPENSATION_FORMULA_ == 0)
/**** compensation formula in fixing point, system must support 64bit value ****/
/*
 * @brief   calculate t_fine for BMP280_Compensate_P_32bit()
 * @notice  if BMP280_Compensate_T_32bit() has already been called, it's no need to call this function
 *          because t_fine has been calculated in BMP280_Compensate_T_32bit().
 * @return  value of t_fine
 * */
float bmp280_calc_t_fine_int64()
{
	int32_t var1, var2;
	int32_t adc_T = BMP280_ReadTemperature_Row(bmp);
	var1 = ((((adc_T >> 3) - ((int32_t)bmp280.calib_param.dig_t1 << 1))) * ((int32_t)bmp280.calib_param.dig_t2)) >> 11;
	var2 = (((((adc_T >> 4) - ((int32_t)bmp280.calib_param.dig_t1)) * ((adc_T >> 4) - ((int32_t)bmp280.calib_param.dig_t1))) >> 12) * ((int32_t)bmp280.calib_param.dig_t3)) >> 14;
	bmp280.calib_param.t_fine = var1 + var2;
	return bmp280.calib_param.t_fine;
}

float BMP280_Compensate_T_int32()
{
	int32_t var1, var2, T;
	int32_t adc_T = BMP280_ReadTemperature_Row(bmp);
	var1 = ((((adc_T >> 3) - ((int32_t)bmp280.calib_param.dig_t1 << 1))) * ((int32_t)bmp280.calib_param.dig_t2)) >> 11;
	var2 = (((((adc_T >> 4) - ((int32_t)bmp280.calib_param.dig_t1)) * ((adc_T >> 4) - ((int32_t)bmp280.calib_param.dig_t1))) >> 12) * ((int32_t)bmp280.calib_param.dig_t3)) >> 14;
	bmp280.calib_param.t_fine = var1 + var2;
	T = (bmp280.calib_param.t_fine * 5 + 128) >> 8;
	bmp280.comp_data.temp = T * 0.01;
	return bmp280.comp_data.temp;
}

float BMP280_Compensate_P_int64()
{
	int64_t var1, var2, p;
	int64_t adc_P = BMP280_ReadPressure_Row(bmp);
	var1 = ((int64_t)bmp280.calib_param.t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)bmp280.calib_param.dig_p6;
	var2 = var2 + ((var1 * (int64_t)bmp280.calib_param.dig_p5) << 17);
	var2 = var2 + (((int64_t)bmp280.calib_param.dig_p4) << 35);
	var1 = ((var1 * var1 * (int64_t)bmp280.calib_param.dig_p3) >> 8) + ((var1 * (int64_t)bmp280.calib_param.dig_p2) << 12);
	var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)bmp280.calib_param.dig_p1) >> 33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576 - adc_P;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = (((int64_t)bmp280.calib_param.dig_p9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((int64_t)bmp280.calib_param.dig_p8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)bmp280.calib_param.dig_p7) << 4);
	bmp280.comp_data.press = p / 256.0;
	return bmp280.comp_data.press;
}
/**** Computation formulae for 32 bit systems ****/
#elif (_COMPENSATION_FORMULA_ == 1)
/**** compensation formula in floating point ****/
/*
 * @brief   use this function as use bmp_calc_t_fine_int32();
 * */
double bmp280_calc_t_fine_double()
{
	double var1, var2;
	int32_t adc_T = BMP280_ReadTemperature_Row();
	var1 = (((double)adc_T) / 16384.0 - ((double)bmp280.calib_param.dig_t1) / 1024.0) * ((double)bmp280.calib_param.dig_t2);
	var2 = ((((double)adc_T) / 131072.0 - ((double)bmp280.calib_param.dig_t1) / 8192.0) * (((double)adc_T) / 131072.0 - ((double)bmp280.calib_param.dig_t1) / 8192.0)) * ((double)bmp280.calib_param.dig_t3);
	bmp280.calib_param.t_fine = (int32_t)(var1 + var2);
	return bmp280.calib_param.t_fine;
}

double BMP280_Compensate_T_double()
{
	double var1, var2, T;
	int32_t adc_T = BMP280_ReadTemperature_Row();
	var1 = (((double)adc_T) / 16384.0 - ((double)bmp280.calib_param.dig_t1) / 1024.0) * ((double)bmp280.calib_param.dig_t2);
	var2 = ((((double)adc_T) / 131072.0 - ((double)bmp280.calib_param.dig_t1) / 8192.0) * (((double)adc_T) / 131072.0 - ((double)bmp280.calib_param.dig_t1) / 8192.0)) * ((double)bmp280.calib_param.dig_t3);
	bmp280.calib_param.t_fine = (int32_t)(var1 + var2);
	T = (var1 + var2) / 5120.0;
	return T;
}

double BMP280_Compensate_P_double()
{
	double var1, var2, p;
	int32_t adc_P = BMP280_ReadPressure_Row();
	var1 = ((double)bmp280.calib_param.t_fine / 2.0) - 64000.0;
	var2 = var1 * var1 * ((double)bmp280.calib_param.dig_p6) / 32768.0;
	var2 = var2 + var1 * ((double)bmp280.calib_param.dig_p5) * 2.0;
	var2 = (var2 / 4.0) + (((double)bmp280.calib_param.dig_p4) * 65536.0);
	var1 = (((double)bmp280.calib_param.dig_p3) * var1 * var1 / 524288.0 + ((double)bmp280.calib_param.dig_p2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0) * ((double)bmp280.calib_param.dig_p1);
	if (var1 == 0.0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576.0 - (double)adc_P;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double)bmp280.calib_param.dig_p9) * p * p / 2147483648.0;
	var2 = p * ((double)bmp280.calib_param.dig_p8) / 32768.0;
	p = p + (var1 + var2 + ((double)bmp280.calib_param.dig_p7)) / 16.0;
	return p;
}
#elif (_COMPENSATION_FORMULA_ == 2)
/**** compensation formula in fixing point ****/
/*
 * @brief   use this function as use bmp_calc_t_fine_int32();
 * */
float bmp280_calc_t_fine_int32()
{
	int32_t var1, var2;
	int32_t adc_T = BMP280_ReadTemperature_Row();
	var1 = ((((adc_T >> 3) - ((int32_t)bmp280.calib_param.dig_t1 << 1))) * ((int32_t)bmp280.calib_param.dig_t2)) >> 11;
	var2 = (((((adc_T >> 4) - ((int32_t)bmp280.calib_param.dig_t1)) * ((adc_T >> 4) - ((int32_t)bmp280.calib_param.dig_t1))) >> 12) * ((int32_t)bmp280.calib_param.dig_t3)) >> 14;
	bmp280.calib_param.t_fine = var1 + var2;
	return bmp280.calib_param.t_fine;
}
float BMP280_Compensate_T_int32()
{
	int32_t var1, var2, T;
	int32_t adc_T = BMP280_ReadTemperature_Row();
	var1 = ((((adc_T >> 3) - ((int32_t)bmp280.calib_param.dig_t1 << 1))) * ((int32_t)bmp280.calib_param.dig_t2)) >> 11;
	var2 = (((((adc_T >> 4) - ((int32_t)bmp280.calib_param.dig_t1)) * ((adc_T >> 4) - ((int32_t)bmp280.calib_param.dig_t1))) >> 12) * ((int32_t)bmp280.calib_param.dig_t3)) >> 14;
	bmp280.calib_param.t_fine = var1 + var2;
	T = (bmp280.calib_param.t_fine * 5 + 128) >> 8;
	return T * 0.01;
}
float BMP280_Compensate_P_int32()
{
	int32_t var1, var2;
	uint32_t p;
	int32_t adc_P = BMP280_ReadPressure_Row();
	var1 = (((int32_t)bmp280.calib_param.t_fine) >> 1) - (int32_t)64000;
	var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)bmp280.calib_param.dig_p6);
	var2 = var2 + ((var1 * ((int32_t)bmp280.calib_param.dig_p5)) << 1);
	var2 = (var2 >> 2) + (((int32_t)bmp280.calib_param.dig_p4) << 16);
	var1 = (((bmp280.calib_param.dig_p3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)bmp280.calib_param.dig_p2) * var1) >> 1)) >> 18;
	var1 = ((((32768 + var1)) * ((int32_t)bmp280.calib_param.dig_p1)) >> 15);
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = (((uint32_t)(((int32_t)1048576) - adc_P) - (var2 >> 12))) * 3125;
	if (p < 0x80000000)
	{
		p = (p << 1) / ((uint32_t)var1);
	}
	else
	{
		p = (p / (uint32_t)var1) * 2;
	}
	var1 = (((int32_t)bmp280.calib_param.dig_p9) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
	var2 = (((int32_t)(p >> 2)) * ((int32_t)bmp280.calib_param.dig_p8)) >> 13;
	p = (uint32_t)((int32_t)p + ((var1 + var2 + bmp280.calib_param.dig_p7) >> 4));
	return p;
}
#endif

void BMP280_ReadData()
{
	bmp280.comp_data.temp = BMP280_Compensate_T();
	bmp280.comp_data.press = BMP280_Compensate_P();
}
/* please check BST-BMP280-DS001-11 for more details */
