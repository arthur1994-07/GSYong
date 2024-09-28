#pragma once

#include "../../../RanLogic/Item/GLItem.h"
#include "../Util/UIWindowEx.h"

class GLGaeaClient;
class CUIGroup;

class CCostumeStatWindow : public CUIWindowEx
{

public :
	enum {
		CS_SUBCTRL_PARTS,
		CS_SUBCTRL_POINT,
		CS_SUBCTRL_STAT_GENERAL,		// �Ϲ�ȿ��
		CS_SUBCTRL_STAT_VARRATE,		// ��ȭ��ȿ��
		CS_SUBCTRL_STAT_ADD,			// ����ȿ��
		CS_SUBCTRL_STAT_EXPLAIN,		// ����
		CS_SUBCTRL_BTN_INIT,			// �ʱ�ȭ ��ư
		CS_SUBCTRL_BTN_APPLY,			// ����
		CS_SUBCTRL_BTN_CANCEL,			// ���
		CS_SUBCTRL_MAX
	};

public:
	CCostumeStatWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CCostumeStatWindow();

private :
	const SINVENITEM* m_pSrcItem;
	const SINVENITEM* m_pDstItem;

	GLGaeaClient* m_pGaeaClient;

	CUIGroup*	m_pSubCtrl[CS_SUBCTRL_MAX];

	CD3DFontPar*	   m_pFont10;

	int   m_nItemPosX;
	int	  m_nItemPosY;

protected:
	void CreateSubControl();

public :
	void CretaeWindow();
	bool SetItemInfo( const SINVENITEM* pSrc, const SINVENITEM* pDest );
	void ResetPoint();
	void UpdateHavePoint();
	void CloseWindow();
	
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
};