#pragma once

#include "../../../../enginelib/GUInterface/UIControl.h"
#include "../../Util/UIGroupHelper.h"
#include "../../../InnerInterface.h"

/*
 * 파티 생성 혹은 가입시 화면 좌측에 생성 되는 파티원 상태 UI;
 * 직업 포트레이트, 학원, 클럽, 이름, 체력, 버프/디버프 아이콘으로 구성;
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

    INTERFACE_CFG m_UICfg;    //파티한명 슬롯의 창크기를 xml에서 얻어와서 저장해놓는다.
	
	bool m_bDragging;
	bool m_bVisibleSkillEffect;
	float m_fControlGapY;	// 파티장마크로 인한 top위치 간격 
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