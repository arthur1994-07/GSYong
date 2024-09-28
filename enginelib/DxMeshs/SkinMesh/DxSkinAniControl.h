
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
	DxSkinAnimation*	m_pDxSkinAni;		// �����Ͽ� ����.

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
	// Note : �������� ���带 ���ش�.
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
	DWORD			m_dwSTime;					//	���� �ð�.
	DWORD			m_dwETime;					//	�� �ð�.
	DWORD			m_UNITTIME;					//	Ű���� �ð�.

	EMANI_MAINTYPE	m_MainType;					//	�� ���� ����.
	EMANI_SUBTYPE	m_SubType;					//	�� ���� ����.

	WORD			m_wDivCount;				//	���ϸ��̼� ���� Ű ����.
	WORD			m_wDivFrame[ACF_DIV];		//	���ϸ��̼� ���� Ű��.
};

struct SANIATTACK
{
	DWORD			m_dwSTime;					//	���� �ð�.
	DWORD			m_dwETime;					//	�� �ð�.
	DWORD			m_UNITTIME;					//	Ű���� �ð�.
	DWORD			m_dwFlags;

	EMANI_MAINTYPE	m_MainType;					//	�� ���� ����.
	EMANI_SUBTYPE	m_SubType;					//	�� ���� ����.

	WORD			m_wDivCount;				//	���ϸ��̼� ���� Ű ����.
	WORD			m_wDivFrame[ACF_DIV];		//	���ϸ��̼� ���� Ű��.

	void Assign ( const SANIMCONINFO *pAnimConInfo )
	{
		GASSERT ( pAnimConInfo && "NULL�� ���Խ��ϴ�." );

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
	BOOL			m_bEndAnim;		// �ִϸ��̼��� ���Ό�� ����.
	int				m_nPrevKeyTime;	// �ִϸ��̼��� ���� �Ǿ����� üũ�Ѵ�.
	PANIMCONTNODE	m_rAnimNode;		// �������ϴ� Animation
	//PANIMCONTNODE	m_rAnimNodeBACKUP;	// ������ ����ߴ� Animation
	PANIMCONTNODE	m_rAnimThreadLoad;	// ���� ����ؾ��ϴ� Animation
	float			m_fTime;
	BOOL			m_bFirstAnimation;	// �ִϸ��̼��� ó�� ���۵Ǵ°�~?

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

	// [AnimationMove] �ִϸ��̼� �������� ĳ���Ϳ� ������ �޴� ������ Ȯ��
	BOOL IsAnimationMove();

	// [AnimationMove] ó�� �ִϸ��̼� �����ΰ�~?
	BOOL IsFirstAnimation();

	// [AnimationMove] ó�� �ִϸ��̼� ������ �۾��Ͽ����� FALSE�� �����Ѵ�.
	void SetFirstAnimationFALSE();

	// [AnimationMove] ó�� �ִϸ��̼� ������ ��ġ�� ��´�.
	const D3DXVECTOR3* GetFirstAnimationPos();

	//////////////////////////////////////////////////////////////////////////





	void CleanUp();

	DxAniControl();
};





struct DxAniControlCORE
{
	DxAniControl	m_sUpBodyAC;			// ��ü �ִϸ��̼�
	DxAniControl	m_sDownBodyAC;			// ��ü �ִϸ��̼�

	DxAniControl	m_sUpBodyAC_Layer;		// ��ü �ִϸ��̼� ( ��Ǻ����� ����. )
	DxAniControl	m_sDownBodyAC_Layer;	// ��ü �ִϸ��̼� ( ��Ǻ����� ����. )

	DxAniControl	m_sMixAC;				// ��ݽ� �ִϸ��̼�

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

	// [AnimationMove] �ִϸ��̼� �������� ĳ���Ϳ� ������ �޴� ������ Ȯ��
	BOOL IsAnimationMove();

	// [AnimationMove] ó�� �ִϸ��̼� �����ΰ�~?
	BOOL IsFirstAnimation();

	// [AnimationMove] ó�� �ִϸ��̼� ������ �۾��Ͽ����� FALSE�� �����Ѵ�.
	void SetFirstAnimationFALSE();

	// [AnimationMove] ó�� �ִϸ��̼� ������ ��ġ�� ��´�.
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

public:

	//////////////////////////////////////////////////////////////////////////
	//					Thread �󿡼� ����Ǵ� ��.
	//		���⿡ �ִ°� Thread �󿡼� ���ǹǷ� ���� �����ؾ� ��.
	//////////////////////////////////////////////////////////////////////////

	// �� DxSkinAniControl �ϳ��� Char �ϳ��� ��Ī�ȴ�. 
	// One Char is matched to each one.


	// Key �� �����Ͽ� �� ���� �������� ���� ������ Matrix ���� ����Ѵ�.
	//In response to the key, a pure Matrix value that is not accumulated in each bone is calculated.


	// ���������� PANIMCONTNODE ���� �����ؼ� �����Ѵ�.
	// Internally, it is transformed by referring to the PANIMCONTNODE value.


	// m_vecReserveFunction �� ����Ͽ� Thread ������ ���. <���� ����ϰ� ������>
	// Improved Thread stability using m_vecReserveFunction. <Let��s make it neat later>


	// ����� Main Thread ���� ������ ������ ���� ����.
	DxAniControlCORE	m_sAniControlCORE;

	// DxSkeletonManager �����Ǵ� static �� Data.
	// Static Data managed by DxSkeletonManager.

	// ������ Matrix ���� ���ȴ�.
	// The accumulated Matrix value is calculated.
    DxSkeletonResource	m_skeletonResThread;

	// Thread �󿡼� m_skeletonResThread �� �ִ� ���� ������ Data
	DxSkeletonMaintain*		m_pSkeletonBackUp;

	// m_rClothColl �� ������ ���̱� ������ �ֿ� �����͸� ������ �ֵ��� �Ѵ�.
	// Thread �󿡼� ���� ������ ��. < PxCloth >
	std::vector<physx::PxClothCollisionSphere>*	m_pvecClothCollSpheresBackUp;

	// ���� ���Ǵ� Matrix��
	// Thread �󿡼� ���� ����ȴ�.
	D3DXMATRIX*			m_pmatWorldBackUp;

	//////////////////////////////////////////////////////////////////////////






	//////////////////////////////////////////////////////////////////////////
	//						�� �ڽ��� ��������
	//		��� Thread ���� �۾� �� ���� Pointer Swap ���� ������ �ִ´�.
	//			����� ���� �ɸ��� �͵� ���ַ� �̷��� �۾���.
	//		���� ���� ����Ǹ� �ȵȴ�. (swap �Ǽ� ��븸 �Ѵ�.)
	//////////////////////////////////////////////////////////////////////////

	// m_skeletonResThread �� �ִ� ���� ������ �� ������ ���ӿ��� ���Ǵ� Data
	// ���� Bone ��ġ ������ �ٲ� ������ Matrix ������ �����Ѵ�.
	// Main Thread ���� ���Ǵ� Data
	// ó�� �����ô� �� ���� ���� ������ ĳ���� �������� �ȵǰ� �ɰ��̴�.
	const DxSkeletonMaintain*		m_pSkeleton;

	// m_rClothColl �� ������ ���̱� ������ �ֿ� �����͸� ������ �ֵ��� �Ѵ�.
	// Main���� ���Ǵ� ��. < PxCloth >
	const std::vector<physx::PxClothCollisionSphere>*	m_pvecClothCollSpheres;

	// ���� ���Ǵ� Matrix ��
	// ���� ������ �� ����.
	const D3DXMATRIX*			m_pmatWorld;

	//////////////////////////////////////////////////////////////////////////







	//////////////////////////////////////////////////////////////////////////
	//			Import�� Main Thread ���� �۾��ϹǷ� ���� ����.
	//			FrameMove_PureThread �� ��� Thread ������ �۾���. 
	//////////////////////////////////////////////////////////////////////////

	boost::scoped_ptr< DxJiggleBone >		m_scpJiggleBone;			// JiggleBone ���� �����Ѵ�.
	boost::scoped_ptr< DxJiggleBoneColl >	m_scpJiggleBoneColl;		// JiggleBone ���� �����Ѵ�.

	//////////////////////////////////////////////////////////////////////////






	//////////////////////////////////////////////////////////////////////////
	//				Main �� ��� Thread ���� �� ����, ���ȴ�.
	//					���Ǵ� ũ�� �ʿ����� ���� �� ����.
	//////////////////////////////////////////////////////////////////////////

	DWORD			m_dwFlags;
	float			m_fWeight;				// ����ġ. ~!! 0 ~ 1 (~!!)
	bool			m_bRandomStartFrame;	// random start frame

	//////////////////////////////////////////////////////////////////////////






	//////////////////////////////////////////////////////////////////////////
	//				SetCharData ���� ������ �ѹ��� �ϴ� ��.
	//		Game������ �׷��� �̾߱�� Edit ������ ������ �̾߱�.
	//				�׷��� ũ�� �����ɲ� ������ �ʴ�.
	//			����� Main �� ��� Thread ���� �� �� �� �ִ�.
	//////////////////////////////////////////////////////////////////////////

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	TSTRING		        m_strSkeleton;
	float				m_fScale;

	VECNODE				m_vecAnim;
	SVEC				m_vecAniType[AN_TYPE_SIZE][AN_SUB_00_SIZE];

	DEQ_BONESCALE		m_deqBoneEditScale;

	// [shhan][2014.12.03] ������ �ؼ� ������ �Ǿ�����. �׷��� shared_ptr �� ����.
	//						�ٸ� Thread ���� ���������鼭 ������ �ּҷ� �۾��� �� ������ �Ǿ���.
	boost::shared_ptr<DxSkinAniControlData>		m_spSkinAniControlData;

	//////////////////////////////////////////////////////////////////////////






	//////////////////////////////////////////////////////////////////////////
	//				Main Thread ������ ����, ����ϵ��� ��.
	//////////////////////////////////////////////////////////////////////////
	DxEffAniPlayer*			m_pEffAniPlayer;
	//////////////////////////////////////////////////////////////////////////
	









	//////////////////////////////////////////////////////////////////////////
	// �Ʒ� �Լ����� ȣ�� ������ ����ϴ� ��.
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
	// [shhan][2014.12.18] GLCharClient ���� FrameMove �� ȣ�� ���� �����鼭 PlayReserveFuction �ҷ����� �ʾ� ClearReserveFuction �� ������ ȣ���ϵ��� ��.
	void ClearReserveFuction();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// m_sAniControlCORE �� �����ִ� ������ �Լ���.
	// �ٷ� ���� �� ��Ű�� ���� �ְ� �����Ű�� ������ ����.
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
	// �� �Լ����� ȣ���� ������ �Ǵ� ��
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


	// ANIMCONTNODE �� �������� M,S ���� ���° Index �� �ִ��� ã�´�.
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
	// ( m_nAnimationMove_PostWork : 2 - ACF_MOVE ����, 1 - AnimationBlend �� ���� m_pQuatPosPREV �� �ʱⰪ���� �������, 0 ���� - ���� ����. )
	int			m_nAnimationMove_PostWork;	// ACF_MOVE ��� ACF_MOVE ���� �� Ư�� �۾��� �ϱ� ���� ���õ�.
	D3DXVECTOR3	m_vAniMovePos;				// �ִϸ��̼� �۵��Ǵ� ��ġ ����ġ
	D3DXVECTOR3 m_vAniMoveOffset;			// �ִϸ��̼� Move�� ���������Ӻ��� �̵��� �Ÿ�
	D3DXVECTOR3 m_vAniMoveOffsetStart;		// AnimationMove ���� Bip01�� Offset

//protected:
//	D3DXMATRIX	m_matAniMoveStartWorld;	// AnimationMove ���� WorldMatrix ( Ghosting ���� ���� )

public:
	BOOL IsAnimationMove();								// �ִϸ��̼� �������� ĳ���Ϳ� ������ �޴� ������ Ȯ��
	const D3DXVECTOR3& GetAnimationMovePos();			// ĳ���Ͱ� �ִϸ��̼����� �󸶸�ŭ ���������� Ȯ��
	//const D3DXMATRIX& GetAnimationMoveStartMatrix();	// AnimationMove �� ���� Matrix

private:
	void AnimationMove_Update();

public:
	D3DXVECTOR3 GetAnimationMoveOffsetFull_EDIT();

	//////////////////////////////////////////////////////////////////////////	





	


	//////////////////////////////////////////////////////////////////////////
	//			Get �� �Ѵ�. return ���� const �� �͵�.
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
	//				 Thread �󿡼� ȣ��Ǵ� �Լ�
	//////////////////////////////////////////////////////////////////////////

	BOOL CalculateBone_PureThread( const D3DXMATRIXA16& matLocalRot, 
									float arrayfKeyJiggleWeight[], 
									DWORD dwPartSize,
									WORDWORD wwMoveParentBoneIDForSafeModeWeaponSlot );		// SafeMode ���� WeaponSlot ��ġ�̵�. Link �� �ƴ� Skin ���� ����

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
												WORDWORD wwMoveParentBoneIDForSafeModeWeaponSlot );		// SafeMode ���� WeaponSlot ��ġ�̵�. Link �� �ƴ� Skin ���� ����

	

	HRESULT CalculateBone_PureThread_HARD ( const D3DXMATRIX &matRot );
	void GetKeyJiggleWeight_PureThread( float afKeyJiggleWeight[], DWORD dwKeyJiggleWeightNUM ) const;
	void UpdateBoneScaleList_PureThread();
	DxSkeletonPart* GetSkeletonPart_PureThread() const;

	//////////////////////////////////////////////////////////////////////////








	//////////////////////////////////////////////////////////////////////////
	//					���� Thread ��� ȣ��Ǵ� �Լ�
	//		��� Thread ������ �� ��찡 �ְ�, SwapBone �� Main Thread ���� �� ��찡 �ִ�.
	//						���̴� �� ���´�.
	//////////////////////////////////////////////////////////////////////////

	BOOL ResetBone_PureThread();

	//////////////////////////////////////////////////////////////////////////







	//////////////////////////////////////////////////////////////////////////
	//	 Main Thread �󿡼��� ȣ��ȴ�. Edit �󿡼� ȣ�� �� ���� �ִ�.
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
	BOOL			LoadSkeletonForEdit( const TCHAR* szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice );
	// ���ο����� ���δ�.
private:
	void			InitAnimation( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinAnimation* pAnimCont );
	void			ClassifyAnimation ();
	void			SetBoneScaleList( const DEQ_BONESCALE& deqBoneScale )	{ m_deqBoneEditScale = deqBoneScale; }

	//////////////////////////////////////////////////////////////////////////







	//////////////////////////////////////////////////////////////////////////
	//			 Edit �󿡼��� ȣ��ȴ�. It is called only on
	//////////////////////////////////////////////////////////////////////////

private:
	void SETCURKEYTIME_FOR_EDIT ( DWORD dwCurKeyTime );

	// CharEdit �󿡼� Bone �� �ε��Ǿ� �ִ� �������� �ƴ����� Ȯ�� �� �� �ִ� �ڵ���.
	// This is a code that can check whether the Bone is loaded or not.
	inline DxSkeleton*	GetSkeletonForEdit() const { return m_skeletonResThread.IsValid() ? m_skeletonResThread.get() : NULL; }

	const DEQ_BONESCALE&	GetBoneScaleList() { return m_deqBoneEditScale; }
	const SBONESCALE*		GetBoneScale( const int nIndex ) const;
	void					AddBoneScale( const SBONESCALE& boneScale );
	bool					EraseBoneScale( const int nIndex );
	DxSkinAnimation*		GETCURANIM_none_const ();
	DxSkinAnimation*		findanicont_none_const( const TCHAR* szAnimNode );
	void					ReAttachBoneForEdit();

//	BOOL					LoadSkeletonForEdit( const TCHAR* szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice );
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

