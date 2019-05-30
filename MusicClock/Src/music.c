#include "music.h"
#include "main.h"
#include "diskio.h"
#include "ff.h"
#include "stm32f4xx_it.h"

extern TIM_HandleTypeDef htim4;
extern I2S_HandleTypeDef hi2s3;


void startSong(char *fileName) {
    if (fileNamesSize > 0) {
        fresult = f_open(&file, fileName, FA_READ);
        fresult = f_lseek(&file, 44);

        // HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
        // HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

        //W��czanie timera
                __HAL_TIM_SetCounter(&htim4, 0);            //Ustawienie licznika na 0
        __HAL_TIM_ENABLE_IT(&htim4, TIM_IT_UPDATE); //Aktywowanie przerwa�
        HAL_TIM_Base_Start_IT(&htim4);                //Uruchomienie licznika
    }
}

void changeFile(int8_t changeValue) {
    currentFile += changeValue;
    if (currentFile >= fileNamesSize) {
        currentFile = 0;
    } else if (currentFile < 0) {
        currentFile = fileNamesSize - 1;
    }
    memset(currentFileName, 0, sizeof(currentFileName));
    strcpy(currentFileName, fileNames[currentFile]);
    startSong(currentFileName);
}

void playSong() {
    if (!f_eof(&file)) {
        //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
        //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
        fresult = f_read(&file, &audioBuffer8, 1, &bytes_read);
        audioBuffer16 = audioBuffer8 * 50;
        // HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, audioBuffer16);
        HAL_I2S_Transmit_DMA(&hi2s3, &audioBuffer16, 1);
    } else {
        //Zatrzymanie DAC
        // HAL_DAC_Stop(&hdac, DAC_CHANNEL_1);
        // HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);

        //Wyłączanie timera
        __HAL_TIM_DISABLE_IT(&htim4, TIM_IT_UPDATE); //Dezaktywowanie przerwań
        HAL_TIM_Base_Stop(&htim4);                     //Zatrzymanie licznika
                __HAL_TIM_SetCounter(&htim4, 0);             //Ustawienie licznika na 0

        fresult = f_close(&file);

        changeFile(1);
    }
}

void map_filenames() {
    const char *path = "/";
    DIR dir;
    FILINFO fno;
    fresult = f_opendir(&dir, path);
    if (fresult == FR_OK) {
        for (;;) {
            fresult = f_readdir(&dir, &fno);
            if (fresult != FR_OK || fno.fname[0] == 0) { //B³ıd
                if (fresult != FR_OK) HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
                break;
            } else if (!(fno.fattrib & AM_DIR) && strstr(fno.fname, ".WAV") != NULL) {

                strcpy(fileNames[fileNamesSize], fno.fname);
                fileNamesSize++;
            }
        }
    } else {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
    }
    f_closedir(&dir);
}
