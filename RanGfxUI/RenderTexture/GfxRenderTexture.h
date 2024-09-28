#pragma once

class GfxRenderTextureBase;

class GfxRenderTexture
{
public :
	GfxRenderTexture(void);
	virtual ~GfxRenderTexture(void);

private :
	// gfx �ؽ�ó
	GfxRenderTextureBase*	m_pTextureBase;
	// �÷��� Linkage ID
	std::string				m_strLinkID;

protected :
	// �ؽ�ó ������
	UINT m_nRenderTextureWidth;
	UINT m_nRenderTextureHeight;

	// ������ �� �ؽ�ó�� ���� �̹��� ��Ⱦ��
	float	m_fAspect;
	// ���۷��� ī��Ʈ
	UINT	m_nRefCount;

public :
	virtual void OnResetDevice();
	virtual void OnLostDevice();

protected :
	// gfx �ؽ�ó ����
	HRESULT CreateGfxTexture( LPDIRECT3DTEXTUREQ pTexture );

public :
	// �÷��� �̹����� �ؽ�ó�� ��ü
	void ReplaceGfxTexture( const std::string& strLinkID = "", const INT nWidth = 0, const INT nHeight = 0 );
	// �ؽ�ó ������ ���
	void GetGfxTextureSize( int& nWidth, int& nHeight );

	// ���۷��� ī��Ʈ ����
	void IncreaseRefCount() { ++m_nRefCount; }
	void ReduceRefCount() { --m_nRefCount; }

	UINT GetRefCount() { return m_nRefCount; }
};