#pragma once

#include <vector>

namespace sc
{
	class SerialFile;
};

typedef std::vector<D3DXVECTOR3>	VEC_VECTOR3;
typedef VEC_VECTOR3::iterator		VEC_VECTOR3_ITER;

struct WATERSURFVERTEX
{
	D3DXVECTOR3	vPos;
	DWORD		dwColor;
	D3DXVECTOR2 vTex;
	const static DWORD FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
};

struct WATERSURF_PROPERTY
{
	BOOL	m_bPlaneInverse;			// 
	float	m_fPlaneWidth;				//	
	float	m_fPlaneHeight;				//	
	float	m_fSurfVelocityINIT;		// �ʱ� ���� �̵� ���ǵ�
	float	m_fEdit_SurfAcceleration;	// [Edit] m_fSurfVelocity �پ��� ��ġ
	float	m_fSurfSleepTimeINIT;		// ���� �ẹ �ð�
	float	m_fAlphaAddition;			// ���� �߰� ���ǵ�
	float	m_fAlphaSubtractionTime;	// ���� ���� �ð�
	float	m_fAlphaSubtraction;		// ���� ���� ���ǵ�
	float	m_fSideLineInterval;		// �ĵ��� �������� ������ �ð�����

	WATERSURF_PROPERTY()
		: m_bPlaneInverse(FALSE)
		, m_fPlaneWidth(30.f)
		, m_fPlaneHeight(30.f)
		, m_fSurfVelocityINIT(30.f)
		, m_fEdit_SurfAcceleration(10.f)
		, m_fSurfSleepTimeINIT(3.f)
		, m_fAlphaAddition(0.7f)
		, m_fAlphaSubtractionTime(3.f)
		, m_fAlphaSubtraction(0.4f)
		, m_fSideLineInterval(0.3f)
	{
	}

	void Save( sc::SerialFile &SFile );
	void Load( sc::SerialFile &SFile );
};

#define PLANE_IN_VERTEX_NUM	6
#define PLANE_LINK_VERTEX   4
#define VERTEX_MIDDLE_RATE	0.2f