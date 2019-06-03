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

static int16_t currentFile;
static FATFS FatFs;
static FRESULT fresult;
static FIL file;
static UINT bytes_read;
static char fileNames[64][13];
static char currentFileName[13];
static uint8_t audioBuffer8;
static uint16_t audioBuffer16;
static int16_t fileNamesSize = 0;

void startSong(char *fileName);

void changeFile(int8_t changeValue);

void playSong();

void map_filenames();