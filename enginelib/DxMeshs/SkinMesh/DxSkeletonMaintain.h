#pragma once

#include <boost/unordered_map.hpp>
#include <vector>

struct DxSkeleton;
struct DxSkeletonPart;
struct DxBoneTrans;
class BoneTransMap;
class DxSkeletonMaintain;


class DxBoneTransMaintain
{
private:
	DWORD			m_dwIndex;

public:
	D3DXMATRIXA16	m_matCombined;

public:
	DWORD GetIndex() const						{ return m_dwIndex; }

	DxBoneTransMaintain( DWORD dwIndex, const D3DXMATRIX& matCombined )
		: m_dwIndex(dwIndex)
		, m_matCombined(matCombined)
	{
	}

	~DxBoneTransMaintain()
	{
	}
};

class BoneTransMapMaintain
{
public:
	typedef boost::unordered_map<TSTRING, std::tr1::shared_ptr<DxBoneTransMaintain> >	BONE_TRANS_MAP_Maintain;
	typedef BONE_TRANS_MAP_Maintain::iterator					BONE_TRANS_MAP_Maintain_ITER;
	typedef BONE_TRANS_MAP_Maintain::const_iterator				BONE_TRANS_MAP_Maintain_CITER;
	typedef BONE_TRANS_MAP_Maintain::value_type					BONE_TRANS_Maintain_VALUE;

protected:
	BONE_TRANS_MAP_Maintain m_mapBoneMap;

public:
	const DxBoneTransMaintain* findData(const TSTRING& strName) const
	{  
		BONE_TRANS_MAP_Maintain_CITER iterFind = m_mapBoneMap.find(strName);
		if (iterFind != m_mapBoneMap.end())
			return (*iterFind).second.get();
		else
			return NULL;
	}

	void Import_PureThread( const BoneTransMap* pBoneTransMap, const std::vector<DxBoneTrans*> vecReferenceBoneTransSRC, std::vector<DxBoneTransMaintain*>& vecReferenceBoneTrans );

	void CleanUp();
};

// DxSkinChar 마다 하나씩 가지고 있는다.
// DxSkeleton 의 간단형 정보이며, DxSkeleton 은 다수의 DxSkinChar 가 공유하기 때문에 DxSkeletonMaintain 가 필요하게 됨.
class DxSkeletonMaintain
{
private:
	BoneTransMapMaintain				m_sBoneMap;
	std::vector<DxBoneTransMaintain*>	m_vecReferenceBoneTrans;	// 참조값들.

public:
	const DxBoneTransMaintain* FindBone( const TSTRING& strName) const;
	const DxBoneTransMaintain* FindBone_Index( int nIndex ) const;

	void CheckSphere( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName ) const;

	BOOL IsActiveBone() const;

	// Import
	void Import_PureThread( const DxSkeleton* pSkeleton );

	void CleanUp();

public:
	DxSkeletonMaintain();
	~DxSkeletonMaintain();
};