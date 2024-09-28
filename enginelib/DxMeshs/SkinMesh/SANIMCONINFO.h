#pragma once

#include <list>
#include <boost/scoped_ptr.hpp>

//#include "../../EngineBaseLib/Define/DxAniTypeDef.h"

#include "../../DxSound/CharacterSound.h"

#include "./SAnimationDefine.h"
#include "./DxAniBoneScale.h"
#include "./DxPieceDefine.h"

class DxAniScale;
class DxEffAniData;
class DxAniJiggleBlend;

struct SANIMSTRIKE
{
	EMPIECECHAR		m_emPiece;
	EMSTRIKE_EFF	m_emEffect;
	DWORD			m_dwFrame;
	//BOOL			m_bRigidity;

	SANIMSTRIKE()
		: m_emPiece(PIECE_RHAND)
		, m_emEffect(EMSF_TARGET)
		, m_dwFrame(0)
	//	, m_bRigidity(TRUE)
	{
	}
};

struct SAttachBoneAnimBasicInfo
{
	int		       nStartAnim;
	EMANI_MAINTYPE emAttAniType;

	SAttachBoneAnimBasicInfo()
		: nStartAnim(0)
		, emAttAniType(AN_ATTBONE_STAY)
	{
	}

	void Init()
	{
		nStartAnim		= 0;
		emAttAniType	= AN_ATTBONE_STAY;
	}
};

typedef std::vector<SAttachBoneAnimBasicInfo> VECATTBASE;
typedef VECATTBASE::iterator				  VECATTBASE_ITER;

struct SAttachBoneAnimInfo
{
	bool					 bUseStrikeAni;
	SAttachBoneAnimBasicInfo sBasicAni;

	VECATTBASE				 vecStrikeAni;

	SAttachBoneAnimInfo()
		: bUseStrikeAni(FALSE)
	{
		sBasicAni.Init();
		vecStrikeAni.clear();
	}

	void Init()
	{
		sBasicAni.Init();
		vecStrikeAni.clear();

		bUseStrikeAni = FALSE;
	}
	SAttachBoneAnimInfo& operator = ( const SAttachBoneAnimInfo &value );
	
};

enum SANIMCONINFO_MSG
{
	SANIMCONINFO_MSG_OK						= 0,
	SANIMCONINFO_MSG_FALSE					= 1,
	SANIMCONINFO_MSG_UNKNOWN_FILE_VERSION	= 2,
};

struct SANIMCONINFO_101
{
	TCHAR			m_szName[ACF_SZNAME];		//	이름.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	뼈다귀 이름.

	DWORD			m_dwFlag;					//	기타 속성.
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime, m_dwETimeOrig;	//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들..

	SANIMCONINFO_101()
	{
		memset( m_szName, 0, sizeof(TCHAR)*ACF_SZNAME );
		memset( m_szSkeletion, 0, sizeof(TCHAR)*ACF_SZNAME );
	}
};

struct SANIMCONINFO_102
{
	TCHAR			m_szName[ACF_SZNAME];		//	이름.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	뼈다귀 이름.

	DWORD			m_dwFlag;					//	기타 속성.
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime, m_dwETimeOrig;	//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들..

	SChaSoundData_102	m_ChaSoundData;				//	사운드 데이타

	SANIMCONINFO_102()
	{
		memset( m_szName, 0, sizeof(TCHAR)*ACF_SZNAME );
		memset( m_szSkeletion, 0, sizeof(TCHAR)*ACF_SZNAME );
	}
};

struct SANIMCONINFO_103
{
	TCHAR			m_szName[ACF_SZNAME];
	TCHAR			m_szSkeletion[ACF_SZNAME];

	DWORD			m_dwFlag;
	DWORD			m_dwSTime;
	DWORD			m_dwETime, m_dwETimeOrig;
	DWORD			m_UNITTIME;

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;
	WORD			m_wDivFrame[ACF_DIV];

	SChaSoundData_103	m_ChaSoundData;

	SANIMCONINFO_103()
	{
		memset( m_szName, 0, sizeof(TCHAR)*ACF_SZNAME );
		memset( m_szSkeletion, 0, sizeof(TCHAR)*ACF_SZNAME );
	}
};

struct SANIMCONINFO_104
{
	TCHAR			m_szName[ACF_SZNAME];		//	이름.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	뼈다귀 이름.

	DWORD			m_dwFlag;					//	기타 속성.
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime, m_dwETimeOrig;	//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	타격 이팩트 시점들.

	SChaSoundData_103	m_ChaSoundData;				//	사운드 데이타

	SANIMCONINFO_104()
	{
		memset( m_szName, 0, sizeof(TCHAR)*ACF_SZNAME );
		memset( m_szSkeletion, 0, sizeof(TCHAR)*ACF_SZNAME );
	}
};

struct SANIMCONINFO_105
{
	typedef std::list<DxEffAniData*>				EFFANILIST;			
	typedef std::list<DxEffAniData*>::iterator		EFFANILIST_ITER;

	TCHAR			m_szName[ACF_SZNAME];		//	이름.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	뼈다귀 이름.

	DWORD			m_dwFlag;					//	기타 속성.
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime, m_dwETimeOrig;	//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	타격 이팩트 시점들.

	SChaSoundData_103	m_ChaSoundData;				//	사운드 데이타

	EFFANILIST		m_listEffAni;				// 모션 효과 !

	SANIMCONINFO_105()
	{
		memset( m_szName, 0, sizeof(TCHAR)*ACF_SZNAME );
		memset( m_szSkeletion, 0, sizeof(TCHAR)*ACF_SZNAME );
	}
};

struct SANIMCONINFO_106
{
	typedef std::list<DxEffAniData*>				EFFANILIST;			
	typedef std::list<DxEffAniData*>::iterator		EFFANILIST_ITER;

	TCHAR			m_szName[ACF_SZNAME];		//	이름.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	뼈다귀 이름.

	DWORD			m_dwFlag;					//	기타 속성.
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime, m_dwETimeOrig;	//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	타격 이팩트 시점들.

	SChaSoundData	m_ChaSoundData;				//	사운드 데이타

	EFFANILIST		m_listEffAni;				// 모션 효과 !

	SANIMCONINFO_106 () :
		m_dwFlag(ACF_LOOP|ACF_NEWINFO),
		m_dwSTime(0),
		m_dwETime(0),
		m_dwETimeOrig(0),
		m_UNITTIME(0),
		m_MainType(AN_GUARD_N),
		m_SubType(AN_SUB_NONE),

		m_wDivCount(0),
		m_wStrikeCount(0)
	{
		memset( m_szName, 0, sizeof(TCHAR)*ACF_SZNAME );
		memset( m_szSkeletion, 0, sizeof(TCHAR)*ACF_SZNAME );

		memset ( m_wDivFrame, 0, sizeof(WORD)*ACF_DIV );
	}
};

struct SANIMCONINFO_PREV				// 110 까지..
{
	typedef std::list<DxEffAniData*>				EFFANILIST;			
	typedef std::list<DxEffAniData*>::iterator		EFFANILIST_ITER;

    std::string		m_Name;	                    //! 이름.
	std::string		m_SkeletionName;	        //! 뼈다귀 이름.

	DWORD			m_dwFlag;					//! 기타 속성.
	DWORD			m_dwSTime;					//! 시작 시간.
	DWORD			m_dwETime;					//! 끝 시간.
	DWORD			m_dwETimeOrig;				//! 끝 시간.
	DWORD			m_UNITTIME;					//! 키단위 시간.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	타격 이팩트 시점들.

	WORD			m_wDivCount;				//	에니메이션 분할 키 갯수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들.

	EFFANILIST		m_listEffAni;				// 모션 효과 !
	DxAniScale*		m_pAniScale;				// 애니메이션 스케일.~!

	SChaSoundData	m_ChaSoundData;				//	사운드 데이타

	// Attach Bone 관련 애니메이션 정보
	SAttachBoneAnimInfo		m_sAttBoneAnimInfo[PIECE_SIZE];

	SANIMCONINFO_PREV();
	virtual ~SANIMCONINFO_PREV();

    void CleanUp();

    SANIMCONINFO_MSG LoadFile( sc::BaseStream& stream, DWORD dwVersion, LPDIRECT3DDEVICEQ pd3dDevice=NULL );

	BOOL LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice=NULL );
	BOOL LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice=NULL );
	BOOL LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice=NULL );
	BOOL LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice=NULL );
	BOOL LoadFile_0105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice=NULL );
	BOOL LoadFile_0106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice=NULL );
	BOOL LoadFile_0107 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice=NULL );
	BOOL LoadFile_0108 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice=NULL );	//	데이터 포맷 변경 진행
	BOOL LoadFile_0109 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice=NULL );	//	어태치본 추가
	BOOL LoadFile_0110 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice=NULL );	//	어태치본 타격 애니 수정

    void SetName(const std::string& Name)
    {
        m_Name = Name;
    }

    void SetSkeletonName(const std::string& Name)
    {
        m_SkeletionName = Name;
    }
};

// (111 이하 버젼에 SAttachBoneAnimInfo에 디폴트 값을 넣어달라는 요청이 옴)
struct SANIMCONINFO				// 111, 112
{
	enum { VERSION=0x0115 };

	typedef std::list<DxEffAniData*>	EFFANILIST;
	typedef EFFANILIST::iterator		EFFANILIST_ITER;
	typedef EFFANILIST::const_iterator	EFFANILIST_CITER;

	TSTRING	        m_strCfgFile;		//	cfg 파일 이름.
	TSTRING	        m_strXFile;			//	x-file 파일 이름.
	TSTRING	        m_strBinFile;		//	bin 파일 이름
	TSTRING	        m_strSkeletion;		//	뼈다귀 이름.

	DWORD			m_dwFlag;					//	기타 속성.	EMANICONINFO
	DWORD			m_dwSTime;					//	시작 시간.
	DWORD			m_dwETime, m_dwETimeOrig;	//	끝 시간.
	DWORD			m_UNITTIME;					//	키단위 시간.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	에니메이션 분할 키 개수.
	WORD			m_wDivFrame[ACF_DIV];		//	에니메이션 분할 키들.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	타격 이팩트 시점들.

	SChaSoundData	m_ChaSoundData;				//	사운드 데이타

	EFFANILIST			m_listEffAni;				// 모션 효과 !
	DxAniScale*			m_pAniScale;				// 애니메이션 스케일.~!

	boost::scoped_ptr< DxAniJiggleBlend >	m_scpAniJiggleBlend;	// JiggleBone 과 KeyAnimation 과의 Blend 를 하도록 해준다.

    // Attach Bone 관련 애니메이션 정보
	SAttachBoneAnimInfo		m_sAttBoneAnimInfo[PIECE_SIZE];

	D3DXVECTOR3				m_vAnimationMoveOffset;	// AnimationMove

	BOOL	IsSameNameCFG( const TCHAR* szNameCFG );

	SANIMCONINFO();
	virtual ~SANIMCONINFO();
	
	void CleanUp();

	SANIMCONINFO_MSG	LoadFile(void* pBuf,UINT bufSize, LPDIRECT3DDEVICEQ pd3dDevice);
	BOOL				LoadFile(const TCHAR *szFileName, LPDIRECT3DDEVICEQ pd3dDevice = NULL, boost::function<void (const TCHAR*)> pArchive = 0);


	BOOL SaveFile( const TCHAR *szNameCFG, const TCHAR *szXFile );

	void InitEffAniList(LPDIRECT3DDEVICEQ pd3dDevice);

	// 0 KeyFrame <-> 1 JiggleBone
	void GetKeyJiggleWeight_PureThread( float afKeyJiggleWeight[], DWORD dwKeyJiggleWeightNUM, DWORD dwKey ) const;

	BOOL IsAnimationMove();

private:
	SANIMCONINFO& operator = ( SANIMCONINFO &value );

	SANIMCONINFO_MSG LoadFile( sc::BaseStream& stream, LPDIRECT3DDEVICEQ pd3dDevice=NULL );
};

class CAnimConfigCache
{
public:
    static const SANIMCONINFO* Load( const TSTRING& strFileName );
    static VOID Release();
};