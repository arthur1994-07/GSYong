#pragma once

#include "../../RanGfxUI/RenderTexture/GfxRenderTexture.h"

// �ؽ�ó Ÿ�� ( ������ WidgetDef.lua ���� ���� ���� )
enum EMWIDGET_TEXTURE
{
	EMWIDGET_TEXTURE_NONE = 0,

	EMWIDGET_TEXTURE_DICE = 1,
	EMWIDGET_TEXTURE_ITEM_PREVIEW = 2,

	EMWIDGET_TEXTURE_SIZE
};

// UI�� �������� �ؽ�ó�� �Ѹ��� ���� Ŭ����
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
	// ����� Ŭ���� �÷�
	D3DCOLOR m_dwClearColor;

protected :
	// �޸� ����
	virtual void Clear();

public :
	// ���� Ŭ�������� �����ǽ� �ݵ�� ȣ��
	virtual bool OnCreate();
	virtual void OnResetDevice();
	virtual void OnLostDevice();

	virtual void OnFrameMove( float fElapsedTime );

public :
	// ����� ������ ����, ���� �������� OnRender() �Լ����� �����Ѵ�.
	void OnFrameRender();

protected :
	// �ؽ�ó ������ ����, default : 1024 x 1024
	virtual void InitTextureSize();
	// ����� Ŭ���� �÷� ����, defalt : 0x00000000 (����)
	virtual void InitClearColor();

	// ���� ������, ���� Ŭ�������� ��뿡 �°� �����Ѵ�.
	virtual void OnRender() = 0;

public :
	virtual void CleanUp() {}
};

typedef std::map< EMWIDGET_TEXTURE, GLWidgetTexture* >	MAP_WIDGET_TEXTURE;
typedef MAP_WIDGET_TEXTURE::iterator					MAP_WIDGET_TEXTURE_ITER;

//---------------------------------------------------------------------------------
// ���� �ؽ�ó ������ Ŭ����
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
	// �ؽ�ó ����
	GfxRenderTexture* CreateTexture( EMWIDGET_TEXTURE emTexture );
	// �ؽ�ó �˻�
	GfxRenderTexture* FindTexture( EMWIDGET_TEXTURE emTexture );
};