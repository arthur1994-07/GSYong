//////////////////////////////////////////////////////////////////////////////////
//	VTCGuard API for launcher
//////////////////////////////////////////////////////////////////////////////////
#ifndef __GUARDAPI_LAUNCHER
#define __GUARDAPI_LAUNCHER

#define VTC_SERVER_DOMAIN "guard.vtc.co.id"
#define VTC_SERVER_DOMAIN_SUBFOLDER "ran"
#define VTC_SERVER_PORT 80

/**
	* lpDomain		: server update
	* subFolder		:
	* nPort			: http port
	* lpGamePath	: path to game folder. Ex: C:\Program Files\Ran
	* Msg			: return message
*/
VOID vt_Autoupdate(IN LPCSTR lpDomain,IN LPCSTR subFolder,IN int nPort,IN LPCSTR lpGamePath,OUT LPSTR Msg);


/**
	* lpGamePath	: parameters passed when calling guard.
		Ex: C:\Program Files\Ran\RanOnline.exe#param1#KJSHDF8932N3B2M3NBCPS8DHLO63G9FV 
		"KJSHDF8932N3B2M3NBCPS8DHLO63G9FV" is constant
	* Msg			: return message
*/
bool vt_run(IN LPCSTR lpArg,OUT LPSTR Msg);

#endif

