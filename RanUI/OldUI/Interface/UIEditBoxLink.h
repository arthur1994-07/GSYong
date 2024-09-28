#pragma once


#include "../../../EngineLib/GUInterface/UIEditBox.h"
//#include "../Interface/ChatType.h"
#include "../../../RanLogic/GLChatLinkDef.h"


class CBasicTextBox;

class CUIEditBoxLink : public CUIEditBox
{
public:
	CUIEditBoxLink(EngineDeviceMan* pEngineDevice);
	virtual ~CUIEditBoxLink();

public:
	virtual void ClearEdit();
	virtual void SetRenderText(const CString& strRenderText) override;

	virtual bool SET_MONEY_FORMAT( int nMoneyUNIT, const CString& strUNIT_SYMBOL = "," ) { return false; }
	virtual void SetHide( BOOL bHide )		{}
	virtual void DoUSE_NUMBER( bool bUse )	{}
	virtual void SetMinusSign( BOOL bUse )	{}

	virtual void UpdateInsertPos();
	virtual void UpdateRenderText();
	virtual void UpdatePosition ();

	virtual void GetInsertPos();
	virtual void SetInsertPos();

public:
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void ChatToLink( const SLINK_DATA_BASIC& sLinkDataBasic, const CString& strLinkName, DWORD dwColor, const SITEMCUSTOM *pItemCustom = NULL);

	bool IsPartyLink( const SLINK_DATA_BASIC& sLinkDataBasic );	// ��Ƽ���� ��ũ ���� üũ
	bool IsChatLink();
	int	 IsDeleteLinkText( int nPos, int nDiffText );
	
	void CalcLinkText();						//	EditBox �Է¿� ���� LinkText ����
	void CalcLinkTextAdd( int nAdd );			//	LinkText �߰��� ���� LinkText ����

	CStringW MutiToWide(const CString& strText);

public:
	int m_xInsertPosBack;
	
	VEC_LINK_TEXT_DATA_EX	m_vecLinkTextData;
};