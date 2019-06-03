#include <music.h>
#include "display.h"
#include "stm32f4xx_it.h"

extern I2C_HandleTypeDef hi2c1;
extern I2S_HandleTypeDef hi2s3;
extern DMA_HandleTypeDef hdma_spi3_tx;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

int displayCounter = 1, songNumber = 1;
int state = 0, displayState = 0;
int clockHour, clockMinute, flashValue = 0, displayOn = 1, isFlashing = 0, alarmHour = 0, alarmMinute = 0, alarmRinging = 0;
int napSet = 1;

uint8_t BCD2DEC(uint8_t data) {
    return (data >> 4u) * 10 + (data & 0x0fu);
}

uint8_t DEC2BCD(uint8_t data) {
    return (data / 10u) << 4u | (data % 10u);
}

void getRTC() {
    data_RTC[0] = 0x00;
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, data_RTC, 1, 50);
    HAL_I2C_Master_Receive(&hi2c1, 0xD0, data_RTC, 11, 50);

    clockHour = BCD2DEC(data_RTC[2]);
    clockMinute = BCD2DEC(data_RTC[1]);
    alarmHour = BCD2DEC(data_RTC[9]);
    alarmMinute = BCD2DEC(data_RTC[8]);
}

void setRTC(uint8_t setClockHour, uint8_t setClockMinute, uint8_t setAlarmHour, uint8_t setAlarmMinute) {
    data_RTC[0] = 0x00;
    data_RTC[2] = DEC2BCD(setClockMinute);
    data_RTC[3] = DEC2BCD(setClockHour);
    data_RTC[9] = DEC2BCD(setAlarmMinute);
    data_RTC[10] = DEC2BCD(setAlarmHour);
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, data_RTC, 11, 50);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch (state) {
        case 0:
            displayState = 0;
            if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET)) {
                isFlashing = 1;
                state = 1;
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET)) {
                isFlashing = 1;
                state = 3;
                displayState = 2;
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == GPIO_PIN_SET)) {
                displayState = 3;
                state = 5;
                displayOn = 0;
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET) && alarmRinging == 1) {
                alarmRinging = 0;
                for (int i = 0; i < 1000000; i++);
            }
            break;
        case 1:
            if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET) {
                if (clockHour == 23)
                    clockHour = 0;
                else clockHour++;
                setRTC(clockHour, clockMinute, alarmHour, alarmMinute);
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET)) {
                if (clockHour == 0)
                    clockHour = 23;
                else clockHour--;
                setRTC(clockHour, clockMinute, alarmHour, alarmMinute);
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET)) {
                state = 2;
                setRTC(clockHour, clockMinute, alarmHour, alarmMinute);
                displayOn = 1;
                for (int i = 0; i < 5000000; i++);
            }
            break;
        case 2:
            if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET)) {
                if (clockMinute == 59)
                    clockMinute = 0;
                else clockMinute++;
                setRTC(clockHour, clockMinute, alarmHour, alarmMinute);
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET)) {
                if (clockMinute == 0)
                    clockMinute = 59;
                else clockMinute--;
                setRTC(clockHour, clockMinute, alarmHour, alarmMinute);
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET)) {
                state = 0;
                displayOn = 1;
                for (int i = 0; i < 1000000; i++);
                isFlashing = 0;
            }
            break;
        case 3:
            displayState = 2;
            if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET)) {
                if (alarmHour == 23)
                    alarmHour = 0;
                else alarmHour++;
                setRTC(clockHour, clockMinute, alarmHour, alarmMinute);
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET)) {
                if (alarmHour == 0)
                    alarmHour = 23;
                else alarmHour--;
                setRTC(clockHour, clockMinute, alarmHour, alarmMinute);
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET)) {
                state = 4;
                displayOn = 1;
                setRTC(clockHour, clockMinute, alarmHour, alarmMinute);
                for (int i = 0; i < 3000000; i++);
            }
            break;
        case 4:
            displayState = 2;
            if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET)) {
                if (alarmMinute == 59)
                    alarmMinute = 0;
                else alarmMinute++;
                setRTC(clockHour, clockMinute, alarmHour, alarmMinute);
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET)) {
                if (alarmMinute == 0)
                    alarmMinute = 59;
                else alarmMinute--;
                setRTC(clockHour, clockMinute, alarmHour, alarmMinute);
                for (int i = 0; i < 1000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET)) {
                state = 0;
                displayOn = 1;
                setRTC(clockHour, clockMinute, alarmHour, alarmMinute);
                for (int i = 0; i < 1000000; i++);
                isFlashing = 0;
            }
            break;
        default:
            break;
        case 5:
            if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == GPIO_PIN_SET)) {
                changeFile(1);
                songNumber++;
                if (songNumber > 3)
                    songNumber = 1;
                for (int i = 0; i < 2000000; i++);
            } else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET)) {
                state = 0;
                displayOn = 1;
                for (int i = 0; i < 1000000; i++);
            }
    }
}

void display_value(int value) {
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
        default:
            break;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        getRTC();
        if (alarmMinute == clockMinute && alarmHour == clockHour && napSet == 1) {
            alarmRinging = 1;
            napSet = 0;
        } else if (alarmMinute != clockMinute && alarmHour != clockHour && napSet == 0) {
            napSet = 1;
        }
    }
    if (htim->Instance == TIM3) {
        flashValue++;
        if (flashValue > 1000)
            flashValue = 0;
        uint8_t h1, h2, m1, m2;
        switch (displayState) {
            case 0:
                if (isFlashing == 1) {
                    if (flashValue > 100)
                        displayOn = 1;
                    else {
                        displayOn = 0;
                    }
                }
                h1 = clockHour / 10;
                h2 = clockHour % 10;
                m1 = clockMinute / 10;
                m2 = clockMinute % 10;
                break;
            case 2:
                if (isFlashing == 1) {
                    if (flashValue > 300)
                        displayOn = 1;
                    else displayOn = 0;
                }
                h1 = alarmHour / 10;
                h2 = alarmHour % 10;
                m1 = alarmMinute / 10;
                m2 = alarmMinute % 10;
                break;
            case 3:
                DISP_1_ON;
                DISP_2_OFF;
                DISP_3_OFF;
                DISP_4_OFF;
                DISP_VAL_NULL;
                DISP_DOT;
                display_value(songNumber);
                break;
            default:
                break;
        }
        if (displayState != 3) {
            DISP_1_OFF;
            DISP_2_OFF;
            DISP_3_OFF;
            DISP_4_OFF;
            DISP_VAL_NULL;
        }
        if (displayOn == 1) {
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
                default:
                    break;
            }
        }
    }
    if (htim->Instance == TIM4) {
        if (alarmRinging == 1) {
            playSong();
        }
    }

}