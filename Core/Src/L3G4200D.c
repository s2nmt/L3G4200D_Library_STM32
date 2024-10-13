/*
 * L3G4200D.c
 *
 *  Created on: Oct 13, 2024
 *      Author: Minh Tuan
 */

#include "L3G4200D.h"


uint8_t L3G4200D_Init(I2C_HandleTypeDef *I2Cx,l3g4200d_dps_t scale){
    uint8_t check;

    HAL_I2C_Mem_Read(I2Cx, L3G4200D_ADDRESS, L3G4200D_REG_WHO_AM_I, 1, &check, 1, 100);

    if (check != 0xD3) // 0x68 will be returned by the sensor if everything goes well
    {
        return 0;
    }
    uint8_t reg1 = 0x00;
    reg1 |= 0x0F; // Enable all axis and setup normal mode
    reg1 |= (L3G4200D_DATARATE_100HZ_25 << 4);
    HAL_I2C_Mem_Write(I2Cx, L3G4200D_ADDRESS, L3G4200D_REG_CTRL_REG1, 1, &reg1, 1, 100);

    // Disable high pass filter
    reg1 = 0x00;
    HAL_I2C_Mem_Write(I2Cx, L3G4200D_ADDRESS, L3G4200D_REG_CTRL_REG2, 1, &reg1, 1, 100);

    // Generata data ready interrupt on INT2
    reg1 = 0x08;
    HAL_I2C_Mem_Write(I2Cx, L3G4200D_ADDRESS, L3G4200D_REG_CTRL_REG3, 1, &reg1, 1, 100);

    // Set full scale selection in continous mode
    reg1 =  scale << 4;
    HAL_I2C_Mem_Write(I2Cx, L3G4200D_ADDRESS, L3G4200D_REG_CTRL_REG4, 1,&reg1 , 1, 100);
    reg1 = 0x00;
    HAL_I2C_Mem_Write(I2Cx, L3G4200D_ADDRESS, L3G4200D_REG_CTRL_REG5, 1, &reg1, 1, 100);
    return 1;
}
uint8_t L3G4200D_Read_Temperature(I2C_HandleTypeDef *I2Cx){
    uint8_t Rec_Data;
    HAL_I2C_Mem_Read(I2Cx,L3G4200D_ADDRESS,L3G4200D_REG_OUT_TEMP,1,&Rec_Data,1,100);
    return Rec_Data;
}

void L3G4200D_Read_Raw(I2C_HandleTypeDef *I2Cx, Vector_t *DataStruct) {
    uint8_t Rec_Data[6];

    // Try reading 6 bytes from the sensor
    if (HAL_I2C_Mem_Read(I2Cx, L3G4200D_ADDRESS, L3G4200D_REG_OUT_X_L | (1 << 7), 1, Rec_Data, 6, 100) != HAL_OK) {
        printf("I2C Read Error\r\n");
        return;
    }

    // Combine high and low bytes as signed 16-bit integers
    int16_t rawX = (int16_t)(Rec_Data[1] << 8 | Rec_Data[0]);
    int16_t rawY = (int16_t)(Rec_Data[3] << 8 | Rec_Data[2]);
    int16_t rawZ = (int16_t)(Rec_Data[5] << 8 | Rec_Data[4]);

    DataStruct->XAxis = rawX * 0.008757f;
    DataStruct->YAxis = rawY * 0.008757f;
    DataStruct->ZAxis = rawZ * 0.008757f;
}

l3g4200d_dps_t getScale(I2C_HandleTypeDef *I2Cx){
	uint8_t Rec_Data;
	HAL_I2C_Mem_Read(I2Cx, L3G4200D_ADDRESS, L3G4200D_REG_CTRL_REG4, 1, &Rec_Data, 1, 100);
	return (Rec_Data >>  4) & 0x03;
}
l3g4200d_odrbw_t getOdrBw(I2C_HandleTypeDef *I2Cx){
	uint8_t Rec_Data;
	HAL_I2C_Mem_Read(I2Cx, L3G4200D_ADDRESS, L3G4200D_REG_CTRL_REG1, 1, &Rec_Data, 1, 100);
	return (Rec_Data >>  4) & 0x0F;
}
void setThreshold(I2C_HandleTypeDef *I2Cx,uint8_t multiple){
    if (multiple > 0)
    {
		// If not calibrated, need calibrate
		if (!useCalibrate)
		{
			calibrate(I2Cx,50);
		}

		// Calculate threshold vectors
		t.XAxis = thresholdX * multiple;
		t.YAxis = thresholdY * multiple;
		t.ZAxis = thresholdZ * multiple;
	} else
		{
		// No threshold
		t.XAxis = 0;
		t.YAxis = 0;
		t.ZAxis = 0;
	}

	// Remember old threshold value
	actualThreshold = multiple;
}
void calibrate(I2C_HandleTypeDef *I2Cx, uint8_t samples){
	// Set calibrate
	useCalibrate = true;

	// Reset values
	float sumX = 0;
	float sumY = 0;
	float sumZ = 0;
	float sigmaX = 0;
	float sigmaY = 0;
	float sigmaZ = 0;

	// Read n-samples
	for (uint8_t i = 0; i < samples; ++i)
	{
		L3G4200D_Read_Raw(I2Cx,&d);
		sumX += r.XAxis;
		sumY += r.YAxis;
		sumZ += r.ZAxis;

		sigmaX += r.XAxis * r.XAxis;
		sigmaY += r.YAxis * r.YAxis;
		sigmaZ += r.ZAxis * r.ZAxis;

		HAL_Delay(5);
	}

	// Calculate delta vectors
	d.XAxis = sumX / samples;
	d.YAxis = sumY / samples;
	d.ZAxis = sumZ / samples;

	// Calculate threshold vectors
	thresholdX = sqrt((sigmaX / samples) - (d.XAxis * d.XAxis));
	thresholdY = sqrt((sigmaY / samples) - (d.YAxis * d.YAxis));
	thresholdZ = sqrt((sigmaZ / samples) - (d.ZAxis * d.ZAxis));

	// If already set threshold, recalculate threshold vectors
	if (actualThreshold > 0)
	{
		setThreshold(I2Cx,actualThreshold);
	}
}
uint8_t getThreshold(void){
	return actualThreshold;
}
