
#pragma once

#include "./GLControlBase.h"


class GLControlTypeFlyCamera : public GLControlBase
{
public:
	enum EMMOVEFLAG
	{
		MOVEROTATESHORTCUTS_FRONT = 0,
		MOVEROTATESHORTCUTS_BACK,
		MOVEROTATESHORTCUTS_LEFT,
		MOVEROTATESHORTCUTS_RIGHT,
		MOVEROTATESHORTCUTS_UP,
		MOVEROTATESHORTCUTS_DOWN,

		VARIATION_INCREASE,
		VARIATION_DECREASE,
	};

public:
	GLControlTypeFlyCamera(GLGaeaClient* pGaeaClient);
	virtual ~GLControlTypeFlyCamera(void);

public:
	virtual void DoInit(void);
	virtual void DoInput(float fTime, float fElapsedTime);

	virtual void DoInputCamera( float fTime, float fElapsedTime );
	virtual void DoUpdate( float fTime, float fElapsedTime );

	virtual const bool DoIsKeyboardBlock( float fTime, float fElapsedTime )  { return false; }
	virtual const bool DoIsMouseBlock( float fTime, float fElapsedTime )	 { return false; }
	virtual const bool DoIsShortcutsBlock( float fTime, float fElapsedTime ) { return false; }
	virtual const bool DoIsCharacterBlock( float fTime, float fElapsedTime ) { return false; }

	// nMoveDir;
private:
	virtual void DoProc_CameraMove(const SKEYCAPS&, const UINT nMoveDir);
	virtual void DoProc_CameraRotate(const SKEYCAPS&, const UINT nRotateDir);
	virtual void DoProc_CameraMoveSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown);
	virtual void DoProc_CameraRotateSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown);
	virtual void DoProc_SelectTarget(const SKEYCAPS& cKeyCaps);
	virtual void DoProc_CameraInterpMoveTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown);
	virtual void DoProc_CameraInterpRotateTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown);
	virtual void DoProc_CameraInterpTargetSelectTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown);
	virtual void DoProc_CameraInterpClear(const SKEYCAPS&);
	virtual void DoProc_CameraPlay(const SKEYCAPS&);
	virtual void DoProc_CameraStop(const SKEYCAPS&);
	virtual void DoProc_CameraRewind(const SKEYCAPS&);
};