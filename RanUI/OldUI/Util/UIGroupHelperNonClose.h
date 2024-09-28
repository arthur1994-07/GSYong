#pragma once

#include "./UIGroupHelper.h"

class CInnerInterface;
class EngieDeviceMan;

class CUIGroupHelperNonClose : public CUIGroupHelper
{
protected:
	enum
	{
		EUIGroupHelperNonCloseBackground = NO_ID + 1,
		EUIGroupHelperNonCloseIDNext
	};

public:
	CUIGroupHelperNonClose(
		EngineDeviceMan* pEngineDevice,
		CInnerInterface* pInterface );
	virtual	~CUIGroupHelperNonClose();

public:
	virtual void CreateSub( CUIControl* pParent, const char* szControlKeyword, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );

	void CreateLightGrayMove( const char* szControlKeyword );
	void CreateLightGrayNonMove( const char* szControlKeyword );

protected:
	// Update;
	virtual void Update( int x, int y,
		BYTE LB, BYTE MB, BYTE RB,
		int nScroll, float fElapsedTime, BOOL bFirstControl );

	// Translate Message;
	virtual void TranslateUIMessage(
		UIGUID ControlID, DWORD dwMsg );

private:
	void DragMessageSelectColor( DWORD dwMsg );

public:
	inline void SetSelectColorState( bool bSelectColor ) { m_bSelectColor = bSelectColor; }
	void CreateNonBackground(bool bMove,bool bSelectColor);
protected:
	CInnerInterface* m_pInterface;

private:
	D3DXVECTOR2 m_vPos;

	bool m_bMove;
	bool m_bSelectColor;
};