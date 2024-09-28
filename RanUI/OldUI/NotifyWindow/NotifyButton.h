#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

class GLGaeaClient;
class NotifyClientMan;

class CNotifyButton : public CUIGroup
{
private:
    enum
    {
        ALARM_TIME          = 86400000,     // ÃÑ ¾Ë¶÷ ½Ã°£
        ALARM_FLASH_CYCLE   = 500,      // ¾Ë¶÷ ÁÖ±â
        ALARM_FLASH_TIME    = 500,      // ±ôºýÀÓ ½Ã°£
    };

	enum
	{
		ID_NOTIFY_BUTTON = NO_ID + 1,
	};

public:
	CNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	void                CreateSubControl();
	virtual	void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    void                SetNotifyAlarm();


protected:
    GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;
    CBasicButton*       m_pButtonActiveImg;
    
    NotifyClientMan*    m_pNotiftMan;
    float               m_fTimer;
};
