#pragma once
#include "stdio.h"
#include "ff.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "diskio.h"
#include "pgmspace.h"
#include "stm32f4xx_it.h"
#include "main.h"

#define PI 3.14159f
#define F_SAMPLE 44100.0f
#define F_OUT 1500.0f
uint16_t sample_N;
TIM_HandleTypeDef htim4;
I2S_HandleTypeDef hi2s3;
int16_t currentFile;

//pliki
static FATFS FatFs; 	//uchwyt
FRESULT fresult; 		//wynik operacji
FIL file;				//otwarty plik
UINT bytes_written;		//liczba zapisanych bajtow
UINT bytes_read;		//liczba odczytanych bajtow
char fileNames[64][13];
char currentFileName[13];
char dataDescription[4];

//odtwarzacz
uint8_t audioBuffer8;
uint16_t audioBuffer16;

void choose();
void startSong(char* fileName);
void changeFile(int8_t changeValue);
void playSong();
