#include "pch.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Item/GLItem.h"

#include "../../RanLogicServer/Util/GLItemLimit.h"

#include "../Database/DBAction/DbActionGameProvideRewardSystem.h"

#include "../FieldServer/GLGaeaServer.h"

#include "./GLProvideRewardManager.h"

namespace ProvideRewardSystem
{

	const DWORD GLProvideRewardManager::PROVIDE_REWARD_LIMIT_COUNT = 10;
	const float GLProvideRewardManager::PROVIDE_REWARD_CLEAR_TIME = 10.f;


	GLProvideRewardManager::GLProvideRewardManager ()
		: m_pServer ( NULL )
	{

	}


	GLProvideRewardManager::~GLProvideRewardManager ( )
	{

	}


	void GLProvideRewardManager::OnInitialize ()
	{
		if ( NULL == m_pServer )
		{
			sc::writeLogError ( "[ ProvideReward Log ] [ Server is NULL. ]" );

			return;
		}

		// 전장서버가 아닌경우에만 동작한다;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return;

		// DB 에서 Login상태로 남아있는 데이터를 신규로 변환;
		m_pServer->AddGameAdoJob( db::DbActionPtr(
			new CProvideRewardInitializeAction( ) ) );
	}


	void GLProvideRewardManager::OnLoginCharacter ( DWORD _dwChaDBNum )
	{
		if ( NULL == m_pServer )
		{
			sc::writeLogError ( "[ ProvideReward Log ] [ Server is NULL. ]" );

			return;
		}

		// 전장서버가 아닌경우에만 동작한다;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return;

		GLChar* pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
		{
			sc::writeLogError ( "[ ProvideReward Log ] [ Invalid Character DBNum. ]" );

			return;
		}

		// DB 에서 Select & DB Login Flag로 변경;
		m_pServer->AddGameAdoJob( db::DbActionPtr(
			new CProvideRewardSelectAction( _dwChaDBNum ) ) );
	}


	void GLProvideRewardManager::OnRegisterProvideReward (
		const PROVIDE_REWARD_DATA& _ProvideRewardData )
	{
		// 전장서버가 아닌경우에만 동작한다;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return;

		// 큐에 등록;
		m_queueProvideReward.push( _ProvideRewardData );
	}


	void GLProvideRewardManager::OnFrameMove ( float fElapsedTime )
	{
		if ( NULL == m_pServer )
		{
			sc::writeLogError ( "[ ProvideReward Log ] [ Server is NULL. ]" );

			return;
		}

		// 전장서버가 아닌경우에만 동작한다;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return;

		DWORD dwCount = 0;

		// 큐에 들어있는 포상 정보 지급 로직;
		while ( !m_queueProvideReward.empty() )
		{
			// 한 Frame에 PROVIDE_REWARD_LIMIT_COUNT 만큼만 돈다;
			// -> 무한 루프가 결코 일어나지 않는다;
			if ( dwCount >= PROVIDE_REWARD_LIMIT_COUNT )
				break;

			// 확인;
			PROVIDE_REWARD_DATA curProvideRewardData =
				m_queueProvideReward.front();

			// Pop;
			m_queueProvideReward.pop();

			// DB 완료 Flag로 변경;
			m_pServer->AddGameAdoJob( db::DbActionPtr(
				new CProvideRewardCompleteAction( curProvideRewardData.GetUniqueID() ) ) );

			// 포상 지급 ( 즉시 지급 로직으로 지급한다 );
			curProvideRewardData.SetProvideType(
				PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_IMMEDIATELY );
			ProvideReward ( curProvideRewardData );

			dwCount++;
		}

		// 완료 Flag 로 변경된 포상정보를 설정 시간마다 Clear 한다;
		static float fTime = 0.f;
		fTime += fElapsedTime;

		if ( fTime >= PROVIDE_REWARD_CLEAR_TIME )
		{
			// 일단 잠시 사용하지 않는다;
			m_pServer->AddGameAdoJob( db::DbActionPtr(
				new CProvideRewardClearDeleteFlagAction( ) ) );

			fTime = 0.f;
		}
	}


	void GLProvideRewardManager::ProvideReward (
		const PROVIDE_REWARD_DATA& _ProvideRewardData )
	{
		// 캐릭터 유효성 체크;
		if ( _ProvideRewardData.GetChaDBNum() == 0 )
		{
			sc::writeLogError ( "[ ProvideReward Log ] [ Invalid Character DBNum. ]" );

			return;
		}

		// 지급 방식 체크;
		switch ( _ProvideRewardData.GetProvideType() )
		{
		case PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_IMMEDIATELY:
			{
				// 전장서버가 아닌경우에만 동작한다;
				if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
					return;

				_ProvideRewardImmediately( _ProvideRewardData );
			}
			break;

		case PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_LOGIN:
			{
				// 전장서버에서만 동작한다;
				// 후에는 전장서버 외에서도 동작하도록 하면 좋다;
				if ( false == GLUseFeatures::GetInstance().IsUsingWorldBattle() )
					return;

				// 전장서버에서는 Login시 보상을 주는 로직만 사용 가능하다;
				_ProvideRewardLoginTime( _ProvideRewardData );
			}
			break;
		}
	}


	void GLProvideRewardManager::_ProvideRewardImmediately ( 
		const PROVIDE_REWARD_DATA& _ProvideRewardData )
	{
		if ( NULL == m_pServer )
		{
			sc::writeLogError ( "[ ProvideReward Log ] [ Server is NULL. ]" );

			return;
		}

		// 전장서버가 아닌경우에만 동작한다;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return;

		// 캐릭터 얻어오기;
		const GLChar* const pChar =
			m_pServer->GetCharByDbNum( _ProvideRewardData.GetChaDBNum() );
		if ( NULL == pChar )
		{
			sc::writeLogError ( "[ ProvideReward Log ] [ Invalid Character DBNum. ]" );

			return;
		}

		// 보상 타입 체크;
		DWORD dwAttatchedMoney = 0;
		SITEMCUSTOM sNewItem;

		switch ( _ProvideRewardData.GetRewardType() )
		{
		case PROVIDE_REWARD_DATA::EMREWARD_TYPE_MONEY:
			{
				// 포함할 돈;
				dwAttatchedMoney = _ProvideRewardData.GetRewardValue1();

				// 메일 보내기;
				m_pServer->PostSendSystemMail(
					pChar->GetCharID(),
					std::string(pChar->m_szName),
					SNATIVEID( false ).dwID,
					static_cast< LONGLONG >( dwAttatchedMoney ),
					ATTENDANCE_REWARD_POST_DESIGN,
					ATTENDANCE_REWARD_POST_SENDER,
					std::string( ID2SERVERTEXT( "SYSTEM_POST_SENDER_NAME" ) ),
					std::string( ID2SERVERTEXT( "SYSTEM_POST_SENDER_NAME" ) ),
					std::string( ID2SERVERTEXT( "SYSTEM_POST_SENDER_CONTENTS", _ProvideRewardData.GetTextIndex() ) )
					);
			}
			break;

		case PROVIDE_REWARD_DATA::EMREWARD_TYPE_ITEM:
			{
				DWORD dwItemMainID = _ProvideRewardData.GetRewardValue1();
				DWORD dwItemSubID = _ProvideRewardData.GetRewardValue2();
				DWORD dwItemCount = _ProvideRewardData.GetRewardValue3();

				// Item 생성;
				const SNATIVEID sItemID( dwItemMainID, dwItemSubID );
				m_pServer->NewItemMake( sNewItem, sItemID, EMGEN_SYSTEM );

				if ( sNewItem.GetNativeID() == SNATIVEID( false ) )
				{
					sc::writeLogError ( "[ ProvideReward Log ] [ Failed Make New Item. ]" );

					return;
				}
				
				// Item 이 유효한지 체크;
				const SITEM* pItem =
					GLogicData::GetInstance().GetItem(
					sNewItem.GetNativeID() );
				if ( NULL == pItem )
				{
					sc::writeLogError ( "[ ProvideReward Log ] [ Invalid Character DBNum. ]" );

					return;
				}

				// Item 의 최대 Pile 개수 체크;
				const int nMaxPileNum = pItem->sDrugOp.GetPileNum();

				// Item 의 개수 변경;
				sNewItem.TurnNumSet( min(
					nMaxPileNum,
					static_cast< int >( dwItemCount ? dwItemCount : 1 ) ) );

				sNewItem.GenerateBasicStat( false );
				sNewItem.GenerateLinkSkill();
				sNewItem.GenerateAddOption();

				m_pServer->GetItemLimit()->LogItemBasicStatOption( sNewItem );
				m_pServer->GetItemLimit()->LogItemLinkSkillOption( sNewItem );
				m_pServer->GetItemLimit()->LogItemAddonOption( sNewItem );

				// Item 랜덤옵션 최적화 및 Log;
				if ( sNewItem.GENERATE_RANDOM_OPT( false ) )
				{
					GLItemLimit* pItemLimit = m_pServer->GetItemLimit();
					pItemLimit->LogItemRandomOption( sNewItem );
				}

			}
			break;

		case PROVIDE_REWARD_DATA::EMREWARD_TYPE_MONEY_AND_ITEM:
			{
				DWORD dwItemMainID = _ProvideRewardData.GetRewardValue2();
				DWORD dwItemSubID = _ProvideRewardData.GetRewardValue3();
				DWORD dwItemCount = _ProvideRewardData.GetRewardValue4();

				// 포함할 돈;
				dwAttatchedMoney = _ProvideRewardData.GetRewardValue1();

				// Item 생성;
				const SNATIVEID sItemID( dwItemMainID, dwItemSubID );
				m_pServer->NewItemMake( sNewItem, sItemID, EMGEN_SYSTEM );

				if ( sNewItem.GetNativeID() == SNATIVEID( false ) )
				{
					sc::writeLogError ( "[ ProvideReward Log ] [ Failed Make New Item. ]" );

					return;
				}

				// Item 이 유효한지 체크;
				const SITEM* pItem =
					GLogicData::GetInstance().GetItem(
					sNewItem.GetNativeID() );
				if ( NULL == pItem )
				{
					sc::writeLogError ( "[ ProvideReward Log ] [ Invalid Item. ]" );

					return;
				}

				// Item 의 최대 Pile 개수 체크;
				const int nMaxPileNum = pItem->sDrugOp.GetPileNum();

				// Item 의 개수 변경;
				sNewItem.TurnNumSet( min(
					nMaxPileNum,
					static_cast< int >( dwItemCount ? dwItemCount : 1 ) ) );

				sNewItem.GenerateBasicStat( false );
				sNewItem.GenerateLinkSkill();
				sNewItem.GenerateAddOption();

				m_pServer->GetItemLimit()->LogItemBasicStatOption( sNewItem );
				m_pServer->GetItemLimit()->LogItemLinkSkillOption( sNewItem );
				m_pServer->GetItemLimit()->LogItemAddonOption( sNewItem );

				// Item 랜덤옵션 최적화 및 Log;
				if ( sNewItem.GENERATE_RANDOM_OPT( false ) )
				{
					GLItemLimit* pItemLimit = m_pServer->GetItemLimit();
					pItemLimit->LogItemRandomOption( sNewItem );
				}
			}
			break;
		}

		// 보상 메일 보내기;
		m_pServer->PostSendSystemMail(
			pChar->GetCharID(),
			std::string( pChar->m_szName ),
			sNewItem,
			static_cast< LONGLONG >( dwAttatchedMoney ),
			ATTENDANCE_REWARD_POST_DESIGN,
			ATTENDANCE_REWARD_POST_SENDER,
			std::string( ID2SERVERTEXT( "SYSTEM_POST_SENDER_NAME" ) ),
			std::string( ID2SERVERTEXT( "SYSTEM_POST_SENDER_NAME" ) ),
			std::string( ID2SERVERTEXT( "SYSTEM_POST_SENDER_CONTENTS", _ProvideRewardData.GetTextIndex() ) )
			);
	}


	void GLProvideRewardManager::_ProvideRewardLoginTime (
		const PROVIDE_REWARD_DATA& _ProvideRewardData )
	{
		if ( NULL == m_pServer )
		{
			sc::writeLogError ( "[ ProvideReward Log ] [ Server is NULL. ]" );

			return;
		}

		// 전장서버에서만 동작한다;
		// 후에는 전장서버 외에서도 동작하도록 하면 좋다;
		if ( false == GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return;

		// DB 신규 Flag로 등록;
		m_pServer->AddGameAdoJob( db::DbActionPtr(
			new CProvideRewardInsertAction( _ProvideRewardData ) ) );
	}

}