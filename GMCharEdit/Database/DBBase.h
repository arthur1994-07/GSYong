#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Seconds of database response time
#ifndef DB_RESPONSE_TIME
#define DB_RESPONSE_TIME	30
#endif
#define DB_CONNECTION_COUNT 10

#define DB_ERROR 	-1
#define DB_OK  0
#define DB_ROWCNT_ERROR 1

#define DB_USE  1
#define DB_NOT_USE 	0

#define DB_CHA_MAX 	-2
#define DB_CHA_DUF 	-3

#define DB_INSERT_ITEM -2
#define DB_NOT_ITEM 	-3

#define EXTREME_OK 	1

#ifndef DB_PACKET_SIZE
#define DB_PACKET_SIZE 8192
#endif

#ifndef DB_IMAGE_BUF_SIZE
#define DB_IMAGE_BUF_SIZE	2048
#endif

#ifndef DB_IMAGE_MIN_SIZE
#define DB_IMAGE_MIN_SIZE   12
#endif

#ifndef DB_POOL_SIZE
#define DB_POOL_SIZE	5
#endif

#ifndef ODBC_ERROR_MESSAGE_LENGTH
#define ODBC_ERROR_MESSAGE_LENGTH 250
#endif

#define MAX_USERID_LENGTH	64
#define MAX_CHANAME_LENGTH	64



// definitions for GMCharEdit

#ifndef USR_ID_LENGTH
#define USR_ID_LENGTH  20
#endif

#ifndef USR_PASS_LENGTH
#define USR_PASS_LENGTH  USR_ID_LENGTH
#endif

#ifndef CHR_ID_LENGTH
#define CHR_ID_LENGTH  33
#endif

#ifndef CHAT_MSG_SIZE
#define CHAT_MSG_SIZE  100
#endif

#ifndef GLOBAL_CHAT_MSG_SIZE
#define GLOBAL_CHAT_MSG_SIZE 981
#endif

#ifndef DB_SVR_NAME_LENGTH
#define DB_SVR_NAME_LENGTH 	50
#endif

#ifndef DB_NAME_LENGTH
#define DB_NAME_LENGTH  50
#endif

// #ifndef MAX_SERVERCHAR_NUM
// #define MAX_SERVERCHAR_NUM 	2
// #endif

//#ifndef MAX_ONESERVERCHAR_NUM
//#define MAX_ONESERVERCHAR_NUM 16 // 한 서버에서 만들수 있는 캐릭터 갯수
//#endif

#define MAX_ONESERVERCHAR_NUM_GMTOOL	30

#ifndef SERVER_NAME_LENGTH
#define SERVER_NAME_LENGTH 	50
#endif

/*
#ifndef MAP_NAME_LENGTH
#define MAP_NAME_LENGTH  50
#endif
*/



//
//ADO Test ConnectionStrings : DO NOT USE FOLLOWING STRINGS for LIVE SERVICE !!
//
//#define ADO_CONN_STR_USERDB	"Provider=SQLOLEDB.1;Password=RanUser@@;Persist Security Info=True;User ID=RanUser;Initial Catalog=RanUser;Data Source=192.168.100.27"
//#define ADO_CONN_STR_GAMEDB	"Provider=SQLOLEDB.1;Password=RanGameS1@@;Persist Security Info=True;User ID=RanGameS1;Initial Catalog=RanGameS1;Data Source=192.168.100.27"
//#define ADO_CONN_STR_LOGDB	"Provider=SQLOLEDB.1;Password=RanLog@@;Persist Security Info=True;User ID=RanLog;Initial Catalog=RanLog;Data Source=192.168.100.27"
//#define ADO_CONN_STR_SHOPDB	"Provider=SQLOLEDB.1;Password=RanShop@@;Persist Security Info=True;User ID=RanShop;Initial Catalog=RanShop;Data Source=192.168.100.27"


#include <vector>
#include <strstream>


#include "./DatabaseTable.h"
#include "../../RanLogic/Character/GLCharData.h"
#include "../../RanLogic/GLogicEx.h"
#include "../../RanLogic/Vehicle/GLVEHICLE.h"

struct ITEM_SEARCH
{
	int ChaNum;
	int InvenType;
	int TurnNum;
	int PetNum;
	int VehicleNum;
	sc::MGUID guid;
	ITEM_SEARCH()
		: ChaNum( 0 )
		, InvenType( 0 )
		, TurnNum( 0 )
		, PetNum( 0 )
		, VehicleNum( 0 )
	{
	}
};

struct ITEM_STATISTIC 
{
	int ItemMid;
	int ItemSid;
	int Count;
	ITEM_STATISTIC()
		: ItemMid( 0 )
		, ItemSid( 0 )
		, Count( 0 )
	{
	}
};

struct LOG_RANDOM_OPTION;
struct LOG_ITEM_EXCHANGE;
namespace gmce
{
	enum ENUM_DB_TYPE
	{
		ADO		= 0,
		ODBC	= 1
	};


	enum ENUM_ADO_INDEX
	{
		ADO_IDX_USERDB	= 0,
		ADO_IDX_GAMEDB	= 1,
		ADO_IDX_LOGDB	= 2
	};






#ifdef TIXML_USE_TICPP
	
	//
	// structures for XML-config decoding
	//
	struct A_DB
	{
		std::string name;
		std::string ip;
		std::string id;
		std::string pw;		
	};


	struct A_GROUP
	{
		std::string title;
		A_DB userDB;
		A_DB gameDB;
		A_DB logDB;
	};

	typedef std::vector<gmce::A_GROUP>::iterator	iterGROUPS;

#else

	//
	// structures for BIN-config decoding
	//
	typedef struct
	{
		TCHAR DB[32];
		TCHAR IP[16];
		TCHAR ID[32];
		TCHAR PW[32];
	} AUTH;


	typedef struct {		
		AUTH UserDB;
		AUTH GameDB;
		AUTH LogDB;
	} DB_INFO;

#endif
	


	//
	// for Vehicle page
	//
	struct VEHICLEINFO
	{
		UINT		nNum;
		CString		csName;
		BOOL		bDeleted;
		GLVEHICLE	glvehicle;
		CTime		ctCreateDate;
		CTime		ctDeleteDate;
	};



	typedef std::vector<VEHICLEINFO> VVEHICLE;
	typedef std::vector<VEHICLEINFO>::iterator ITER_VVEHICLE;

class IDBManager
{
public:
    IDBManager(ENUM_DB_TYPE ConnectionType)
        : m_ConnectionType(ConnectionType)
    {
    }
    virtual ~IDBManager() {}
    ENUM_DB_TYPE GetConnectionType() const { return m_ConnectionType; }

protected:
    ENUM_DB_TYPE m_ConnectionType;

public:
	virtual int GmLogin( CString strGMID, CString strGMPassword, CString strUserIP )	= 0;
	virtual int GetGmUserNum()															= 0;
	virtual int GetUserNum( CString strUserID )											= 0; 
	virtual int GetCharacterInfo( int nChaNum, int nUserNumber, GLCHARLOGIC* pChaData2 )					= 0;
    virtual int RestoreCharacterInfoBackup( int nChaNum ) = 0;
    virtual int GetCharacterSkill( int nChaNum, GLCHARLOGIC* pChaData2 ) = 0;
	virtual int	GetServerInfo( std::vector<viewServerInfo> &v )							= 0;

	//virtual int GetChaInfoFromChaNum( int nChaNum, std::vector<CharInfo> &v )			= 0;
	//virtual int GetChaInfoFromChaName( CString strChaName, std::vector<CharInfo> &v )	= 0;
	//virtual int GetChaInfoFromUserNum( int nUserNum, std::vector<CharInfo> &v )		= 0;

    virtual int SearchCharacter(const std::string& Query, std::vector<CharInfo>& v)     = 0;
	virtual int SearchCharacter( std::strstream& strSearch, std::vector<CharInfo> &v )	= 0;
	virtual int GetAllChar( std::vector<AllCharNum> &v )								= 0;
	virtual int GetAllInven( std::vector<AllCharNum> &v )								= 0;
    virtual int GetExchangeItemLog( const std::vector<int>& GenType, const std::vector<int>& ExchangFlag, std::vector<ExchangeItemLog>& vOut ) = 0;
    virtual int GetExchanageItemHistory( LONGLONG InMakeNum, std::vector<ExchangeItemHistory>& vOut ) = 0;

	virtual int GetUserInven( GLCHARLOGIC* pChaData2 )									= 0;
	virtual int SaveCharacter( LPVOID _pbuffer )										= 0;
	virtual int SaveCharacterBase( LPVOID _pbuffer )									= 0;
	virtual int SaveCharacterInven( LPVOID _pbuffer, bool bClone = false )				= 0;
	virtual int SaveCharacterSkill( LPVOID _pbuffer )									= 0;
	virtual int SaveCharacterQuest( LPVOID _pbuffer )									= 0;
	virtual int SaveCharacterUserInven( LPVOID _pbuffer )								= 0;
	virtual int SaveCharacterPutOnItem( LPVOID _pbuffer, bool bClone = false )			= 0;
	virtual int SaveCharacterSkillQuickSlot( LPVOID _pbuffer )							= 0;
	virtual int SaveCharacterActionQuickSlot( LPVOID _pbuffer )							= 0;
	virtual int SaveCharacterSkillFact( LPVOID _pbuffer )								= 0;
	virtual int SaveCharacterSlots ( LPVOID _pbuffer )									= 0;
	virtual int SaveStorageDate( int nUserNum, int nStorageNum, __time64_t tTime )		= 0;
    virtual int CopyCharacterProductInfo( DWORD dwOriginalChaNum, DWORD dwNewChaNum )   = 0;
	virtual int ItemSearch( int mid, int sid, std::vector< ITEM_SEARCH >& result )		= 0;
	virtual int ItemSearch( CString& guid, std::vector< ITEM_SEARCH >& result )			= 0;
	virtual int ItemStatistic( std::vector< ITEM_STATISTIC >& result )					= 0;
	virtual int ItemSelect( int Owner, int InvenType, std::vector< SINVENITEM_SAVE >& vecItems ) = 0;

	virtual int CreateNewCharacter( SCHARDATA2* pCharData2, int nSvrNum=1 )				= 0;
	virtual int CreateNewCharacter2( SCHARDATA2* pCharData2, int nSvrNum=1 )				= 0;
    virtual int CharacterInfoBackup( int nChaNum ) = 0;
	virtual BOOL CheckInven( DWORD dwUserNum )											= 0;
	virtual int MakeUserInven( DWORD dwUserNum )										= 0;
	virtual int ReadUserInven( GLCHARLOGIC* pChaData2 )									= 0;	
	
	//virtual int ReadUserInven( DWORD dwUserNum, se::ByteStream &ByteStream )			= 0;
	
	virtual int WriteUserInven( LONGLONG llMoney, DWORD dwUserNum, char* pData, int nSize ) = 0;
	virtual int WriteUserInven( LONGLONG llMoney, DWORD dwUserNum, GLCHARLOGIC *pCharData2 )= 0;	

	//virtual int WriteGMLog( int nGmNum, std::strstream& strCmd )						= 0;
	virtual int WriteGMLog( int nGmNum, std::strstream& strCmd, CString strUserIP )		= 0;
	virtual int WriteLogRandomItem(const LOG_RANDOM_OPTION &_sLOG) = 0;
	virtual int WriteLogItemExchange(const LOG_ITEM_EXCHANGE& sEXCHANGE) = 0;

	/* //only for ODBC
	virtual int	ReadImage( const char* objName, int nChaNum, se::ByteStream &ByteStream ) = 0;
	virtual int	WriteImage( const char* objName, int nChaNum, char* pData, int nSize )	= 0;
	virtual int	GetInt( std::string& strSearch )										= 0;
	virtual LONGLONG GetBigInt( std::string& strSearch )								= 0;
	*/

	virtual int GetActiveChaNum()														= 0;
	virtual int GetDeletedChaNum()														= 0;
	virtual int GetUserInvenNum()														= 0;
	virtual int GetClubNum()															= 0;

	virtual LONGLONG GetSumChaMoney()													= 0;
	virtual LONGLONG GetSumUserInvenMoney()												= 0;
	virtual LONGLONG GetSumClubInvenMoney()												= 0;
	
	virtual int GetClubInfo( const char* szSearch, std::vector<viewClubInfo> &v )		= 0;

	virtual void SetLocalIP()															= 0;
	virtual CString GetLocalIP()														= 0;

	virtual int GetVehicleInfo(gmce::VVEHICLE &v, int nChaNum, int nVehicleNum = -1)	= 0;

	virtual int GetProductInfo( int nChaNum )											= 0;
	virtual int SaveProductChangeSet()													= 0;
	virtual int LearnProductType( DWORD dwProductType )									= 0;
	virtual int ResetProductType( DWORD dwProductType, int& addPoint )									= 0;
	virtual int LearnProductBook( DWORD dwProductType, DWORD dwProductBook )			= 0;
	virtual int ResetProductBook( DWORD dwProductType, DWORD dwProductBook )			= 0;
	virtual int LearnProductExp( DWORD dwProductType, DWORD dwProductID, int& addPoint )= 0;
	virtual int ResetProductExp( DWORD dwProductType, DWORD dwProductID, int& addPoint )= 0;

	virtual int GetClubInven( int nChaNum )												= 0;
	virtual int SaveClubInven( int nChaNum, DWORD ClubNum, std::vector< SINVENITEM_SAVE >& vecItems )								= 0;

	virtual int GetClubNumToTransfer( std::vector< DWORD >& vecClubNum ) = 0;
	virtual int TransferClubLockerItems( DWORD ClubNum = 0 ) = 0;
	virtual int GetUserNumToTransfer( std::vector< DWORD >& vecUserNum ) = 0;
	virtual int TransferLockerItems( DWORD UserNum = 0 ) = 0;
	virtual int GetChaNumToTransfer( std::vector< DWORD >& vecChaNum ) = 0;
	virtual int TransferCharacterItems( DWORD ChaNum = 0 ) = 0;
	virtual int ItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) = 0;
	virtual int ItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) = 0;
	virtual int ItemDelete( sc::MGUID& Guid, int State, int CallType ) = 0;

	virtual int GetCharacterItemTransferTarget( int UserNum, std::vector< DWORD >& vecChaNum ) = 0;
	//virtual int GetTotalUserNumToTransfer( size_t& TotalUserNum ) = 0;
	virtual int PrepareItemTransfer( DWORD UserNum ) = 0;
	virtual int CompleteItemTransfer( DWORD UserNum ) = 0;
	virtual int UpdateUserInvenOption( DWORD UserNum, int Option ) = 0;

};


};
