#pragma once

#include <GUIBase/IPage.h>
#include <GUIBase/UIEvent.h>

class GLGaeaClient;
class EngineDeviceMan;

namespace HTML5UI
{

class CUIGamePage : public GUIBase::IPage
{
public:
	CUIGamePage( GLGaeaClient* pGaeaClient
		, EngineDeviceMan* pEngineDevice );
	~CUIGamePage();

public:
	virtual void Initialize();
	virtual void Destroy();

	/// Loop
	virtual bool FrameMove( float timeDelta );
	virtual bool Render( float timeDelta );

private:
	GLGaeaClient* m_pGaeaClient;
	EngineDeviceMan* m_pEngineDevice;
};

}