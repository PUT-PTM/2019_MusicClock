#include "display.h"
#include "stm32f4xx_it.h"

const uint8_t daysInMonth [] PROGMEM = {31,28,31,30,31,30,31,31,30,31,30,31};
int displayCounter = 1;
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d);
int state = 0, displayState=0;
int godzina,minuta,budzikgodzina,budzikminuta,timerValue=0,temp_flag=1,flash=0;
uint8_t BCD2DEC(uint8_t data){
	return (data>>4)*10+(data&0x0f);
}

uint8_t DEC2BCD(uint8_t data){
	return (data/10)<<4|(data%10);
}

uint16_t date2days(uint16_t y, uint8_t m, uint8_t d){
	if(y>=2000){
		y-=2000;
	}
	uint16_t days = d;
	for(uint8_t i=1; i<m; ++i){
		days += pgm_read_byte (daysInMonth + i - 1);
	}
	if (m>2 && y%4==0){
		++days;
	}
	return days+365*y+(y+3)/4-1;
}

uint8_t dayOfTheWeek(int thn,int bln,int tgl){
	uint16_t day = date2days(thn,bln,tgl);
	return (day+6)%7;
}

void getRTC(){
	data_RTC[0]=0x00;
	HAL_I2C_Master_Transmit(&hi2c1,0xD0,data_RTC,1,50);
	HAL_I2C_Master_Receive(&hi2c1,0xD0,data_RTC,7,50);

	hour = BCD2DEC(data_RTC[2]);
	min = BCD2DEC(data_RTC[1]);
	sec = BCD2DEC(data_RTC[0]);
	year2digit = BCD2DEC(data_RTC[6]);
	month = BCD2DEC(data_RTC[5]);
	date = BCD2DEC(data_RTC[4]);

	year4digit = 2000+(year2digit%100);
	day = dayOfTheWeek(year2digit,month,date);
}

void setRTC(uint8_t sdate, uint8_t smonth, uint16_t syear, uint8_t shour, uint8_t smin, uint8_t ssec){
	uint8_t data_RTC[8];
	data_RTC[0] = 0x00;
	data_RTC[1] = DEC2BCD(ssec);//seconds
	data_RTC[2] = DEC2BCD(smin); //set minute
	data_RTC[3] = DEC2BCD(shour); //set hour

	data_RTC[5] = DEC2BCD(sdate);  //set date
	data_RTC[6] = DEC2BCD(smonth); //set month
	data_RTC[7] = DEC2BCD(syear-2000);   //set year

	HAL_I2C_Master_Transmit(&hi2c1,0xD0,data_RTC,8,50);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(state){
	case 0:
		displayState=0;
		if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET))
		{
			flash=1;
			state=1;
			minuta=(int)min;
			godzina=(int)hour;
			for(int i=0;i<1000000;i++);
		}
		if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET))
		{
			flash=1;
			state=3;
			displayState=2;
			for(int i=0;i<1000000;i++);
		}
		break;
	//ustawianie godziny
	case 1:

		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET){
			if(godzina==23)
				godzina=0;
			else godzina++;
			setRTC(date,month,year4digit,godzina,minuta,sec);
			for(int i=0;i<1000000;i++);
		}
		else if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET)){
			if(godzina==0)
				godzina=23;
			else godzina--;
			setRTC(date,month,year4digit,godzina,minuta,sec);
			for(int i=0;i<1000000;i++);
		}
		else if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET)){
			state=2;
			setRTC(date,month,year4digit,godzina,minuta,sec);
			for(int i=0;i<8000000;i++);
		}
		break;
	case 2:

		if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET))
		{
			if(minuta==59)
				minuta=0;
			else minuta++;
			setRTC(date,month,year4digit,godzina,minuta,sec);
			for(int i=0;i<1000000;i++);
		}
		else if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET))
		{
			if(minuta==0)
				minuta=59;
			else minuta--;
			setRTC(date,month,year4digit,godzina,minuta,sec);
			for(int i=0;i<1000000;i++);
		}
		else if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET))
		{
			state=0;
			for(int i=0;i<1000000;i++);
			flash=0;
		}
		break;
	//ustawianie budzika
	case 3:
		displayState=2;
		if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET))
		{
			if(budzikgodzina==23)
				budzikgodzina=0;
			else budzikgodzina++;
			for(int i=0;i<1000000;i++);
		}
		else if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET))
		{
			if(budzikgodzina==0)
				budzikgodzina=23;
			else budzikgodzina--;
			for(int i=0;i<1000000;i++);
		}
		else if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET))
		{
			state=4;
			for(int i=0;i<8000000;i++);
		}
		break;
	case 4:
		displayState=2;
		if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET))
		{
			if(budzikminuta==59)
				budzikminuta=0;
			else budzikminuta++;
			for(int i=0;i<1000000;i++);
		}
		else if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET))
		{
			if(budzikminuta==0)
				budzikminuta=59;
			else budzikminuta--;
			for(int i=0;i<1000000;i++);
		}
		else if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET))
		{
			state=0;
			for(int i=0;i<1000000;i++);
			flash=0;
		}
		break;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)
	{
		timerValue++;
		if(timerValue>1500) timerValue=0;
		getRTC();
		uint8_t h1,h2,m1,m2;
		switch(displayState){
		case 0:
			if(flash==1){
				if(timerValue>300)
					temp_flag=1;
				else temp_flag=0;
			}

			h1=hour/10;
			h2 = hour%10;
			m1=min/10;
			m2 = min%10;
			break;
		case 2:
			if(flash==1){
				if(timerValue>300)
					temp_flag=1;
				else temp_flag=0;
			}


			h1=budzikgodzina/10;
			h2 = budzikgodzina%10;
			m1=budzikminuta/10;
			m2 = budzikminuta%10;
			break;
		}
			DISP_1_OFF;
			DISP_2_OFF;
			DISP_3_OFF;
			DISP_4_OFF;
			DISP_VAL_NULL;
			if(temp_flag==1){
			switch(displayCounter){
			case 1:
				DISP_1_ON;
				switch (h1) {
				case 0: DISP_VAL_0; break;
				case 1: DISP_VAL_1; break;
				case 2: DISP_VAL_2; break;
		    		}
		    		displayCounter++;
		    		break;
		    case 2:
		    	DISP_2_ON;
		    	switch (h2) {
		    	case 0: DISP_VAL_0; break;
		    	case 1: DISP_VAL_1; break;
		    	case 2: DISP_VAL_2; break;
		    	case 3: DISP_VAL_3; break;
		    	case 4: DISP_VAL_4; break;
		    	case 5: DISP_VAL_5; break;
		    	case 6: DISP_VAL_6; break;
		    	case 7: DISP_VAL_7; break;
		    	case 8: DISP_VAL_8; break;
		    	case 9: DISP_VAL_9; break;
		    	}
		    	DISP_DOT;
		    	displayCounter++;
		    	break;
		    case 3:
		    	DISP_3_ON;
		    	switch (m1) {
		    	case 0: DISP_VAL_0; break;
		    	case 1: DISP_VAL_1; break;
		    	case 2: DISP_VAL_2; break;
		    	case 3: DISP_VAL_3; break;
		    	case 4: DISP_VAL_4; break;
		    	case 5: DISP_VAL_5; break;
		    	case 6: DISP_VAL_6; break;
		    	}
		    	displayCounter++;
		    	break;
		    case 4:
		    	DISP_4_ON;
		    	switch (m2) {
		    	case 0: DISP_VAL_0; break;
		    	case 1: DISP_VAL_1; break;
		    	case 2: DISP_VAL_2; break;
		    	case 3: DISP_VAL_3; break;
		    	case 4: DISP_VAL_4; break;
		    	case 5: DISP_VAL_5; break;
		    	case 6: DISP_VAL_6; break;
		    	case 7: DISP_VAL_7; break;
		    	case 8: DISP_VAL_8; break;
		    	case 9: DISP_VAL_9; break;
		    	}
		    	displayCounter=1;
		    	break;
			}
			}
	}
	else if (htim->Instance == TIM4)
		{
			playSong();
		}
}
