#include "music.h"
#include "stm32f4xx_it.h"

int32_t sampleRate=0;
int16_t audioBits=0;

int32_t sChunkSize = 0;
int32_t hChunkSize = 0;

int16_t fileNamesSize =0;
int16_t currentFile=0;

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

		//i2c/s
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
			//HAL_I2S_Transmit_DMA(&hi2s3, audioBuffer16*8000, &bytes_read);
		}
		else if (audioBits == 8) {
			fresult = f_read(&file, &audioBuffer8, 1, &bytes_read);
			audioBuffer16 = (double)audioBuffer8 / 60.0;
			//HAL_I2S_Transmit_DMA(&hi2s3, audioBuffer16*8000, &bytes_read);
		}
	}
	else
	{
		fresult = f_close(&file);
		change_file(1);
	}
}

