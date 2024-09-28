//
//mjeon.ado
// 2010.12.29
//

#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>
//#include <stdio.h>

#include <vector>
#include <sstream>
#include <hash_map>

#include "../../../SigmaCore/Util/Lock.h"

#include "../DBBase.h"
#include "ADOClass.hpp"


struct LOG_RANDOM_OPTION;
struct LOG_ITEM_EXCHANGE;
//
// ADOManager MUST be updated to improve the efficiency of using CjADO.
//  ,i.e.,CjADO pool will be a good choice for the purpose.
//
class CADOManager : public gmce::IDBManager, public sc::DefaultLock
{
public:	
	static CADOManager* GetInstance();
	static void ReleaseInstance();

	virtual ~CADOManager(void);

private:
	CADOManager(void);
	
	static CADOManager* SelfInstance;

	CString	m_csUserDBConnString;
	CString	m_csGameDBConnString;
	CString	m_csLogDBConnString;


	CjADO	m_adoUser;
	CjADO	m_adoGame;
	CjADO	m_adoLog;


	BOOL	m_bSetConnStrings;

public:
	void SetConnStrings(CString &csUserDB, CString &csGameDB, CString &csLogDB);
	BOOL GetConnString(gmce::ENUM_ADO_INDEX iDB, CString &csConnString);
	BOOL ConnStringsAvailable(void) { return m_bSetConnStrings; }

	BOOL EnableConnectionPooling();

private:
	int ExecuteStoredProcedure(const std::string& SpName, CjADO& Ado);
    int ExecuteStoredProcedureIntReturn(const std::string& SpName, CjADO& Ado, const std::string& OutPutParamName);

public:
	int TestBLOB();	//Test for BLOB reading & writing

	int GmLogin( CString strGMID, CString strGMPassword, CString strUserIP );
	int GetGmUserNum();
	int GetUserNum( CString strUserID ); 
	int GetCharacterInfo( int nChaNum, int nUserNumber, GLCHARLOGIC* pChaData2 );
    virtual int RestoreCharacterInfoBackup( int nChaNum ) override;
    int GetCharacterSkill( int nChaNum, GLCHARLOGIC* pChaData2 );
	int	GetServerInfo( std::vector<viewServerInfo> &v );

    int SearchCharacter(const std::string& Query, std::vector<CharInfo>& v);
	int SearchCharacter(std::strstream& strSearch, std::vector<CharInfo>& v);
    int SearchCharacter(CjADO& Ado, std::vector<CharInfo>& v);

	int GetAllChar( std::vector<AllCharNum> &v );
	int GetAllInven( std::vector<AllCharNum> &v );

    virtual int GetExchangeItemLog( const std::vector<int>& GenType, const std::vector<int>& ExchangFlag, std::vector<ExchangeItemLog>& vOut ) override;
    virtual int GetExchanageItemHistory( LONGLONG InMakeNum, std::vector<ExchangeItemHistory>& vOut ) override;

	int GetUserInven( GLCHARLOGIC* pChaData2 );
	int SaveCharacter( LPVOID _pbuffer );
	int SaveCharacterBase( LPVOID _pbuffer );
	int SaveCharacterInven( LPVOID _pbuffer, bool bClone = false );
	int SaveCharacterInvenForClone( LPVOID _pbuffer );
	int SaveCharacterSkill( LPVOID _pbuffer );
	int SaveCharacterQuest( LPVOID _pbuffer );
	int SaveCharacterUserInven( LPVOID _pbuffer );
	int SaveCharacterPutOnItem( LPVOID _pbuffer, bool bClone = false );
	int SaveCharacterPutOnItemForClone( LPVOID _pbuffer );
	int SaveCharacterSkillQuickSlot( LPVOID _pbuffer );
	int SaveCharacterActionQuickSlot( LPVOID _pbuffer );
	int SaveCharacterSkillFact( LPVOID _pbuffer );
	int SaveCharacterSlots ( LPVOID _pbuffer );
	int SaveStorageDate( int nUserNum, int nStorageNum, __time64_t tTime );
    int CopyCharacterProductInfo( DWORD dwOriginalChaNum, DWORD dwNewChaNum );
	int ItemSearch( int mid, int sid, std::vector< ITEM_SEARCH >& result );
	int ItemSearch( CString& guid, std::vector< ITEM_SEARCH >& result );
	int ItemStatistic( std::vector< ITEM_STATISTIC >& result );
	int ItemSelect( int Owner, int InvenType, std::vector< SINVENITEM_SAVE >& vecItems );

	int CreateNewCharacter( SCHARDATA2* pCharData2, int nSvrNum=1 );
	int CreateNewCharacter2( SCHARDATA2* pCharData2, int nSvrNum=1 );
    virtual int CharacterInfoBackup( int nChaNum ) override;
	BOOL CheckInven( DWORD dwUserNum );
	int MakeUserInven( DWORD dwUserNum );
	int ReadUserInven( GLCHARLOGIC* pChaData2 );
	
	int WriteUserInven( LONGLONG llMoney, DWORD dwUserNum, char* pData, int nSize );
	int WriteUserInven( LONGLONG llMoney, DWORD dwUserNum, GLCHARLOGIC *pCharData2 );

	int WriteGMLog(CString &csCmd);
	int WriteGMLog(std::string &strCmd);
	int WriteGMLog(std::string strCmd, std::string strIP, int nGmNum);
	
	int WriteGMLog(std::strstream& strCmd, CString strUserIP);
	int WriteGMLog(int nGmNum, std::strstream& strCmd, CString strUserIP);
	int WriteLogRandomItem(const LOG_RANDOM_OPTION &_sLOG);
	int WriteLogItemExchange(const LOG_ITEM_EXCHANGE& sEXCHANGE);

	int GetActiveChaNum();
	int GetDeletedChaNum();
	int GetUserInvenNum();
	int GetClubNum();

	LONGLONG GetSumChaMoney();
	LONGLONG GetSumUserInvenMoney();
	LONGLONG GetSumClubInvenMoney();

	int GetClubInfo( const char* szSearch, std::vector<viewClubInfo> &v );

	void SetLocalIP();
	CString GetLocalIP();

	//
	//mjeon.ado
	// newly added features
	//	
	int GetVehicleInfo(gmce::VVEHICLE &v, int nChaNum, int nVehicleNum = -1);

	int GetVehiclePutonItems( int VehicleNum, GLVEHICLE& Vehicle );

	// Product;
	virtual int GetProductInfo( int nChaNum );
	virtual int SaveProductChangeSet();
	virtual int LearnProductType( DWORD dwProductType );
	virtual int ResetProductType( DWORD dwProductType, int& addPoint );
	virtual int LearnProductBook( DWORD dwProductType, DWORD dwProductBook );
	virtual int ResetProductBook( DWORD dwProductType, DWORD dwProductBook );
	virtual int LearnProductExp( DWORD dwProductType, DWORD dwProductID, int& addPoint );
	virtual int ResetProductExp( DWORD dwProductType, DWORD dwProductID, int& addPoint );

	// ClubInven;
	virtual int GetClubInven( int nChaNum );
	virtual int SaveClubInven( int nChaNum, DWORD ClubNum, std::vector< SINVENITEM_SAVE >& vecItems );

	virtual int GetClubNumToTransfer( std::vector< DWORD >& vecClubNum );
	virtual int TransferClubLockerItems( DWORD ClubNum = 0 );
	virtual int GetUserNumToTransfer( std::vector< DWORD >& vecUserNum );
	virtual int TransferLockerItems( DWORD UserNum = 0 );
	virtual int GetChaNumToTransfer( std::vector< DWORD >& vecChaNum );
	virtual int TransferCharacterItems( DWORD ChaNum = 0 );
	virtual int ItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType );
	virtual int ItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType );
	virtual int ItemDelete( sc::MGUID& Guid, int State, int CallType );

	virtual int GetCharacterItemTransferTarget( int UserNum, std::vector< DWORD >& vecChaNum );
	//virtual int GetTotalUserNumToTransfer( size_t& TotalUserNum );
	virtual int PrepareItemTransfer( DWORD UserNum );
	virtual int CompleteItemTransfer( DWORD UserNum );
	virtual int UpdateUserInvenOption( DWORD UserNum, int Option );

private:
    int GetCharacterInfoBase      (int nUserNumber, int nChaNum, SCHARDATA2* pChaData2);
	int GetCharacterInfoStats ( int nChaNum, SCHARDATA2* pChaData2 );
    int GetCharacterInfoSkill     (int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoSkillSlot (int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoActionSlot(int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoQuest     (int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoPutOnItems(int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoInven     (int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoVTAddInven(int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoCoolTime  (int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoSkillFact (int nChaNum, SCHARDATA2* pChaData2);
	int GetCharacterInfoSlots ( int nChaNum, SCHARDATA2* pChaData2 );

	int GetAllPetPutonItems( DWORD ChaDbNum, std::vector< GLPET >& vecPets );
	int GetAllVehiclePutonItems( DWORD ChaDbNum, std::vector< GLVEHICLE >& vecVehicles );

	// 바이너리 정보 읽기
	int GetCharacterBinaryPutOnItems( int ChaNum, SCHARDATA2* pChaData2, int MigrationTarget );
	int GetCharacterBinaryInvenItems( int ChaNum, SCHARDATA2* pChaData2, int MigrationTarget );
	int GetCharacterBinaryQuest( int ChaNum, SCHARDATA2* pChaData2, int MigrationTarget );
	int GetUserBinaryInvenItems( int UserNum, SCHARDATA2* pChaData2 );

	int SetPetMigrationState( int ChaNum, int PetNum, int MigrationState );
	int SetVehicleMigrationState( int ChaNum, int VehicleNum, int MigrationState );
	int SetMigrationTarget( int ChaNum );
public:
	void SetUpGmInfo(UINT nUserNum, CString &csUserID, CString &csUserIP);
	//현재 사용중인 GM 정보
	CString m_strUserIP;
	CString m_strUserID;
	UINT	m_nGmUserNum;
};
