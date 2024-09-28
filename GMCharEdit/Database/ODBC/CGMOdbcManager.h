#ifndef _C_CGMOdbcManager_H_
#define _C_CGMOdbcManager_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// To use the installer DLL functions, you must: 
// Use #include < ODBCINST.H>.
// Link with Odbcinst.lib for 16-bit applications.
// Link with Odbccp32.lib for 32-bit applications. 
// #pragma comment (lib,"Odbccp32.lib")
#pragma comment (lib,"odbc32.lib")
#include <odbcinst.h>
#include <sqlext.h> // ODBC library

//#include <Windows.h>
//#include <sqlext.h> // ODBC library
//#include <odbcinst.h>


#include "../../../SigmaCore/Util/Lock.h"

#include "../DBBase.h"

struct LOG_RANDOM_OPTION;
struct LOG_ITEM_EXCHANGE;

class CGMOdbcManager : public gmce::IDBManager, public sc::DefaultLock
{
public:
	static CGMOdbcManager* GetInstance();
	static void	ReleaseInstance();

private:
	CGMOdbcManager(void);
	~CGMOdbcManager(void);

	static CGMOdbcManager* SelfInstance;

protected:
   	SQLHENV m_hEnvUser;  // Handle of sql (void*)	
	SQLHDBC m_hOdbcUser; // Handle of ODBC (void*)	

    SQLHENV m_hEnvGame;  // Handle of sql (void*)	
	SQLHDBC m_hOdbcGame; // Handle of ODBC (void*)	

public:

    // Basic Function
    int			OpenDB(char* szOdbcName, 
					   char* szUsrID, 
					   char* szUsrPasswd,
					   char* szDbName,
                       SQLHENV hEnv,
                       SQLHDBC hOdbc);
	
	void		CloseDB(SQLHENV hEnv, SQLHDBC hOdbc);

    SQLHSTMT    GetStmt     (SQLHDBC hOdbc);
    void        FreeStmt    (SQLHSTMT hStmt);
	int			ExecuteSQL	(SQLHDBC hOdbc, CString strSQL);
	int			Insert		(SQLHDBC hOdbc, CString strSQL);
	int			Update		(SQLHDBC hOdbc, CString strSQL);
	int			Delete		(SQLHDBC hOdbc, CString strSQL);

    ///////////////////////////////////////////////////////////////////////////
    int         OpenUserDB(char* szOdbcName, 
					char* szUsrID, 
					char* szUsrPasswd);

    int         OpenGameDB( const char* szOdbcName, 
					const char* szUsrID, 
					const char* szUsrPasswd);

	void		CloseUserDB();
	void		CloseGameDB();

    ///////////////////////////////////////////////////////////////////////////
    int			GmLogin              (CString strGMID, CString strGMPassword, CString strUserIP);
	int			GetGmUserNum()	{return 0;}
	int         GetUserNum           (CString strUserID); // 사용자 ID 를 바탕으로 사용자 번호를 가져온다	    
	int         GetCharacterInfo     (int nChaNum, int nUserNumber, GLCHARLOGIC* pChaData2);
    virtual int RestoreCharacterInfoBackup( int nChaNum ) override;
    virtual int GetCharacterSkill( int nChaNum, GLCHARLOGIC* pChaData2 ) override;
	int			GetServerInfo		 (std::vector<viewServerInfo> &v);
    
	int         GetChaInfoFromChaNum (int nChaNum, std::vector<CharInfo> &v);
    int         GetChaInfoFromChaName(CString strChaName, std::vector<CharInfo> &v);
    int         GetChaInfoFromUserNum(int nUserNum, std::vector<CharInfo> &v);	
	
    virtual int SearchCharacter(const std::string& Query, std::vector<CharInfo>& v) override;
    virtual int SearchCharacter(std::strstream& strSearch, std::vector<CharInfo> &v) override;

	int			GetAllChar			 (std::vector<AllCharNum> &v);
	int			GetAllInven			 (std::vector<AllCharNum> &v);

    virtual int GetExchangeItemLog( const std::vector<int>& GenType, const std::vector<int>& ExchangFlag, std::vector<ExchangeItemLog>& vOut ) override;
    virtual int GetExchanageItemHistory( LONGLONG InMakeNum, std::vector<ExchangeItemHistory>& vOut ) override;

	int			GetUserInven					(GLCHARLOGIC* pChaData2);
	int			SaveCharacter					(LPVOID _pbuffer);
	int			SaveCharacterBase				(LPVOID _pbuffer);
	int			SaveCharacterInven				( LPVOID _pbuffer, bool bClone = false );
	int			SaveCharacterSkill				(LPVOID _pbuffer);
	int			SaveCharacterQuest				(LPVOID _pbuffer);
	int			SaveCharacterUserInven			(LPVOID _pbuffer);
	int			SaveCharacterPutOnItem			( LPVOID _pbuffer, bool bClone = false );
	int         SaveCharacterSkillQuickSlot		(LPVOID _pbuffer);
	int         SaveCharacterActionQuickSlot	(LPVOID _pbuffer);
	int			SaveCharacterSkillFact       	(LPVOID _pbuffer);
	int			SaveCharacterSlots				(LPVOID _pbuffer);
    int         SaveStorageDate(int nUserNum, int nStorageNum, __time64_t tTime);
    int         CopyCharacterProductInfo( DWORD dwOriginalChaNum, DWORD dwNewChaNum );
	int			ItemSearch( int mid, int sid, std::vector< ITEM_SEARCH >& result );
	int			ItemSearch( CString& guid, std::vector< ITEM_SEARCH >& result );
	int			ItemStatistic( std::vector< ITEM_STATISTIC >& result );
	int			ItemSelect( int Owner, int InvenType, std::vector< SINVENITEM_SAVE >& vecItems );
	
	int			CreateNewCharacter	 (SCHARDATA2* pCharData2, int nSvrNum=1);
	int			CreateNewCharacter2	 (SCHARDATA2* pCharData2, int nSvrNum=1) { return 0; }
    virtual int CharacterInfoBackup( int nChaNum ) override;
	BOOL		CheckInven			 (DWORD dwUserNum);
	int			MakeUserInven		 (DWORD dwUserNum);
	int			ReadUserInven		 (GLCHARLOGIC* pChaData2);	
	int			ReadUserInven		 (DWORD dwUserNum, se::ByteStream &ByteStream);


		
	//mjeon.ado
	// this method is implemented only for ADO.	
	int			WriteUserInven( LONGLONG llMoney, DWORD dwUserNum, GLCHARLOGIC *pCharData2 );


	int			WriteUserInven		 (LONGLONG llMoney, // LONGLONG __int64, 저장할 돈의 양
									  DWORD dwUserNum, // 사용자번호
									  char* pData, // 데이타포인터
									  int nSize); // 사이즈

    int	        ReadImage			 (const char* objName, int nChaNum, se::ByteStream &ByteStream);
    int	        WriteImage			 (const char* objName, int nChaNum, char* pData, int nSize);

	int         WriteGMLog           (int nGmNum, std::strstream& strCmd);
	int         WriteGMLog           (int nGmNum, std::strstream& strCmd, CString strUserIP);
	int			WriteLogRandomItem(const LOG_RANDOM_OPTION &_sLOG);
	int			WriteLogItemExchange(const LOG_ITEM_EXCHANGE& sEXCHANGE){ return 0; };


	int         GetInt               (std::string& strSearch);
	LONGLONG    GetBigInt            (std::string& strSearch);

	int         GetActiveChaNum();
	int         GetDeletedChaNum();
	int         GetUserInvenNum();
	int         GetClubNum();
	LONGLONG    GetSumChaMoney();
	LONGLONG    GetSumUserInvenMoney();
	LONGLONG    GetSumClubInvenMoney();
	
	///////////////////
	// Club
	int         GetClubInfo          (const char* szSearch, std::vector<viewClubInfo> &v);




/*
	int GetVehicleBattery(int nVehicleNum, int nCharNum);

	int GetVehicle(GLVEHICLE* pVehicle,	int nVehicleNum, int nCharNum);

	int GetVehicleInven(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum );

	int GetVehicleColor(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum );
*/


	
	//
	//mjeon.ado
	//
	// newly added features will be only supported by ADO.
	//
	int GetVehicleInfo(gmce::VVEHICLE &v, int nChaNum, int nVehicleNum = -1) {return DB_OK;}
	virtual int GetProductInfo( int nChaNum ) { return DB_OK; }
	virtual int SaveProductChangeSet() { return DB_OK; }
	virtual int LearnProductType( DWORD dwProductType ) { return DB_OK; }
	virtual int ResetProductType( DWORD dwProductType, int& addPoint ) { return DB_OK; }
	virtual int LearnProductBook( DWORD dwProductType, DWORD dwProductBook ) { return DB_OK; }
	virtual int ResetProductBook( DWORD dwProductType, DWORD dwProductBook ) { return DB_OK; }
	virtual int LearnProductExp( DWORD dwProductType, DWORD dwProductID, int& addPoint ) { return DB_OK; }
	virtual int ResetProductExp( DWORD dwProductType, DWORD dwProductID, int& addPoint ) { return DB_OK; }

	virtual int GetClubInven( int nChaNum ) { return DB_OK; }
	virtual int SaveClubInven( int nChaNum, DWORD ClubNum, std::vector< SINVENITEM_SAVE >& vecItems ) { return DB_OK; }

	virtual int GetClubNumToTransfer( std::vector< DWORD >& vecClubNum ) { return DB_OK; }
	virtual int TransferClubLockerItems( DWORD ClubNum = 0 ) { return DB_OK; }
	virtual int GetUserNumToTransfer( std::vector< DWORD >& vecUserNum ) { return DB_OK; }
	virtual int TransferLockerItems( DWORD UserNum = 0 ) { return DB_OK; }
	virtual int GetChaNumToTransfer( std::vector< DWORD >& vecChaNum ) { return DB_OK; }
	virtual int TransferCharacterItems( DWORD ChaNum = 0 ) { return DB_OK; }
	virtual int ItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) { return DB_OK; }
	virtual int ItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) { return DB_OK; }
	virtual int ItemDelete( sc::MGUID& Guid, int State, int CallType ) { return DB_OK; }

	virtual int GetCharacterItemTransferTarget( int UserNum, std::vector< DWORD >& vecChaNum ) { return DB_OK; }

	//virtual int GetTotalUserNumToTransfer( size_t& TotalUserNum ) { return DB_OK; }

	virtual int PrepareItemTransfer( DWORD UserNum ) { return DB_OK; }
	virtual int CompleteItemTransfer( DWORD UserNum ) { return DB_OK; }
	virtual int UpdateUserInvenOption( DWORD UserNum, int Option ) { return DB_OK; }

	CString		m_strUserIP;
	void		SetLocalIP();
	CString		GetLocalIP();

	//gmce::ENUM_DB_TYPE	m_nDBType;
};

#endif _C_CGMOdbcManager_H_