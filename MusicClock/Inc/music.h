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
static uint16_t sample_N;

static int16_t currentFile;

//pliki
static FATFS FatFs; 	//uchwyt
static FRESULT fresult; 		//wynik operacji
static FIL file;				//otwarty plik
static UINT bytes_written;		//liczba zapisanych bajtow
static UINT bytes_read;		//liczba odczytanych bajtow
static char fileNames[64][13];
static char currentFileName[13];
static char dataDescription[4];

//odtwarzacz
static uint8_t audioBuffer8;
static uint16_t audioBuffer16;

static int16_t fileNamesSize =0;

void choose();
void startSong(char* fileName);
void changeFile(int8_t changeValue);
void playSong();
void map_filenames();
