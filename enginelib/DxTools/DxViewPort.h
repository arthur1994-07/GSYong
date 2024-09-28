// dxviewport.h: interface for the DxViewPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DXVIEWPORT_H__INCLUDED_)
#define _DXVIEWPORT_H__INCLUDED_

#pragma once

#include "DxObject.h"
#include "DxCustomTypes.h"
#include "DxCamAniMan.h"
#include "DxDamper.h"

class GLActiveMap;

struct SCameraControl
{
	D3DXVECTOR3 vBaseCamPos;
	float		fUp;
	float		fDown;
	float		fLeft;
	float		fRight;
	float		fFar;
	float		fNear;

	bool		bCameraControl;


    SCameraControl()
		: vBaseCamPos( 0.0f, 0.0f, 0.0f )
		, fUp( 0.0f )
		, fDown( 0.0f )
		, fLeft( 0.0f )
		, fRight( 0.0f )
		, fFar( 0.0f )
		, fNear( 0.0f )
		, bCameraControl( FALSE )
	{
	}

	void Init()
	{
		vBaseCamPos    = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		fUp			   = 0.0f;
		fDown		   = 0.0f;
		fLeft		   = 0.0f;
		fRight		   = 0.0f;
		bCameraControl = FALSE;
	}

	D3DXVECTOR3 GetMaxPos()
	{
		return D3DXVECTOR3( vBaseCamPos.x + fRight, vBaseCamPos.y + fUp, vBaseCamPos.z + fFar );
	}

	D3DXVECTOR3 GetMinPos()
	{
		return D3DXVECTOR3( vBaseCamPos.x + fLeft, vBaseCamPos.y + fDown, vBaseCamPos.z + fNear );
	}
};

class DxViewPort : DxObject
{
protected:
	HWND				m_hWnd;
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	GLActiveMap*		m_pActiveMap;

protected:
	CAMERA_TYPE		m_emCamera;
	DWORD			m_dwFlag;
	bool			m_bHandling;
	bool			m_bKeyboard;

protected:
	float			m_fZOOMIN_COLL;	// 충돌에 의해 최소화 될 수 있는 한계치 Zoom Min
	float			m_fZOOMIN;
	float			m_fZOOMOUT;
	float			m_fZOOMOUT_MapSetting;	// Map 에 셋팅되어진 값.
	float			m_fHIGHXDIR;
	float			m_fLOWXDIR;

	float			m_fVELOZOOM;
	float			m_fVELODIR;
	float			m_fVELOMOVE;

	float			m_fSpecCamMove;

	DxDamper<D3DXVECTOR3>	m_damperVeloMove;
	DxDamper<float>			m_damperVeloZoom;
	DxDamper<D3DXVECTOR3>	m_damperVeloDir;

protected:
	D3DXMATRIX		m_matView;
	D3DXVECTOR3		m_vFromPt;
	D3DXVECTOR3		m_vLookatPt;
	D3DXVECTOR3		m_vUpVec;

	float			m_fLength;
	float			m_fLengthModify;

protected:
	D3DXVECTOR3		m_vLookDir;
	D3DXVECTOR3		m_vLookDir_Y0;
	float			m_fDistance;
	SCameraControl  m_sCameraControl;

protected:
	D3DXMATRIX		m_matProj;
	float			m_fFOV;
	float			m_fWidth;
	float			m_fHeight;
	float			m_fNearPlane;
	float			m_fFarPlane;
    float			m_fClip_FarPlane;		// 충돌처리를 위한 FarPlane
	float			m_fBackUp_FarPlane;		// 백업 FarPlane

	D3DXVECTOR3		m_ptViewPort[8];

	D3DVIEWPORTQ	m_ViewPort;

	BOOL			m_bQUAKE_ACTIVE;
	float			m_fQUAKE_LIFE;
	float			m_fQUAKE_TERM;
	float			m_fQUAKE_SCALE;

	float			m_fQUAKE_TIMER;
	float			m_fQUAKE_TERM_TIMER;

	D3DXVECTOR3		m_vQUAKE_FRMDIS;
	D3DXVECTOR3		m_vQUAKE_LKTDIS;

	bool			m_bCamCollTest;
	DxLandMan		*m_pWorldLandMan;

	// for tutorial. by luxes.
	bool			m_bTutorial;
	bool			m_bTutoCanCamMove;

	D3DXVECTOR3		m_vTutoCamRot;
	float			m_fTutoCamZoom;

	D3DXVECTOR3		m_vTutoCamPos;
	float			m_fTutoCamLength;
	float			m_fTutoThetaX;
	float			m_fTutoThetaY;
	//

protected:
	float			m_fToDoHeight;
	float			m_fElapsedTime;

public:
	CLIPVOLUME		m_CV;

public:
	void SetActiveMap ( GLActiveMap *pActiveMap )	{ m_pActiveMap = pActiveMap; }

public:
	BYTE CollisionMap ( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, D3DXVECTOR3 &vColl );

public:
	void SetCameraType ( CAMERA_TYPE toType );
	CAMERA_TYPE GetCameraType ()				{ return m_emCamera; }
	void SetHandling ( bool bHandling )			{ m_bHandling = bHandling; }
	void SetKeyboard ( bool bHandling )			{ m_bKeyboard = bHandling; }

public:
	HRESULT SetViewTrans ( const D3DXVECTOR3 &vFromPt, const D3DXVECTOR3 &vLookatPt, const D3DXVECTOR3 &vUpVec );
    HRESULT SetProjection();
	HRESULT SetProjection ( float fFOV, float fWidth, float fHeight, float fNearPlane, float fFarPlane );
	HRESULT SetReflectProjection ( float fNearPlane );
	HRESULT SetProjection_FarPlane ( float fFarPlane );
	HRESULT SetProjection_FarPlane ( float fFarPlane, float fFOV, float fAspect );
	HRESULT SetProjectionOrthoLH( float fWidth, float fHeight, float fNearPlane, float fFarPlane );
	HRESULT SetViewPort ();
	void	SetFarPlane ( float fFarPlane );
	void	SetCamCollisionTest( const bool bCollisionTest, DxLandMan *pLandMan = NULL )	{ m_bCamCollTest = bCollisionTest; m_pWorldLandMan = pLandMan; }

    void BackUpFarPlane();		// 백업
	void RollBackFarPlane( LPDIRECT3DDEVICEQ pd3dDevice );	// 롤빽

public:
	D3DXVECTOR3& GetFromPt ()						{ return m_vFromPt; }
	void SetFromPt( const D3DXVECTOR3& vFromPt )	{ m_vFromPt=vFromPt; }
	D3DXVECTOR3& GetLookatPt ()						{ return m_vLookatPt; }
	D3DXMATRIX& GetMatProj ()						{ return m_matProj; } 
	D3DXMATRIX& GetMatView ()						{ return m_matView; }
	HRESULT GetViewTrans ( D3DXVECTOR3 &vFromPt, D3DXVECTOR3 &vLookatPt, D3DXVECTOR3 &vUpVec );

	void SetCameraControl( SCameraControl sCameraControl ) { m_sCameraControl = sCameraControl; }
	void SetCameraControl( bool bCameraControl, D3DXVECTOR3 vBaseCamPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 
						   float fUp = 0.0f, float fDown = 0.0f, float fLeft = 0.0f, float fRight = 0.0f, float fFar = 0.0f, float fNear = 0.0f );

	void SetZoomOutMapSetting( float fZoomOut )		{ m_fZOOMOUT_MapSetting = fZoomOut; }
	void SetZoomOutDefaultGameSetting()				{ m_fZOOMOUT_MapSetting = 200.f;  }

	CLIPVOLUME& GetClipVolume ()	{ return m_CV; }
	D3DVIEWPORTQ& GetViewPort ()	{ return m_ViewPort; }

	const D3DXVECTOR3& GetViewPortPoint( int nIndex )			{ return m_ptViewPort[nIndex]; }

	float	GetWidth ()				{ return m_fWidth; }
	float	GetHeight ()			{ return m_fHeight; }

	//	Note : 마우스 포인트 위치상의 3D 좌표.
	//
	BOOL GetMouseTargetPosWnd ( D3DXVECTOR3& vTargetPos );

	D3DXVECTOR3 GetLookDir ()		{ return m_vLookDir; }		// 카메라에서 시점
	D3DXVECTOR3 GetLookDir_Y0 ()	{ return m_vLookDir_Y0; }	// 카메라에서 시점 _ Y 값은 0으로 제한한다.
	float	GetDistance ()			{ return m_fDistance; }		// 카메라에서 시점까지의 거리
	float GetFOV()					{ return m_fFOV; }
	float GetAspect()				{ return m_fWidth/m_fHeight; }
	float GetNearPlane()			{ return m_fNearPlane; }

public:
	D3DXVECTOR3 ComputeUpVecter ( const D3DXVECTOR3 &vLookatPt, const D3DXVECTOR3 &vFromPt );
	void ComputeBoundBox ( D3DXVECTOR3 *pVectors, float fClip_FarPlane );
	void ComputeClipVolume ();
	CLIPVOLUME ComputeClipVolume ( D3DXVECTOR3	&vLookatPt, D3DXVECTOR3 &vFromPt, D3DXVECTOR3 &vUpVec, float fNearLength, BOOL bFar=FALSE );
	CLIPVOLUME ComputeClipVolumeGameSetting ( D3DXVECTOR3	&vLookatPt, D3DXVECTOR3 &vFromPt, D3DXVECTOR3 &vUpVec );
	CLIPVOLUME ComputeClipVolume ( D3DXVECTOR3	&vLookatPt, D3DXVECTOR3 &vFromPt, D3DXVECTOR3 &vUpVec, 
										  float fNearPlane, float fFarPlane, float fWidth, float fHeight, float fFOV );

	void ComputeClipVolumeOrtho( CLIPVOLUME& cv, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, float fWidth, float fHeight );

	float ComputeDistSize ( const float fDist );
	D3DXVECTOR3 ComputeVec3Project ( const D3DXVECTOR3* pV, const D3DXMATRIX* pWorld ); 

	void ComputeClipPoint ();

public:
	void CameraJump ( D3DXVECTOR3 &vJump );
	void CameraMove ( D3DXVECTOR3 &vMove, BOOL bLandScapeMove=TRUE, BOOL bLookAtMove=TRUE );
	void CameraZoom ( float fZoomOut );
	void CameraRotation ( D3DXVECTOR3 &vRotate, BOOL bFrom=FALSE );

	void CameraHeight ( float fHeight );

	void CameraQuake ( float fLife, float fTerm, float fScale );

	bool CameraLimitCheck();

public:
	const bool IsTutorial() const { return m_bTutorial; }
	const bool IsTutorialCanCamMove() const { return m_bTutoCanCamMove; }
	void SetTutorialCamRot(IN const D3DXVECTOR3& vRot) { m_vTutoCamRot = vRot; }
	void SetTutorialCamZoom(IN const float fZoom) { m_fTutoCamZoom = fZoom; }
	void GetTutorialCamPos(OUT D3DXVECTOR3& vOut) const { vOut = m_vTutoCamPos; }
	const float GetTutorialCamLength() const { return m_fTutoCamLength; }
	const float GetTutorialThetaX() const { return m_fTutoThetaX; }
	const float GetTutorialThetaY() const { return m_fTutoThetaY; }

	const float GetLength() const { return m_fLength; }
	const float GetVeloZoom() const { return m_fVELOZOOM; }
	const float GetVeloDir() const { return m_fVELODIR; }
	const float GetVeloMove() const { return m_fVELOMOVE; }

public:
	void CalculateVeloMoveDamper( OUT D3DXVECTOR3& vOut, IN const D3DXVECTOR3& NewVel, IN const float fElapsedTime ) 
	{ 
		vOut = m_damperVeloMove.Calculate(NewVel,fElapsedTime);
	}

	const float CalculateVeloZoomDamper( IN const float NewVel, IN const float fElapsedTime )
	{
		return m_damperVeloZoom.Calculate(NewVel,fElapsedTime);
	}

	void CalculateVeloDirDamper( OUT D3DXVECTOR3& vOut, IN const D3DXVECTOR3& NewVel, IN const float fElapsedTime )
	{
		vOut = m_damperVeloDir.Calculate(NewVel,fElapsedTime);
	}

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, HWND hWnd=NULL )
	{
		m_hWnd = hWnd;
		m_pd3dDevice = pd3dDevice;
		return S_OK;
	}

	virtual HRESULT FrameMove(float fTime, float fElapsedTime);
	void FrameMoveRAN(float fTime, float fElapsedTime);
	void FrameMoveMAX( float fElapsedTime, const D3DXVECTOR3& vCenter );
	void FrameMoveSpecCam( float fElapsedTime );
	void FrameMove_FlyCamera(void);

private:
	virtual HRESULT _FrameMove_20111118(float fTime, float fElapsedTime);


public:
	HRESULT SetGameCamera ();
	HRESULT SetLobyCamera ();
	HRESULT	SetVehicleCamera ();
	HRESULT	SetToolCamera();
	HRESULT SetToolCameraMiniMap();
	void SetFlyCameraFlag();

public: // for tutorial. by luxes.
	void SetTutorial( bool bTutorial ) { m_bTutorial = bTutorial; }
	void SetTutorialCanCameraMove( bool bCan ) { m_bTutoCanCamMove = bCan; }

	HRESULT	SetTutorialCamera(); // 튜토리얼을 위한 default 카메라 세팅. by luxes
	void SetTutorialCameraPos( D3DXVECTOR3 vCamPos ) { m_vTutoCamPos = vCamPos; }
	void SetTutorialCameraLength( float fLength ) { m_fTutoCamLength = fLength; }
	void SetTutorialCameraThetaX( float fThetaX ) { m_fTutoThetaX = fThetaX; }
	void SetTutorialCameraThetaY( float fThetaY ) { m_fTutoThetaY = fThetaY; }

	D3DXVECTOR3 GetTutorialCameraRotation() { return m_vTutoCamRot; }
	float GetTutorialCameraZoom()	{ return m_fTutoCamZoom; }

protected:
	DxViewPort();

public:
	virtual ~DxViewPort();

	//	Note : 싱글톤 클래스 제어.
	//
public:
	static DxViewPort& GetInstance();
};

#endif // !defined(_DXVIEWPORT_H__INCLUDED_)
