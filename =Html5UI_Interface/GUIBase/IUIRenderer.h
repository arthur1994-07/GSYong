#pragma once

#include "UIPosData.h"
#include "UIImageData.h"

namespace GUIBase
{

/*--------------------------------------
	Rendering을 위한 Interface;
--------------------------------------*/
class IUIRenderer
{
public:
	IUIRenderer() { }
	virtual ~IUIRenderer() { }

	//! 세팅 Render State;
	virtual void SetRenderState() = 0;

	//! 회복 Render State;
	virtual void RecoverRenderState() = 0;

	//! Device
	virtual bool InitDevice() = 0;
	virtual bool RestoreDevice() = 0;
	virtual bool InvalidateDevice() = 0;
	virtual bool DeleteDevice() = 0;

	//! 파일을 이용해 Texture 그리기;
	virtual bool RenderTextureUsingFile(
		UIID uiID,
		const std::string& textureID,
		COMPONENT_POSITION& compPosition ) = 0;

	//! size 크기의 Texture에 Image Data 갱신시키기;
	virtual bool RenderTextureUsingData(
		UIID uiID,
		UI_IMAGE_DATA& imgData,
		COMPONENT_POSITION& compPosition ) = 0;

	//! 만들어진 UI 그리기;
	virtual void RenderUI( UIRECT rcGlobalPos
		, UIID uiID ) = 0;
};

}