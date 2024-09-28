#pragma once

#include "GfxScene.h"

class GfxLoginScene : public GfxScene
{
public :
	GfxLoginScene();
	virtual ~GfxLoginScene();

public :
	virtual HRESULT OnInit( LPDIRECT3DDEVICE9 pDevice );
};
