#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class GLGaeaClient;

class CActivityHelpWindow : public CUIWindowEx
{

public:
    CActivityHelpWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, CUIGroup* pCaller, EngineDeviceMan* pEngineDevice );
    virtual	~CActivityHelpWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
    void	CreateSubControl();

public:
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle( BOOL bVisible );

private:
    CUIGroup* m_pCaller;

};
