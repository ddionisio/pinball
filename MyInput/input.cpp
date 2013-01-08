#include "input.h"

//This includes the direct input device(mouse, keyboard, joystick)
//TrioInput G_diitems;

//DImouse G_mouse_data;



mouse themouse;
BYTE Keyboard[MaxKey]; //The whole keyboard TRUE(1) if pressed

/**********************************************************
;
;	Name:		get_mouse_loc
;
;	Purpose:	to get the mouse location
;
;	Input:		themouse
;
;	Output:		the location in COORD
;
;	Return:		A COORD with X & Y
;
**********************************************************/

COORD get_mouse_loc(void)
{
	return themouse.location;
}

/**********************************************************
;
;	Name:		is_mouse_button_down
;
;	Purpose:	To determine whether a given mouse
;				button is down	
;
;	Input:		An enumerated index into the mouse
;				button array is given as input
;
;	Output:		A bool value which determines if it is down
;
;	Return:		TRUE/FALSE
;
**********************************************************/

BOOL is_mouse_button_down (enum eMouseButton which)
{
	return themouse.button[which];
}

/**********************************************************
;
;	Name:		mouse_is_hidden
;
;	Purpose:	determine if mouse is hidden or not
;
;	Input:		none
;
;	Output:		A bool value which determines if hidden or not
;
;	Return:		TRUE/FALSE
;
**********************************************************/

BOOL mouse_is_hidden(void)
{
	return themouse.isHidden;
}

/**********************************************************
;
;	Name:		hide_mouse
;
;	Purpose:	Hides the mouse cursor
;
;	Input:		The isHidden field of the global
;				mouse structure	
;
;	Output:		Changes the isHidden field of the	
;				global mouse structure	
;
;	Return:		Nothing	
;
**********************************************************/

void hide_mouse(void)
{
	if(!themouse.isHidden)
		themouse.isHidden = TRUE;
	ShowCursor (FALSE);
}

/**********************************************************
;
;	Name:		show_mouse
;
;	Purpose:	Shows the mouse cursor	
;
;	Input:		The isHidden field of the global
;				mouse structure	
;
;	Output:		Changes the isHidden field of the
;				global mouse structure
;
;	Return:		Nothing	
;
**********************************************************/

void show_mouse(void)
{
	if(themouse.isHidden)
		themouse.isHidden = FALSE;
	ShowCursor (TRUE);
}

/**********************************************************
;
;	Name:		update_mouse_button	
;
;	Purpose:	To update the button array of the
;				global mouse structure	
;
;	Input:		An index into the button array and a
;				button status flag are given as input
;
;	Output:		The button array is updated according
;				to its input
;
;	Return:		Nothing	
;
**********************************************************/

void update_mouse_button(enum eMouseButton which, BOOL status)
{
	themouse.button[which] = status;
}

/**********************************************************
;
;	Name:		update_mouse_loc
;
;	Purpose:	To update the current mouse location
;
;	Input:		The address of a tPoint structure to
;				be filled in
;
;	Output:		it_GetMouseLocation returns the	current 
;				mouse location, which is passed
;				to the input structure	
;
;	Return:		Nothing
;
**********************************************************/
void update_mouse_loc(COORD * loc)
{
	themouse.location.X = loc->X;
	themouse.location.Y = loc->Y;
}

/**********************************************************
;
;	Name:		any_key_is_pressed
;
;	Purpose:	Determines whether any of the keyboard
;				keys are depressed
;
;	Input:		Global keyboard array
;
;	Output:		N/A	
;
;	Return:		Returns TRUE/FALSE if a key is down
;
**********************************************************/

BOOL any_key_is_pressed(void)
{
	int count;

	for (count = 0; count < MaxKey; count++)
	{
		if (Keyboard[count] & 0x80)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/**********************************************************
;
;	Name:		is_key_pressed
;
;	Purpose:	Determines whether the specific key
;				is pressed
;
;	Input:		Takes a keycode as input
;
;	Output:		N/A	
;
;	Return:		Returns TRUE/FALSE if the key is pressed
;
**********************************************************/
BOOL is_key_pressed(BYTE kcode)
{
	return (Keyboard[kcode] & 0x80) ? TRUE : FALSE;
}

/**********************************************************
;
;	Name:		
;
;	Purpose:	
;
;	Input:		
;
;	Output:		
;
;	Return:		
;
**********************************************************/
/*char get_ascii(BYTE kcode)
{
	char hey = 0;
	if(int ToAscii(
		kcode,           // virtual-key code
		UINT uScanCode,          // scan code
		CONST PBYTE lpKeyState,  // key-state array
		LPWORD lpChar,           // buffer for translated key
		UINT uFlags              // active-menu option
		) == 1)

}*/

/**********************************************************
;
;	Name:		update_keyboard
;
;	Purpose:	Updates keyboard array with regard
;				to which keys are currently depressed
;
;	Input:		None				
;
;	Output:		The keyboard array is updated according
;				to its input
;
;	Return:		Nothing		
;
**********************************************************/

void update_keyboard()
{
	GetKeyboardState (Keyboard);
}

/**********************************************************
;
;	Name:		
;
;	Purpose:	
;
;	Input:		
;
;	Output:		
;
;	Return:		
;
**********************************************************/
void clear_keyboard()
{
	memset(Keyboard, 0, sizeof(BYTE)*MaxKey);
}

#if 0
/**********************************************************
;
;	Name:		init_the_dinput
;
;	Purpose:	initializes the three direct input device along with direct input itself, will be improved next time
;
;	Input:		mouse cooperation flag, keyboard cooperation flag, joystick cooperation flag
;
;	Output:		global direct input related stuff are initialize
;
;	Return:		none
;
**********************************************************/
void init_the_dinput(DWORD mouse_coop_flag, DWORD keyboard_coop_flag, DWORD joystick_coop_flag)
{
	dinput_init(G_hwnd, G_hinstance, &G_diitems, mouse_coop_flag, keyboard_coop_flag, joystick_coop_flag);
}

/**********************************************************
;
;	Name:		destroy_the_dinput	
;
;	Purpose:	To destroy the direct input, yes everything!
;
;	Input:		none
;
;	Output:		no more pesky Direct Input :)
;
;	Return:		none
;
**********************************************************/
void destroy_the_dinput()
{
	dinput_term(&G_diitems);
	//Making sure it's really dead :)
	memset(&G_diitems, 0, sizeof(TrioInput));
}

/**********************************************************
;
;	Name:		update_mouse_dinput
;
;	Purpose:	Updates the global mouse data with buffered inputs
;
;	Input:		none
;
;	Output:		the global mouse data is updated
;
;	Return:		none
;
**********************************************************/
void update_mouse_dinput()
{
	diupdate_mouse(G_hwnd, G_diitems.dmouse, &G_mouse_data);

	//after that, check if the mouse is out of boundary...
	if(G_mouse_data.location.X < G_clientarea.left)
		G_mouse_data.location.X = (short)G_clientarea.left;
	if(G_mouse_data.location.X > G_clientarea.right)
		G_mouse_data.location.X = (short)G_clientarea.right;
	if(G_mouse_data.location.Y < G_clientarea.top)
		G_mouse_data.location.Y = (short)G_clientarea.top;
	if(G_mouse_data.location.Y > G_clientarea.bottom)
		G_mouse_data.location.Y = (short)G_clientarea.bottom;
}

/**********************************************************
;
;	Name:		update_keyboard_dinput
;
;	Purpose:	Updates the global keyboard array with which buttons pressed/depressed
;
;	Input:		none
;
;	Output:		the global mouse data is updated
;
;	Return:		none
;
**********************************************************/
void update_keyboard_dinput()
{
	diupdate_keyboard(G_hwnd, G_diitems.dkeyboard, Keyboard);
}
#endif