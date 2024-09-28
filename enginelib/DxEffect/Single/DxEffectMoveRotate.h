#ifndef __DxEffectMoveRotate_H_
#define __DxEffectMoveRotate_H_

#pragma once

#include "DxEffSingle.h"

#define	USERANDROTATE					0x00000001	// 처음의 랜덤 회전
#define	USEGOTOCENTER					0x00004000	// 센터 방향으로 이동 시킴
#define USEPARENTROTATE					0x00040000	// 부모 회전 영향받음;

// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  ( x = (x<min ? min : x<max ? x : max) )

struct MOVEROTATE_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_MOVEROTATE, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY_100
	{
		float		m_fRotateAngelX;
		float		m_fRotateAngelY;
		float		m_fRotateAngelZ;
	};

	struct PROPERTY_101
	{
		D3DXVECTOR3		m_vVelocity;

		float			m_fRotateAngelX;
		float			m_fRotateAngelY;
		float			m_fRotateAngelZ;
	};

	struct PROPERTY		// Ver. 102 ~ 103
	{
		DWORD			m_dwFlag;

		D3DXVECTOR3		m_vVelocity;

		float			m_fRotateAngelX;
		float			m_fRotateAngelY;
		float			m_fRotateAngelZ;
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
			D3DXVECTOR3		m_vVelocity;
			float			m_fRotateAngelX;
			float			m_fRotateAngelY;
			float			m_fRotateAngelZ;
		};
	};

	//	Note : 이팩트 생성자.
	//
	virtual DxEffSingle* NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup );
	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE );
	virtual HRESULT SaveFile ( sc::SerialFile &SFile );

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache);
	virtual HRESULT DeleteDeviceObjects();

	MOVEROTATE_PROPERTY () : EFF_PROPERTY(),
		m_vVelocity(0.f,0.f,0.f),
		m_fRotateAngelX(0.f),
		m_fRotateAngelY(0.f),
		m_fRotateAngelZ(0.f)
	{
		m_dwFlag = 0L;
	}
    virtual ~MOVEROTATE_PROPERTY()
    {
    }
};

class DxEffectMoveRotate : public DxEffSingle
{
	//	Note : 이펙트 타입 정의.		--	정적 정의 코드부 [시작]
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
	virtual void CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

public:
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha );

	virtual HRESULT FinalCleanup();

public:
	DxEffectMoveRotate(void);
	virtual ~DxEffectMoveRotate(void);

public:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	D3DXMATRIX			m_matWorld;

public:
	D3DXVECTOR3		m_vGLocation;

	float			m_fTransTime;
	float			m_fRotateTime;

	DWORD			m_dwFlag;

	D3DXVECTOR3		m_vVelocity;

	float			m_fRotateAngelXSum;
	float			m_fRotateAngelYSum;
	float			m_fRotateAngelZSum;

public:
	float			m_fRand;
	float			m_fRotateAngelX;
	float			m_fRotateAngelY;
	float			m_fRotateAngelZ;

protected:
	float			m_fSettingX;
	float			m_fSettingY;
	float			m_fSettingZ;
};

#endif //__DxEffectMoveRotate_H_