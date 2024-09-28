#pragma once

#include <vector>
#include "./WaterSurfDef.h"

class DxSetLandMan;


class WaterSurfPlane_Part
{
private:
	struct SURF_PART_DATA
	{
		D3DXVECTOR2 m_sTexUV[PLANE_IN_VERTEX_NUM];	// Surf ����
		float	m_fTime;							//
		BOOL	m_bPlay;							// Play ���ΰ� ?
		float	m_fAlpha;							// ���İ�
		float	m_fSurfVelocity;					// ���� �̵� ���ǵ�					m_fEdit_SurfVelocity * 0.01f
		float	m_fSurfSleepTime;					// ���� �ẹ �ð�					������� ������ �����̴� ���̴�.

		void Create();
		void Play( float fSurfSleepTime );
		BOOL IsPlay()										{ return m_bPlay; }
		void FrameMove( const WATERSURF_PROPERTY& sProp, 
						float fSurfAcceleration, 
						float fElapsedTime );
		void ResetData( float fSurfVelocityINIT );

		SURF_PART_DATA();
	};

private:
	SURF_PART_DATA*	m_pSurfPartData;
	int				m_nPlaneNUM;
	int				m_nPlaneStart;
	int				m_nPlaneEnd;
	int				m_nPlaneNUM_Div_3;	// m_nPlaneNUM / 3

private:
	float m_fSurfSleepTime;		// ���� �ẹ �ð�					������� ������ �����̴� ���̴�.
	float m_fSurfAcceleration;	// m_fSurfVelocity �پ��� ��ġ	m_fEdit_SurfAcceleration * 0.01f

public:
	void Create( const WATERSURF_PROPERTY& sProp, WATERSURFVERTEX* pVertex, int nPlaneNUM );

public:
	void ResetData( const WATERSURF_PROPERTY& sProp, BOOL bInit );			// bInit - ó�������ΰ� ?

public:
	void Play( const WATERSURF_PROPERTY& sProp, BOOL bInit );				// bInit - ó�������ΰ� ?
	void FrameMove( const WATERSURF_PROPERTY& sProp, float fElapsedTime );
	void Render( WATERSURFVERTEX* pVertex, WATERSURFVERTEX*& pLocked, bool& bFirst );

public:
	WaterSurfPlane_Part();
	~WaterSurfPlane_Part();

    int GetPlaneStart() { return m_nPlaneStart; }
    int GetPlaneEnd() { return m_nPlaneEnd; }
};

class WaterSurfPlane
{
private:
	enum { VERSION = 0x0100 };

private:
	WaterSurfPlane_Part*	m_pWaterSurtPlane;

private:
	int					m_nPlaneNUM;
	WATERSURFVERTEX*	m_pVertex;

public:
	void Play();
	void ResetData( const WATERSURF_PROPERTY& sProp );
	void AddHeight( float fHeight );
    void SetObjRotate90( const D3DXMATRIX& matRotate );
    void CountVertexMax( UINT& nVertexMax );
    void CountVertexRender( UINT& nVertexMax );

public:
	BOOL Create( DxSetLandMan* pSetLandMan, const WATERSURF_PROPERTY& sProp, const VEC_VECTOR3& vecWaterSurfPOS );
	void FrameMove( const WATERSURF_PROPERTY& sProp, float fElapsedTime );
	void Render( WATERSURFVERTEX*& pLocked, bool& bFirst );

private:
	void ResetPosition( DxSetLandMan* pSetLandMan, float fHeight, const VEC_VECTOR3& vecWaterSurfPOS, const VEC_VECTOR3& vecPlaneUp, const VEC_VECTOR3& vecCross );
	void PositionDot( D3DXVECTOR3& vOut, const D3DXVECTOR3& vCenter );

public:
	void Save( sc::SerialFile &SFile );
	void Load( sc::SerialFile &SFile, const WATERSURF_PROPERTY& sProp );

	void Import( const WaterSurfPlane* pSrc, const WATERSURF_PROPERTY& sProp );

private:
	void CleanUp();

public:
	WaterSurfPlane();
	~WaterSurfPlane();
};
