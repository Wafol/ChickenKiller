#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "../I2C_Master_H_file/I2C_Master_H_file.h"
#include "ClockManager.h"

/////////////read/////////////////////////

#define Device_Write_address	0xD0				// Define RTC DS1307 slave address for write operation
#define Device_Read_address		0xD1				// Make LSB bit high of slave address for read operation
#define TimeFormat12			0x40				// Define 12 hour format
//#define AMPM					0x20

void RTC_Read_Clock(char read_clock_address); // Read the clock with second address i.e location is 0 (RTC_Read_Clock(0);)
void RTC_Read_Calendar(char read_calendar_address);// Read the calender with day address i.e location is 3 (RTC_Read_Calendar(3);)

void setClockAndCalendar(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t date, uint8_t month, uint8_t year);
void RTC_Calendar_Write(char day, char date, char month, char year); // function for calendar
void RTC_Clock_Write(char hour, char minute, char second, char AMPM);

char* days[7] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};

void clockInit() {
	I2C_Init();										// Initialize I2C
}

//data will be stored at second,minute... int variables (at ClockManager.h)
void readClock() {
	RTC_Read_Clock(0);
}

//data will be stored at date,month... int variables (at ClockManager.h)
void readCalendar() {
	RTC_Read_Calendar(3);
}

void RTC_Read_Clock(char read_clock_address) {
	I2C_Start(Device_Write_address);				// Start I2C communication with RTC
	I2C_Write(read_clock_address);					// Write address to read
	I2C_Repeated_Start(Device_Read_address);		// Repeated start with device read address

	_second = I2C_Read_Ack();						// Read second
	_minute = I2C_Read_Ack();						// Read minute
	_hour = I2C_Read_Nack();							// Read hour with Nack
	I2C_Stop();										// Stop i2C communication
}
void RTC_Read_Calendar(char read_calendar_address) {
	I2C_Start(Device_Write_address);
	I2C_Write(read_calendar_address);
	I2C_Repeated_Start(Device_Read_address);

	_day = I2C_Read_Ack();							// Read day
	_date = I2C_Read_Ack();							// Read date
	_month = I2C_Read_Ack();							// Read month
	_year = I2C_Read_Nack();							// Read the year with Nack
	I2C_Stop();										// Stop i2C communication
}



///////////////write////////////////////
#define hour_24					0x00

void RTC_Clock_Write(char hour, char minute, char second, char AMPM) {
	hour |= AMPM;
	I2C_Start(Device_Write_address);			// Start I2C communication with RTC
	I2C_Write(0);								// Write on 0 location for second value
	I2C_Write(second);							// Write second value on 00 location
	I2C_Write(minute);							// Write minute value on 01 location
	I2C_Write(hour);							// Write hour value on 02 location
	I2C_Stop();									// Stop I2C communication
}

void RTC_Calendar_Write(char day, char date, char month, char year)	{ // function for calendar

	I2C_Start(Device_Write_address);			// Start I2C communication with RTC
	I2C_Write(3);								// Write on 3 location for day value
	I2C_Write(day);							// Write day value on 03 location
	I2C_Write(date);							// Write date value on 04 location
	I2C_Write(month);							// Write month value on 05 location
	I2C_Write(year);							// Write year value on 06 location
	I2C_Stop();									// Stop I2C communication
}


//day -> 0x00-Sobota, 0x06-Patek
void setClockAndCalendar(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t date, uint8_t month, uint8_t year) {
	RTC_Clock_Write(hour, minute, second, hour_24);// Write Hour Minute Second Format
	RTC_Calendar_Write(day, date, month, year);	// Write day date month and year
}