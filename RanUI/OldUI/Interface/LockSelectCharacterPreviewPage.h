#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/GLogicData.h"

class COuterInterface;
class GLGaeaClient;

class CLockSelectCharacterPreviewPage : public CUIGroup
{
private:
    enum
    {
        LOCK_IMAGE = NO_ID + 1,
    };

public:
    CLockSelectCharacterPreviewPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CLockSelectCharacterPreviewPage ();

public:
    void CreateSubControl ();

public:
    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
    bool IsVisiblePos( D3DXVECTOR3& vOutScreen, const D3DXVECTOR3& vPosition );

public:
    void SetLockImagePos( const D3DXVECTOR3& vCharPos, int nIndex );
    void SetVisibleLockImage( bool bVisible, int nIndex );

private:
    CUIControl* m_pLockImage[ MAX_SELECTCHAR ];
    CUIControl* m_pLockImageDummy;

protected:
    COuterInterface* m_pInterface;
    GLGaeaClient* m_pGaeaClient;
};