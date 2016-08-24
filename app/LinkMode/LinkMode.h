#ifndef _LINKMODE_H_
#define _LINKMODE_H_
/*
descript the  state of linking , include BLE wifi 
*/
typedef enum
{
	BuildLinkFail,
	BuildLinkSuccess,
	SendDataFail,
	GetDataFail
} E_CurrentLinkState;
typedef enum
{
	AloneMode,
	WIFIMode,
	BLEMode
	
} E_CurrentMode;

/*
extern struct _STR_CURRENT_STATE_
{
	E_CurrentMode 			currentMode;
	E_CurrentLinkState  currentState;	
};
*/



#endif

