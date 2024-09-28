#pragma once

class LoadingDirect
{
public:
	LoadingDirect() {}
	virtual ~LoadingDirect() {}

public:
	virtual void Initialize(BOOL _bCheckTime) {}
	virtual void SetMode( WORD wType, BOOL bShowWinnerCountryFlag ) {}
	virtual void ResetMode() {}
	virtual void FrameMoveRender( BOOL bImmediately, BOOL bCheckTime ) {}
};