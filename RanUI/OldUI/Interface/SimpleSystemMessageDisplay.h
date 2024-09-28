// 인던 시스템 추가로 인한 스크립트용 공용 UI 제작;
// 화면 중안 상단에 간단한 텍스트 메시시 출력;

#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;
class CBasicTextBox;
class GLGaeaClient;

class SimpleSystemMessageDisplay : public CUIGroup
{
private:
    static	const	float	fLIFE_TIME;

public:
    SimpleSystemMessageDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~SimpleSystemMessageDisplay ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
    void	CreateSubControl ();

public:
    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) override;
    virtual void SetVisibleSingle( BOOL bVisible ) override;

public:
    void    PrintSimpleSystemMessage( const CString& strMessage, const D3DCOLOR dwMessageColor, const DWORD dwFontSize, const float fLifeTime );
    void    ClearSimpleSystemMessage();

private:
    CBasicTextBox*	m_pMessageDisplay;	

private:
    float	m_fLifeTime;
    bool    m_bEternity;

    bool    m_bShowtip; // 팁 보기 옵션을 강제로 끄기 위해 이전 옵션상태를 저장;

public:
    float GetLifeTime ()	{ return m_fLifeTime; }

protected:
    CInnerInterface* m_pInterface;
};

class MySimpleSystemMessageDisplay: public ISimpleSystemMessageDisplay, private SimpleSystemMessageDisplay
{
public:
    MySimpleSystemMessageDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
        :SimpleSystemMessageDisplay( pGaeaClient, pInterface, pEngineDevice )
    {
    }
    virtual ~MySimpleSystemMessageDisplay()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnerShip();
    virtual void PrintSimpleSystemMessage( const CString& strMessage, const D3DCOLOR dwMessageColor, const DWORD dwFontSize, const float fLifeTime );
    virtual void ClearSimpleSystemMessage();
};