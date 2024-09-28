#include "stdafx.h"
#include "../RanLogic/GLogicDataMan.h"
#include "PreDownloadDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace	PreDownloadDefine
{
	CString	g_strDownedList = "cPreDownedList.bin";

	TCHAR*	g_strServerPreFileList = "PreFileList.bin";
	TCHAR*	g_strClientPreFileList = "cPreFileList.bin";

	CString g_strPreDownloadDirectory = "PreDownload\\";

	CString g_strAppPath;
	CString g_strProFile;

	CString g_strMenuPause = "Stop";
	CString g_strMenuResume = "Resume";
};