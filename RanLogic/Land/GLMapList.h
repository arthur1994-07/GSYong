#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <boost/tr1/memory.hpp>
#include <string>
#include <map>

#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../item/GLItemDef.h"
#include "../../SigmaCore/Lua/MinLua.h"

struct F_SERVER_INFO;
struct SLEVEL_REQUIRE;
struct SLEVEL_ETC_FUNC;

namespace sc
{
    class SerialFile;
    class BaseStream;
}   

enum
{
	MAP_NAME_MAX	= 16,
};

typedef enum
{ // Prision map limit
	PML_CHATTING,  // 채팅
	PML_WARP,      // 맵간이동
	PML_ITEM,	   // 아이템 사용
	PML_WAR,	   // 불가
	PML_PARTY,
	PML_NONE,
} EPML;

struct SMAPNODE
{
	enum
	{
		VERSION	= 0x0110,
	};
	enum EMINSTANCE_TYPE
	{
		EMINSTANCE_NORMAL,
		EMINSTANCE_OLD_INSTANCE,
		EMINSTANCE_NEW_INSTANCE,
		EMINSTANCE_NSIZE,
	};
	
	bool			m_bUse;          			//! 사용/사용하지 않음
	std::string		m_LevelFile;     			//! Level file
	SNATIVEID		m_NativeID;      			//! ID
	DWORD			m_FieldServerID;

	bool			m_bBattleZone;      		//! 선도클럽 결정전이 열리는 Map;
	bool			m_bPeaceZone;       		//! 평화지역;	
	bool			m_bCommission;      		//! 수수료 지역;

	bool			m_bPKZone;          		//! PK 가능 지역;
	bool			m_bFreePK;          		//! 자유 PK 지역;	
	bool			m_bClubPkRecord;			//! PK시 클럽 로그에 기록할 것인가?

	bool			m_bItemDrop;        		//! 사망시 아이템 드랍 가능;
	bool			m_bFriendMove;      		//! 친구이동 가능;
	bool			m_bRestart;         		//! 사망후 재시작 가능한가?;
	bool			m_bPetActivity;     		//! Pet 가능?;
	bool			m_bDecExp;          		//! 사망시 경험치 감소?;
	bool			m_bVehicleActivity; 		//! 탈것 가능;

	bool			m_bClubBattleZone;  		//! Club Battle zone;
	bool			m_bCDMZone;         		//! Club Death Match zone;
	bool			m_bCaptureTheField; 		//! Capture the field // mjeon.CaptureTheField;

	bool			m_bChangeSchool;			// 학원 변경 기능 허용 여부

	std::string		m_MapID;					//! MAP_MID_SID [ ex) MAP_0_0 ]
	std::string		m_BgmName;
	std::string		m_LoadingImageName;			
	
	bool			m_bQBoxDrop;				//! Question Box Drop;	
	bool			m_bLunchBoxForbid;  		//! Lunch box enable?;
	bool			m_bCPReset;					//! Combat point reset;
	bool			m_bPKMap;					//! PK;

	bool			m_bUIMapSelect;				//! 지도 보기 UI 리스트에 등록 가/부;
	bool			m_bUIMapInfo;				//! 지도 보기 UI 기능(설정,정보,파티,검색) 중 설정만 이용 가능한지 가/부;
	bool			m_bPartyMap;				// 파티 전용 맵;
	bool			m_bExpeditionMap;			// 원정대 전용 맵;

    bool			m_bPrivateMarketOpenable;	//! 개인상점 오픈가능?;
    bool			m_bSparring;				//! 1:1, 파티 대련 가능?;
	bool			m_bCullByObjectMap;			//! 케릭터 시점에서 사물 지형에 의한 가려짐 켬/끔;
	bool			m_bTutorialMap;
	bool			m_bDropBlockItem;		// 아이템 드랖이 불가능한가?
	bool			m_bDisableSwitchMob;	// 스위치몹을 허용 하는가?;

	float			m_fRateExpDrop;			// 경험치 배율;
	float			m_fRateItemDrop;		// 아이템 드랖 배율;

	EMINSTANCE_TYPE			m_emInstanceType;				//! 인던인가?(구인던과의 구분을 위해 bool에서 변형);

	SLEVEL_REQUIRE* m_pLevelRequire;
	SLEVEL_ETC_FUNC* m_pLevelEtcFunc;

	typedef std::vector<EMITEM_TYPE> ItemTypeList;
	typedef ItemTypeList::value_type ItemTypeValue;
	typedef ItemTypeList::iterator ItemTypeListIt;
	typedef ItemTypeList::const_iterator ItemTypeListItCIt;

	ItemTypeList	m_prohibiteditems;			//! 금지 아이템 리스트
	bool			m_bRebirthBlock;			//! ver.0x0109 부활 스킬이 불가능한가?;
	bool			m_bMacro;					// 매크로 사용가능한가?;
	bool			m_bPrison;

	bool			m_bChangeCharSlot;			// 캐릭터 슬롯 변경이 가능한 맵인가?;

public:
    SMAPNODE();
    virtual ~SMAPNODE();

	SMAPNODE (const SMAPNODE& value);
	SMAPNODE& operator = (const SMAPNODE& value);	

public:
    //! ID
    const SNATIVEID& GetId() const { return m_NativeID; }
    SNATIVEID GetId() { return m_NativeID; }

    //! Field Server Number
    DWORD GetFieldServerNum() const { return m_FieldServerID; }

	//! Instance Map Type;
	const EMINSTANCE_TYPE GetInstanceType(void) const { return m_emInstanceType; }

	inline bool IsUse(void) const { return m_bUse; }
    //! 선도클럽 결정전이 열리는 Map
    inline bool IsLeaderClubZone() const { return m_bBattleZone; }
    inline void SetLeaderClubZone(bool Set) { m_bBattleZone = Set; }

    //! 평화지역
    inline bool IsPeaceZone() const { return m_bPeaceZone; }

    //! 수수료 지역
    inline bool IsCommission() const { return m_bCommission; }

    //! PK 가능 지역
    inline bool IsPkZone() const { return m_bPKZone; }

    //! 자유 PK 지역
    inline bool IsFreePk() const { return m_bFreePK; }

    //! PK
    inline bool IsPkMap() const { return m_bPKMap; }

    //! 사망시 아이템 드랍 가능
    inline bool IsItemDrop() const { return m_bItemDrop; }

    //! 친구이동 가능
    inline bool IsFriendMove() const { return m_bFriendMove; }

    //! PK시 클럽 로그에 기록할 것인가?
    inline bool IsClubPkRecord() const { return m_bClubPkRecord; }

    //! 인던인가?
    inline bool IsInstantMap() const { return m_emInstanceType != EMINSTANCE_NORMAL; }
    
    // new instance System?
	__forceinline const bool IsOldInstanceMap() const { return	m_emInstanceType == EMINSTANCE_OLD_INSTANCE; }
    __forceinline const bool IsNewInstanceMap() const { return  m_emInstanceType == EMINSTANCE_NEW_INSTANCE; }

    //! Question Box Drop
    inline bool IsQBoxDrop() const { return m_bQBoxDrop; }

    //! 사망후 재시작 가능한가?
    inline bool IsRestrt() const { return m_bRestart; }

    //! Pet 가능?
    inline bool IsPetActivity() const { return m_bPetActivity; }

    //! 사망시 경험치 감소?
    inline bool IsDecExp() const { return m_bDecExp; }

    //! 탈것 가능
    inline bool IsVehicleActivity() const { return m_bVehicleActivity; }

    //! Club Battle zone
    inline bool IsClubBattleZone() const { return m_bClubBattleZone; }

    //! Club Death Match zone
    inline bool IsClubDeathMatchZone() const { return m_bCDMZone; }

    //! Capture the field // mjeon.CaptureTheField
    inline bool IsCaptureTheField() const { return m_bCaptureTheField; }

    //! Lunch box enable?
    inline bool IsLunchBoxForbid() const { return m_bLunchBoxForbid; }

    //! Combat point reset
    inline bool IsCombatPointReset() const { return m_bCPReset; }

	//! Party Map
	inline bool IsPartyMap() const { return m_bPartyMap; }

	//! Expedition Map
	inline bool IsExpeditionMap() const { return m_bExpeditionMap; }

    //! 개인상점 개설이 가능한가?;
    inline bool IsPrivateMarketOpen() const { return m_bPrivateMarketOpenable; }

    //! 1:1, 파티 대련이 가능한가?;
    inline bool IsSparring() const { return m_bSparring; }

	inline bool IsUIMapSelect() const { return m_bUIMapSelect; }
	inline bool IsUIMapInfo() const { return m_bUIMapInfo; }

	//! 케릭터 시점 기준(카메라 아님)에서 지형에 의한 주변 액터 컬링을 할 것인가?;
	inline bool IsCullByObjectMap() const { return m_bCullByObjectMap; }

	// 학원변경기능 허용 여부
	inline bool IsChangeSchool() const { return m_bChangeSchool; }

	//! 사용 가능 아이템 인가? -> 사용 불가능한 아이템인가?
	inline bool IsBlockDrugItem(const ItemTypeValue eDrug) const
	{
// 		if ( m_emInstanceType == EMINSTANCE_NEW_INSTANCE )
// 			return true; // 신규 인던인 경우 mapslist의 설정을 무시함;

		return std::find(m_prohibiteditems.begin(), m_prohibiteditems.end(), eDrug) != m_prohibiteditems.end();
	}

	//! 부활이 불가능한 맵인가?;
	inline bool IsRebirthBlock() const { return m_bRebirthBlock; }

	inline const bool IsDropBlockItem(void) const { return m_bDropBlockItem; }

	inline const bool IsDisableSwitchMob(void) const { return m_bDisableSwitchMob; }

	inline const float GetRateExpDrop(void) const { return m_fRateExpDrop; }
	inline const float GetRateItemDrop(void) const { return m_fRateItemDrop; }
	inline const bool IsMacroUsable(void) const {return m_bMacro;}
	inline const bool IsPrisonMap(void) const {return m_bPrison;}

	inline const bool IsChangeCharSlot ( void ) const { return m_bChangeCharSlot; }
};

class MapAxisInfoMan;
class GLMapAxisInfo;

class GLMapList
{
	enum
	{
		VERSION	= 0x0100,		
	};

public:
	static const char* _FILEHEAD;
	
    typedef std::map<DWORD, SMAPNODE> FIELDMAP;
	typedef FIELDMAP::iterator		  FIELDMAP_ITER;
    typedef FIELDMAP::const_iterator  FIELDMAP_CITER;
    typedef FIELDMAP::value_type      FIELDMAP_VALUE;

	typedef std::vector<DWORD>				VEC_INSTANT_MAPID;
	typedef VEC_INSTANT_MAPID::iterator		VEC_INSTANT_MAPID_ITER;

protected:
	FIELDMAP		m_MapList;
    MapAxisInfoMan* m_pAxisMan;

public:
	BOOL IsRegMapNode(const SMAPNODE& MapNode) const;	
    HRESULT LoadMapList(BOOL bTool = FALSE, F_SERVER_INFO* pServerInfo = NULL, int nServerNum = 0, const bool bEmulator = false);

protected:
	//  [9/11/2013 gbgim];
	// - 107 버전 이전 버전일 경우 이걸로 로드한다;
	//   기존에(1.4이전) mapslist.mst를 로드하던 것을 mapslist.lua로 로드하고 저장하도록;
	//   수정이 됬는데 버전에 따른 컨버터 기능이 .mst에만 존재하여 문제가 생김;
	// - 기존 lua파일로 버전업(데이터 추가 및 변경)이 이뤄질 경우 버전체크가 안되어;
	//   수작업이 필요하던 부분을 없애기 위해 추가함;
	bool LoadLuaCurr(const std::string& FileName, BOOL bTool=FALSE, F_SERVER_INFO* pServerInfo=NULL, int nServerNum=0, const bool bEmulator = false);
	bool LoadLua( const std::string& FileName
		, BOOL bTool=FALSE
		, F_SERVER_INFO* pServerInfo=NULL
		, int nServerNum=0, const bool bEmulator = false);

public:
	//  [9/12/2013 gbgim];
	HRESULT SAVE( const std::string& szMapListFile );
	bool SaveLua( const TCHAR* szMapListFile = NULL );
	
	HRESULT CleanUpMapList();

	static void SaveCsvHead(std::fstream& SFile);
	bool        SaveCsv    (std::fstream& SFile);
	bool	    LoadCsv    (std::string& strFileName);

public:
	SMAPNODE* FindMapNode(const SNATIVEID& sNativeID);
    const SMAPNODE* FindMapNode(const SNATIVEID& sNativeID) const;
	virtual const char* GetMapName(const SNATIVEID& sNativeID);
    virtual std::string GetMapNameStr(const SNATIVEID& MapID) const;
	const SLEVEL_REQUIRE*  GetLevelRequire(const SNATIVEID& sNativeID);
	const SLEVEL_ETC_FUNC* GetLevelEtcFunc(const SNATIVEID& sNativeID);

	FIELDMAP& GetMapList() { return m_MapList; }
    
    MapAxisInfoMan* GetAxisMan() { return m_pAxisMan; }
    
    void ConvertWorldToMap(const SNATIVEID& baseMapID, const D3DXVECTOR3& vPos, int& nPosX, int& nPosY);
    void ConvertMapToWorld(const SNATIVEID& baseMapID, int PosX, int PosY, D3DXVECTOR3& vPos);

    std::tr1::shared_ptr<GLMapAxisInfo> GetAxisInfo(const std::string& FileName);
    
public:
	GLMapList();
	virtual ~GLMapList();

private:
	bool _GetBoolean( LuaPlus::LuaObject& LuaObj, const std::string& strKey, bool& bOutput );
	bool _GetIntager( LuaPlus::LuaObject& LuaObj, const std::string& strKey, INT&  nOutput );
	bool _GetWString( LuaPlus::LuaObject& LuaObj, const std::string& strKey, std::string& szOutput );
	bool _GetFloat( LuaPlus::LuaObject& LuaObj, const std::string& strKey, float& fOutput);
};