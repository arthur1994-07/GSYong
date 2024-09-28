#ifndef __DXEFFECTCAMERA_H_
#define __DXEFFECTCAMERA_H_

#pragma once

#include "DxEffSingle.h"

// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  ( x = (x<min ? min : x<max ? x : max) )

struct CAMERA_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_CAMERA, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY		// Note : Ver.100 ~ 101
	{
		int				m_iCameraSet;
		float			m_fScale;
		float			m_fPlayNum;
	};

	union
	{
		struct
		{
			PROPERTY		m_Property;
		};

		struct
		{
			int				m_iCameraSet;
			float			m_fValue;
			float			m_fPlayNum;
		};
	};

	//	Note : 이팩트 생성자.
	//
	virtual DxEffSingle* NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup );
	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE );
	virtual HRESULT SaveFile ( sc::SerialFile &SFile );

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache);
	virtual HRESULT DeleteDeviceObjects();

	CAMERA_PROPERTY () : EFF_PROPERTY(),
		m_iCameraSet(1),
		m_fValue(0.5f),
		m_fPlayNum(20.f)
	{
	}
    virtual ~CAMERA_PROPERTY()
    {
    }
};

class DxEffectCamera : public DxEffSingle
{
public:
	enum
	{
		CAMERA_MOVE_UP_DOWN	= 1,
		CAMERA_ROTATION_Y	= 2,
	};

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

	//	Note : 파티클 공통 리소스 자원들 관리.
	//
protected:
	static BOOL						VAILEDDEVICE;

protected:
	DWORD					m_dwSavedRenderStats;
	DWORD					m_dwDrawRenderStats;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnInitDevice_STATIC();
	static void OnDeleteDevice_STATIC();

public:
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha );

	virtual HRESULT FinalCleanup();

public:
	DxEffectCamera(void);
	virtual ~DxEffectCamera(void);

public:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	int				m_iCameraSet;
	float			m_fValue;		// CAMERA_MOVE_UP_DOWN - 진동강도, CAMERA_ROTATION_Y - 회전속도,
	float			m_fPlayTime;

public:
	D3DXVECTOR3		m_vCameraMoveSum;
	float			m_fTimeSum;
};

#endif //__DXEFFECTCAMERA_H_