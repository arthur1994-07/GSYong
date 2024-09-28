#pragma once

#include "../../../../enginelib/GUInterface/UIControl.h"
#include "../../Util/UIGroupHelper.h"
#include "../../../InnerInterface.h"

/*
 * ��Ƽ ���� Ȥ�� ���Խ� ȭ�� ������ ���� �Ǵ� ��Ƽ�� ���� UI;
 * ���� ��Ʈ����Ʈ, �п�, Ŭ��, �̸�, ü��, ����/����� ���������� ����;
*/

class GLGaeaClient;
class CInnerInterface;
class MemberStateGroup;
class PartyStateWindow : public CUIGroupHelper
{
private:
	enum
	{
		ID_PARTY_STATE_GROUP = NO_ID + 1,
		ID_PARTY_STATE_CONTROL = NO_ID + 2,
	};
public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	void SetVisibleSkillEffect(const BOOL bShow);
	void CreateSubControl(void);

	void Resize(void);

public:
	PartyStateWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~PartyStateWindow(void){};

protected:
	CInnerInterface* m_pInterface;
	GLGaeaClient* m_pGaeaClient;
	MemberStateGroup* m_pPartyStateGroup;
	CUIControl* m_pControl;
	
	D3DXVECTOR3 m_vOldMousePosition;
	UIRECT m_vOldUIPosition;

    INTERFACE_CFG m_UICfg;    //��Ƽ�Ѹ� ������ âũ�⸦ xml���� ���ͼ� �����س��´�.
	
	bool m_bDragging;
	bool m_bVisibleSkillEffect;
	float m_fControlGapY;	// ��Ƽ�帶ũ�� ���� top��ġ ���� 
};

class MyPartyStateWindow : public IPartyStateWindow, public PartyStateWindow
{
public:
	MyPartyStateWindow( GLGaeaClient* pGaeaCleint, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyPartyStateWindow(void){};

public:
	virtual void CreateUIWindowAndRegisterOwnership(void);
	virtual void SetVisibleSkillEffect(const BOOL bShow) override;

};