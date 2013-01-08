#ifndef _timer_h
#define _timer_h

#include "..\common.h"

typedef struct _TIMER {
	Dword lastCheck;		//Tick count at previous check
	Dword currentCheck;		//Current tick count at check
	Dword ellapsed;			//Stores the ellapsed time between checks
} TIMER, * PTIMER;

/*********************** Initialization/Deinitialization ***************/

// - Purpose:    Initializes a timer object
// - Return:     The timer if successful.  NULL if failed
PROTECTED PTIMER TimerInit();

// - Purpose:    Deinitializes a timer object
// - Return:     True is successful.  False if failed
PROTECTED BOOL TimerTerm (PTIMER timerObj);

/*********************** Regulation ************************************/

// - Purpose:    Sets the currentCheck to the current tick count
// - Return:     none
PUBLIC  void TimerCheckTime (PTIMER timerObj);

// - Purpose:    Sets the lastCheck to the currentCheck
// - Return:     none
PUBLIC  void TimerUpdateTimer (PTIMER timerObj);

// - Purpose:    Gets the currentCheck 
// - Return:     Value of currentCheck
PUBLIC  Dword TimerGetCurrent (PTIMER timerObj);

// - Purpose:    Gets the lastCheck 
// - Return:     Value of lastCheck
PUBLIC  Dword TimerGetLast (PTIMER timerObj);

// - Purpose:    Gets the ellapsed time
// - Return:     Value of ellapsed
PUBLIC  Dword TimerGetEllapsed (PTIMER timerObj);

// - Purpose:    Calculates the amount of time passed since the last Update
// - Return:     currentCheck - lastCheck = difference in time ellapsed
PUBLIC  Dword TimerDifference (PTIMER timerObj);

// - Purpose:    Used to delay an event from happening
// - Return:     none
PUBLIC  void TimerIncLast (PTIMER timerObj, Dword amount);

// - Purpose:    Used to speed up an event process
// - Return:     none
PUBLIC  void TimerDecLast (PTIMER timerObj, Dword amount);

#endif