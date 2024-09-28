#pragma once

#include "../MatchingManager.h"
#include "../../../RanLogic/Tournament/GLTournamentInfoDef.h"

#include <vector>

namespace MatchingSystem
{
	
	class GLTournamentBetting
	{
	public:
		GLTournamentBetting( MatchingGameBase* parent,sc::LuaInstance* script );
		~GLTournamentBetting();
		void ResualtWinner(DWORD Groupid);


		//Agent에는 돈을 관리 못하므로 Field로 떨어졌다가 온다.
		void SetBetting(DWORD ClientDbid, DWORD GroupId, LONGLONG BettingMoney, DWORD LeageID);
		bool CanselBetting(DWORD ClientDbid);
		void FieldCheckMoneyOK(DWORD ClientDbid,BOOL Isok);
		void FieldSpendMoneyOK(DWORD ClientDbid,BOOL Isok);


		void RequestMyBettingInfo(DWORD ClientDbid);

		void RequestGroupInfo(DWORD ClientDbid);
		void SendTournamentBettingRaito(DWORD ClientDbid,DWORD LeageId);
		void SendTournamentGroupInfo(DWORD ClientDbid,DWORD Groupid);

		float CarcurateBettingRatio(DWORD Groupid);

		void PastBettingUserProcess(DWORD outChaDbNum, LONGLONG outBettingCount);
 
		void SetCanBetting(bool CanBetting) { IsBettingTime= CanBetting;}
		bool GetCanBetting() { return IsBettingTime; }
		void registLuaCallBack( sc::LuaInstance* script );
		std::vector<BettingTable> m_BettingTable;

		std::vector<BettingTable> m_BettingApointTable;

		typedef std::vector<BettingRatio> BETTINGINFO_VEC;
		typedef BETTINGINFO_VEC::iterator				BETTINGINFO_ITER;
		typedef BETTINGINFO_VEC::const_iterator			BETTINGINFO_CITER;

		BETTINGINFO_VEC m_BettingRatioTable;

	private:
		MatchingGameBase* m_parent;
		sc::LuaInstance* m_script;

		LONGLONG Winner_Bet_Money_Min;
		LONGLONG Winner_Bet_Money_Max;

		DWORD Winner_Bet_Money_Type;

		float Winner_forecast_fee;
		bool IsBettingTime;
	};


}