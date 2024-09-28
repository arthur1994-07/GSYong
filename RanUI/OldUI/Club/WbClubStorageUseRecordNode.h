

#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../../RanLogic/Character/GLCharDefine.h"

class CInnerInterface;
class GLGaeaClient;

class CWbClubStorageUseRecordNode : public CUIGroup
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
    CWbClubStorageUseRecordNode(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CWbClubStorageUseRecordNode();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    void CreateSubControl();

    void SetContent( std::string& strContent );
    void SetDate( __time64_t LogTime );

    void ClearInfo();

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
    CBasicTextBox* m_pUseRecordContent;
    CBasicTextBox* m_pUseRecordDate;
};