#include <music.h>
#include "display.h"
#include "stm32f4xx_it.h"

extern I2C_HandleTypeDef hi2c1;
extern I2S_HandleTypeDef hi2s3;
extern DMA_HandleTypeDef hdma_spi3_tx;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

int displayCounter = 1;
int state = 0, displayState = 0;
int godzina, minuta, timerValue = 0, temp_flag = 1, flash = 0, agodzina = 0, aminuta = 0, ifalarm=0;
uint8_t alarmHour, alarmMinute;

uint8_t BCD2DEC(uint8_t data) {
    return (data >> 4) * 10 + (data & 0x0f);
}

uint8_t DEC2BCD(uint8_t data) {
    return (data / 10) << 4 | (data % 10);
}

void getRTC() {
    data_RTC[0] = 0x00;
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, data_RTC, 1, 50);
    HAL_I2C_Master_Receive(&hi2c1, 0xD0, data_RTC, 11, 50);

    godzina = BCD2DEC(data_RTC[2]);
    minuta = BCD2DEC(data_RTC[1]);
    sec = BCD2DEC(data_RTC[0]);
    agodzina = BCD2DEC(data_RTC[9]);
    aminuta = BCD2DEC(data_RTC[8]);
}

void setRTC(uint8_t shour, uint8_t smin, uint8_t ssec, uint8_t alarmHour, uint8_t alarmMinute) {
    uint8_t data_RTC[11];
    data_RTC[0] = 0x00;
    data_RTC[1] = DEC2BCD(ssec);//seconds
    data_RTC[2] = DEC2BCD(smin); //set minute
    data_RTC[3] = DEC2BCD(shour); //set hour
    data_RTC[9] = DEC2BCD(alarmMinute);
    data_RTC[10] = DEC2BCD(alarmHour);
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, data_RTC, 11, 50);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    //minuta = (int) min;
    //godzina = (int) hour;
    //aminuta = (int) alarmMinute;
    //agodzina = (int) alarmHour;
    switch (state) {
        case 0:
            displayState = 0;
            if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET)) {
                flash = 1;
                state = 1;
                for (int i = 0; i < 1000000; i++);
            }
            if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET)) {
                flash = 1;
                state = 3;
                displayState = 2;
                for (int i = 0; i < 1000000; i++);
            }
            if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == GPIO_PIN_SET)) {
                displayState=3;
                state = 5;
                temp_flag=0;
                for (int i = 0; i < 1000000; i++);
            }
            break;
            //ustawianie godziny
        case 1:
            if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET) {
                if (godzina == 23)
                    godzina = 0;
                else godzina++;
                setRTC(godzina, minuta, sec, agodzina, aminuta);
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET)) {
                if (godzina == 0)
                    godzina = 23;
                else godzina--;
                setRTC(godzina, minuta, sec, agodzina, aminuta);
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET)) {
                state = 2;
                setRTC(godzina, minuta, sec, agodzina, aminuta);
                for (int i = 0; i < 8000000; i++);
            }
            break;
        case 2:

            if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET)) {
                if (minuta == 59)
                    minuta = 0;
                else minuta++;
                setRTC(godzina, minuta, sec, agodzina, aminuta);
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET)) {
                if (minuta == 0)
                    minuta = 59;
                else minuta--;
                setRTC(godzina, minuta, sec, agodzina, aminuta);
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET)) {
                state = 0;
                for (int i = 0; i < 1000000; i++);
                flash = 0;
            }
            break;
            //ustawianie budzika
        case 3:
            displayState = 2;
            if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET)) {
                if (agodzina == 23)
                    agodzina = 0;
                else agodzina++;
                setRTC(godzina, minuta, sec, agodzina, aminuta);
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET)) {
                if (agodzina == 0)
                    agodzina = 23;
                else agodzina--;
                setRTC(godzina, minuta, sec, agodzina, aminuta);
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET)) {
                state = 4;
                setRTC(godzina, minuta, sec, agodzina, aminuta);
                for (int i = 0; i < 8000000; i++);
            }
            break;
        case 4:
            displayState = 2;
            if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET)) {
                if (aminuta == 59)
                    aminuta = 0;
                else aminuta++;
                setRTC(godzina, minuta, sec, agodzina, aminuta);
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET)) {
                if (aminuta == 0)
                    aminuta = 59;
                else aminuta--;
                setRTC(godzina, minuta, sec, agodzina, aminuta);
                for (int i = 0; i < 1000000; i++);
            }
            else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET)) {
                state = 0;
                setRTC(godzina, minuta, sec, agodzina, aminuta);
                for (int i = 0; i < 1000000; i++);
                flash = 0;
            }
            break;
        case 5:
            if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == GPIO_PIN_SET)) {
                changeFile(1);
                for (int i = 0; i < 1000000; i++);
            }
            if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET)) {
                state=0;
                temp_flag=1;
                for (int i = 0; i < 1000000; i++);
            }
    }
}
void display_value(int value){
    switch (value) {
        case 0:
            DISP_VAL_0;
            break;
        case 1:
            DISP_VAL_1;
            break;
        case 2:
            DISP_VAL_2;
            break;
        case 3:
            DISP_VAL_3;
            break;
        case 4:
            DISP_VAL_4;
            break;
        case 5:
            DISP_VAL_5;
            break;
        case 6:
            DISP_VAL_6;
            break;
        case 7:
            DISP_VAL_7;
            break;
        case 8:
            DISP_VAL_8;
            break;
        case 9:
            DISP_VAL_9;
            break;
    }
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        getRTC();
        if(aminuta==minuta&&agodzina==godzina){
            ifalarm=1;
        }
        else ifalarm=0;
    }
    if (htim->Instance == TIM3) {
        timerValue++;
        if (timerValue > 1500) timerValue = 0;
        uint8_t h1, h2, m1, m2;
        switch (displayState) {
            case 0:
                if (flash == 1) {
                    if (timerValue > 300)
                        temp_flag = 1;
                    else temp_flag = 0;
                }

                h1 = godzina / 10;
                h2 = godzina % 10;
                m1 = minuta / 10;
                m2 = minuta % 10;
                break;
            case 2:
                if (flash == 1) {
                    if (timerValue > 300)
                        temp_flag = 1;
                    else temp_flag = 0;
                }


                h1 = agodzina / 10;
                h2 = agodzina % 10;
                m1 = aminuta / 10;
                m2 = aminuta % 10;
                break;
            case 3:
                h1=currentFile;
                DISP_1_ON;
                DISP_2_OFF;
                DISP_3_OFF;
                DISP_4_OFF;
                display_value(h1);
                break;
        }
        DISP_1_OFF;
        DISP_2_OFF;
        DISP_3_OFF;
        DISP_4_OFF;
        DISP_VAL_NULL;
        if (temp_flag == 1) {
            switch (displayCounter) {
                case 1:
                    DISP_1_ON;
                    display_value(h1);
                    displayCounter++;
                    break;
                case 2:
                    DISP_2_ON;
                   display_value(h2);
                    DISP_DOT;
                    displayCounter++;
                    break;
                case 3:
                    DISP_3_ON;
                    display_value(m1);
                    displayCounter++;
                    break;
                case 4:
                    DISP_4_ON;
                    display_value(m2);
                    displayCounter = 1;
                    break;
            }
        }
    }
    if (htim->Instance == TIM4) {
        if(ifalarm==1) {
            playSong();
        }
    }

}
