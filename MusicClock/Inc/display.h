#pragma once
#include "stdio.h"
#include "ff.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "pgmspace.h"
#include "stm32f4xx_it.h"
#include "main.h"

//defines
#define DISP_1_ON   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)
#define DISP_1_OFF  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)
#define DISP_2_ON   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET)
#define DISP_2_OFF  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET)
#define DISP_3_ON   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)
#define DISP_3_OFF  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)
#define DISP_4_ON   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET)
#define DISP_4_OFF  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET)

#define DISP_VAL_NULL HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0 | GPIO_PIN_1 | \
                                               GPIO_PIN_2 | GPIO_PIN_3 | \
                                               GPIO_PIN_4 | GPIO_PIN_5 | \
                                               GPIO_PIN_6 | GPIO_PIN_7 , \
                                               GPIO_PIN_SET)

#define DISP_VAL_0	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0 | GPIO_PIN_1 | \
                                              GPIO_PIN_2 | GPIO_PIN_3 | \
                                              GPIO_PIN_4 | GPIO_PIN_5 , \
                                              GPIO_PIN_RESET)
#define DISP_VAL_1	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1 | GPIO_PIN_2 , \
                                              GPIO_PIN_RESET)
#define DISP_VAL_2	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0 | GPIO_PIN_1 | \
                                              GPIO_PIN_3 | GPIO_PIN_4 | \
                                              GPIO_PIN_6 , \
                                              GPIO_PIN_RESET)
#define DISP_VAL_3	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0 | GPIO_PIN_1 | \
                                              GPIO_PIN_2 | GPIO_PIN_3 | \
                                              GPIO_PIN_6 , \
                                              GPIO_PIN_RESET)
#define DISP_VAL_4	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1 | GPIO_PIN_2 | \
    	                                       GPIO_PIN_5 | GPIO_PIN_6 , \
                                              GPIO_PIN_RESET)
#define DISP_VAL_5	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0 | GPIO_PIN_2 | \
                                              GPIO_PIN_3 | GPIO_PIN_5 | \
                                              GPIO_PIN_6 , \
                                              GPIO_PIN_RESET)
#define DISP_VAL_6	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0 | GPIO_PIN_2 | \
                                              GPIO_PIN_3 | GPIO_PIN_4 | \
                                              GPIO_PIN_5 | GPIO_PIN_6 , \
                                              GPIO_PIN_RESET)
#define DISP_VAL_7	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0 | GPIO_PIN_1 | \
                                              GPIO_PIN_2 , \
                                              GPIO_PIN_RESET)
#define DISP_VAL_8	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0 | GPIO_PIN_1 | \
                                              GPIO_PIN_2 | GPIO_PIN_3 | \
                                              GPIO_PIN_4 | GPIO_PIN_5 | \
                                              GPIO_PIN_6 , \
                                              GPIO_PIN_RESET)
#define DISP_VAL_9	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0 | GPIO_PIN_1 | \
                                              GPIO_PIN_2 | GPIO_PIN_3 | \
                                              GPIO_PIN_5 | GPIO_PIN_6 , \
                                              GPIO_PIN_RESET)

#define DISP_DOT  	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET)
#define pgm_read_byte(x) (*(x))

//variables
uint8_t hour,min,sec,date,month,year2digit,day; //rtc
uint8_t data_RTC[8];
int year4digit;
uint8_t BCD2DEC(uint8_t data);
uint8_t DEC2BCD(uint8_t data);
uint8_t dayOfTheWeek(int thn,int bln,int tgl);
I2C_HandleTypeDef hi2c1;
I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi3_tx;
SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

//function prototypes
void vprint(const char *fmt,va_list argp);
void my_prinft(const char *fmt, ...);
void getRTC();
void setRTC(uint8_t tgl, uint8_t bln, uint16_t thn, uint8_t jam, uint8_t mnt, uint8_t dtk);


