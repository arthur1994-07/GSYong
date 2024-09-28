//	��ų �̵�
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.12.8]
//			@ �ۼ�
//

#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../InnerInterface.h"

class CSkillImage;
class GLGaeaClient;
class CInnerInterface;

class CSkillWindowToTray : public CUIGroup
{
protected:
static const DWORD	dwDEFAULT_TRANSPARENCY;
static const float	fDEFAULT_MOUSE_INTERPOLIATION;

public:
	CSkillWindowToTray(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSkillWindowToTray();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
	void	CreateSubControl();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	void		SetSkill( SNATIVEID sNativeID );
	SNATIVEID	GetSkill()							{ return m_sNativeID; }
	void		ResetSkill();

public:
	void	SetUseSnap( BOOL bUseSnap )			{ m_bUseSnap = bUseSnap; }
	BOOL	IsUseSnap()							{ return m_bUseSnap; }

private:
	CSkillImage*	m_pSkillImage;
	SNATIVEID		m_sNativeID;

private:
	BOOL	m_bUseSnap;
};

class MySkillWindowToTray : public ISkillWindowToTray, private CSkillWindowToTray
{
public:
    MySkillWindowToTray( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MySkillWindowToTray()  {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void SetSkill( SNATIVEID sNativeID );
    virtual SNATIVEID GetSkill();
    virtual void ResetSkill();
    virtual void SetUIWindowGlobalPos( const UIRECT& rcPos );
    virtual void SetUseSnap( BOOL bUseSnap );
};