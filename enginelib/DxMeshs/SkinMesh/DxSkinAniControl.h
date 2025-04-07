
#pragma once

#include <map>
#include <deque>
#include <list>
#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>

#include "../../../SigmaCore/Container/MList.h"
#include "../../DxSound/CharacterSound.h"
#include "../../DxTools/DxVertexFVF.h"

#include "../DxAniKeys.h"
#include "./DxSkeletonManager.h"
#include "./SAnimationDefine.h"
#include "./DxBoneScaleDef.h"
#include "./DxPieceDefine.h"
#include "./SANIMCONINFO.h"
#include "./SAnimation.h"

struct SKINEFFDATA;
struct	DxSkinAnimation;
struct DxAttBoneLinkThreadNeedData;
struct DxVehicleThreadNeedData;
class DxEffAniPlayer;
class DxClothColl;
class DxJiggleBone;
class DxJiggleBoneColl;
class DxSkinCharData;
class DxSkeletonMaintain;
class DxSkinAniControlData;
class DxSkinCharEffAniBase;

namespace physx
{
	struct PxClothCollisionSphere;
};

struct ANIMCONTNODE
{
	DxSkinAnimation*	m_pDxSkinAni;		// 참조하여 쓴다.

	SChaSound			ChaSound;
	D3DXVECTOR3			vPos;

	WORD				wBackFrame;

	ANIMCONTNODE ( DxSkinAnimation*	pDxSkinAni )
		: m_pDxSkinAni(pDxSkinAni)
		, vPos(FLT_MAX,FLT_MAX,FLT_MAX)
		, wBackFrame(0)
	{		
	}
	~ANIMCONTNODE();

public:
	void StartAnim ();

public:
	EMANI_MAINTYPE	GetMType() const;
	EMANI_SUBTYPE	GetSType() const;
	DWORD GetTimeLength();

	//void SetSTime( int nSTime );
	//void SetETime( int nETime );

public:
	BOOL	FrameMove ( float &fCurTime, BOOL bContinue, DWORD dwFlag );

public:
	void Import_OnlySetting( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* pName );

public:
	// Note : 루프돌던 사운드를 꺼준다.
	void	StopLoopSound ();

};
typedef ANIMCONTNODE* PANIMCONTNODE;

enum EMANI_FLAGS
{
	EMANI_ENDFREEZE				= 0x0001,
	EMANI_IGNORELOOP			= 0x0002,
	EMANI_ONLYZEROFRAMESTART	= 0x0004,
};

struct SANIATTACK_100
{
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime;					//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.

	EMANI_MAINTYPE	m_MainType;					//	주 에니 종류.
	EMANI_SUBTYPE	m_SubType;					//	부 에니 종류.

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들.
};

struct SANIATTACK
{
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime;					//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.
	DWORD			m_dwFlags;

	EMANI_MAINTYPE	m_MainType;					//	주 에니 종류.
	EMANI_SUBTYPE	m_SubType;					//	부 에니 종류.

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들.

	void Assign ( const SANIMCONINFO *pAnimConInfo )
	{
		GASSERT ( pAnimConInfo && "NULL이 들어왔습니다." );

		m_dwSTime = pAnimConInfo->m_dwSTime;
		m_dwETime = pAnimConInfo->m_dwETime;
		m_UNITTIME = pAnimConInfo->m_UNITTIME;
		m_dwFlags = pAnimConInfo->m_dwFlag;

		m_MainType = pAnimConInfo->m_MainType;
		m_SubType = pAnimConInfo->m_SubType;

		m_wDivCount = pAnimConInfo->m_wStrikeCount;
		for ( int i = 0; i < ACF_DIV; ++i )
		{
			m_wDivFrame[i] = (WORD)pAnimConInfo->m_sStrikeEff[i].m_dwFrame;
		}
	}

	void Assign ( const SANIATTACK_100 &value )
	{
		m_dwSTime = value.m_dwSTime;
		m_dwETime = value.m_dwETime;
		m_UNITTIME = value.m_UNITTIME;

		m_MainType = value.m_MainType;
		m_SubType = value.m_SubType;

		m_wDivCount = value.m_wDivCount;
		memcpy ( m_wDivFrame, value.m_wDivFrame, sizeof(WORD)*ACF_DIV );
	}

	SANIATTACK () :
		m_dwSTime(0),
		m_dwETime(0),
		m_UNITTIME(INT_MAX),
		m_dwFlags(NULL),

		m_MainType(AN_GUARD_N),
		m_SubType(AN_SUB_NONE),
		m_wDivCount(0)
	{
	}

	bool IsLOOP () const		{ return (m_dwFlags&ACF_LOOP)!=NULL; }
};

typedef std::vector<SANIATTACK>	VECANIATTACK;





struct DxAniControl
{
	BOOL			m_bEndAnim;		// 애니메이션의 종료예정 유뮤.
	int				m_nPrevKeyTime;	// 애니메이션이 시작 되었는지 체크한다.
	PANIMCONTNODE	m_rAnimNode;		// 현재사용하는 Animation
	//PANIMCONTNODE	m_rAnimNodeBACKUP;	// 이전에 사용했던 Animation
	PANIMCONTNODE	m_rAnimThreadLoad;	// 실제 사용해야하는 Animation
	float			m_fTime;
	BOOL			m_bFirstAnimation;	// 애니메이션이 처음 동작되는가~?

	void FrameMove( float fElapsedTime, BOOL bContinue, DWORD dwFlags, DxAniControl& sAniControlBACKUP );
	void FrameMoveLayer( float fElapsedTime, BOOL bContinue, DWORD dwFlags );
	void FrameMoveMIX( float fElapsedTime );
	void UpdateBoneScale( float fWeight, BOOL bFirst );

	void ToStartTime( float fCurTime );
	void ToEndTime( float fCurTime );
	BOOL SetAnimation( ANIMCONTNODE* pNode, DxAniControl& sAniControlBACKUP, BOOL bSameAniRefresh = FALSE );
	void SetMixAni( ANIMCONTNODE* pNode );

	void StopLoopSound();

	void Validate();





	//////////////////////////////////////////////////////////////////////////
	//						Pure Thread
	BOOL UpdateTime_PureThread( float fWeight=1.f, BOOL bFirst=TRUE, EMBODY emBody=EMBODY_DEFAULT );
	void UpdateMix_PureThread();
	BOOL UpdateBoneBip01_PureThread( float fWeight, BOOL bFirst );

	// 0 KeyFrame <-> 1 JiggleBone
	void GetKeyJiggleWeight_PureThread( float afKeyJiggleWeight[], DWORD dwKeyJiggleWeightNUM, DWORD dwKey ) const;
	//////////////////////////////////////////////////////////////////////////





	//////////////////////////////////////////////////////////////////////////
	//						AnimationMove
	//////////////////////////////////////////////////////////////////////////

	// [AnimationMove] 애니메이션 움직임이 캐릭터에 영향을 받는 것인지 확인
	BOOL IsAnimationMove();

	// [AnimationMove] 처음 애니메이션 시작인가~?
	BOOL IsFirstAnimation();

	// [AnimationMove] 처음 애니메이션 동작을 작업하였으면 FALSE로 셋팅한다.
	void SetFirstAnimationFALSE();

	// [AnimationMove] 처음 애니메이션 동작의 위치를 얻는다.
	const D3DXVECTOR3* GetFirstAnimationPos();

	//////////////////////////////////////////////////////////////////////////





	void CleanUp();

	DxAniControl();
};





struct DxAniControlCORE
{
	DxAniControl	m_sUpBodyAC;			// 상체 애니메이션
	DxAniControl	m_sDownBodyAC;			// 하체 애니메이션

	DxAniControl	m_sUpBodyAC_Layer;		// 상체 애니메이션 ( 모션블렌딩을 위함. )
	DxAniControl	m_sDownBodyAC_Layer;	// 하체 애니메이션 ( 모션블렌딩을 위함. )

	DxAniControl	m_sMixAC;				// 충격시 애니메이션

	void FrameMove( float fElapsedTime, BOOL bContinue, DWORD dwFlags, float fWeight );

	BOOL SetAnimation( ANIMCONTNODE* pNode, bool bResetSkillAni = FALSE );
	BOOL SetAnimationEDIT( ANIMCONTNODE* pNode, BOOL bSameAniRefresh );
	void SetMixAni( ANIMCONTNODE* pNode );

	void StopLoopSound();





	//////////////////////////////////////////////////////////////////////////
	//						Pure Thread
	void UpdateTime_PureThread( float fWeight );
	void UpdateMix_PureThread();
	void UpdateBoneBip01_PureThread( float fWeight );

	// 0 KeyFrame <-> 1 JiggleBone
	void GetKeyJiggleWeight_PureThread( float afKeyJiggleWeight[], DWORD dwKeyJiggleWeightNUM, DWORD dwKey ) const;
	//////////////////////////////////////////////////////////////////////////





	//////////////////////////////////////////////////////////////////////////
	//							AnimationMove
	//////////////////////////////////////////////////////////////////////////

	// [AnimationMove] 애니메이션 움직임이 캐릭터에 영향을 받는 것인지 확인
	BOOL IsAnimationMove();

	// [AnimationMove] 처음 애니메이션 시작인가~?
	BOOL IsFirstAnimation();

	// [AnimationMove] 처음 애니메이션 동작을 작업하였으면 FALSE로 셋팅한다.
	void SetFirstAnimationFALSE();

	// [AnimationMove] 처음 애니메이션 동작의 위치를 얻는다.
	const D3DXVECTOR3* GetFirstAnimationPos();

	//////////////////////////////////////////////////////////////////////////





	void CleanUp();
};





class DxSkinAniControl
{
public:
	enum { ANI_MAX = 6, };
	struct SVEC
	{
		DWORD			m_dwSize;
		PANIMCONTNODE	m_pANI[ANI_MAX];

		SVEC()
		{
			clear();
		}

		__forceinline void clear()
		{
			m_dwSize = 0;
			memset ( m_pANI, 0, sizeof(PANIMCONTNODE)*ANI_MAX );
		}

		void insert ( PANIMCONTNODE pValue );

		DWORD size () const	{ return m_dwSize; }

		const PANIMCONTNODE get ( DWORD i ) const
		{
			return m_pANI[i];
		}
	};

	typedef std::vector<ANIMCONTNODE*>	VECNODE;
	typedef VECNODE::iterator			VECNODE_ITER;
	typedef VECNODE::const_iterator		VECNODE_CITER;

protected:

	//////////////////////////////////////////////////////////////////////////
	//					Thread 상에서 변경되는 것.
	//		여기에 있는건 Thread 상에서 사용되므로 사용시 주의해야 함.
	//////////////////////////////////////////////////////////////////////////

	// 본 DxSkinAniControl 하나에 Char 하나가 매칭된다.
	// Key 에 반응하여 각 본에 누적되지 않은 순수한 Matrix 값을 계산한다.
	// 내부적으로 PANIMCONTNODE 값을 참조해서 변형한다.
	// m_vecReserveFunction 를 사용하여 Thread 안정성 향상. <추후 깔끔하게 만들자>
	// 사운드는 Main Thread 에서 돌리기 때문에 문제 없다.
	DxAniControlCORE	m_sAniControlCORE;

	// DxSkeletonManager 관리되는 static 한 Data.
	// 누적된 Matrix 값이 계산된다.
    DxSkeletonResource	m_skeletonResThread;

	// Thread 상에서 m_skeletonResThread 에 있는 값을 복사한 Data
	DxSkeletonMaintain*		m_pSkeletonBackUp;

	// m_rClothColl 는 참조된 값이기 때문에 주요 데이터를 가지고 있도록 한다.
	// Thread 상에서 값을 얻어오는 것. < PxCloth >
	std::vector<physx::PxClothCollisionSphere>*	m_pvecClothCollSpheresBackUp;

	// 현재 계산되는 Matrix값
	// Thread 상에서 값이 변경된다.
	D3DXMATRIX*			m_pmatWorldBackUp;

	//////////////////////////////////////////////////////////////////////////






	//////////////////////////////////////////////////////////////////////////
	//						내 자신이 오리지널
	//		계산 Thread 에서 작업 된 것을 Pointer Swap 으로 가지고 있는다.
	//			계산이 오래 걸리는 것들 위주로 이렇게 작업됨.
	//		여기 값은 변경되면 안된다. (swap 되서 사용만 한다.)
	//////////////////////////////////////////////////////////////////////////

	// m_skeletonResThread 에 있는 값을 복사한 후 실제로 게임에서 사용되는 Data
	// 다음 Bone 위치 정보가 바뀔 때까지 Matrix 정보를 유지한다.
	// Main Thread 에서 사용되는 Data
	// 처음 렌더시는 이 값이 없기 때문에 캐릭터 렌더링은 안되게 될것이다.
	const DxSkeletonMaintain*		m_pSkeleton;

	// m_rClothColl 는 참조된 값이기 때문에 주요 데이터를 가지고 있도록 한다.
	// Main에서 사용되는 것. < PxCloth >
	const std::vector<physx::PxClothCollisionSphere>*	m_pvecClothCollSpheres;

	// 현재 사용되는 Matrix 값
	// 값을 변경할 수 없다.
	const D3DXMATRIX*			m_pmatWorld;

	//////////////////////////////////////////////////////////////////////////







	//////////////////////////////////////////////////////////////////////////
	//			Import는 Main Thread 에서 작업하므로 문제 없음.
	//			FrameMove_PureThread 는 계산 Thread 에서만 작업됨. 
	//////////////////////////////////////////////////////////////////////////

	boost::scoped_ptr< DxJiggleBone >		m_scpJiggleBone;			// JiggleBone 깊이 복사한다.
	boost::scoped_ptr< DxJiggleBoneColl >	m_scpJiggleBoneColl;		// JiggleBone 깊이 복사한다.

	//////////////////////////////////////////////////////////////////////////






	//////////////////////////////////////////////////////////////////////////
	//				Main 과 계산 Thread 에서 다 셋팅, 사용된다.
	//					주의는 크게 필요하지 않을 것 같음.
	//////////////////////////////////////////////////////////////////////////

	DWORD			m_dwFlags;
	float			m_fWeight;				// 가중치. ~!! 0 ~ 1 (~!!)
	bool			m_bRandomStartFrame;	// random start frame

	//////////////////////////////////////////////////////////////////////////






	//////////////////////////////////////////////////////////////////////////
	//				SetCharData 에서 셋팅은 한번만 하는 것.
	//		Game에서만 그렇단 이야기고 Edit 에서는 별개의 이야기.
	//				그래서 크게 문제될꺼 같지는 않다.
	//			사용은 Main 과 계산 Thread 양쪽 다 될 수 있다.
	//////////////////////////////////////////////////////////////////////////

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	TSTRING		        m_strSkeleton;
	float				m_fScale;

	VECNODE				m_vecAnim;
	SVEC				m_vecAniType[AN_TYPE_SIZE][AN_SUB_00_SIZE];

	DEQ_BONESCALE		m_deqBoneEditScale;

	// [shhan][2014.12.03] 참조를 해서 문제가 되었었다. 그래서 shared_ptr 로 변경.
	//						다른 Thread 에서 지워버리면서 쓰레기 주소로 작업을 해 문제가 되었다.
	boost::shared_ptr<DxSkinAniControlData>		m_spSkinAniControlData;

	//////////////////////////////////////////////////////////////////////////






	//////////////////////////////////////////////////////////////////////////
	//				Main Thread 에서만 셋팅, 사용하도록 함.
	//////////////////////////////////////////////////////////////////////////
	DxEffAniPlayer*			m_pEffAniPlayer;
	//////////////////////////////////////////////////////////////////////////
	









	//////////////////////////////////////////////////////////////////////////
	// 아래 함수들의 호출 정보를 기록하는 곳.
	//////////////////////////////////////////////////////////////////////////
public:
	struct MAIN
	{
		BYTE	m_cIndex;
		BYTE	m_cMType;
		BYTE	m_cSType;
		BYTE	m_cAniIndex;

		D3DXVECTOR3 m_vPos;
		DWORD	m_dwFlags;
		float	m_fElapsedTime;

		MAIN( BYTE cIndex )
			: m_cIndex(cIndex)
			, m_cMType(0)
			, m_cSType(0)
			, m_cAniIndex(0)
			, m_vPos(D3DXVECTOR3(0.f,0.f,0.f))
			, m_dwFlags(0L)
			, m_fElapsedTime(0.f)
		{}

		//MAIN( BYTE cIndex, BYTE	cMType, BYTE cSType, BYTE cAniIndex, const D3DXVECTOR3* pPos, DWORD dwFlags, float fElapsedTime )
		//	: m_cIndex(cIndex)
		//	, m_cMType(cMType)
		//	, m_cSType(cSType)
		//	, m_cAniIndex(cAniIndex)
		//	, m_vPos(pPos ? *pPos : D3DXVECTOR3(0.f,0.f,0.f))
		//	, m_dwFlags(dwFlags)
		//	, m_fElapsedTime(fElapsedTime)
		//{}
	};

private:
	std::vector<MAIN*>	m_vecReserveFunction;

	void PlayReserveFuction();

public:
	// [shhan][2014.12.18] GLCharClient 에서 FrameMove 를 호출 하지 않으면서 PlayReserveFuction 불려지지 않아 ClearReserveFuction 를 강제로 호출하도록 함.
	void ClearReserveFuction();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// m_sAniControlCORE 에 영향주는 위험한 함수들.
	// 바로 적용 안 시키고 텀을 주고 적용시키는 것으로 변경.
	//////////////////////////////////////////////////////////////////////////
public:
	void TOSTARTTIME();
	void TOENDTIME();
	void DOSHOCKMIX();
	void SetPosition( const D3DXVECTOR3& vPos );
	void DEFAULTANI();
	void SELECTANI( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE, DWORD dwFlags=NULL, DWORD dwSelect=0 );
	void FrameMove_AniTime( float fElapsedTime, BOOL bContinue=TRUE );

	void SELECTANI( const TCHAR* szAnim, DWORD dwFlags=NULL );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 위 함수들의 호출이 릴레이 되는 것
	//////////////////////////////////////////////////////////////////////////
private:
	void re_TOSTARTTIME ();
	void re_TOENDTIME ();
	void re_DOSHOCKMIX ();
	void re_SetPosition ( const D3DXVECTOR3& vPos )
	{	
		if ( m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode )	
			m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->vPos = vPos; 
	}
	void re_DEFAULTANI ();
	void re_SELECTANI ( const TCHAR* szAnim, DWORD dwFlags );
	void re_SELECTANI ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwFlags, DWORD dwSelect );
	void re_FrameMove_AniTime ( float fElapsedTime, BOOL bContinue );
	//////////////////////////////////////////////////////////////////////////


	// ANIMCONTNODE 를 바탕으로 M,S 기준 몇번째 Index 에 있는지 찾는다.
	DWORD GetAniIndex( ANIMCONTNODE* pNode ) const;




public:
	static bool				m_bBIGHEAD;
	static bool				m_bBIGHAND;
	static float			m_fBIGHEAD;
	static float			m_fBIGHAND;








	//////////////////////////////////////////////////////////////////////////
	//						AnimationMove
	//////////////////////////////////////////////////////////////////////////

private:
	// ( m_nAnimationMove_PostWork : 2 - ACF_MOVE 상태, 1 - AnimationBlend 로 인해 m_pQuatPosPREV 를 초기값으로 해줘야함, 0 이하 - 문제 없다. )
	int			m_nAnimationMove_PostWork;	// ACF_MOVE 라면 ACF_MOVE 종료 후 특정 작업을 하기 위해 셋팅됨.
	D3DXVECTOR3	m_vAniMovePos;				// 애니메이션 작동되는 위치 현위치
	D3DXVECTOR3 m_vAniMoveOffset;			// 애니메이션 Move로 이전프레임보다 이동된 거리
	D3DXVECTOR3 m_vAniMoveOffsetStart;		// AnimationMove 시작 Bip01의 Offset

//protected:
//	D3DXMATRIX	m_matAniMoveStartWorld;	// AnimationMove 시작 WorldMatrix ( Ghosting 에서 사용됨 )

public:
	BOOL IsAnimationMove();								// 애니메이션 움직임이 캐릭터에 영향을 받는 것인지 확인
	const D3DXVECTOR3& GetAnimationMovePos();			// 캐릭터가 애니메이션으로 얼마만큼 움직였는지 확인
	//const D3DXMATRIX& GetAnimationMoveStartMatrix();	// AnimationMove 의 시작 Matrix

private:
	void AnimationMove_Update();

public:
	D3DXVECTOR3 GetAnimationMoveOffsetFull_EDIT();

	//////////////////////////////////////////////////////////////////////////	





	


	//////////////////////////////////////////////////////////////////////////
	//			Get 만 한다. return 값도 const 인 것들.
	//////////////////////////////////////////////////////////////////////////

public:
	BOOL ISENDANIM () const;
	BOOL ISENDANIM( EMANICONINFO emBody ) const;
	BOOL ISSHOCKMIX () const						{ return (m_sAniControlCORE.m_sMixAC.m_rAnimNode)?TRUE:FALSE; }
	bool IsAni(EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE) const;
	BOOL IsCurAniConInfo(const EMANICONINFO emAniConInfo) const;
	BOOL IsValid_Skeleton() const;
	BOOL IsValid_Skeleton_DefaultAni() const;

	DWORD GETUNITTIME () const;
	float GETSTARTTIME () const;
	float GETENDTIME () const;
	float GETCURTIME () const					{ return m_sAniControlCORE.m_sDownBodyAC.m_fTime; }
	float GetScale() const						{ return m_fScale; }

	EMANI_MAINTYPE	GETCURMTYPE () const;
	EMANI_SUBTYPE	GETCURSTYPE () const;

	float			GETCURANITIME () const;
	DWORD			GETCURKEYTIME () const;

	int				GetPrevKeyFrame() const		{ return m_sAniControlCORE.m_sDownBodyAC.m_nPrevKeyTime; }	

	DWORD			GETANIAMOUNT () const;
	DWORD			GETANIAMOUNT ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType ) const;

	const PANIMCONTNODE	GETANI ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE ) const;

	const VECNODE&		GetAniList () const			{ return m_vecAnim; }
	const TCHAR*		GetSkeletonName () const	{ return m_strSkeleton.c_str(); }

	const DxSkinAnimation*	GETCURANIM() const;
	const PANIMCONTNODE		GETCURANIMNODE() const;

	const ANIMCONTNODE*		findanicontnode(const std::string& AnimNode) const;
	const DxSkinAnimation*	findanicont( const TCHAR* szAnimNode ) const;

	const DxSkeletonMaintain*	GetSkeletonMaintain() const			{ return m_pSkeleton; }
	const DxSkeletonMaintain*	GetSkeletonMaintainBackUp() const	{ return m_pSkeletonBackUp; }

	const DxClothColl*									GetClothColl() const;
	const std::vector<physx::PxClothCollisionSphere>*	GetClothCollSphere() const	{ return m_pvecClothCollSpheres; }

	const D3DXMATRIX& GetWorld()			{ return *m_pmatWorld; }

	BOOL IsAnimName( const TCHAR* szAnim ) const;

	//DxSkinAnimation*	GETCURANIM_UPBODY();

	//////////////////////////////////////////////////////////////////////////








	//////////////////////////////////////////////////////////////////////////
	//							Rendering
	//////////////////////////////////////////////////////////////////////////

	void Render_EffAniPlayer( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharEffAniBase* pSkinChar, float fAlpha );
	//void Render_EffAniPlayer( const LPDIRECT3DDEVICEQ pd3dDevice, DxAttBone* pAttBone, const D3DXMATRIXA16& matLocalRot, float fAlpha );
	//void Render_EffAniPlayer( const LPDIRECT3DDEVICEQ pd3dDevice, DxVehicle* pVehicle, const D3DXMATRIXA16& matLocalRot, float fAlpha );
	void RenderBone( LPDIRECT3DDEVICEQ pd3dDevice );

	//////////////////////////////////////////////////////////////////////////








	//////////////////////////////////////////////////////////////////////////
	//				 Thread 상에서 호출되는 함수
	//////////////////////////////////////////////////////////////////////////

	BOOL CalculateBone_PureThread( const D3DXMATRIXA16& matLocalRot, 
									float arrayfKeyJiggleWeight[], 
									DWORD dwPartSize,
									WORDWORD wwMoveParentBoneIDForSafeModeWeaponSlot );		// SafeMode 에서 WeaponSlot 위치이동. Link 가 아닌 Skin 만을 위함

	BOOL CalculateBoneVehicle_PureThread( const std::tr1::shared_ptr<DxVehicleThreadNeedData>& spVehicleThreadNeedData, 
											const D3DXMATRIXA16& matLocalRot,
											float arrayfKeyJiggleWeight[], 
											DWORD dwPartSize );

	BOOL CalculateBoneAttBone_PureThread( D3DXMATRIX& matLocalRotOUT,
											const std::tr1::shared_ptr<DxSkinAniControl>& spLinkSkinAniControl, 
											const std::tr1::shared_ptr<DxAttBoneLinkThreadNeedData>& spThreadNeedData, 
											float fKeyJiggleWeight,
											float fScale );

	BOOL CalculateBoneCharOnVehicle_PureThread( D3DXMATRIX& matLocalRotOUT,
												float arrayfKeyJiggleWeight[], 
												DWORD dwPartSize,
												const std::tr1::shared_ptr<DxVehicleThreadNeedData>& spVehicleThreadNeedData, 
												DWORD dwPassengerID,
												WORDWORD wwMoveParentBoneIDForSafeModeWeaponSlot );		// SafeMode 에서 WeaponSlot 위치이동. Link 가 아닌 Skin 만을 위함

	

	HRESULT CalculateBone_PureThread_HARD ( const D3DXMATRIX &matRot );
	void GetKeyJiggleWeight_PureThread( float afKeyJiggleWeight[], DWORD dwKeyJiggleWeightNUM ) const;
	void UpdateBoneScaleList_PureThread();
	DxSkeletonPart* GetSkeletonPart_PureThread() const;

	//////////////////////////////////////////////////////////////////////////








	//////////////////////////////////////////////////////////////////////////
	//					단일 Thread 상과 호출되는 함수
	//		계산 Thread 에서만 될 경우가 있고, SwapBone 의 Main Thread 에서 될 경우가 있다.
	//						같이는 못 들어온다.
	//////////////////////////////////////////////////////////////////////////

	BOOL ResetBone_PureThread();

	//////////////////////////////////////////////////////////////////////////







	//////////////////////////////////////////////////////////////////////////
	//	 Main Thread 상에서만 호출된다. Edit 상에서 호출 될 수도 있다.
	//////////////////////////////////////////////////////////////////////////

public:
	HRESULT			RestoreDeviceObjects ();
	HRESULT			InvalidateDeviceObjects ();
	HRESULT			DeleteDeviceObjects ();
	void			SetCharData ( boost::shared_ptr<DxSkinAniControlData> spCharData, LPDIRECT3DDEVICEQ pd3dDevice, float fScale, const TCHAR* rFileName, BOOL bMaintainAniminfo = FALSE );
	void			FrameMove_EffAniPlayer( DxSkinCharEffAniBase* pSkinCharBase, SKINEFFDATA& sSkinEffData, float fTime, float fElapsedTime, BOOL bFreeze );
	void			SetPrevKeyFrame()										{ m_sAniControlCORE.m_sDownBodyAC.m_nPrevKeyTime = GETCURKEYTIME(); }
	ANIMCONTNODE*	findanicontnode_none_const(const std::string& AnimNode);
	void			SwapBone( const D3DXMATRIX& matWorld );
	void			StopLoopSound();
	void			SETCURTIME ( float fCurTime )		{ m_sAniControlCORE.m_sDownBodyAC.m_fTime = fCurTime; }
	void			SELECTANI_EDIT( const TCHAR* szAnim, BOOL bSameAniRefresh = TRUE, DWORD dwFlag = 0 );
	void			SETCURTIME_ALL_FOR_EDIT( float fCurTime );
	void			ClearAll();
	PANIMCONTNODE	GETANI_none_const( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwCount ) const;
	//void SETANIUPBODY( const TCHAR* szAnim );
	//void SETANIUPBODY( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType );

	// 내부에서만 쓰인다.
private:
	void			InitAnimation( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinAnimation* pAnimCont );
	void			ClassifyAnimation ();
	void			SetBoneScaleList( const DEQ_BONESCALE& deqBoneScale )	{ m_deqBoneEditScale = deqBoneScale; }

	//////////////////////////////////////////////////////////////////////////







	//////////////////////////////////////////////////////////////////////////
	//			 Edit 상에서만 호출된다.
	//////////////////////////////////////////////////////////////////////////

private:
	void SETCURKEYTIME_FOR_EDIT ( DWORD dwCurKeyTime );

	// CharEdit 상에서 Bone 이 로딩되어 있는 상태인지 아닌지도 확인 할 수 있는 코드임.
	inline DxSkeleton*	GetSkeletonForEdit() const { return m_skeletonResThread.IsValid() ? m_skeletonResThread.get() : NULL; }

	const DEQ_BONESCALE&	GetBoneScaleList() { return m_deqBoneEditScale; }
	const SBONESCALE*		GetBoneScale( const int nIndex ) const;
	void					AddBoneScale( const SBONESCALE& boneScale );
	bool					EraseBoneScale( const int nIndex );
	DxSkinAnimation*		GETCURANIM_none_const ();
	DxSkinAnimation*		findanicont_none_const( const TCHAR* szAnimNode );
	void					ReAttachBoneForEdit();

	BOOL					LoadSkeletonForEdit( const TCHAR* szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL					LoadAnimationForEdit(const std::string& AnimName, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL					ReleaseAnimationForEdit(const std::string& AnimName);

	friend class CFramePage;
	friend class CAttBonePage;
	friend class CAnimPage;
	friend class CAnimSetPage;
	friend class CBoardEditDlg;
	friend class CBoneEdit;
	friend class CCharEditView;
	friend class CEffAniPage;
	friend class CEffAniSinglePage;
	friend class CJiggleBonePage;
	friend class CJiggleBoneCollPage;
	friend class CClothCollPage;
	friend class CVehiclePage;
	friend class CCharPage;
	friend class CPartSkeletonFileNotifier;

	//////////////////////////////////////////////////////////////////////////


	

public:
	DxSkinAniControl ();
	virtual ~DxSkinAniControl ();
};

