#ifndef DXCHARSHAM_H_
#define DXCHARSHAM_H_

#pragma once

//#include "../../SigmaCore/Net/MsgList.h"
#include "../../EngineLib/DxTools/DxCustomTypes.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../RanLogic/GLogicEx.h"
#include "../../RanLogic/MSG/GLContrlMsg.h"

class DxCharSham : public GLCHARLOGIC
{
public:
	enum
	{
		RECEIVE_CHARDATA	= 0x0001,
	};

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	DxSkinChar*			m_pSkinChar;
	D3DXVECTOR3			m_vDir;
	D3DXVECTOR3			m_vDirOrig;
	D3DXVECTOR3			m_vPos;
	D3DXMATRIX			m_matTrans;

	BOOL				m_bSelect;
	BOOL				m_bEffect;

	//	Note : 현제 상태.
	//
	float				m_fAge;
	EMACTIONTYPE		m_Action;
	float				m_fIdleTime;
	BOOL				m_bPeaceZone;
	BOOL				m_bIdleReserv;

	//	Note : AABB
	//
	D3DXVECTOR3			m_vMaxOrg;
	D3DXVECTOR3			m_vMinOrg;

	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;

	//	Note : 기초 대이터.
	//
protected:
	int					m_nID;
	BOOL				m_bNeedData;
	DWORD				m_dwRevData;
	SCHARINFO_LOBBY		m_CharInfo;

public:
	bool IsValidData() const { return (m_dwRevData&RECEIVE_CHARDATA)!=NULL; }

public:
	void GetAABB(D3DXVECTOR3& vMax, D3DXVECTOR3& vMin);
	void SetSelect(BOOL bSel, BOOL bReserv = FALSE);

public:
	HRESULT Create(const D3DXVECTOR3* pvPos, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bNeedData=FALSE, int nID=0 );
	void	DeleteSkin(); 

	HRESULT UpdateSuit ();
	BOOL	UpdateFaceHair();

	HRESULT SetCharInfo(const SCHARINFO_LOBBY& CharInfo);
	void ResetCharInfo() { m_dwRevData = NULL; }

	void SetDirection(const D3DXVECTOR3& vDir )		{ m_vDir = vDir; }
	const D3DXVECTOR3& GetPosition() const			{ return m_vPos; }

protected:
	void SummonStateVisibleUpdate(void) override {}

protected:
	void TurnAction(EMACTIONTYPE toAction);

public:
	HRESULT FrameMove(float fTime, float fElapsedTime);
	HRESULT Render(LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME& cv, BOOL bRendAABB, const D3DXVECTOR3 &vDiffuse, const D3DXVECTOR3 &vPointLight);
	HRESULT RenderShadow(LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem);

public:
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT InvalidateDeviceObjects();

public:
	DxCharSham();
	~DxCharSham();
};

#endif // DXCHARSHAM_H_
