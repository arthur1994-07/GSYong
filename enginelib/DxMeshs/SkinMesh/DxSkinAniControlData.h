#pragma once

//#include "../../EngineBaseLib/Define/DxAniTypeDef.h"
#include "../../DxTools\DxVertexFVF.h"
#include "./DxSkinAnimationManager.h"
#include "./DxSkeletonManager.h"
#include "./DxBoneScaleDef.h"


class OSStayMotion;
class ASFallDownMotion;
class DxJiggleBone;
class DxJiggleBoneColl;
class DxClothColl;
class DxFootColl;
struct DxSkeleton;
struct DxSkinAnimation;

namespace sc 
{
	class BaseStream;
};

class DxSkinAniControlData
{
public:
	enum 
	{ 
		VERSION = 0x0100,
	};

public:
	typedef std::vector<DxSkinAnimationRes>	VEC_ANIMATION;
	typedef VEC_ANIMATION::iterator			VEC_ANIMATION_ITER;

private:
	TSTRING				m_strSkeleton;
	DxSkeletonResource  m_skeletonRes;
	

	VEC_ANIMATION		m_skinAniResVec;

    DEQ_BONESCALE		m_deqBoneEditScale;

	//////////////////////////////////////////////////////////////////////////
	//  [2/19/2014 shhan] bsf - Bone Setting File �� ���� �Ǵ� �͵�.
	DxJiggleBone*						m_pJiggleBone;			// JiggleBone
	DxJiggleBoneColl*					m_pJiggleBoneColl;		// JiggleBoneColl
	DxClothColl*						m_pClothColl;
	std::tr1::shared_ptr< DxFootColl >	m_smpFootColl;
	//////////////////////////////////////////////////////////////////////////

public:
	const DxJiggleBone* GetJiggleBone() const				{ return m_pJiggleBone; }
	const DxJiggleBoneColl* GetJiggleBoneColl() const		{ return m_pJiggleBoneColl; }
	const DxClothColl* GetClothColl() const					{ return m_pClothColl; }
	std::tr1::shared_ptr< DxFootColl > GetFootColl() const	{ return m_smpFootColl; }

public:
	const VEC_ANIMATION&	GetAniList() const		{ return m_skinAniResVec; }
	const TCHAR*			GetSkeletonName() const	{ return m_strSkeleton.c_str(); }
	BOOL					IsValidSkeleton() const	{ return m_skeletonRes.IsValid() ? TRUE : FALSE; }

	// CreateSkinAni_EDIT ���� �ٸ��� vecAniFile ���� ��ȭ�� �����.
	void					CreateSkinAni( IN OUT VEC_TSTRING& vecAniFile, BOOL bTool_NoRefresh );

	// [shhan][2014.12.08] chf �� �ִ� cfg �� ����ִ� bin ���ϵ��� Thread �� �ϵ� �ƴϵ� ���� �ε��� �ϵ��� �Ѵ�.
	//						���� : �ڽ��� ĳ���Ͱ� �ִϸ��̼��� ��� �˸��� ������ ���ϴ� ���� �ذ�.
	void LoadBIN_HaveToAnimationForMyChar( BOOL bAniThread );
	void LoadBIN_HaveToAnimationForMyChar( EMANI_MAINTYPE emANIMTYPE, EMANI_SUBTYPE emANISTYPE, BOOL bAniThread );

	// Edit
public:
	DxSkinAnimation* FindAnimation( const TCHAR* szNameCFG );
	BOOL LoadSkeleton ( const TCHAR* szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice );
	void CreateSkinAni_EDIT( const VEC_TSTRING& vecAniFile );
	void CleanUp();

	// BoneScale
public:
	const DEQ_BONESCALE&	GetBoneScaleList() const			{ return m_deqBoneEditScale; }
	const SBONESCALE*		GetBoneScale( const int nIndex ) const;
	void					AddBoneScale( const SBONESCALE& boneScale );
	bool					EraseBoneScale( const int nIndex );

	void			SetBoneScaleList( const DEQ_BONESCALE& deqBoneScale ) { m_deqBoneEditScale = deqBoneScale; }
	void			UpdateBoneScaleList();

public:
	BOOL LoadFile_BSF( boost::function<void (const TCHAR*)> pArchive = 0 );
	BOOL SaveFile_BSF( boost::function<void (const TCHAR*)> pArchive = 0 );

private:
	BOOL LOAD_BSF_Ver100( sc::BaseStream &SFile );

	// ���� ���� �ʴ� �Լ�.
public:
	void ErrorLoadJiggleBone( sc::BaseStream &SFile );
	void LoadPrevFootColl( sc::BaseStream &SFile );


public:
	DxSkinAniControlData();
	~DxSkinAniControlData();
};