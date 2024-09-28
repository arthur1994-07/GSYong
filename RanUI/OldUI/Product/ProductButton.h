#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

class GLGaeaClient;

class CProductButton : public CUIGroup
{
private:
	enum
	{
		ID_PRODUCT_BUTTON = NO_ID + 1,
	};

protected:
	GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
	CProductButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

public:
	void CreateSubControl();

public:
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
};

class MyProductButton : public IProductButton, private CProductButton
{
public:
	MyProductButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

public:
	virtual void CreateUIWindowAndRegisterOwnership();
};