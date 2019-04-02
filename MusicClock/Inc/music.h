#pragma once
#include "stdio.h"
#include "ff.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "pgmspace.h"
#include "stm32f4xx_it.h"
#include "main.h"

#define PI 3.14159f
#define F_SAMPLE 50000.0f
#define F_OUT 1500.0f

TIM_HandleTypeDef htim4;
static FATFS FatFs;
FRESULT fresult;
FIL file;
UINT bytes_written;
UINT bytes_read;

uint8_t audioBuffer8;
uint16_t audioBuffer16;
char fileNames[64][13];
char currentFileName[13];
char dataDescription[4];

void choose();
void startSong(char* fileName);
void changeFile(int8_t changeValue);
void playSong();
