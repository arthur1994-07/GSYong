#ifndef _RANUI_MINI_PARTY_SLSOT_H_
#define _RANUI_MINI_PARTY_SLSOT_H_

#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"

#include "../../../RanLogic/Character/GLCharDefine.h"
#include "../../../RanLogic/Party/GLPartyDefine.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_RB_EVENT = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CBasicTextBox;
class CBasicProgressBar;

class CMiniPartySlot : public CUIGroup
{
protected:
	enum
	{
		HP_BAR = NO_ID + 1,
	};

	CBasicTextBox*		m_pPlayerName;
	CBasicProgressBar*	m_pHP;
	CUIControl*			m_pNumber[MAXPARTY];
	//CUIControl*		m_pClass[GLCI_NUM];

public:
	CMiniPartySlot(EngineDeviceMan* pEngineDevice);
	virtual	~CMiniPartySlot();

public:
	void	CreateSubControl ();

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	SetPlayerName ( CString strName )	{ m_pPlayerName->SetOneLineText( strName ); }
	void	SetHP ( float fPercent )			{ m_pHP->SetPercent( fPercent ); }
	void	SetSameMap ( BOOL bSameMap )		{ m_pHP->SetVisibleSingle( bSameMap ); }
	//void	SetClass ( int nClassType );
	void	SetNumber ( int nSlotNumber );

protected:
	CUIControl*		CreateControl ( const char* szControl );
	CBasicTextBox*	CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );
};

#endif // _RANUI_MINI_PARTY_SLSOT_H_