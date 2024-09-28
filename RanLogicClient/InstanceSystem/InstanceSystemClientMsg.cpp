#include "pch.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../enginelib/GUInterface/UITextControl.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../enginelib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "../Char/GLCharClient.h"
#include "InstanceSystemClient.h"

namespace InstanceSystem
{

	const bool ManagerClient::OnMessageProc ( const GLMSG::NET_INSTANCE_MESSAGE_HEADER* pMsg )
	{
		switch ( pMsg->Type() )
		{
		case NET_MSG_GCTRL_INSTANCE_RETRY_FACTION_FB:
			return MsgRetryFactionFB( static_cast< const GLMSG::NET_RETRY_FACTION_FB* >( pMsg ) );

		case NET_MSG_GCTRL_INSTANCE_SHOW_TIME_UI_FB:
			return MsgShowTimeUIFB( static_cast< const GLMSG::NET_SHOW_TIME_UI_FB* >( pMsg ) );

		case NET_MSG_GCTRL_INSTANCE_SHOW_ENTRANCE_STATE_UI_FB:
			return MsgShowEntranceStateUIFB( static_cast< const GLMSG::NET_SHOW_ENTRANCE_STATE_UI_FB* >( pMsg ) );
		}

		return true;
	}

	const bool ManagerClient::MsgRetryFactionFB ( const GLMSG::NET_RETRY_FACTION_FB* pMsg )
	{
		if ( NULL == pMsg )
			return false;

		float fTime = pMsg->fTime;
		DWORD dwMoney = pMsg->dwMoney;
		InstanceSystem::InstanceMapID nInstanceID = pMsg->nInstanceID;
		SNATIVEID sKeyMapID = pMsg->sKeyMapID;

		m_nRetryInstanceID = nInstanceID;
		m_sRetryKeyMapID = sKeyMapID;

		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_NewInstance_Retry,
			"-n, -n, -b",
			dwMoney, static_cast< int >( fTime ), pMsg->bShow );

		return true;
	}

	const bool ManagerClient::MsgShowTimeUIFB ( const GLMSG::NET_SHOW_TIME_UI_FB* pMsg )
	{
		if ( NULL == pMsg )
			return false;

		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_PVE_StageIndicator, 
			"-n, -n, -n, -n, -b, -b"
			, 0, 0, 0, static_cast< int >( pMsg->fSec ), false, pMsg->bShow );

		return true;
	}

	const bool ManagerClient::MsgShowEntranceStateUIFB ( const GLMSG::NET_SHOW_ENTRANCE_STATE_UI_FB* pMsg )
	{
		if ( NULL == pMsg )
			return false;

		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_PVE_EntranceState, 
			"-n, -n, -n, -b",
			pMsg->dwCurCount,
			pMsg->dwMaxCount,
			static_cast< int >( pMsg->fSec ),
			pMsg->bShow );

		return true;
	}


	/// 기존 Instance System 의 Message 처리기;
	// -> 패킷모니터링에 반영되지 않는다;

	template <typename TemplateMessage>
    const bool CALLBACK _messageProcedureNotifyGlobalVariable( GLGaeaClient* const _pGaeaClient, const NET_MSG_GENERIC* const _pMessage)
    {
        const TemplateMessage* const _pTemplateMessage((const TemplateMessage* const)_pMessage);

        const unsigned int _colorTomato(D3DCOLOR_XRGB(239, 132, 84));

        const unsigned int _colorNormal(D3DCOLOR_XRGB(239, 228, 176));
        const SNATIVEID& _keyMapID(_pTemplateMessage->keyMapID);
        const InstanceInformationClient* const _pInstanceInformation(_pGaeaClient->getInstanceInformation(_keyMapID));
        if ( _pInstanceInformation == 0 )
        {
            _pGaeaClient->PrintMsgText(_colorNormal, sc::string::format("invalid keyMapID(%1%/%2%)", _keyMapID.wMainID, _keyMapID.wSubID));
            return false;
        }

        const char* const _instanceName(_pInstanceInformation->instanceName);
        const char* const _globalName(_pTemplateMessage->globalName);
        const char* const _valueType(_pTemplateMessage->bTableValue == true ? "(table)" : "(single)");
        const InstanceMapID _instanceMapID(_pTemplateMessage->instanceMapID);

        _pGaeaClient->PrintMsgText(_colorTomato, sc::string::format(":[%1%(%2%)] in %3%(%4%)", _globalName, _valueType, _instanceName, _instanceMapID));

        const unsigned int _nSize(_pTemplateMessage->nVariable);
        if ( _nSize == 0 )
        {
            _pGaeaClient->PrintMsgText(_colorNormal, std::string("no value"));
            return true;
        }

        for ( unsigned int _i(_nSize); _i--; )
            _pGaeaClient->PrintMsgText(_colorNormal, sc::string::format("-key[%1%]/value[%2%]", _pTemplateMessage->globalVariable[_i].nKey, _pTemplateMessage->globalVariable[_i].value));

        return true;
    }	

    const bool CALLBACK ManagerClient::_messageProcedure(const NET_MSG_GENERIC* const _pMessage)
    {
        const GLMSG::NET_INSTANCE_MESSAGE* const _pInstanceMessage((const GLMSG::NET_INSTANCE_MESSAGE* const)_pMessage);
        switch ( _pInstanceMessage->emType )
        {
        case InstanceSystem::EMMESSAGE_ERROR:
            return ManagerClient::_messageProcedureError(_pMessage);
        case InstanceSystem::EMMESSAGE_NOTIFY_PLAYER_POSITION:
            return ManagerClient::_messageProcedureNotifyPlayerPosition(_pMessage);
        case InstanceSystem::EMMESSAGE_NOTIFY_INSTANCE_LOG:
            return ManagerClient::_messageProcedureNotifyInstanceLog(_pMessage);
        case InstanceSystem::EMMESSAGE_NOTIFY_INSTANCE_LIST:
            return ManagerClient::_messageProcedureNotifyInstanceList(_pMessage);
        case InstanceSystem::EMMESSAGE_NOTIFY_CHILD_LAND:
            return ManagerClient::_messageProcedureNotifyChildLand(_pMessage);
        case InstanceSystem::EMMESSAGE_NOTIFY_PLAYER_LIST:
            return ManagerClient::_messageProcedureNotifyPlayerList(_pMessage);

        case InstanceSystem::EMMESSAGE_NOTIFY_GLOBAL_FLOAT:
            return InstanceSystem::_messageProcedureNotifyGlobalVariable<GLMSG::NET_INSTANCE_NOTIFY_GLOBAL_FLOAT>( this->pGaeaClient, _pMessage);
        case InstanceSystem::EMMESSAGE_NOTIFY_GLOBAL_INTEGER:
            return InstanceSystem::_messageProcedureNotifyGlobalVariable<GLMSG::NET_INSTANCE_NOTIFY_GLOBAL_INTEGER>( this->pGaeaClient, _pMessage);
        case InstanceSystem::EMMESSAGE_NOTIFY_GLOBAL_BOOLEAN:
            return InstanceSystem::_messageProcedureNotifyGlobalVariable<GLMSG::NET_INSTANCE_NOTIFY_GLOBAL_BOOLEAN>( this->pGaeaClient, _pMessage);
        case InstanceSystem::EMMESSAGE_NOTIFY_GLOBAL_STRING:
            return InstanceSystem::_messageProcedureNotifyGlobalVariable<GLMSG::NET_INSTANCE_NOTIFY_GLOBAL_STRING>( this->pGaeaClient, _pMessage);

        case InstanceSystem::EMMESSAGE_NEW_CUSTOM_MESSAGE:
            return ManagerClient::_messageProcedureNewCustomMessage(_pMessage);
        case InstanceSystem::EMMESSAGE_SET_MOTION_BRD:
            return ManagerClient::_messageProcedureSetMotion(_pMessage);
        case InstanceSystem::EMMESSAGE_SET_ACT_STATE_BRD:
            return ManagerClient::_messageProcedureSetActState(_pMessage);

		case InstanceSystem::EMMESSAGE_NOTIFY_COUNT_PACKET:
			return ManagerClient::_messageProcedureNotifyCountPacket(_pMessage);
        }

        return false;
    }

	const bool CALLBACK ManagerClient::_messageProcedureNotifyCountPacket(const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_INSTANCE_NOTIFY_COUNT_PACKET* const _pMessageNotifyCountPacket((const GLMSG::NET_INSTANCE_NOTIFY_COUNT_PACKET* const)_pMessage);
		const unsigned int _sizePacket(_pMessageNotifyCountPacket->sizePacket);

		const unsigned int _colorTomato(D3DCOLOR_XRGB(239, 132, 84));
		const unsigned int _colorNormal(D3DCOLOR_XRGB(239, 228, 176));

		GLGaeaClient* const _pGaeaClient(this->pGaeaClient);
		if ( _sizePacket == 0 )
		{
			_pGaeaClient->PrintMsgText(_colorTomato, std::string("no received message"));
			return true;
		}

		_pGaeaClient->PrintMsgText(_colorTomato, std::string("~~~~~~~~~~~~~~~~~~~~~~~~"));
		_pGaeaClient->PrintMsgText(_colorTomato, std::string("matchingSystem received message"));
		for ( unsigned int _index(0); _index < _sizePacket; ++_index )
		{
			const GLMSG::NET_INSTANCE_NOTIFY_COUNT_PACKET::PACKET_COUNT& _notifyCountPacket(_pMessageNotifyCountPacket->countPacket[_index]);
			const unsigned int _indexPacket(_notifyCountPacket.indexPacket);
			const unsigned int _countPacket(_notifyCountPacket.countPacket);
			if ( _countPacket == 0 )
				continue;
			_pGaeaClient->PrintMsgText(_colorNormal, sc::string::format("indexPacket[%1%], countPacket[%2%]", _indexPacket, _countPacket));
		}		
		_pGaeaClient->PrintMsgText(_colorTomato, std::string("~~~~~~~~~~~~~~~~~~~~~~~~"));
        return true;
	}

    const bool CALLBACK ManagerClient::_messageProcedureMsgPack(const NET_MSG_GENERIC* const _pMessage)
    {
        NET_MSG_PACK* pPacket = (NET_MSG_PACK*) _pMessage;
        GLMSG::NET_INSTANCE_MSGPACK_MESSAGE _headerData;
        if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), _headerData) == false)
            return false;

        switch ( _headerData.emType )
        {
        case InstanceSystem::EMMESSAGE_UI_CHATMSG:
            return ManagerClient::_messageProcedureUIChatMsg(_pMessage);
        case InstanceSystem::EMMESSAGE_UI_CHATXMLMSG:
            return ManagerClient::_messageProcedureUIChatXmlMsg(_pMessage);
        case InstanceSystem::EMMESSAGE_UI_SIMPLE_MSG:
            return ManagerClient::_messageProcedureUISimpleMsg(_pMessage);
        case InstanceSystem::EMMESSAGE_UI_SIMPLE_XMLMSG:
            return ManagerClient::_messageProcedureUISimpleXmlMsg(_pMessage);
        case InstanceSystem::EMMESSAGE_UI_TEXT_DLG_MSG:
            return ManagerClient::_messageProcedureUITextDlgMsg(_pMessage);
        case InstanceSystem::EMMESSAGE_UI_TEXT_DLG_XMLMSG:
            return ManagerClient::_messageProcedureUITextDlgXmlMsg(_pMessage);
        case InstanceSystem::EMMESSAGE_UI_TIMER_MSGBOX:
            return ManagerClient::_messageProcedureUITimerMsgBox(_pMessage);

        case InstanceSystem::EMMESSAGE_EFFECT:
            return ManagerClient::_messageProcedureEffect(_pMessage);
        }

        return false;
    }

    const bool CALLBACK ManagerClient::_messageProcedure_Contents(const NET_MSG_GENERIC* const _pMessage)
    {
        const GLMSG::NET_INSTANCE_CONTENTS_MESSAGE* const _pInstanceMessage((const GLMSG::NET_INSTANCE_CONTENTS_MESSAGE* const)_pMessage);
        switch ( _pInstanceMessage->emType )
        {
        case InstanceSystem::EMMSG_RNCDM_PROGRESS_UPDATE_FC:
            return ManagerClient::_msgProc_RNCDM_ProgressUpdate(_pMessage);
        case InstanceSystem::EMMSG_RNCDM_PROGRESS_OFF_FC:
            return ManagerClient::_msgProc_RNCDM_ProcessOff(_pMessage);
        //case InstanceSystem::EMMSG_RNCDM_RANK_INFO_FC:
            //return ManagerClient::_msgProc_RNCDM_RankInfo(_pMessage);
        case InstanceSystem::EMMSG_RNCDM_RANK_SHOW_FC:
            return ManagerClient::_msgProc_RNCDM_RankShow(_pMessage);
        //case InstanceSystem::EMMSG_RNCDM_MAP_INFO_FC:
            //return ManagerClient::_msgProc_RNCDM_MapInfo(_pMessage);
        //case InstanceSystem::EMMSG_RNCDM_MAP_IMAGE_FC:
            //return ManagerClient::_msgProc_RNCDM_MapImageName(_pMessage);
        //case InstanceSystem::EMMSG_RNCDM_BATTLESTATE_FC:
            //return ManagerClient::_msgProc_RNCDM_BattleState(_pMessage);
        case InstanceSystem::EMMSG_RNCDM_START_LOGO_SHOW_FC:
            return ManagerClient::_msgProc_RNCDM_StartLogoShow(_pMessage);
        case InstanceSystem::EMMSG_RNCDM_END_LOGO_SHOW_FC:
            return ManagerClient::_msgProc_RNCDM_EndLogoShow(_pMessage);
        //case InstanceSystem::EMMSG_RNCDM_TOP_CLUB_INFO_FC:
            //return ManagerClient::_msgProc_RNCDM_TopClubInfo(_pMessage);

        case InstanceSystem::EMMSG_CTF_FLAG_UPDATE_FC:
            return ManagerClient::_msgProc_CaptureTheFlag_FlagUpdate(_pMessage);

//         case InstanceSystem::EMMSG_COMPETITION_RESULT_TAB_FC:
//         case InstanceSystem::EMMSG_COMPETITION_RESULT_INFO_FC:
//         case InstanceSystem::EMMSG_COMPETITION_RESULT_END_INFO_FC:
        case  InstanceSystem::EMMSG_GUIDANCE_UI_AUTH:
            return ManagerClient::_msgProc_Guidance_UI_Auth(_pMessage);
        case  InstanceSystem::EMMSG_GUIDANCE_UI_PROGRESS:
            return ManagerClient::_msgProc_Guidance_UI_Progress(_pMessage);
        //case  InstanceSystem::EMMSG_GUIDANCE_UI_RANK:
            //return ManagerClient::_msgProc_Guidance_UI_Rank(_pMessage);
        case  InstanceSystem::EMMSG_GUIDANCE_UI_RANK_MY_INFO:
            return ManagerClient::_msgProc_Guidance_UI_Rank_MyInfo(_pMessage);
        //case  InstanceSystem::EMMSG_GUIDANCE_UI_AUTH_INFO:
            //return ManagerClient::_msgProc_Guidance_UI_AuthInfo(_pMessage);

		case  InstanceSystem::EMMSG_CTI_UI_AUTH:
			return ManagerClient::_msgProc_CTI_UI_Auth(_pMessage);
		case  InstanceSystem::EMMSG_CTI_UI_PROGRESS:
			return ManagerClient::_msgProc_CTI_UI_Progress(_pMessage);
		case  InstanceSystem::EMMSG_CTI_UI_RANK_MY_INFO:
			return ManagerClient::_msgProc_CTI_UI_Rank_MyInfo(_pMessage);
        }

        /*m_GLCharacterMsgFunc[NET_MSG_GCTRL_RNCDM_PROGRESS_UPDATE_FC].Msgfunc = &GLCharacter::MsgRnCDMProgessUpdate;
        m_GLCharacterMsgFunc[NET_MSG_GCTRL_RNCDM_PROGRESS_OFF_FC].Msgfunc = &GLCharacter::MsgRnCDMProgessOff;
        m_GLCharacterMsgFunc[NET_MSG_GCTRL_RNCDM_RANK_INFO_FC].Msgfunc = &GLCharacter::MsgRnCDMRankInfo;
        m_GLCharacterMsgFunc[NET_MSG_GCTRL_RNCDM_RANK_SHOW_FC].Msgfunc = &GLCharacter::MsgRnCDMRankShow;
        m_GLCharacterMsgFunc[NET_MSG_GCTRL_RNCDM_START_LOGO_SHOW_FC].Msgfunc = &GLCharacter::MsgRnCDMStartLogoShow;
        m_GLCharacterMsgFunc[NET_MSG_GCTRL_RNCDM_END_LOGO_SHOW_FC].Msgfunc = &GLCharacter::MsgRnCDMEndLogoShow;*/

        return false;
    }

    const bool CALLBACK ManagerClient::_messageProcedureMsgPack_Contents(const NET_MSG_GENERIC* const _pMessage)
    {
        NET_MSG_PACK* pPacket = (NET_MSG_PACK*) _pMessage;
        GLMSG::NET_INSTANCE_MSGPACK_MESSAGE _headerData;
        if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), _headerData) == false)
            return false;

        switch ( _headerData.emType )
        {
        //case InstanceSystem::EMMSG_RNCDM_PROGRESS_UPDATE_FC:
            //return ManagerClient::_msgProc_RNCDM_ProgressUpdate(_pMessage);
        //case InstanceSystem::EMMSG_RNCDM_PROGRESS_OFF_FC:
            //return ManagerClient::_msgProc_RNCDM_ProcessOff(_pMessage);
        case InstanceSystem::EMMSG_RNCDM_RANK_INFO_FC:
            return ManagerClient::_msgProc_RNCDM_RankInfo(_pMessage);
        //case InstanceSystem::EMMSG_RNCDM_RANK_SHOW_FC:
            //return ManagerClient::_msgProc_RNCDM_RankShow(_pMessage);
        case InstanceSystem::EMMSG_RNCDM_MAP_INFO_FC:
            return ManagerClient::_msgProc_RNCDM_MapInfo(_pMessage);
        case InstanceSystem::EMMSG_RNCDM_MAP_IMAGE_FC:
            return ManagerClient::_msgProc_RNCDM_MapImageName(_pMessage);
        case InstanceSystem::EMMSG_RNCDM_BATTLESTATE_FC:
            return ManagerClient::_msgProc_RNCDM_BattleState(_pMessage);
        //case InstanceSystem::EMMSG_RNCDM_START_LOGO_SHOW_FC:
            //return ManagerClient::_msgProc_RNCDM_StartLogoShow(_pMessage);
        //case InstanceSystem::EMMSG_RNCDM_END_LOGO_SHOW_FC:
            //return ManagerClient::_msgProc_RNCDM_EndLogoShow(_pMessage);
        case InstanceSystem::EMMSG_RNCDM_TOP_CLUB_INFO_FC:
            return ManagerClient::_msgProc_RNCDM_TopClubInfo(_pMessage);

		case InstanceSystem::EMMSG_RNCDM_DOUBLE_POINT_FC:
			return ManagerClient::_msgProc_RNCDM_DoublePoint(_pMessage);

        case InstanceSystem::EMMSG_RNCDM_PREVIOUS_INFO_FAC:
            return ManagerClient::_msgProc_RNCDM_PreviousInfo(_pMessage);

        //case InstanceSystem::EMMSG_CTF_FLAG_UPDATE_FC:
            //return ManagerClient::_msgProc_CaptureTheFlag_FlagUpdate(_pMessage);

        case InstanceSystem::EMMSG_COMPETITION_RESULT_TAB_FC:
        case InstanceSystem::EMMSG_COMPETITION_RESULT_INFO_FC:
        case InstanceSystem::EMMSG_COMPETITION_RESULT_END_INFO_FC:
            return ManagerClient::_msgProc_RnCompetitionResult(_pMessage);
            break;

        //case  InstanceSystem::EMMSG_GUIDANCE_UI_AUTH:
            //return ManagerClient::_msgProc_Guidance_UI_Auth(_pMessage);
        //case  InstanceSystem::EMMSG_GUIDANCE_UI_PROGRESS:
            //return ManagerClient::_msgProc_Guidance_UI_Progress(_pMessage);
        case  InstanceSystem::EMMSG_GUIDANCE_UI_RANK:
            return ManagerClient::_msgProc_Guidance_UI_Rank(_pMessage);
        //case  InstanceSystem::EMMSG_GUIDANCE_UI_RANK_MY_INFO:
            //return ManagerClient::_msgProc_Guidance_UI_Rank_MyInfo(_pMessage);
        case  InstanceSystem::EMMSG_GUIDANCE_UI_AUTH_INFO:
            return ManagerClient::_msgProc_Guidance_UI_AuthInfo(_pMessage);

		case  InstanceSystem::EMMSG_CTI_UI_RANK:
			return ManagerClient::_msgProc_CTI_UI_Rank(_pMessage);
		case InstanceSystem::EMMSG_CTI_UI_AUTH_INFO:
			return ManagerClient::_msgProc_CTI_UI_AuthInfo( _pMessage );
        }

        return false;
    }

	const bool CALLBACK ManagerClient::_messageProcedureUIChatMsg(const NET_MSG_GENERIC* const _pMessage)
	{
		NET_MSG_PACK* const pPacket = (NET_MSG_PACK* const)_pMessage;
		GLMSG::NET_INSTANCE_UI_CHATMSG _PackData;
		if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), _PackData) == false)
			return false;

		this->pGaeaClient->PrintMsgText(_PackData.dwColor, _PackData.strText);

		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureUIChatXmlMsg(const NET_MSG_GENERIC* const _pMessage)
	{
		NET_MSG_PACK* const pPacket = (NET_MSG_PACK* const)_pMessage;
		GLMSG::NET_INSTANCE_UI_CHATXMLMSG _PackData;
		if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), _PackData) == false)
			return false;

		std::string _msgString;
		if ( InstanceXMLStringToString(_PackData.xmlString, _msgString) == false)
			return false;

		this->pGaeaClient->PrintMsgText(_PackData.dwColor, _msgString);
		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureUISimpleMsg(const NET_MSG_GENERIC* const _pMessage)
	{
		NET_MSG_PACK* const pPacket = (NET_MSG_PACK* const)_pMessage;
		GLMSG::NET_INSTANCE_UI_SIMPLE_MSG _PackData;
		if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), _PackData) == false)
			return false;

		GLWidgetScript& _widgetScript(GLWidgetScript::GetInstance());	
		_widgetScript.LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Toggle_Public_Notify_Message,
			"-s, -dw, -dw, -f",
			_PackData.strText.c_str(), _PackData.dwColor, _PackData.dwFontSize, _PackData.fShowTime );

		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureUISimpleXmlMsg(const NET_MSG_GENERIC* const _pMessage)
	{
		NET_MSG_PACK* const pPacket = (NET_MSG_PACK* const)_pMessage;
		GLMSG::NET_INSTANCE_UI_SIMPLE_XMLMSG _PackData;
		if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), _PackData) == false)
			return false;

		std::string _msgString;
		if ( InstanceXMLStringToString(_PackData.xmlString, _msgString) == false)
			return false;

		GLWidgetScript& _widgetScript(GLWidgetScript::GetInstance());	
		_widgetScript.LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Toggle_Public_Notify_Message,
			"-s, -dw, -dw, -f",
			_msgString.c_str(), _PackData.dwColor, _PackData.dwFontSize, _PackData.fShowTime);

		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureUITextDlgMsg(const NET_MSG_GENERIC* const _pMessage)
	{
		NET_MSG_PACK* const pPacket = (NET_MSG_PACK* const)_pMessage;
		GLMSG::NET_INSTANCE_UI_TEXT_DLG_MSG _PackData;
		if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), _PackData) == false)
			return false;

		this->pGaeaClient->PrintMsgTextDlg(_PackData.dwColor, _PackData.strText);
		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureUITextDlgXmlMsg(const NET_MSG_GENERIC* const _pMessage)
	{
		NET_MSG_PACK* const pPacket = (NET_MSG_PACK* const)_pMessage;
		GLMSG::NET_INSTANCE_UI_TEXT_DLG_XMLMSG _PackData;
		if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), _PackData) == false)
			return false;

		std::string _msgString;
		if ( InstanceXMLStringToString(_PackData.xmlString, _msgString) == false)
			return false;

		this->pGaeaClient->PrintMsgTextDlg(_PackData.dwColor, _msgString);
		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureUITimerMsgBox ( const NET_MSG_GENERIC* const _pMessage )
	{
		NET_MSG_PACK* const pMsg = (NET_MSG_PACK* const)_pMessage;
		GLMSG::NET_INSTANCE_UI_TIMER_MSGBOX sMsg;
		if ( false == msgpack::BufferToObject( pMsg->DataBuffer(), pMsg->DataSize(), sMsg ) )
			return false;

		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( 1, sMsg.emResponseType );
		tbItem.set( 2, sMsg.dwInstanceID );
		tbItem.set( 3, sMsg.dwResponseMsgParam1 );
		tbItem.set( 4, sMsg.dwResponseMsgParam2 );

		GLWidgetCallFuncValue args[ 4 ];
		args[ 0 ].SetBool( true );
		args[ 1 ].SetString( sMsg._strContentText );
		args[ 2 ].SetInt( sMsg.fTimer );
		args[ 3 ].SetTable( &tbItem );

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_UITimerMessageBox, args, 4 );

		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureEffect(const NET_MSG_GENERIC* const _pMessage)
	{
		NET_MSG_PACK* const pPacket = (NET_MSG_PACK* const)_pMessage;
		GLMSG::NET_INSTANCE_EFFECT _PackData;
		if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), _PackData) == false)
			return false;

		D3DXMATRIX _matTrans;
		D3DXMatrixIdentity(&_matTrans);
		_matTrans._41 = _PackData._fEffectZonePosX;
		_matTrans._42 = _PackData._fEffectZonePosY;
		_matTrans._43 = _PackData._fEffectZonePosZ;

		STARGETID _targetActor(_PackData._targetActorType, _PackData._targetActorID);

		DxEffGroupPlayer::GetInstance().NewEffGroup ( _PackData._strEffectName.c_str(), _matTrans, &_targetActor, FALSE, FALSE );
		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureError(const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_INSTANCE_ERROR* const _pErrorMessage((const GLMSG::NET_INSTANCE_ERROR* const)_pMessage);

		switch ( _pErrorMessage->emError )
		{
		case InstanceSystem::EMERROR_CANNOT_JOIN_INSTANCE:
		case InstanceSystem::EMERROR_CANNOT_JOIN_INSTANCE_IN_INSTANCE:
		case InstanceSystem::EMERROR_MISMATCH_INFORMATION:
		case InstanceSystem::EMERROR_ACCESS_DENIED_INSTANCE:
		case InstanceSystem::EMERROR_CANNOT_CREATE_INSTANCE:
		case InstanceSystem::EMERROR_CANNOT_CREATE_INSTANCE_IN_INSTANCE:
		case InstanceSystem::EMERROR_RELOAD_SCRIPT:
		case InstanceSystem::EMERROR_FAILED_TO_CREATE_INSTANCE:
		case InstanceSystem::EMERROR_NOT_EXIST_INSTANCE:
		case InstanceSystem::EMERROR_NOT_EXIST_TYPE_OF_INSTANCE:
		case InstanceSystem::EMERROR_REJECT_JOIN:
		case InstanceSystem::EMERROR_DESTROY_INSTANCE:
		case InstanceSystem::EMERROR_FIELD_SERVER_OUT:
		case InstanceSystem::EMERROR_INVALID_REQUEST_INFORMATION:
			break;
		}

		GLCharacter* const _pPlayer(this->pGaeaClient->GetCharacter());
		if ( _pPlayer == 0 )
			return false;
		_pPlayer->ReSetActState(EM_REQ_GATEOUT);

		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureNotifyPlayerPosition(const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_INSTANCE_NOTIFY_PLAYER_POSITION* const _pPlayerPositionMessage((const GLMSG::NET_INSTANCE_NOTIFY_PLAYER_POSITION* const)_pMessage);

		const unsigned int _colorNormal(D3DCOLOR_XRGB(239, 228, 176));

		ManagerClient::_printGuideLine();
		this->pGaeaClient->PrintMsgText(_colorNormal,
			sc::string::format("name : %1%(dbNum : %2%)", _pPlayerPositionMessage->targetPlayerName, _pPlayerPositionMessage->dbNum));

		const InstanceInformationClient* const _pInstanceInformation(this->pGaeaClient->getInstanceInformation(_pPlayerPositionMessage->keyMapID));
		if ( _pInstanceInformation == 0 )
		{
			this->pGaeaClient->PrintMsgText(_colorNormal, std::string("not in instance"));
		}
		else
		{
			const unsigned int _instanceMainMapID(_pPlayerPositionMessage->instanceMapID.wMainID);
			const unsigned int _instanceSubMapID(_pPlayerPositionMessage->instanceMapID.wSubID);
			const char* const _instanceName(_pInstanceInformation->instanceName);
			const SNATIVEID& _keyMapID(_pPlayerPositionMessage->keyMapID);

			this->pGaeaClient->PrintMsgText(_colorNormal,
				sc::string::format("instance : %1%(keyMapID : %2%/%3%)", _instanceName, _keyMapID.wMainID, _keyMapID.wSubID));
			this->pGaeaClient->PrintMsgText(_colorNormal,
				sc::string::format("instance Map ID : %1%/%2%", _instanceMainMapID, _instanceSubMapID));
		}


		const unsigned int _baseMainMapID(_pPlayerPositionMessage->baseMapID.wMainID);
		const unsigned int _baseSubMapID(_pPlayerPositionMessage->baseMapID.wSubID);	
		const float _fPositionX(_pPlayerPositionMessage->fPositionX);
		const float _fPositionZ(_pPlayerPositionMessage->fPositionZ);
		this->pGaeaClient->PrintMsgText(_colorNormal,
			sc::string::format("location : %1%,%2%(MapID : %3%/%4%)", _fPositionX, _fPositionZ, _baseMainMapID, _baseSubMapID));

		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureNotifyInstanceLog(const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_INSTANCE_NOTIFY_INSTANCE_LOG* const _pInstanceLogMessage((const GLMSG::NET_INSTANCE_NOTIFY_INSTANCE_LOG* const)_pMessage);

		ManagerClient::_printGuideLine();

		const unsigned int _colorNormal(D3DCOLOR_XRGB(239, 228, 176));
		const unsigned int _nInstance(_pInstanceLogMessage->nInstance);
		for ( unsigned int _index(_nInstance); _index--; )
		{
			const InstanceLog& _instanceLog(_pInstanceLogMessage->instanceLog[_index]);

			const SNATIVEID& _keyMapID(_instanceLog.keyMapID);
			const InstanceInformationClient* const _pInstanceInformation(this->pGaeaClient->getInstanceInformation(_keyMapID));
			if ( _pInstanceInformation == 0 )
				continue;

			const unsigned int _nUsedCount(_instanceLog.nUsedCount);
			const unsigned int _nRunningCount(_instanceLog.nRunningCount);
			const unsigned int _nMaxCreate(_instanceLog.nMaxCreate);
			const char* const _instanceName(_pInstanceInformation->instanceName);

			this->pGaeaClient->PrintMsgText(_colorNormal,
				sc::string::format("instance : %1%(key Map ID : %2%/%3%)", _instanceName, _keyMapID.wMainID, _keyMapID.wSubID));
			this->pGaeaClient->PrintMsgText(_colorNormal,
				sc::string::format("- used Count : %1%, running Count : %2%/%3%;", _nUsedCount, _nRunningCount, _nMaxCreate));

		}

		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureNotifyInstanceList(const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_INSTANCE_NOTIFY_INSTANCE_LIST* const _pInstanceListMessage((const GLMSG::NET_INSTANCE_NOTIFY_INSTANCE_LIST* const)_pMessage);

		ManagerClient::_printGuideLine();

		const unsigned int _colorNormal(D3DCOLOR_XRGB(239, 228, 176));
		const unsigned int _nViewPage(_pInstanceListMessage->nViewPage);
		const unsigned int _nMaxPage(_pInstanceListMessage->nMaxPage);
		const unsigned int _nElement(_pInstanceListMessage->nMaxInstance);

		this->pGaeaClient->PrintMsgText(_colorNormal,
			sc::string::format("list : %1%/%2% page(total : %3%)", _nViewPage, _nMaxPage, _nElement));

		if ( _nElement == 0 )
			return true;

		for ( unsigned int _index(_nElement); _index--; )
		{
			const InstanceList& _instanceList(_pInstanceListMessage->instanceList[_index]);
			const SNATIVEID& _keyMapID(_instanceList.keyMapID);

			const InstanceInformationClient* const _pInstanceInformation(this->pGaeaClient->getInstanceInformation(_keyMapID));
			if ( _pInstanceInformation == 0 )
				continue;

			const InstanceMapID _instanceMapID(_instanceList.instanceMapID);
			const char* const _instanceName(_pInstanceInformation->instanceName);

			this->pGaeaClient->PrintMsgText(_colorNormal,
				sc::string::format("%1% _ %2%(%3%/%4%)", _instanceMapID, _instanceName, _keyMapID.wMainID, _keyMapID.wSubID));
		}

		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureNotifyChildLand(const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_INSTANCE_NOTIFY_CHILD_LAND* const _pChildLandMessage((const GLMSG::NET_INSTANCE_NOTIFY_CHILD_LAND* const)_pMessage);
		const SNATIVEID& _keyMapID(_pChildLandMessage->keyMapID);

		ManagerClient::_printGuideLine();

		const unsigned int _colorNormal(D3DCOLOR_XRGB(239, 228, 176));
		const InstanceInformationClient* const _pInstanceInformation(this->pGaeaClient->getInstanceInformation(_keyMapID));
		if ( _pInstanceInformation == 0 )
		{
			this->pGaeaClient->PrintMsgText(_colorNormal, sc::string::format("invalid keyMapID(%1%/%2%)", _keyMapID.wMainID, _keyMapID.wSubID));
			return false;
		}

		const char* const _instanceName(_pInstanceInformation->instanceName);
		const InstanceMapID _instanceMapID(_pChildLandMessage->instanceMapID);

		this->pGaeaClient->PrintMsgText(_colorNormal, sc::string::format(": %1%(%2%)", _instanceName, _instanceMapID));

		const unsigned int _nChildMap(_pChildLandMessage->nChildMap);
		if ( _nChildMap == 0 )
			this->pGaeaClient->PrintMsgText(_colorNormal, std::string("- CRITICAL ERROR : no child map"));
		else
		{
			for ( unsigned int _index(_nChildMap); _index--; )
			{
				const SNATIVEID& _childMapID(_pChildLandMessage->childMapID[_index]);
				if ( _childMapID == SNATIVEID(false) )
				{
					this->pGaeaClient->PrintMsgText(_colorNormal, std::string("- CRITICAL ERROR : null ID"));
					break;
				}

				const unsigned int _mainMapID(_childMapID.wMainID);
				const unsigned int _subMapID(_childMapID.wSubID);

				this->pGaeaClient->PrintMsgText(_colorNormal, 
					sc::string::format("baseMapID : %1%/%2%", _mainMapID, _subMapID));
			}	
		}	

		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureNotifyPlayerList(const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_INSTANCE_NOTIFY_PLAYER_LIST* const _pPlayerListMessage((const GLMSG::NET_INSTANCE_NOTIFY_PLAYER_LIST* const)_pMessage);
		const SNATIVEID& _keyMapID(_pPlayerListMessage->keyMapID);
		const InstanceMapID _instanceMapID(_pPlayerListMessage->instanceMapID);

		ManagerClient::_printGuideLine();

		const unsigned int _colorNormal(D3DCOLOR_XRGB(239, 228, 176));
		const InstanceInformationClient* const _pInstanceInformation(this->pGaeaClient->getInstanceInformation(_keyMapID));
		if ( _pInstanceInformation == 0 )
		{
			this->pGaeaClient->PrintMsgText(_colorNormal, sc::string::format("invalid keyMapID(%1%/%2%)", _keyMapID.wMainID, _keyMapID.wSubID));
			return false;
		}

		const char* const _instanceName(_pInstanceInformation->instanceName);

		this->pGaeaClient->PrintMsgText(_colorNormal, sc::string::format(": %1%(%2%)", _instanceName, _instanceMapID));		

		const unsigned int _nPlayer(_pPlayerListMessage->nPlayer);
		this->pGaeaClient->PrintMsgText(_colorNormal, sc::string::format("Current Player : %1%", _nPlayer));
		for ( unsigned int _index(_nPlayer); _index--; )
		{
			const GLMSG::NET_INSTANCE_NOTIFY_PLAYER_LIST::PlayerElement& _player(_pPlayerListMessage->playerList[_index]);
			const DBNUM _dbNum(_player.dbNum);
			const char* const _playerName(_player.playerName);

			const char* const _authority[InstanceSystem::EMAUTHORITY_NSIZE] =
			{
				"normal", "normal_forced", "GM", "observer"
			};


			this->pGaeaClient->PrintMsgText(_colorNormal, 
				sc::string::format("%1%(%2%) : %3%", _playerName, _dbNum, _authority[_player.typeAuthority]));
		}

		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureNewCustomMessage(const NET_MSG_GENERIC* const _pMessage)
	{
		GLMSG::NET_INSTANCE_NEW_CUSTOM_MESSAGE* NetMsg((GLMSG::NET_INSTANCE_NEW_CUSTOM_MESSAGE* )_pMessage);
		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureSetMotion(const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_INSTANCE_SET_MOTION_BRD* const _pNetMsg((const GLMSG::NET_INSTANCE_SET_MOTION_BRD* const)_pMessage);
		if ( pGaeaClient->GETMYGAEAID() == _pNetMsg->dwGaeaID )
		{
			GLCharacter* const pCharacter(pGaeaClient->GetCharacter());
			if ( pCharacter )
			{
				if ( _pNetMsg->emMainType == UINT_MAX )
					pCharacter->ResetAction();
				else
					pCharacter->SetMotion((EMANI_MAINTYPE)_pNetMsg->emMainType, (EMANI_SUBTYPE)_pNetMsg->emSubType, _pNetMsg->fAniSec);
			}
		}
		else
		{
			std::tr1::shared_ptr<GLCharClient> spChar = pGaeaClient->GetChar(_pNetMsg->dwGaeaID);
			if ( spChar )
			{
				if ( _pNetMsg->emMainType == UINT_MAX )
					spChar->TurnAction(GLAT_IDLE);
				else
					spChar->SetMotion((EMANI_MAINTYPE)_pNetMsg->emMainType, (EMANI_SUBTYPE)_pNetMsg->emSubType, _pNetMsg->fAniSec);
			}
		}
		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureSetActState(const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_INSTANCE_SET_ACT_STATE_BRD* const _pNetMsg((const GLMSG::NET_INSTANCE_SET_ACT_STATE_BRD* const)_pMessage);
		if ( pGaeaClient->GETMYGAEAID() == _pNetMsg->dwGaeaID )
		{
			GLCharacter* pCharacter(pGaeaClient->GetCharacter());
			if ( pCharacter )
			{
				if ( _pNetMsg->bState )
					pCharacter->SetActState(_pNetMsg->dwActStateType);
				else
					pCharacter->ReSetActState(_pNetMsg->dwActStateType);
			}
		}
		else
		{
			std::tr1::shared_ptr<GLCharClient> spChar = pGaeaClient->GetChar(_pNetMsg->dwGaeaID);
			if ( spChar )
			{
				if ( _pNetMsg->bState )
					spChar->SetActState(_pNetMsg->dwActStateType);
				else
					spChar->ReSetActState(_pNetMsg->dwActStateType);
			}
		}
		return true;
	}

	const bool CALLBACK ManagerClient::_msgProc_RnCompetitionResult(const NET_MSG_GENERIC* const _pMessage)
	{
		GLMSG::NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE HeaderMsg;
		if( CastNetMsg( _pMessage, HeaderMsg ) == false )
			return false;

		GLWidgetScript& widgetScript = GLWidgetScript::GetInstance();
		switch( HeaderMsg.emType )
		{
		case InstanceSystem::EMMSG_COMPETITION_RESULT_TAB_FC :
			{
				// list 출력 전에 오는 메세지, 아이템 포맷에 대한 정보가 담겨 있다;
				GLMSG::NET_INSTANCE_COMPETITION_RESULT_TAB_FC NetMsg;
				if( CastNetMsg( _pMessage, NetMsg ) == false )
					return false;

				const std::vector< InstanceSystem::RnCompetitionResultTitleObj >& vecTitle = 
					NetMsg.sTabInfo.sTitle.vecTitleObj;

				switch( NetMsg.typeCompetition )
				{
				case InstanceSystem::COMPETITION_CDM :
					{
						if( 9 != vecTitle.size() )
							break;

						LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
						tb.set( 1, vecTitle[ 0 ].strTitleName.c_str() );
						tb.set( 2, vecTitle[ 1 ].strTitleName.c_str() );
						tb.set( 3, vecTitle[ 2 ].strTitleName.c_str() );
						tb.set( 4, vecTitle[ 3 ].strTitleName.c_str() );
						tb.set( 5, vecTitle[ 4 ].strTitleName.c_str() );
						tb.set( 6, vecTitle[ 5 ].strTitleName.c_str() );
						tb.set( 7, vecTitle[ 6 ].strTitleName.c_str() );
						tb.set( 8, vecTitle[ 7 ].strTitleName.c_str() );
						tb.set( 9, vecTitle[ 8 ].strTitleName.c_str() );

						GLWidgetCallFuncValue arg;
						arg.SetTable( &tb );

						widgetScript.LuaCallFunc< void >(
							NSWIDGET_SCRIPT::g_strFunc_Competition_CDM_Clear_List, &arg, 1 );
					}
					break;

				case InstanceSystem::COMPETITION_CTFLAG :
					{
						if( 8 != vecTitle.size() )
							break;

						LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
						tb.set( 1, vecTitle[ 0 ].strTitleName.c_str() );
						tb.set( 2, vecTitle[ 1 ].strTitleName.c_str() );
						tb.set( 3, vecTitle[ 2 ].strTitleName.c_str() );
						tb.set( 4, vecTitle[ 3 ].strTitleName.c_str() );
						tb.set( 5, vecTitle[ 4 ].strTitleName.c_str() );
						tb.set( 6, vecTitle[ 5 ].strTitleName.c_str() );
						tb.set( 7, vecTitle[ 6 ].strTitleName.c_str() );
						tb.set( 8, vecTitle[ 7 ].strTitleName.c_str() );

						GLWidgetCallFuncValue arg;
						arg.SetTable( &tb );

						widgetScript.LuaCallFunc< void >(
							NSWIDGET_SCRIPT::g_strFunc_CTFlagResult_SetTitle, &arg, 1 );
					}
					break;

				case InstanceSystem::COMPETITION_CTI :
					{
						if( 6 != vecTitle.size() )
							break;

						LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
						tb.set( 1, vecTitle[ 0 ].strTitleName.c_str() );
						tb.set( 2, vecTitle[ 1 ].strTitleName.c_str() );
						tb.set( 3, vecTitle[ 2 ].strTitleName.c_str() );
						tb.set( 4, vecTitle[ 3 ].strTitleName.c_str() );
						tb.set( 5, vecTitle[ 4 ].strTitleName.c_str() );
						tb.set( 6, vecTitle[ 5 ].strTitleName.c_str() );

						GLWidgetCallFuncValue arg;
						arg.SetTable( &tb );

						widgetScript.LuaCallFunc< void >(
							NSWIDGET_SCRIPT::g_strFunc_CTI_SetRewardTitle, &arg, 1 );
					}
					break;

				case InstanceSystem::COMPETITION_GUIDANCE :
					{
						if( 7 != vecTitle.size() )
							break;

						LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
						tb.set( 1, vecTitle[ 0 ].strTitleName.c_str() );
						tb.set( 2, vecTitle[ 1 ].strTitleName.c_str() );
						tb.set( 3, vecTitle[ 2 ].strTitleName.c_str() );
						tb.set( 4, vecTitle[ 3 ].strTitleName.c_str() );
						tb.set( 5, vecTitle[ 4 ].strTitleName.c_str() );
						tb.set( 6, vecTitle[ 5 ].strTitleName.c_str() );
						tb.set( 7, vecTitle[ 6 ].strTitleName.c_str() );

						GLWidgetCallFuncValue arg;
						arg.SetTable( &tb );

						widgetScript.LuaCallFunc< void >(
							NSWIDGET_SCRIPT::g_strFunc_GuidanceResult_SetTitle, &arg, 1 );
					}
					break;
				}
			}
			break;

		case InstanceSystem::EMMSG_COMPETITION_RESULT_INFO_FC:
			{
				// list 항목에 대한 정보;
				GLMSG::NET_INSTANCE_COMPETITION_RESULT_INFO_FC NetMsg;
				if( CastNetMsg( _pMessage, NetMsg ) == false )
					return false;

				const std::vector< InstanceSystem::RnCompetitionResultContentObj >& vecResult = 
					NetMsg.sContents.vecResultObj;

				switch( NetMsg.typeCompetition )
				{
				case InstanceSystem::COMPETITION_CDM :
					{
						if( 8 != vecResult.size() )
							break;

						LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
						tb.set( 1, NetMsg.sContents.dwKeyID );			// 클럽 ID
						tb.set( 2, vecResult[ 0 ].ContentItemInt );		// 순위
						tb.set( 3, vecResult[ 1 ].ContentItemInt );		// 학원
						tb.set( 4, vecResult[ 2 ].ContentItemString );	// 클럽 이름
						tb.set( 5, vecResult[ 3 ].ContentItemString );	// 클럼 마스터 이름
						tb.set( 6, vecResult[ 4 ].ContentItemInt );		// 획득 포인트
						tb.set( 7, vecResult[ 5 ].ContentItemInt );		// 사살
						tb.set( 8, vecResult[ 6 ].ContentItemInt );		// 죽음
						tb.set( 9, vecResult[ 7 ].ContentItemInt );		// 부활 수

						// 보상 아이템
						LuaTable tbReawrd( GLWidgetScript::GetInstance().GetLuaState() );
						if( 0 != NetMsg.sContents.vecRewardObj.size() )
						{
							typedef InstanceSystem::RnCompetitionResultContentRewardObj::RewardItem rewardItem;

							const std::vector< rewardItem > vecReward = NetMsg.sContents.vecRewardObj[ 0 ].vecItemReward;
							for( size_t i = 0; i < vecReward.size(); ++i )
							{
								const rewardItem& sReward = vecReward[ i ];

								LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
								tbItem.set( 1, sReward.itemID.Mid() );
								tbItem.set( 2, sReward.itemID.Sid() );
								tbItem.set( 3, sReward.wAmount );
								tbReawrd.set( i + 1, tbItem );
							}
						}
						tb.set( 10, tbReawrd );

						GLWidgetCallFuncValue arg;
						arg.SetTable( &tb );

						widgetScript.LuaCallFunc< void >(
							NSWIDGET_SCRIPT::g_strFunc_Competition_CDM_Add_List_Item, &arg, 1 );
					}					
					break;

				case InstanceSystem::COMPETITION_CTFLAG :
					{
						int nMe = ( NetMsg.sContents.dwKeyID == pGaeaClient->GetCharacter()->CharDbNum() ) ? 1 : 0;

						LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
						tb.set( 1, vecResult[ 0 ].ContentItemInt );		// Win / Lose
						tb.set( 2, vecResult[ 1 ].ContentItemInt );		// 학원
						tb.set( 3, vecResult[ 2 ].ContentItemInt );		// 클래스
						tb.set( 4, vecResult[ 3 ].ContentItemString );	// 캐릭터 이름
						tb.set( 5, vecResult[ 4 ].ContentItemInt );		// 깃발 탈취 수
						tb.set( 6, vecResult[ 5 ].ContentItemInt );		// 사살 수
						tb.set( 7, vecResult[ 6 ].ContentItemInt );		// 사망 수
						tb.set( 8, nMe );

						LuaTable tbReawrd( GLWidgetScript::GetInstance().GetLuaState() );
						if( 0 != NetMsg.sContents.vecRewardObj.size() )
						{
							typedef InstanceSystem::RnCompetitionResultContentRewardObj::RewardItem rewardItem;

							const std::vector< rewardItem > vecReward = NetMsg.sContents.vecRewardObj[ 0 ].vecItemReward;
							for( size_t i = 0; i < vecReward.size(); ++i )
							{
								const rewardItem& sReward = vecReward[ i ];
								
								LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
								tbItem.set( 1, sReward.itemID.Mid() );
								tbItem.set( 2, sReward.itemID.Sid() );
								tbItem.set( 3, sReward.wAmount );
								tbReawrd.set( i + 1, tbItem );
							}
						}
						tb.set( 9, tbReawrd );

						GLWidgetCallFuncValue arg;
						arg.SetTable( &tb );

						widgetScript.LuaCallFunc< void >(
							NSWIDGET_SCRIPT::g_strFunc_CTFlagResult_AddInfo, &arg, 1 );
					}
					break;

				case InstanceSystem::COMPETITION_CTI :
					{
						if( 6 != vecResult.size() )
							break;

						LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
						tb.set( 1, NetMsg.sContents.dwKeyID );
						tb.set( 2, vecResult[ 0 ].ContentItemInt );
						tb.set( 3, vecResult[ 1 ].ContentItemInt );
						tb.set( 4, vecResult[ 2 ].ContentItemString );
						tb.set( 5, vecResult[ 3 ].ContentItemInt );
						tb.set( 6, vecResult[ 4 ].ContentItemInt );
						tb.set( 7, vecResult[ 5 ].ContentItemInt );

						GLWidgetCallFuncValue arg;
						arg.SetTable( &tb );

						widgetScript.LuaCallFunc< void >(
							NSWIDGET_SCRIPT::g_strFunc_CTI_AddRewardData, &arg, 1 );

					}
					break;

				case InstanceSystem::COMPETITION_GUIDANCE :
					{
						if( 7 != vecResult.size() )
							break;

						LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
						tb.set( 1, vecResult[ 0 ].ContentItemInt );
						tb.set( 2, vecResult[ 1 ].ContentItemInt );
						tb.set( 3, vecResult[ 2 ].ContentItemString );
						tb.set( 4, vecResult[ 3 ].ContentItemString );
						tb.set( 5, vecResult[ 4 ].ContentItemInt );
						tb.set( 6, vecResult[ 5 ].ContentItemInt );
						tb.set( 7, vecResult[ 6 ].ContentItemInt );

						GLWidgetCallFuncValue arg;
						arg.SetTable( &tb );

						widgetScript.LuaCallFunc< void >(
							NSWIDGET_SCRIPT::g_strFunc_GuidancResult_AddInfo, &arg, 1 );
					}
					break;
				}
			}
			break;

		case InstanceSystem::EMMSG_COMPETITION_RESULT_END_INFO_FC:
			{
				// list 전송이 모두 끝나면 오는 메세지;
				GLMSG::NET_INSTANCE_COMPETITION_RESULT_END_INFO_FC NetMsg;
				if( CastNetMsg( _pMessage, NetMsg ) == false )
					return false;

				this->nCloseButtonParameter = NetMsg.nCloseButtonParam;

				const int nOutTime = static_cast< float >( NetMsg.fOutTime );
				switch( NetMsg.typeCompetition )
				{
				case InstanceSystem::COMPETITION_CDM :
					{
						widgetScript.LuaCallFunc< void >(
							NSWIDGET_SCRIPT::g_strFunc_Competition_CDM_Open,
							"-n", nOutTime );
					}
					break;

				case InstanceSystem::COMPETITION_CTFLAG :
					{
						widgetScript.LuaCallFunc< void >(
							NSWIDGET_SCRIPT::g_strFunc_CTFlagResult_Open,
							"-n", nOutTime );
					}
					break;

				case InstanceSystem::COMPETITION_CTI :
					{
						widgetScript.LuaCallFunc< void >(
							NSWIDGET_SCRIPT::g_strFunc_CTI_OpenReward,
							"-n", nOutTime );
					}
					break;

				case InstanceSystem::COMPETITION_GUIDANCE :
					{
						widgetScript.LuaCallFunc< void >(
							NSWIDGET_SCRIPT::g_strFunc_GuidanceResult_Open,
							"-n", nOutTime );
					}
					break;

				default :
					{
						widgetScript.LuaCallFunc< void >(
							NSWIDGET_SCRIPT::g_strFunc_DungeonExit_Show,
							"-n", nOutTime );
					}
					break;
				}
			}
			break;
		}		
	}

}