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


		// [shhan][2015.06.15] ������ �޸𸮸� �����Ѵ�.
		//						�ε� �� �������ϸ� �ε��� ����ߴ� �ʿ���� ������ �޸𸮵鵵 ���� �� �־��.
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