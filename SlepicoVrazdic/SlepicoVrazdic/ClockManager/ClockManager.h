#ifndef CLOCKMANAGER_H_
#define CLOCKMANAGER_H_

int _second,_minute,_hour,_day,_date,_month,_year;

//when you wanna get you write -> days[day-1]
char* days[7];

void clockInit();

void readClock();
void readCalendar();

void setClockAndCalendar(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t date, uint8_t month, uint8_t year);

void stopClock();


#endif /* CLOCKMANAGER_H_ */