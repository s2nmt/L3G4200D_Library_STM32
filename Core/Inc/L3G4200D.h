/*
 * L3G4200D.h
 *
 *  Created on: Oct 13, 2024
 *      Author: Minh Tuan
 */

#ifndef INC_L3G4200D_H_
#define INC_L3G4200D_H_

#include "main.h"
#include "stdint.h"
#include "stdio.h"
#include "stm32f1xx_hal.h"
#include "stdbool.h"
#include "math.h"
#define L3G4200D_ADDRESS           (0xD2)

#define L3G4200D_REG_WHO_AM_I      (0x0F)

#define L3G4200D_REG_CTRL_REG1     (0x20)
#define L3G4200D_REG_CTRL_REG2     (0x21)
#define L3G4200D_REG_CTRL_REG3     (0x22)
#define L3G4200D_REG_CTRL_REG4     (0x23)
#define L3G4200D_REG_CTRL_REG5     (0x24)
#define L3G4200D_REG_REFERENCE     (0x25)
#define L3G4200D_REG_OUT_TEMP      (0x26)
#define L3G4200D_REG_STATUS_REG    (0x27)

#define L3G4200D_REG_OUT_X_L       (0x28)
#define L3G4200D_REG_OUT_X_H       (0x29)
#define L3G4200D_REG_OUT_Y_L       (0x2A)
#define L3G4200D_REG_OUT_Y_H       (0x2B)
#define L3G4200D_REG_OUT_Z_L       (0x2C)
#define L3G4200D_REG_OUT_Z_H       (0x2D)

#define L3G4200D_REG_FIFO_CTRL_REG (0x2E)
#define L3G4200D_REG_FIFO_SRC_REG  (0x2F)

#define L3G4200D_REG_INT1_CFG      (0x30)
#define L3G4200D_REG_INT1_SRC      (0x31)
#define L3G4200D_REG_INT1_THS_XH   (0x32)
#define L3G4200D_REG_INT1_THS_XL   (0x33)
#define L3G4200D_REG_INT1_THS_YH   (0x34)
#define L3G4200D_REG_INT1_THS_YL   (0x35)
#define L3G4200D_REG_INT1_THS_ZH   (0x36)
#define L3G4200D_REG_INT1_THS_ZL   (0x37)
#define L3G4200D_REG_INT1_DURATION (0x38)



typedef struct Vector
{
    float XAxis;
    float YAxis;
    float ZAxis;
}Vector_t;

typedef enum
{
    L3G4200D_SCALE_2000DPS = 0b10,
    L3G4200D_SCALE_500DPS  = 0b01,
    L3G4200D_SCALE_250DPS  = 0b00
} l3g4200d_dps_t;

typedef enum
{
    L3G4200D_DATARATE_800HZ_110  = 0b1111,
    L3G4200D_DATARATE_800HZ_50   = 0b1110,
    L3G4200D_DATARATE_800HZ_35   = 0b1101,
    L3G4200D_DATARATE_800HZ_30   = 0b1100,
    L3G4200D_DATARATE_400HZ_110  = 0b1011,
    L3G4200D_DATARATE_400HZ_50   = 0b1010,
    L3G4200D_DATARATE_400HZ_25   = 0b1001,
    L3G4200D_DATARATE_400HZ_20   = 0b1000,
    L3G4200D_DATARATE_200HZ_70   = 0b0111,
    L3G4200D_DATARATE_200HZ_50   = 0b0110,
    L3G4200D_DATARATE_200HZ_25   = 0b0101,
    L3G4200D_DATARATE_200HZ_12_5 = 0b0100,
    L3G4200D_DATARATE_100HZ_25   = 0b0001,
    L3G4200D_DATARATE_100HZ_12_5 = 0b0000
} l3g4200d_odrbw_t;

Vector_t r;
Vector_t n;
Vector_t d;
Vector_t t;

bool useCalibrate;
float actualThreshold;
float dpsPerDigit;
float thresholdX;
float thresholdY;
float thresholdZ;

uint8_t L3G4200D_Init(I2C_HandleTypeDef *I2Cx,l3g4200d_dps_t scale);
uint8_t L3G4200D_Read_Temperature(I2C_HandleTypeDef *I2Cx);
l3g4200d_dps_t getScale(I2C_HandleTypeDef *I2Cx);
l3g4200d_odrbw_t getOdrBw(I2C_HandleTypeDef *I2Cx);
void L3G4200D_Read_Raw(I2C_HandleTypeDef *I2Cx, Vector_t *DataStruct);
void setThreshold(I2C_HandleTypeDef *I2Cx,uint8_t multiple);
void calibrate(I2C_HandleTypeDef *I2Cx, uint8_t samples);
uint8_t getThreshold(void);
#endif /* INC_L3G4200D_H_ */
