// DxStaticMaterialMeshFrameAni.h
//
//	class DxStaticMaterialMeshFrameAni : 내부에 DxStaticMaterialMeshAni 정보가 아닌 ID 를 가지고 있다.
//										내부에 둘 경우 Render 시 필요없는 Tree 를 타야할 경우도 생겨서, 분리시켜 놓았다.
//
//	Name : Sung-Hwan Han
//	Begin :2013/07/09
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "../../../SigmaCore/File/SerialFile.h"

struct DxAnimation;
struct DxFrame;
class CMaterialSetting;
class DxStaticMaterialMeshAniMan;


class DxStaticMaterialMeshFrameAni
{
private:
	enum
	{
		VERSION = 0x0100,           // 기본적인 Version
	};

private:

	union
	{
		struct { D3DXVECTOR3 m_vTreeMax; };
		struct { float m_fMaxX, m_fMaxY, m_fMaxZ; };
	};
	union
	{
		struct { D3DXVECTOR3 m_vTreeMin; };
		struct { float m_fMinX, m_fMinY, m_fMinZ; };
	};

	//	Note : 트렌스폼 정보.
	//
	D3DXMATRIX		m_matRot;				//	해당시간(에니가 있을때)의 트렌스폼.
	D3DXMATRIX		m_matCombined;			//	부모 계층 프레임 누적 트렌스폼.

	DxAnimation		*m_pframeFromAnimate;	//	이 프레임을 제어하는 에니메이션.

	DWORD			m_dwStaticMeshMaterialAniID;	// DxStaticMaterialMeshAni 과 연결된 ID ( 나중에 Load 시 연결해주기 위함. )

	//	Note : 자식 노드 와 형제 노드 관리.
	DxStaticMaterialMeshFrameAni			*m_pframeSibling;		//	형제 노드들
	DxStaticMaterialMeshFrameAni			*m_pframeFirstChild;	//	자식 노드들

public:
	void UpdateMatrix( float fGlobalTime, const D3DXMATRIX &matCur, bool bAniLoop=true );
	void UpdateMirrorMatrix( float fHeight );

	void GetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3&  vMin );

	void Import( LPDIRECT3DDEVICEQ pd3dDevice, 
				DxFrame* pframeCur, 
				float fUNITTIME, 
				DxStaticMaterialMeshAniMan* pStaticMaterialMeshAniMan, 
				const CMaterialSetting *pMaterialSetting, 
				std::vector<D3DXMATRIX*>& vecAnimationMatrixPointer,
				float fCellSize,
				BOOL bParentAnimation=FALSE );			// bParentAnimation - 내부에서 쓰이는 코드. 사용자가 셋팅할 필요가 없음.

	void CleanUp();

public:
	void Save ( sc::SerialFile& SFile );
	void Load ( sc::SerialFile& SFile, DxStaticMaterialMeshAniMan* pStaticMaterialMeshAniMan, std::vector<D3DXMATRIX*>& vecAnimationMatrixPointer );

private:
	void SaveTree( sc::SerialFile& SFile );
	void LoadTree( sc::SerialFile& SFile, DxStaticMaterialMeshAniMan* pStaticMaterialMeshAniMan, std::vector<D3DXMATRIX*>& vecAnimationMatrixPointer );

public:
	DxStaticMaterialMeshFrameAni();
	~DxStaticMaterialMeshFrameAni();
};