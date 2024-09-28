#ifndef _EMUL_DB_MAN_H_
#define _EMUL_DB_MAN_H_

#include "DbManager.h"

//	에뮬 실행시 dbman 실제로 db랑 연결을 하지 않음, 메모리 릭 방지용.
//
class GLEmulatorDBMan : public db::IDbManager
{
public:
    GLEmulatorDBMan(NetServer* pServer, db::SERVER_TYPE ServerType, EMSERVICE_PROVIDER ServiceProvider);
    virtual ~GLEmulatorDBMan();

public:
    virtual bool IsAlive() override { return true; }    
    virtual bool Reconnect() override { return true; }
    virtual bool CheckHdd() override { return true; }
    virtual bool IsAllDbJobCompleted() override { return true; }
    virtual void WaitAllDbJobCompleted(DWORD WaitLimitTime) override { return; }

    //! -----------------------------------------------------------------------
    //! 호환성 유지를 위해서 임시로...
    virtual int	OdbcOpenGameDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override { return 1; }
    virtual int	OdbcOpenUserDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override { return 1; }
    virtual int	OdbcOpenBoardDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override { return 1; }
    virtual int	OdbcOpenLogDB  (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override { return 1; }
    virtual int OdbcOpenShopDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override { return 1; }
    virtual int	OdbcOpenTerraDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override { return 1; }
    virtual int	OdbcOpenGspDB  (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override { return 1; }
    virtual int	OdbcOpenThaiDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override { return 1; }
    virtual int	OdbcOpenKorDB  (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override { return 1; }
    //! 말레이시아 PC방 이벤트
    virtual int	OdbcOpenMyDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override { return 1; }

    //! Ado
    virtual bool SetConnectionString(db::DB_TYPE Type, const std::string& Ip, const std::string& DbName, const std::string& Id, const std::string& Password, int AdoMaxPoolSize, int DatabasePacketSize) { return false; }
    //! -----------------------------------------------------------------------

	virtual int UpdateUserLockerOption( DWORD UserNum, int Option ) override { return 0; }
	virtual int SaveCharacter( LPVOID _pbuffer, bool bAutoSave = false ) override { return 0; }
    virtual int SaveCharacterBase(SCHARDATA2* pCharData2, DWORD ChaDbNum) override { return 0; }
	virtual int SaveCharacterExtraWithAdo(LPVOID _pbuffer) override { return 0; }	//mjeon.ado

	virtual int SaveInventoryAndGameMoney( LPVOID _pbuffer ) { return 0; }

	virtual int	ReadUserInven( SCHARDATA2* pChaData2 ) { return 0; }
	virtual int	ReadUserInvenNew( SCHARDATA2* pChaData2, int FirstLoad = 0 ) { return 0; }

    //! Product ---------------------------------------------------------------
    virtual int AddProductType(const DWORD dwChaNum, const WORD nProductType) override { return 0;  };
    virtual int GetProductType(const DWORD dwChaNum, std::vector<WORD>& vecProductType OUT) override { return 0;  };
    virtual int DelProductType(const DWORD dwChaNum, const WORD nProductType) override { return 0;  };

    virtual int AddProductIsGetExp(const DWORD dwChaNum, const DWORD dwProductID) override { return 0;  };
    virtual int GetProductIsGetExp(const DWORD dwChaNum, std::vector<WORD>& vecProductID OUT) override { return 0;  };
    virtual int DelProductIsGetExp(const DWORD dwChaNum, const PRODUCT_ITEM& productItem) override { return 0;  };

    virtual int SaveProductBook(DWORD nChaNum, const std::vector<DWORD>& setLearnProductBook) override { return 0;  };
    virtual int LoadProductBook(DWORD nChaNum, std::vector<DWORD>& setLearnProductBook OUT) override { return 0;  };
    virtual int DeleteProductBook(DWORD nChaNum, const std::vector<PRODUCT_ITEM>& vecBook) override { return 0; };

	virtual int CharProductLog( const CHAR_PRODUCT_LOG& productLog ) { return 0; }
	virtual int ProductExchangeLog( const PRODUCT_EXCHANGE_LOG& productLog ) { return 0; }

	virtual int DelChaFriend(int nChaP, int nChaS) override { return 0; }
	virtual int AddChaFriend(int nChaP, int nChaS) override { return 0; }
	virtual int SetChaFriend(int nChaP, int nChaS, int nFlag) override { return 0; }
    virtual int ChaFriendFlagSelect(int ChaP, int ChaS, int& Flag) override { return 0; }

	//
	//mjeon.Ping
	//
	virtual int AddLogPing( int nUserNum, int nRTT, const char* szIP, __time64_t tTime ) { return 0; }
	
	//
	//mjeon.Post
	//
	virtual int PostSend(SPOSTINFO &PostInfo, SITEMCUSTOM &itemcustom) { return 0; }
	virtual int PostSendRollBack(LONGLONG &llPostID) { return 0; }
	virtual int PostOpenAndTake(LONGLONG llPostID, BOOL bMoneyAccept, BOOL bAttach01Accept, BYTE bChargePayment, BYTE bReturnedPost ) { return 0; }
	virtual int PostSendCharge( SAPOST &aPOST, DWORD dwOpenAndTakeUserNum ) { return 0; }

	//virtual int PostCheckAttachMoney(DWORD dwClientID, SPOSTINFO &PostInfo, SINVENITEM *pItem) { return 0; }

	virtual int LogPartyMatch( int nSGNum, int nSvrNum, WORD wWin, WORD wLost ) { return 0; }

    virtual LONGLONG GetItemMaxNum(int nSGNum, int nSvrNum,	int nFldNum, std::vector<VIEWLOGITEMEXCHANGEMAX> &v ) { return 0; }
    virtual int GetLogItem(std::vector<DWORD>& LogItemData) override { return 0; }
	virtual int LogItemExchange( const LOG_ITEM_EXCHANGE &sEXCHANGE ) override { return 0; }
	virtual int LogMoneyExchange( const LOG_MONEY_EXCHANGE &_sLOG ) override { return 0; }
	virtual int LogPointShopExchange(int nFromType, DWORD ChaDbNum, int nExchangeFlag, WORD ItemMid, WORD ItemSid, int nPoint, int nExchangeFlagDetail ) override { return 0; };
    virtual int LogItemConversion( const LOG_ITEM_CONVERSION &_sLOG ) override { return 0; }
    virtual int LogAction(const LOG_ACTION& LogData) { return 0; }
	
	//	선도전 로그
	virtual int LogGuidance( const LOG_GUIDANCE &sLog )	{ return 0; }

	//	CDM 로그
	virtual int LogCDM( const LOG_CDM &sLog )	{ return 0; }

	//	CDM 로그 ( 최종 ) 
	virtual int LogCDMReuslt( const LOG_CDM_RESULT &sLog )	{ return 0; }
		
    virtual int GetPurchaseItem(const CString& strUID, std::vector<SHOPPURCHASE>& v) override { return 0; }

	virtual int SetPurchaseItem(const CString& strPurKey, int nFlag=1) override { return 0; }

	virtual int UserChaNumIncrease(int nUserNum) override { return 0; }

    virtual int SetChaStorageDate(
					int nUserNum,
					int nStorageNum,
					__time64_t tTime )	{ return 0; }
    
    virtual int SetChaInvenNum(
					int nChaNum,
					WORD wInvenLine )	{ return 0; }
    
    virtual int SetPremiumTime(
					int nUserNum,
					__time64_t tPremiumTime )	{ return 0; }
    
    virtual int SetLastCallPos(
					int nChaNum,
					DWORD dwMapID,
					D3DXVECTOR3 vPos )	{ return 0; }
    
    virtual int	CreateNewCharacter(SCHARDATA2* pCharData2)	{ return 0; }

	//mjeon.ado
	virtual int CreateNewCharacterExtraWithAdo(SCHARDATA2* pCharData2) { return 0; }

    //! ChaInfo Backup.
    virtual int CharacterInfoBackup(SCHARDATA2* pCharData2) { return 0; }
 
    virtual int CreateClub(const std::string& ClubName, DWORD ChaDbNum, wb::EM_COUNTRY Country) override	{ return 0; }
    
    virtual int DeleteClub(DWORD dwClub, DWORD dwChaNum)	{ return 0; }
    
    virtual int SetClubRank(DWORD dwClub, DWORD dwRank)	{ return 0; }


    virtual int ReadClubMarkImage(
					DWORD dwClub, 
					se::ByteStream &ByteStream )	{ return 0; }


    virtual int WriteClubMarkImage(
					DWORD dwClub,
					DWORD dwMarkVer,
					BYTE* pData,
					int nSize )	{ return 0; }
    
    virtual int SetClubDissolutionTime(DWORD dwClub, __time64_t tDiss)	{ return 0; }

	virtual int SetClubAllianceSec(DWORD dwClub, __time64_t tSec)	{ return 0; }

	virtual int SetClubAllianceDis(DWORD dwClub, __time64_t tDis)	{ return 0; }

	virtual int SetClubAuthorityTime(DWORD dwClub, __time64_t tAuthority)	{ return 0; }
	
    virtual int GetClubInfo(std::vector<std::tr1::shared_ptr<GLClub> > &vClub)	{ return 0; }   
    
    virtual int GetClubMember(DWORD dwClub, std::vector<GLCLUBMEMBER>& vMember) override { return 0; }

    virtual int GetClubAuth(DWORD ClubDbNum, std::vector<club::AUTH_GRADE_DB>& vAuth) override { return 0; }
    virtual int ResetClubPosition(DWORD ClubDbNum) override { return 0; }
    
    virtual int AddClubMember(DWORD dwClub, DWORD dwChaNum) override { return 0; }
    
    virtual int DeleteClubMember(DWORD dwChaNum) override { return 0; }

	virtual int DeleteClubMemberWithNoPenalty( DWORD dwChaNum ) override { return 0; }

	virtual int SetClubBattle( DWORD dwClubP, DWORD dwClub, DWORD dwEndTime, int nAlliance )	{ return 0; }

	virtual int EndClubBattle( DWORD dwClubP, DWORD dwClubS, int nGuFlag, int nGuKillNum, int nGuDeathNum, bool bAlliance )	{ return 0; }

	virtual int SaveClubBattle( DWORD dwClubP, DWORD dwClubS, int nGuKillNum, int nGuDeathNum )	{ return 0; }
	
	virtual int ReSetAllianceBattle( DWORD dwClub )	{ return 0; }

    virtual int GetClubBattleInfo( DWORD dwClub, std::vector<GLCLUBBATTLE> &vBattleInfo )	{ return 0; }

	virtual int UpdateClubName( DWORD dwClub, const std::string& ClubName ) { return 0; }

    virtual int SetChaExp(int nChaNum, LONGLONG llExp) override { return 0; }

    virtual LONGLONG GetChaExp(int nChaNum)	{ return 0; }
    
    virtual int LogAppeal(int nSGNum, int nChaNum, CString strMsg)	{ return 0; }
    
    virtual int GetClubRegion(std::vector<GLGUID_DB> &vGUID_DB)	{ return 0; }
    
    virtual int SetClubRegion(DWORD dwRegionID, DWORD dwClub=0, float fTax=0)	{ return 0; }
    
    virtual int DelClubRegion(DWORD dwRegionID, DWORD dwClub=0)	{ return 0; }
	
    virtual LONGLONG GetClubMoney(DWORD dwClub)	{ return 0; }

	virtual int	SetClubMoney(DWORD dwClub, LONGLONG llMoney)	{ return 0; }
	
	virtual int	SetClubIncomeMoney(DWORD dwClub, LONGLONG llMoney)	{ return 0; }
    
	virtual LONGLONG GetClubIncomeMoney(DWORD dwClub)	{ return 0; }
    
	virtual int ReadClubStorage(DWORD dwClub, se::ByteStream &ByteStream)	{ return 0; }
	virtual int GetGuildMigrationState( DWORD dwClub )	{ return 0; }
    
	virtual int	WriteClubStorage(DWORD dwClub, BYTE* pData, int nSize)	{ return 0; }
	
	virtual int LogHackProgram(
		int nSGNum, 
		int nSvrNum, 
		int nUserNum, 
		int nChaNum, 
		int nHackProgramNum,
		const TCHAR* szComment)	{ return 0; }
	
	virtual int	SetClubMasterFlags(
		DWORD dwClub,
		DWORD dwChaNum,
		DWORD dwSubMasterFlags)	{ return 0; }

	virtual int	SetClubAlliance(DWORD dwClubP, DWORD dwClubS) override { return 0; }

	virtual int	DelClubAlliance(DWORD dwClubP, DWORD dwClubS) override { return 0; }
	
	virtual int	GetClubAlliance(std::vector< std::pair<DWORD,DWORD> >& vecAlliance) override { return 0; }

	virtual __time64_t GetChatBlockTime(int nUserNum) override { return 0; }

	virtual int SetChatBlockTime (int nUserNum, __time64_t tBlockTime) override { return 0; }
    virtual int SetClubNotice(const std::string& ChaName, DWORD dwClub, const std::string& ClubNotice) override { return 0; }
	virtual int SetClubDeputy (DWORD dwClub, DWORD dwDeputy) override { return 0; }
	virtual int SetClubAuthority (DWORD dwClub, DWORD dwMasterID) override { return 0; }
    virtual int InsertClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName) override { return 0; }
    virtual int UpdateClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName) override { return 0; }
    virtual int ChangeClubAuthIndex(DWORD ClubDbNum, size_t AuthIndexA, size_t AuthIndexB) override { return 0; }
    virtual int ChangeClubMemberGrade(DWORD ChaDbNum, int Grade) override { return 0; }

	virtual int RenameCharacter(DWORD dwChaNum, char* szCharName) override { return 0; }

	virtual int SetChaHairColor(DWORD dwChaNum, int nHairColor)	override { return 0; }
	
	virtual int SetChaHairStyle(DWORD dwChaNum, int nHairStyle)	override { return 0; }

	virtual int SetChaFaceStyle(DWORD dwChaNum, int nFaceStyle)	override { return 0; }

	virtual int SetChaGenderChange(DWORD dwChaNum, int nClass, int nSex, int nFace, int nHair, int nHairColor) override { return 0; }

	virtual int SetChaSchoolChange(DWORD dwChaNum, int nSchool) override { return 0; }

	virtual int	SaveChaPos(DWORD dwChaNum,
				 DWORD dwStartMapID,
				 DWORD dwStartGate,
				 FLOAT fStartPosX,
				 FLOAT fStartPosY,
				 FLOAT fStartPosZ,
				 DWORD dwSaveMapID,
				 FLOAT fSavePosX,
				 FLOAT fSavePosY,
				 FLOAT fSavePosZ,
				 DWORD dwLastCallMapID,
				 FLOAT fLastCallPosX,
				 FLOAT fLastCallPosY,
				 FLOAT fLastCallPosZ) override { return 0; }

	virtual int CreatePet(
		int nChaNum,
		const TCHAR* szPetName,
		int nPetType,
		int nPetMID,
		int nPetSID,
		int nPetStyle,
		int nPetColor,
		int nPetCardMID,
	    int nPetCardSID) override { return 0; }

	virtual int RenamePet( int nChaNum, int nPetNum, const TCHAR* szPetName) override { return 0; }

	virtual int ExchangePet(int nChaNum, int nPetNum) override { return 0; }

	virtual int SetPetColor(int nChaNum, int nPetNum, int nPetColor) override { return 0; }

	virtual int SetPetStyle( int nChaNum, int nPetNum, int nPetStyle) override { return 0; }

	virtual int SetPetFull( int nChaNum, int nPetNum, int nPetFull) override { return 0; }

	virtual int SetPetInven(int nChaNum, int nPetNum, se::ByteStream &ByteStream) override { return 0; }

	virtual int SetPetMigrationState( int ChaNum, int PetNum, int MigrationState ) override { return 0; }
	virtual int SetVehicleMigrationState( int ChaNum, int VehicleNum, int MigrationState ) override { return 0; }

	virtual int SetPetSkill(
		int nChaNum,
		int nPetNum,
		int nPetInvenType,
		int nPetInvenMID,
		int nPetInvenSID,
		int nPetInvenCMID,
		int nPetInvenCSID,
        int nPetInvenAvailable) override { return 0; }

	virtual int SetPetSkillSlotNum(
		int nChaNum,
		int nPetNum,
		WORD wSkillSlotNum) override { return 0; }

	virtual int	DeletePet(int nChaNum, int nPetNum)	override { return 0; }
	
	virtual int GetPetSkill(int nChaNum, GLPET* pPet, DWORD dwPetNum) override { return 0; }
	
	virtual int GetPetInven(int nChaNum, GLPET* pPet, DWORD dwPetNum) override { return 0; }
	
	virtual int GetPet(GLPetField* pPet, DWORD dwPetNum, DWORD dwClientID) override { return 0; }

	virtual int GetPetFull(int nChaNum, DWORD dwPetNum) override { return 0; }

	virtual int GetRestorePetList(int nChaNum, std::vector<PET_RELIST> &vecPetReList) override { return 0; }

	virtual int RestorePet(DWORD dwPetNum, int nChaNum) override { return 0; }

	virtual int UpdatePetSkin( int nChaNum,
							   DWORD dwPetNum,
							   int nPetSkinMID,
							   int nPetSkinSID,
							   int nPetSkinScale,
							   int nPetSkinTime ) override { return 0; }

	virtual int LogSMS(
					int nSendChaNum,					
					int nReceiveChaNum,
					const TCHAR* szReceivePhone,
					const TCHAR* szSmsMsg )	override { return 0; }

	virtual int LogRandomItem(const LOG_RANDOM_OPTION& _sLOG) override { return 0; }
	virtual int LogItemCostumeStat(const LOG_COSTUME_STATS &_sLOG) override { return 0; }

	virtual int LogAddonItem( const LOG_ADDON_OPTION &_sLOG ) override { return 0; }
	virtual int LogLinkSkillItem( const LOG_LINKSKILL_OPTION &_sLOG ) override { return 0; }
	virtual int LogBasicStatItem( const LOG_BASICSTAT_OPTION &_sLOG ) override { return 0; }

	virtual int SetChaPhoneNumber(int nChaNum, const TCHAR* szPhoneNumber) override { return 0; }

	virtual int LogPetAction(const LOG_PET_ACTION &_sLOG) override { return 0; }

	virtual int TerraUserPassCheck(
					const TCHAR* szTID,
					const TCHAR* szPasswd,
					int	nCheckFlag) override { return 0; }
	
	virtual int DaumUserPassCheck(const TCHAR* szDaumGID, 
								  const TCHAR* szDaumPasswd,
								  int	nCheckFlag) override { return 0; }


	virtual int ExciteUserPassCheck(const TCHAR* szUID,
								  const TCHAR* szUserID,
								  const TCHAR* szPasswd,
								  int	nCheckFlag) override { return 0; }

	virtual int	ThaiUserClassCheck(const TCHAR* szUsrIP) override { return 0; }

	virtual int	MyUserClassCheck(const TCHAR* szUsrIP) override { return 0; }

	virtual int	PhUserClassCheck(const TCHAR* szUsrIP) override { return 0; }

	virtual int CreateVehicle(
		int nCharNum,
		const TCHAR* szVehicleName,
		int nVehicleType,
		int nVehicleCardMID,
		int nVehicleCardSID) override { return 0; }

	virtual int SetVehicleBattery(int nVehicleNum, int nCharNum, int nVehicleBttery) override { return 0; }

	virtual int GetVehicleBattery(int nVehicleNum, int nCharNum) override { return 0; }

	virtual int DeleteVehicle(int nVehicle, int nCharNum) override { return 0; }

	virtual int RebuyVehicle(int nVehicle, int nCharNum) override { return 0; }

	virtual int GetVehicle(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) override { return 0; }

	//virtual int GetVehicleInven(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) override { return 0; }

	virtual int SetVehicleInven(int nVehicle, int nCharNum, se::ByteStream &ByteStream) override { return 0; }

	//virtual int GetVehicleColor(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) override { return 0; }

	virtual int SetVehicleColor(int nVehicle, int nCharNum, se::ByteStream &ByteStream) override { return 0; }
	
	virtual int SetVehicleBooster(int nVehicleNum, int nCharNum, DWORD dwVehicleBooster) override { return 0; }

	virtual int LogVehicleAction(const LOG_VEHICLE_ACTION &_sLOG) override { return 0; }

	virtual int LogUserAttend( int nUserNum, std::vector<USER_ATTEND_INFO> &vecUserAttend )	{ return 0; }

	virtual int InsertUserAttend( int nUserNum, CString strUserName, __time64_t tAttendTime, int nCount,
								  int nAttendReward, int nItemMID, int nItemSID )	{ return 0; }

	virtual int InsertShopItemMap( int nItemMID, int nItemSID )	{ return 0; }

	//	UserMoney Add
	virtual int UpdateUserMoneyAdd( DWORD dwUserID, LONGLONG lnUserMoney )	{ return 0; }
	virtual int UpdateUserLastInfoAdd( DWORD dwUserID, LONGLONG lnUserMoney )	{ return 0; }

	virtual int TransDBDataToWorldBattle(SCHARDATA2* pCharData2, DWORD dwChaNum) override { return 0; }
	virtual int CalculateExpCompressor( DWORD dwChaDbNum, __int64 nCurExp, __int64 nCompressedExp ) override { return 0; }

public:
    virtual void AddGameJob(db::DbActionPtr spJob, IDbManager *pSubDbMan = NULL) override {}
    virtual void AddLogJob(db::DbActionPtr spJob, IDbManager *pSubDbMan = NULL) override {}
    virtual void AddUserJob(db::DbActionPtr spJob, IDbManager *pSubDbMan = NULL) override {}
    virtual void AddWebJob(db::DbActionPtr spJob, IDbManager *pSubDbMan = NULL) override {}
    virtual void ExecuteLastJob() {}

	//
	//mjeon.dbman.from odbcmanager
	//
	virtual	int		SetAllCharacterOffline()					{ return 0; }
	virtual int		AllServerUserLogout(int nSvrGrp)			{ return 0; }
	
	//
	//mjeon.dbman.from DbLogicAction
	//


	//GameActions
	virtual int	PostMaintenance()												{ return 0; }
	virtual int	PostCleanUp(char *pszCurrentDate, char *pszTargetDate)			{ return 0; }
	virtual int	PostReturn(LONGLONG llPostID)									{ return 0; }
	virtual int	PostDelete(LONGLONG llPostID)									{ return 0; }
	virtual int	PostOpen(LONGLONG llPostID)										{ return 0; }
	virtual int	PostGetInfo(int iRecverNum, int &cntAllPost, int &cntNewPost)	{ return 0; }
	virtual int	PostGetData(int iRecverNum, std::vector<SAPOST>& vPost, int &nRowCount, DWORD dwRecverUserNum)	{ return 0; }
    virtual int	PostGetState(int iRecverNum, std::vector<SAPOSTSTATE>& vPost, int &nRowCount) { return 0; }
    virtual int	PostGetSendState(int iRecverNum, std::vector<SAPOSTSTATE>& vPost, int &nRowCount) { return 0; }
    virtual int	PostGetRecvState(int iRecverNum, std::vector<SAPOSTSTATE>& vPost, int &nRowCount) { return 0; }
	virtual int	GetChaNum(const char *szCharName, int *pnChaNum, int *pnUserNum) { return 0; }

	virtual int	DelCharacter(int nUsrNum, int nChaNum, const char* szPass2)		{ return 0; }
	virtual int	DaumDelCharacter(int nUsrNum, int nChaNum, const char* szPass2)						{ return 0; }
	virtual int	IdnDelCharacter(int nUsrNum, int nChaNum)						{ return 0; }
	virtual int	GspDelCharacter(int nUsrNum, int nChaNum)						{ return 0; }
	virtual int	TerraDelCharacter(int nUsrNum, int nChaNum)						{ return 0; }
	virtual int	ExciteDelCharacter(int nUsrNum, int nChaNum)					{ return 0; }
	virtual int	JapanDelCharacter(int nUsrNum, int nChaNum)						{ return 0; }
	virtual int	GsDelCharacter(int nUsrNum, int nChaNum, const char* szPass2)						{ return 0; }
	//virtual int GetChaBAInfo(int nUsrNum, int nSvrGrp, NET_CHA_BBA_INFO_DH* ncbi)	{ return 0; }
    virtual int GetChaBAInfo(int UserNum, int SvrGrp, std::vector<int>& vChaNum) { return 0; }
	virtual int	GetUserTempInfo( int nUserNum )									{ return 0; }
	virtual int	GetChaBInfo(int nUserNum, int nChaNum, SCHARINFO_LOBBY* sci)	{ return 0; }
	virtual int	GetChaBInfoForMigration( int nUserNum, int nChaNum, SCHARINFO_LOBBY* sci )	{ return 0; }
    virtual int GetChaBInfoAll(int UserDbNum, int ServerGroup, std::vector<SCHARINFO_LOBBY>& vCharInfo) { return 0; }
	virtual int GetChaBInfoAllForMigration( int UserDbNum, int ServerGroup, std::vector< SCHARINFO_LOBBY >& vCharInfo ) { return 0; }
	virtual int	GetCharacterInfo(int nUserNumber, int nChaNum, GLCHARAG_DATA* pChaData) { return 0; }
	virtual int SetCharacterOption( int ChaNum, int CharacterOption ) { return 0; }
	virtual int GetCharacterOption( int ChaNum ) { return 0; }
	virtual int	GetCharacterInfo(int nUserNumber, int nChaNum, SCHARDATA2* pChaData2)	{ return 0; }
	virtual int	GetCharacterInfoExtraWithAdo(int nUserNumber, int nChaNum, SCHARDATA2* pChaData2)  { return 0; }
	virtual int	GetChaFriend(int nChaNum, std::vector<CHA_FRIEND> &vecFriend )	{ return 0; }
	virtual int	SetCharacterOnline(int nChaNum)									{ return 0; }
	virtual int	SetCharacterOffline(int nChaNum)								{ return 0; }
    virtual int GetChaFriendSMS( int nChaNum, std::vector<CHA_FRIEND_SMS> &vFriend ) { return 0;}
	virtual int	Certification2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass) { return 0; } // 2차비번
	virtual int	Update2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass) { return 0; } // 2차비번

	virtual int GetUserInvenOption( int UserNum, int& Option ) { return 0; }
	
	//UserActions
	virtual int		GetUserType(int nUserNum)					{ return 0; }
	virtual int		GetUserCountryInfo ( IN int nUserNum, OUT Country::SCOUNTRY_INFO& sCountryInfo )			{ return 0; }
	virtual int		UserTestChaNumDecrease(int nUserNum)		{ return 0; }
	virtual int		UserChaNumDecrease(int nUserNum)				{ return 0; }
	virtual int		GetChaGuildNum(DWORD nChaNum)				{ return 0; }
	virtual int		UserTestChaNumIncrease(int nUserNum)		{ return 0; }
	virtual int		UserUpdateCha(int nUsrNum, const TCHAR* szChaName) { return 0; }
	virtual int		UserLogoutSimple(const TCHAR* szUsrID)		{ return 0; }
	virtual int		UserLogoutSimple2(int nUserNum)				{ return 0; }
	virtual int		UserLogout(const TCHAR* szUsrID, 
								const TCHAR* szUsrIP, 
								int nUsrNum, 
								int nGameTime,
								int nChaNum,
								int nSvrGrp,
								int nSvrNum, 
								int nTotalGameTime,
								int nOfflineTime )				{ return 0; }
	virtual int		ThaiUserCheck(const TCHAR* szUsrID,
								const TCHAR* szPasswd,
								const TCHAR* szUsrIP,
								int nSvrGrp,
								int nSvrNum )					{ return 0; }
	virtual int ThaiGetUserInfo(const TCHAR* szUserID, NET_LOGIN_FEEDBACK_DATA2* pMsg) override { return 0; }
	virtual int	UserCheck(
        const TCHAR* szUsrID, 
        const TCHAR* szPasswd, 
        const TCHAR* szUsrIP,
        const TCHAR* szRandomPasswd,
        int nRandomNum,
        int nSvrGrp,
        int nSvrNum) override { return 0; }

	virtual int IdnUserCheck(int nUserNum, const std::string &strId, const std::string &strIP, int nSvrGrp, int nSvrNum) { return 0; }

	virtual int GetUserInfo(const TCHAR* szUserID, NET_LOGIN_FEEDBACK_DATA2* pMsg) override { return 0; }
	virtual USER_INFO_BASIC GetUserInfo(const TCHAR* szUsrId, const TCHAR* szPasswd) override { USER_INFO_BASIC tmp; return tmp; }

	virtual int IdnGetUserInfo(const std::string &strId, NET_LOGIN_FEEDBACK_DATA2* pMsg) override { return 0; }

	virtual int	DaumUserCheck(TCHAR* szDaumGID, const TCHAR* szUUID, const TCHAR* szUserIP, int nSvrGrp, int nSvrNum, int& Age OUT) override { return 0; }
	virtual int	DaumGetUserInfo(const TCHAR* szGID,	DAUM_NET_LOGIN_FEEDBACK_DATA2* pMsg) override { return 0; }
	virtual int GspUserCheck(TCHAR* szGspUserID, const TCHAR* szUUID, const TCHAR* szUserIP, int nSvrGrp, int nSvrNum ) override { return 0; }
	virtual int GspGetUserInfo(const TCHAR* szUserID, GSP_NET_LOGIN_FEEDBACK_DATA2* pMsg) override { return 0; }
	virtual int TerraUserCheck(TCHAR* szTerraLoginName, const TCHAR* szTerraDecodedTID, const TCHAR* szUsrIP, int nSvrGrp, int nSvrNum) override { return 0; }
	virtual int TerraGetUserInfo(const TCHAR* szTLoginName, TERRA_NET_LOGIN_FEEDBACK_DATA2* pMsg) override { return 0; }
	virtual int GsUserCheck(const TCHAR* szUsrID, const TCHAR* szPasswd, const TCHAR* szUsrIP, int nSvrGrp, int nSvrNum) override { return 0; }
	virtual int GsGetUserInfo(const TCHAR* szUserID, GS_NET_LOGIN_FEEDBACK_DATA2* pMsg) override { return 0; }
	virtual int GmUserCheck( const TCHAR* szUsrID, const TCHAR* szPasswd, const TCHAR* szUsrIP, int nSvrGrp, int nSvrNum ) override { return 0; }
	virtual int GmGetUserInfo( const TCHAR* szUserID, GM_NET_LOGIN_FEEDBACK_DATA2* pMsg ) override { return 0; }
	virtual int UsUserCheck( const TCHAR* szUsrID, const TCHAR* szPasswd, const TCHAR* szUsrIP, int nSvrGrp, int nSvrNum ) override { return 0; }
	virtual int UsGetUserInfo( const TCHAR* szUserID, US_NET_LOGIN_FEEDBACK_DATA2* pMsg ) override { return 0; }
	virtual int ExciteUserCheck(const TCHAR* szUserID, const TCHAR* szUserIP, int nSvrGrp, int nSvrNum) override { return 0; }
	
	virtual int ExciteGetUserInfo(const TCHAR* szUserID, EXCITE_NET_LOGIN_FEEDBACK_DATA2* pMsg) override { return 0; }
	virtual int JapanUserCheck(
        const TCHAR* szUserID,
        const TCHAR* szUserIP, 
        int nSvrGrp, 
        int nSvrNum,
        int nUserNum) override { return 0; }
	virtual int JapanGetUserInfo(const TCHAR* szUserID, JAPAN_NET_LOGIN_FEEDBACK_DATA2* pMsg) override { return 0; }
	virtual int WorldBattleUserCheck(
		const std::string& UserId,
		const std::string& UserPass,
		const std::string& UserIp,
		int ServerGroup,
		int ServerNumber) override { return 0; }
    virtual int WorldBattleGetUserInfo(const std::string& UserId, NET_LOGIN_FEEDBACK_DATA2& Msg) override { return 0; }
	virtual int UserLogoutSvr(int nSvrGrp, int nSvrNum) override { return 0; }
	virtual int UserBlock(int nUserNum, int nDay) override { return 0; }
	virtual int UserBlockDate(int nUserNum, __time64_t tBlockTime) override { return 0; }
	virtual WORD ThaiGetTodayPlayTime(int nUserNum) override { return 0; }
	virtual int UserLogoutSimple3(const TCHAR* szUsrID, int nSvrGrp) override { return 0; }
    virtual int UserDbLogItemDataInsertUpdate(int ItemMid, int ItemSid, const std::string& ItemName) override { return 0; }
	
	//LogActions
	virtual int	AddLogServerState(const __time64_t& LogDate, int nSGNum, int nSvrNum, int nUserNum, int nUserMax ) override { return 0; }
	//virtual int LogLottery(CString strLottery, CString strIP, CString strUserUID, int nUserNum) { return 0; }
	//virtual int LogPungPungWrite(CString strIP, CString strUserUID, int nUserNum, int nPrize)	{ return 0; }
	//virtual int LogPungPungCheck(CString strIP, int nUserNum)	{ return 0; }
	virtual int AllUserLogout()	{ return 0; }

    //
    //mjeon.sns
    //
    virtual int SNSAuthedFB(SFACEBOOK &AuthFB)						override { return 0; }
    virtual int SNSUnAuthedFB(int nChaNum)							override { return 0; }
    virtual int SNSGetInfoFB(SFACEBOOK &AuthFB)						override { return 0; }
    virtual int SNSGetUidFB(int nTargetChaNum, std::string &m_UID)	override { return 0; }
    
	virtual int SNSAuthedTW(STWITTER &AuthTW)						override { return 0; }
    virtual int SNSUnAuthedTW(int nChaNum)							override { return 0; }
    virtual int SNSGetInfoTW(STWITTER &AuthTW)						override { return 0; }
    virtual int SNSGetUidTW(int nTargetChaNum, std::string &m_UID)	override { return 0; }

    virtual int SNSGetInfo(SFACEBOOK &AuthFB, STWITTER &AuthTW)		override { return 0; }

	
	//
	//mjeon.attendance
	//
	virtual int AttendanceLoad(OUT AttendanceManager &AttendanceMan, UINT nChaNum)		override { return 0; }
	virtual int AttendanceSave(IN  AttendanceManager &AttendanceMan, UINT nChaNum, LONGLONG nChaCurExp)		override { return 0; }
	virtual int AttendanceMaintenance()													override { return 0; }
    virtual int AttendanceLogInsert(int ChaNum, int Type, const std::string& LoadType, int DayN, int Status) override { return 0; }
    virtual int AttendanceTaskInsert(
        int ChaNum, LONGLONG ChaCurExp, int TaskN, int Type, int Status, int Value,
        int ValueBase, int RewardPoint, int RewardExp, int RewardMoney, int RewardItems,
        int RewardItem1, int RewardItem2, int RewardItem3, int TookRewards) override { return 0; }

	//
	//mjeon.activity
	//
	virtual int ActivityGetClosedActivityID(OUT ActivityBaseData &abd)					override { return 0; }
	virtual int ActivityLoad(MMACTIVITIES &mmActivities, UINT nChaNum)					override { return 0; }
	virtual int ActivitySave(MMACTIVITIES &mmActivities, UINT nChaNum, UINT nChaLevel)	override { return 0; }	
	virtual int ActivityOrdering(Activity &actv, UINT nChaNum, UINT nChaLevel)			override { return 0; }
	virtual int ActivityGetPoint(OUT LONGLONG *pllRewardPoint, UINT nChaNum)				override { return 0; }
	virtual int ActivityLoadBadge(UINT nChaNum, TitleManager &TitleMan)					override { return 0; }
	virtual int ActivitySetPoint(IN LONGLONG *pllRewardPoint, UINT nChaNum)				override { return 0; }
	virtual int ActivitySaveBadge(UINT nChaNum, TitleManager &TitleMan)					override { return 0; }

	//
	// Provide Reward System
	//
	virtual int ProvideRewardInitialize ( )																	override { return 0; }
	virtual int ProvideRewardInsert ( const ProvideRewardSystem::PROVIDE_REWARD_DATA& _ProvideRewardData )	override { return 0; }
	virtual int ProvideRewardSelect ( DWORD _dwChaDBNum, DWORD _dwServerGroupNum, OUT ProvideRewardSystem::VEC_PROVIDE_REWARD& _vecProvideRewardData )													override { return 0; }
	virtual int ProvideRewardClearDeleteFlag ( )															override { return 0; }
	virtual int ProvideRewardComplete ( DWORD _dwUniqueID )													override { return 0; }

	//
	// Country System
	//
	virtual int UpdateUserCountry ( IN DWORD dwUserID, IN Country::SCOUNTRY_INFO& sCountryInfo )			override { return 0; }
	virtual int LoadVictoriousCountry ( OUT Country::VCOUNTRY_VEC& vecVCountry )							override { return 0; }
	virtual int AddVictoriousCountry ( IN Country::SVICTORIOUS_COUNTRY& sVictoriousCountry )				override { return 0; }
	virtual int ResetVictoriousCountry ( IN Country::EMCOUNTRY_SYSTEM_FLAG emFlag )							override { return 0; }
	virtual int ConfirmStateVictoriousCountry ( IN Country::EMCOUNTRY_SYSTEM_FLAG emFlag, OUT Country::VCOUNTRY_VEC& vecVCountry )							override { return 0; }

	//
	// Lotto System
	//
	virtual int LottoSystemLoad( unsigned int systemId )								override { return 0; }

	virtual int LottoDrawingMachineNumSelect(
		unsigned int lottoNum, unsigned int systemId )									override { return 0; }
	virtual int LottoSystemCurTurnSelect( unsigned int lottoSystemId )					override { return 0; }
	virtual int LottoAccumulateMoneySelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					override { return 0; }
	virtual int LottoWinNumSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					override { return 0; }
	virtual int LottoCharBuyListSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					override { return 0; }
	virtual int LottoCharNonConfirmBuyListSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int systemId, int confirmTicket )						override { return 0; }
	virtual int LottoWinManListSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					override { return 0; }
	virtual int LottoInventWinNumSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					override { return 0; }

	virtual int LottoDrawingMachineNumSave(
		unsigned int lottoNum,
		unsigned int accumulateCount,
		unsigned int addCount, unsigned int systemId, bool bUpdate = true )				override { return 0; }
	virtual int LottoSystemTurnSave( unsigned int turnNum, unsigned int systemId )		override { return 0; }

	virtual int LottoCharBuyListInsert(
		LONGLONG accumulateMoney,
		LONGLONG charge,
		int buyType, const std::string& drawingNum,
		LONGLONG drawingNumInt, int confirmTicket,
		DWORD dwUserNum, DWORD dwDbNum, DWORD dwServerGroup, char* szChaName, unsigned int turnNum, unsigned int systemId )					override { return 0; }

	virtual int LottoWinManListUpdate(
		unsigned int ranking, LONGLONG winMoney, SNATIVEID sItemID, DWORD dwItemCount,
		unsigned int turnNum, unsigned int systemId )									override { return 0; }
	virtual int LottoWinManListInsert(
		unsigned int ranking, LONGLONG winNumInt,
		LONGLONG bonusNumInt, unsigned int systemId )									override { return 0; }

	virtual int LottoAccumulateMoneyUpdate( LONGLONG accumulateMoney,
		unsigned int turnNum, unsigned int systemId )									override { return 0; }
	virtual int LottoWinNumUpdate( const std::string& winNum, LONGLONG winNumInt,
		LONGLONG bonusNumInt, unsigned int turnNum, unsigned int systemId )				override { return 0; }
	virtual int LottoCharNonConfirmBuyListUpdate(
		DWORD dwDbNum, DWORD dwServerGroup, LONGLONG invenMoney, unsigned int systemId )						override { return 0; }

	virtual int LottoSystemNextTurn( unsigned int systemId )							override { return 0; }
	virtual int LottoConfirmWinMan( unsigned int systemId )								override { return 0; }
	virtual int LottoChangeInventWinNum( unsigned int turnNum, unsigned int systemId )	override { return 0; }
	virtual int LottoWinMoneyUpdate( unsigned int turnNum, unsigned int systemId )		override { return 0; }

	virtual int LottoSystemBuyLog( const LottoSystem::LOTTO_SYSTEM_BUY_LOG& logInfo )	override { return 0; }
	virtual int LottoSystemWinManLog( unsigned int turnNum, unsigned int systemId )		override { return 0; }
	virtual int LottoSystemGiveWinTicketLog( unsigned int systemId,
		DWORD dwTicketID, LONGLONG invenMoney )											override { return 0; }
	virtual int LottoSystemTurnInfoLog(
		const LottoSystem::LOTTO_SYSTEM_TURN_LOG& logInfo )								override { return 0; }

    //! PointShop
    virtual int PointShopList(__time64_t& UpdateTime, std::vector<pointshop::SellItem>& ItemList, bool bStandAlone) override { return 0; }
    virtual int GetChaPoint(int ChaDbNum, __int64& PointRefundable, __int64& PointNotRefundable) override { return 0; }
    virtual int SetChaPoint(int ChaDbNum, const __int64& PointRefundable, const __int64& PointNotRefundable) override { return 0; }
    virtual int LogPointShopInsert(DWORD ChaDbNum, const std::string& LogMessage) override { return 0; }    
	virtual int LogPointShopInsert(
		DWORD ChaDbNum, int LogType, 
		__int64& BeforeRefundable, 
		__int64& BeforeNotRefundable, 
		__int64& AfterRefundable, 
		__int64& AfterNotRefundable, 
		WORD ItemMid, 
		WORD ItemSid) override { return 0; }    
    virtual int ShopSaleLog(
        const SNATIVEID& ItemId,
        __int64 Price,
        db::EM_BILLING_SELL_TYPE Type,
        int UserDbNum,
        int ServerNum,
        int ChaDbNum,
        int ChaLevel) override
    { 
        return 0;
    }

	//mjeon.CaptureTheField
	virtual int CTFStatLog(UINT *arrPlayerNum, UINT state)									override { return 0; }
	virtual int CTFResultLog(WORD wSchoolAuthed, CTF_REWARD_BUFF *arrReward, UINT nHowMany)	override { return 0; }
	virtual int CTFPlayLog(UINT nChaNum, WORD wSchool, int nTotalPoint, int nRewardPoint)	override { return 0; }
	virtual int GetCTFInfo(UINT rowcount ,std::vector<DWORD >& vAuthSchool)					override { return 0; }
	virtual int LoadChaExtraInfo(UINT nChaNum, int &nContributionPoint)						override { return 0; }
	virtual int SaveChaExtraInfo(UINT nChaNum, int nContributionPoint)						override { return 0; }

    virtual int LogItemListInsertUpdate(int ItemMid, int ItemSid, const std::string& ItemName) override { return 0; }
	virtual int LogQuestListInsertUpdate( int Mid, int Sid, const std::string& Name ) override { return 0; }
	virtual int LogSkillListInsertUpdate( int Mid, int Sid, const std::string& Name, int Grade ) override { return 0; }

	virtual int LogGmCommandInsert( int ChaNum, const std::string& Command, const std::string& Param ) override { return 0; }

	virtual int LogGMChat( int nChatType, int nGMType, const std::string& SendName, const std::string& RecvName, const std::string& ChatMsg ) override { return 0; }

    virtual int ExecuteSqlLogDb(const std::string& SqlStr) override { return 0; }

    virtual int LogMiniGameOddEvenInsert(
        DWORD UserDbNum,
        DWORD CharDbNum,        
        BYTE  CurrentRound,
        BYTE  WinLost,
        __int64 BattingMoney,
        __int64 DividendMoney,
        __int64 ChaMoney) override { return 0; }
    virtual int UserCountryUpdate(int UserDbNum, wb::EM_COUNTRY Country) override { return 0; }

    virtual int ClubNickUpdate(DWORD ChaDbNum, const std::string& NickName) override { return 0; }
    virtual int ClubPublicFlagUpdate(DWORD ClubDbNum, DWORD Flag) override { return 0; }
    virtual int ClubAutoKickUpdate(DWORD ClubDbNum, size_t GradeIndex, WORD Day, bool AutoKickOut) override { return 0; }
    virtual int ClubNewbieNoticeUpdate(DWORD ClubDbNum, const std::string& Notice) override { return 0; }
    virtual int ChaClubSecedeTimeReset(DWORD ChaDbNum) override { return 0; }

    virtual int ChaFriendGroupSelect(int ChaDbNum, std::vector<FRIEND_GROUP>& vGroup) override { return 0; }
    virtual int ChaFriendGroupInsert(int ChaDbNum, WORD GroupNum, const std::string& GroupName) override { return 0; }
    virtual int ChaFriendGroupUpdate(int ChaDbNum, WORD GroupNum, const std::string& GroupName) override { return 0; }
    virtual int ChaFriendGroupDelete(int ChaDbNum, WORD GroupNum) override { return 0; }
    virtual int ChaFriendGroupSet(int ChaP, int ChaS, WORD GroupNum) override { return 0; }

    virtual int NotifySelect(int ChaDbNum, std::vector<std::tr1::shared_ptr<gamelogic::NotifyBase> >& vNotify) override { return 0; }
    virtual int NotifyInsert(int ChaDbNum, gamelogic::notify::NOTIFY_TYPE Type, sc::MGUID& Guid, const std::string& Data) override { return 0; }
    virtual int NotifyDelete(sc::MGUID& Guid) override { return 0; }


    // 로그 타입을 정의
    virtual int InstantDungeonLogTypeInsert( 
        const db::EM_INSTANT_LOG_TYPE& nLogType,
        const char* strParamName1=0,  const char* strParamName2=0,  const char* strParamName3=0,  const char* strParamName4=0,  const char* strParamName5=0,
        const char* strParamName6=0,  const char* strParamName7=0,  const char* strParamName8=0,  const char* strParamName9=0,  const char* strParamName10=0,
        const char* strParamName11=0, const char* strParamName12=0, const char* strParamName13=0, const char* strParamName14=0, const char* strParamName15=0,
        const char* strParamName16=0, const char* strParamName17=0, const char* strParamName18=0, const char* strParamName19=0, const char* strParamName20=0 ) override { return 0; };

    // 실제 로그 입력
    virtual int InstantDungeonLogInsert( 
        const db::EM_INSTANT_LOG_TYPE& nLogType,
        const char* strParam1=0,  const char* strParam2=0,  const char* strParam3=0,  const char* strParam4=0,  const char* strParam5=0,
        const char* strParam6=0,  const char* strParam7=0,  const char* strParam8=0,  const char* strParam9=0,  const char* strParam10=0,
        const char* strParam11=0, const char* strParam12=0, const char* strParam13=0, const char* strParam14=0, const char* strParam15=0,
        const char* strParam16=0, const char* strParam17=0, const char* strParam18=0, const char* strParam19=0, const char* strParam20=0 ) override { return 0; };

    /////////////////////////////////////////////////////////////////////////
    // 전장 점수 CompetitionRating

    // 전장 점수 얻어옴;
    virtual int CompetitionRatingGetInfo(
        const std::string& strContentType,
        const DWORD charDbNum,
        int& Rating,
        int& WinningStreak,
        int& PlayCount,
        __time64_t& LastPlayDate
        ) override { return 0;};

    // 전장 점수 저장;
    virtual int CompetitionRatingUpdate(
        const std::string& strContentType,
        const DWORD charDbNum,
        const int Rating,
        const int WinningStreak,
        const int PlayCount,
        const __time64_t tLastPlayDate
        ) override { return 0;};

    // 전장 시즌 업데이트;
    virtual int CompetitionSeasonUpdate(
        const std::string& strContentType,
        const int seasonNum,
        const int baseRating
        ) override { return 0;};

    // 전장 로그 CompetitionLog


	// 인증 서버 로그 기록
	virtual int AddAuthServerState( int nCounrty, int nServerType, const TCHAR* szIP, int nPort, int nCurUser, int nMaxUser, const __time64_t& LogDate ) override { return 0; };
	// 인증 절차 수행
	virtual int ProcessCertificationForAuth( int isSessionSvr, int nCounrty, int nServerType, const TCHAR* szIP, int nPort, std::string strUniqKey, int &certification, std::string &strNewUniqKey, int &nSessionSvrID ) override { return 0; };
		// 이벤트 정보 기록
	virtual int AddAuthEventInfo( G_AUTH_EVENT_INFO *eventInfo ) override { return 0; };

	virtual int AddAuthServerConnectionState( int nSessionSvrID, int nCounrty, int nServerType, const TCHAR* szIP, int nPort ) override { return 0; };
	virtual int AddAuthServerConnectionStateInit() override { return 0; };
	virtual int AuthCountryAccessApproveSelect( int nCounrty, int nServerType, const TCHAR* szIP, int nPort, std::vector<int>& vecIndex OUT ) override { return 0; };

	virtual int TexasHoldemChipMapSelect( std::vector< TexasHoldem::sChipPrice >& vecChip ) override { return 0; }
	virtual int TexasHoldemChipMapVersionSelect( LONGLONG& Version ) override { return 0; }
	virtual int	TexasHoldemChipSelect( SCHARDATA2* pChaData2 ) override { return 0; }
	virtual int	TexasHoldemChipUpdate( DWORD dwChaDbNum, LONGLONG& chip, bool bPlus = false ) override { return 0; }
	virtual int	TexasHoldemChipLogInsert( DWORD dwChaDbNum, LONGLONG& chip, LONGLONG& GameMoney, LONGLONG& Point ) override { return 0; }
	virtual int	TexasHoldemChipSelectFromWeb( int nServerGroup, DWORD dwChaDbNum, LONGLONG& chip ) override { return 0; };
	virtual int	TexasHoldemAuthKeyInsert( DWORD dwChaDbNum, const std::string& strGUID ) override { return 0; }
	virtual int	TexasHoldemGameTimeInsert( DWORD dwChaDbNum, DWORD dwUserNum, int nSecond ) override { return 0; }

    //! ITEM ID를 통해 아이템을 가지고 온다. ( bDateCheck => 날짜(Date)검사 여부 )
    virtual int ConsignmentsSaleGet( sc::MGUID& Guid, private_market::ConsignmentSaleItem& SaleItem, bool bDateCheck = false  ) override { return 0; };
    //! ChaNum을 통해 등록한 물품을 가지고 온다. 
    virtual int ConsignmentsSaleSelect( private_market::SALE_ITEMS_VECTOR& vecItem, const DWORD& dwChaNum = 0, const int& ServerGroup = 0 ) override { return 0; };
    //! 남은 갯수를 바꾼다.
    virtual int ConsignmentsUpdateTurnNum( sc::MGUID& Guid, int nTurnNum ) override { return 0; };

    virtual int ConsignmentsSaleInsert( private_market::ConsignmentSaleItem& SaleItem ) override { return 0; };
    virtual int ConsignmentsSaleDelete( sc::MGUID& Guid ) override { return 0; };
    virtual int	ConfignmentSaleSlotExpansionDateSelect( SCHARDATA2* pChaData2 ) override { return 0; };
    virtual int	ConfignmentSaleSlotExpansionDateSet( DWORD dwChaDbNum, __time64_t tExpansionDate ) override { return 0; };
	virtual int ConsignmentSaleLogInsert( 
		sc::MGUID&	Guid,
		DWORD		dwChaDbNum,
		WORD		wType,
		WORD		wState,
		LONGLONG	llPrice,
		LONGLONG	llFee,
		WORD		wItemMid,
		WORD		wItemSid,
		LONGLONG    llItemMakeNum,
		int			nItemAmount,
		WORD		TradeCount,
		WORD		wRemainingTime ) override { return 0; }
	virtual int MacroLogInsert(DWORD dwChaDbNum, WORD  wState) override { return 0; }
	virtual int ItemPeriodExLogInsert(DWORD  chaNum
		, WORD itemMid
		, WORD itemSid	
		, WORD remainNum
		, __time64_t previousPeriod
		, __time64_t currentPeriod) override { return 0; }

	virtual int	UserSystemInfoInsert( 
		DWORD dwUserNum, 
		DWORD dwCpuCoreNumber, 
		DWORD dwCpuClock, 
		DWORDLONG dwMemorySize,
		DWORD dwResolutionX,
		DWORD dwResolutionY,
		DWORD dwGpuVendor,
		DWORD dwGpuModel,
		DWORD dwShaderModel,
		int languageCode ) override { return 0; }

	virtual int	GetChaResetInfo( DWORD dwChaDbNum ) override { return 0; }

	// 필리핀 유저이전
	virtual int	IsMigrationTarget( DWORD dwChaDbNum ) override { return 0; }
	virtual int GetCharacterInfoForMigration( int nUserNumber, int nChaNum, SCHARDATA2* pChaData2 ) override { return 0; }
	virtual int SetMigrationTarget( DWORD dwChaDbNum ) override { return 0; }

	//////////////////////////////////////////////////////////////////////////
	//
	// TOURNAMENT BETTING SYSTEM
	//
	virtual int TournamentBettingSelect( DWORD ClientDBid, DWORD BettingStatus ) override { return 0; };
	virtual int TournamentBettingInsert( DWORD ChaNum,LONGLONG BettingCount, DWORD BettingGameSciptid,DWORD BettingGroup)override { return 0; };
	virtual int TournamentBettingUpdate( DWORD ChaNum,DWORD BettingGameSciptid,DWORD BettingStatus)override { return 0; };
	virtual int TournamentBettingDelete( DWORD ChaNum )override { return 0; };


	//////////////////////////////////////////////////////////////////////////
	//
	// MATCHING SYSTEM
	//

	virtual int MatchingOnload( std::string Matching_Content_Type,
		std::string Betting_Script,
		std::string Room_Admin_Script,
		std::string Matching_Script,
		std::string Lobby_Field_Manager )override { return 0; };
	virtual int MatchingGroup( DWORD GroupID,
		DWORD GameScriptID,
		bool  IsDeleted)override { return 0; };
	virtual int MatchingGroupUser( DWORD GroupID,
		DWORD GameScriptID,
		DWORD UserDBNum,
		bool  IsDeleted)override { return 0; };
	virtual int MatchingQueueIn( DWORD ClientID,
		DWORD GameScriptID,
		bool  IsDeleted)override { return 0; };
	virtual int MatchingInstanceIn( DWORD GroupID,
		DWORD GameScriptID,
		DWORD  MatchingScriptID)override { return 0; };
	virtual int MatchingInstanceJoin( DWORD GroupID,
		DWORD MapID,
		DWORD GameScriptID,
		DWORD  MatchingScriptID)override { return 0; };
	virtual int MatchingInstanceOnStart( DWORD GroupReadySize,
		DWORD GameScriptID,
		DWORD MatchingScriptID)override { return 0; };
	virtual int MatchingInstanceOut( DWORD GroupID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)override { return 0; };
	virtual int MatchingInstanceResualt( DWORD Faction,
		bool IsLose,
		DWORD MapID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)override { return 0; };
	virtual int MatchingRoomDestroy( DWORD MapID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)override { return 0; };
	virtual int MatchingRoomReq( DWORD MapID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)override { return 0; };
	virtual int MatchingFinalWinner( DWORD GroupID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)override { return 0; };
	virtual int MatchingInstanceTournament( int   tournamentPosition,
		std::string GroupName,
		int   GroupIndexPosition,
		int   GroupSize,
		int   GroupMaxSize,
		int   GroupHeadLevel,
		DWORD GroupID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)override { return 0; };

	virtual int MatchingJoinCountLoad ( MatchingSystem::MAP_CHAR_JOIN_COUNT_LIST& _refMapCharJoinCountList )			{ return 0; }
	virtual int MatchingJoinCountReset ( )																				{ return 0; }
	virtual int MatchingJoinCountUpdate ( const DWORD _dwCharDBNum, const MatchingSystem::SJOIN_COUNT _sJoinCount )		{ return 0; }


    // GM LOG ( GM 아이템 넣어주기 )
    virtual int WriteGMLog(int nGmNum, std::string& strCmd, std::string& strUserIP) override { return 0; };

	// 란모바일
	virtual int RanMobileRequest(DWORD dwUserDBNum, DWORD dwChaDBNum, int nServerNum, int nCommand, int nValue, const std::string& strChaName, int nChaClass ) override { return 0; };

	// Log Money;
	virtual int MoneyLogInsert ( const int _nMainType, const int _nSubType, const LONGLONG _llMoney ) { return 0; }

	//////////////////////////////////////////////////////////////////////////
	// 기간제 아이템 시간 연장
	virtual int InspectionRewardPolicySelect() override { return 0; }
	virtual int InspectionRewardPolicyInsert( const __time64_t tStart, const __time64_t tEnd, const int nRewordMinute ) override { return 0; };
	virtual int InspectionRewardGetMinute( SCHARDATA2* pChaData2 ) override { return 0; };

	virtual int MoneyNotifyInsert( DWORD dwChaDbNum, __time64_t tDate, LONGLONG llOldMoney, LONGLONG llNewMoney, int nCause, int nChannel, int nField ) override { return 0; }

	// PostRenew
	// PostRenew
	virtual int PostRnSelectAll( std::vector<PostInfo>& vecList OUT, DWORD dwRecvChaNum ) override { return 0; }
	virtual int PostRnSetRead( DWORD dwPostID )		override { return 0; }
	virtual int PostRnDelAttach( DWORD dwPostID )	override { return 0; }
	virtual int PostRnDel( DWORD dwPostID )			override { return 0; }
	virtual int PostRnSend(
		DWORD dwSendChaDBNum, 
		std::string strSendChaName, 
		DWORD dwSendUserNum, 
		DWORD dwRecvChaDbNum, 
		int nStyle, 
		int wMailType,
		int wAttachType,
		int wMoneyType,
		LONGLONG llMoney, 
		SITEMCUSTOM sItemCustom,
		std::string strTitle, 
		std::string strContent ) override { return 0; }
	virtual int PostRnReturn( DWORD dwPostID, DWORD dwReturnChaNum, std::string strReturnChaName, DWORD dwReturnUserNum, DWORD dwRecvChaNum  ) override { return 0; }
	virtual int PostCharacterCheck(DWORD dwRecvChaDbNum) override { return 0; }
	virtual int GetinfofromCharDb(const char *szCharName, int *pnChaNum, int *pnUserNum, int *pClass, int *Lv) override { return 0; }

	virtual int GetCharacterItemTransferTarget( int UserNum, std::vector< DWORD >& vecChaNum ) override { return 0; }

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.17, Randombox 확률제어 항목 완료로그 저장
     * modified : 
     */
    virtual int LogRandomboxChanceComplete(const SNATIVEID& BoxId,  /* RandomBox ID */
                                           const SNATIVEID& ItemId, /* 당첨 Item ID */
                                           INT32 ReqCount,          /* 목표 오픈 수량 */
                                           DWORD ChaNum,            /* 목표 캐릭터 번호 */
                                           DWORD GMNum,             /* 등록 GM Number */
                                           INT32 OpenCount,         /* 실제 오픈 수량 */
                                           DWORD Winner             /* 당첨자 */
                                          ) override { return 0; }
	// item
	virtual int ItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) override { return 0; }
	virtual int DeletedItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) override { return 0; }
	virtual int ItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) override { return 0; }
	virtual int EntireItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) override { return 0; }
	virtual int ItemDelete( sc::MGUID& Guid, int State, int CallType ) override { return 0; }
	virtual int CompletelyItemDelete( sc::MGUID& Guid, int State, int CallType ) override { return 0; }
	virtual int ItemSaleToNpc( sc::MGUID& Guid, int State, int CallType ) override { return 0; }
	virtual int ItemSelect( DWORD ChaDbNum, INVEN_TYPE InvenType, std::vector< SINVENITEM_SAVE >& vecItems ) override { return 0; }
	virtual int ItemSelect( DWORD ChaDbNum, INVEN_TYPE InvenType, std::multimap< DWORD, SINVENITEM_SAVE >& mapItems ) override { return 0; }
	virtual int GetPutonItems( DWORD ChaDbNum, SCHARDATA2* pChaData2 ) override { return 0; }
	virtual int GetInvenItems( DWORD ChaDbNum, SCHARDATA2* pChaData2 ) override { return 0; }
	virtual int GetLockerItems( DWORD UserNum, SCHARDATA2* pChaData2 ) override { return 0; }
	virtual int SavePetInven( int nChaNum, int nPetNum, std::vector< SINVENITEM_SAVE >& vecItems ) override { return 0; }
	virtual int SaveVehicleInven( int nVehicle, int nCharNum, std::vector< SINVENITEM_SAVE >& vecItems ) override { return 0; }
	virtual int GetClubLockerItems( DWORD ClubNum, std::vector< SINVENITEM_SAVE >& vecItems ) override { return 0; }
	virtual int SaveClubLockerItems( DWORD ClubNum, std::vector< SINVENITEM_SAVE >& vecItems ) override { return 0; }

	virtual int TransferItems( DWORD UserNum, std::vector< DWORD >& vecChaNum ) override { return 0; }
	virtual int TransferClubLockerItems( DWORD ClubNum = 0 ) override { return 0; }
	virtual int GetGuildItemTransferStatus( DWORD ClubNum, int& Result, int& MigrationState ) override { return 0; }
	virtual int TransferLockerItems( DWORD UserNum = 0 ) override { return 0; }
	virtual int GetChaNumToTransfer( std::vector< DWORD >& vecChaNum ) { return 0; }
	virtual int TransferCharacterItems( DWORD ChaDbNum = 0 ) override { return 0; }

	virtual int ItemUpdatePos( DWORD ChaDbNum, INVEN_TYPE InvenType, sc::MGUID& Guid, WORD PosX, WORD PosY, int CallType ) override { return 0; }
	virtual int ItemUpdateTurnNum( sc::MGUID& Guid, WORD TurnNum, int CallType ) override { return 0; }
	virtual int ItemInsertError( sc::MGUID& Guid, int CallType, __time64_t& Date ) override { return 0; }

	virtual int GetItemTransferStatus( DWORD UserNum, int& Result ) override { return 0; }
};

#endif // _EMUL_DB_MAN_H_
