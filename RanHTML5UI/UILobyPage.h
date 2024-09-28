#pragma once

#include <GUIBase/IPage.h>
#include <GUIBase/UIEvent.h>

class GLGaeaClient;
class EngineDeviceMan;

namespace HTML5UI
{

class CUILobyPage : public GUIBase::IPage
{
public:
	CUILobyPage( GLGaeaClient* pGaeaClient
		, EngineDeviceMan* pEngineDevice );
	~CUILobyPage();

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