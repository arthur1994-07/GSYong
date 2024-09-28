
#pragma once

//#include "../../EngineLib/DxCommon9/dxstdafx.h"

struct NET_MSG_GENERIC;

//-------------------------------------------------------------------//

class GLLandEvent
{

public :
	GLLandEvent();
	virtual ~GLLandEvent() = 0;

public :
	virtual void CleanUp()                             = 0;
	virtual void FrameMove( const FLOAT fElapsedTime ) = 0;

public :
	virtual void MsgProcess ( NET_MSG_GENERIC* nmg )   = 0;

};
