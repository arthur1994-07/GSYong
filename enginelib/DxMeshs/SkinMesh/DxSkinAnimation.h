#pragma once

#include <list>
#include "./SANIMCONINFO.h"
#include "./SAnimationDefine.h"
#include "./DxSkeletonManagerDef.h"
#include "./DxSkeletonManager.h"
#include "./DxAnimationListManager.h"

struct	IDirectXFileData;
struct	DxSkeleton;
struct	DxAnimationList;

// �� �����ӿ��� ������ ���� �ִϸ��̼� ����   Animation information for each bone in one frame
struct DxSkinAnimation : public SANIMCONINFO
{
public:
	DxSkinAnimation();
	virtual ~DxSkinAnimation();


private:
	DxAnimationListResource	m_dxAniListRes;

protected:
	HRESULT LoadAnimationSet( IDirectXFileData* pxofobjCur );
	HRESULT LoadAnimation( IDirectXFileData* pxofobjCur );

public:
	DxSkeletonResource	m_skeletonRes;		// �� ������. bone pointer	// �������̴�. It is a reference value.

	HRESULT CreateAnimationDataFromTool(BOOL bRefesh);

	BOOL IsValidateANI();
	//HRESULT SaveToBinFile();

    DxAnimationListResource GetAniListResource() { return m_dxAniListRes; }

	//!	�⺻ ���� ����. Set basic information
	void LoadSkeleton( BOOL bPreLoad );

	void SetAnimationList( BOOL bThread );

	bool UpdateTime_PureThread(
		float fCurTime,
		float fWeight=1.f,
		BOOL bFirst=TRUE,
		EMBODY emBody=EMBODY_DEFAULT );

    void SetAttachBoneAnimInfo( const SAttachBoneAnimInfo &sAttBoneAnimInfo, EMPIECECHAR emPieceType ) { m_sAttBoneAnimInfo[emPieceType] = sAttBoneAnimInfo; }
	const SAttachBoneAnimInfo* GetAttachBoneAnimInfo() const { return m_sAttBoneAnimInfo; }

	bool UpdateMix_PureThread( float fCurMixTime );

	bool UpdateBoneBip01_PureThread( float fCurTime, float fWeight, BOOL bFirst );

	void UpdateBoneScale( float fWeight, BOOL bFirst, float fTime );

	void ChangeXfile( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* pNameXFILE );

//	void ChangeMap( LPDIRECT3DDEVICEQ pd3dDevice );

	const D3DXVECTOR3* GetStartPos_Bip01();

	void Import_OnlySetting( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* pName );
};

