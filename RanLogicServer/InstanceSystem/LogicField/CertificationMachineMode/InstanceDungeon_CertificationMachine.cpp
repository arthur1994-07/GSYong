#include "pch.h"

#include "../../../../enginelib/GUInterface/GameTextControl.h"
#include "../../../../enginelib/GUInterface/UITextControl.h"

#include "../IInstanceFieldMode.h"

#include "./InstanceDungeon_CertificationMachine.h"

namespace InstanceSystem
{

	//int											TIME_ACCELERATOR_COOLTIME;
	//int											TIME_ACCELERATOR_NOTIFY_COOLTIME;
	//int											TIME_GAME_START_HELP_NOTIFY_TIME;
	//int											TIME_CERTIFY_POINT_TIME;
	//int											TIME_SAFE_TIME;
	//int											TIME_ACCELERATOR_BUFFTIME;
	//int											TIME_CERTIFY_WAITTIME;
	//int											TIME_CERTIFY_TIME;
	//int											TIME_GAME_DESTROY_WAITTIME;
	//int											TIME_RESURRECTION_WAITTIME;
	//int											TIME_JOIN_BUFFTIME;
	//int											TIME_JOIN_DEBUFFTIME;
	//int											TIME_CONFIRM_CERTIFICATION_TIME;
	//DWORD											NUM_TARGET_POINT;
	//DWORD											NUM_APPROACH_POINT;
	//DWORD											NUM_ONCE_LIMIT_POINT;
	//DWORD											NUM_MINIMUM_REWARD_POINT;
	//DWORD											NUM_PROGRESS_UI_INDEX;
	//std::string									STR_INSTANCE_DUNGEON_NAME;
	//SNATIVEID										MAP_LOBBY;
	//SNATIVEID										MAP_BATTLE;
	//SNATIVEID										MOTION_CERTIFY;
	//SNATIVEID										SKILL_CERTIFY_ADD_POINT_BUFF;
	//SNATIVEID										SKILL_RESURRECTION_TIME_REDUCE_BUFF;
	//SNATIVEID										SKILL_CERTIFY_TIME_REDUCE_BUFF;
	//VEC_DWORD										VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING;
	//VEC_NATIVEID									VEC_REMOVE_BUFF_BY_CERTIFYING;
	//VEC_NATIVEID									VEC_ACCELERATOR_BUFF;
	//VEC_NATIVEID									VEC_JOIN_BUFF;
	//VEC_NATIVEID									VEC_JOIN_DEBUFF;
	//VEC_REWARD_ITEM								VEC_REWARD_WIN;
	//VEC_REWARD_ITEM								VEC_REWARD_PARTICIPANT;
	//VEC_CERTIFICATION_INFO						VEC_CERTIFICATION_MACHINE_LIST;
	//VEC_ACCELERATOR_INFO							VEC_ACCELERATOR_LIST;
	const bool SCERTIFICATION_MACHINE_DUNGEON_INFO::LoadConfig (
		sc::LuaInstance& _refLuaInstance )
	{
		// Integer;
		{
			int nTempValue = 0;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "TIME_ACCELERATOR_COOLTIME", nTempValue ) )
				TIME_ACCELERATOR_COOLTIME = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "TIME_ACCELERATOR_NOTIFY_COOLTIME", nTempValue ) )
				TIME_ACCELERATOR_NOTIFY_COOLTIME = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "TIME_GAME_START_HELP_NOTIFY_TIME", nTempValue ) )
				TIME_GAME_START_HELP_NOTIFY_TIME = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "TIME_CERTIFY_POINT_TIME", nTempValue ) )
				TIME_CERTIFY_POINT_TIME = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "TIME_SAFE_TIME", nTempValue ) )
				TIME_SAFE_TIME = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "TIME_ACCELERATOR_BUFFTIME", nTempValue ) )
				TIME_ACCELERATOR_BUFFTIME = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "TIME_CERTIFY_WAITTIME", nTempValue ) )
				TIME_CERTIFY_WAITTIME = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "TIME_CERTIFICATION_MACHINE_LOCKTIME", nTempValue ) )
				TIME_CERTIFICATION_MACHINE_LOCKTIME = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "TIME_CERTIFY_TIME", nTempValue ) )
				TIME_CERTIFY_TIME = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "TIME_GAME_DESTROY_WAITTIME", nTempValue ) )
				TIME_GAME_DESTROY_WAITTIME = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "TIME_RESURRECTION_WAITTIME", nTempValue ) )
				TIME_RESURRECTION_WAITTIME = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "TIME_JOIN_BUFFTIME", nTempValue ) )
				TIME_JOIN_BUFFTIME = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "TIME_JOIN_DEBUFFTIME", nTempValue ) )
				TIME_JOIN_DEBUFFTIME = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "TIME_CONFIRM_CERTIFICATION_TIME", nTempValue ) )
				TIME_CONFIRM_CERTIFICATION_TIME = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "NUM_TARGET_POINT", nTempValue ) )
				NUM_TARGET_POINT = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "NUM_APPROACH_POINT", nTempValue ) )
				NUM_APPROACH_POINT = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "NUM_ONCE_LIMIT_POINT", nTempValue ) )
				NUM_ONCE_LIMIT_POINT = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "NUM_MINIMUM_REWARD_POINT", nTempValue ) )
				NUM_MINIMUM_REWARD_POINT = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "NUM_PROGRESS_UI_INDEX", nTempValue ) )
				NUM_PROGRESS_UI_INDEX = nTempValue;

			if ( _refLuaInstance.GetIntegerGlobalValue_Lua( "NUM_MINIMAP_ICON_TYPE", nTempValue ) )
				NUM_MINIMAP_ICON_TYPE = nTempValue;	
		}

		// string;
		{
			std::string strTempValue;
			if ( _refLuaInstance.GetStringGlobalValue_Lua( "STR_INSTANCE_DUNGEON_NAME", strTempValue ) )
				STR_INSTANCE_DUNGEON_NAME = strTempValue;
		}
		
		// SNATIVEID;
		{
			SNATIVEID sTempNativeID( false );

			if ( _refLuaInstance.IsTable( "MAP_LOBBY" ) )
			{
				LuaPlus::LuaObject luaObjectMapLobby = _refLuaInstance.GetGlobalValue_Lua( "MAP_LOBBY" );
				LuaPlus::LuaTableIterator iterMapLobby( luaObjectMapLobby );
				if ( iterMapLobby.IsValid() )
				{
					sTempNativeID.SetMid( iterMapLobby.GetValue().GetInteger() );
					iterMapLobby.Next();
					sTempNativeID.SetSid( iterMapLobby.GetValue().GetInteger() );
					iterMapLobby.Next();
					MAP_LOBBY = sTempNativeID;
				}
			}

			if ( _refLuaInstance.IsTable( "MAP_BATTLE" ) )
			{
				LuaPlus::LuaObject luaObjectMapBattle = _refLuaInstance.GetGlobalValue_Lua( "MAP_BATTLE" );
				LuaPlus::LuaTableIterator iterMapBattle( luaObjectMapBattle );
				if ( iterMapBattle.IsValid() )
				{
					sTempNativeID.SetMid( iterMapBattle.GetValue().GetInteger() );
					iterMapBattle.Next();
					sTempNativeID.SetSid( iterMapBattle.GetValue().GetInteger() );
					iterMapBattle.Next();
					MAP_BATTLE = sTempNativeID;
				}
			}

			if ( _refLuaInstance.IsTable( "MOTION_CERTIFY" ) )
			{
				LuaPlus::LuaObject luaObjectMotionCertify = _refLuaInstance.GetGlobalValue_Lua( "MOTION_CERTIFY" );
				LuaPlus::LuaTableIterator iterMotionCertify( luaObjectMotionCertify );
				if ( iterMotionCertify.IsValid() )
				{
					sTempNativeID.SetMid( iterMotionCertify.GetValue().GetInteger() );
					iterMotionCertify.Next();
					sTempNativeID.SetSid( iterMotionCertify.GetValue().GetInteger() );
					iterMotionCertify.Next();
					MOTION_CERTIFY = sTempNativeID;
				}
			}

			if ( _refLuaInstance.IsTable( "SKILL_CERTIFY_ADD_POINT_BUFF" ) )
			{
				LuaPlus::LuaObject luaObjectSkillCertifyAddPointBuff = _refLuaInstance.GetGlobalValue_Lua( "SKILL_CERTIFY_ADD_POINT_BUFF" );
				LuaPlus::LuaTableIterator iterSkillCertifyAddPointBuff( luaObjectSkillCertifyAddPointBuff );
				if ( iterSkillCertifyAddPointBuff.IsValid() )
				{
					sTempNativeID.SetMid( iterSkillCertifyAddPointBuff.GetValue().GetInteger() );
					iterSkillCertifyAddPointBuff.Next();
					sTempNativeID.SetSid( iterSkillCertifyAddPointBuff.GetValue().GetInteger() );
					iterSkillCertifyAddPointBuff.Next();
					SKILL_CERTIFY_ADD_POINT_BUFF = sTempNativeID;
				}
			}

			if ( _refLuaInstance.IsTable( "SKILL_RESURRECTION_TIME_REDUCE_BUFF" ) )
			{
				LuaPlus::LuaObject luaObjectSkillResurrectionTimeReduceBuff = _refLuaInstance.GetGlobalValue_Lua( "SKILL_RESURRECTION_TIME_REDUCE_BUFF" );
				LuaPlus::LuaTableIterator iterSkillResurrectionTimeReduceBuff( luaObjectSkillResurrectionTimeReduceBuff );
				if ( iterSkillResurrectionTimeReduceBuff.IsValid() )
				{
					sTempNativeID.SetMid( iterSkillResurrectionTimeReduceBuff.GetValue().GetInteger() );
					iterSkillResurrectionTimeReduceBuff.Next();
					sTempNativeID.SetSid( iterSkillResurrectionTimeReduceBuff.GetValue().GetInteger() );
					iterSkillResurrectionTimeReduceBuff.Next();
					SKILL_RESURRECTION_TIME_REDUCE_BUFF = sTempNativeID;
				}
			}

			if ( _refLuaInstance.IsTable( "SKILL_CERTIFY_TIME_REDUCE_BUFF" ) )
			{
				LuaPlus::LuaObject luaObjectSkillCertifyTimeReduceBuff = _refLuaInstance.GetGlobalValue_Lua( "SKILL_CERTIFY_TIME_REDUCE_BUFF" );
				LuaPlus::LuaTableIterator iterSkillCertifyTimeReduceBuff( luaObjectSkillCertifyTimeReduceBuff );
				if ( iterSkillCertifyTimeReduceBuff.IsValid() )
				{
					sTempNativeID.SetMid( iterSkillCertifyTimeReduceBuff.GetValue().GetInteger() );
					iterSkillCertifyTimeReduceBuff.Next();
					sTempNativeID.SetSid( iterSkillCertifyTimeReduceBuff.GetValue().GetInteger() );
					iterSkillCertifyTimeReduceBuff.Next();
					SKILL_CERTIFY_TIME_REDUCE_BUFF = sTempNativeID;
				}
			}
		}

		// VEC_DWORD;
		{
			if ( _refLuaInstance.IsTable( "VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING" ) )
			{
				LuaPlus::LuaObject luaObjectRemoveSkillEffectCertifying = _refLuaInstance.GetGlobalValue_Lua( "VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING" );
				LuaPlus::LuaTableIterator iterRemoveSkillEffectCertifying( luaObjectRemoveSkillEffectCertifying );
				for ( ; iterRemoveSkillEffectCertifying ; iterRemoveSkillEffectCertifying.Next() )
				{
					DWORD dwTempSkillEffect = 0;
					dwTempSkillEffect = iterRemoveSkillEffectCertifying.GetValue().GetInteger();

					VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING.push_back( dwTempSkillEffect );
				}
			}
		}

		// VEC_NATIVEID;
		{
			if ( _refLuaInstance.IsTable( "VEC_REMOVE_BUFF_BY_CERTIFYING" ) )
			{
				LuaPlus::LuaObject luaObjectRemoveBuffCertifying = _refLuaInstance.GetGlobalValue_Lua( "VEC_REMOVE_BUFF_BY_CERTIFYING" );
				LuaPlus::LuaTableIterator iterRemoveBuffCertifying( luaObjectRemoveBuffCertifying );
				for ( ; iterRemoveBuffCertifying ; iterRemoveBuffCertifying.Next() )
				{
					LuaPlus::LuaTableIterator iterNode( iterRemoveBuffCertifying.GetValue() );
					if ( iterNode.IsValid() )
					{
						SNATIVEID sTempNativeID( false );
						sTempNativeID.SetMid( iterNode.GetValue().GetInteger() );
						iterNode.Next();
						sTempNativeID.SetSid( iterNode.GetValue().GetInteger() );
						iterNode.Next();

						VEC_REMOVE_BUFF_BY_CERTIFYING.push_back( sTempNativeID );
					}
				}
			}

			if ( _refLuaInstance.IsTable( "VEC_ACCELERATOR_BUFF" ) )
			{
				LuaPlus::LuaObject luaObjectAcceleratorBuff = _refLuaInstance.GetGlobalValue_Lua( "VEC_ACCELERATOR_BUFF" );
				LuaPlus::LuaTableIterator iterAcceleratorBuff( luaObjectAcceleratorBuff );
				for ( ; iterAcceleratorBuff ; iterAcceleratorBuff.Next() )
				{
					LuaPlus::LuaTableIterator iterNode( iterAcceleratorBuff.GetValue() );
					if ( iterNode.IsValid() )
					{
						SNATIVEID sTempNativeID( false );
						sTempNativeID.SetMid( iterNode.GetValue().GetInteger() );
						iterNode.Next();
						sTempNativeID.SetSid( iterNode.GetValue().GetInteger() );
						iterNode.Next();

						VEC_ACCELERATOR_BUFF.push_back( sTempNativeID );
					}
				}
			}

			if ( _refLuaInstance.IsTable( "VEC_JOIN_BUFF" ) )
			{
				LuaPlus::LuaObject luaObjectJoinBuff = _refLuaInstance.GetGlobalValue_Lua( "VEC_JOIN_BUFF" );
				LuaPlus::LuaTableIterator iterJoinBuff( luaObjectJoinBuff );
				for ( ; iterJoinBuff ; iterJoinBuff.Next() )
				{
					LuaPlus::LuaTableIterator iterNode( iterJoinBuff.GetValue() );
					if ( iterNode.IsValid() )
					{
						SNATIVEID sTempNativeID( false );
						sTempNativeID.SetMid( iterNode.GetValue().GetInteger() );
						iterNode.Next();
						sTempNativeID.SetSid( iterNode.GetValue().GetInteger() );
						iterNode.Next();

						VEC_JOIN_BUFF.push_back( sTempNativeID );
					}
				}
			}

			if ( _refLuaInstance.IsTable( "VEC_JOIN_DEBUFF" ) )
			{
				LuaPlus::LuaObject luaObjectJoinDebuff = _refLuaInstance.GetGlobalValue_Lua( "VEC_JOIN_DEBUFF" );
				LuaPlus::LuaTableIterator iterJoinDebuff( luaObjectJoinDebuff );
				for ( ; iterJoinDebuff ; iterJoinDebuff.Next() )
				{
					LuaPlus::LuaTableIterator iterNode( iterJoinDebuff.GetValue() );
					if ( iterNode.IsValid() )
					{
						SNATIVEID sTempNativeID( false );
						sTempNativeID.SetMid( iterNode.GetValue().GetInteger() );
						iterNode.Next();
						sTempNativeID.SetSid( iterNode.GetValue().GetInteger() );
						iterNode.Next();

						VEC_JOIN_DEBUFF.push_back( sTempNativeID );
					}
				}
			}
		}

		// VEC_REWARD_ITEM
		{
			if ( _refLuaInstance.IsTable( "VEC_REWARD_WIN" ) )
			{
				LuaPlus::LuaObject luaObjectRewardWin = _refLuaInstance.GetGlobalValue_Lua( "VEC_REWARD_WIN" );
				LuaPlus::LuaTableIterator iterRewardWin( luaObjectRewardWin );
				for ( ; iterRewardWin ; iterRewardWin.Next() )
				{
					LuaPlus::LuaTableIterator iterNode( iterRewardWin.GetValue() );
					if ( iterNode.IsValid() )
					{
						SREWARD_ITEM sTempRewardItem;
						sTempRewardItem.ITEM_REWARD.SetMid( iterNode.GetValue().GetInteger() );
						iterNode.Next();
						sTempRewardItem.ITEM_REWARD.SetSid( iterNode.GetValue().GetInteger() );
						iterNode.Next();
						sTempRewardItem.NUM_ITEM_REWARD = iterNode.GetValue().GetInteger();
						iterNode.Next();
						sTempRewardItem.INDEX_POST_CONTENTS_STRING = iterNode.GetValue().GetInteger();
						iterNode.Next();

						VEC_REWARD_WIN.push_back( sTempRewardItem );
					}
				}
			}

			if ( _refLuaInstance.IsTable( "VEC_REWARD_PARTICIPANT" ) )
			{
				LuaPlus::LuaObject luaObjectRewardParticipant = _refLuaInstance.GetGlobalValue_Lua( "VEC_REWARD_PARTICIPANT" );
				LuaPlus::LuaTableIterator iterRewardParticipant( luaObjectRewardParticipant );
				for ( ; iterRewardParticipant ; iterRewardParticipant.Next() )
				{
					LuaPlus::LuaTableIterator iterNode( iterRewardParticipant.GetValue() );
					if ( iterNode.IsValid() )
					{
						SREWARD_ITEM sTempRewardItem;
						sTempRewardItem.ITEM_REWARD.SetMid( iterNode.GetValue().GetInteger() );
						iterNode.Next();
						sTempRewardItem.ITEM_REWARD.SetSid( iterNode.GetValue().GetInteger() );
						iterNode.Next();
						sTempRewardItem.NUM_ITEM_REWARD = iterNode.GetValue().GetInteger();
						iterNode.Next();
						sTempRewardItem.INDEX_POST_CONTENTS_STRING = iterNode.GetValue().GetInteger();
						iterNode.Next();
						sTempRewardItem.NUM_MINIMUM_POINT = iterNode.GetValue().GetInteger();
						iterNode.Next();
						sTempRewardItem.NUM_MAXIMUM_POINT = iterNode.GetValue().GetInteger();
						iterNode.Next();

						VEC_REWARD_PARTICIPANT.push_back( sTempRewardItem );
					}
				}
			}
		}

		// VEC_CERTIFICATION_INFO
		{
			if ( _refLuaInstance.IsTable( "VEC_CERTIFICATION_INFO" ) )
			{
				LuaPlus::LuaObject luaObjectCertificationInfo = _refLuaInstance.GetGlobalValue_Lua( "VEC_CERTIFICATION_INFO" );
				LuaPlus::LuaTableIterator iterCertificationInfo( luaObjectCertificationInfo );
				for ( ; iterCertificationInfo ; iterCertificationInfo.Next() )
				{
					LuaPlus::LuaObject luaObject( iterCertificationInfo.GetValue() );
					if ( false == luaObject.IsTable() )
						continue;
					
					SNATIVEID sTempIDCrow( false );
					SNATIVEID sTempIDMap( false );
					SNATIVEID sTempCertifyEffectBuff( false );
					D3DXVECTOR3 vTempPosition;
					float fTempRotate = 0.f;
					int nTempPoint = 0;
					int nTempPointCount = 0;
					int nTempAddPointCount = 0;
					std::string strTempCertificationEffect;
					std::string strTempUncertificationEffect;

					// ID_CROW;
					LuaPlus::LuaTableIterator iterIDCrow( luaObject.GetByName( "ID_CROW" ) );
					if ( iterIDCrow.IsValid() )
					{
						sTempIDCrow.SetMid( iterIDCrow.GetValue().GetInteger() );
						iterIDCrow.Next();
						sTempIDCrow.SetSid( iterIDCrow.GetValue().GetInteger() );
						iterIDCrow.Next();
					}

					// ID_MAP;
					LuaPlus::LuaTableIterator iterIDMap( luaObject.GetByName( "ID_MAP" ) );
					if ( iterIDMap.IsValid() )
					{
						sTempIDMap.SetMid( iterIDMap.GetValue().GetInteger() );
						iterIDMap.Next();
						sTempIDMap.SetSid( iterIDMap.GetValue().GetInteger() );
						iterIDMap.Next();
					}

					// ID_CERTIFY_EFFECT_BUFF;
					LuaPlus::LuaTableIterator iterCertifyEffectBuff( luaObject.GetByName( "ID_CERTIFY_EFFECT_BUFF" ) );
					if ( iterCertifyEffectBuff.IsValid() )
					{
						sTempCertifyEffectBuff.SetMid( iterCertifyEffectBuff.GetValue().GetInteger() );
						iterCertifyEffectBuff.Next();
						sTempCertifyEffectBuff.SetSid( iterCertifyEffectBuff.GetValue().GetInteger() );
						iterCertifyEffectBuff.Next();
					}

					// POSITION;
					LuaPlus::LuaTableIterator iterPosition( luaObject.GetByName( "POSITION" ) );
					if ( iterPosition.IsValid() )
					{
						vTempPosition.x = iterPosition.GetValue().GetNumber();
						iterPosition.Next();
						vTempPosition.y = iterPosition.GetValue().GetNumber();
						iterPosition.Next();
						vTempPosition.z = iterPosition.GetValue().GetNumber();
						iterPosition.Next();
					}

					_refLuaInstance.GetNumberGlobalValueObject_Lua(
						luaObject.GetByName( "ROTATE" ),
						fTempRotate );

					_refLuaInstance.GetIntegerGlobalValueObject_Lua(
						luaObject.GetByName( "NUM_POINT" ),
						nTempPoint );
					_refLuaInstance.GetIntegerGlobalValueObject_Lua(
						luaObject.GetByName( "NUM_POINT_COUNT" ),
						nTempPointCount );
					_refLuaInstance.GetIntegerGlobalValueObject_Lua(
						luaObject.GetByName( "NUM_ADD_POINT_COUNT" ),
						nTempAddPointCount );

					_refLuaInstance.GetStringGlobalValueObject_Lua(
						luaObject.GetByName( "STR_CERTIFICATION_EFFECT" ),
						strTempCertificationEffect );
					_refLuaInstance.GetStringGlobalValueObject_Lua(
						luaObject.GetByName( "STR_UNCERTIFICATION_EFFECT" ),
						strTempUncertificationEffect );

					SCERTIFICATION_MACHINE_INFO sTempInfo(
						sTempIDCrow,
						sTempIDMap,
						sTempCertifyEffectBuff,
						vTempPosition,
						fTempRotate,
						static_cast< DWORD >( nTempPoint ),
						static_cast< DWORD >( nTempPointCount ),
						static_cast< DWORD >( nTempAddPointCount ),
						strTempCertificationEffect,
						strTempUncertificationEffect );

					VEC_CERTIFICATION_MACHINE_LIST.push_back( sTempInfo );
				}
			}
		}

		// VEC_ACCELERATOR_INFO
		{
			if ( _refLuaInstance.IsTable( "VEC_ACCELERATOR_INFO" ) )
			{
				LuaPlus::LuaObject luaObjectAcceleratorInfo = _refLuaInstance.GetGlobalValue_Lua( "VEC_ACCELERATOR_INFO" );
				LuaPlus::LuaTableIterator iterAcceleratorInfo( luaObjectAcceleratorInfo );
				for ( ; iterAcceleratorInfo ; iterAcceleratorInfo.Next() )
				{
					LuaPlus::LuaObject luaObject( iterAcceleratorInfo.GetValue() );
					if ( false == luaObject.IsTable() )
						continue;

					SNATIVEID sTempIDCrow;
					SNATIVEID sTempIDEffect;
					SNATIVEID sTempIDMap;
					D3DXVECTOR3 vTempPosition;
					float fTempRotate = 0.f;
					int nTempPoint = 0;
					std::string strTempEffect1;
					std::string strTempEffect2;

					// ID_CROW;
					LuaPlus::LuaTableIterator iterIDCrow( luaObject.GetByName( "ID_CROW" ) );
					if ( iterIDCrow.IsValid() )
					{
						sTempIDCrow.SetMid( iterIDCrow.GetValue().GetInteger() );
						iterIDCrow.Next();
						sTempIDCrow.SetSid( iterIDCrow.GetValue().GetInteger() );
						iterIDCrow.Next();
					}

					// ID_EFFECT;
					LuaPlus::LuaTableIterator iterCertifyEffectBuff( luaObject.GetByName( "ID_EFFECT" ) );
					if ( iterCertifyEffectBuff.IsValid() )
					{
						sTempIDMap.SetMid( iterCertifyEffectBuff.GetValue().GetInteger() );
						iterCertifyEffectBuff.Next();
						sTempIDMap.SetSid( iterCertifyEffectBuff.GetValue().GetInteger() );
						iterCertifyEffectBuff.Next();
					}

					// ID_MAP;
					LuaPlus::LuaTableIterator iterIDMap( luaObject.GetByName( "ID_MAP" ) );
					if ( iterIDMap.IsValid() )
					{
						sTempIDEffect.SetMid( iterIDMap.GetValue().GetInteger() );
						iterIDMap.Next();
						sTempIDEffect.SetSid( iterIDMap.GetValue().GetInteger() );
						iterIDMap.Next();
					}

					// POSITION;
					LuaPlus::LuaTableIterator iterPosition( luaObject.GetByName( "POSITION" ) );
					if ( iterPosition.IsValid() )
					{
						vTempPosition.x = iterPosition.GetValue().GetNumber();
						iterPosition.Next();
						vTempPosition.y = iterPosition.GetValue().GetNumber();
						iterPosition.Next();
						vTempPosition.z = iterPosition.GetValue().GetNumber();
						iterPosition.Next();
					}

					_refLuaInstance.GetNumberGlobalValueObject_Lua(
						luaObject.GetByName( "ROTATE" ),
						fTempRotate );

					_refLuaInstance.GetIntegerGlobalValueObject_Lua(
						luaObject.GetByName( "NUM_POINT" ),
						nTempPoint );

					_refLuaInstance.GetStringGlobalValueObject_Lua(
						luaObject.GetByName( "STR_EFFECT1" ),
						strTempEffect1 );
					_refLuaInstance.GetStringGlobalValueObject_Lua(
						luaObject.GetByName( "STR_EFFECT2" ),
						strTempEffect2 );	

					SACCELERATOR_INFO sTempInfo(
						sTempIDCrow,
						sTempIDEffect,
						sTempIDMap,
						vTempPosition,
						fTempRotate,
						nTempPoint,
						strTempEffect1,
						strTempEffect2 );

					VEC_ACCELERATOR_LIST.push_back( sTempInfo );
				}
			}
		}

		return true;
	}




	const bool CALLBACK CInstanceDungeon_CertificationMachine::OnInitialize (
		const bool _bClubFaction )
	{
		if ( NULL == m_pInstanceField )
			return false;

		m_pInstanceField->SendCustomMessage(
			EM_MESSAGE_AGENT_BASE,
			m_pInstanceField->GetKeyMapID().Mid(),
			EMCUSTOM_MSG_CERTIFICATION_MODE_CREATEMAP_FA,
			m_pInstanceField->GetInstanceMapID() );

		m_bClubFaction = _bClubFaction;

		return true;
	}


	const bool CALLBACK CInstanceDungeon_CertificationMachine::OnJoin (
		const DWORD _dwChaDBNum,
		const DWORD _dwID )
	{
		// 캐릭터를 던전에 입장시킨다;
		m_sCharacterManager.Join( _dwChaDBNum, _dwID, m_bClubFaction );

		// 캐릭터의 입장을 Agent 에 알린다;
		m_pInstanceField->SendCustomMessage(
			EM_MESSAGE_AGENT_BASE,
			m_pInstanceField->GetKeyMapID().Mid(),
			CInstanceDungeon_CertificationMachine::EMCUSTOM_MSG_CERTIFICATION_MODE_JOIN_FA,
			_dwID );

		// 로그를 남긴다;
		m_pInstanceField->Log_CertificationMachineMode_In( _dwChaDBNum );

		sc::writeLogInfo ( sc::string::format(
			"[ %1% Log ] [ Join %1%, %2% ]",
			GetInstanceDungeonName(),
			_dwChaDBNum ) );

		return true;
	}


	const bool CALLBACK CInstanceDungeon_CertificationMachine::OnOut (
		const DWORD _dwChaDBNum )
	{
		// 캐릭터를 던전에서 퇴장시킨다;
		m_sCharacterManager.Out( _dwChaDBNum );

		const DWORD dwFactionID = m_sCharacterManager.GetCertifyingFactionID( _dwChaDBNum );
		m_pInstanceField->SendCustomMessage(
			EM_MESSAGE_AGENT_BASE,
			m_pInstanceField->GetKeyMapID().Mid(),
			CInstanceDungeon_CertificationMachine::EMCUSTOM_MSG_CERTIFICATION_MODE_OUT_FA,
			dwFactionID );

		// 미니맵에 저장된 정보를 삭제한다;
		GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC sNetMsg;
		sNetMsg.dwGuidanceMapID = MAP_LOBBY.dwID;
		sNetMsg.dwIconType = NUM_MINIMAP_ICON_TYPE;
		m_pInstanceField->UI_CertificationMachineMode_Minimap( _dwChaDBNum, &sNetMsg );

		sNetMsg.dwGuidanceMapID = MAP_BATTLE.dwID;
		sNetMsg.dwIconType = NUM_MINIMAP_ICON_TYPE;
		m_pInstanceField->UI_CertificationMachineMode_Minimap( _dwChaDBNum, &sNetMsg );

		// 로그를 남긴다;
		m_pInstanceField->Log_CertificationMachineMode_Out( _dwChaDBNum );

		sc::writeLogInfo ( sc::string::format(
			"[ %1% Log ] [ Out %1%, %2% ]",
			GetInstanceDungeonName(),
			_dwChaDBNum ) );

		return true;
	}


	const bool CALLBACK CInstanceDungeon_CertificationMachine::OnEnterMap (
		const DWORD _dwChaDBNum,
		const SNATIVEID _sEnterMapID )
	{
		if ( NULL == m_pInstanceField )
			return false;

		// 캐릭터를 맵에 입장시킨다;
		m_sCharacterManager.EnterMap( _dwChaDBNum, _sEnterMapID );

		// 진행 UI를 보여준다;
		m_pInstanceField->UI_CertificationMachineMode_Progress(
			NUM_PROGRESS_UI_INDEX,
			0,
			NUM_TARGET_POINT,
			GetDurationTime() );

		// 입장하는 맵이 전장맵이라면 처리한다;
		if ( _sEnterMapID == MAP_BATTLE )
		{
			if ( false == m_bInitializeBattleMap )
			{
				_InitializeBattleMap();
			}

			// 현재 미니맵 정보를 받아온다;
			GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC sNetMsg;
			sNetMsg.dwGuidanceMapID = MAP_BATTLE.dwID;
			sNetMsg.dwIconType = NUM_MINIMAP_ICON_TYPE;
			m_sCertificationMachineManager.SetMinimapInfo( MAP_BATTLE, &sNetMsg );
			m_sAcceleratorManager.SetMinimapInfo( MAP_BATTLE, &sNetMsg );

			m_pInstanceField->UI_CertificationMachineMode_Minimap( _dwChaDBNum, &sNetMsg );
		}
		// 입장하는 맵이 로비맵이라면 처리한다;
		else if ( _sEnterMapID == MAP_LOBBY )
		{
			// 로비맵 입장일 경우 미니맵에 저장된 정보를 삭제한다;
			GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC sNetMsg;
			sNetMsg.dwGuidanceMapID = MAP_LOBBY.dwID;
			sNetMsg.dwIconType = NUM_MINIMAP_ICON_TYPE;
			m_pInstanceField->UI_CertificationMachineMode_Minimap( _dwChaDBNum, &sNetMsg );
		}

		return true;
	}


	const bool CALLBACK CInstanceDungeon_CertificationMachine::OnStartCertification (
		const DWORD _dwMachineGaeaID,
		const DWORD _dwChaDBNum )
	{
		// 인증하려는 머신의 ID 를 구한다;
		const DWORD dwMachineID = m_sCertificationMachineManager.GetCertificationMachineID( _dwMachineGaeaID );
		if ( MAXDWORD == dwMachineID )
			return false;

		// 현재 인증하려는 머신을 인증하고 있는 진영을 구한다;
		const DWORD dwCapturedFactionID = m_sCertificationMachineManager.GetCapturedFaction( dwMachineID );

		// 인증하려는 머신이 인증 가능한 상태인지 확인한다;
		if ( false == m_sCertificationMachineManager.IsValidation( dwMachineID ) )
		{
			m_pInstanceField->RegistTimer(
				0,
				CertificationMachineMode::EMTIMER_ID_CERTIFY_FAIL_NOTIFY,
				_dwChaDBNum );

			return false;
		}

		// 인증을 시도한다;
		return m_sCharacterManager.StartCertification( _dwChaDBNum, dwMachineID, dwCapturedFactionID );
	}


	const bool CALLBACK CInstanceDungeon_CertificationMachine::OnEndCertification (
		const DWORD _dwChaDBNum,
		const DWORD _dwRemainMotionSec )
	{
		// 모션이 모두 이루어졌다면 인증을 성공시킨다;
		if ( _dwRemainMotionSec <= 0 )
		{
			const DWORD dwMachineID = m_sCharacterManager.GetCertifyingMachine( _dwChaDBNum );
			const DWORD dwFactionID = m_sCharacterManager.GetCertifyingFactionID( _dwChaDBNum );

			// 인증기를 인증한다;
			m_sCharacterManager.SuccessCertification( _dwChaDBNum );

			// 진영이 AddPoint 상태인지 확인한다;
			const bool bAddPoint = m_sCharacterManager.IsAddPointFaction( _dwChaDBNum );

			// 인증기가 이미 인증상태이면 인증상태 진영의 인증을 해제한다;
			const DWORD dwCapturedFaction = m_sCertificationMachineManager.GetCapturedFaction( dwMachineID );
			if ( MAXDWORD != dwCapturedFaction )
			{
				// 진영의 인증 정보를 중립화한다;
				m_sCharacterManager.CertificationMachineNeutralization( dwCapturedFaction, dwMachineID );
			}

			// 인증기를 인증상태로 변환한다;
			m_sCertificationMachineManager.CertificationMachine( dwMachineID, dwFactionID, _dwChaDBNum, bAddPoint );

			// 인증을 성공하면 미니맵을 갱신한다;
			GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC sLobbyNetMsg;
			sLobbyNetMsg.dwGuidanceMapID = MAP_LOBBY.dwID;
			sLobbyNetMsg.dwIconType = NUM_MINIMAP_ICON_TYPE;
			m_sCertificationMachineManager.SetMinimapInfo( MAP_LOBBY, &sLobbyNetMsg );
			m_sAcceleratorManager.SetMinimapInfo( MAP_LOBBY, &sLobbyNetMsg );

			m_pInstanceField->UI_CertificationMachineMode_Minimap_All( MAP_LOBBY, &sLobbyNetMsg );

			GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC sBattleNetMsg;
			sBattleNetMsg.dwGuidanceMapID = MAP_BATTLE.dwID;
			sBattleNetMsg.dwIconType = NUM_MINIMAP_ICON_TYPE;
			m_sCertificationMachineManager.SetMinimapInfo( MAP_BATTLE, &sBattleNetMsg );
			m_sAcceleratorManager.SetMinimapInfo( MAP_BATTLE, &sBattleNetMsg );
			
			m_pInstanceField->UI_CertificationMachineMode_Minimap_All( MAP_BATTLE, &sBattleNetMsg );

			// 인증 성공 타이머를 증록한다;
			m_pInstanceField->RegistTimer(
				0,
				CertificationMachineMode::EMTIMER_ID_CERTIFY_SUCCESS,
				dwFactionID,
				dwMachineID );
		}

		// 인증을 종료한다;
		m_sCharacterManager.EndCertification( _dwChaDBNum );

		return true;
	}


	const bool CALLBACK CInstanceDungeon_CertificationMachine::OnTimer ( 
		DWORD _dwHandle,
		DWORD _dwParam0,
		DWORD _dwParam1,
		DWORD _dwParam2,
		DWORD _dwParam3 )
	{
		switch ( _dwParam0 )
		{
		case CertificationMachineMode::EMTIMER_ID_GAME_DURATION:
			_GameEnd();
			break;

		case CertificationMachineMode::EMTIMER_ID_SAFETIME:
			_ReleaseSafeFaction( _dwParam1 );
			break;

		case CertificationMachineMode::EMTIMER_ID_DESTROY_WAITTIME:
			_DestroyInstanceDungeon();
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFY_POINT:
			_UpdateCertificationMachine();
			break;

		case CertificationMachineMode::EMTIMER_ID_TRY_CERTIFY_WAITTIME:
			_UnlockCertification( _dwParam1 );
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_LOCKTIME:
			_UnlockCertificationMachine( _dwParam1 );
			break;

		case CertificationMachineMode::EMTIMER_ID_ACCELERATOR_RECHARGE:
			_RechargeAccelerator();
			break;

		case CertificationMachineMode::EMTIMER_ID_ACCELERATOR_BUFFTIME:
			_ExitAcceleratorBuff( _dwParam1 );
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_NEUTRALIZATION:
			_CertificationMachineNeutralization( _dwParam1, _dwParam2 );
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_ACTIVATE_ADDPOINT:
			_CertificationMachineActivateAddPoint( _dwParam1 );
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_INACTIVATE_ADDPOINT:
			_CertificationMachineInactivateAddPoint( _dwParam1 );
			break;

		case CertificationMachineMode::EMTIMER_ID_REWARD_WINNER:
			_RewardWin( Faction::FactionID( Faction::EMFACTION_TEAM, _dwParam1 ) );
			_RewardWin( Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwParam1 ) );

			m_pInstanceField->SendCustomMessage(
				EM_MESSAGE_AGENT_BASE,
				m_pInstanceField->GetKeyMapID().Mid(),
				CInstanceDungeon_CertificationMachine::EMCUSTOM_MSG_CERTIFICATION_MODE_BATTLE_CLOSE_WIN_FA,
				m_pInstanceField->GetKeyMapID().Mid(),
				m_pInstanceField->GetKeyMapID().Sid(),
				_dwParam1 );
			break;

		case CertificationMachineMode::EMTIMER_ID_REWARD_PARTICIPANT:
			_RewardParticipant( Faction::FactionID( Faction::EMFACTION_TEAM, _dwParam1 ), _dwParam2 );
			_RewardParticipant( Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwParam1 ), _dwParam2 );
			break;

		case CertificationMachineMode::EMTIMER_ID_EXIT_JOIN_BUFFTIME:
			_ReleaseJoinBuff();
			break;

		case CertificationMachineMode::EMTIMER_ID_EXIT_JOIN_DEBUFFTIME:
			_ReleaseJoinDebuff();
			break;

		case CertificationMachineMode::EMTIMER_ID_CONFIRM_CERTIFICATION:
			_ConfirmCertification();
			break;

		case CertificationMachineMode::EMTIMER_ID_HELP_NOTIFY_1:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_GAMEHELP1, vecParameter );

				m_pInstanceField->RegistTimer( 5, CertificationMachineMode::EMTIMER_ID_HELP_NOTIFY_2 );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_HELP_NOTIFY_2:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_GAMEHELP2, vecParameter );

				m_pInstanceField->RegistTimer( 5, CertificationMachineMode::EMTIMER_ID_HELP_NOTIFY_3 );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_HELP_NOTIFY_3:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_GAMEHELP3, vecParameter );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_ACCELERATOR_CREATE_NOTIFY:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_ACCELERATOR_CREATE, vecParameter );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_ACCELERATOR_CAPTURE_NOTIFY:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam1 ) ) );
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_ACCELERATOR_CAPTURE, vecParameter );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_ACCELERATOR_CAPTURE_ADDPOINT_NOTIFY:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam1 ) ) );
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam2 ) ) );
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam3 ) ) );
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_ACCELERATOR_CAPTURE_ADDPOINT, vecParameter );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_ACCELERATOR_RECHARGE_NOTIFY:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam1 ) ) );
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_ACCELERATOR_RECHARGE, vecParameter );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFY_SUCCESS_NOTIFY:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam1 ) ) );
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam2 ) ) );
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam3 ) ) );
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_CERTIFY_SUCCESS, vecParameter );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFY_DUPLICATE_FAIL_NOTIFY:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam1 ) ) );
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_CERTIFY_DUPLICATE_FAIL, vecParameter );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_NEUTRALIZATION_WAIT_NOTIFY:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam1 ) ) );
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam2 ) ) );
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam3 ) ) );
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_CERTIFICATION_MACHINE_NEUTRALIZATION_WAIT, vecParameter );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_NEUTRALIZATION_NOTIFY:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam1 ) ) );
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam2 ) ) );
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_CERTIFICATION_MACHINE_NEUTRALIZATION, vecParameter );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFY_FAIL_NOTIFY:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam1 ) ) );
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_CERTIFY_FAIL, vecParameter );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFY_CAPTURED_FAIL_NOTIFY:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam1 ) ) );
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam2 ) ) );
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_CERTIFY_CAPTURED_FAIL, vecParameter );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_APPROACHED_GOAL_NOTIFY:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam1 ) ) );
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_APPROACHED_GOAL, vecParameter );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_EXIT_GAME_NOTIFY:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam1 ) ) );
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_EXIT_GAME, vecParameter );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_CONFIRM_CERTIFICATION_NOTIFY:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam1 ) ) );
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam2 ) ) );
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_CONFIRM_CERTIFICATION, vecParameter );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_TRY_CERTIFY_NOTIFY:
			{
				VEC_CALLBACK_PARAMETER vecParameter;
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( _dwParam1 ) ) );
				vecParameter.push_back( SCALLBACK_PARAMETER( static_cast< int >( m_sCertificationMachineManager.GetCertificationMachineGaeaID( _dwParam2 ) ) ) );
				m_pInstanceField->CallbackFunction( CertificationMachineMode::STR_CFUNCTION_TRY_CERTIFY, vecParameter );
			}
			break;
		}

		return true;
	}


	const bool CALLBACK CInstanceDungeon_CertificationMachine::OnDie (
		const Faction::GLActorID _sDieActorID,
		const Faction::GLActorID _sKillActorID )
	{
		if ( NULL == m_pInstanceField )
			return false;

		switch ( _sDieActorID.actorType )
		{
		case CROW_PC:
			{
				// 모션중이라면 모션을 해제한다;
				if ( m_pInstanceField->IsMotion( _sDieActorID, MOTION_CERTIFY ) )
				{
					m_pInstanceField->ResetMotion( _sDieActorID );
				}

				// 부활시간 감소 버프가 있다면 시간을 감소시킨다;
				int nResurrectionTime = TIME_RESURRECTION_WAITTIME;
				if ( m_pInstanceField->IsHaveSkillFact( _sDieActorID, SKILL_RESURRECTION_TIME_REDUCE_BUFF ) )
				{
					if ( nResurrectionTime > 0 )
						nResurrectionTime = nResurrectionTime / 2;
				}

				// 강제 부활 시킨다;
				m_pInstanceField->SetResurrectionForcedInstance(
					_sDieActorID.actorID_Num,
					MAP_LOBBY,
					0,
					nResurrectionTime,
					95 );
				m_pInstanceField->SetResurrectionDisableSkill( _sDieActorID.actorID_Num, true );
			}
			break;

		case CROW_NPC:
		case CROW_MOB:
			{
				const bool bCaptured = m_sAcceleratorManager.CaptureTheAccelerator( _sDieActorID.actorID_Num );
				if ( bCaptured )
				{
					m_sCharacterManager.CaptureAcceleratorBuff( _sKillActorID.actorID_Num );

					const DWORD dwFactionID = m_sCharacterManager.GetCertifyingFactionID( _sKillActorID.actorID_Num );

					// 가속기 파괴시 증가 포인트가 있다면 증가시킨다;
					const DWORD dwAcceleratorPoint = m_sAcceleratorManager.GetPointAccelerator( _sDieActorID.actorID_Num );
					if ( dwAcceleratorPoint > 0 )
					{
						m_sCharacterManager.UpdatePointDirect(
							dwFactionID,
							dwAcceleratorPoint );

						// 가속기 파괴 후 포인트가 증가됨을 공지하는 타이머를 등록한다;
						m_pInstanceField->RegistTimer(
							0,
							CertificationMachineMode::EMTIMER_ID_ACCELERATOR_CAPTURE_ADDPOINT_NOTIFY,
							dwFactionID,
							_sDieActorID.actorID_Num,
							dwAcceleratorPoint );
					}
					else
					{
						// 가속기 파괴만을 공지하는 타이머를 등록한다;
						m_pInstanceField->RegistTimer(
							0,
							CertificationMachineMode::EMTIMER_ID_ACCELERATOR_CAPTURE_NOTIFY,
							dwFactionID );
					}

					// 가속기의 경우 파괴되자마자 HP가 다 회복되므로 파괴시키지 않는다;
					return false;
				}
			}
			break;

		default:
			break;
		}

		return true;
	}


	const bool CALLBACK CInstanceDungeon_CertificationMachine::OnCustomMessage (
		const DWORD _dwChaDBNum,
		const DWORD _dwParam0,
		const DWORD _dwParam1,
		const DWORD _dwParam2,
		const DWORD _dwParam3 )
	{
		if ( NULL == m_pInstanceField )
			return false;

		switch ( _dwParam0 )
		{
		case EMCUSTOM_MSG_EXIT_BUTTON:
			m_pInstanceField->DoOut( _dwChaDBNum );
			break;

		case EMCUSTOM_MSG_CERTIFICATION_MODE_UPDATE_POINT:
			m_sCharacterManager.SyncronizeFactionInfo( _dwParam1, _dwParam2, _dwParam3 );
			break;

		case EMCUSTOM_MSG_CERTIFICATION_MODE_UPDATE_CERTIFYMACHINE:
			m_sCertificationMachineManager.Syncronize( _dwParam1, _dwParam2, _dwParam3 );
			m_sCharacterManager.SyncronizeCertificationMachine( _dwParam1, _dwParam2 );
			break;

		case EMCUSTOM_MSG_CERTIFICATION_MODE_UPDATE_PROGRESS_TIME:
			_Initialize( _dwParam1, _dwParam2, _dwParam3 );
			break;
		}

		return true;
	}




	const DWORD CInstanceDungeon_CertificationMachine::GetPointFaction (
		const DWORD _dwFactionID )
	{
		return m_sCharacterManager.GetPointFaction( _dwFactionID );
	}


	const DWORD CInstanceDungeon_CertificationMachine::GetDurationTime ( void )
	{
		if ( NULL == m_pInstanceField )
			return 0;

		return m_pInstanceField->GetLeftTime( m_hTimerGameDuration );
	}





	void CInstanceDungeon_CertificationMachine::_Initialize (
		const int _nProgressTime,
		const int _nAlltime,
		const int _nOpenCount )
	{
		if ( NULL == m_pInstanceField )
			return;

		m_dwAllTime = _nAlltime;

		// 최초 생성이 아닌경우 처리한다 ( 이어하기일 경우에 처리된다 );
		if ( _nOpenCount > 1 )
		{
			// 지나간 시간을 계산한다;
			int nPastTime = _nAlltime - _nProgressTime;

			// 진행된 시간을 계산한 후의 가속기 타이머를 등록한다;
			int nAcceleratorPastTime = nPastTime % TIME_ACCELERATOR_COOLTIME;
			int nTempAcceleratorCoolTime = TIME_ACCELERATOR_COOLTIME - nAcceleratorPastTime;
			m_pInstanceField->RegistTimer(
				nTempAcceleratorCoolTime,
				CertificationMachineMode::EMTIMER_ID_ACCELERATOR_RECHARGE );

			// 가속기 공지시간보다 많은 시간이 남았다면 남은시간에 맞게 가속기 공지 타이머를 등록한다;
			int nPreNotifyTime = TIME_ACCELERATOR_COOLTIME - TIME_ACCELERATOR_NOTIFY_COOLTIME;
			if ( nPreNotifyTime >= 0 && nPreNotifyTime <= nTempAcceleratorCoolTime )
			{
				int nTempAcceleratorNotifyCoolTime = nTempAcceleratorCoolTime - nPreNotifyTime;
				m_pInstanceField->RegistTimer(
					nTempAcceleratorNotifyCoolTime,
					CertificationMachineMode::EMTIMER_ID_ACCELERATOR_RECHARGE_NOTIFY,
					1 );
			}

			// 입장 버프 시간이 지나지 않았으면 입장 버프 처리를 해준다;
			if ( nPastTime < TIME_JOIN_BUFFTIME )
			{
				int nTempJoinBuffTime = TIME_JOIN_BUFFTIME - nPastTime;

				m_bJoinBuff = true;
				m_pInstanceField->RegistTimer(
					nTempJoinBuffTime,
					CertificationMachineMode::EMTIMER_ID_EXIT_JOIN_BUFFTIME );
			}

			// 입장 디버프 시간이 지나지 않았으면 입장 디버프 처리를 해준다;
			if ( nPastTime < TIME_JOIN_BUFFTIME )
			{
				int nTempJoinDebuffTime = TIME_JOIN_DEBUFFTIME - nPastTime;

				m_bJoinDebuff = true;
				m_pInstanceField->RegistTimer(
					nTempJoinDebuffTime,
					CertificationMachineMode::EMTIMER_ID_EXIT_JOIN_DEBUFFTIME );
			}
		}
		// 최초 생성일 경우 처리한다;
		else
		{
			// 최초 생성시에는 도움말 타이머를 등록한다;
			m_pInstanceField->RegistTimer(
				TIME_GAME_START_HELP_NOTIFY_TIME,
				CertificationMachineMode::EMTIMER_ID_HELP_NOTIFY_1 );

			// 가속기 생성 타이머를 등록한다;
			m_hTimerAcceleratorRecharge = m_pInstanceField->RegistTimer(
				TIME_ACCELERATOR_COOLTIME,
				CertificationMachineMode::EMTIMER_ID_ACCELERATOR_RECHARGE );

			// 가속기 공지 타이머를 등록한다;
			m_pInstanceField->RegistTimer(
				TIME_ACCELERATOR_NOTIFY_COOLTIME,
				CertificationMachineMode::EMTIMER_ID_ACCELERATOR_RECHARGE_NOTIFY,
				1 );

			// 입장 버프, 디버프 상태를 활성화한다;
			m_bJoinBuff = true;
			m_bJoinDebuff = true;

			// 입장 버프, 디버프 상태 해제 타이머를 등록한다;
			m_pInstanceField->RegistTimer(
				TIME_JOIN_BUFFTIME,
				CertificationMachineMode::EMTIMER_ID_EXIT_JOIN_BUFFTIME );

			m_pInstanceField->RegistTimer(
				TIME_JOIN_BUFFTIME,
				CertificationMachineMode::EMTIMER_ID_EXIT_JOIN_DEBUFFTIME );
		}

		// 포인트 갱신 주기 타이머를 등록한다;
		m_hTimerCertifyPoint = m_pInstanceField->RegistTimer(
			0,
			CertificationMachineMode::EMTIMER_ID_CERTIFY_POINT );

		// 인증 확인 주기 타이머를 등록한다;
		m_pInstanceField->RegistTimer(
			TIME_CONFIRM_CERTIFICATION_TIME,
			CertificationMachineMode::EMTIMER_ID_CONFIRM_CERTIFICATION );

		// 게임 종료 공지 타이머를 등록한다 ( 끝나기 5분전부터 1분단위로 공지한다 );
		for ( int i=1; i<=5; ++i )
		{
			m_pInstanceField->RegistTimer(
				_nProgressTime - i*60,
				CertificationMachineMode::EMTIMER_ID_EXIT_GAME_NOTIFY,
				i );
		}

		// 게임 진행 타이머를 등록한다;
		m_hTimerGameDuration = m_pInstanceField->RegistTimer(
			_nProgressTime,
			CertificationMachineMode::EMTIMER_ID_GAME_DURATION );

		// HP 를 보여준다;
		m_pInstanceField->SetHPVisible( true );

		// 진영채팅을 활성화한다;
		m_pInstanceField->ActiveFactionChatting();

		// 초기화를 완료한다;
		m_bInitialize = true;
		m_bPlay = true;
	}


	void CInstanceDungeon_CertificationMachine::_InitializeBattleMap ( void )
	{
		if ( NULL == m_pInstanceField )
			return;

		// 모든 인증기를 생성한다;
		m_sCertificationMachineManager.CreateAllCertificationMachine();

		// 모든 가속기를 생성한다;
		m_sAcceleratorManager.CreateAllAccelerator();

		// 초기화를 완료한다;
		m_bInitializeBattleMap = true;
	}


	void CInstanceDungeon_CertificationMachine::_GameEnd ( void )
	{
		if ( NULL == m_pInstanceField )
			return;

		m_bPlay = false;

		// 모든 타이머를 삭제한다;
		m_pInstanceField->RemoveAllTimer();

		m_sCharacterManager.GameEnd();
		const bool bReward = m_sCharacterManager.GameReward();

		// 인던 파괴 타이머를 등록한다;
		m_pInstanceField->RegistTimer(
			TIME_GAME_DESTROY_WAITTIME,
			CertificationMachineMode::EMTIMER_ID_DESTROY_WAITTIME );

		// 진행 UI를 종료상태로 변경한다;
		m_pInstanceField->UI_CertificationMachineMode_Progress(
			NUM_PROGRESS_UI_INDEX,
			1,
			NUM_TARGET_POINT,
			TIME_GAME_DESTROY_WAITTIME );

		if ( m_sCharacterManager.IsEmpty() ||
			false == bReward )
		{
			m_pInstanceField->SendCustomMessage(
				EM_MESSAGE_AGENT_BASE,
				m_pInstanceField->GetKeyMapID().Mid(),
				CInstanceDungeon_CertificationMachine::EMCUSTOM_MSG_CERTIFICATION_MODE_BATTLE_CLOSE_FA );
		}
	}


	void CInstanceDungeon_CertificationMachine::_DestroyInstanceDungeon ( void )
	{
		if ( NULL == m_pInstanceField )
			return;

		m_pInstanceField->DoDestroy();
	}


	void CInstanceDungeon_CertificationMachine::_ReleaseSafeFaction (
		const DWORD _dwChaDBNum )
	{
		m_sCharacterManager.ReleaseSafeFaction( _dwChaDBNum );
	}


	void CInstanceDungeon_CertificationMachine::_UnlockCertification (
		const DWORD _dwChaDBNum )
	{
		m_sCharacterManager.UnlockCertification( _dwChaDBNum );
	}


	void CInstanceDungeon_CertificationMachine::_UnlockCertificationMachine ( const DWORD _dwMachineID )
	{
		m_sCertificationMachineManager.ActivateMachine( _dwMachineID );
	}


	void CInstanceDungeon_CertificationMachine::_UpdateCertificationMachine ( void )
	{
		if ( NULL == m_pInstanceField )
			return;

		// 포인트를 갱신한다;
		CertificationMachineMode::MAP_FACTION_POINT mapFactionPoint;
		m_sCertificationMachineManager.GenerateFactionPoint( mapFactionPoint );

		// 각 진영별 Point 를 갱신한다;
		if ( m_sCharacterManager.UpdatePoint( mapFactionPoint ) )
		{
			// 목표점수가 달성되었을 게임을 종료시킨다;
			_GameEnd();
		}
		else
		{
			// 포인트 갱신 주기 타이머를 등록한다;
			m_hTimerCertifyPoint = m_pInstanceField->RegistTimer(
				TIME_CERTIFY_POINT_TIME,
				CertificationMachineMode::EMTIMER_ID_CERTIFY_POINT );
		}

		// Agent 에 이어하기를 대비하여 동기화한다;
		m_sCharacterManager.SendSyncronizeMessage();
		m_sCertificationMachineManager.SendSyncronizeMessage();

		m_pInstanceField->SendCustomMessage(
			EM_MESSAGE_AGENT_BASE,
			m_pInstanceField->GetKeyMapID().Mid(),
			CInstanceDungeon_CertificationMachine::EMCUSTOM_MSG_CERTIFICATION_MODE_UPDATE_PROGRESS_TIME,
			GetDurationTime() );

		// 포인트를 갱신하는 시점에 미니맵을 갱신한다;
		GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC sLobbyNetMsg;
		sLobbyNetMsg.dwGuidanceMapID = MAP_LOBBY.dwID;
		sLobbyNetMsg.dwIconType = NUM_MINIMAP_ICON_TYPE;
		m_sCertificationMachineManager.SetMinimapInfo( MAP_LOBBY, &sLobbyNetMsg );
		m_sAcceleratorManager.SetMinimapInfo( MAP_LOBBY, &sLobbyNetMsg );

		m_pInstanceField->UI_CertificationMachineMode_Minimap_All( MAP_LOBBY, &sLobbyNetMsg );

		GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC sBattleNetMsg;
		sBattleNetMsg.dwGuidanceMapID = MAP_BATTLE.dwID;
		sBattleNetMsg.dwIconType = NUM_MINIMAP_ICON_TYPE;
		m_sCertificationMachineManager.SetMinimapInfo( MAP_BATTLE, &sBattleNetMsg );
		m_sAcceleratorManager.SetMinimapInfo( MAP_BATTLE, &sBattleNetMsg );

		m_pInstanceField->UI_CertificationMachineMode_Minimap_All( MAP_BATTLE, &sBattleNetMsg );
	}


	void CInstanceDungeon_CertificationMachine::_RechargeAccelerator ( void )
	{
		m_sAcceleratorManager.ActivateAllAccelerator();

		// 가속기 생성 공지 타이머를 등록한다;
		m_pInstanceField->RegistTimer(
			0,
			CertificationMachineMode::EMTIMER_ID_ACCELERATOR_CREATE_NOTIFY );

		// 가속기 재생성 타이머를 등록한다;
		m_hTimerAcceleratorRecharge = m_pInstanceField->RegistTimer(
			TIME_ACCELERATOR_COOLTIME,
			CertificationMachineMode::EMTIMER_ID_ACCELERATOR_RECHARGE );

		// 가속기 재생성 1분전 공지 타이머를 등록한다;
		m_pInstanceField->RegistTimer(
			TIME_ACCELERATOR_NOTIFY_COOLTIME,
			CertificationMachineMode::EMTIMER_ID_ACCELERATOR_RECHARGE_NOTIFY,
			1 );
	}


	void CInstanceDungeon_CertificationMachine::_ExitAcceleratorBuff (
		const DWORD _dwFactionID )
	{
		m_sCharacterManager.ExitAcceleratorBuff( _dwFactionID );
	}


	void CInstanceDungeon_CertificationMachine::_CertificationMachineNeutralization (
		const DWORD _dwFactionID,
		const DWORD _dwMachineID )
	{
		m_sCharacterManager.CertificationMachineNeutralization( _dwFactionID, _dwMachineID );
	}


	void CInstanceDungeon_CertificationMachine::_CertificationMachineActivateAddPoint ( const DWORD _dwMachineID )
	{
		m_sCertificationMachineManager.ActivateAddPoint( _dwMachineID );
	}


	void CInstanceDungeon_CertificationMachine::_CertificationMachineInactivateAddPoint ( const DWORD _dwMachineID )
	{
		m_sCertificationMachineManager.InactivateAddPoint( _dwMachineID );
	}


	void CInstanceDungeon_CertificationMachine::_RewardWin (
		const Faction::FactionID& _sFactionID )
	{
		for ( int i=0; i<VEC_REWARD_WIN.size(); ++i )
		{
			const SREWARD_ITEM sRewardItem = VEC_REWARD_WIN.at( i );

			m_pInstanceField->SendSystemMailFaction(
				_sFactionID,
				ID2SERVERTEXT( "SYSTEM_POST_SENDER_NAME", 0 ),
				ID2SERVERTEXT( "SYSTEM_POST_SENDER_CONTENTS", sRewardItem.INDEX_POST_CONTENTS_STRING ),
				0,
				sRewardItem.ITEM_REWARD,
				sRewardItem.NUM_ITEM_REWARD,
				sRewardItem.INDEX_POST_CONTENTS_STRING );
		}
	}


	void CInstanceDungeon_CertificationMachine::_RewardParticipant (
		const Faction::FactionID& _sFactionID,
		const DWORD _dwPoint )
	{
		for ( int i=0; i<VEC_REWARD_PARTICIPANT.size(); ++i )
		{
			const SREWARD_ITEM sRewardItem = VEC_REWARD_PARTICIPANT.at( i );
			if ( _dwPoint < sRewardItem.NUM_MINIMUM_POINT || sRewardItem.NUM_MAXIMUM_POINT < _dwPoint )
				continue;

			m_pInstanceField->SendSystemMailFaction(
				_sFactionID,
				ID2SERVERTEXT( "SYSTEM_POST_SENDER_NAME", 0 ),
				ID2SERVERTEXT( "SYSTEM_POST_SENDER_CONTENTS", sRewardItem.INDEX_POST_CONTENTS_STRING ),
				0,
				sRewardItem.ITEM_REWARD,
				sRewardItem.NUM_ITEM_REWARD,
				sRewardItem.INDEX_POST_CONTENTS_STRING );
		}
	}


	void CInstanceDungeon_CertificationMachine::_ReleaseJoinBuff ( void )
	{
		m_bJoinBuff = false;
	}


	void CInstanceDungeon_CertificationMachine::_ReleaseJoinDebuff ( void )
	{
		m_bJoinDebuff = false;
	}


	void CInstanceDungeon_CertificationMachine::_ConfirmCertification ( void )
	{
		m_sCertificationMachineManager.ConfirmCertification();

		// 인증 확인 주기 타이머를 등록한다;
		m_pInstanceField->RegistTimer(
			TIME_CONFIRM_CERTIFICATION_TIME,
			CertificationMachineMode::EMTIMER_ID_CONFIRM_CERTIFICATION );
	}

}