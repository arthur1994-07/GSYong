#pragma	once

#include "../Util/WebWindowBase.h"

class CInnerInterface;
class GLGaeaClient;

class CItemShopWindowWeb : public CWebWindowBase
{
public:
	CItemShopWindowWeb( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	VOID SetVisibleSingle( BOOL bVisible );
};