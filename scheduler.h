#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <avr/interrupt.h>

#ifndef TIMERS_AVAILABLE
#define TIMERS_AVAILABLE 8
#endif

#if TIMERS_AVAILABLE > 64
#define TIMERS_AVAILABLE 64
#endif

unsigned long timerTable[TIMERS_AVAILABLE];
unsigned long timerReloadTable[TIMERS_AVAILABLE];

#if TIMERS_AVAILABLE > 32
unsigned long long timerActive;
unsigned long long runTimer
#elif TIMERS_AVAILABLE > 16
unsigned long timerActive;
unsigned long runTimer;
#elif TIMERS_AVAILABLE > 8
unsigned int timerActive;
unsigned int runTimer;
#else
unsigned char timerActive;
unsigned char runTimer;
#endif

typedef void (*timerFunct)(unsigned char);
timerFunct timerFunctions[TIMERS_AVAILABLE];

void initTimers(void);
void execTimers(void);
unsigned char assignTimer(void (*funcpt)(unsigned char),unsigned char timerNumber,unsigned long timeMs);
void deleteTimer(unsigned char timerNumber);
void deleteTimerByFunction(void (*funcpt)(unsigned char));
void updateTimer(unsigned char timerNumber, unsigned long timeMs);


void timerTick(void);

ISR(TIMER2_OVF_vect) {
	timerTick();
}


void no_timer(unsigned char timer) {

}

unsigned char getNextFreeTimer(void) {
	unsigned char i;
	for(i=0;i<TIMERS_AVAILABLE;i++)
		if(timerReloadTable[i] == 0)
			return i;
	return 255;
}

unsigned char getTimersUsed(void) {
	unsigned char i,retVal = 0;
	for(i=0;i<TIMERS_AVAILABLE;i++)
		if(timerReloadTable[i] > 0)
			retVal++;
	return retVal;
}

void execTimers(void) {
	//while(runTimer == 0) {};
	unsigned char i;
	for(i = 0;i < TIMERS_AVAILABLE;i++)
		if(runTimer&(1<<i)){
			runTimer &= ~(1<<i);
			(*timerFunctions[i])(i);
		}
}

void initTimers(void) {
	unsigned char i;
	for(i = 0;i < TIMERS_AVAILABLE;i++){
		timerTable[i] = 0;
		timerReloadTable[i] = 0;
		timerFunctions[i] = &no_timer; //prevent from hang
	}
	timerActive = 0;	//set flags to zero
	runTimer = 0;
	TIMSK2 |= (1<<TOIE2);	//enable interrupt TIMER2_OVF
}

void updateTimer(unsigned char timerNumber, unsigned long timeMs){
	if(timerNumber > TIMERS_AVAILABLE || timeMs == 0)
		return;
	timerTable[timerNumber] = timeMs;
	timerReloadTable[timerNumber] = timeMs;
}

unsigned char assignTimer(void (*funcpt)(unsigned char),unsigned char timerNumber,unsigned long timeMs){
	if(timerNumber > TIMERS_AVAILABLE || timeMs == 0)
		return 255;
	deleteTimerByFunction(funcpt);
	timerTable[timerNumber] = timeMs;
	timerReloadTable[timerNumber] = timeMs;
	timerActive |= (1<<timerNumber);
	timerFunctions[timerNumber] = funcpt;
	return timerNumber;
}

void deleteTimerByFunction(void (*funcpt)(unsigned char)) {
	for(unsigned char i=0;i<TIMERS_AVAILABLE;i++)
		if(timerFunctions[i] == funcpt)
			deleteTimer(i);
}

void deleteTimer(unsigned char timerNumber) {
	if(timerNumber > TIMERS_AVAILABLE)
		return;
	timerActive &= ~(1<<timerNumber);
	timerTable[timerNumber] = 0;
	timerReloadTable[timerNumber] = 0;
}

void timerTick(void) {
	for(unsigned char i = 0;i < TIMERS_AVAILABLE;i++) {
		if((timerTable[i] == 0)&&(timerActive&(1<<i))){
			timerTable[i] = timerReloadTable[i];
			runTimer |= (1<<i);
		}
		else
			timerTable[i]--;
	}
}

#endif
