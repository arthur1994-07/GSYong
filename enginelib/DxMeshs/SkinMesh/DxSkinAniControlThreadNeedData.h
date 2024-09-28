#pragma once

#include <list>
#include <deque>
#include <vector>
#include <tr1/memory>
#include "../../DxTools/DxVertexFVF.h"

struct SMeshContainer;
class DxSkinAniControl;
class DxSkeletonMaintain;



//////////////////////////////////////////////////////////////////////////
//					DxSkinAniControlThreadNeedData
//////////////////////////////////////////////////////////////////////////
class DxSkinAniControlThreadNeedData
{
public:
	float*		m_arrayKeyJiggleWeight;
	DWORD		m_dwCharPartNUM;
	WORDWORD	m_wwMoveParentBoneIDForSafeModeWeaponSlot;
	//float			m_afKeyJiggleWeight[PIECE_SIZE+1];	// KeyFrame 과 JiggleBone 의 가중치 값.	 [PIECE_SIZE 은 기본 Skin Bone] [0 부터는 Parts~ Attach Bone]

public:
	DxSkinAniControlThreadNeedData( int wCharPartNUM );
	~DxSkinAniControlThreadNeedData();
};



//////////////////////////////////////////////////////////////////////////
//						CharPartBoneMatrices
//////////////////////////////////////////////////////////////////////////
class CharPartBoneMatrices
{
private:
	// 기본
	const SMeshContainer*	m_rMeshContainer;

	// m_vecarrayBoneMatrices 사용되는 Index
	DWORD					m_dwUsedIndex;

	// 계산이 진행중인 Index
	DWORD					m_dwCalculateIndex;

	// 사용되는 KetTime
	INT						m_nKeyTime;

	// Bone 갯수
	DWORD					m_dwBoneNum;

	// Thread 상 쓰인다.
	// 매번 복사하던 걸 생성자 때 한번만 복사하도록 한다.
	std::tr1::shared_ptr<DxSkinAniControl> m_spSkinAniControlThread;

	// Bone Matrices 를 저장해 놓고 여러군데에서 사용하도록 한다.
	// Opaque, Hard Alpha, Soft Alplha, Shadow, Glow
	// 배열값 => m_pmcMesh->m_dwNumAttributeGroups * m_pmcMesh->m_dwNumPaletteEntries
	//
	// Thread Loading 때문에 m_arrayBoneMatrices 이 NULL 인 상태로 Render 나 이곳 저곳 들어올 수 있다. 사용시 NULL 체크 후 사용하자.
	//
	std::vector<D3DXMATRIXA16*>	m_vecarrayBoneMatrices;


	//////////////////////////////////////////////////////////////////////////
	//								고스팅.
	//////////////////////////////////////////////////////////////////////////
	//					Main Thread 에서만 동작된다.
	//////////////////////////////////////////////////////////////////////////
	// 사용된다는 신호가 오면 m_listarrayBoneMatricesBackUp 에 50개를 등록한다.
	// Key 가 달라지면 값을 m_listarrayBoneMatricesBackUp 에 있는 걸 m_listarrayBoneMatricesUse 에 넣도록 한다.
	// 시간으로 해서 다 사용되면 m_listarrayBoneMatricesUse 에 있는 걸 m_listarrayBoneMatricesBackUp 로 돌려준다.
	// 갯수가 모자를 경우 m_listarrayBoneMatricesUse 에 가장 오래 사용된 걸 m_listarrayBoneMatricesUse 에 다시 등록한다.
	// 
	// 속도상의 이유로 m_vecarrayBoneMatrices 의 주소값과 스왑함.
	//
	// 스왑을 하기 위해 메모리단편화가 생기더라도 Pool 을 사용하지 않음.
	//
	struct GhostingData
	{
		float			m_fLifeTime;
		D3DXMATRIXA16*	m_arrayBoneMatrices;

		GhostingData()
			: m_fLifeTime(0.f)
			, m_arrayBoneMatrices(NULL)
		{
		}
	};
	std::list<GhostingData*>	m_listarrayGhostingDataBackUp;
	std::deque<GhostingData*>	m_dequearrayGhostingDataUse;
	BOOL						m_bGhostingON;						// Ghosting 이 작동되는 중인가~?
	INT							m_nPrevKeyTimeForGhosting;
	float						m_fLifeTimeForGhosting;


public:
	void FrameMove_PureThread();

	const D3DXMATRIXA16* GetBoneMatrices() const;

	void SwapBoneMatrices( float fElapsedTime );


	// Ghosting
public:
	// Ghosting 활성화
	void GhostingON( BOOL bGhosting, float fLifeTime );

	const D3DXMATRIXA16* GetBoneMatricesForGhosting( size_t nIndex ) const;


	// 위치값을 바꿔져야 할 때 사용한다.
public:
	void SetPosOffset( const D3DXVECTOR3& vPosOffset );


private:
	void CaculateBoneMatrices( const SMeshContainer& sMeshContainer, 
								D3DXMATRIXA16* arrayBoneMatrices,
								const DxSkeletonMaintain& sSkeleton );

public:
	CharPartBoneMatrices( const SMeshContainer& sMeshContainer, std::tr1::shared_ptr<DxSkinAniControl>& spSkinAniControlThread );
	~CharPartBoneMatrices();
};