#pragma once

#include "../../../InnerInterface.h"
#include "../../Util/UIWindowEx.h"

namespace MiniGame
{
	class IDice;
	class GLManagerClient;
	class GLOddEvenClient;
}

struct RenderState
{	
	DWORD dwLight;
	DWORD dwNormalize;
	DWORD dwSpecular;
	DWORD dwAntialiased;
	DWORD dwMagFilter, dwMinFilter;	
	D3DLIGHT9 Light[3];
	D3DMATERIAL9 Material;
	D3DXMATRIXA16 mViewPREV;
	D3DXMATRIXA16 mProjPREV;	
};

class CBasicTextBox;
class CBasicButton;
class CItemImage;
class RnButton;
class RnProgressBar;

