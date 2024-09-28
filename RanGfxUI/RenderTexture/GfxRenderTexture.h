#pragma once

class GfxRenderTextureBase;

class GfxRenderTexture
{
public :
	GfxRenderTexture(void);
	virtual ~GfxRenderTexture(void);

private :
	// gfx 텍스처
	GfxRenderTextureBase*	m_pTextureBase;
	// 플래시 Linkage ID
	std::string				m_strLinkID;

protected :
	// 텍스처 사이즈
	UINT m_nRenderTextureWidth;
	UINT m_nRenderTextureHeight;

	// 렌더링 될 텍스처와 실제 이미지 종횡비
	float	m_fAspect;
	// 레퍼런스 카운트
	UINT	m_nRefCount;

public :
	virtual void OnResetDevice();
	virtual void OnLostDevice();

protected :
	// gfx 텍스처 생성
	HRESULT CreateGfxTexture( LPDIRECT3DTEXTUREQ pTexture );

public :
	// 플래시 이미지를 텍스처로 교체
	void ReplaceGfxTexture( const std::string& strLinkID = "", const INT nWidth = 0, const INT nHeight = 0 );
	// 텍스처 사이즈 얻기
	void GetGfxTextureSize( int& nWidth, int& nHeight );

	// 레퍼런스 카운트 증감
	void IncreaseRefCount() { ++m_nRefCount; }
	void ReduceRefCount() { --m_nRefCount; }

	UINT GetRefCount() { return m_nRefCount; }
};