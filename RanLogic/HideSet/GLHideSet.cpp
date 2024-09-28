#include "pch.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Memory/SmartPtr.h"

#include "../../EngineLib/G-Logic/GLogic.h"

#include "../GLogicDataMan.h"
#include "../GLogicFile.h"
#include "GLHideSet.h"

inline void RW_HideSet_General(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode);
inline void RW_HideSet_FieldMove(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode);
inline void RW_HideSet_Combat(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode);
inline void RW_HideSet_QBox(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode);
inline void RW_HideSet_Item(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode);
inline void RW_HideSet_Vehicle(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode);
inline void RW_HideSet_Confront(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode);
inline void RW_HideSet_Store(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode);
inline void RW_HideSet_NPC(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode);
inline void RW_HideSet_Deal(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode);
inline void RW_HideSet_Pet(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode);
inline void RW_HideSet_GuidanceCertify(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode);

namespace GLHIDESET
{
	bool bHideSet[EMHIDE_SIZE];
	float fInvisiblePerceiveAngle = 100.0f;
	float fInvisiblePerceiveRadius = 100.0f;
	float fInvisiblePerceiveURadius = 1.0f / 100.0f;
	float fInvisiblePerceiveLevel = 100.0f / 200.0f;
	float fInvisibleUpdateTime = 3.0f;

	const bool CheckHideSet(const EMHIDESET _IndexHideSet)
	{
		if ( _IndexHideSet < EMHIDE_SIZE )
			return GLHIDESET::bHideSet[_IndexHideSet];
		else
			return true;
	}
	HRESULT LoadFile(const std::string& _FileName, const bool _bPastLoad)
	{
		if ( _bPastLoad == false )
			return S_OK;

		sc::lua_init();

		std::string FullPath(GLOGIC::GetServerPath());
		FullPath.append("scripts\\");
		FullPath.append(_FileName);

		if ( sc::lua_doFile(FullPath) == false )
			return E_FAIL;

		LuaPlus::LuaObject _HideSettings = sc::lua_getGlobalFromName("HideSet_Settings");	

		fInvisiblePerceiveAngle = (float)::cos(_HideSettings.GetByName("fInvisiblePerceiveAngle").GetFloat() * (3.14159265 / 360.0f));		
		fInvisiblePerceiveRadius = _HideSettings.GetByName("fInvisiblePerceiveRadius").GetFloat();
		fInvisiblePerceiveURadius = 1.0f / fInvisiblePerceiveRadius;
		fInvisiblePerceiveLevel = 100.0f / _HideSettings.GetByName("fInvisiblePerceiveLevel").GetFloat();
		fInvisibleUpdateTime = _HideSettings.GetByName("fInvisibleUpdateTime").GetFloat();
		
		// 변수명 변경하면 안됨, 아래 매크로에서 변수 이름 사용;
		// 현재(2012.02.22) Lua파일을 순차적으로 읽지 못해 키 값 대조로 삽입;
		// LuaIterator 사용해 읽을 경우 순차가 아닌 정렬로 읽힘;
		LuaPlus::LuaObject _HideSetGroup = sc::lua_getGlobalFromName("HideSet_Option");
		LuaPlus::LuaObject _HideSetNode;			
			
		::RW_HideSet_General(_HideSetGroup, _HideSetNode); // 일반;
		::RW_HideSet_General(_HideSetGroup, _HideSetNode); // 일반;
		::RW_HideSet_FieldMove(_HideSetGroup, _HideSetNode); // 필드 이동;
		::RW_HideSet_Combat(_HideSetGroup, _HideSetNode); // 전투;
		::RW_HideSet_QBox(_HideSetGroup, _HideSetNode); // QBox;
		::RW_HideSet_Item(_HideSetGroup, _HideSetNode); // 아이템
		::RW_HideSet_Vehicle(_HideSetGroup, _HideSetNode); // 탈것;
		::RW_HideSet_Confront(_HideSetGroup, _HideSetNode); // 대련;		
		::RW_HideSet_Store(_HideSetGroup, _HideSetNode); // 개인 상점;
		::RW_HideSet_NPC(_HideSetGroup, _HideSetNode); // NPC;
		::RW_HideSet_Deal(_HideSetGroup, _HideSetNode); // 개인 거래;		
		::RW_HideSet_Pet(_HideSetGroup, _HideSetNode); // 펫;	
		::RW_HideSet_GuidanceCertify(_HideSetGroup, _HideSetNode);
		// 값 확인 디버깅 용;
//		bool _bHideSet[EMHIDE_SIZE];
//		::memcpy(_bHideSet, GLHIDESET::bHideSet, sizeof(bool) * EMHIDE_SIZE);

		return S_OK;
	}
};

#define READ_WRITE_HIDESET(a) \
	_HideSetNode = _HideSetGroup.Lookup(#a); \
	GLHIDESET::bHideSet[a] = _HideSetNode.GetBoolean();		

inline void RW_HideSet_General(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode){
	READ_WRITE_HIDESET(EMHIDE_PICKUP_ITEM)
	READ_WRITE_HIDESET(EMHIDE_GETON_VEHICLE)
	READ_WRITE_HIDESET(EMHIDE_GETOFF_VEHICLE)
	READ_WRITE_HIDESET(EMHIDE_DEATH)

	READ_WRITE_HIDESET(EMHIDE_UPDATE_EXP)
	READ_WRITE_HIDESET(EMHIDE_LEVELUP)
	READ_WRITE_HIDESET(EMHIDE_LEVELDOWN)
}

inline void RW_HideSet_FieldMove(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode){
	READ_WRITE_HIDESET(EMHIDE_TURN_WALK)
	READ_WRITE_HIDESET(EMHIDE_TURN_RUN)
	READ_WRITE_HIDESET(EMHIDE_ON_VEHICLE)
	READ_WRITE_HIDESET(EMHIDE_BUS)

	READ_WRITE_HIDESET(EMHIDE_TAXI)
}
inline void RW_HideSet_Combat(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode){
	READ_WRITE_HIDESET(EMHIDE_START_ATTACK)
	READ_WRITE_HIDESET(EMHIDE_USE_SKILL)
	READ_WRITE_HIDESET(EMHIDE_TAKE_BUFF)
	READ_WRITE_HIDESET(EMHIDE_TAKE_LIMIT)

	READ_WRITE_HIDESET(EMHIDE_TAKE_DEBUFF)
	READ_WRITE_HIDESET(EMHIDE_ON_HIT)
	READ_WRITE_HIDESET(EMHIDE_TAKE_DAMAGE_BY_DEBUFF)
	READ_WRITE_HIDESET(EMHIDE_AVOID)
}
inline void RW_HideSet_QBox(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode){
	READ_WRITE_HIDESET(EMHIDE_SPEEDUP)
	READ_WRITE_HIDESET(EMHIDE_SPEEDUP_MAX)
	READ_WRITE_HIDESET(EMHIDE_CRAZY)
	READ_WRITE_HIDESET(EMHIDE_MADNESS)

	READ_WRITE_HIDESET(EMHIDE_EXP_TIME)
	READ_WRITE_HIDESET(EMHIDE_EXP_GET)
	READ_WRITE_HIDESET(EMHIDE_POWERUP)
	READ_WRITE_HIDESET(EMHIDE_POWERUP_MAX)

	READ_WRITE_HIDESET(EMHIDE_BOMB)
	READ_WRITE_HIDESET(EMHIDE_HEAL)
	READ_WRITE_HIDESET(EMHIDE_LUCKY)
	READ_WRITE_HIDESET(EMHIDE_SUMMON)
}
inline void RW_HideSet_Item(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode){
	READ_WRITE_HIDESET(EMHIDE_USE_SKILLBOOK)
	READ_WRITE_HIDESET(EMHIDE_USE_ACCREDIT)
	READ_WRITE_HIDESET(EMHIDE_RESET_SKILL_STAT)
	READ_WRITE_HIDESET(EMHIDE_USE_GRINDER)

	READ_WRITE_HIDESET(EMHIDE_USE_INVEN_EXTENSION)
	READ_WRITE_HIDESET(EMHIDE_USE_STORAGE_EXTENSION)
	READ_WRITE_HIDESET(EMHIDE_USE_STORAGE_LINK)
	READ_WRITE_HIDESET(EMHIDE_USE_MARKET_GRANT)

	READ_WRITE_HIDESET(EMHIDE_USE_DISGUISE_SPLIT)
	READ_WRITE_HIDESET(EMHIDE_USE_DISGUISE_REMOVE)
	READ_WRITE_HIDESET(EMHIDE_USE_LOOK_CHANGE)
	READ_WRITE_HIDESET(EMHIDE_USE_PET_CHANGE)

	READ_WRITE_HIDESET(EMHIDE_USE_LUNCH)
	READ_WRITE_HIDESET(EMHIDE_USE_POINT_CHARGE)	
}
inline void RW_HideSet_Vehicle(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode){
	READ_WRITE_HIDESET(EMHIDE_REMODEL_VEHICLE)
}
inline void RW_HideSet_Confront(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode){	
	READ_WRITE_HIDESET(EMHIDE_BEING_SELF)
	READ_WRITE_HIDESET(EMHIDE_BEING_PARTY)
	READ_WRITE_HIDESET(EMHIDE_BEING_CLUB)
}
inline void RW_HideSet_Store(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode){
	READ_WRITE_HIDESET(EMHIDE_STORE_OPEN)
	READ_WRITE_HIDESET(EMHIDE_STORE_CLOSE)
}
inline void RW_HideSet_NPC(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode){
	READ_WRITE_HIDESET(EMHIDE_TALK_START)
	READ_WRITE_HIDESET(EMHIDE_TALK_END)
	READ_WRITE_HIDESET(EMHIDE_QUEST_ACCEPT)
	READ_WRITE_HIDESET(EMHIDE_QUEST_COMPLETE)
}
inline void RW_HideSet_Deal(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode){
	READ_WRITE_HIDESET(EMHIDE_DEAL_ACCEPT_SELF)
	READ_WRITE_HIDESET(EMHIDE_DEAL_ACCEPT_TARGET)
	READ_WRITE_HIDESET(EMHIDE_DEAL_REQUET_SEND)
	READ_WRITE_HIDESET(EMHIDE_DEAL_REQUEST_RECEIVE)
}
inline void RW_HideSet_Pet(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode){
	READ_WRITE_HIDESET(EMHIDE_PET_SUMMON)
	READ_WRITE_HIDESET(EMHIDE_PET_SUMMONNING)
	READ_WRITE_HIDESET(EMHIDE_PET_SUMMON_RELEASE)
	READ_WRITE_HIDESET(EMHIDE_PET_SKILL_LEARN)
}
inline void RW_HideSet_GuidanceCertify(LuaPlus::LuaObject& _HideSetGroup, LuaPlus::LuaObject& _HideSetNode)
{
	READ_WRITE_HIDESET(EMHIDE_GUIDANCE_CERTIFY_HIDECANCLE)
}
