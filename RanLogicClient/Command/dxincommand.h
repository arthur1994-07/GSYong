#ifndef _DX_INCOMMAND_H_
#define _DX_INCOMMAND_H_

#pragma once

#include <string>
#include <vector>
#include <boost/thread.hpp>

class GLGaeaClient;

namespace dxincommand
{
    extern bool bDISP_FPS;
    extern bool bDISP_CONSOLE;
	extern bool bDISP_POSITION;
} // namespace dxincommand

class GameCommand
{
public:
    GameCommand(GLGaeaClient* pGaeaClient);
    ~GameCommand();

private:
    typedef std::vector<std::string> splitVector;
    typedef splitVector::iterator    splitVectorIter;
    
    GLGaeaClient* m_pGaeaClient;

	typedef boost::function<bool(splitVector& SplitVec)> COMMAND_FUNC;
	typedef std::map<std::string, COMMAND_FUNC> COMMAND_DATA;
	typedef COMMAND_DATA::iterator              COMMAND_DATA_ITER;
	typedef COMMAND_DATA::value_type            COMMAND_DATA_VALUE;

	COMMAND_DATA m_GmCommandData;
	COMMAND_DATA m_UserGmCommandData;

public:
    bool command( const std::string& Command );
	void commandlog( splitVector& command );
    
private:
    CString BOOL2STR(bool bOn);

	bool gm( splitVector& SplitVec );

    bool PointShop(splitVector& SplitVec);
    bool Db(splitVector& SplitVec);
    bool Dsplay(splitVector& SplitVec);
    bool Render(splitVector& SplitVec);
    bool ShopInfo(splitVector& SplitVec);
    bool Ping(splitVector& SplitVec);
    bool PingTrace(splitVector& SplitVec);
    bool AttendanceMaintenance(splitVector& SplitVec);
    bool CTF(splitVector& SplitVec);
	bool MatchTest(splitVector& SplitVec);
    bool PostTake(splitVector& SplitVec);
    bool FpsLimit(splitVector& SplitVec);
    bool Profile(splitVector& SplitVec);
	bool NetworkProfile(splitVector& SplitVec);
    bool ChatLog(splitVector& SplitVec);
    bool ShowMeTheMoney(splitVector& SplitVec);
    bool Whysoserious(splitVector& SplitVec);
    bool Weather(splitVector& SplitVec);     
    bool Time(splitVector& SplitVec);
    bool Month(splitVector& SplitVec);
    bool ItemHold(splitVector& SplitVec);
    bool MoveToGate(splitVector& SplitVec);
    bool MoveToPos(splitVector& SplitVec);
    bool MoveToCharCn(splitVector& SplitVec);
    bool MoveToCharCi(splitVector& SplitVec);
    bool MsgView(splitVector& SplitVec);
    bool InstNum(splitVector& SplitVec);
    bool GenCharCn(splitVector& SplitVec);
    bool GenCharCi(splitVector& SplitVec);
    bool Visible(splitVector& SplitVec);
    bool Whisper(splitVector& SplitVec);
    bool KickUserCi(splitVector& SplitVec);
	bool KickUserCn(splitVector& SplitVec);
    bool EventExpBegin(splitVector& SplitVec);
    bool EventExpEnd(splitVector& SplitVec);
    bool LimitEventBegin(splitVector& SplitVec);
    bool LimitEventEnd(splitVector& SplitVec);
    bool EventItemBegin(splitVector& SplitVec);
    bool EventItemEnd(splitVector& SplitVec);
    bool EventMoneyBegin(splitVector& SplitVec);
    bool EventMoneyEnd(splitVector& SplitVec);
    bool EventGradeBegin(splitVector& SplitVec);
    bool EventGradeEnd(splitVector& SplitVec);
    bool ClassEventBegin(splitVector& SplitVec);
    bool ClassEventEnd(splitVector& SplitVec);
    bool Club(splitVector& SplitVec);
    bool PkMode(splitVector& SplitVec);
    bool NonConfrontOn(splitVector& SplitVec);    
    bool NonConfrontOff(splitVector& SplitVec);
    bool NonConfrontView(splitVector& SplitVec);
    bool WhereNpc(splitVector& SplitVec);
    bool WhereNpcAll(splitVector& SplitVec);
    bool CrowList(splitVector& SplitVec);
    bool BigHead(splitVector& SplitVec);
    bool BigHand(splitVector& SplitVec);
    bool MobGen(splitVector& SplitVec);
    bool MobGenEx(splitVector& SplitVec);
    bool MobDel(splitVector& SplitVec);
    bool MobDelEx(splitVector& SplitVec);        
    bool ChatBlockUa(splitVector& SplitVec);
    bool ChatBlockCn(splitVector& SplitVec);
    bool ChatBlockCi(splitVector& SplitVec);
    bool CharInfoCn(splitVector& SplitVec);
    bool CharInfoCi(splitVector& SplitVec);
    bool FreePkOn(splitVector& SplitVec);
    bool FreePkOff(splitVector& SplitVec);
    bool WarningMessage(splitVector& SplitVec);
    bool EventExBegin(splitVector& SplitVec);
    bool EventExEnd(splitVector& SplitVec);
    bool Login(splitVector& SplitVec);
    bool UiReload(splitVector& SplitVec);
    bool CreateInst(splitVector& SplitVec);
    bool Summon(splitVector& SplitVec);
	bool SummonEventSkill(splitVector& SplitVec);
    //bool Server(splitVector& SplitVec);
    bool ReferCharCommand(splitVector& SplitVec);
    bool ItemRepairCommand(splitVector& SplitVec);
    bool PrivateMarketCommand(splitVector& SplitVec);
    bool For(splitVector& SplitVec);
    bool SafeModeTestOn(splitVector& SplitVec);
    bool Check(splitVector& SplitVec);
    bool Whois(splitVector& SplitVec);
    void Help(splitVector& SplitVec);
    bool WorldBattle(splitVector& SplitVec);
    bool LogItemReload(splitVector& SplitVec);
    bool ServerItemListUpdate(splitVector& SplitVec);
	bool ServerQuestListUpdate(splitVector& SplitVec);
	bool ServerSkillListUpdate(splitVector& SplitVec);
    bool Script(splitVector& SplitVec);
	bool ClubMasterMandate(splitVector& SplitVec);
	bool FreePkGMCommand(splitVector& SplitVec);
    bool Log2DB(splitVector& SplitVec);

    bool Hack(splitVector& SplitVec);
    bool Protocol(splitVector& SplitVec);
    bool Instance(splitVector& SplitVec);
    bool MiniGame(splitVector& SplitVec);
	bool DPS(splitVector& SplitVec);
    bool DbUnlock(splitVector& SplitVec);
    bool GuidanceInfo(splitVector& SplitVec);
    bool ShowContentId(splitVector& SplitVec);

	bool NetNsgDelay( splitVector& SplitVec );
	bool PrivateMarket( splitVector& SplitVec );
	bool CDMOnOff( splitVector& SplitVec );
	bool ReferenceCount( splitVector& SplitVec );

    bool LoadIPEvent( splitVector& SplitVec );
    bool SetIPEvent( splitVector& SplitVec );

    bool AcquireEXP( splitVector& SplitVec );

	bool ReloadTrigger( splitVector& SplitVec );
	bool GoStage( splitVector& SplitVec );

	bool TexasHoldem( splitVector& SplitVec );

	// Lotto System -------------------------------------
	bool LottoSystemID( splitVector& SplitVec );
	bool LottoViewSystemID( splitVector& SplitVec );
	bool LottoSystemChangeState( splitVector& SplitVec );
	bool LottoSystemNumDSP( splitVector& SplitVec );
	bool LottoSystemNum( splitVector& SplitVec );
	bool LottoSystemNumInit( splitVector& SplitVec );
	bool LottoSystemNumAdd( splitVector& SplitVec );
	bool LottoSystemNumDel( splitVector& SplitVec );

	bool LottoSystemViewDrawingType( splitVector& SplitVec );
	bool LottoSystemSetWinNum( splitVector& SplitVec );
	bool LottoSystemResetWinNum( splitVector& SplitVec );

	bool LottoSystemCantBuy( splitVector& SplitVec );
	bool LottoSystemCanBuy( splitVector& SplitVec );
	bool LottoSystemTurnControl( splitVector& SplitVec );

	// Lotto System Develop Mode ------------------------
	bool LottoBuyTicket( splitVector& SplitVec );
	bool LottoAccumulateMoney( splitVector& SplitVec );
	bool LottoViewBuyList( splitVector& SplitVec );
	bool LottoViewDrawingDate( splitVector& SplitVec );
	bool LottoWinNum( splitVector& SplitVec );
	bool LottoViewWinManList( splitVector& SplitVec );
	// --------------------------------------------------

	// Reload AI FSM ------------------------------------
	bool ReloadAIFSM( splitVector& SplitVec );
	// --------------------------------------------------

	// Change Basic HireSummon --------------------------
	bool ChangeBasicHireSummon( splitVector& SplitVec );
	// --------------------------------------------------

	// Show Faction Information -------------------------
	bool ShowFactionInformation( splitVector& SplitVec );
	// --------------------------------------------------

	bool Matching(splitVector& SplitVec);
	bool AutoLevel(splitVector& SplitVec);
	bool MapLayer(splitVector& SplitVec);

	bool Land(splitVector& SplitVec);

	bool FlyCameraControlOn(splitVector& SplitVec);
	bool FlyCameraControlCommand(splitVector& SplitVec);

	// PostRenew Command
	bool PostRenew(splitVector& SplitVec);

	bool Event(splitVector& SplitVec);

	bool CountRecorder(splitVector& splitVec);

	bool SetNotifyMoney( splitVector& SplitVec );

	// 학원 변경
	bool ChangeSchool( splitVector& SplitVec );

	bool TransDB2WorldBattle( splitVector& SplitVec );
	
	bool JumpingLevelUp( splitVector& SplitVec );
	bool ShowLog ( splitVector& SplitVec );

	bool ChangeWBJoinState( splitVector& SplitVec );
	bool Scaleform_ProfilingOn(splitVector& SplitVec);

	bool QuestSelfStart( splitVector& SplitVec );
	
    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.14
     * modified : 
     */
    bool SetRandomboxChance(splitVector& SplitVec);
    bool GetRandomboxChance(splitVector& SplitVec);
    bool DelRandomboxChance(splitVector& SplitVec);

	// 란모바일
	bool RanMobileCommand(splitVector& SplitVec);
	bool RanMobileCommandEx(splitVector& SplitVec);

	// Information Command;
	bool InformationCommand ( splitVector& _vecSplitCommand );

	// CharSlot Command;
	bool CharSlotCommand ( splitVector& _vecSplitCommand );

	// Jackpot
	bool Jackpot( splitVector& SplitVec );

	// GM이 있는 맵의 모든 캐릭터들의 점령전 버프 삭제
	//bool ReleaseCTFBuff( splitVector& SplitVec );

    //! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
    bool RandomboxRCReload(splitVector& SplitVec);

public:
    boost::thread m_TestThread;
    void Test();
    void HackChat4();
}; // class GameCommand

#endif // _DX_INCOMMAND_H_
