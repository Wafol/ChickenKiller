#ifndef ALARMCLOCKMANAGER_H_
#define ALARMCLOCKMANAGER_H_

#include <stdbool.h>

//time when will next alarm clock fire
//in hexdecimal (= 35 minut je 0x35)
int next_hour;
int next_minute;

void setNextAlarm(uint8_t after_days);
bool checkAlarm();

void initMonthsSunRises();

#endif /* ALARMCLOCKMANAGER_H_ */