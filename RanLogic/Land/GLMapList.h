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
	PML_CHATTING,  // ä��
	PML_WARP,      // �ʰ��̵�
	PML_ITEM,	   // ������ ���
	PML_WAR,	   // �Ұ�
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
	
	bool			m_bUse;          			//! ���/������� ����
	std::string		m_LevelFile;     			//! Level file
	SNATIVEID		m_NativeID;      			//! ID
	DWORD			m_FieldServerID;

	bool			m_bBattleZone;      		//! ����Ŭ�� �������� ������ Map;
	bool			m_bPeaceZone;       		//! ��ȭ����;	
	bool			m_bCommission;      		//! ������ ����;

	bool			m_bPKZone;          		//! PK ���� ����;
	bool			m_bFreePK;          		//! ���� PK ����;	
	bool			m_bClubPkRecord;			//! PK�� Ŭ�� �α׿� ����� ���ΰ�?

	bool			m_bItemDrop;        		//! ����� ������ ��� ����;
	bool			m_bFriendMove;      		//! ģ���̵� ����;
	bool			m_bRestart;         		//! ����� ����� �����Ѱ�?;
	bool			m_bPetActivity;     		//! Pet ����?;
	bool			m_bDecExp;          		//! ����� ����ġ ����?;
	bool			m_bVehicleActivity; 		//! Ż�� ����;

	bool			m_bClubBattleZone;  		//! Club Battle zone;
	bool			m_bCDMZone;         		//! Club Death Match zone;
	bool			m_bCaptureTheField; 		//! Capture the field // mjeon.CaptureTheField;

	bool			m_bChangeSchool;			// �п� ���� ��� ��� ����

	std::string		m_MapID;					//! MAP_MID_SID [ ex) MAP_0_0 ]
	std::string		m_BgmName;
	std::string		m_LoadingImageName;			
	
	bool			m_bQBoxDrop;				//! Question Box Drop;	
	bool			m_bLunchBoxForbid;  		//! Lunch box enable?;
	bool			m_bCPReset;					//! Combat point reset;
	bool			m_bPKMap;					//! PK;

	bool			m_bUIMapSelect;				//! ���� ���� UI ����Ʈ�� ��� ��/��;
	bool			m_bUIMapInfo;				//! ���� ���� UI ���(����,����,��Ƽ,�˻�) �� ������ �̿� �������� ��/��;
	bool			m_bPartyMap;				// ��Ƽ ���� ��;
	bool			m_bExpeditionMap;			// ������ ���� ��;

    bool			m_bPrivateMarketOpenable;	//! ���λ��� ���°���?;
    bool			m_bSparring;				//! 1:1, ��Ƽ ��� ����?;
	bool			m_bCullByObjectMap;			//! �ɸ��� �������� �繰 ������ ���� ������ ��/��;
	bool			m_bTutorialMap;
	bool			m_bDropBlockItem;		// ������ ����� �Ұ����Ѱ�?
	bool			m_bDisableSwitchMob;	// ����ġ���� ��� �ϴ°�?;

	float			m_fRateExpDrop;			// ����ġ ����;
	float			m_fRateItemDrop;		// ������ ��� ����;

	EMINSTANCE_TYPE			m_emInstanceType;				//! �δ��ΰ�?(���δ����� ������ ���� bool���� ����);

	SLEVEL_REQUIRE* m_pLevelRequire;
	SLEVEL_ETC_FUNC* m_pLevelEtcFunc;

	typedef std::vector<EMITEM_TYPE> ItemTypeList;
	typedef ItemTypeList::value_type ItemTypeValue;
	typedef ItemTypeList::iterator ItemTypeListIt;
	typedef ItemTypeList::const_iterator ItemTypeListItCIt;

	ItemTypeList	m_prohibiteditems;			//! ���� ������ ����Ʈ
	bool			m_bRebirthBlock;			//! ver.0x0109 ��Ȱ ��ų�� �Ұ����Ѱ�?;
	bool			m_bMacro;					// ��ũ�� ��밡���Ѱ�?;
	bool			m_bPrison;

	bool			m_bChangeCharSlot;			// ĳ���� ���� ������ ������ ���ΰ�?;

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
    //! ����Ŭ�� �������� ������ Map
    inline bool IsLeaderClubZone() const { return m_bBattleZone; }
    inline void SetLeaderClubZone(bool Set) { m_bBattleZone = Set; }

    //! ��ȭ����
    inline bool IsPeaceZone() const { return m_bPeaceZone; }

    //! ������ ����
    inline bool IsCommission() const { return m_bCommission; }

    //! PK ���� ����
    inline bool IsPkZone() const { return m_bPKZone; }

    //! ���� PK ����
    inline bool IsFreePk() const { return m_bFreePK; }

    //! PK
    inline bool IsPkMap() const { return m_bPKMap; }

    //! ����� ������ ��� ����
    inline bool IsItemDrop() const { return m_bItemDrop; }

    //! ģ���̵� ����
    inline bool IsFriendMove() const { return m_bFriendMove; }

    //! PK�� Ŭ�� �α׿� ����� ���ΰ�?
    inline bool IsClubPkRecord() const { return m_bClubPkRecord; }

    //! �δ��ΰ�?
    inline bool IsInstantMap() const { return m_emInstanceType != EMINSTANCE_NORMAL; }
    
    // new instance System?
	__forceinline const bool IsOldInstanceMap() const { return	m_emInstanceType == EMINSTANCE_OLD_INSTANCE; }
    __forceinline const bool IsNewInstanceMap() const { return  m_emInstanceType == EMINSTANCE_NEW_INSTANCE; }

    //! Question Box Drop
    inline bool IsQBoxDrop() const { return m_bQBoxDrop; }

    //! ����� ����� �����Ѱ�?
    inline bool IsRestrt() const { return m_bRestart; }

    //! Pet ����?
    inline bool IsPetActivity() const { return m_bPetActivity; }

    //! ����� ����ġ ����?
    inline bool IsDecExp() const { return m_bDecExp; }

    //! Ż�� ����
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

    //! ���λ��� ������ �����Ѱ�?;
    inline bool IsPrivateMarketOpen() const { return m_bPrivateMarketOpenable; }

    //! 1:1, ��Ƽ ����� �����Ѱ�?;
    inline bool IsSparring() const { return m_bSparring; }

	inline bool IsUIMapSelect() const { return m_bUIMapSelect; }
	inline bool IsUIMapInfo() const { return m_bUIMapInfo; }

	//! �ɸ��� ���� ����(ī�޶� �ƴ�)���� ������ ���� �ֺ� ���� �ø��� �� ���ΰ�?;
	inline bool IsCullByObjectMap() const { return m_bCullByObjectMap; }

	// �п������� ��� ����
	inline bool IsChangeSchool() const { return m_bChangeSchool; }

	//! ��� ���� ������ �ΰ�? -> ��� �Ұ����� �������ΰ�?
	inline bool IsBlockDrugItem(const ItemTypeValue eDrug) const
	{
// 		if ( m_emInstanceType == EMINSTANCE_NEW_INSTANCE )
// 			return true; // �ű� �δ��� ��� mapslist�� ������ ������;

		return std::find(m_prohibiteditems.begin(), m_prohibiteditems.end(), eDrug) != m_prohibiteditems.end();
	}

	//! ��Ȱ�� �Ұ����� ���ΰ�?;
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
	// - 107 ���� ���� ������ ��� �̰ɷ� �ε��Ѵ�;
	//   ������(1.4����) mapslist.mst�� �ε��ϴ� ���� mapslist.lua�� �ε��ϰ� �����ϵ���;
	//   ������ ��µ� ������ ���� ������ ����� .mst���� �����Ͽ� ������ ����;
	// - ���� lua���Ϸ� ������(������ �߰� �� ����)�� �̷��� ��� ����üũ�� �ȵǾ�;
	//   ���۾��� �ʿ��ϴ� �κ��� ���ֱ� ���� �߰���;
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