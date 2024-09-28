#pragma once

#include "../../RanGfxUI/RenderTexture/GfxRenderTexture.h"

// 텍스처 타입 ( 수정시 WidgetDef.lua 파일 같이 수정 )
enum EMWIDGET_TEXTURE
{
	EMWIDGET_TEXTURE_NONE = 0,

	EMWIDGET_TEXTURE_DICE = 1,
	EMWIDGET_TEXTURE_ITEM_PREVIEW = 2,

	EMWIDGET_TEXTURE_SIZE
};

// UI에 렌더링된 텍스처를 뿌리기 위한 클래스
class GLWidgetTexture : public GfxRenderTexture
{
public :
	GLWidgetTexture( LPDIRECT3DDEVICEQ pDeivce );
	virtual ~GLWidgetTexture(void);

protected :
	LPDIRECT3DDEVICEQ	m_pDevice;

	LPDIRECT3DSURFACEQ	m_pSurface;
	LPDIRECT3DTEXTUREQ	m_pTexture;

protected :
	// 백버퍼 클리어 컬러
	D3DCOLOR m_dwClearColor;

protected :
	// 메모리 해제
	virtual void Clear();

public :
	// 하위 클래스에서 재정의시 반드시 호출
	virtual bool OnCreate();
	virtual void OnResetDevice();
	virtual void OnLostDevice();

	virtual void OnFrameMove( float fElapsedTime );

public :
	// 공통된 렌더링 정의, 실제 렌더링은 OnRender() 함수에서 정의한다.
	void OnFrameRender();

protected :
	// 텍스처 사이즈 설정, default : 1024 x 1024
	virtual void InitTextureSize();
	// 백버퍼 클리어 컬러 설정, defalt : 0x00000000 (투명)
	virtual void InitClearColor();

	// 실제 렌더링, 하위 클래스에서 사용에 맞게 정의한다.
	virtual void OnRender() = 0;

public :
	virtual void CleanUp() {}
};

typedef std::map< EMWIDGET_TEXTURE, GLWidgetTexture* >	MAP_WIDGET_TEXTURE;
typedef MAP_WIDGET_TEXTURE::iterator					MAP_WIDGET_TEXTURE_ITER;

//---------------------------------------------------------------------------------
// 렌더 텍스처 관리용 클래스
class GLWidgetTextureMan
{
private :
	GLWidgetTextureMan(void);
	GLWidgetTextureMan( const GLWidgetTextureMan& value );
	~GLWidgetTextureMan(void);

public :
	static GLWidgetTextureMan& GetInstance();

private :
	LPDIRECT3DDEVICEQ	m_pDevice;

	MAP_WIDGET_TEXTURE	m_mapWidgetTexture;

public :
	void OnCreateDevice( LPDIRECT3DDEVICEQ pDevice );
	void OnResetDevice();
	void OnLostDevice();
	void OnDestroyDevice();
	void OnFrameMove( float fElapsedTime );
	void OnFrameRender();
	void CleanUp();

public :
	// 텍스처 생성
	GfxRenderTexture* CreateTexture( EMWIDGET_TEXTURE emTexture );
	// 텍스처 검색
	GfxRenderTexture* FindTexture( EMWIDGET_TEXTURE emTexture );
};