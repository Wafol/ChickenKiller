#define F_CPU 1000000UL

#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>

#include "LCD16x2_4Bit/LCD16x2_4Bit.h"
#include "ClockManager/ClockManager.h"

#include "AlarmClockManager/AlarmClockManager.h"


void mainInit();

void updateDisplayText();

void doorOpeningManager();
void firstlySwitched();
void leverHandler();

void startDoorOpening();
void startMotorClockwise();
void startMotorAntiClockwise();
void stopMotor();

void motorTest();

void set8SecondsDeepSleep();
void watchdogSetup();

#define delay_ms 1

char clock_date_buffer[17];
char alarm_clock_buffer[17];

unsigned int timer = UINT_MAX;
unsigned int motor_timer; //hlida jestli se motor netoci moc dlouho (-> hardware_error=true)

//jestli prave probiha otevirani dveri ci nikoli
bool door_is_opening;
uint8_t motor_direction; //0 = clockwise, 1 = anticlockwise

bool is_display_on;

#define TOLERATED_SWITCH_TIME 10
//kolikrat byla packa otocena rychle po sobe (aby se mohlo udelat testovaci spusteni)
uint8_t lever_counter = 0;
int lever_timer = TOLERATED_SWITCH_TIME;

bool after_8sleep;

//when some mechanical part stops working (v podstate jen kontroluju jestli motor nebezi moc dlouho)
bool hardware_error;

int main() {
	mainInit();
	setClockAndCalendar(0x20, 0x51, 0x00, 0x05, 0x20, 0x05, 0x21);
	
	setNextAlarm(1);
	
	setBacklit(true);
	
	while(1) {
		if (hardware_error) {
			stopMotor();
			
			sprintf(clock_date_buffer, "hardware error");
			sprintf(clock_date_buffer, '\0');
			updateDisplayText();
		} else {
			leverHandler();
			
			if (!door_is_opening) {
				if (!is_display_on && lever_counter == 0) {
					set8SecondsDeepSleep();
					continue;
				}
				
				if (after_8sleep) {
					lcdinit();
					after_8sleep = false;	
				}
				
				readClock();
				readCalendar();
			
				int last_num_of_year = _year - ((_year/16)*16); //vybere z roku posledni cislo (0x21 => 0x01)
				
				sprintf(clock_date_buffer, "%02x:%02x:%02x %02x/%02x/%01x", _hour, _minute, _second, _date, _month, last_num_of_year);
				sprintf(alarm_clock_buffer, "budicek v: %02x:%02x", next_hour, next_minute);
			
				updateDisplayText();
			
				if (checkAlarm()) {
					startDoorOpening();
					
					setNextAlarm(1);
				}
			} else {
				doorOpeningManager();
			}
		}
		
		_delay_ms(delay_ms);
	}
	
}

void mainInit() {
	DDRC = 0xff;
	DDRB = 0x00;
	
	PORTC |= 0b00001000;
	
	clockInit();
	lcdinit();
	initMonthsSunRises();
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	watchdogSetup();
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
		}
		
		motor_timer += delay_ms;
	} else {
		if (motor_timer >= 1000)
			hardware_error = true;
	}
}

void firstlySwitched() {
	if (lever_timer <= TOLERATED_SWITCH_TIME) {
		lever_counter++;
		
		if (lever_counter == 4) { //test door open
			lever_counter = 0;
			
			startDoorOpening();
		}
	}
	
	lever_timer = 0;
}

//call ones per frame
void leverHandler() {
	if ((PINB&0b00000100) == 0b00000100) {
		if (!is_display_on) //kontroluje jestli je prvni prubeh
			firstlySwitched();
		
		setBacklit(true);
		is_display_on = true;
	} else {
		if (is_display_on)
			firstlySwitched();
		
		lcd_clear();
		//setBacklit(false);
		is_display_on = false;
	}
	
	if (lever_timer >= TOLERATED_SWITCH_TIME) {
		lever_counter = 0;
	}
	
	if (lever_timer < TOLERATED_SWITCH_TIME && lever_counter > 0)
		lever_timer += 1;
}

void startDoorOpening() {
	door_is_opening = true;
	motor_direction = 0;
	motor_timer = 0;
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
	
	lever_counter = 0;
	lever_timer = 0;
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

void set8SecondsDeepSleep() {	
	DDRC=0x0;
	DDRD=0x0;
	
	PORTC &= 0b11110111;
	setBacklit(false);
	lcd_clear();
	
	wdt_reset();
	sleep_mode(); // sleep now!
	
	DDRC=0xff;
	DDRD=0xff;
	PORTC |= 0b00001000;
	
	lever_counter = 0;
	lever_timer = 0;
	
	after_8sleep = true;
}

void watchdogSetup() {
	cli();
	wdt_reset();
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR = (1<<WDIE) | (0<<WDE) | (1<<WDP3) | (1<<WDP0);  // 8s / interrupt, no system reset (8s is max watchdog oscillation)
	sei();
}

ISR(WDT_vect) { 
	
}
