#ifndef __DXEFFECTBLURSYS_H_
#define __DXEFFECTBLURSYS_H_

#pragma once

#include <queue>

#include "../../DxTools/DxVertexFVF.h"

#include "DxEffSingle.h"

#define	USEABSOLUTE		0x00000001	// ���� ��ǥ <-> �����ǥ
#define	NOUSE_BEZIER	0x00000002	// ������ �
//#define	USEPARENTEND	0x00000004	// �θ� ������ ���� ������.
#define	USE_LOOP_RESET	0x00000008	// ������ ������ ������ �ʱ�ȭ
#define	USEREFRACT		0x00000010	// ����

struct BLURSYS_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_BLURSYS, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY_100
	{
		BOOL			m_bAbsolute;
		BOOL			m_bBezier;

		int				m_nBlend;
		int				m_nNum;

		float			m_fLife;

		float			m_fLengthStart;
		float			m_fLengthEnd;

		float			m_fAlphaStart;
		float			m_fAlphaEnd;

		D3DXCOLOR		m_cColorStart;
		D3DXCOLOR		m_cColorEnd;

		char			m_szTexture[256];

		PROPERTY_100()
		{
			memset( m_szTexture, 0, sizeof(char)*256 );
		};
	};

	struct PROPERTY			// Note : Ver.101 ~ 102
	{
		DWORD			m_dwFlag;

		int				m_nBlend;
		int				m_nNum;			// ������������ �������� ����.

		float			m_fLife;

		float			m_fLengthStart;
		float			m_fLengthEnd;

		float			m_fAlphaStart;
		float			m_fAlphaEnd;

		D3DXCOLOR		m_cColorStart;
		D3DXCOLOR		m_cColorEnd;

		char			m_szTexture[256];

		PROPERTY()
		{
			memset( m_szTexture, 0, sizeof(char)*256 );
		};
	};

	union
	{
		struct
		{
			PROPERTY		m_Property;
		};

		struct
		{
			DWORD			m_dwFlag;

			int				m_nBlend;
			int				m_nNum;

			float			m_fLife;

			float			m_fLengthStart;
			float			m_fLengthEnd;

			float			m_fAlphaStart;
			float			m_fAlphaEnd;

			D3DXCOLOR		m_cColorStart;
			D3DXCOLOR		m_cColorEnd;

			char			m_szTexture[256];
		};
	};

	//	Note : Ÿ�� ���� �����ϴ� ���ҽ�.
	//
	TextureResource m_textureRes;
	BOOL			m_bBackUpCache;

	//	Note : ����Ʈ ������.
	//
	virtual DxEffSingle* NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup );
	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE );
	virtual HRESULT SaveFile ( sc::SerialFile &SFile );

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache);
	virtual HRESULT DeleteDeviceObjects();

	BLURSYS_PROPERTY () : EFF_PROPERTY(),
		m_nBlend(1),
		m_nNum(10),

		m_fLife(0.5f),

		m_fLengthStart(0.0f),
		m_fLengthEnd(1.0f),

		m_fAlphaStart(1.0f),
		m_fAlphaEnd(0.0f),

		m_cColorStart(1.0f,1.0f,1.0f,1.0f),
		m_cColorEnd(1.0f,1.0f,1.0f,1.0f)

		, m_bBackUpCache(FALSE)
	{
		m_dwFlag = 0;
		m_dwFlag |= USEABSOLUTE;
		StringCchCopy( m_szTexture, 256, "_Eff_flare.tga" );
	}
    virtual ~BLURSYS_PROPERTY()
    {
    }
};

class DxEffectBlurSys : public DxEffSingle
{
	//	Note : ����Ʈ Ÿ�� ����.		--	���� ���� �ڵ�� [����]
	//
public:
	const static DWORD	TYPEID;
	const static DWORD	FLAG;
	const static char	NAME[];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnInitDevice_STATIC();
	static void OnDeleteDevice_STATIC();

public:
	virtual void ReStartEff ();

public:
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha );

	virtual HRESULT FinalCleanup();

public:
	DxEffectBlurSys();
	virtual ~DxEffectBlurSys();

private:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectSB;

private:
	D3DXMATRIX	m_matIdentity;

	// Note : �ִϸ��̼ǿ� ���� ������ ��.
private:
	struct VERTEX_UPDOWN : public sc::PoolAllocMgr
	{
		D3DXVECTOR3	sPos;
	};

	typedef std::deque<VERTEX_UPDOWN*>	DEQ_STOREPOINT;

	DWORD			m_dwKeyPointCOUNT;
	DEQ_STOREPOINT	m_deqKeyPoint;

	// Note : �ܻ��� ����� ���ؼ� ��� ���,�����Ǵ� ��.
private:
	struct HERMITEPOINT : public sc::PoolAllocMgr
	{
		D3DXVECTOR3		vBasePos;
		D3DXVECTOR3		vLookatCross;
		float			fLength;
		D3DXCOLOR		cColor;
		float			fLife;
	};

	typedef std::deque<HERMITEPOINT*>		DEQ_HERMITEPOINT;
	typedef DEQ_HERMITEPOINT::iterator		DEQ_HERMITEPOINT_ITER;

	DEQ_HERMITEPOINT	m_deqHermitePoint;

	// Note : �ܻ��� ����� ���ؼ� ��� ���,�����Ǵ� ��.
private:
	struct BLUR_UPDOWN : public sc::PoolAllocMgr
	{
		VERTEXCOLORTEX	sVertex[2];
	};

	typedef std::deque<BLUR_UPDOWN*>	DEQ_BLURPOINT;
	typedef DEQ_BLURPOINT::iterator		DEQ_BLURPOINT_ITER;

	DEQ_BLURPOINT	m_deqBlurPoint;

	// Note : Į�� �پ� �ִ� �ܻ�
private:
	BOOL			m_bBaseBlurRender;
	VERTEXCOLORTEX	m_sBaseBlur[4];

private:
	float		m_fOneDiveFullLife;	// 1.f / FullLife
	DWORD		m_dwPrevKeyFrame;	// ���� Ű �������� ���� ���� ���´�.

private:
	BOOL IsUnionKey( const D3DXVECTOR3& vBase );
	void CreateNewKeyFrame( D3DXVECTOR3& vBase );
	void FrameMoveKeyFrame();
	void ClearKeyFrame();

	void CreateHermite( int nSubHermite );
	void FrameMoveHermitePoint( float fElapsedTime );
	void ClearHermitePoint();

	void ClearBlurData();
	void CreateBlur( float fAlpha );
	void FrameMoveBlur();

private:
	int SeparateNumOfLine( int nSubHermite );				// Line�� ���� ����
	void DxInsertHermitePoint( const D3DXVECTOR3& vPos );
	void DxInsertHermitePoint( float fRate );
	void GetLookatCross( const D3DXVECTOR3& vBase, const D3DXVECTOR3& vTarget, D3DXVECTOR3& vLookatCross );

private:
	void RenderBlur( LPDIRECT3DDEVICEQ pd3dDevice );


public:
	LPDIRECT3DDEVICEQ		m_pd3dDevice;

public:
	std::string			m_strTexture;
	TextureResource		m_textureRes;

public:
	float			m_fTime;
	float			m_fTime30;
	float			m_fElapsedTime;

	DWORD			m_dwFlag;

	int				m_nBlend;

	float			m_fLife;

	float			m_fLength;
	float			m_fLengthDelta;

	float			m_fAlpha;
	D3DXCOLOR		m_cColor;
	D3DXCOLOR		m_cColorDelta;

private:
	D3DXMATRIX	m_matResult;
};

#endif //__DXEFFECTCAMERA_H_