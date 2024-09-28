#include "stdafx.h"
#include "./NSLoadingDirect.h"

#include "./LoadingDirect.h"

#include "../DebugInclude.h"

namespace NSLoadingDirect
{
    static DWORD g_dwOwnerThread = 0;
	static LoadingDirect* g_pLoadingDirect = NULL;

	void Set( LoadingDirect* pLoadingDirect, WORD wType, BOOL bShowWinnerCountryFlag )
	{
        g_dwOwnerThread = GetCurrentThreadId();

        g_pLoadingDirect = pLoadingDirect;
        if( g_pLoadingDirect )
            g_pLoadingDirect->SetMode( wType, bShowWinnerCountryFlag );
	}

	void Reset()
	{
        if( g_pLoadingDirect )
        {
            g_pLoadingDirect->ResetMode();
            g_pLoadingDirect = NULL;
        }

        g_dwOwnerThread = 0;


		// [shhan][2015.06.15] 물리적 메모리를 정리한다.
		//						로딩 다 끝나고하면 로딩시 사용했던 필요없는 물리적 메모리들도 지울 수 있어서다.
		HANDLE hProcess	= GetCurrentProcess();
		if (hProcess)
		{
			EmptyWorkingSet( hProcess );
		}
	}

	void FrameMoveRender( BOOL bImmediately, BOOL bCheckTime )
	{
		if( !g_pLoadingDirect )
            return;

        if( g_dwOwnerThread != GetCurrentThreadId() )
            return;

		g_pLoadingDirect->FrameMoveRender( bImmediately, bCheckTime );
	}

    bool IsEnable()
    {
        return ( g_pLoadingDirect != NULL ) ? true : false;
    }
}