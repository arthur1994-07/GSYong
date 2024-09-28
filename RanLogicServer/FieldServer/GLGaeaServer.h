#ifndef _GL_GAEA_SERVER_H_
#define _GL_GAEA_SERVER_H_

#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <boost/pool/object_pool.hpp>
//#include "../../SigmaCore/Memory/CMemPool.h"
#include "../../SigmaCore/Memory/ObjectPool.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Math/SeqUniqueGuid.h"

#include "../../EngineLib/DxLand/DxLandGateMan.h"
#include "../../EngineLib/G-Logic/DxConsoleMsg.h"
#include "../../EngineLib/G-Logic/DxMsgServer.h"
#include "../../EngineLib/G-Logic/GLPeriod.h"

#include "../../RanLogic/Item/GLITEMLMT.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Guidance/GLGuidanceDefine.h"
#include "../../RanLogic/Market/MarketDefine.h"
#include "../../RanLogic/PointShop/PointShopDefine.h"

#include "../Land/GLMapListServer.h"
#include "../AroundSlot/GLARoundSlot.h"
#include "../Character/GLChar.h"
#include "../PK/GLFreePK.h"
#include "../Land/GLLandMan.h"
#include "../Pet/GLPetField.h"
#include "../Summon/GLSummonField.h"
#include "../Database/DBAction/DbAction.h"
#include "../Database/DBAction/DbActionGame.h"
#include "../Server/s_CFieldServer.h"
#include "../InstantMap/InstantMapTypes.h"
#include "../MatchSystem/MatchingManager.h"
#include "../Event/EventField/EventField.h"
#include "../Money/MoneyManagerField.h"
#include "../Jackpot/GLJackpotDefine.h"

#include "../Attendance/AttendanceScriptMan.h"
#include "../CaptureTheField/CaptureTheFieldManager.h"
#include "../ServerActor.h"

//! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
#include "../RandomChance/RandomChanceCtrl.h"

//
//mjeon.tbb
//
#ifdef _USE_TBB

#include "../../=TBB/include/tbb/concurrent_vector.h"
#include "../../=TBB/include/tbb/concurrent_unordered_map.h"
#include "../../=TBB/include/tbb/concurrent_queue.h"

#endif

// 인던을 생성하기 위한 시스템 메모리 허용 최대량
// 향후 이 값을 늘리면 메모리 허용내에서 인던 생성을 늘릴 수 있다.
// 2기가에서 3기가로 메모리 사용을 늘린 후 1900 -> 2800 으로 상향 했다.
// 3500 으로 상향 20140925 khkim, 이까지 메모리 사용량이 늘면 자동 재시작하도록 한다, 필드서버
#define MAX_ALLOW_MEMORY_CREATE_INSTANTMAP 3500

#ifndef GLARoundSlot
	class GLARoundSlot;
#endif

namespace MiniGame
{
	class GLManagerField;
}
namespace InstanceSystem
{
	class ManagerField;
}
namespace GLEvent
{
	class ManagerField;
}
namespace Ping
{
	class ManagerField;
}
class GLNetMsgDelayMgr;
struct GLPartyField;
class GLCrow;
class GLPartyManagerField;
class GLPartyTenderManagerField;
class GLClubDeathMatchFieldMan;
class GLClubManField;
class GLClubField;
class GLClubDeathMatch;
class GLEventProc;
class GLGuidanceFieldMan;
class GLSchoolFreePKField;
class GLItemLimit;;
class VehicleCacheField;

class GLConsignmentSaleField;

class CharacterBackup;

class GLUserLockerLockMgr;

struct CHAR_BASE_CACHE_DATA;

float GetCurrentFrameTime();

namespace pointshop {
    class PointShopField;
}

namespace buffmanager {
    class GLBuffManagerFD;
}

namespace TexasHoldem
{
	class GLTexasHoldemField;
}

namespace MatchingSystem
{
	class GLTournamentNMatchingMoneyField;
}

namespace private_market
{
	class GLSearchDelayMgr;
}

namespace randomchance
{
	class RandomChanceCtrl;
}

struct SFIELDCROW
{
	EMCROW					emCROW;
	DWORD					dwID;
	DWORD					dwFRAME;
	DWORD					invisible;
	//sc::SGLNODE<SFIELDCROW*>*	pNODE;

	SFIELDCROW()
        : emCROW(CROW_PC)
        , dwID(0)
        , dwFRAME(0)
		, invisible(false)
        //, pNODE(NULL)
	{
	}

	void RESET()
	{
		emCROW = CROW_PC;
		dwID = 0;

		dwFRAME = 0;
		invisible = false;
		//pNODE = NULL;
	}
};
typedef SFIELDCROW* PSFIELDCROW;

struct SFIELDLANDEVENT
{
	DWORD					    dwID;
	DWORD					    dwFRAME;
    //sc::SGLNODE<SFIELDLANDEVENT*>*	pNODE;

	SFIELDLANDEVENT()
        : dwID(0)
        , dwFRAME(0)
        //, pNODE(NULL)
	{
	}

	void RESET()
	{
		dwID = 0;

		dwFRAME = 0;
		//pNODE = NULL;
	}
};
typedef SFIELDLANDEVENT* PSFIELDLANDEVENT;

namespace db {
class CharacterDbToAgentMessage;
}

struct SDROPOUTINFO
{
	DWORD				m_dwGaeaID;
	DWORD				m_dwUserID;
    db::DbActionPtr	    m_spMsg;

	SDROPOUTINFO()
        : m_dwGaeaID(0)
        , m_dwUserID(0)        
	{
	}

    SDROPOUTINFO(DWORD dwGaeaID, DWORD dwUserID, db::DbActionPtr spMsg)
        : m_dwGaeaID(dwGaeaID)
        , m_dwUserID(dwUserID)
        , m_spMsg(spMsg)
	{
	}

	const bool operator == ( const SDROPOUTINFO& rhs ) const
	{
		if ( m_dwGaeaID == rhs.m_dwGaeaID )
		{
			return true;
		}

		return false;
	}
};

struct SDROPOUTPETINFO
{
	DWORD dwPetGuid;
	bool  bLeaveFieldServer;
	bool  bMoveMap;

	SDROPOUTPETINFO() :
		dwPetGuid(UINT_MAX),
		bLeaveFieldServer(false),
		bMoveMap(false)
	{
	}

	SDROPOUTPETINFO( DWORD _dwPetGuid, bool _bLeaveFieldServer, bool _bMoveMap ) :
		dwPetGuid(_dwPetGuid),
		bLeaveFieldServer(_bLeaveFieldServer),
		bMoveMap(_bMoveMap)
	{
	}
};

struct SCLASSEVENTDATA
{
	float fItemGainRate[GLCI_NUM_ACTOR];
	float fExpGainRate[GLCI_NUM_ACTOR];
	float fMoneyGainRate[GLCI_NUM_ACTOR];

	DWORD dwExpMinLevel[GLCI_NUM_ACTOR];
	DWORD dwExpMaxLevel[GLCI_NUM_ACTOR];
	DWORD dwItemMinLevel[GLCI_NUM_ACTOR];
	DWORD dwItemMaxLevel[GLCI_NUM_ACTOR];
	DWORD dwMoneyMinLevel[GLCI_NUM_ACTOR];
	DWORD dwMoneyMaxLevel[GLCI_NUM_ACTOR];

	SCLASSEVENTDATA()
	{
		AllInitExpRate();
		AllInitItemRate();
		AllInitMoneyRate();
	}

	void AllInitExpRate()
	{
		for( BYTE i = 0; i < GLCI_NUM_ACTOR; i++ )
		{
			InitExpRate(i);
		}
	}
	void AllInitItemRate()
	{
		for( BYTE i = 0; i < GLCI_NUM_ACTOR; i++ )
		{
			InitItemRate(i);
		}
	}
	void AllInitMoneyRate()
	{
		for( BYTE i = 0; i < GLCI_NUM_ACTOR; i++ )
		{
			InitMoneyRate(i);
		}
	}

	void InitExpRate(BYTE dwClass)
	{
		fExpGainRate[dwClass]   = 1.0f;
		dwExpMinLevel[dwClass] = dwExpMaxLevel[dwClass] = 0;
	}
	void InitItemRate(BYTE dwClass)
	{
		fItemGainRate[dwClass]  = 1.0f;
		dwItemMinLevel[dwClass] = dwItemMaxLevel[dwClass] = 0;
	}
	void InitMoneyRate(BYTE dwClass)
	{
		fMoneyGainRate[dwClass] = 1.0f;
		dwMoneyMinLevel[dwClass] = dwMoneyMaxLevel[dwClass] = 0;
	}
};


struct SReserveMSG
{
	CTime  sendTime;
	BYTE   sendMsg[NET_DATA_BUFSIZE];
	DWORD  dwClientID;
	DWORD  dwGaeaID;
	
};

typedef boost::unordered_map<DWORD, GLChar*>	UMAP_GLCHAR;
typedef UMAP_GLCHAR::iterator					UMAP_GLCHAR_ITER;
typedef UMAP_GLCHAR::const_iterator				UMAP_GLCHAR_CITER;
typedef UMAP_GLCHAR::value_type					UMAP_GLCHAR_VALUETYPE;

struct TRANSFER_INFO;

typedef std::set<WORD>								SET_SEQ_RECEIVED;

typedef std::map<DWORD64, TRANSFER_INFO>				MAP_TRANSFER;
typedef std::map<DWORD64, TRANSFER_INFO>::iterator		MAP_TRANSFER_ITER;
typedef std::map<DWORD64, TRANSFER_INFO>::value_type	MAP_TRANSFER_VALUETYPE;

//
//mjeon.instance.transfer
//
struct TRANSFER_INFO
{
	TRANSFER_INFO()
		:dw64UniqueID(0xffffffffffffffff)
		,pBuffer(NULL)
		,dwSize(0)
		,bDone(FALSE)
		,dwTargetMapID(MAXDWORD)
		,dwTargetGateID(MAXDWORD)
	{
	}

	TRANSFER_INFO(const DWORD64& _dw64UniqueID, BYTE *_pBuffer, SET_SEQ_RECEIVED &_setSeq)
		:dw64UniqueID(_dw64UniqueID)
		,pBuffer(_pBuffer)
		,dwSize(0)
		,setSeq(_setSeq)
		,bDone(FALSE)
		,dwTargetMapID(MAXDWORD)
		,dwTargetGateID(MAXDWORD)
	{
	}

	DWORD GetSize()
	{
		return dwSize;
	}

	BYTE* GetBuffer()
	{
		return pBuffer;
	}

	DWORD GetMapID()
	{
		return dwTargetMapID;
	}

	DWORD GetGateID()
	{
		return dwTargetGateID;
	}

	bool Receive(BYTE *pData, DWORD dwDataSize, WORD wSeq, WORD wLastSeq)
	{
		memcpy(pBuffer + (GLMSG::SNET_TRANSFER_CHAR::BUF_SIZE * (wSeq - 1)), pData, dwDataSize);
		dwSize += dwDataSize;

		bool bOK = setSeq.insert(wSeq).second;
		
		if (bOK == false)
		{
			sc::writeLogError(sc::string::format("Transfer - Char(%1%)'s %2%th seq. transfer message is double received.", dw64UniqueID, wSeq));
		}

		bDone = ((WORD)setSeq.size() == wLastSeq);

		return bOK;
	}

	BOOL SetDestination(DWORD dwMapID, DWORD dwGateID)
	{
		GASSERT(dwMapID != MAXDWORD);
		GASSERT(dwGateID != MAXDWORD);

		if ( (dwMapID == MAXDWORD) || (dwGateID == MAXDWORD) )
			return FALSE;

		dwTargetMapID	= dwMapID;
		dwTargetGateID	= dwGateID;

		return TRUE;
	}
	
	BOOL IsDone()
	{
		return bDone;
	}

private:
	DWORD64				dw64UniqueID;
	BYTE				*pBuffer;
	SET_SEQ_RECEIVED	setSeq;
	DWORD				dwSize;
	DWORD				dwTargetMapID;
	DWORD				dwTargetGateID;
	BOOL				bDone;
};

struct CTF_REBIRTH 
{
	DWORD dwChaDbNum;
	SNATIVEID moveMapID;
	DWORD dwGateID;
	float fRestore;

	CTF_REBIRTH()
		: dwChaDbNum( 0 )
		, moveMapID( false )
		, dwGateID( 0 )
		, fRestore( 0.0f )
	{
	}

	CTF_REBIRTH( DWORD _dwChaDbNum, SNATIVEID _moveMapID, DWORD _dwGateID, float _fRestore )
		: dwChaDbNum( _dwChaDbNum )
		, moveMapID( _moveMapID.dwID )
		, dwGateID( _dwGateID )
		, fRestore( _fRestore )
	{
	}
};

//
//mjeon.instance.transfer
//
class TransferManager
{
public:

	//TRUE: data 조합 완료
	BOOL Assemble(const DWORD64& dw64UniqueID, BYTE *pData, DWORD dwDataSize, WORD wSeq, WORD wLastSeq)
	{
		BYTE *pBuffer = NULL;

		MAP_TRANSFER_ITER iter = m_mapTransfer.find(dw64UniqueID);
		if (iter == m_mapTransfer.end())
		{
			pBuffer = new BYTE[GLMSG::SNET_TRANSFER_CHAR::BUF_SIZE * wLastSeq];
			
			SET_SEQ_RECEIVED setSeq;

			TRANSFER_INFO ti(dw64UniqueID, pBuffer, setSeq);

			iter = m_mapTransfer.insert(MAP_TRANSFER_VALUETYPE(dw64UniqueID, ti)).first;
		}

		TRANSFER_INFO &rti = iter->second;

		pBuffer = rti.GetBuffer();
		
		GASSERT(pBuffer);
		if (!pBuffer)
			return NULL;

		rti.Receive(pData, dwDataSize, wSeq, wLastSeq);

		if (rti.IsDone())
			return TRUE;
		else
			return FALSE;
	}

	BYTE* Take(const DWORD64& dw64UniqueID, OUT DWORD &dwSize, OUT DWORD &dwMapID, OUT DWORD &dwGateID)
	{
		MAP_TRANSFER_ITER iter = m_mapTransfer.find(dw64UniqueID);
		if (iter == m_mapTransfer.end())
		{
			return NULL;
		}

		TRANSFER_INFO &rti = iter->second;

		BYTE *pBuffer = rti.GetBuffer();

		GASSERT(pBuffer);
		if (!pBuffer)
		{
			dwSize = 0;
			return NULL;
		}

		dwSize	= rti.GetSize();
		dwMapID	= rti.GetMapID();
		dwGateID= rti.GetGateID();

		m_mapTransfer.erase(dw64UniqueID);

		return pBuffer;
	}

	BOOL SetDestination(const DWORD64& dw64UniqueID, DWORD dwMapID, DWORD dwGateID)
	{
		MAP_TRANSFER_ITER iter = m_mapTransfer.find(dw64UniqueID);
		if (iter == m_mapTransfer.end())
		{
			return FALSE;
		}

		TRANSFER_INFO &rti = iter->second;

		return rti.SetDestination(dwMapID, dwGateID);
	}

	BOOL IsDone(const DWORD64& dw64UniqueID)
	{
		MAP_TRANSFER_ITER iter = m_mapTransfer.find(dw64UniqueID);
		if (iter == m_mapTransfer.end())
		{
			return FALSE;
		}

		TRANSFER_INFO &rti = iter->second;

		return rti.IsDone();
	}

	void Clear(const DWORD64& dw64UniqueID)
	{
		MAP_TRANSFER_ITER iter = m_mapTransfer.find(dw64UniqueID);
		if (iter != m_mapTransfer.end())
		{
			m_mapTransfer.erase(iter);
		}
	}

private:
	MAP_TRANSFER	m_mapTransfer;
};


class ServerCalcDamage;
class CharManField;
class PrivateMarketMan;
class PostRenewField;

typedef std::tr1::shared_ptr<PostRenewField> PostRenewFieldPtr;

class ARCounter
{
public:
	struct ARData
	{
		INT nSuccess;
		INT nTryCount;	// 시도 횟수;
		ARData(void) : nSuccess(0), nTryCount(0) {}
		ARData(INT success, INT tryCount): nSuccess(success), nTryCount(tryCount) {}
	};

	// DBNum, ARCounter; <방어자, 자료>;
	typedef std::map<DWORD, ARData>	MAP_ARDATA;
	typedef MAP_ARDATA::iterator	MAP_ARDATA_ITR;
	// DBNum, MAP_ARCOUNT; <공격자, MAP_ARCOUNT>;
	typedef std::map<DWORD, MAP_ARDATA>	MAP_ARCOUNT;
	typedef MAP_ARCOUNT::iterator		MAP_ARCOUNT_ITR;
	typedef MAP_ARCOUNT::value_type		MAP_ARCOUNT_VT;

public:
	MAP_ARCOUNT	m_mapARCount;

public:
	ARCounter(void)	{}
	~ARCounter(void){}

public:
	void record(DWORD dwAttacker, DWORD dwDefender, bool bSuccess)
	{
		MAP_ARCOUNT_ITR mapARCountItr = m_mapARCount.find(dwAttacker);
		if ( mapARCountItr == m_mapARCount.end() )
		{
			MAP_ARDATA mapARCountVt;
			m_mapARCount.insert( std::make_pair(dwAttacker, mapARCountVt) );
		}

		MAP_ARDATA& mapARData = m_mapARCount[dwAttacker];
		MAP_ARDATA_ITR mapARDataItr = mapARData.find(dwDefender);
		if ( mapARDataItr == mapARData.end() )
		{
			ARData arData(bSuccess?1:0,1);
			mapARData[dwDefender] = arData;
		}
		else
		{
			ARData& arData = mapARData[dwDefender];
			++arData.nTryCount;
			arData.nSuccess += (bSuccess? 1:0);
		}			
	}
};

class GLGaeaServer //: public GLMapListServer
{
public:

    typedef std::map<DWORD, DWORD>    CLIENTMAP;
	typedef CLIENTMAP::iterator	      CLIENTMAP_ITER;
    typedef CLIENTMAP::const_iterator CLIENTMAP_CITER;
    typedef CLIENTMAP::value_type     CLIENTMAP_VALUE;

	typedef std::vector<SDROPOUTINFO> VPCID;
	typedef VPCID::iterator			  VPCID_ITER;

	typedef std::set<DWORD>	PCID;
	typedef PCID::iterator	PCID_ITER;

#ifdef _USE_TBB
	typedef tbb::concurrent_vector<SDROPOUTPETINFO>	CVECPETDROPOUTINFO;
	typedef CVECPETDROPOUTINFO::iterator		ITER_CVECPETDROPOUTINFO;
#else
	typedef std::vector<SDROPOUTPETINFO>		VPETID;
	typedef VPETID::iterator					VPETID_ITER;
#endif

	
	//
	//mjeon.tbb
	// separate the process of static-maps and instant-maps.
	//
	typedef std::vector<GLLandMan*>				VECLANDMAN;			//manages main-maps
	typedef VECLANDMAN::iterator				ITER_VECLANDMAN;

    struct LandManagerPair
    {
        DWORD LandBaseID;
        GLLandManager* pLandManager;
        LandManagerPair( DWORD _LandBaseID, GLLandManager* _pLandManager )
            : LandBaseID(_LandBaseID)
            , pLandManager(_pLandManager)
        {

        }
    };

	//
	//mjeon.tbb
	//
#ifdef _USE_TBB
	/*
	Likewise std::vector, the tbb::concurrent_vector doesn't actually provide per-item locking mechanism yet
	(like tbb::concurrent_hash_map do). 
	 It is user responsibility to use vector's items without data-races.
	 Instead, the concurrent_vector provides ability to grow concurrently.
	*/
	typedef tbb::concurrent_vector<GLLandManager*>			CVECLANDMAN;
	typedef CVECLANDMAN::iterator						ITER_CVECLANDMAN;

    typedef tbb::concurrent_unordered_map<DWORD, GLLandManager*>	LandManagerMap;
    typedef LandManagerMap::iterator						LandManagerMapIter;	

    typedef tbb::concurrent_vector<LandManagerPair>     CVECLANDMANAGER;
    typedef CVECLANDMANAGER::iterator					ITER_CVECLANDMANAGER;

	//==============================================================================================
	//loop for FrameMoveLandMan

	class LoopBodyFrame
	{
		float			    m_fTime;
		float			    m_fElapsedTime;
		__time64_t&         m_tCurrentTime;
        CVECLANDMANAGER&    m_vecLandManager;
		
		public:
			LoopBodyFrame(float fTime_, float fElapsedTime_, __time64_t& m_tCurrentTime_, CVECLANDMANAGER& _vecLandManager)
			:m_fTime(fTime_)
			,m_fElapsedTime(fElapsedTime_)
			,m_tCurrentTime(m_tCurrentTime_)
            ,m_vecLandManager(_vecLandManager)
			{
				;
			}

            void operator()(const tbb::blocked_range<int>& range) const
            {	
                for(int i=range.begin(); i!=range.end(); i++)
                {
                    m_vecLandManager[i].pLandManager->frameMove(m_fTime, m_fElapsedTime, m_tCurrentTime);
                }
            }
	};
	//==============================================================================================

#else
    typedef std::map<DWORD, GLLandManager*>				LandManagerMap;			//manages instant-maps
    typedef LandManagerMap::iterator				LandManagerMapIter;

    typedef std::vector<LandManagerPair>        CVECLANDMANAGER;
    typedef CVECLANDMANAGER::iterator           ITER_CVECLANDMANAGER;

#endif	//_USE_TBB

	typedef std::set<DWORD>						SET_INSTANTMAP_ID;
	typedef std::set<DWORD>::iterator			SET_INSTANTMAP_ID_ITER;

	typedef std::list<DWORD>					LISTSEARCHSHOP;
	typedef LISTSEARCHSHOP::iterator			LISTSEARCHSHOP_ITER;

	typedef std::list<SReserveMSG>				RESERVEMSGLIST;
	typedef RESERVEMSGLIST::iterator			RESERVEMSGLIST_ITER;

	//mjeon.instance
    typedef std::map<DWORD _KEY_, GLLandManager *>				MAP_INSTANCE_LAND;
    typedef std::map<DWORD _KEY_, GLLandManager *>::iterator	MAP_INSTANCE_LAND_ITER;
    typedef std::map<DWORD _KEY_, GLLandManager *>::value_type	MAP_INSTANCE_LAND_VALUETYPE;


public:
    GLGaeaServer(EMSERVICE_PROVIDER ServiceProvider, SUBPATH* pPath);
    virtual ~GLGaeaServer();

protected:
	//
	//mjeon.attendance
	//
	AttendanceScriptMan		*m_pAttendanceScriptMan;

public:
	AttendanceScriptMan* GetAttendanceScriptMan()
	{
		return m_pAttendanceScriptMan;
	}


protected:
	//
	//mjeon.CaptureTheField
	//
	CaptureTheFieldManFld*	m_pCTFMan;
	
    //! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
    randomchance::RandomChanceCtrl* m_pRandomChanceCtrl;

    BOOL RandomChanceLoadScript() const;

protected:
    //
    //bjju.buffmanager
    //
    buffmanager::GLBuffManagerFD* m_pBuffMan;

public:
    buffmanager::GLBuffManagerFD* GetBuffMan()
    {
        return m_pBuffMan;
    }


protected:
	CString	m_strPath;
    EMSERVICE_PROVIDER m_ServiceProvider;
    SUBPATH* m_pPath;

public:
	CString& GetPath() { return m_strPath; }
	void SetPath(char* szPath) { m_strPath = szPath; }
    inline EMSERVICE_PROVIDER GetServiceProvider() const { return m_ServiceProvider; }
    void SetServiceProvider(EMSERVICE_PROVIDER ServiceProvider) { m_ServiceProvider=ServiceProvider; }

    inline SUBPATH* GetSubPath() { return m_pPath; }

public:
    ServerCalcDamage* m_pDamageCalculator;

	//mjeon
	//m_bTracePing: a flag enabling/disabling of pingtrace.
	float					m_fTime;
	bool					m_bTracePing;	//consider volatie for this flag.
	bool					m_bBigHead;
	bool					m_bBigHand;
	float					m_fBigHead;
	float					m_fBigHand;
	bool					m_bBrightEvent;
	bool					m_bPKServer;
	
	SEVENT_FACT				m_sEVENTFACT;
	SEventState				m_sEventState;

	bool					m_bEmptyMsg;

protected:
	LPDIRECT3DDEVICEQ		m_pd3dDevice;
	DxMsgServer*			m_pMsgServer;
	//DxConsoleMsg*			m_pConsoleMsg;
	db::IDbManager*			m_pDBMan;
	db::IDbManager*			m_pAdoMan;
    GLClubDeathMatchFieldMan* m_pDeathMatchMan;
    GLGuidanceFieldMan*     m_pGuidance;
    GLSchoolFreePKField*    m_pSchoolFreePk;
	GLEventProc*			m_pEventProc;
    GLItemLimit*            m_pItemLimit;
    pointshop::PointShopField* m_pPointShop;
	TexasHoldem::GLTexasHoldemField* m_pTexasHoldemMgr;
	CharacterBackup* m_pCharacterBackup;
	GLUserLockerLockMgr* m_pUserLockerLockMgr;
	private_market::GLSearchDelayMgr* m_pPrivateMarketSearchDelayMgr;
	private_market::GLSearchDelayMgr* m_pPrivateMarketLogDelayMgr;

	DWORD m_dwError;

	int						m_nServerChannel;
	DWORD					m_dwFieldSvrID;
	DWORD					m_dwAgentSlot;

	DWORD					m_dwSvrGroupNum;
	DWORD					m_dwBaseGaeaId;

	BOOL					m_bUpdate;
	bool					m_bEmulator;
	bool					m_bReservedStop;		// 서버 정지 예약

	bool					m_bGenItemHold;
	bool					m_bClubDMStarted;		// 클럽데스매치 시작여부

	//float					m_fMaxDelayMsgProc;		//	메시지 처리의 최대 지연 시간. ( 서버 Flip Time )

	DWORD					m_dwMaxClient;
	
	//PGLCHAR*				m_PCArray;				//! Character 배열

	UMAP_GLCHAR				m_mapCharacters;		//Characters
	
	GLCHARLIST				m_GaeaPCList;			//! Character
	//GLCHAR_MAP			m_PCNameMap;			//! Character Name/Pointer
	//CLIENTMAP				m_PCClientIDMAP;		//! Client slot/Gaea ID
	//CLIENTMAP				m_mapCHARID;			//! Character DB 번호/Gaea ID

    // PET
    typedef std::map<DWORD, GLPetField*>  PET_FIELD_MAP;
    typedef PET_FIELD_MAP::iterator       PET_FIELD_MAP_ITER;
    typedef PET_FIELD_MAP::const_iterator PET_FIELD_MAP_CITER;
    typedef PET_FIELD_MAP::value_type     PET_FIELD_MAP_VALUE;
    
    PET_FIELD_MAP m_PetFieldMap;
	//PGLPETFIELD* m_PETArray;    
	sc::BOOST_OBJECT_POOL_CHECKER<GLPetField> m_poolPET;
	sc::CMList<DWORD> m_FreePETGIDs; // 미사용된 PET GlobID 들.

#ifdef _USE_TBB
	CVECPETDROPOUTINFO		m_reqDropOutPet;
#else
	VPETID					m_reqDropOutPet;		//  DropOutPet PET GUID
#endif
	
//	VECLANDMAN				m_vecInstantMapSrcLandMan;			// 인던 원본 LandMan들

	QUEUE_INSTANT_INFO          m_queInstantMapToCreate;		// mjeon.생성할 인던정보(구버전)
	PVE_QUEUE_INSTANT_INFO      m_quePVEInstantMapToCreate;		// PVE 인던 생성 정보
	InstanceMapInformationQueue m_queInstanceMapToCreate;		// 생성할 인던 정보(신버전);
	DestroyInstanceMapQueue     m_queDestroyInstanceMapID;		// 삭제할 인던 정보;, 이 객체는 tbb parallel_for 안에서 돈다.(push)

    //CRITICAL_SECTION            m_CS_queDestroyInstanceMapID;   // 삭제할 인던 정보 큐를 위한 섹션;
	
	//mjeon
	// vector를 set으로 교체
	SET_INSTANTMAP_ID		m_setInstantMapIDs;					// 생성된 InstantMap의 ID들
	
	//
	//mjeon.instance
	//  이제 이 배열은 일반 필드 맵과 기존(old)의 Instant관련 코드로 생성되는 맵들만을 관리한다.
	//  새로운 instance는 m_mapInstanceLand가 관리
	//
    GLLandManager*			m_pLandManager[MAX_LAND_MID];
		
	tbb::concurrent_queue< JACKPOT_ITEM > m_JackpotItem;

    //
	//mjeon.tbb
	//
#ifdef _USE_TBB
	CVECLANDMANAGER				m_landManagerMap;	//concurrent_unordered_set
	tbb::concurrent_queue< CTF_REBIRTH > m_CTFRebirthReserved;

#else
	CVECLANDMANAGER				m_landManagerMap;		//manages basic-maps(not modified while server runs)	
	std::queue< CTF_REBIRTH >	m_CTFRebirthReserved;

#endif

	//
	//mjeon.instance
	//
	TransferManager			m_TransferMan;

	//
	//InstanceServer에서는 map으로 Key와 LandMan을 관리한다. m_pLandMan 배열은 사용하지 않는다.
	//	
	MAP_INSTANCE_LAND		m_mapInstanceLand;	//KEY <-> Instance LandMan

	

	//////////////////////////////////////////////////////////////////////////


	//
	//mjeon
	//
	// used to check whether CTime value is in valid range(year 1970 ~ 3000).
	//
public:
	BOOL					IsValidTime(CTime &);

protected:
	CTime					m_ctLowerLimit;		//1970.01.01 00:00:00
	CTime					m_ctUpperLimit;		//3000.12.31 23:59:59

	InstanceSystem::ManagerField* m_pInstanceManager; //! 인스턴스 던전 관리;
	MiniGame::GLManagerField*	m_pMiniGameManager; //! 홀짝 게임 관리;
	Ping::ManagerField*				m_pPingManager; //! ping 감청;
	GLPartyManagerField*			m_pPartyFieldMan; //! 파티관리	
	GLPartyTenderManagerField*	m_pPartyTenderManager; //! 주사위 입찰 아이템 관리;
	GLEvent::ManagerField*		m_pEventManager; //! 이벤트 매니저;

	GLClubManField*		m_pClubMan; //! 클럽관리
	VPCID					m_reqDropOutChar;		//	dropout 요청.

	Money::CMoneyManagerField*		m_pMoneyMan;
	
	//PCID					m_reqSaveDBUserID;		//	저장요청한 userid.
	//std::set<DWORD>		m_setCreatingChar;		// 캐릭터를 생성중인 UserID 목록 - 캐릭터 생성을 처리중일 떄, 접속을 종류하고 다른 캐릭터로 접속하는 것을 방지(인도네시아 락커 해킹)

	float					m_fTIMER_CLUB;


	CRITICAL_SECTION		m_CSPCLock;
	//CRITICAL_SECTION		m_csCreatingCharUserID;
	CRITICAL_SECTION		m_CS_ClubBattle;


	//mjeon.memory pool을 위한 lock
	CRITICAL_SECTION		m_CS_Char;
	CRITICAL_SECTION		m_CS_MobSchedule;
	CRITICAL_SECTION		m_CS_Crow;
	CRITICAL_SECTION		m_CS_FieldCrow;
	CRITICAL_SECTION		m_CS_Land;
	CRITICAL_SECTION		m_CS_LandEvent;
	CRITICAL_SECTION		m_CS_Pet;
	CRITICAL_SECTION		m_CS_Summon;
	CRITICAL_SECTION		m_CS_Material;
	CRITICAL_SECTION		m_CS_Pet2;

	SCLASSEVENTDATA			m_ClassEventData;

    //! Private Market/개인상점/개인 상점
	LISTSEARCHSHOP			m_listSearchShop;
    PrivateMarketMan*       m_pPrivateMarketMan;
    GLConsignmentSaleField* m_pConsignmentSaleField;   // 위탁판매

	RESERVEMSGLIST			m_listReserveMsg;
	
    sc::BOOST_OBJECT_POOL_CHECKER<GLChar>			m_poolCHAR;
	sc::BOOST_OBJECT_POOL_CHECKER<GLMobSchedule>	m_poolMobSchedule;
    sc::BOOST_OBJECT_POOL_CHECKER<GLCrow>			m_poolCROW;
    sc::BOOST_OBJECT_POOL_CHECKER<GLMaterial>		m_poolMATERIAL;

	sc::BOOST_OBJECT_POOL_LOCK_CHECKER<SFIELDCROW>		m_poolFIELDCROW;//sc::ObjectPoolLock<SFIELDCROW>	  m_poolFIELDCROW;//sc::CMemPool<SFIELDCROW>	  m_poolFIELDCROW;
    sc::BOOST_OBJECT_POOL_LOCK_CHECKER<SFIELDLANDEVENT>	m_poolFIELDLANDEVENT;//sc::ObjectPoolLock<SFIELDLANDEVENT> m_poolFIELDLANDEVENT;//sc::CMemPool<SFIELDLANDEVENT> m_poolFIELDLANDEVENT;

    sc::BOOST_OBJECT_POOL_CHECKER<GLSummonField> m_poolSummon;

    GLMapListServer* m_pMapList;

    CharManField* m_pCharMan;

    VehicleCacheField* m_pVehicleCache;

	GLNetMsgDelayMgr* m_pNetMsgMgr;

	// Post Renew
	PostRenewFieldPtr m_spPostField;

protected:
	double					m_fCrashTimer;
	double					m_fDelayMilliSec;
	DWORD					m_dwServerCrashTime;
	WORD					m_wSkipPacketNum;

public:
	inline const int GetCrowPool_MaxSize() { return m_poolCROW.GetMaxObjectSize(); }

public:
	void ReserveCTFRebirth( DWORD dwChaDbNum, const SNATIVEID& _moveMapID, const DWORD _dwGateID, const float _fRestore );
	void ProcessCTFRebirth();

public:
	void Online(const DWORD dwGaeaID);
	void Offline(const DWORD dwGaeaID);

	void ReportFieldServerStateToAgent(const int _nUsedMemory);
	void ReportUsedMemoryPerLand(void);

    //! Guidance
    GLGuidanceFieldMan* GuidanceMan();
    GLGuidance* GuidanceFind(DWORD MapId);
    DWORD GuidanceId(DWORD ClubId);
    EMCHECKCERTIFY GuidanceCheckCertify(DWORD dwID, DWORD dwCHARID);
    void GuidanceDoCertify(DWORD dwID, DWORD dwCHARID, DWORD dwNpcGlobID, const D3DXVECTOR3 &vPOS);
    const std::string GuidanceGetName(DWORD Id);
    void GuidanceSetState(VECGUID_DB& vecGUID);
	void SendWorldBattleNationFlag( GLChar* pChar );
    bool IsGuidanceBattle() const;

    bool SchoolFreePkOn() const;
    void SchoolFreePkSet(bool Set);

	void AddGameDbJob(db::DbActionPtr spJob);
	//mjeon.ado
	void AddGameAdoJob(db::DbActionPtr spJob);
	void AddLogAdoJob(db::DbActionPtr spJob);
	void AddUserAdoJob(db::DbActionPtr spJob);
	void AddShopAdoJob(db::DbActionPtr spJob);

    void LoadClubDeathMatch(const std::vector<std::string>& vecFile, bool bToolMode);
    bool IsClubDeathMatch(DWORD MapId);
    DWORD ClubDeathMatchAccrueCount(DWORD MapId, DWORD CharId);
    GLClubDeathMatch* ClubDeathMatchFind(DWORD MapId);
    void ClubDeathMatchAddCdmScore(GLClubDeathMatch* pCDM, DWORD dwKillClub, DWORD dwDeathClub, DWORD dwKillChar, DWORD dwDeathChar);

	void SENDTOCLIENT_PINGREQ( DWORD dwClientID );
	void SetServerCrashTime( DWORD dwSec )	{ m_dwServerCrashTime = dwSec; }
	BOOL SetServerCrashTime(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void SetDelayMilliSec( DWORD dwSec )	{ m_fDelayMilliSec =(double)dwSec; }
	BOOL SetDelayMilliSec(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void SetSkipPacketNum( WORD wNum )		{ m_wSkipPacketNum = wNum; }
	BOOL SetSkipPacketNum(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	double GetLimitFPS() const	{ return(m_fDelayMilliSec*0.001f); }

	float GetExpGainRate( EMCHARINDEX CLASS, DWORD dwLevel );
	float GetItemGainRate( EMCHARINDEX CLASS, DWORD dwLevel );
	float GetMoneyGainRate( EMCHARINDEX CLASS, DWORD dwLevel );

	SENTRYINSTMAPINFO		PopCreateInstMapInfo();	

	SCLASSEVENTDATA&		GetClassEventData() { return m_ClassEventData; }

    GLItemLimit* GetItemLimit() { return m_pItemLimit; }

	TexasHoldem::GLTexasHoldemField* GetTexasHoldemManager() { return m_pTexasHoldemMgr; } 

public:
	GLMobSchedule* NEW_MOBSCHEDULE ();
	void RELEASE_MOBSCHEDULE ( GLMobSchedule* pSch );

	GLCrow* NEW_CROW();
	void RELEASE_CROW( GLCrow* pCROW );

    GLChar* NEW_CHAR(wb::EM_COUNTRY Country);
	void RELEASE_CHAR( GLChar* pCHAR );

	GLMaterial* NEW_MATERIAL();
	void RELEASE_MATERIAL( GLMaterial* pMaterial );

	SFIELDCROW* NEW_FIELDCROW();
	void RELEASE_FIELDCROW( SFIELDCROW* pFIELDCROW );

	SFIELDLANDEVENT* NEW_FIELDLANDEVENT();
	void RELEASE_FIELDLANDEVENT( SFIELDLANDEVENT* pFIELDLANDEVENT);

	PGLPETFIELD NEW_PET();
	void RELEASE_PET( PGLPETFIELD pPet );

	PGLSUMMONFIELD NEW_SUMMON();
	void RELEASE_SUMMON( PGLSUMMONFIELD pSummon );

    GLLandManager* NEW_GLLANDMANAGER(const DWORD nInstanceMapID, const DWORD nBaseMapID);
    void RELEASE_GLLANDMANAGER(GLLandManager* const _pLandManager);

public:
    const std::map<DWORD, DWORD>& GetCharIDMap();
	const std::map<DWORD64, DWORD>& GetCharIDMap64();

public:
	void SetAgentSlot(DWORD dwClient) { m_dwAgentSlot = dwClient; }
	DWORD GetAgentSlot() const { return m_dwAgentSlot; }
	void SetReserveServerStop() { m_bReservedStop = true; }
	bool IsReserveServerStop() const { return m_bReservedStop; }

public:
	bool IsGenItemHold() const { return m_bGenItemHold; }
	bool IsBRIGHTEVENT() const { return m_bBrightEvent; }
	bool IsClubDMStarted() const { return m_bClubDMStarted; }
	bool IsPKServer() const { return m_bPKServer; }

public:
	DxMsgServer* GetMsgServer()	{ return m_pMsgServer; }
	//DxConsoleMsg* GetConsoleMsg()	{ return m_pConsoleMsg; }
	db::IDbManager* GetDBMan() { return m_pDBMan; }
	db::IDbManager* GetAdoMan() { return m_pAdoMan; }

	DWORD GetFieldSvrID() const	{ return m_dwFieldSvrID; }
    int GetServerChannel() const { return m_nServerChannel; }

	DWORD GetServerGroup() const {return m_dwSvrGroupNum;}

	net::EMSERVERTYPE GetServerType() { return m_pMsgServer->GetServerType(); }

	BOOL IsInstance() 
	{
		if( !m_pMsgServer )
			return FALSE;
		return (m_pMsgServer->GetServerType() == net::SERVER_INSTANCE);
	}

	BOOL IsField()
	{
		if( IsEmulatorMode() )
			return TRUE;
		if( !m_pMsgServer )
			return TRUE;
		return (m_pMsgServer->GetServerType() == net::SERVER_FIELD);
	}

public: //! Party;	
	GLPartyField* const GetParty(const GLPartyID& PartyID);
	GLPartyManagerField* GetPartyManager();
	GLPartyTenderManagerField* const GetPartyTenderItemManager(void) const;


	GLClubManField* GetClubMan();
    std::tr1::shared_ptr<GLClubField> GetClub(DWORD ClubNum);

public:
	inline GLChar* GetChar(DWORD dwGaeaId) const
    {
		//GASSERT(dwGaeaId != GAEAID_NULL);
		if (dwGaeaId == GAEAID_NULL)
			return NULL;
		//GASSERT(GaeaId < m_dwMaxClient);	//Char가 아닌 것들의 GaeaID가 넘어오기도 한다. 이 때, Null일 경우 Char가 아닌 것으로 간주하므로 이 범위의 값들에 대해서 GASSERT 해제.	

		// 일반적인 Char의 GaeaID는 0일수가 없음(에뮬레이터 모드에서만 0);
        if (dwGaeaId == 0 && (IsEmulatorMode() == false) ) 
            return NULL;

		UMAP_GLCHAR_CITER iter = m_mapCharacters.find(dwGaeaId);
		if (iter != m_mapCharacters.end())
			return iter->second;
		else
			return NULL;
    }

	void RegisterCharacter(DWORD dwGaeaId, GLChar* pChar)
	{
		UMAP_GLCHAR_ITER iter = m_mapCharacters.find(dwGaeaId);
		if (iter != m_mapCharacters.end())
		{
			GLChar* const pCharPrevious(iter->second);
			const MapID& _mapIDPrevious(pCharPrevious->GetCurrentMap());
			const MapID& _mapIDCurrent(pChar->GetCurrentMap());

			const DWORD dwPointerValuePrevious = DWORD(pCharPrevious);
			const DWORD dbNumPrevious(pCharPrevious->CharDbNum());
			const SNATIVEID _baseMapIDPrevious(_mapIDPrevious.getBaseMapID());
			const SNATIVEID _gaeaMapIDPrevious(_mapIDPrevious.getGaeaMapID());

			const DWORD dwPointerValueCurrent = DWORD(pChar);
			const DWORD dbNumCurrent(pChar->CharDbNum());			
			const SNATIVEID _baseMapIDCurrent(_mapIDCurrent.getBaseMapID());
			const SNATIVEID _gaeaMapIDCurrent(_mapIDCurrent.getGaeaMapID());

			const std::string _stringErrorMessage(sc::string::format(
				"RegisterCharacter() : duplicate Character _ charPrev(%1%)[%2%] map((%3%/%4%)(%5%/%6%)) _ charCurr(%7%)[%8%] map((%9%/%10%)(%11%/%12%))",
				dwPointerValuePrevious, dbNumPrevious, _baseMapIDPrevious.Mid(), _baseMapIDPrevious.Sid(), _gaeaMapIDPrevious.Mid(), _gaeaMapIDPrevious.Sid(),
				dwPointerValueCurrent, dbNumCurrent, _baseMapIDCurrent.Mid(), _baseMapIDCurrent.Sid(), _gaeaMapIDCurrent.Mid(), _gaeaMapIDCurrent.Sid()));

			sc::writeLogError(_stringErrorMessage);

			m_mapCharacters.erase(iter);
		}

		m_mapCharacters.insert(UMAP_GLCHAR_VALUETYPE(dwGaeaId, pChar));    
	}

	void UnregisterCharacter(DWORD dwGaeaId)
	{
		UMAP_GLCHAR_ITER iter = m_mapCharacters.find(dwGaeaId);
		if (iter != m_mapCharacters.end())
			m_mapCharacters.erase(iter);
	}

	BOOL CheckCharacterExistance(DWORD dwGaeaId)
	{
		UMAP_GLCHAR_ITER iter = m_mapCharacters.find(dwGaeaId);
		
		return (iter != m_mapCharacters.end());
	}

	//mjeon.instance	
	GLChar* GetChar64(DWORD dwChaNum, DWORD dwSvrGroupNum = MAXDWORD);

	//반드시 아래의 GetChar64()를 통해서 Char*를 획득하라.
public://protected:	<-- GetCharByDbNum을 모두 GetChar64로 대체하거나 대체를 완료했을때 protected로 교체하자.
	GLChar* GetCharByDbNum(DWORD ChaDbNum);
	GLChar* GetCharByUniqueID(UNIQUEID &dw64UniqueID);

	Country::SCOUNTRY_INFO GetCountry( DWORD ChaDbNum );
	void InitializeCountry ( DWORD dwClientID );

    GLCHARLIST& GetCharList() { return m_GaeaPCList; }
	DWORD GetNumPC() const { return m_mapCharacters.size(); }
	DWORD GetMaxClient() const { return m_dwMaxClient; }

	BOOL IsValidGaeaID(DWORD dwGaeaId) const
	{
		if (m_pMsgServer->GetServerType() == net::SERVER_INSTANCE)	//InstanceServer에는 모든 서버군의 GaeaID들이 들어온다.
		{
			if (dwGaeaId == GAEAID_NULL)
				return FALSE;
			else
				return TRUE;
		}		

		return ((dwGaeaId >= m_dwBaseGaeaId) && (dwGaeaId < m_dwBaseGaeaId + m_dwMaxClient));
	}

public:
	void ChangeNameMap( PGLCHAR pChar, const char* pszOldName, const char* pszNewName );
	void ChangeNameMap( PGLCHAR pChar, const TCHAR* pszPhoneNumber );

protected:
	BOOL SaveCheckInstantMap(GLChar *pChar);
	BOOL DropPC(GLLandMan *pLandMan, const D3DXVECTOR3& vPos, GLChar *pPC);
	BOOL DropOutPC(GLChar *pChar);

public:
	//! mjeon.character full DB save
    BOOL SaveCharacterIntoDB( GLChar* pChar, db::DbActionPtr spAfterJob, bool bSaveToSqlite = false );

	// 서버가 강제 종료되면서 sqlite 에 저장한다.
	BOOL SaveCharacterIntoSqlite( GLChar* pChar );
	
	const bool PreSaveCharDB ( DWORD _dwChaDBNum );
	BOOL SaveCharDB( DWORD dwGaeaID, bool bSaveWorldBattle = false, bool bAutoSave = false );

public:
	BOOL ClearReservedDropOutPC();

public:
	BOOL ReserveServerStop(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	// 종료시 모든 클럽 배틀 진행상황을 저장한다.
public:
	BOOL SaveClubBattle();
	void DelPlayHostileClubBattle( DWORD dwClub_P, DWORD dwClub_S );
	void DelPlayHostileAllianceBattle( DWORD dwClub_P, DWORD dwClub_S );

public:
	const bool IsEmulatorMode(void) const { return m_bEmulator; }

	void InsertSearchShop( DWORD dwGaeaID );
	void EraseSearchShop( DWORD dwGaeaID );
	bool FindSearchShop( DWORD dwGaeaID );

public:
    BOOL ReserveDropOutPC(DWORD dwGaeaID, db::DbActionPtr spDbActToAgentMsg);
	
	BOOL FindSaveDBUserID(DWORD dwUserID);
	//void SetSaveDBUserID( DWORD dwUserID );
	BOOL ResetSaveDBUserID( DWORD dwUserID );

	BOOL FindCreatingCharUserID(DWORD dwUserID);
	void SetCreatingCharUserID(DWORD dwUserID);
	BOOL ResetCreatingCharUserID(DWORD dwUserID);

	BOOL SaveNpcCommission( DWORD dwCharID, DWORD dwUserID, LONGLONG lnCommission );
	BOOL SaveNpcCommissionDB( DWORD dwCharID, DWORD dwUserID, LONGLONG lnCommission );

public:
	GLChar* CreateCharacter(
        PCHARDATA2 pCharData,
        DWORD dwChaNum, //DWORD dwClientID,
        DWORD dwGaeaID,	
		GLPartyID PartyID,
        wb::EM_COUNTRY Country,	// 사용하지 않는다;
		DWORD dwCountryID,
        BOOL bNEW=FALSE,
		MapID _StartMap = MapID(),
        DWORD _dwStartGate=0,
        D3DXVECTOR3 vPos=D3DXVECTOR3(0,0,0),
		EMGAME_JOINTYPE emJOINTYPE=EMJOINTYPE_FIRST,
		__time64_t loginTime=0,
        EMIP_BONUS_CLASS emIPBonus=EMIP_BONUS_NONE);

	GLChar* InitSerializedChar(GLChar *pChar, DWORD dwKey, SNATIVEID _StartMap, DWORD _dwStartGate, D3DXVECTOR3 _vPos = D3DXVECTOR3(0,0,0));

    GLChar* CreateCharacterFail(GLChar* pChar, DWORD dwChaNum);

	void UpdateUserLockerOption( DWORD UserNum, int Option );

	//
	//mjeon.instance.transfer
	//
	int SerializeFromChar(IN GLChar *pChar, OUT BYTE buffer[MAX_SERIALIZED_CHAR_SIZE]);
	int SerializeToChar(IN BYTE buffer[MAX_SERIALIZED_CHAR_SIZE], int nCompressedLength, OUT GLChar *pChar);

	BOOL TransferTo(GLChar *pChar, DWORD dwInstanceServerNum, SNATIVEID idMap, DWORD dwGateID, BOOL bOnly4DBSave = FALSE);
	
	BOOL		EnterLand( DWORD dwGaeaID, DWORD dwGateID, DWORD dwGateToIndex, SNATIVEID sMapID = SNATIVEID(false) );
    BOOL		EnterLand( const DWORD dwCharDbNum,  const SNATIVEID toGaeaMapID, const DWORD dwToGateID, const DWORD dwGateToIndex, const D3DXVECTOR3& vPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//mjeon.CaptureTheField	
	//	
	BOOL		RecallTo(GLChar *pChar, SNATIVEID &idMap, DWORD dwGateID);	// recall a character to the specific map's gate.	
	BOOL		IsCTFPvPMap(const SNATIVEID& mapID);
	BOOL		IsCTFPvEMap(const SNATIVEID& mapID);
	BOOL		IsCTFPlaying();
	BOOL		IsCTFRewarding();	
	BOOL		IsCTFStop();
	CTime GetCTFReadyTime();

	ENUM_CTF_STATE GetCTFState();
	
	DWORD		GetCTFPvPMapID()
	{
		GASSERT(m_pCTFMan != NULL);
		return m_pCTFMan->GetPvPMapID();
	}
	
	DWORD		GetCTFPvEMapID()
	{
		GASSERT(m_pCTFMan != NULL);
		return m_pCTFMan->GetPvEMapID();
	}

	DWORD		GetCTFRevivalGate(WORD wSchool)
	{
		return m_pCTFMan->GetRevivalGate(wSchool);
	}

    ENUM_SCHOOL	GetCTFAuthedSchool()
    {
        return ENUM_SCHOOL(m_pCTFMan->GetAuthedSchool());
    }

    CTime& GetCTFStartTime()
    {
        GASSERT(m_pCTFMan != NULL);
        return m_pCTFMan->GetStartTime();
    }

	CTime& GetCTFEndTime()
    {
        GASSERT(m_pCTFMan != NULL);
        return m_pCTFMan->GetEndTime();
    }

    UINT GetCTFStartCycleS()
    {
        GASSERT(m_pCTFMan != NULL);
        return m_pCTFMan->GetStartCycleS();
    }

	UINT GetCTFPlayTimeM()
    {
        GASSERT(m_pCTFMan != NULL);
        return m_pCTFMan->GetPlayTimeM();
    }

	UINT GetRewardTimeM()
	{
        GASSERT(m_pCTFMan != NULL);
        return m_pCTFMan->GetRewardTimeM();
	}

	void CTFNewPlayerHasCome(GLChar *pChar)
	{
		GASSERT(pChar != NULL);
		m_pCTFMan->NewPlayerHasCome(pChar);
	}

	void CTFRebirthPlayerBuff(GLChar *pChar)
	{
		GASSERT(pChar != NULL);
		m_pCTFMan->Rebirth(pChar);
	}
    
    UINT GetCTFMaxNumOfPlayers()
    {
        GASSERT(m_pCTFMan != NULL);
		return m_pCTFMan->GetMaxNumOfPlayers();
    }

	const D3DXVECTOR3& GetCertPos1()
	{
		return m_pCTFMan->GetCertPos1();
	}

	const D3DXVECTOR3& GetCertPos2()
	{
		return m_pCTFMan->GetCertPos2();
	}

	const D3DXVECTOR3& GetCertPos3()
	{
		return m_pCTFMan->GetCertPos3();
	}

	WORD GetSchoolAuthedCert1()
	{
		return m_pCTFMan->GetSchoolAuthedCert1();
	}

	WORD GetSchoolAuthedCert2()
	{
		return m_pCTFMan->GetSchoolAuthedCert2();
	}

	WORD GetSchoolAuthedCert3()
	{
		return m_pCTFMan->GetSchoolAuthedCert3();
	}

	DWORD GetCTFDamage()
	{
		return m_pCTFMan->GetDamage();
	}
	DWORD GetCTFRecovery()
	{
		return m_pCTFMan->GetRecovery();
	}



	DWORD		GetCTFRebirthTime(WORD wSchool);
    void        AddCTFRewardBuff(SNATIVEID id, WORD wLevel);
    void        GetCTFRewardBuff(UINT idx, OUT CTF_REWARD_BUFF &rewardBuff);
    UINT        GetCTFRewardBuffSize();
    void        ClearCTFRewardBuff()    { m_pCTFMan->ClearRewardBuff(); }

	BOOL		IsCTFBattleGround()	{ return m_pCTFMan->IsBattleGround(); }


	// PET
	GLPetField* CreatePET( GLPET* pPetData, DWORD OwnerDbNum, DWORD PetDbNum, bool bValid = true );
	BOOL		CreatePETOnDB(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL		GetPETInfoFromDB( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL		GetPETInfoFromDBError(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	// NET_MSG_PET_REQ_PETCARDINFO
	BOOL		MsgPETInfoCardReq(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL		DropPET( PGLPETFIELD pPet, SNATIVEID sMapID );
	BOOL		DropOutPET( DWORD dwGUID, bool bLeaveFieldServer, bool bMoveMap );
	GLPetField* GetPET(DWORD dwGUID);
	void		ReserveDropOutPet(const SDROPOUTPETINFO& sPetInfo ) { m_reqDropOutPet.push_back(sPetInfo); }
	void		ClearReserveDropOutPet();

	BOOL		RequestUsePETCARD(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	// NET_MSG_PET_REQ_UNUSECARD
	BOOL		RequestUnUsePETCARD(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL		RequestRevivePet(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL		RevivePet( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

	// VEHICLE
	BOOL		ReqActiveVehicle(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL		ReqGetVehicle(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL		CreateVehicle( DWORD dwClientID, DWORD dwGaeaID, DWORD dwVehicleID );
	BOOL		CreateVehicleOnDB(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL		GetVehicleInfoFromDB( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL		GetVehicleInfoFromDBError(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void		SaveVehicle( DWORD dwClientID, DWORD dwGaeaID, bool bLeaveFieldServer );
	void		SaveVehicle( DWORD dwClientID, GLChar *pChar, bool bLeaveFieldServer );
	void		SetMapMoveVehicle( DWORD dwClientID, DWORD dwGaeaID, DWORD dwGateToIndex );
	void		SetActiveVehicle( DWORD dwClientID, DWORD dwGaeaID, bool bActive, bool bAutoRide );
	BOOL		GetVehicleItemInfo(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL		WithVehicle(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL		WithVehicleTarAns(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	// 소환수
	BOOL		RequestSummon   (DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL		DropOutAllSummon( DWORD dwGaeaID );
	BOOL        DropOutAllSummon( PGLCHAR pChar  );

	//	예약 메시지 처리
	void		ReserveMessage( DWORD dwClientID, DWORD dwGaeaID, CTime time, LPVOID nmg );
	void		ReserveMessage( DWORD dwClientID, DWORD dwGaeaID, DWORD dwLatterSec, LPVOID nmg );
	void		ReserveMessageProcess();
	
    // QBox On/Off 옵션
	BOOL		ReqQBoxEnable(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//	클럽데스매치 랭킹 요청
	BOOL	ReqClubDeathMatchRanking(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	// 란모바일
	BOOL RanMobileBuyItem( GLChar* pChar, const SNATIVEID& ItemID, DWORD ItemNum, float fExpRaio = 0.0f );
public:
	struct GLFieldPacketFunc
	{
		( BOOL )( GLGaeaServer::*Msgfunc )(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
		GLFieldPacketFunc()
		{
			Msgfunc = NULL;
		}
	};
	GLFieldPacketFunc m_MsgFunc[NET_MSG_GCTRL_MAX];

	void initPacketFunc();

public:
	BOOL MsgReqMoveMap( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL RequestMustLeaveMap(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestGateOutReq(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	//BOOL RequestReBirth(const DWORD dwGaeaID, GLLandMan *pLandMan, const DWORD dwGenGate, const D3DXVECTOR3 &_vPos);
	BOOL RequestMoveMapPC(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestLandIn(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
		
	BOOL TransferToInstance(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL TransferChar(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL TransferCharJoinAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL TransferFromInstance( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );

	BOOL RequestMoveMapStateFC(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestFieldSvrOut(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL RequestTrade(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestTradeTarAns(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL RequestTradeMoney(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestTradeItemResist(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestTradeItemReMove(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL RequestTradeLock(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL RequestTradeAgree(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestTradeCancel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL RequestFieldSvrCharChk(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL RequestConfrontTarget(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL RequestConfrontPartyChkMbr(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestConfrontParty(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestConfrontPartyEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL RequestConfrontClubChkMbr(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestConfrontClub(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestConfrontClubEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL RequestClubRank2Fld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestClubInfo(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgClubAuthAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestClubDissolution2Fld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestClubDel2Fld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	
	BOOL RequestClubAdd2Fld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestClubDel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestClubMarkChange(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	//BOOL RequestClubSubMaster(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgClubCdCertifyChangeAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestClubAuthority(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	

	BOOL RequestChargedItem2Inven(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    void PurchaseItemRestoreDB(const char* szPurKey);

	BOOL RequestConftSPtyExp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL RequestGenItemFieldFB( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

	BOOL RequestInvenRecallThisSvr( GLChar* pPC, const MapID& mapID, DWORD dwGATEID, const D3DXVECTOR3& vPOS );

	BOOL RequestInvenTeleport(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestInvenRecall(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestBus( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL RequestTaxi( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL RequestTaxiNpcPos( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL Request2FriendCK(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL Request2FriendREQ(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestMove2CharPos(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	
	BOOL RequestCreateInstantMapReq(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);    
	BOOL RequestCreateInstantMap (DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL RequestInstantMapMoveOut(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL RequestNpcRecall( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL RequestNpcCommission(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	
public:
    // Guidance Battle
	BOOL ServerClub_GuidBattleStart(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerClub_GuidBattleEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	
	BOOL ServerClubDeathMatchReady(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerClubDeathMatchStart(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerClubDeathMatchEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	
	BOOL ServerClubCertify(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerClubGuidCommission(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerClubNotice(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	//BOOL ServerClubStorageGetDB(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	//BOOL ServerClubInComeDn(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerClubAllianceAddFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerClubAllianceDel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerClubAllianceDis(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL ServerClubBattleBeginFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerClubBattleOverFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerClubBattleKillUpdate(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerClubBattleLastKillUpdate(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL ServerLevelEventEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerLevelEventWarning(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerLevelEventCountdown(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL ServerFieldInfoReset(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerSchoolFreePk(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL ServerCtrlPeriod(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerCtrlWeather(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerCtrlWeather2(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerCtrlTime(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerCtrlMonth(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerCtrlGenItemHold(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerCtrlPlayerKillingMode(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ServerCtrlCharDropOutForced(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);


public:
	//
	//mjeon.Send.Post
	//	
	BOOL MsgPostSend( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgPostOpenAndTakeAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgPostToDoAfterSendFF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );

	//
	// Called directly by server-side (NOT a message handler)
	// parameter bAttachMoneyLimit 개인상점 시스템 우편일 경우 금액 제한 사용하지 않도록 하기 위해서
	BOOL PostSendSystemMail( UINT nRecver, std::string &strRecverName, DWORD dwAttachedItemID, LONGLONG llAttachedMoney, UINT iDesign, UINT nSenderSystem, std::string &strSenderName, std::string &strTitle, std::string &strContent, bool bAttachMoneyLimit = true);
	BOOL PostSendSystemMail( DWORD dwRecverDbNum, std::string& strRecverName, CItemDrop* pItemDrop, LONGLONG llAttachedMoney, UINT iDesign, UINT nSenderSystem, std::string &strSenderName, std::string &strTitle, std::string &strContent, bool bAttachMoneyLimit = true);
	BOOL PostSendSystemMail( DWORD dwRecverDbNum, std::string& strRecverName, SITEMCUSTOM& sItem, LONGLONG llAttachedMoney, UINT iDesign, UINT nSenderSystem, std::string &strSenderName, std::string &strTitle, std::string &strContent, bool bAttachMoneyLimit = true );

    //BOOL PostSendSystemMail( DWORD dwRecverDbNum, std::string& strRecverName, SITEMCUSTOM& sItem, LONGLONG llAttachedMoney, UINT iDesign, UINT nSenderSystem, std::string &strSenderName, std::string &strTitle, std::string &strContent, bool bAttachMoneyLimit = true );

    //
    //mjeon.sns
    //
    BOOL MsgSNSFBUpdateAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
    BOOL MsgSNSTWUpdateAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );

	//
	//mjeon.activity
	//	
	BOOL MsgActivityDoneItemTake( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgActivityChangeBadge( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgActivityCompletedFF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgActivityLoadCompletedFF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );	

	//
	//mjeon.attendance
	//	
	BOOL MsgAttendanceSyncFF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgAttendanceResetFF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgAttendanceUpgradeFF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgAttendanceDailyMaintenanceStartAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgAttendanceDailyMaintenanceEndAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgAttendanceTakeRewardCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgAttendanceDonePointItemCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgAttendanceWhisperDoneAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgAttendanceLoadEnd( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	

	//
	//mjeon.CaptureTheField
	//
	BOOL MsgCTFReadyAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgCTFPrepareAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgCTFStartAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );	
	BOOL MsgCTFJoinCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );	
	BOOL MsgCTFRecallToAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
    BOOL MsgCTFRewardAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgCTFStopAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgCTFDoneAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );	
	BOOL MsgCTFSyncPlayerNumAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgCTFQuitCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
    BOOL MsgCTFQuitAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );
	BOOL MsgCTFNextReadyTimeAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg );

	// Lotto System
	BOOL MsgLottoSystemProc( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

	// Summon;
	BOOL MsgReqSummonProc ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgReqSummonEventSkillProc ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgHireBasicSummonProc ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

	//mjeon
	//Process Ping Msg
	BOOL ProcessPingMsg( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC * nmg );
	BOOL ProcessPingTraceMsg( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC * nmg );

	BOOL GMCtrlNonConfrontMode(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL GMCtrolMove2Gate(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMCtrolMove2MapPos(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL GMCtrlWhereNpc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMCtrlWhereNpcALL(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMCtrolWherePcPos(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMCtrlWarningMSG(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL GMCtrolBigHead(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMCtrolBigHand(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL GMCtrolFreePK(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMKicUser(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMCtrolShowMeTheMoney(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL GmShowMoTheMoneyEmulatorCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL GMCtrolWhySoSerious( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
    BOOL MsgLogToDB(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    BOOL GmMakeItem( GLChar* pChar, const SNATIVEID& ItemID, DWORD ItemNum );

    BOOL GmWhySoSeriousEmulatorCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL GMCtrolMobGen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMCtrolMobDel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL GMCtrolMoPrintCrowList(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL GMCtrolMobGenEx(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMCtrolMobDelEx(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL GMCtrolMobLevel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMCtrolMobLevelLayer(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMCtrolMobLevelList(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMCtrolMobLevelClear(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMCtrolChatBlockFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMCtrolCharInfoFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL UserCtrolCharInfoFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMFlyCameraControlCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMFlyCameraSetViewSight(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL GMFlyCameraControlMoveCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	// GM이 있는 맵의 모든 캐릭터들의 점령전 버프를 삭제한다.
	BOOL GMCtrolReleaseCTFBuff( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

	BOOL RequestViewAllPlayer(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL ReqClubMemberRename(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    void ClubMemberLevelUp(DWORD ClubDbNum, DWORD ChaDbNum, WORD ChaLevel);

	BOOL ReqSearchShopItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ReqSearchResultShopItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL RequestShopInfo(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL CyberCafeClassUpdate(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL CheckMarketState(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL VietnamAllInitTime(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL MsgVietTimeReset( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_VIETNAM_TIME_REQ_FB *pNetMsg ); //vietnamtest%%% && vietnamtest2

	//! MiniGame msg;
	BOOL MsgMiniGame(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! Instance System;
	BOOL MsgInstanceSystemProc ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg );
	BOOL MsgInstanceSystem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgInstanceSystemContents(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgInstanceSystemContentsMsgPack(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgInstanceMail(const NET_MSG_GENERIC* const _pMessage);

	//! Faction System;
	BOOL MsgFactionSystem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! ping;
	BOOL MsgPing(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! party msg
	BOOL MsgParty(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPartyTender(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
// 	BOOL MsgPartyFNew(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
// 	BOOL MsgPartyAdd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
// 	BOOL MsgPartyDel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
// 	BOOL MsgPartyDissolve(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
// 	BOOL MsgPartyAuthority(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
// 	BOOL MsgPartyMbrMoveMap(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
// 	BOOL MsgPartyMbrRename(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
// 	BOOL MsgPartyMasterRenew(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
// 	BOOL MsgExpeditionFNew(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
// 	BOOL MsgPartyChangeOption(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	 	

	//! land msg;
	BOOL MsgLand(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! event msg
	BOOL MsgEventExp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgEventExpEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgEventItemGen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgEventItemGenEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgEventGrade(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgEventGradeEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgEventMoneyGen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgEventMoneyGenEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgEventEx(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgEventExEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgLimitEventBegin(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgLimitEventTimeReset(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgLimitEventEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgClassEvent(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! pet
    void RegistPet(DWORD GaeaId, GLPetField* pPet);
    void UnRegistPet(DWORD GaeaId);

	BOOL MsgPetGoto(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPetStop(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPetUpdateMoveState(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPetChangeActiveSkill(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPetRename(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL PetRenameFeedBack(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPetChangeColor(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPetChangeStyle(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPetAccInvenExSlot(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPetAccInvenToSlot(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPetAccSlotToInven(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPetLearnSkill(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPetSkillSlotExpansion(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPetFunnyReq(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPetRemoveSlotItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPetSkinPackItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	

	//! char msg
	BOOL MsgReady(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgActState(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgMoveState(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgGoto(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgDominateMoveTo(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgAniMoveStartCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgAniMoveEndCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgAttack(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgAttackCancel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgSetTarget(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqFieldToInven( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgReqFieldToHold(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqInvenToHold(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg); 		
	BOOL MsgReqInvenExInven(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg); 		
    BOOL MsgReqStorageExInven(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg); 		
	BOOL MsgReqInvenToSlot(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
    BOOL MsgReqSlotToInven(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqVNGainToHold(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqVNGainExHold(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqHoldToVNGain(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqVNInvenToInven(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqVNGainInvenReset(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqSlotToHold(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqItemSort(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqItemSortStart(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqItemSortData(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqItemSortEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqInvenSplit(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqInvenToField(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgReqHoldToInven(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqInvenToInven(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgReqStorageToInven(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqHoldToSlot(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqSlotChange(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqBuyFromNpc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqExchangeBuyFromNpc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqSaleToNpc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqReBirth(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgResurrectMoveMap(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgReqResurrect(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqLevelUp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);				
	BOOL MsgReqJumpingLevelUp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);				
	BOOL MsgReqJumpingLevelUpCA(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);				
	BOOL MsgReqJumpingLevelUpByExp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);				
	BOOL MsgReqStatsUp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqStatsUpEx(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqLearnSkill(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqSkillUp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);				
    BOOL MsgReqRnSkillUp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);				
	BOOL MsgReqFireCracker(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqInvenDrug(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqInvenLunchBox(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenBoxOpen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenBoxInfo(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenDisguise(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenCleanser(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenChargedItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgReqInvenRandomBoxOpen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.16, Randombox Open시 확률제어 적용 여부 요청 응답
     * modified : 
     */
	BOOL MsgReqInvenRandomBoxOpenFB(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL MsgReqInvenSelformBoxSelectOpen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqInvenDisJunction(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
    BOOL MsgReqInvenBoxWrapping(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgReqInvenBoxUnwrapping(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgInvenInitializeCountry ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg );
	BOOL MsgReqSkill(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);				
	BOOL MsgReqSkillHoldReset(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqSkillCancel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqSkillQSetActive(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqSkillQSet(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqSkillQReSet(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqActionQSet(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqActionQReSet(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqGetStorageSpecificItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqGetStorage(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqStorageDrug(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqStorageSkill(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqStorageToHold(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenExStorage(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenToStorage(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqStorageToStorage(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqStorageSaveMoney(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgReqStorageDrawMoney(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgReqStorageSplit(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqMoneyToField(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenGrinding(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenPeriodExtend(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqInvenLockBoxUnLock(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqInvenLockBoxOpen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqInvenResetSkSt(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqReGenGate(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqCure(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqFriendFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqMove2CharFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqSkillRevivel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqGetExpRecoveryNpc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgReqRecoveryNpc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqCharReset(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqCharCard(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqStorageCard(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqInvenLine(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqInvenStorageOpen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgReqInvenRemodelOpen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgReqInvenGarbageOpen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgReqInvenStorageClose(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgReqInvenPMarketSearchOpenCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgReqInvenPMarketSearchOpenAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqInvenPremiumSet(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenHairChange(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenFaceChange(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenHairStyleChange(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenHairColorChange(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenFaceStyleChange(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenGenderChange(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgReqInvenSchoolChange(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgReqInvenRename(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgInvenRename(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqPostBoxOpen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqPostBoxOpenCard(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgReqPostBoxClose(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgInvenVietnamGet(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqConFrontStart(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqConFrontEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqNpcItemTrade(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgReqNpcQuestStart(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqNpcQuestStartWithoutNPC(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqNpcQuestTalk(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqQuestGiveUp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqQuestReadReset(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqQuestComplete(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqGesture(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgLoudSpeaker(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgChatPartyRecruitFB(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgClubNew(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);				
	BOOL MsgClubRank(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);				
	BOOL MsgClubNew2FLD(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	//BOOL MsgClubMemberReq(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	//BOOL MsgClubMemberAns(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgClubMemberNick(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgClubCDCertify(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	//BOOL MsgReqClubGetStorage(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqClubStorageToInven( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );	
	BOOL MsgReqClubStorageExInven(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgReqClubInvenToStorage(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgReqClubHoldToStorageAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	//BOOL MsgReqClubStorageSplit(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgReqClubStorageSaveMoney(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgReqClubStorageSaveMoneyAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgReqClubStorageDrawMoney(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	//BOOL MsgReqClubInComeReNew(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqRebuildResult(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqRebuildMoveItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqRebuildInputMoney(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgReqGarbageResult(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	//BOOL MsgReqPhoneNumber(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	//BOOL MsgPhoneNumber(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
	BOOL MsgReqSendSMS(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);				
	BOOL MsgSendSMS(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgItemShopOpen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqItemMix(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqDialogueWindowOpen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqNpcTalkBasic(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqGetRightOfItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgGiveFood(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgGetPetFullFromDB(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPetReviveInfo(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqInvenCostumColorChange(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgVehicleInvenExSlot(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgVehicleInvenToSlot(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgVehicleSlotToInven(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgVehicleRemoveSlot(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgVehicleGiveBattery(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgGetVehicleFullFromDB(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqInvenVehicleColorChange(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqVehicleGetOff(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqLearnBikeBoost(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqUseBikeBoost(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqVietnamGainType(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgGathering(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgGatheringCancel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgChaSaveDF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

	BOOL MsgReqInvenTLGrinding(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	
    //! PointShop -------------------------------------------------------------
    BOOL MsgPointShopGmCharPointAddCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg);
    
    //! Client->Field:PointShop 아이템 구매 요청
    BOOL MsgPointShopBuyItemCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg);
    
    //! Client->Feild:PointShop 카드 충전
    BOOL MsgPointShopPointCardCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg);

    //! 재매입.
    BOOL MsgBuyRebuyItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! ReferChar
	BOOL MsgReferCharPublic(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReferCharRequest(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);


	//
	BOOL MsgMapSearchNameDetailInfo(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL MsgMapSearchNamePosition(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    // IP event
    BOOL MsgGmLoadIPEvent(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgGmSetIPEvent(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    BOOL MsgGmAcquireEXP(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! Rnattendance
	BOOL MsgReqRnattendanceQuestStart(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	// Reload AI FSM
	BOOL MsgReloadAIFSMProc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	// Country System
	BOOL MsgVCountryAdd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgVCountryReset(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL MsgGmSetNotifyMoneyAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

	BOOL MsgTransDB2WorldBattle(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgTransDB2WorldBattleCharSave(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgTransDB2WorldBattleAns(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			

	BOOL MsgGmNetworkProfileReq(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	// 경험치 압축 / 캡슐
	BOOL MsgReqExpCompressor(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);				
	BOOL MsgReqExpCapsule(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		

	// Macro;
	BOOL MsgReqMacroModeOnoffCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	BOOL MsgReqMacroModeNearestCrowInfoCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL MsgChaSaveAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgChaSaveEndAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

	BOOL MsgGMReqUserCountLandCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pMsg );

	// 란모바일
	BOOL MsgGmRanMobileCommandRequest(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgGmRanMobileCommandResponse(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    //! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
    BOOL MsgGmRandomboxNotifyItemReloadReqAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	// 캐릭터 슬롯;
	BOOL MsgReqLockCharSlotCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pNetMsg );
	BOOL MsgReqUnlockCharSlotCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pNetMsg );
	BOOL MsgReqChangeCharSlotCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pNetMsg );
	BOOL MsgReqChangeCharSlotUseItemCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pNetMsg );

	// 아이템 스킬 변경;
	BOOL MsgReqInvenChangeItemSkillCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pNetMsg );

	// 아이템 기본 능력치 변경;
	BOOL MsgReqInvenChangeBasicStatsCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pNetMsg );

	// 아이템 가산 능력치 변경;
	BOOL MsgReqInvenChangeAddStatsCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pNetMsg );

	// 스킬UI에서 스킬,스텟 초기화 요청(게임머니 돌려주지 않고, 전체 스킬, 스텟 초기화만 진행됨)
	BOOL MsgReqResetSkillStatsBySkillUI( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
private:
    sc::DefaultLock m_ItemMakeLock;
	MatchingSystem::MatchingCore* m_pMatchingCore;
public:
    bool NewItemMake( SITEMCUSTOM& NewItem, const SNATIVEID& ItemId, EMITEMGEN GenType );

public:
	BOOL ValidCheckTarget( GLLandMan* pLandMan, STARGETID &sTargetID );

	ServerActor* GetTarget( const GLLandMan* pLandMan, const STARGETID &sTargetID );
    ServerActor* GetActor(const GLLandMan* pLandMan, const Faction::GLActorID& actorID);
    ServerActor* GetActor(const GLLandManager* pLandMan, const Faction::GLActorID& actorID);
	const D3DXVECTOR3& GetTargetPos( const GLLandMan* pLandMan, const STARGETID &sTargetID );
	WORD GetTargetBodyRadius( GLLandMan* pLandMan, STARGETID &sTargetID );

	GLARoundSlot* GetARoundSlot( const STARGETID &sTargetID );

public:
	GLLandMan*	GetRootMap();
	void		SetMapState();
	GLLandMan*	const GetLand(const MapID &mapID);
	GLLandMan*	const GetLand(const SNATIVEID &mapID);
	GLLandManager* const GetLandManager(const MapID& mapID);
    GLLandManager* const GetLandManager(const SNATIVEID& mapID);
    GLLandManager* const GetLandManager(const DWORD mapID);

    // new instance system ;
    InstanceSystem::ManagerField* const GetInstanceManagerField();

	// event Manager;
	GLEvent::ManagerField* const GetEventManagerField(void);
	
	Money::CMoneyManagerField* const GetMoneyManager ( void );

	//
	//mjeon.tbb
	//	
    void	CreateAndEnterInstantMaps();	

private:
	// 기존의 동일한 mapID를 여러 인던에서 활용이 안되는 점,
	// 필드 서버에 고정이 아닌 동적으로 생성 불가등의 이유로 다른 생성 방식을 갖는다;	
    void	CreateOldInstanceMaps();
    void	CreatePVEInstanceMaps(double dTimer);
    void	CreateNewInstanceMaps(void);

    HRESULT	CreateInstantMap(const SENTRYINSTMAPINFO& createInstMapInfo);

public:	
	void	CreateInstanceMaps(void);	
	void	DeleteInstantMaps(float fElapsedTime);

	void reserveCreateInstance(const InstanceMapInformation& _instanceMapInformation);
	const bool reserveDestroyInstance(const DWORD _instanceMapID);

	//mjeon.instance
	BOOL CreateInstance(DWORD dwkey, DWORD dwMapID);
	BOOL DeleteInstance(DWORD dwKey);

public:
	void PushJackpotItem( JACKPOT_ITEM& JackpotItem );
	bool PopJackpotItem( JACKPOT_ITEM& JackpotItem );

protected:
	HRESULT InsertMap( GLLandManager* const pNewLandMan );

public:
	HRESULT Create(
        DWORD dwMaxClient,
        DxMsgServer *pMsgServer,
        db::IDbManager* pDBMan,
		db::IDbManager* pAdoMan,
        EMSERVICE_PROVIDER emServiceProvider, 
        const char* szMapList/*=NULL*/,
        DWORD dwFieldSID/*=FIELDSERVER_MAX*/,
        int nChannel/*=0*/,        
        BYTE ExcessExpProcessType/*=0*/,
        bool bToolMode);

	HRESULT Create4Level( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT CleanUp();

	void SetUpdate( BOOL bUpdate )		{ m_bUpdate = bUpdate; }

public:
	HRESULT ClearDropObj();

public:
	HRESULT OneTimeSceneInit();

public:	
	HRESULT FrameMove(float fTime, float fElapsedTime);

	//
	//mjeon.tbb
	//FrameMoveLandMan(): FrameMove for static Main Maps(not inserted or deleted during the game.)
	//	
	void	FrameMoveLandMan(float fTime, float fElapsedTime, __time64_t &CurrentTime);
	void FrameMoveInstanceMapExtra( float fElapsedTime );
	
	void	FrameMoveMiniGame(const float fTime, const float fElapsedTime);
	void	FrameMovePartyTender(float fTime, float fElapsedTime);
    void	FrameMoveClubDeathMatch(float fElapsedAppTime);
    void	FrameMoveGuidance(float fElapsedTime);
	HRESULT Render(LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME& cv);

	void	FrameMoveCTF(float fElapsedTime);

public:
	void	OnChangeMoney ( EMGAME_MONEY_CHANGE _emMoneyChange, LONGLONG _llChangeMoney );
	void	OnChangeMapMoney ( EMGAME_MONEY_CHANGE _emChangeType, int _nMapID, LONGLONG _llChangeMoney );

protected:
	//mjeon.Ping
	BOOL InsertPingLog( DWORD dwClientID, DWORD dwGaeaID, DWORD dwRTT );
	BOOL ChatMsgLinkProc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL ChatMsgLinkAFProc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

public:
	//BOOL TraceFilter(EMNET_MSG nType);	//mjeon.AF.추적용 필터 (임시)
	BOOL MsgProcess( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

public:
	void SENDTOALLCLIENT(NET_MSG_GENERIC* nmg);
    void SENDTOALLCLIENT(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);
    void SENDTOALLCLIENT(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);
	
    void SENDTOCLIENT(DWORD dwClientID, NET_MSG_GENERIC* nmg);
    void SENDTOCLIENT(DWORD dwClientID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);
    void SENDTOCLIENT(DWORD dwClientID, EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

	void SENDTOAGENT(NET_MSG_GENERIC* nmg);
    void SENDTOAGENT(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);
    void SENDTOAGENT(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

    void SENDTOCACHE(NET_MSG_GENERIC* pMsg);
    void SENDTOCACHE(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);
    void SENDTOCACHE(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

	void SENDTOINTEGRATIONCACHE( NET_MSG_GENERIC* pMsg );
	void SENDTOINTEGRATIONCACHE( EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc = false );
	void SENDTOINTEGRATIONCACHE( EMNET_MSG MsgType, const std::vector< std::tr1::shared_ptr< msgpack::sbuffer > >& vecSpBuffer, bool UseCrc = false );

    void SENDCLUBLOG(DWORD ClubDbNum, club::EM_LOG_TYPE LogType, const std::string& LogMsg);

	void SENDTOPARTYCLIENT(const GLPartyID& PartyID, const MapID& mapID, NET_MSG_GENERIC* nmg);
    void SENDTOPARTYCLIENT(const GLPartyID& PartyID, const MapID& mapID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);
	void SENDTOPARTYCLIENT(const GLPartyID& PartyID, const MapID& mapID, const D3DXVECTOR3& vReferencePosition, const float fRange, NET_MSG_GENERIC* nmg);
	void SENDTOPARTYCLIENT(const GLPartyID& PartyID, const MapID& mapID, const D3DXVECTOR3& vReferencePosition, const float fRange, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);

	void SENDTOCLUBCLIENT(DWORD dwClubID, NET_MSG_GENERIC* nmg);
    void SENDTOCLUBCLIENT(DWORD dwClubID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);
	
	void SENDTOCLIENT_ONMAP(DWORD dwMapID, NET_MSG_GENERIC* nmg);
    void SENDTOCLIENT_ONMAP(DWORD dwMapID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);

	void SENDTOCLUBCLIENT_ONMAP(DWORD dwMapID, DWORD dwClubID, NET_MSG_GENERIC* nmg);
    void SENDTOCLUBCLIENT_ONMAP(DWORD dwMapID, DWORD dwClubID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);

    //! 같은 인던에 있는 모든 유저에게 발송
	void SENDTOCLIENT_ONMAP_INSTANT(DWORD dwMapID, NET_MSG_GENERIC* nmg);
    void SENDTOCLIENT_ONMAP_INSTANT(DWORD dwMapID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);

	void SENDTOMYSELF(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);
	void SENDTOMYSELF(NET_MSG_GENERIC* nmg);

	void SendAgentQuestNpcPos();	

    void ReloadLogItem();

public:
	void CHECKVALID();

public:
    //! Point Shop
    void PointShopClear(__time64_t UpdateTime);
    void PointShopDataEnd();
    void PointShopDataInsert(const pointshop::SellItem& Item);
    void SetCharPointShopPoint(DWORD ChaDbNum, const CHARACTER_POINT& Point);

public:
    //! Club member 계급 변경
    void ClubMemberGradeChange(DWORD ClubDbNum, DWORD MemberDbNum, size_t Grade);
    //! Club 계급 이름 변경
    void ClubGradeNameChange(DWORD ClubDbNum, size_t GradeIndex, const std::string& GradeName);
    BOOL MsgClubGradeFlagChangeAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgClubPublicFlagChangeAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgClubAutoKickAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgClubNewbieNoticeAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgClubJoinTimeResetAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgClubAllianceTimeResetAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgClubNameChangeCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgClubNameChangeDF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgClubNameChangeAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

public:    
    //! 캐릭터 Action Log
	// 통계 및 로그 수정
    void LogCharAction(
        DWORD dwCI, gamelog::EMACTION emACT, 
        gamelog::EMIDTYPE emTAR, DWORD dwTAR, 
        __int64 nEXP, int nBRIGHT, int nLIFE, int nMONEY, const MapID &sMapID);

public:
    void TxtLogToDb();

    DWORD CalcDamage(
        GLChar* pChar,
        int& rResultDAMAGE,
        const DWORD dwGaeaID,
        const STARGETID &cTargetID,
        const GLLandMan* pLandMan,
        const GLSKILL* pSkill,
        const DWORD dwskill_lev,
        const DWORD dwWeatherFlag,
        const DWORD dwDivCount);

    //! 공격력 변화율 반영
    void AdjuctDamage(
        EMCROW CrowType,
        int& nDamageLow,
		int& nDamageHigh, 
        float DamageRate,
        float OwnerDamagePer);

    void AdjustLevel(int& ndxLvl, int& nExtFORCE, int TarLevel, WORD MyLevel);

    int CalcCriticalDamage(bool bCritical, bool bCrushingBlow, int Damage);

    //! Critical 발생 여부 판단
    bool CalcCritical(EMCROW CrowType, DWORD NowHp, DWORD MaxHp, int ndxLvl, int IncCritical = 0);

    //! Damage 최대 최소값 사이의 랜덤 Damage 결정
    int CalcRandomDamage(const int& nDamageLow, const int& nDamageHigh, int ExtForce);

    //! 최저 수용 Damage 산출
    int CalcLowDamage(EMCROW CrowType, int DamageOld, int Defense, float StateDamage);

    //! 방어구 Damage 흡수율 반영
    int CalcDefenseRate(EMCROW CrowType, int DefaultDefense, int ItemDefense, int Damage, WORD Level);

    //! GLMapListServer
    SMAPNODE* FindMapNode(const SNATIVEID& sNativeID);
	SMAPNODE* FindMapNode(const MapID& mapID);
    const SLEVEL_REQUIRE* GetLevelRequire(const SNATIVEID& sNativeID);
    const char* GetMapName(const SNATIVEID& sNativeID);
    GLMapList::FIELDMAP& GetMapList();
    HRESULT LoadMapList(
        BOOL bTool = FALSE,
        LPF_SERVER_INFO pServerInfo = NULL,
        int nServerNum = 0);
    void ConvertWorldToMap(const MapID& mapID, const D3DXVECTOR3& vPos, int& nPosX, int& nPosY);
    void ConvertMapToWorld(const MapID& mapID, int PosX, int PosY, D3DXVECTOR3& vPos);

	bool MoveMap( GLChar* pChar, const MapID& mapID, DWORD dwGate, const D3DXVECTOR3& vPos );

    BOOL GmDbUnlockUserAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    std::tr1::shared_ptr<GLMapAxisInfo> GetAxisInfo(const std::string& FileName);

    //! 캐릭터 이름/캐릭터 DB 번호, 캐릭터 DB 번호/캐릭터 이름
    void ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType);
	void ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType, int iClass, int iLv);

    //! 캐릭터 이름/캐릭터 DB 번호
    DWORD GetChaDbNumByChaName(const std::string& ChaName);

	    //! 캐릭터 DB 번호/캐릭터 이름
    std::string GetChaNameByDbNum(DWORD ChaDbNum);

    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> GetCharCacheData(const std::string& ChaName);
    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> GetCharCacheData(DWORD ChaDbNum);

    //! Private Market/개인상점/개인 상점
    void PrivateMarketOpen(DWORD ChaDbNum);
    void PrivateMarketClose(DWORD ChaDbNum);
    void PrivateMarketAddItem(DWORD ChaDbNum, const SSEARCHITEMDATA& Data);
    void PrivateMarketDelItem(DWORD ChaDbNum);

    BOOL MsgPMarketTitle(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
    BOOL MsgPMarketReqItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
    BOOL MsgPMarketDisItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
    BOOL MsgPMarketOpen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
    BOOL MsgPMarketClose(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);			
    BOOL MsgPMarketItemInfo(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
    BOOL MsgPMarketBuy(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL MsgPrivateMarketItemBuyCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketItemPayMoneyHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketItemPayMoneyRollbackHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketItemBuyHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketItemIsHoldHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketCommonFB( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketOptionAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

	BOOL MsgPrivateMarketSearchClose( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketItemSearchBasicCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketItemNumTotalHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketItemNumHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketItemHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketItemEndHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketItemSearchNameCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketLogCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketLogNumHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketLogHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketLogEndHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketStateCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketStateHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPrivateMarketPostSendHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

	BOOL MsgGMNetMsgDelay( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

    // 위탁판매
    BOOL MsgConsignmentSaleGetListReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
    BOOL MsgConsignmentSaleRegistReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
    BOOL MsgConsignmentSaleSendBackReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
    BOOL MsgConsignmentSaleSendBackAck( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
    BOOL MsgConsignmentSaleSendBackAckDB( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
    BOOL MsgConsignmentSalePurchaseReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgConsignmentSalePayMoney( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgConsignmentSaleSendPostAndLog( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgConsignmentSalePurchaseAckDB( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

public:
    //! Product
    BOOL MsgProductReqAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
    BOOL MsgProductLearnBookAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
    BOOL MsgProductTypeDelAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

public:
    std::string GetClientIP( DWORD dwClientID );
    BOOL MsgItemRepairReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
    BOOL MsgItemRepairCardUseReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

protected:
    //! PVE	
    BOOL MsgReqPVECreateInstantMapAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgReqPVEEntranceInstantMapAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgPVEReqPortalCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);    
    BOOL MsgPVEReqReturnAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgPVEReqRetryCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgPVEReqSavePosAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgPVEStageResultAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPVEReqExitCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPVEReqReloadTriggerAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgPVEReqGoStageCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    BOOL MsgPVEReqEntranceConfirmCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL MsgReqInvenStagePass(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgReqInvenStagePassOk( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgReqInvenEnterNumCharge( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgReqInvenEnterNumChargeOk( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

protected:
	// TexasHoldem
	BOOL MsgTexasHoldem( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

protected:
	BOOL MsgMsgTournamentAgent( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

protected:
    //! 위탁판매 ConsignmentSale, 
    BOOL MsgConsignmentSlotExpand( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

public:
	// 우편
	BOOL MsgPostRenewListReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPostRenewListAck( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPostRenewSendReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPostRenewSendAck( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPostRenewChaCheckDB( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPostRenewDelReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPostRenewDelAck( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPostRenewAttachReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPostRenewAttachAck( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPostRenewAttachAckDB( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPostRenewReadReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPostRenewReadAck( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPostRenewReturnReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL MsgPostRenewReturnAck( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

	BOOL MsgCountRecorderInfoReq(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	BOOL MsgItemDecomposeReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
public:
	//! event;
	BOOL MsgEvent(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	BOOL MsgReqInvenCostumeStats(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgReqInvenCostumeStats_Release(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		

	BOOL MsgMapSearchNameCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		
	BOOL MsgMapSearchNameCA(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);		

public:
	void SaveCharacterInSqlite( SCHARDATA2* pChadata, bool bSendAgent = true );
	void UnblockForEmergencySavedUser();

	void UserLockerLock( DWORD UserNum );
	void UserLockerUnLock( DWORD UserNum );

	void DisconnectAllCharacter();
	BOOL SendServerStopMsg( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

public:
	// 아이템 관련 처리 도중 에러가 발생해서 롤백까지 했으나 제대로 되지 않을 경우
	// 아이템 guid 와 원인을 parameter 로 넘겨준다.
	// 내부 동작은 디비에 특정 테이블에 아이템 guid, 원인, 발생 시간 이렇게 저장이 된다.
	// 예외가 발생한 아이템은 결국 디비에 있기 때문에 로그 확인 및 아이템 테이블과
	// 가칭 에러 아이템 테이블에서 guid 로 아이템을 찾을 수 있고 후 처리가 가능할 것 이다.
	// 가칭 에러 아이템 테이블에 guid 는 중복해서 들어갈 수 있도록 테이블 설계
	void InsertErrorItem( sc::MGUID& guid, int CallType );

	// db action 제작 필요 제작했다. 디비 함수 제작 및 프로시저 남았음
	// guid, 변경될 인벤타입, 좌표 필요하다.
	// sItemCustom 의 인벤타입enum INVEN_TYPE 참조
	// 아이템 세부 정보 변경없이, 단지 위치만 변경되는 경우에 사용된다. 예를 들어 창고에 있던 아이템이 인벤토리로 이동한 경우
	// 장착창에서 인벤토리로 이동한 경우, 인벤토리에서 인벤토리로 이동한 경우등
	// 디비에서 저장 위치와, 좌표값만 변경한다. 아이템 세부항목 전체 업데이트 비용을 줄이기 위한 용도이다.
	void UpdateItemInvenType( DWORD Owner, const SITEMCUSTOM& sItemCustom, INVEN_TYPE InvenType, WORD PosX, WORD PosY, DWORD Channel = 0 );

public:
	// 통합 캐시서버와의 연결이 끊어졌다가 다시 연결될 때 필드서버가 가지고 있는 개인상점 정보를 전송한다.
	BOOL MsgPrivateMarketSendPrivateMarketData( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

	BOOL MsgJackpotPush( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

public:
	BOOL ResetChaSkill( GLChar* pChar );
	BOOL ResetChaSkillNew( GLChar* pChar );

};

#endif // _GL_GAEA_SERVER_H_
