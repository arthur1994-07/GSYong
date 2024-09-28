#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Character/GLCharDefine.h"

#include "../../InnerInterface.h"

class GLGaeaClient;
class CInnerInterface;
class CBasicTextBox;
class CSwapImage;

class CInfoCTFDisplay : public CUIGroup
{

private:
    enum
    {
       INFO_CTF_DISPLAY_NONE = NO_ID + 1,
    };

public:
    CInfoCTFDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    ~CInfoCTFDisplay ();

public:
    struct AUTHINFO : public CUIGroup
    {
        CBasicTextBox* pLABEL;
        CSwapImage*    pMARK;

        AUTHINFO( EngineDeviceMan* pEngineDevice )
            : CUIGroup(pEngineDevice)
            , pLABEL(NULL)
            , pMARK(NULL)
        {
        }

        void CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice, const std::string& strLABEL );
        void SetAuthSchool ( ENUM_SCHOOL emAuthSchool );
    };

public:
    void CreateSubControl ();

public:
    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    void Update();

public:
    virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

private:
    void        ResizePannel ( const UINT nMax );
    AUTHINFO*   CreateAuthInfo( const std::string& strLABEL, const UINT nIndex, const UINT nMax );
    CUIControl* CreateLine( const UINT nIndex, const UINT nMax );

private:
    const std::string GetTimeLeft( __time64_t tDest );

private:
    CUIControl* m_pBackground_Left;
    CUIControl* m_pBackground_Body;
    CUIControl* m_pBackground_Right;

    CBasicTextBox* m_pCTF_Progress;
    CBasicTextBox* m_pCTF_Time;
    
    AUTHINFO**     m_ppAuthInfo;
    CUIControl**   m_ppLine;

private:
    LPDIRECT3DDEVICEQ   m_pd3dDevice;
    D3DXVECTOR2         m_vPos;
    UINT                m_nAuthInfoNums;
    bool                m_bCTFProgress;

protected:
    GLGaeaClient*    m_pGaeaClient;
    CInnerInterface* m_pInterface;
   
};

class MyInfoCTFDisplay : public IInfoCTFDisplay, private CInfoCTFDisplay
{
public:
    MyInfoCTFDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyInfoCTFDisplay() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();
    virtual void Update();

};
