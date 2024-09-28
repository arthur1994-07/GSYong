#pragma once

#include "UIPosData.h"
#include "UIImageData.h"

namespace GUIBase
{

/*--------------------------------------
	Rendering�� ���� Interface;
--------------------------------------*/
class IUIRenderer
{
public:
	IUIRenderer() { }
	virtual ~IUIRenderer() { }

	//! ���� Render State;
	virtual void SetRenderState() = 0;

	//! ȸ�� Render State;
	virtual void RecoverRenderState() = 0;

	//! Device
	virtual bool InitDevice() = 0;
	virtual bool RestoreDevice() = 0;
	virtual bool InvalidateDevice() = 0;
	virtual bool DeleteDevice() = 0;

	//! ������ �̿��� Texture �׸���;
	virtual bool RenderTextureUsingFile(
		UIID uiID,
		const std::string& textureID,
		COMPONENT_POSITION& compPosition ) = 0;

	//! size ũ���� Texture�� Image Data ���Ž�Ű��;
	virtual bool RenderTextureUsingData(
		UIID uiID,
		UI_IMAGE_DATA& imgData,
		COMPONENT_POSITION& compPosition ) = 0;

	//! ������� UI �׸���;
	virtual void RenderUI( UIRECT rcGlobalPos
		, UIID uiID ) = 0;
};

}