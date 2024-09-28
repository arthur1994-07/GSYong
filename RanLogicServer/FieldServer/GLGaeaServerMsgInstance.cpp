#include "pch.h"

#include "../../RanLogic/GLUseFeatures.h"

// Provide Reward System
#include "../ProvideRewardSystem/GLProvideRewardManager.h"

#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "GLGaeaServer.h"

BOOL GLGaeaServer::MsgInstanceSystemProc ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg )
{
	if ( NULL == m_pInstanceManager )
		return FALSE;

	return m_pInstanceManager->OnMessageProc(
		static_cast< GLMSG::NET_INSTANCE_MESSAGE_HEADER* >( pMsg ),
		dwGaeaID ) ? TRUE : FALSE;
}

BOOL GLGaeaServer::MsgInstanceSystem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{	
    return m_pInstanceManager->_messageProcedure(nmg, dwGaeaID) ? TRUE : FALSE;
}

BOOL GLGaeaServer::MsgInstanceSystemContents(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{	
    return FALSE;
}

BOOL GLGaeaServer::MsgInstanceSystemContentsMsgPack(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{	
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
	GLMSG::NET_INSTANCE_MSGPACK_MESSAGE _headerData;
	if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), _headerData) == false)
		return false;

	switch ( _headerData.emType )
	{
	case InstanceSystem::EMMESSAGE_MATCHING_MAIL_AF:
		return MsgInstanceMail(nmg);
	}


    return FALSE;
}

BOOL GLGaeaServer::MsgInstanceMail(const NET_MSG_GENERIC* const _pMessage)
{
	NET_MSG_PACK* const pPacket = (NET_MSG_PACK* const)_pMessage;
	GLMSG::NET_INSTANCE_MAIL _PackData;
	if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), _PackData) == false)
		return false;

	ProvideRewardSystem::GLProvideRewardManager* pProvideRewardMan =
		ProvideRewardSystem::GLProvideRewardManager::Instance();
	if ( NULL == pProvideRewardMan )
		return false;
	
	if(_PackData.Mail.IsUseItem)
	{
		DWORD _dwItemNum = _PackData.Mail.ItemNum;

		ProvideRewardSystem::PROVIDE_REWARD_DATA provideRewardData;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
		{
			provideRewardData.SetCharacter( _PackData.Mail.CharID );
			provideRewardData.SetProvideType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_LOGIN );
			provideRewardData.SetRewardType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMREWARD_TYPE_MONEY_AND_ITEM );
			provideRewardData.SetRewardValue(
				_PackData.Mail.AttachedMoney,
				_PackData.Mail.NewItem.wMainID,
				_PackData.Mail.NewItem.wSubID,
				_dwItemNum );
			provideRewardData.SetTextIndex( _PackData.Mail.dwContentTextIndex );
		}
		else
		{
			/*provideRewardData.SetCharacter( _PackData.Mail.CharID );
			provideRewardData.SetProvideType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_IMMEDIATELY );
			provideRewardData.SetRewardType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMREWARD_TYPE_MONEY_AND_ITEM );
			provideRewardData.SetRewardValue(
				_PackData.Mail.AttachedMoney,
				_PackData.Mail.NewItem.wMainID,
				_PackData.Mail.NewItem.wSubID,
				_dwItemNum );
			provideRewardData.SetTextIndex( _PackData.Mail.dwContentTextIndex );*/

			const SNATIVEID receiveItemID(_PackData.Mail.NewItem.wMainID, _PackData.Mail.NewItem.wSubID);        
			SITEMCUSTOM newItem;
			InstanceSystem::gpGaeaServer->NewItemMake(newItem, receiveItemID, EMGEN_SYSTEM);
			if ( newItem.GetNativeID() != SNATIVEID(false) )
			{
				const SITEM* pITEM = GLogicData::GetInstance().GetItem( newItem.GetNativeID() );
				if ( pITEM == NULL )
					return 0;

				const int maxPileNum = pITEM->sDrugOp.GetPileNum();
				newItem.TurnNumSet( min(maxPileNum, (int)(_dwItemNum ? _dwItemNum : 1)) );

				newItem.GenerateBasicStat( false );
				newItem.GenerateLinkSkill();
				newItem.GenerateAddOption();

				InstanceSystem::gpGaeaServer->GetItemLimit()->LogItemBasicStatOption( newItem );
				InstanceSystem::gpGaeaServer->GetItemLimit()->LogItemLinkSkillOption( newItem );
				InstanceSystem::gpGaeaServer->GetItemLimit()->LogItemAddonOption( newItem );

				if ( newItem.GENERATE_RANDOM_OPT( false ) )
				{
					GLItemLimit* pItemLimit = InstanceSystem::gpGaeaServer->GetItemLimit();
					pItemLimit->LogItemRandomOption(newItem);
				}
			}


			InstanceSystem::gpGaeaServer->PostSendSystemMail(
				_PackData.Mail.CharID,
				_PackData.Mail.szName,
				newItem,
				_PackData.Mail.AttachedMoney,
				ATTENDANCE_REWARD_POST_DESIGN,
				ATTENDANCE_REWARD_POST_SENDER,
				_PackData.Mail.Sender,
				_PackData.Mail.PostTitle,
				_PackData.Mail.PostContent,
				false
				);
		}

		
	}
	else
	{
		ProvideRewardSystem::PROVIDE_REWARD_DATA provideRewardData;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
		{
			provideRewardData.SetCharacter( _PackData.Mail.CharID );
			provideRewardData.SetProvideType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_LOGIN );
			provideRewardData.SetRewardType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMREWARD_TYPE_MONEY );
			provideRewardData.SetRewardValue( _PackData.Mail.AttachedMoney );
			provideRewardData.SetTextIndex( 0 );
		}
		else
		{
			provideRewardData.SetCharacter( _PackData.Mail.CharID );
			provideRewardData.SetProvideType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_IMMEDIATELY );
			provideRewardData.SetRewardType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMREWARD_TYPE_MONEY );
			provideRewardData.SetRewardValue( _PackData.Mail.AttachedMoney );
			provideRewardData.SetTextIndex( 0 );

			/*InstanceSystem::gpGaeaServer->PostSendSystemMail(
				_PackData.Mail.CharID,
				_PackData.Mail.szName,
				SNATIVEID( false ).dwID,
				_PackData.Mail.AttachedMoney,
				ATTENDANCE_REWARD_POST_DESIGN,
				ATTENDANCE_REWARD_POST_SENDER,
				_PackData.Mail.Sender,
				_PackData.Mail.PostTitle,
				_PackData.Mail.PostContent,
				false
				);*/
		}
	}
	
	return true;
}
