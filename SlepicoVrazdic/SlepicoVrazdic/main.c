#define F_CPU 1000000UL

#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "LCD16x2_4Bit/LCD16x2_4Bit.h"
#include "ClockManager/ClockManager.h"

#include "AlarmClockManager/AlarmClockManager.h"

void mainInit();

void updateDisplayText();
void doorOpeningManager();

void startMotorClockwise();
void startMotorAntiClockwise();
void stopMotor();

void motorTest();

#define MOTOR_DELAY 1
#define LOOP_DELAY 1000

unsigned int delay_ms = LOOP_DELAY;

char clock_date_buffer[17];
char alarm_clock_buffer[17];

unsigned int timer = UINT_MAX;
unsigned int motor_timer; //hlida jestli se motor netoci moc dlouho (-> hardware_error=true)

//jestli prave probiha otevirani dveri ci nikoli
bool door_is_opening;
uint8_t motor_direction; //0 = clockwise, 1 = anticlockwise

bool is_display_on;

//when some mechanical part stops working (v podstate jen kontroluju jestli motor nebezi moc dlouho)
bool hardware_error;

int main() {
	mainInit();
	setClockAndCalendar(0x17, 0x20, 0x50, 0x02, 0x17, 0x05, 0x21);
	
	setBacklit(true);

	setNextAlarm(1);
	
	while(1) {
		if (hardware_error) {
			stopMotor();
			
			sprintf(clock_date_buffer, "hardware error");
			sprintf(clock_date_buffer, "");
			updateDisplayText();
		} else {
			if (timer >= 1000) {
				readClock();
				readCalendar();
			
				int last_num_of_year = _year - ((_year/16)*16); //vybere z roku posledni cislo (0x21 => 0x01)
			
				sprintf(clock_date_buffer, "%02x:%02x:%02x %02x/%02x/%01x", _hour, _minute, _second, _date, _month, last_num_of_year);
				sprintf(alarm_clock_buffer, "budicek v: %02x:%02x", next_hour, next_minute);
			
				updateDisplayText();
			
				if (checkAlarm()) {
					door_is_opening = true;
					motor_direction = 0;
					motor_timer = 0;
					delay_ms = MOTOR_DELAY;
					
					setNextAlarm(1);
				}
			
				timer = 0;
			}
		
			doorOpeningManager();
			
			if ((PINB&0b00000100) == 0b00000100) {
				setBacklit(true);
				is_display_on = true;
			} else {
				lcd_clear();
				setBacklit(false);
				is_display_on = false;
			}
		}
		
		//tohle proto, ze ve funkci delay musi byt vzdy konstanta
		if (delay_ms == LOOP_DELAY)
			_delay_ms(LOOP_DELAY);
		else if (delay_ms == MOTOR_DELAY)
			_delay_ms(MOTOR_DELAY);
			
		timer += delay_ms;
	}
	
}

void mainInit() {
	DDRC = 0xff;
	DDRB = 0x00;
	
	clockInit();
	lcdinit();
	initMonthsSunRises();
	
	is_display_on = true;
}

//call ones per frame
void updateDisplayText() {
	if (is_display_on) {
		lcd_print_xy(0, 0, clock_date_buffer);
		lcd_print_xy(1, 0, alarm_clock_buffer);
	}
}

void doorOpeningManager() {
	if (door_is_opening) {
		if (motor_direction == 0)
			startMotorClockwise();
		else if (motor_direction == 1)
			startMotorAntiClockwise();
		
		
		if ((PINB&0b00000001) == 0b00000001) { //zacni zasunovat
			motor_direction = 1;
		}
		
		if ((PINB&0b00000010) == 0b00000010 && motor_direction == 1) { //zacni vysunovat
			stopMotor();
			door_is_opening = false;
			delay_ms = LOOP_DELAY;
		}
		
		motor_timer += delay_ms;
	} else {
		if (motor_timer >= 8000)
			hardware_error = true;
	}
}

void startMotorClockwise() { //set portc c to 0b000000 01
	PORTC |= 0b00000001;
	PORTC &= 0b11111101;
}

void startMotorAntiClockwise() { //set portc c to 0b000000 10
	PORTC |= 0b00000010;
	PORTC &= 0b11111110;
}

void stopMotor() {
	PORTC &= 0b11111100;
}


//call ones per frame
void motorTest() {
	if ((PINB&0b00000001) == 0b00000001) { //zacni zasunovat
		startMotorAntiClockwise();
	}
	
	if ((PINB&0b00000010) == 0b00000010) { //zacni vysunovat
		startMotorClockwise();
	}
}
