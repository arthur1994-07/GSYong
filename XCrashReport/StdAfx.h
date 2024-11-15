// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef STDAFX_H
#define STDAFX_H

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <winsock.h>        // Sockets support
#include <afxpriv.h>        // T2A macros
#include <afxole.h>         // MFC ole 

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "XTrace.h"

#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //STDAFX_H
