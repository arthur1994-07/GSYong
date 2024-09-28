#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class  CInnerInterface;
class  GLGaeaClient;
class  CBasicTextButton;
class  CBasicTextBox;
class  CActivitySalesItemList;

class CActivitySalesItemWindow : public CUIWindowEx
{
public:
    enum
    {
        ACTIVITY_SALESITEM_WINDOW_BUTTON_CLOSE = ET_CONTROL_NEXT,
    };

    enum EMSALESTYPE
    {
        EMSALESTYPE_LOW,
        EMSALESTYPE_MIDIUM,
        EMSALESTYPE_HIGH,
        EMSALESTYPE_MAX,
    };

public:
    CActivitySalesItemWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, CUIGroup* pCaller, EngineDeviceMan* pEngineDevice );
    virtual	~CActivitySalesItemWindow();

public:
    void	     CreateSubControl();

public:
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle( BOOL bVisible );

public:
    void         InitSalesList ( const EMSALESTYPE emType );

private:
    CActivitySalesItemList*     m_pItemList;
    CBasicTextBox*              m_pDesc;
    CBasicTextButton*           m_pCloseButton;

private:
    CUIGroup*                   m_pCaller;

protected:
    GLGaeaClient*               m_pGaeaClient;

};
