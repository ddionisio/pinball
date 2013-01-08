#include "i_timer.h"

/*********************** Initialization/Deinitialization ***************/

// - Purpose:    Initializes a timer object
// - Return:     The timer if successful.  NULL if failed
PROTECTED PTIMER TimerInit()
{
	PTIMER newTimer = NULL;

	if(MemAlloc((void**)&newTimer, sizeof(TIMER)) != RETCODE_SUCCESS)
		return newTimer;

	newTimer->currentCheck = GetTickCount();
	TimerUpdateTimer(newTimer);
	newTimer->ellapsed = TimerDifference(newTimer);
	
	return newTimer;
}

// - Purpose:    Deinitializes a timer object
// - Return:     True is successful.  False if failed
PROTECTED BOOL TimerTerm (PTIMER timerObj)
{
	if(MemFree((void**)&timerObj) != RETCODE_SUCCESS)
		return false;

	return true;
}

/*********************** Regulation ************************************/

// - Purpose:    Sets the currentCheck to the current tick count
// - Return:     none
PUBLIC  void TimerCheckTime (PTIMER timerObj)
{
	timerObj->currentCheck = GetTickCount();
	timerObj->ellapsed = TimerDifference(timerObj);
}

// - Purpose:    Sets the lastCheck to the currentCheck
// - Return:     none
PUBLIC  void TimerUpdateTimer (PTIMER timerObj)
{
	timerObj->lastCheck = timerObj->currentCheck;
	timerObj->ellapsed = TimerDifference(timerObj);
}

// - Purpose:    Gets the currentCheck 
// - Return:     Value of currentCheck
PUBLIC  Dword TimerGetCurrent (PTIMER timerObj)
{
	return timerObj->currentCheck;
}

// - Purpose:    Gets the lastCheck 
// - Return:     Value of lastCheck
PUBLIC  Dword TimerGetLast (PTIMER timerObj)
{
	return timerObj->lastCheck;
}

// - Purpose:    Gets the ellapsed time
// - Return:     Value of ellapsed
PUBLIC  Dword TimerGetEllapsed (PTIMER timerObj)
{
	return timerObj->ellapsed;
}

// - Purpose:    Calculates the amount of time passed since the last Update
// - Return:     currentCheck - lastCheck = difference in time ellapsed
PUBLIC  Dword TimerDifference (PTIMER timerObj)
{
	return timerObj->currentCheck - timerObj->lastCheck;
}

// - Purpose:    Used to delay an event from happening
// - Return:     none
PUBLIC  void TimerIncLast (PTIMER timerObj, Dword amount)
{
	timerObj->lastCheck += amount;
	timerObj->ellapsed = TimerDifference(timerObj);
}

// - Purpose:    Used to speed up an event
// - Return:     none
PUBLIC  void TimerDecLast (PTIMER timerObj, Dword amount)
{
	timerObj->lastCheck -= amount;
	timerObj->ellapsed = TimerDifference(timerObj);
}