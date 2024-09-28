#pragma once

#include "InstanceSystemClientDefine.h"

#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"

namespace InstanceSystem
{
    class ManagerClient
    {
	private:
        friend class GLGaeaClient;

	public:
		ManagerClient ( GLGaeaClient* const _pGaeaClient );
		~ManagerClient ( void );

		const ManagerClient& operator = ( const ManagerClient& _rhs ) { }

	private:
		//--------- Instance System Message Procedure -----------
		// �Ʒ��� Message ó�� ����� ���� ��Ŷ����͸��� �ϱ� ����� �����̸�;
		// ���� �� �ý��۰��� ���� �ʱ� ������ ��Ŷ����͸��� �� �� �ִ� ó���������;
		// �ٲٴ� ���� �����ϴ�;
		const bool		CALLBACK OnMessageProc ( const GLMSG::NET_INSTANCE_MESSAGE_HEADER* pMsg );

		//--------- Frame Move ----------------------------------
		void			CALLBACK OnFrameMove ( float fElapsedTime, const CTime& sCurrentTime );

	private:
		// ��Ŷ����͸��� �� �� �ֵ��� �����Ǵ� ��Ŷ�� �̰��� �����Ѵ�;
		// ��� ��Ŷ�� �����ϱ⿣ �۾����� ���� ���ݾ� �����س����� �Ѵ�;
		const bool		CALLBACK MsgRetryFactionFB ( const GLMSG::NET_RETRY_FACTION_FB* pMsg );
		const bool		CALLBACK MsgShowTimeUIFB ( const GLMSG::NET_SHOW_TIME_UI_FB* pMsg );
		const bool		CALLBACK MsgShowEntranceStateUIFB ( const GLMSG::NET_SHOW_ENTRANCE_STATE_UI_FB* pMsg );

	private:
		/// ��ŷ ����͸��� �ϱ� ����� Message ó�����;
        //--------- instance system GenericMsg ----------------
        const bool		CALLBACK _messageProcedure(const NET_MSG_GENERIC* const _pMessage);

        //--------- instance system MsgPack -------------------
        const bool		CALLBACK _messageProcedureMsgPack(const NET_MSG_GENERIC* const _pMessage);

        //--------- instance contents GenericMsg ----------------
        const bool		CALLBACK _messageProcedure_Contents(const NET_MSG_GENERIC* const _pMessage);

        //--------- instance contents MsgPack -------------------
        const bool		CALLBACK _messageProcedureMsgPack_Contents(const NET_MSG_GENERIC* const _pMessage);

	private:
		const bool		CALLBACK _messageProcedureError(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureNotifyPlayerPosition(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureNotifyInstanceLog(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureNotifyInstanceList(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureNotifyChildLand(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureNotifyPlayerList(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureNewCustomMessage(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureSetMotion(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureSetActState(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureUIChatMsg(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureUIChatXmlMsg(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureUISimpleMsg(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureUISimpleXmlMsg(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureUITextDlgMsg(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureUITextDlgXmlMsg(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureUITimerMsgBox(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureEffect(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _messageProcedureNotifyCountPacket(const NET_MSG_GENERIC* const _pMessage);

	private:
		// Rn CDM message procedure
		const bool		CALLBACK _msgProc_RNCDM_ProgressUpdate(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _msgProc_RNCDM_ProcessOff(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _msgProc_RNCDM_RankInfo(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _msgProc_RNCDM_RankShow(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _msgProc_RNCDM_MapInfo(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _msgProc_RNCDM_MapImageName(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _msgProc_RNCDM_BattleState(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _msgProc_RNCDM_StartLogoShow(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _msgProc_RNCDM_EndLogoShow(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _msgProc_RNCDM_TopClubInfo(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _msgProc_RNCDM_DoublePoint(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _msgProc_RNCDM_PreviousInfo(const NET_MSG_GENERIC* const _pMessage);

		// Capture The Flag procedure
		const bool		CALLBACK _msgProc_CaptureTheFlag_FlagUpdate(const NET_MSG_GENERIC* const _pMessage);

		// RnCompetitionResultUI procedure
		const bool		CALLBACK _msgProc_RnCompetitionResult(const NET_MSG_GENERIC* const _pMessage);

		// Guidance Club Battle
		const bool		CALLBACK _msgProc_Guidance_UI_Auth(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _msgProc_Guidance_UI_Progress(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _msgProc_Guidance_UI_Rank(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _msgProc_Guidance_UI_Rank_MyInfo(const NET_MSG_GENERIC* const _pMessage);
		const bool		CALLBACK _msgProc_Guidance_UI_AuthInfo(const NET_MSG_GENERIC* const _pMessage);

		// CTI
		const bool		CALLBACK _msgProc_CTI_UI_Auth ( const NET_MSG_GENERIC* const pMsg );
		const bool		CALLBACK _msgProc_CTI_UI_Progress ( const NET_MSG_GENERIC* const pMsg );
		const bool		CALLBACK _msgProc_CTI_UI_Rank ( const NET_MSG_GENERIC* const pMsg );
		const bool		CALLBACK _msgProc_CTI_UI_Rank_MyInfo ( const NET_MSG_GENERIC* const pMsg );
		const bool		CALLBACK _msgProc_CTI_UI_AuthInfo ( const NET_MSG_GENERIC* const pMsg );

	public :
		void requestClose(void);

    private:
        GLGaeaClient* const pGaeaClient;
		unsigned int nCloseButtonParameter;

		// �絵��;
		InstanceSystem::InstanceMapID m_nRetryInstanceID;
		SNATIVEID m_sRetryKeyMapID;

	public:
		inline const InstanceSystem::InstanceMapID GetRetryInstanceID () const { return m_nRetryInstanceID; }
		inline const SNATIVEID GetRetryKeyMapID () const { return m_sRetryKeyMapID; }

    

    private:
		/// Wrapping Function;
        void _printGuideLine(void);
        const bool InstanceXMLStringToString(const InstanceXmlString& _xmlString, std::string& _msgString);
    };
}
