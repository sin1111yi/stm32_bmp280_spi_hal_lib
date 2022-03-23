# **stm32_bmp280_spi_hal_lib**

***v1.0.0***   
2022/3/23
Now it can be used on stm32 through hal lib, example TBD.   
Update operate way, please check file lib/mpu9250.h for details.   
But it can only drive 1 sensor now, which "stm32_mpu9250_spi_hal_lib" can drive several sensors.

---

## **details**
**Callable function**
|   type   |             name             |
| :------: | :--------------------------: |
| uint8_t  |      bmp280_r_ChipId()       |
|   void   |   BMP280_Set_RegCtrlMeas()   |
|   void   |    BMP280_Set_RegConfig()    |
|   void   |       BMP280_Config()        |
| uint8_t  |        BMP280_Init()         |
| int32_t  |     BMP280_ReadStatus()      |
| int32_t  | BMP280_ReadTemperature_Row() |
| Optional |     bmp280_calc_t_fine()     |
| Optional |    BMP280_Compensate_T()     |
| Optional |    BMP280_Compensate_P()     |

  *Because of there are 3 optional ways to set compensate way, the last 3 functions' type is optional.*   
  *In file lib/bmp280.h, there is a marco define named "_COMPENSATION_FORMULA_" in line 202.*
  *Check line 203 & line 213 & line 223 to understand how it works*   
 *please check file lib/bmp280.c for more details*

 ---
 ## **Author**
 ***contact me by email sin1111yi@foxmail.com***




