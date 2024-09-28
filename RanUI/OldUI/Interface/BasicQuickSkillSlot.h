//	스킬 트레이
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.12.5]
//			@ 스킬 이미지(CSkillImage) 클래스 작성후, 모듈 분리
//		[2003.11.21]
//			@ 작성

#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../InnerInterface.h"

class CSkillImage;
class GLGaeaClient;

class CBasicQuickSkillSlot : public CUIGroup
{
protected:
    enum
    {
        QUICK_SKILL_MOUSE_OVER = NO_ID + 1,
    };

public:
    CBasicQuickSkillSlot(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CBasicQuickSkillSlot();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    virtual	void	CreateSubControl();

public:
    virtual void TurnHorizontal ();
    virtual void TurnVertical   ();

public:
    UIRECT GetAbsPosSkillImage();
    void SetUseSkillImageTwinkle( bool bUse );
    CSkillImage* getSkillImage() { return m_pSkillImage; }
    const UIRECT& GetSkillImageGlobalPos();

protected:	
    void	CreateSkillImage();
    void	CreateMouseOver();

protected:
    void	UpdateSlot( SNATIVEID sNativeID );

public:
    virtual void	Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
    CSkillImage*		m_pSkillImage;

protected:
    CUIControl*			m_pSkillMouseOver;

private:
    LPDIRECT3DDEVICEQ	m_pd3dDevice;
};

class MyBasicQuickSkillSlot : public IBasicQuickSkillSlot, private CBasicQuickSkillSlot
{
public:
    MyBasicQuickSkillSlot( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyBasicQuickSkillSlot()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual const UIRECT& GetSkillImageGlobalPos();
};