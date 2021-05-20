#include <stdint.h>
#include "../ClockManager/ClockManager.h"
#include "AlarmClockManager.h"

struct MonthSunRises {
	//sun rise at the first day of month
	uint8_t first_hour;
	uint8_t first_minute;
	
	//sun rise at the last day of month
	uint8_t last_hour;
	uint8_t last_minute;
	
	//days in month
	uint8_t days_count;
};
float calculateDayAverageTimeChange(struct MonthSunRises months_sun_rises);
int hourAndMinuteToMinutes(uint8_t hour, uint8_t minute);

uint8_t byteHexToDecimal(int hex);
int decimalToHex(uint8_t dec);

//leden [0] -> prosinec [11]
struct MonthSunRises months_sun_rises[12];

//after_days = 1 => zitra
void setNextAlarm(uint8_t after_days) {
	readCalendar();
	uint8_t dec_month = byteHexToDecimal(_month) - 1; //-1 aby leden nebyl 1 ale 0
	uint8_t dec_date = byteHexToDecimal(_date) - 1 + after_days; //day in month (-1 -> stejny duvod)
	
	struct MonthSunRises month_sun_rises = months_sun_rises[dec_month];
	
	float average_change = calculateDayAverageTimeChange(month_sun_rises);
	int final_change = (int) (average_change*((float)dec_date)); //odchylka od (first_hour+first_hour)
	
	int first_time_minutes = hourAndMinuteToMinutes(month_sun_rises.first_hour, month_sun_rises.first_minute);
	
	int final_time_minutes = first_time_minutes + final_change;
	//minutes to hours and minutes
	uint8_t hours = final_time_minutes/60;
	uint8_t minutes = final_time_minutes - (hours*60);
	
	next_hour = decimalToHex(hours);
	next_minute = decimalToHex(minutes);
}

//vrati jestli je cas budiku stejny jak aktualni cas
bool checkAlarm() {
	return (next_hour == _hour) && (next_minute == _minute);
}

float calculateDayAverageTimeChange(struct MonthSunRises month_sun_rises) {
	int first_time = hourAndMinuteToMinutes(month_sun_rises.first_hour, month_sun_rises.first_minute);
	int last_time = hourAndMinuteToMinutes(month_sun_rises.last_hour, month_sun_rises.last_minute);
	
	int diference = last_time - first_time;
	return ((float) diference)/((float) month_sun_rises.days_count);
}

int hourAndMinuteToMinutes(uint8_t hour, uint8_t minute) {
	return (hour*60) + minute;
}

//0x21 -> 21
uint8_t byteHexToDecimal(int hex) { //max byte!!!!!!
	//in 0x21 it is digit "2"
	int second_degree = hex/16;
	int first_degree = hex - (16*second_degree);
	
	return (second_degree*10) + first_degree;
}

//21 -> 0x21
int decimalToHex(uint8_t dec) {
	uint8_t second_degree = dec/10;
	uint8_t first_degree = dec - (10*second_degree);
	
	return (second_degree*16) + first_degree;
}

void initMonthsSunRises() {
	//leden
	months_sun_rises[0].first_hour = 7;
	months_sun_rises[0].first_minute = 54;
	months_sun_rises[0].last_hour = 7;
	months_sun_rises[0].last_minute = 33;
	months_sun_rises[0].days_count = 31;
	
	//unor
	months_sun_rises[1].first_hour = 7;
	months_sun_rises[1].first_minute = 31;
	months_sun_rises[1].last_hour = 6;
	months_sun_rises[1].last_minute = 44;
	months_sun_rises[1].days_count = 28;
	
	//brezen
	months_sun_rises[2].first_hour = 6;
	months_sun_rises[2].first_minute = 42;
	months_sun_rises[2].last_hour = 6;
	months_sun_rises[2].last_minute = 38;
	months_sun_rises[2].days_count = 31;
	
	//duben
	months_sun_rises[3].first_hour = 6;
	months_sun_rises[3].first_minute = 35;
	months_sun_rises[3].last_hour = 5;
	months_sun_rises[3].last_minute = 36;
	months_sun_rises[3].days_count = 30;
	
	//kveten
	months_sun_rises[4].first_hour = 5;
	months_sun_rises[4].first_minute = 34;
	months_sun_rises[4].last_hour = 4;
	months_sun_rises[4].last_minute = 53;
	months_sun_rises[4].days_count = 31;
	
	//cerven
	months_sun_rises[5].first_hour = 4;
	months_sun_rises[5].first_minute = 52;
	months_sun_rises[5].last_hour = 4;
	months_sun_rises[5].last_minute = 49;
	months_sun_rises[5].days_count = 30;
	
	//cervenec
	months_sun_rises[6].first_hour = 4;
	months_sun_rises[6].first_minute = 50;
	months_sun_rises[6].last_hour = 5;
	months_sun_rises[6].last_minute = 21;
	months_sun_rises[6].days_count = 31;
	
	//srpen
	months_sun_rises[7].first_hour = 5;
	months_sun_rises[7].first_minute = 23;
	months_sun_rises[7].last_hour = 6;
	months_sun_rises[7].last_minute = 07;
	months_sun_rises[7].days_count = 31;
	
	//zari
	months_sun_rises[8].first_hour = 6;
	months_sun_rises[8].first_minute = 8;
	months_sun_rises[8].last_hour = 6;
	months_sun_rises[8].last_minute = 52;
	months_sun_rises[8].days_count = 30;
	
	//rijen
	months_sun_rises[9].first_hour = 6;
	months_sun_rises[9].first_minute = 53;
	months_sun_rises[9].last_hour = 6;
	months_sun_rises[9].last_minute = 41;
	months_sun_rises[9].days_count = 31;	
	
	//listopad
	months_sun_rises[10].first_hour = 6;
	months_sun_rises[10].first_minute = 43;
	months_sun_rises[10].last_hour = 7;
	months_sun_rises[10].last_minute = 29;
	months_sun_rises[10].days_count = 30;
	
	//prosinec
	months_sun_rises[11].first_hour = 7;
	months_sun_rises[11].first_minute = 30;
	months_sun_rises[11].last_hour = 7;
	months_sun_rises[11].last_minute = 54;
	months_sun_rises[11].days_count = 31;
}