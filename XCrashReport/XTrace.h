// XTrace.h  Version 1.2
//
// Author:       Paul Mclachlan
//
// Modified by:  Hans Dietrich
//               hdietrich2@hotmail.com
//
// Version 1.2:  added TRACEPOINT macro
//
// Version 1.1:  added Unicode support
//               added optional thread id to output string
//               added option to enable/disable full path
//               added TRACERECT macro
//               changed name to avoid conflicts with Paul's class.
//
// This code was taken from article by Paul Mclachlan, "Getting around
// the need for a vararg #define just to automatically use __FILE__ and
// __LINE__ in a TRACE macro".  For original article, see
//     http://www.codeproject.com/useritems/location_trace.asp
//
// XTrace.h is a drop-in replacement for MFC's TRACE facility.  It has no
// dependency on MFC.  It is thread-safe and uses no globals or statics.
//
// It optionally adds source module/line number and thread id to each line
// of TRACE output.  To control these features, use the following defines:
//
//     XTRACE_SHOW_FULLPATH
//     XTRACE_SHOW_THREAD_ID
//
// XTrace.h also provides the TRACERECT and TRACEPOINT macros, which output 
// the contents of a RECT struct and a POINT struct.  
//
// In Release builds, no output will be produced.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XTRACE_H
#define XTRACE_H

#include <stdarg.h>
#include <stdio.h>
#include <windows.h>
#include <tchar.h>

#pragma warning(push)
#pragma warning(disable : 4127)			// conditional expression is constant

#define XTRACE_SHOW_FULLPATH	FALSE	// FALSE = only show base name of file
#define XTRACE_SHOW_THREAD_ID	TRUE	// TRUE = include thread id in output

class xtracing_output_debug_string
{
public:
	xtracing_output_debug_string(LPCTSTR lpszFile, int line) :
		m_file(lpszFile),
		m_line(line)
	{
	}

	void operator() (LPCTSTR lpszFormat, ...)
	{
		va_list va;
		va_start(va, lpszFormat);

		TCHAR buf1[BUFFER_SIZE];
		TCHAR buf2[BUFFER_SIZE];

		// add the __FILE__ and __LINE__ to the front
		LPTSTR cp = (LPTSTR) m_file;

		if (!XTRACE_SHOW_FULLPATH)
		{
			cp = _tcsrchr((LPTSTR)m_file, _T('\\'));
			if (cp)
				cp++;
		}

		if (XTRACE_SHOW_THREAD_ID)
		{
			if (_sntprintf(buf1, BUFFER_SIZE-1, _T("%s(%d) : [%X] %s"),
						cp, m_line, GetCurrentThreadId(), lpszFormat) < 0)
				buf1[BUFFER_SIZE-1] = _T('\0');
		}
		else
		{
			if (_sntprintf(buf1, BUFFER_SIZE-1, _T("%s(%d) : %s"),
						cp, m_line, lpszFormat) < 0)
				buf1[BUFFER_SIZE-1] = _T('\0');
		}

		// format the message as requested
		if (_vsntprintf(buf2, BUFFER_SIZE-1, buf1, va) < 0)
			buf2[BUFFER_SIZE-1] = _T('\0');

		va_end(va);

		// write it out
		OutputDebugString(buf2);
	}

private:
	LPCTSTR m_file;
	int     m_line;
	enum    { BUFFER_SIZE = 4096 };
};

#undef TRACE

#ifdef _DEBUG
#define TRACE (xtracing_output_debug_string(_T(__FILE__), __LINE__ ))
#define TRACEERROR (xtracing_output_debug_string(_T(__FILE__), __LINE__ ))
#else
#define TRACE ((void)0)
#define TRACEERROR ((void)0)
#endif

#define TRACERECT(r) TRACE(_T(#r) _T(":  left = %d  top = %d  right = %d  bottom = %d\n"), \
                           (r).left, (r).top, (r).right, (r).bottom)

#define TRACEPOINT(p) TRACE(_T(#p) _T(":  x = %d  y = %d\n"), (p).x, (p).y)

#pragma warning(pop)

#endif //XTRACE_H
