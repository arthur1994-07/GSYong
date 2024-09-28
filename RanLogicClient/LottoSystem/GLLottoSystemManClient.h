#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"
#include "../../RanLogic/LottoSystem/ILottoSystemMan.h"

#include "./LottoSystemClient.h"

#include "../GLGaeaClient.h"

namespace LottoSystem
{

	// Lotto System�� Client ������;
	// Lotto System ���� �����Ѵ�;
	// Lotto System ���� ����ϴ� ��ɵ��� �����Ѵ�;
	class GLLottoSystemManClient : public ILottoSystemMan, public CSingletone<GLLottoSystemManClient>
	{
	protected:
		friend CSingletone;

	public:
		GLLottoSystemManClient();
		~GLLottoSystemManClient();

	public:
		virtual void Initialize();
		virtual void Destroy();

		virtual bool Update( float fElapsedTime, const CTime& currentTime );
		virtual bool OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	public:
		// Agent Server���� ���;
		void SendToAgentServer( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

		// Field Server���� ���;
		void SendToFieldServer( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

	public:
		const int GetConfirmBuyListCount();
		CLottoSystemClient* GetLottoSystemUsingTabIdx( unsigned int useIdx );

	public:
		// GM ��ɾ�;
		bool GMCommandSetLottoSystemID( unsigned int lottoSystemId );
		bool GMCommandViewLottoSystemID();
		bool GMCommandChangeState();
		bool GMCommandNumDSP( unsigned int num = 0 );
		bool GMCommandNum( unsigned int num = 0 );
		bool GMCommandNumInit();
		bool GMCommandNumAdd( unsigned int num, unsigned int count );
		bool GMCommandNumDel( unsigned int num, unsigned int count );

		bool GMCommandViewDrawingType();
		bool GMCommandSetWinNum( const std::string& strWinNum );
		bool GMCommandResetWinNum();

		bool GMCommandCantBuy();
		bool GMCommandCanBuy();
		bool GMCommandChangeTurnOn();
		bool GMCommandChangeTurnOff();

		// GM ��ɾ� ( Develop Mode );
		bool GMCommandBuyTicket();
		bool GMCommandAccumulateMoney();
		bool GMCommandViewBuyList();
		bool GMCommandViewDrawingDate();
		bool GMCommandViewWinNum();
		bool GMCommandViewWinManList();

	public:
		// Gaea Client�� ����� Ȱ���ϱ� ���Ͽ� �������ش�;
		inline void SetGaeaClient( GLGaeaClient* pGaeaClient ) { m_pGaeaClient = pGaeaClient; }

		// Getter;
		// GM ��ɾ ����� Lotto System�� ID;
		inline const unsigned int GetGmCommandLottoSystemId() const { return m_gmCommandLottoSystemId; }

		inline const unsigned int GetLottoSystemCount() const { return m_pLottoSystems.size(); }
		inline CLottoSystemClient* GetLottoSystem( LOTTO_SYSTEM_ID id )
		{
			if( id >= 0 && id < m_pLottoSystems.size() )
				return m_pLottoSystems.at( id );

			return NULL;
		}

		/// Client�� ��ü �α׸� ����� ���� �ӽ� ���;
		/// Log �̿��� ������δ� ������� ����;
		/// UI�ʿ��� �Ѹ����� ������ �̹� �ִ� �ڵ带 Ȱ���ߴ�;
		inline void ConsoleLog( const std::string& log ) { m_pGaeaClient->PrintConsoleText( log ); }

	private :
		CLottoSystemClient* FindSystem( unsigned int nID );

	public:
		// �ζ� ���� ȸ�� ����Ʈ
		LuaTable GetPrevTurnList( unsigned int nID );
		// Ư�� ȸ�� ��÷ ����
		LuaTable GetPrevTurnWinnerList( unsigned int nID );
		// ���� ȸ�� �����ݾ�
		LONGLONG GetAccumulateMoney( unsigned int nID );
		// ���� �������� ȸ��		
		int GetCurTurnNum( unsigned int nID );
		// �ζ� ���۽ð�
		LuaTable GetStartTime( unsigned int nID );
		// �ִ� ���� ����
		int GetBuyCountMax( unsigned int nID );
		// Total Num;
		int GetTotalNum ( unsigned int nID );
		// ���� ����
		int GetBuyCount( unsigned int nID, bool bConfirm );
		// ��÷�� ����������
		bool GetLockBuyState( unsigned int nID );
		// �ζ� ���� ���
		LONGLONG GetBuyCost( unsigned int nID );
		// �ζ� ���� ��ȣ 
		LuaTable GetAutoNum( unsigned int nID );
		//���� ȸ���� ���� ���Ÿ���Ʈ
		LuaTable GetBuyList( unsigned int nID, bool bConfirm );
		//�ζ� Ȯ������ ���� ȸ�� ��÷ ��ȣ 
		LuaTable GetConfirmNum( unsigned int nID );

	public:
		void ReqPreWinNum( unsigned int nID, unsigned int nTurnNum );
		void ReqPreWinManList( unsigned int nID, unsigned int nTurnNum );
		void ReqPreAccumulateMoney( unsigned int nID, unsigned int nTurnNum );
		CLottoSystemClient::BUY_LOTTO_FLAG ReqBuyLotto( unsigned int nID, LuaTable tbNum, bool bAuto );
		CLottoSystemClient::BUY_LOTTO_FLAG ReqBuyLottoList ( unsigned int nID, unsigned int nCount );
		void ReqPreBuyList( unsigned int nID, unsigned int nTurnNum );
		void ReqConfirmBuyList( unsigned int nID );

		int GetLottoSystemState ( unsigned int nID );
		int GetViewNumCharName ();

	private:
		// �� Lotto System;
		std::vector<CLottoSystemClient*> m_pLottoSystems;

		// Gaea Client;
		GLGaeaClient* m_pGaeaClient;

		// GMCommand�� ���� Lotto System ID;
		unsigned int m_gmCommandLottoSystemId;
	};

}