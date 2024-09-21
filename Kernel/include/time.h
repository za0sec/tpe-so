#ifndef _TIME_H_
#define _TIME_H_


void timer_handler();
int ticks_elapsed();
int seconds_elapsed();
int ms_elapsed();
void timer_wait(int delta);
void sleep(int millis);

#endif