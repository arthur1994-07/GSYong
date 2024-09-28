

#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../../RanLogic/Character/GLCharDefine.h"

class CInnerInterface;
class GLGaeaClient;

class CWbClubStorageUseRecordNode;

class CWbClubStorageUseRecordPage : public CUIGroup
{
private:
    static const float fRecordNodeOffset;
    enum
    {
        NUM_RECORD_NODE = 5,
    };

private:
    enum
    {
        BACK = NO_ID + 1,
        PRE_BTN,
        NEXT_BTN,
    };

public:
    CWbClubStorageUseRecordPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CWbClubStorageUseRecordPage();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    void CreateSubControl();

    void Refresh();
    void ClubStorageUpdateLog();

private:
    void ClearInfo();

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
    CWbClubStorageUseRecordNode* m_pUseRecordNode[ NUM_RECORD_NODE ];

    RnButton* m_pPreBtn;
    RnButton* m_pNextBtn;

private:
    WORD m_Page;
};