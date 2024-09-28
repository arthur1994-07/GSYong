#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Character/GLCharData.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class GLGaeaClient;

class CSystemBuffTimeDisplay : public CUIGroup
{
private:
    enum
    {
        SYSTEMBUFFTIME_DUMMY_BASE = NO_ID + 1,
        SYSTEMBUFFTIME_DUMMY_SIZE = 5,
        SYSTEMBUFFTIME_UNIT = 1000,
        SYSTEMBUFFTIME_UNIT_END = SYSTEMBUFFTIME_UNIT + 10000,
    };

public:
    CSystemBuffTimeDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CSystemBuffTimeDisplay ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
    void	CreateSubControl();

public:
    void	SetEnableAll( const BOOL bEnable );

private:
    bool	NEW_PLAY_SYSTEMBUFF( const DWORD& dwSkillID, const WORD wLEVEL, const WORD wSlot );
    UIGUID	GET_EXIST_PLAY_SYSTEMBUFF_CONTROL( const DWORD& dwSkillID );
    bool	GET_PLAY_SYSTEMBUFF( const DWORD& dwSkillID, const WORD& wLevel, SSKILLFACT& sSKILLFACT );

private:
    bool	GET_SYSTEMBUFF_LIFE_TIME( const DWORD& dwSkillID, const WORD wLEVEL, float& fLifeTime );

public:
    virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
    //! 스킬 아이디 뿐만아니라 레벨도 추가
    typedef	std::pair<std::pair<DWORD,WORD>,UIGUID>	SYSTEMBUFFTIME_PAIR;
    typedef	std::list<SYSTEMBUFFTIME_PAIR>		    SYSTEMBUFFTIME_DISPLAY_LIST;
    typedef	SYSTEMBUFFTIME_DISPLAY_LIST::iterator	SYSTEMBUFFTIME_DISPLAY_LIST_ITER;

    SYSTEMBUFFTIME_DISPLAY_LIST	m_SystemBuffPlayList;
    UIGUID						m_NEWID;

    LPDIRECT3DDEVICEQ			m_pd3dDevice;

private:
    CUIControl* m_pSystemBuffUnitDummy[SKILLFACT_SIZE_SYSTEM];

protected:
    CInnerInterface* m_pInterface;

};

class MySystemBuffTimeDisplay : public ISystemBuffTimeDisplay, private CSystemBuffTimeDisplay
{
public:
    MySystemBuffTimeDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MySystemBuffTimeDisplay()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();
};