#include "display.h"
#include "stm32f4xx_it.h"

const uint8_t daysInMonth [] PROGMEM = {31,28,31,30,31,30,31,31,30,31,30,31};
int licznik = 1;
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d);

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
//call data from RTC
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
///set data and time
void setRTC(uint8_t tgl, uint8_t bln, uint16_t thn, uint8_t jam, uint8_t mnt, uint8_t dtk){
	uint8_t data_RTC[8];
	data_RTC[0] = 0x00;
	data_RTC[1] = DEC2BCD(dtk);//seconds
	data_RTC[2] = DEC2BCD(mnt); //set minute
	data_RTC[3] = DEC2BCD(jam); //set hour

	data_RTC[5] = DEC2BCD(tgl);  //set date
	data_RTC[6] = DEC2BCD(bln); //set month
	data_RTC[7] = DEC2BCD(thn-2000);   //set year

	HAL_I2C_Master_Transmit(&hi2c1,0xD0,data_RTC,8,50);
	HAL_Delay(100);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)
	{
		uint8_t h1=hour/10;
		uint8_t h2 = hour%10;
		uint8_t m1=min/10;
		uint8_t m2 = min%10;
		DISP_1_OFF;
		DISP_2_OFF;
		DISP_3_OFF;
		DISP_4_OFF;
		DISP_VAL_NULL;
		switch(licznik){
		  case 1:
		   DISP_1_ON;
		   switch (h1) {
		   	   case 0: DISP_VAL_0; break;
		   	   case 1: DISP_VAL_1; break;
		   	   case 2: DISP_VAL_2; break;
		    	}
		    	licznik++;
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
		   licznik++;
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
		    licznik++;
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
		    licznik=1;
		    break;
		   }
	}
	else if (htim->Instance == TIM4)
		{
			playSong();
		}
}

