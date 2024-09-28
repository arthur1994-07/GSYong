
#pragma once

#include "./GLFlyCameraBaseMode.h"

class GLFlyCameraFixedMode : public GLFlyCameraBaseMode
{
private:

public:
	GLFlyCameraFixedMode( GLGaeaClient* pGaeaClient, GLFlyCameraControl* pParentCtrl );
	virtual ~GLFlyCameraFixedMode(void);

public:
	void Init(void) override;
	void MsgProcedures ( DWORD dwType, DWORD dwMsg ) override;
	void Update( float fTime, float fElapsedTime ) override;
};