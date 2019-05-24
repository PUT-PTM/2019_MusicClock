#include "music.h"
#include "main.h"
#include "diskio.h"
#include "ff.h"
#include "stm32f4xx_it.h"

int32_t sampleRate=0;
int16_t audioBits=0;

int32_t sChunkSize = 0;
int32_t hChunkSize = 0;

int16_t fileNamesSize =0;
DAC_HandleTypeDef hdac;

void choose(){
	if(sampleRate==44100) { htim4.Instance->ARR = 951; }
	else if(sampleRate == 8000)  { TIM4->ARR = 5249; }
	else if(sampleRate == 16000) { TIM4->ARR = 2624; }
	else if(sampleRate == 22050) { TIM4->ARR = 1903; }
	else if(sampleRate == 24000) { TIM4->ARR = 1749; }
	else if(sampleRate == 32000) { TIM4->ARR = 1311; }
	else if(sampleRate == 48000) { TIM4->ARR = 874;  }
	else if(sampleRate == 88200) { TIM4->ARR = 475;  }
	else if(sampleRate == 96000) { TIM4->ARR = 436;  }
	else { htim4.Instance->ARR = 951; }
}
void startSong(char* fileName){
	if(fileNamesSize > 0){
		fresult = f_open(&file, fileName, FA_READ);
		fresult = f_lseek(&file, f_tell(file) + 16);
		fresult = f_read(&file, &sChunkSize, 4, &bytes_read);
		fresult = f_lseek(&file, f_tell(&file) + 4);
		fresult = f_read(&file, &sampleRate, 4, &bytes_read);

		choose();

		fresult = f_lseek(&file, f_tell(&file) + 6 );
		fresult = f_read(&file, &audioBits, 2, &bytes_read);

		sChunkSize -= 16;
		fresult = f_lseek(&file, f_tell(&file) + sChunkSize );
		memset(dataDescription, 0, sizeof(dataDescription));
		fresult = f_read(&file, &dataDescription, 4, &bytes_read);

		if(strcmp(dataDescription,"DATA") == 0){
			fresult = f_lseek(&file, f_tell(&file) + 4 );
		}
		else {
			fresult = f_read(&file, &hChunkSize, 4, &bytes_read);
			hChunkSize += 8;
			fresult = f_lseek(&file, f_tell(&file) + hChunkSize );
		}
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
			HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

			//W��czanie timera
			__HAL_TIM_SetCounter(&htim4, 0);			//Ustawienie licznika na 0
			__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_UPDATE); //Aktywowanie przerwa�
			HAL_TIM_Base_Start_IT(&htim4);				//Uruchomienie licznika
	}
}
void changeFile(int8_t changeValue){
	currentFile+=changeValue;
	if(currentFile >= fileNamesSize){
		currentFile=0;
	}
	else if(currentFile < 0){
		currentFile = fileNamesSize -1;
	}
	memset(currentFileName, 0, sizeof(currentFileName));
	strcpy(currentFileName, fileNames[currentFile]);
	startSong(currentFileName);
}
void playSong() {

	if (!f_eof(&file))
		{
			if(audioBits == 16) {
				fresult = f_read(&file, &audioBuffer16, 2, &bytes_read);
				audioBuffer16 = (double)audioBuffer16 / 60.0;
				HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, audioBuffer16);
				HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
			}
			else if (audioBits == 8) {
				fresult = f_read(&file, &audioBuffer8, 1, &bytes_read);
				audioBuffer16 = (double)audioBuffer8 / 60.0;
				HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, audioBuffer16);
				HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
			}
		}
		else
		{

			//Zatrzymanie DAC
			HAL_DAC_Stop(&hdac, DAC_CHANNEL_1);
			HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);

			//Wyłączanie timera
			__HAL_TIM_DISABLE_IT(&htim4, TIM_IT_UPDATE); //Dezaktywowanie przerwań
			HAL_TIM_Base_Stop(&htim4);			         //Zatrzymanie licznika
			__HAL_TIM_SetCounter(&htim4, 0);	         //Ustawienie licznika na 0

			fresult = f_close(&file);

			changeFile(1);
		}
}
void map_filenames()
{
	const char* path = "/";
	DIR dir;
	FILINFO fno;
	fresult = f_opendir(&dir, path);
	if (fresult == FR_OK) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	    for (;;) {
	        fresult = f_readdir(&dir, &fno);
	        if (fresult != FR_OK || fno.fname[0] == 0) { //B³ıd
	        	if(fresult!= FR_OK) HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
				break;
			}
	        else if (!(fno.fattrib & AM_DIR) && strstr(fno.fname,".WAV") != NULL) {
	        	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	        	strcpy(fileNames[fileNamesSize], fno.fname);
				fileNamesSize++;
	        }
	    }
	}
	else {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	}
	f_closedir(&dir);
}
