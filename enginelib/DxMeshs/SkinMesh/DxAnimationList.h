#pragma once

#include "../../../../RanThirdParty/DirectX/Include/dxfile.h"
#include "./DxSkeletonManagerDef.h"
#include "./SAnimationDefine.h"
#include "./SANIMCONINFO.h"

struct	DxSkeleton;
struct	SANIMCONINFO;	// �и��ؾ���.

struct DxAnimationList
{
public:
	enum { VERSION = 0x0104, };
	const static TCHAR szFILETYPE[];

protected:
	DWORD					m_dwRefCount;

	SANILIST				m_listAniROOT;		// Root �ִϸ��̼� ��.		// Ver.102���� �� ��.
	SANILIST				m_listAnimation;	// ��ü ���� �ִϸ��̼� ��.
	SANILIST				m_listAniUPBODY;	// ��ü ���� �ִϸ��̼� ��.	// Ver.101���� �� ��.

	D3DXVECTOR3				m_vStartPos_Bip01;	// Bip01�� ���� ��ġ.

protected:
	HRESULT LoadAnimationSet( IDirectXFileData* pxofobjCur, SANIMCONINFO& sAniContInfo, DxSkeleton* pSkeleton );
	HRESULT LoadAnimation( IDirectXFileData* pxofobjCur, SANIMCONINFO& sAniContInfo, DxSkeleton* pSkeleton );

public:
	HRESULT LoadAnimList_XFile( LPDIRECT3DDEVICEQ pd3dDevice,
								SANIMCONINFO &sAnimInfo,						
								DxSkeleton* pSkeleton );

	HRESULT SaveToFile( const TCHAR* pNameXFile );


	HRESULT LoadAnimList_BIN(void* pBuf,UINT bufSize, const DxSkeleton* pSkeleton );
	//!	���Ͽ��� �б�.
	HRESULT LoadAnimList_BIN( const TSTRING &strNameBIN, const DxSkeleton* pSkeleton, boost::function<void (const TCHAR*)> pArchive = 0 );

	void UpdateTime_PureThread( float fCurTime,
								float fWeight=1.f,
								BOOL bFirst=TRUE,
								EMBODY emBody=EMBODY_DEFAULT );

	void UpdateMix_PureThread( float fCurMixTime );

	void UpdateBoneBip01_PureThread( float fCurTime, float fWeight, BOOL bFirst );

	//	//!	���Ͽ��� �б�. ( �������. )
	//static HRESULT LoadAnimFromFile(
	//	const TSTRING &strFilePath,
	//	DxSkeleton* pSkeleton,
	//	SANILIST &listANIROOT,
	//	SANILIST &listANI,
	//	SANILIST &listANIUPBODY );

	const D3DXVECTOR3* GetStartPos_Bip01()	{ return &m_vStartPos_Bip01; }

    void ReAttachBoneForEdit(DxSkeleton* pSkeleton);

private:
	HRESULT LoadAnimList_BIN( sc::BaseStream& stream, const DxSkeleton* pSkeleton );

	void CleanUp();

public:
	DxAnimationList();
	~DxAnimationList();
};

// m_pAnimationList �� �Ҹ��ڿ��� Release ���� �ʴ´�.
class DxAnimationListDummy
{
public:
	DxAnimationList* m_pAnimationList;

public:
	DxAnimationListDummy()
		: m_pAnimationList(NULL)
	{
	}
};