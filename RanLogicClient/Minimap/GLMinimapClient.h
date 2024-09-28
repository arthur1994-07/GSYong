#pragma once

#include "../../RanLogic/Land/GLMapAxisInfo.h"
#include "../Widget/GLWidgetScript.h"

class GLLevelFileClient;
class GLMobSchedule;
struct SCROWDATA;

class GLMinimapClient
{
private :
	GLMinimapClient(void);
	GLMinimapClient( const GLMinimapClient& value );
	~GLMinimapClient(void);

public :
	static GLMinimapClient& GetInstance();

private :
	enum QUEST_TYPE
	{
		QUEST_START,
		QUEST_STEP,
		QUEST_FINISH
	};

	enum MONSTER_TYPE
	{
		/*
		MONSTER_TYPE_QUEST,
		MONSTER_TYPE_BOSS,
		MONSTER_TYPE_VERY_WEAK,
		MONSTER_TYPE_WEAK,
		MONSTER_TYPE_NORMAL,
		MONSTER_TYPE_STRONG
		*/
		MONSTER_TYPE_VERY_WEAK = 0, //LowerWord
		MONSTER_TYPE_WEAK,
		MONSTER_TYPE_NORMAL,
		MONSTER_TYPE_STRONG,

		MONSTER_TYPE_QUEST = 0x10000,	// HiWord	>> 16 (1)
		MONSTER_TYPE_BOSS = 0x20000,	//			>> 16 (2)
	};

	enum TRADE 
	{
		NO_VISIBLE_TRADE,
		NO_TRADE,
		CAN_TRADE
	};

private :
	GLLevelFileClient*	m_pMinimap;
	
public :
	LuaTable GetMapList();

	LuaTable ImageToMap( DWORD dwMapID, int nPosX, int nPosY );
	LuaTable PositionToMap( DWORD dwMapID, float fPosX, float fPosY, float fPosZ );
	std::string GetMapImgFile( DWORD dwMapID );

	LuaTable GetMapZoneName( DWORD dwMapID );
	LuaTable GetMapGateName( DWORD dwMapID );
	LuaTable GetMapNormalNpc( DWORD dwMapID );
	LuaTable GetMapPost( DWORD dwMapID );
	LuaTable GetMapTalkNpc( DWORD dwMapID, int nTalkType );
	LuaTable GetMapMob( DWORD dwMapID );
	LuaTable GetMapMobList( DWORD dwMapID );
	LuaTable GetMobGenItemList( LuaTable tbMobID );
	LuaTable GetAuthenticatorInfo( DWORD dwMapID );
	LuaTable GetAllianceInfoList( DWORD dwMapID );
	LuaTable GetLevelCondition( DWORD dwMapID );

public :
	void ConvertWorldToMap( const GLMapAxisInfo& sMapAxisInfo, const D3DXVECTOR3& vPosition, int& nPosX, int& nPosY );
	void ConvertImgMapToWorld( const GLMapAxisInfo& sMapAxisInfo, int nPosX, int nPosY, D3DXVECTOR3& vPosition );
	void ConvertImgToMapPos( const GLMapAxisInfo& sMapAxisInfo, int nPosX, int nPosY, int& nMapPosX, int& nMapPosY );

	GLLevelFileClient* GetLevelClient( DWORD dwMapID );

private :
	bool haveInvenItem( const SCROWDATA* pCrowData );

	MONSTER_TYPE GetMobStrongType( SCROWDATA* pCrowData );

	void DeleteSameName( const std::vector< std::string >& vecName, std::vector< GLMobSchedule* >& vecCrow );

	void GetCrowDataFromNpcType( const GLLevelFileClient* pLevelFile, 
		int nNpcType, std::vector< GLMobSchedule* >& vecCrow );
	void GetCrowDataFromQuest( const GLLevelFileClient* pLevelFile, QUEST_TYPE questType,
		std::vector< GLMobSchedule* >& vecQuestNpc,
		std::vector< std::string >* pvecNpcName = NULL );
	void GetCrowDataFromNpcTalk( const GLLevelFileClient* pLevelFile, int m_talkType,
		std::vector< GLMobSchedule* >& vecCrow );
	void GetCrowDataFromNpcInven( const GLLevelFileClient* pLevelFile,
		std::vector< GLMobSchedule* >& vecCrow,
		std::vector< std::string >* pvecQuestNpc = NULL );
	void GetMobData( const GLLevelFileClient* pLevelFile, std::vector< GLMobSchedule* >& vecCrow );

	void SetDataInTable( const GLLevelFileClient* pLevelFile, LuaTable& tbData,
		std::vector< GLMobSchedule* > vecCrow, int* pnTableIndex = NULL );
	void SetQuestDataInTable( const GLLevelFileClient* pLevelFile, LuaTable& tbData,
		std::vector< GLMobSchedule* > vecCrow, QUEST_TYPE questType,
		int* pnTableIndex = NULL );
	void SetMarketDataInTable( const GLLevelFileClient* pLevelFile, LuaTable& tbData,
		std::vector< GLMobSchedule* > vecCrow, int* pnTableIndex = NULL );
	// SetMobDataInTable에서 낱개로 보내는 몹정보와 SfLargemap_MakeTooltip_Mob와 동일하게 사용된다; 
	void SetMobDataInTable( const GLLevelFileClient* pLevelFile, LuaTable& tbData,
		std::vector< GLMobSchedule* > vecCrow );

public:
	std::string SfLargemap_MakeTooltip(const EMCROW nCrowType, const INT nCrowSubType, const DWORD dwID, const DWORD dwMapID);

private:
	BOOL SfLargemap_MakeTooltip_Char(std::string& strResult, const DWORD dwID, const DWORD dwMapID);
	BOOL SfLargemap_MakeTooltip_StartPoint(std::string& strResult, const EMCROW nCrowType, const INT nSubType, const DWORD dwID, const DWORD dwMapID);
	BOOL SfLargemap_MakeTooltip_NormalNpc(std::string& strResult, const DWORD dwID, const DWORD dwMapID);
	BOOL SfLargemap_MakeTooltip_Gate(std::string& strResult, const INT nSubType, const DWORD dwID, const DWORD dwMapID);
	// SetMobDataInTable에서 낱개로 보내는 몹정보와 SfLargemap_MakeTooltip_Mob와 동일하게 사용된다; 
	BOOL SfLargemap_MakeTooltip_Mob(std::string& strResult, const DWORD dwNativeID );

	BOOL SfLargemap_MakeTooltip_NPCDefaultTooltip(std::string& strResult, const DWORD dwID, std::string strAdditionalExplan);
	void SfGetCrowDataFromQuest( QUEST_TYPE questType
		, std::vector< std::string >* pvecNpcName
		, DWORD dwCrowID );
	BOOL IsCrowDataFromNpcType(DWORD dwCrowID, int nNpcType);
};
