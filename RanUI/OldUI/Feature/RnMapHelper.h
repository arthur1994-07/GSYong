
#pragma once



class GLMobScheduleMan;
class GLLevelFileClient;
class GLMobSchedule;
class CUIControl;
class GLGaeaClient;
struct SCROWDATA;

namespace RnMapPrivate
{
	class ISettingInterface
	{
	public:
		virtual ~ISettingInterface() {}

		virtual bool IsVisibleGate() = 0; 
		virtual bool IsVisibleNormalNpc() = 0; 
		virtual bool IsVisiblePlaceName() = 0; 
		virtual bool IsVisibleBusStation() = 0; 
		virtual bool IsVisibleLockerManager() = 0; 
		virtual bool IsVisibleStartPosition() = 0; 
		virtual bool IsVisibleClubManager() = 0; 
// 		virtual bool IsVisibleItemSearch() = 0; 
// 		virtual bool IsVisibleItemMix() = 0; 
		virtual bool IsVisiblePostBox() = 0;
		virtual bool IsVisiblePartyMember() = 0;

		virtual bool IsAutoRideVehicle() = 0;
	};
	
	class IMapInterface
	{
	public:
		virtual ~IMapInterface() {}

		virtual void		SetMapLevelFile(DWORD mapID) = 0;
		virtual std::string GetMapName(DWORD mapID) = 0;
		virtual DWORD		GetSelectMap() = 0;
	};


	class IMapInfoInterface
	{
	public:
		virtual ~IMapInfoInterface() {}

		virtual bool IsVisibleMonster(DWORD monsterID) = 0;
	};


	void GetQuestFromMonsterID(GLGaeaClient* pGaeaClient,DWORD monsterID,std::vector<DWORD>* pQuestIdVec );

	enum MONSTER_TYPE{MONSTER_TYPE_QUEST,MONSTER_TYPE_BOSS,MONSTER_TYPE_VERY_WEAK,MONSTER_TYPE_WEAK,MONSTER_TYPE_NORMAL,MONSTER_TYPE_STRONG};
	MONSTER_TYPE GetMonsterStrongType(GLGaeaClient* pGaeaClient,SCROWDATA* pCrowData);
	MONSTER_TYPE GetMonsterIconType(GLGaeaClient* pGaeaClient,SCROWDATA* pCrowData);
	void ChangeTexture(CUIControl* pControl,MONSTER_TYPE monsterType);



	class IMapSearchInterface
	{
	public:
		virtual ~IMapSearchInterface() {}
		
		virtual bool GetSearchedCharacter(int* pLevel,std::string* pName,DWORD* pMapID,D3DXVECTOR3* pPos) = 0;
	};
}
