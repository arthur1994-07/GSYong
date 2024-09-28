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
	TCHAR			m_szName[ACF_SZNAME];		//	�̸�.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	���ٱ� �̸�.

	DWORD			m_dwFlag;					//	��Ÿ �Ӽ�.
	DWORD			m_dwSTime;					//	���� �ð�.
	DWORD			m_dwETime, m_dwETimeOrig;	//	�� �ð�.
	DWORD			m_UNITTIME;					//	Ű���� �ð�.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	���ϸ��̼� ���� Ű ����.
	WORD			m_wDivFrame[ACF_DIV];		//	���ϸ��̼� ���� Ű��..

	SANIMCONINFO_101()
	{
		memset( m_szName, 0, sizeof(TCHAR)*ACF_SZNAME );
		memset( m_szSkeletion, 0, sizeof(TCHAR)*ACF_SZNAME );
	}
};

struct SANIMCONINFO_102
{
	TCHAR			m_szName[ACF_SZNAME];		//	�̸�.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	���ٱ� �̸�.

	DWORD			m_dwFlag;					//	��Ÿ �Ӽ�.
	DWORD			m_dwSTime;					//	���� �ð�.
	DWORD			m_dwETime, m_dwETimeOrig;	//	�� �ð�.
	DWORD			m_UNITTIME;					//	Ű���� �ð�.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	���ϸ��̼� ���� Ű ����.
	WORD			m_wDivFrame[ACF_DIV];		//	���ϸ��̼� ���� Ű��..

	SChaSoundData_102	m_ChaSoundData;				//	���� ����Ÿ

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
	TCHAR			m_szName[ACF_SZNAME];		//	�̸�.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	���ٱ� �̸�.

	DWORD			m_dwFlag;					//	��Ÿ �Ӽ�.
	DWORD			m_dwSTime;					//	���� �ð�.
	DWORD			m_dwETime, m_dwETimeOrig;	//	�� �ð�.
	DWORD			m_UNITTIME;					//	Ű���� �ð�.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	���ϸ��̼� ���� Ű ����.
	WORD			m_wDivFrame[ACF_DIV];		//	���ϸ��̼� ���� Ű��.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	Ÿ�� ����Ʈ ������.

	SChaSoundData_103	m_ChaSoundData;				//	���� ����Ÿ

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

	TCHAR			m_szName[ACF_SZNAME];		//	�̸�.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	���ٱ� �̸�.

	DWORD			m_dwFlag;					//	��Ÿ �Ӽ�.
	DWORD			m_dwSTime;					//	���� �ð�.
	DWORD			m_dwETime, m_dwETimeOrig;	//	�� �ð�.
	DWORD			m_UNITTIME;					//	Ű���� �ð�.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	���ϸ��̼� ���� Ű ����.
	WORD			m_wDivFrame[ACF_DIV];		//	���ϸ��̼� ���� Ű��.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	Ÿ�� ����Ʈ ������.

	SChaSoundData_103	m_ChaSoundData;				//	���� ����Ÿ

	EFFANILIST		m_listEffAni;				// ��� ȿ�� !

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

	TCHAR			m_szName[ACF_SZNAME];		//	�̸�.
	TCHAR			m_szSkeletion[ACF_SZNAME];	//	���ٱ� �̸�.

	DWORD			m_dwFlag;					//	��Ÿ �Ӽ�.
	DWORD			m_dwSTime;					//	���� �ð�.
	DWORD			m_dwETime, m_dwETimeOrig;	//	�� �ð�.
	DWORD			m_UNITTIME;					//	Ű���� �ð�.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	���ϸ��̼� ���� Ű ����.
	WORD			m_wDivFrame[ACF_DIV];		//	���ϸ��̼� ���� Ű��.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	Ÿ�� ����Ʈ ������.

	SChaSoundData	m_ChaSoundData;				//	���� ����Ÿ

	EFFANILIST		m_listEffAni;				// ��� ȿ�� !

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

struct SANIMCONINFO_PREV				// 110 ����..
{
	typedef std::list<DxEffAniData*>				EFFANILIST;			
	typedef std::list<DxEffAniData*>::iterator		EFFANILIST_ITER;

    std::string		m_Name;	                    //! �̸�.
	std::string		m_SkeletionName;	        //! ���ٱ� �̸�.

	DWORD			m_dwFlag;					//! ��Ÿ �Ӽ�.
	DWORD			m_dwSTime;					//! ���� �ð�.
	DWORD			m_dwETime;					//! �� �ð�.
	DWORD			m_dwETimeOrig;				//! �� �ð�.
	DWORD			m_UNITTIME;					//! Ű���� �ð�.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	Ÿ�� ����Ʈ ������.

	WORD			m_wDivCount;				//	���ϸ��̼� ���� Ű ����.
	WORD			m_wDivFrame[ACF_DIV];		//	���ϸ��̼� ���� Ű��.

	EFFANILIST		m_listEffAni;				// ��� ȿ�� !
	DxAniScale*		m_pAniScale;				// �ִϸ��̼� ������.~!

	SChaSoundData	m_ChaSoundData;				//	���� ����Ÿ

	// Attach Bone ���� �ִϸ��̼� ����
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
	BOOL LoadFile_0108 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice=NULL );	//	������ ���� ���� ����
	BOOL LoadFile_0109 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice=NULL );	//	����ġ�� �߰�
	BOOL LoadFile_0110 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice=NULL );	//	����ġ�� Ÿ�� �ִ� ����

    void SetName(const std::string& Name)
    {
        m_Name = Name;
    }

    void SetSkeletonName(const std::string& Name)
    {
        m_SkeletionName = Name;
    }
};

// (111 ���� ������ SAttachBoneAnimInfo�� ����Ʈ ���� �־�޶�� ��û�� ��)
struct SANIMCONINFO				// 111, 112
{
	enum { VERSION=0x0115 };

	typedef std::list<DxEffAniData*>	EFFANILIST;
	typedef EFFANILIST::iterator		EFFANILIST_ITER;
	typedef EFFANILIST::const_iterator	EFFANILIST_CITER;

	TSTRING	        m_strCfgFile;		//	cfg ���� �̸�.
	TSTRING	        m_strXFile;			//	x-file ���� �̸�.
	TSTRING	        m_strBinFile;		//	bin ���� �̸�
	TSTRING	        m_strSkeletion;		//	���ٱ� �̸�.

	DWORD			m_dwFlag;					//	��Ÿ �Ӽ�.	EMANICONINFO
	DWORD			m_dwSTime;					//	���� �ð�.
	DWORD			m_dwETime, m_dwETimeOrig;	//	�� �ð�.
	DWORD			m_UNITTIME;					//	Ű���� �ð�.

	EMANI_MAINTYPE	m_MainType;
	EMANI_SUBTYPE	m_SubType;

	WORD			m_wDivCount;				//	���ϸ��̼� ���� Ű ����.
	WORD			m_wDivFrame[ACF_DIV];		//	���ϸ��̼� ���� Ű��.

	WORD			m_wStrikeCount;
	SANIMSTRIKE		m_sStrikeEff[ACF_STRIKE];	//	Ÿ�� ����Ʈ ������.

	SChaSoundData	m_ChaSoundData;				//	���� ����Ÿ

	EFFANILIST			m_listEffAni;				// ��� ȿ�� !
	DxAniScale*			m_pAniScale;				// �ִϸ��̼� ������.~!

	boost::scoped_ptr< DxAniJiggleBlend >	m_scpAniJiggleBlend;	// JiggleBone �� KeyAnimation ���� Blend �� �ϵ��� ���ش�.

    // Attach Bone ���� �ִϸ��̼� ����
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