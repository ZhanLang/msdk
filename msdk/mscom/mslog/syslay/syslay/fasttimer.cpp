// fasttimer.cpp
// cpu ±÷”ºÏ≤‚

#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include "mslog/Syslay/fasttimer.h"


//=========================================
int64 g_ClockFreq;	// Clocks/sec
unsigned long g_dwClockFreq;
double g_MrPeriods;
double g_MsPeriods;
double g_SsPeriods;


// Constructor init the clock speed.
CClockSpeedInit g_ClockSpeedInit;

