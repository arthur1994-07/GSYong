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
	//float			m_afKeyJiggleWeight[PIECE_SIZE+1];	// KeyFrame �� JiggleBone �� ����ġ ��.	 [PIECE_SIZE �� �⺻ Skin Bone] [0 ���ʹ� Parts~ Attach Bone]

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
	// �⺻
	const SMeshContainer*	m_rMeshContainer;

	// m_vecarrayBoneMatrices ���Ǵ� Index
	DWORD					m_dwUsedIndex;

	// ����� �������� Index
	DWORD					m_dwCalculateIndex;

	// ���Ǵ� KetTime
	INT						m_nKeyTime;

	// Bone ����
	DWORD					m_dwBoneNum;

	// Thread �� ���δ�.
	// �Ź� �����ϴ� �� ������ �� �ѹ��� �����ϵ��� �Ѵ�.
	std::tr1::shared_ptr<DxSkinAniControl> m_spSkinAniControlThread;

	// Bone Matrices �� ������ ���� ������������ ����ϵ��� �Ѵ�.
	// Opaque, Hard Alpha, Soft Alplha, Shadow, Glow
	// �迭�� => m_pmcMesh->m_dwNumAttributeGroups * m_pmcMesh->m_dwNumPaletteEntries
	//
	// Thread Loading ������ m_arrayBoneMatrices �� NULL �� ���·� Render �� �̰� ���� ���� �� �ִ�. ���� NULL üũ �� �������.
	//
	std::vector<D3DXMATRIXA16*>	m_vecarrayBoneMatrices;


	//////////////////////////////////////////////////////////////////////////
	//								����.
	//////////////////////////////////////////////////////////////////////////
	//					Main Thread ������ ���۵ȴ�.
	//////////////////////////////////////////////////////////////////////////
	// ���ȴٴ� ��ȣ�� ���� m_listarrayBoneMatricesBackUp �� 50���� ����Ѵ�.
	// Key �� �޶����� ���� m_listarrayBoneMatricesBackUp �� �ִ� �� m_listarrayBoneMatricesUse �� �ֵ��� �Ѵ�.
	// �ð����� �ؼ� �� ���Ǹ� m_listarrayBoneMatricesUse �� �ִ� �� m_listarrayBoneMatricesBackUp �� �����ش�.
	// ������ ���ڸ� ��� m_listarrayBoneMatricesUse �� ���� ���� ���� �� m_listarrayBoneMatricesUse �� �ٽ� ����Ѵ�.
	// 
	// �ӵ����� ������ m_vecarrayBoneMatrices �� �ּҰ��� ������.
	//
	// ������ �ϱ� ���� �޸𸮴���ȭ�� ������� Pool �� ������� ����.
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
	BOOL						m_bGhostingON;						// Ghosting �� �۵��Ǵ� ���ΰ�~?
	INT							m_nPrevKeyTimeForGhosting;
	float						m_fLifeTimeForGhosting;


public:
	void FrameMove_PureThread();

	const D3DXMATRIXA16* GetBoneMatrices() const;

	void SwapBoneMatrices( float fElapsedTime );


	// Ghosting
public:
	// Ghosting Ȱ��ȭ
	void GhostingON( BOOL bGhosting, float fLifeTime );

	const D3DXMATRIXA16* GetBoneMatricesForGhosting( size_t nIndex ) const;


	// ��ġ���� �ٲ����� �� �� ����Ѵ�.
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