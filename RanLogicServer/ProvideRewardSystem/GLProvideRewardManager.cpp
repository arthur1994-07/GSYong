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

		// ���弭���� �ƴѰ�쿡�� �����Ѵ�;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return;

		// DB ���� Login���·� �����ִ� �����͸� �űԷ� ��ȯ;
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

		// ���弭���� �ƴѰ�쿡�� �����Ѵ�;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return;

		GLChar* pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
		{
			sc::writeLogError ( "[ ProvideReward Log ] [ Invalid Character DBNum. ]" );

			return;
		}

		// DB ���� Select & DB Login Flag�� ����;
		m_pServer->AddGameAdoJob( db::DbActionPtr(
			new CProvideRewardSelectAction( _dwChaDBNum ) ) );
	}


	void GLProvideRewardManager::OnRegisterProvideReward (
		const PROVIDE_REWARD_DATA& _ProvideRewardData )
	{
		// ���弭���� �ƴѰ�쿡�� �����Ѵ�;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return;

		// ť�� ���;
		m_queueProvideReward.push( _ProvideRewardData );
	}


	void GLProvideRewardManager::OnFrameMove ( float fElapsedTime )
	{
		if ( NULL == m_pServer )
		{
			sc::writeLogError ( "[ ProvideReward Log ] [ Server is NULL. ]" );

			return;
		}

		// ���弭���� �ƴѰ�쿡�� �����Ѵ�;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return;

		DWORD dwCount = 0;

		// ť�� ����ִ� ���� ���� ���� ����;
		while ( !m_queueProvideReward.empty() )
		{
			// �� Frame�� PROVIDE_REWARD_LIMIT_COUNT ��ŭ�� ����;
			// -> ���� ������ ���� �Ͼ�� �ʴ´�;
			if ( dwCount >= PROVIDE_REWARD_LIMIT_COUNT )
				break;

			// Ȯ��;
			PROVIDE_REWARD_DATA curProvideRewardData =
				m_queueProvideReward.front();

			// Pop;
			m_queueProvideReward.pop();

			// DB �Ϸ� Flag�� ����;
			m_pServer->AddGameAdoJob( db::DbActionPtr(
				new CProvideRewardCompleteAction( curProvideRewardData.GetUniqueID() ) ) );

			// ���� ���� ( ��� ���� �������� �����Ѵ� );
			curProvideRewardData.SetProvideType(
				PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_IMMEDIATELY );
			ProvideReward ( curProvideRewardData );

			dwCount++;
		}

		// �Ϸ� Flag �� ����� ���������� ���� �ð����� Clear �Ѵ�;
		static float fTime = 0.f;
		fTime += fElapsedTime;

		if ( fTime >= PROVIDE_REWARD_CLEAR_TIME )
		{
			// �ϴ� ��� ������� �ʴ´�;
			m_pServer->AddGameAdoJob( db::DbActionPtr(
				new CProvideRewardClearDeleteFlagAction( ) ) );

			fTime = 0.f;
		}
	}


	void GLProvideRewardManager::ProvideReward (
		const PROVIDE_REWARD_DATA& _ProvideRewardData )
	{
		// ĳ���� ��ȿ�� üũ;
		if ( _ProvideRewardData.GetChaDBNum() == 0 )
		{
			sc::writeLogError ( "[ ProvideReward Log ] [ Invalid Character DBNum. ]" );

			return;
		}

		// ���� ��� üũ;
		switch ( _ProvideRewardData.GetProvideType() )
		{
		case PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_IMMEDIATELY:
			{
				// ���弭���� �ƴѰ�쿡�� �����Ѵ�;
				if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
					return;

				_ProvideRewardImmediately( _ProvideRewardData );
			}
			break;

		case PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_LOGIN:
			{
				// ���弭�������� �����Ѵ�;
				// �Ŀ��� ���弭�� �ܿ����� �����ϵ��� �ϸ� ����;
				if ( false == GLUseFeatures::GetInstance().IsUsingWorldBattle() )
					return;

				// ���弭�������� Login�� ������ �ִ� ������ ��� �����ϴ�;
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

		// ���弭���� �ƴѰ�쿡�� �����Ѵ�;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return;

		// ĳ���� ������;
		const GLChar* const pChar =
			m_pServer->GetCharByDbNum( _ProvideRewardData.GetChaDBNum() );
		if ( NULL == pChar )
		{
			sc::writeLogError ( "[ ProvideReward Log ] [ Invalid Character DBNum. ]" );

			return;
		}

		// ���� Ÿ�� üũ;
		DWORD dwAttatchedMoney = 0;
		SITEMCUSTOM sNewItem;

		switch ( _ProvideRewardData.GetRewardType() )
		{
		case PROVIDE_REWARD_DATA::EMREWARD_TYPE_MONEY:
			{
				// ������ ��;
				dwAttatchedMoney = _ProvideRewardData.GetRewardValue1();

				// ���� ������;
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

				// Item ����;
				const SNATIVEID sItemID( dwItemMainID, dwItemSubID );
				m_pServer->NewItemMake( sNewItem, sItemID, EMGEN_SYSTEM );

				if ( sNewItem.GetNativeID() == SNATIVEID( false ) )
				{
					sc::writeLogError ( "[ ProvideReward Log ] [ Failed Make New Item. ]" );

					return;
				}
				
				// Item �� ��ȿ���� üũ;
				const SITEM* pItem =
					GLogicData::GetInstance().GetItem(
					sNewItem.GetNativeID() );
				if ( NULL == pItem )
				{
					sc::writeLogError ( "[ ProvideReward Log ] [ Invalid Character DBNum. ]" );

					return;
				}

				// Item �� �ִ� Pile ���� üũ;
				const int nMaxPileNum = pItem->sDrugOp.GetPileNum();

				// Item �� ���� ����;
				sNewItem.TurnNumSet( min(
					nMaxPileNum,
					static_cast< int >( dwItemCount ? dwItemCount : 1 ) ) );

				sNewItem.GenerateBasicStat( false );
				sNewItem.GenerateLinkSkill();
				sNewItem.GenerateAddOption();

				m_pServer->GetItemLimit()->LogItemBasicStatOption( sNewItem );
				m_pServer->GetItemLimit()->LogItemLinkSkillOption( sNewItem );
				m_pServer->GetItemLimit()->LogItemAddonOption( sNewItem );

				// Item �����ɼ� ����ȭ �� Log;
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

				// ������ ��;
				dwAttatchedMoney = _ProvideRewardData.GetRewardValue1();

				// Item ����;
				const SNATIVEID sItemID( dwItemMainID, dwItemSubID );
				m_pServer->NewItemMake( sNewItem, sItemID, EMGEN_SYSTEM );

				if ( sNewItem.GetNativeID() == SNATIVEID( false ) )
				{
					sc::writeLogError ( "[ ProvideReward Log ] [ Failed Make New Item. ]" );

					return;
				}

				// Item �� ��ȿ���� üũ;
				const SITEM* pItem =
					GLogicData::GetInstance().GetItem(
					sNewItem.GetNativeID() );
				if ( NULL == pItem )
				{
					sc::writeLogError ( "[ ProvideReward Log ] [ Invalid Item. ]" );

					return;
				}

				// Item �� �ִ� Pile ���� üũ;
				const int nMaxPileNum = pItem->sDrugOp.GetPileNum();

				// Item �� ���� ����;
				sNewItem.TurnNumSet( min(
					nMaxPileNum,
					static_cast< int >( dwItemCount ? dwItemCount : 1 ) ) );

				sNewItem.GenerateBasicStat( false );
				sNewItem.GenerateLinkSkill();
				sNewItem.GenerateAddOption();

				m_pServer->GetItemLimit()->LogItemBasicStatOption( sNewItem );
				m_pServer->GetItemLimit()->LogItemLinkSkillOption( sNewItem );
				m_pServer->GetItemLimit()->LogItemAddonOption( sNewItem );

				// Item �����ɼ� ����ȭ �� Log;
				if ( sNewItem.GENERATE_RANDOM_OPT( false ) )
				{
					GLItemLimit* pItemLimit = m_pServer->GetItemLimit();
					pItemLimit->LogItemRandomOption( sNewItem );
				}
			}
			break;
		}

		// ���� ���� ������;
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

		// ���弭�������� �����Ѵ�;
		// �Ŀ��� ���弭�� �ܿ����� �����ϵ��� �ϸ� ����;
		if ( false == GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return;

		// DB �ű� Flag�� ���;
		m_pServer->AddGameAdoJob( db::DbActionPtr(
			new CProvideRewardInsertAction( _ProvideRewardData ) ) );
	}

}