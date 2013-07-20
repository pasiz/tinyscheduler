tinyscheduler
=============

small scheduler written for atmel avr 328


To use the scheduler, assign timer2 overflow to 1ms and use the
```
	initTimers();
```

scheduled function must be:

```
void scheduleFunction(unsigned char selfTimer)
```

inside function you can call deleteTimer(selfTimer) to stop scheduling



when you want to assign task to shedule, use assignTimer(pointer_to_function,timernumber, milliseconds)

There are helper that gets next free timer to prevent overlapping

```
	assignTimer(&updateDisplay,getNextFreeTimer(),500);
```

To delete a timer from another function, you can use deleteTimerByFunction(pointer_to_function)

```
deleteTimerByFunction(&scheduleFunction);
```


