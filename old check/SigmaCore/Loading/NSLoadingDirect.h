#pragma once

class LoadingDirect;

namespace NSLoadingDirect
{
	void Set( LoadingDirect* pLoadingDirect, WORD wType, BOOL bShowWinnerCountryFlag=FALSE );
	void Reset();
	void FrameMoveRender( BOOL bImmediately, BOOL bCheckTime = TRUE );
    bool IsEnable();
}