#pragma once

#include <string>
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/File/StringFile.h"
#include "../SigmaCore/File/BaseStream.h"
#include "../SigmaCore/File/SerialFile.h"

#include "../EngineLib/DxTools/DxCustomTypes.h"
#include "../EngineLib/DxLand/DxLandDef.h"
#include "../EngineLib/DxLand/DxLandGateMan.h"
#include "../EngineLib/G-Logic/GLDefine.h"
#include "../EngineLib/G-Logic/GLQuadTree.h"

#include "./Party/GLPartyDefine.h"
#include "./Land/GLMapAxisInfo.h"
#include "./Crow/GLMobSchedule.h"
#include "./Character/GLCharDefine.h"
#include "./GLogicEx.h"

class SimpleCollisionBase;
class LayerCollision;

struct DXLANDEFF;
typedef DXLANDEFF* PLANDEFF;

struct GLLANDMARK;
typedef GLLANDMARK* PLANDMARK;
typedef std::vector<PLANDMARK>	VECLANDMARK;

//const int MAX_CREATE_INSTANT_MAP_NUM	= 300;

enum EMCDT_SIGN
{
	EMSIGN_SAME			= 0,	//	=
	EMSIGN_HIGHER		= 1,	//	<
	EMSIGN_LOWER		= 2,	//	>
	EMSIGN_SAMEHIGHER	= 3,	//	<=
	EMSIGN_SAMELOWER	= 4,	//	>=

	EMSIGN_ALWAYS		= 5,	//	항상.
	EMSIGN_FROMTO		= 6,	//	~

    EMSIGN_SIZE			= 7
};

enum EMREQFAIL
{
	EMREQUIRE_COMPLETE	= 0,
	EMREQUIRE_LEVEL		= 1,
	EMREQUIRE_ITEM		= 2,
	EMREQUIRE_SKILL		= 3,
	EMREQUIRE_QUEST_COM	= 4,
	EMREQUIRE_QUEST_ACT	= 5,

	EMREQUIRE_LIVING	= 6,
	EMREQUIRE_BRIGHT	= 7,

	EMREQUIRE_SIZE		= 8,
};

struct SLEVEL_REQUIRE
{
	enum { VERSION = 0x0105, MAX_ITEM = 4};

	EMCDT_SIGN	m_signLevel;
	WORD		m_wLevel;
	WORD		m_wLevel2;

	SNATIVEID	m_sActQuestID;
	SNATIVEID	m_sComQuestID;

	SNATIVEID	m_sSkillID;
	SNATIVEID	m_sItemID;
	SNATIVEID	m_sItemIDMulti[MAX_ITEM];

	EMCDT_SIGN	m_signBright;
	int			m_nBright;

	EMCDT_SIGN	m_signLiving;
	int			m_nLiving;

	bool		m_bPartyMbr;
	bool		m_bForceExit;	// 아이템 시효성 완료시 강제퇴장

	SLEVEL_REQUIRE() :
		m_bForceExit( false ),
		m_bPartyMbr(false),

		m_signLevel(EMSIGN_ALWAYS),
		m_wLevel(0),
		m_wLevel2(0),

		m_sItemID(false),
		m_sSkillID(false),
	
		m_sComQuestID(false),
		m_sActQuestID(false),

		m_signLiving(EMSIGN_ALWAYS),
		m_nLiving(0),
		
		m_signBright(EMSIGN_ALWAYS),
		m_nBright(0)
	{
		for( int i = 0; i < MAX_ITEM; ++i ) m_sItemIDMulti[i] = SNATIVEID(false);
	}

	BOOL SAVE ( sc::SerialFile &SFile );
	BOOL LOAD ( sc::BaseStream &SFile );

	BOOL LOAD_0101 ( sc::BaseStream &SFile, DWORD dwSize );
	BOOL LOAD_0102 ( sc::BaseStream &SFile, DWORD dwSize );
	BOOL LOAD_0103 ( sc::BaseStream &SFile, DWORD dwSize );	//	데이터 포맷 변경 진행
	BOOL LOAD_0104 ( sc::BaseStream &SFile, DWORD dwSize );
	BOOL LOAD_0105 ( sc::BaseStream &SFile, DWORD dwSize );


	static VOID SaveCsvHead( std::fstream &SFile );
	VOID SaveCsv( std::fstream &SFile );
	VOID LoadCsv( CStringArray &StrArray );

	EMREQFAIL ISCOMPLETE ( GLCHARLOGIC * pCHARLOGIC ) const;

	BOOL IsHaveItem( GLCHARLOGIC * pCHARLOGIC, std::vector<SNATIVEID>& vList ) const;
	BOOL IsCheckItem() const;
};

struct SLEVEL_HEAD_100
{
	char					m_szWldFile[MAX_PATH];
	char					m_szMapName[MAXLANDNAME];
	SNATIVEID				m_MapID;
	EMDIVISION				m_eDivision;
};

struct SLEVEL_HEAD
{
	enum { VERSION = 0x0103, };

	std::string				m_strMapName;
	std::string				m_strWldFile;
	std::string				m_strNaviBypassFile;	// wld 에 내장된게 아닌 lev 에서 셋팅된 navi 파일을 사용하게 된다. (존재할경우)
	EMDIVISION				m_eDivision;
	EMBRIGHT				m_emBright;

	SLEVEL_HEAD () :
		m_emBright(BRIGHT_BOTH),
		m_eDivision(QUAD_DEFAULT_DIV)
	{
	}

	BOOL SAVE ( sc::SerialFile &SFile );
	BOOL LOAD ( sc::BaseStream &SFile );

	BOOL LOAD_0101 ( sc::BaseStream &SFile, DWORD dwSize );
	BOOL LOAD_0102 ( sc::BaseStream &SFile, DWORD dwSize );	//	데이터 포맷 변경 진행
	BOOL LOAD_0103 ( sc::BaseStream &SFile, DWORD dwSize );	//	네비변경기능추가

	void Assign ( const SLEVEL_HEAD_100 &sOldData );
};

struct SLIMITTIME
{
	int nLimitMinute;
	int nTellTermMinute;

	SLIMITTIME()
		: nLimitMinute(0)
		, nTellTermMinute(0)

	{
	}

	void Init()
	{
		nLimitMinute	= 0;
		nTellTermMinute = 0;
	}
};

struct SCONTROLCAM
{
	float		fUp;
	float		fDown;
	float		fLeft;
	float		fRight;
	float		fFar;
	float		fNear;
	D3DXVECTOR3 vCamPos;

	SCONTROLCAM()
		: fUp(0.0f)
		, fDown(0.0f)
		, fLeft(0.0f)
		, fRight(0.0f)
		, fNear(0.0f)
		, fFar(0.0f)
		, vCamPos( 0.0f, 0.0f, 0.0f )

	{
	}

	void Init()
	{
		vCamPos	   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		fUp		   = 0.0f;
		fDown	   = 0.0f;
		fLeft	   = 0.0f;
		fRight	   = 0.0f;
		fFar	   = 0.0f;
		fNear	   = 0.0f;

	}
};

struct SLEVEL_ETC_FUNC
{
	enum { VERSION = 0x0106, };

	bool		m_bUseFunction[EMETCFUNC_SIZE];
	SCONTROLCAM m_sControlCam;
	SLIMITTIME  m_sLimitTime;
	VEC_LANDEFF m_vecLandEffect;
	DWORD		m_dwReEntryTime;
    DWORD       m_dwVersion;

    /*
	SLANDEFFECT FindLandEffect( DWORD dwNum )
	{
		SLANDEFFECT landEffect;
		if (dwNum >= m_vecLandEffect.size())
            return landEffect;
        
		landEffect = m_vecLandEffect[dwNum];
		return landEffect;
	};
    */

	SLEVEL_ETC_FUNC()
        : m_dwReEntryTime(0)
	{
		ZeroMemory(m_bUseFunction, sizeof(m_bUseFunction));
		m_vecLandEffect.clear();
	}

	SLEVEL_ETC_FUNC& operator= ( const SLEVEL_ETC_FUNC &rvalue )
	{
		for( int i = 0; i < EMETCFUNC_SIZE; i++ )
		{
			m_bUseFunction[i] = rvalue.m_bUseFunction[i];
		}
		m_sControlCam	= rvalue.m_sControlCam;
		m_sLimitTime	= rvalue.m_sLimitTime;
		m_vecLandEffect = rvalue.m_vecLandEffect;
		m_dwReEntryTime  = rvalue.m_dwReEntryTime;
		

		return *this;
	}


	BOOL SAVE ( sc::SerialFile &SFile );
	BOOL LOAD ( sc::BaseStream &SFile );

	BOOL LOAD_0100 ( sc::BaseStream &SFile, DWORD dwSize );
	BOOL LOAD_0101 ( sc::BaseStream &SFile, DWORD dwSize );
	BOOL LOAD_0102 ( sc::BaseStream &SFile, DWORD dwSize );
	BOOL LOAD_0103 ( sc::BaseStream &SFile, DWORD dwSize );	//	데이터 포맷 변경 진행;
	BOOL LOAD_0104 ( sc::BaseStream &SFile, DWORD dwSize );	//  영역표시에 원 모양 추가;
	BOOL LOAD_0105 ( sc::BaseStream &SFile, DWORD dwSize );	//  재입장 시간 추가;
    BOOL LOAD_0106 ( sc::BaseStream &SFile, DWORD dwSize ); //  영역위치정보에 y값 추가;
};

namespace COMMENT
{
	extern std::string CDT_SIGN[EMSIGN_SIZE];

	extern std::string CDT_SIGN_ID[EMSIGN_SIZE];
};









class GLLevelFile
{
public:
	enum
	{
		VERSION = 0x0109,

		VERSION_BEFORE_ENCODE	= 0x0100,
		VERSION_BEFORE_GATE		= 0x0101,
	};

	enum EMINSTANCE_TYPE
	{
		EMINSTANCE_NORMAL,
		EMINSTANCE_OLD_INSTANCE,
		EMINSTANCE_NEW_INSTANCE,
		EMINSTANCE_NSIZE,
	};

public:
	const static char*		FILE_EXT;
	static char				szPATH[MAX_PATH];

public:
	static void SetPath ( const char*szPath )	{ StringCchCopy(szPATH,MAX_PATH,szPath); }
	static char* GetPath ()						{ return szPATH; }

protected:
	std::string	m_strFileName;
	DWORD		m_dwFileVer;
	MapID		m_mapID;

	bool		m_bPeaceZone;       //! true:무조건 PK 안됨
	bool		m_bPKZone;          //! true:pk 할수 있다 없다를 나타냄/적대관계 설정됨
	bool		m_bFreePK;          //! true:성향치가 감소되지 않는다/적대관계 설정됨
	bool		m_bPKMap;	        //! FreePKMap이지만 z키를 이용하지 않고 PK 가능하다.
	bool		m_bItemDrop;        //! true:아이템 드랍이 가능
	bool		m_bPetActivity;     //! true:팻활동 가능
	bool		m_bDECEXP;          //! 경험치 드랍 유무
	bool		m_bVehicleActivity;	//! true:탈것 활성화 가능
	bool		m_bClubBattleZone;
	bool		m_bLunchBoxForbid;
	bool		m_bCPReset;
	bool		m_bUIMapSelect; //! 지도 보기 UI 리스트에 등록 가/부;
	bool		m_bUIMapInfo; //! 지도 보기 UI 기능(설정,정보,파티,검색) 중 설정만 이용 가능한지 가/부;
	bool		m_bPartyMap; //! 파티 전용 맵;
	bool		m_bExpeditionMap; //! 원정대 전용 맵;
    bool		m_bClubPkRecord; //! PK시 클럽 로그에 기록할 것인가?
	bool		m_bCullByObjectMap; // 케릭터 시점에서 사물 지형에 의한 가려짐 켬/끔;
	bool		m_bChangeSchool; // 학원 변경 카드 허용 여부

protected:
	EMINSTANCE_TYPE	m_emInstanceType;
	DWORD					m_dwInstantHostID;  // 인던주인의 GAEA ID
	bool					m_bPartyInstantMap; // 파티 인던인지 아닌지. 

protected:
	SLEVEL_HEAD				m_sLevelHead;
	SLEVEL_REQUIRE			m_sLevelRequire;
	SLEVEL_ETC_FUNC			m_sLevelEtcFunc;

protected:
	typedef std::vector<DxLandGateMan*>	LandGateManContainer;
	typedef LandGateManContainer::iterator		LandGateManContainerIterator;
	typedef LandGateManContainer::const_iterator	LandGateManContainerConstIterator;

	LandGateManContainer m_LandGateManContainer;
	DxLandGateMan*			m_pLandGateMan;

protected:
	GLMapAxisInfo			m_sLevelAxisInfo;

protected:
	//	Note : Single Effect
	//
	//std::vector<DWORD>							m_vecdwNumLandEff;

	template <typename T>
	struct DXLANDEFF_CAPSULE
	{
		T*	m_pData;

		DXLANDEFF_CAPSULE()
			: m_pData(NULL)
		{
		}

		~DXLANDEFF_CAPSULE()
		{
			SAFE_DELETE( m_pData );
		}
	};

	//// Base Effect
	//boost::shared_ptr<DXLANDEFF_CAPSULE<DXLANDEFF>>		m_spLandEffList;
	//boost::shared_ptr<DXLANDEFF_CAPSULE<OBJAABBNode>>	m_spLandEffTree;


	// [shhan][2015.10.01] 만드는 것을 반대했지만 기획팀에선 끝내 만들어 달라고 한다. 얼마나 활용을 할지 의심이 든다.
	//
	// Layer 별로 셋팅된 Effect
	class LayerEffect
	{
	private:
		enum
		{
			VERSION = 0x101,	
		};

	private:
		BOOL												m_bActiveClientRender;		// Server 쪽은 MobLayer 활성화 여부로 알 수 있다.
		boost::shared_ptr<DXLANDEFF_CAPSULE<DXLANDEFF>>		m_spLandEffList;
		boost::shared_ptr<DXLANDEFF_CAPSULE<OBJAABBNode>>	m_spLandEffTree;

	public:
		void InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
		void RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
		void InvalidateDeviceObjects();
		void DeleteDeviceObjects();
		void Render_EFF ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv );

		// Edit
		PLANDEFF	GetLandEffHEAD() const;
		PLANDEFF	FindLandEff ( const char* szName ) const;
		void		AddLandEff ( PLANDEFF pLandEff );
		void		DelLandEff ( PLANDEFF pLandEff );
		void		SetObjRotate( const D3DXMATRIX& matRotate );
		void		RenderEffectName( LPDIRECT3DDEVICEQ pd3dDevice );

		void BuildSingleEffTree();

		const boost::shared_ptr<DXLANDEFF_CAPSULE<DXLANDEFF>> GetLandEffList() const
		{
			return m_spLandEffList;
		}

		void SetActiveLayer();
		void SetDeActiveLayer();
		void SetWorldBattleFlag_LandEff( const TCHAR* pWorldBattleFlag );

		// 서버상 확인. Effect 가 있다면 Client 에 Msg 보내기 위한 함수.
		BOOL IsExistEffectForServer() const
		{ 
			return m_spLandEffList->m_pData ? TRUE : FALSE; 
		}

	public:
		void Save( sc::SerialFile &SFile );
		void Load( sc::BaseStream &SFile, BOOL bSkip );
		void LoadOLD( sc::BaseStream &SFile, BOOL bExceptEffect );
		void LoadOLD_2( sc::BaseStream &SFile, BOOL bSkip );

	public:
		LayerEffect( BOOL bActiveClientRender );
		~LayerEffect();
	};

	// m_vecLayerEffect 는 기본적으로 무조건 한개는 있다.
	std::vector<boost::shared_ptr<LayerEffect>>	m_vecLayerEffect;	// 기본 Base ( 0 )
																	// Layer 전체 정보 ( 1 부터 시작 ) 

private:
	


protected:
	// m_vecLayerCollision 는 기본적으로 무조건 한개는 있다.
	std::vector<boost::shared_ptr<LayerCollision>>	m_vecLayerCollision;	// 기본 Base ( 0 )
	// Layer 전체 정보 ( 1 부터 시작 ) 

public:
	const std::vector<boost::shared_ptr<LayerCollision>>& GetvecLayerCollision() const
	{
		return m_vecLayerCollision;
	}


protected:
	//DWORD			m_dwNumLandMark;
	//PLANDMARK		m_pLandMarkList;
	VECLANDMARK		m_vecLandMark;
	//OBJAABBNode*	m_pLandMarkTree;

//protected:
//	VECLANDMARK		m_vecLANDMARK_DISP;

public:
	void SetObjRotate90();

public:
	// 공격가능 거리가 되고, LayerSimpleCollision 에 닿지 않으면 TRUE
	// 하나라도 문제가 되면 FALSE
	BOOL IsAbleAttack_Range_LayerSimpleCollision( float fTakeAbleDis, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fOffset );

public:
	friend class CRequirePage;
	friend class CLevelEditView;
	friend class CEtcFuncPage;
	friend class CLevelCollision;

public:
	BOOL IsLayerSimpleCollision( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fOffset ) const;
	const LayerCollision*		GetLayerSimpleCollisionList( size_t unLayer ) const;
	const SimpleCollisionBase*	FindLayerSimpleCollision ( size_t unLayer, const char* szName ) const;
	void AddLayerSimpleCollision ( size_t unLayer, boost::shared_ptr<SimpleCollisionBase> spNew );
	void ChangeLayerSimpleCollision ( size_t unLayer, int nIndex, boost::shared_ptr<SimpleCollisionBase> spNew );
	void DelLayerSimpleCollision ( size_t unLayer, DWORD dwIndex );
	void RenderLayerSimpleCollision() const;									// Client 에서만 사용된다.
	void RenderLayerSimpleCollisionName( size_t unLayer ) const;				// Client 에서만 사용된다.

public:
	//DWORD GetNumLandEff ()				{ return m_dwNumLandEff; }

	PLANDEFF GetLandEffList( size_t unLayer );
	PLANDEFF FindLandEff ( size_t unLayer, char* szName );
	void AddLandEff ( size_t unLayer, PLANDEFF pLandEff );
	void DelLandEff ( size_t unLayer, PLANDEFF pLandEff );
	void BuildSingleEffTree ( size_t unLayer );
	void RenderEffectName( LPDIRECT3DDEVICEQ pd3dDevice, size_t unLayer );

public:
	void SetActiveLayer( size_t unLayer );
	void SetDeActiveLayer( size_t unLayer );
	void SetDeActiveAllLayer();
	BOOL IsExistLayerObjectForServer( size_t unLayer );


public:
	DWORD GetNumLandMark ()	{ return static_cast<DWORD>( m_vecLandMark.size() ); }
	PLANDMARK GetLandMark(DWORD index) 
	{
        DWORD MarkSize = static_cast<DWORD> (m_vecLandMark.size());
		GASSERT(index < MarkSize);
        if (index < MarkSize)
		    return m_vecLandMark[index]; 
        else
            return NULL;
	}

	void AddLandMark ( PLANDMARK pLandMark );
	void DelLandMark ( PLANDMARK pLandMark );
	PLANDMARK FindLandMark ( char* szName );

	//void BuildLandMarkTree ();

public:
	void SetFileName ( const char* szFile );
	const char* GetFileName () const;
	const DWORD GetFileVer () const;
	const DWORD GetInstantMapHostID () const { return m_dwInstantHostID; }

public:
	void SetMapID(const MapID& mapID) { m_mapID = mapID; }
	void SetBaseMapID( const SNATIVEID& baseMapID) { m_mapID.setBaseMapID(baseMapID); }
	void SetGaeaMapID( const SNATIVEID& gaeaMapID) { m_mapID.setGaeaMapID(gaeaMapID); }
	void SetPeaceZone(const bool bPeaceZone) { m_bPeaceZone = bPeaceZone; }
	void SetPKZone(const bool bPKZone) { m_bPKZone = bPKZone; }
	void SetInstantMapData( const DWORD dwHostID, const GLPartyID& PartyID );
	void SetChangeSchool( const bool bChangeSchool ) { m_bChangeSchool = bChangeSchool; }

	const SNATIVEID& GetBaseMapID(void) const { return m_mapID.getBaseMapID(); }
	const SNATIVEID& GetGaeaMapID(void) const { return m_mapID.getGaeaMapID(); }
	const MapID& GetMapID(void) const { return m_mapID; }

	const char* GetWldFileName () const;
	const char* GetNaviBypassFileName () const;
	BOOL		IsEmptyNaviBypassFileName () const;
	const EMBRIGHT GETBRIGHT () const;
	const EMINSTANCE_TYPE GetInstantType() const		{ return m_emInstanceType; }

	const bool isMapInstant(void) const { return m_mapID.isSameID() == true ? false : true; }
	const bool isMapRawData(void) const { return m_mapID.isValid() == true ? false : true; }

	bool IsPeaceZone() const		{ return m_bPeaceZone; }
	bool IsPKZone() const			{ return m_bPKZone; }
	bool IsFreePK() const			{ return m_bFreePK; }
	bool IsPKMap() const			{ return m_bPKMap; }
	bool IsItemDrop() const			{ return m_bItemDrop; }
	bool IsPetActivity() const		{ return m_bPetActivity; }
	bool IsDecreaseExpMap() const	{ return m_bDECEXP; }
	bool IsVehicleActivity() const	{ return m_bVehicleActivity; }
	bool IsClubBattleZone()	const	{ return m_bClubBattleZone; }
	bool IsInstantMap() const		{ return m_emInstanceType != EMINSTANCE_NORMAL; }
	bool IsNewInstantMap() const	{ return m_emInstanceType == EMINSTANCE_NEW_INSTANCE; }
	bool IsOldInstantMap() const	{ return m_emInstanceType == EMINSTANCE_OLD_INSTANCE; }
	bool IsPartyInstantMap() const	{ return m_bPartyInstantMap; }	
	bool IsLunchBoxForbid() const   { return m_bLunchBoxForbid; }
	bool IsCPReset() const			{ return m_bCPReset; }
    bool IsClubPkRecord() const     { return m_bClubPkRecord; }
	bool IsUIMapSelect() const	{ return m_bUIMapSelect; }
	bool IsUIMapInfo() const		{ return m_bUIMapInfo; }
	bool IsPartyMap() const		{ return m_bPartyMap; }
	bool IsExpeditionMap() const	{ return m_bExpeditionMap; }
	bool IsCullByObjectMap(void) const { return m_bCullByObjectMap; }
	bool IsChangeSchool() const { return m_bChangeSchool; }

	void SetCullByObjectMap(const bool bCullByObjectMap) { m_bCullByObjectMap = bCullByObjectMap; }	
	void SetUseInstantMap(const EMINSTANCE_TYPE emInstasnceType) { m_emInstanceType = emInstasnceType; }
	void SetClubBattleZone(bool bClubBattleZone) { m_bClubBattleZone = bClubBattleZone; }
	void SetLunchBoxForbid(bool bForbid)		 { m_bLunchBoxForbid = bForbid; }

public:
	EMDIVISION GetLandTreeDiv () const;
	void SetLandTreeDiv ( EMDIVISION eDivision );

public:
	const SLEVEL_ETC_FUNC* GetLevelEtcFunc() const { return &m_sLevelEtcFunc; }
	const SLEVEL_REQUIRE* GetLevelRequire() const { return &m_sLevelRequire; }
	const GLMapAxisInfo& GetMapAxisInfo() const	{ return m_sLevelAxisInfo; }
	DxLandGateMan* GetLandGateMan()	{ return m_pLandGateMan; }
	DxLandGateMan* GetLandGateMan( UINT nLayer );		

	BOOL GetUseLevelEtcFunc(const EMETCFUNC emEtcFunc);

	// WorldBattle
public:
	//! 승리한 클럽의 국기를 맵에 셋팅한다.
	//void SetFlagWinnerClubCountry( DWORD dwGuidanceID, const TSTRING& stTextureName, const D3DXVECTOR4& vFlagConvert );

	void SetWorldBattleFlag_LandEff( const TCHAR* pWorldBattleFlag );

public:
	HRESULT FrameMove(float fTime, float fElapsedTime);
	HRESULT Render(LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME& cv);
	HRESULT Render_EFF(LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME& cv);

public:
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);

	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();	
	void	ResetData();
	void clearGateMan();

	// [shhan][2015.10.02] m_vecLayerEffect 의 0 번째만 사용되도록 함.
    void SetLandEffect(DWORD dwGateID, bool bEnable);

public:
	GLLevelFile();
	virtual ~GLLevelFile();

public:
	GLLevelFile& operator= (const GLLevelFile &rvalue );
};