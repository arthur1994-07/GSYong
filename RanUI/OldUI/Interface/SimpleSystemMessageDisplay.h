// �δ� �ý��� �߰��� ���� ��ũ��Ʈ�� ���� UI ����;
// ȭ�� �߾� ��ܿ� ������ �ؽ�Ʈ �޽ý� ���;

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

    bool    m_bShowtip; // �� ���� �ɼ��� ������ ���� ���� ���� �ɼǻ��¸� ����;

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