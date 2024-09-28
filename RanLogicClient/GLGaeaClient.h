#pragma once

#include <string>
#include "../SigmaCore/Json/json_spirit_value.h"

#include "../EngineLib/DxTools/Collision/CollisionDef.h"
#include "../EngineLib/G-Logic/GLActiveMap.h"
#include "../EngineLib/G-Logic/GLCrowTracer.h"

#include "../RanLogic/GLogicExDefine.h"
#include "../RanLogic/GLCDMDefine.h"
#include "../RanLogic/WorldBattle/WorldBattleDefine.h"

#include "./Land/GLMapListClient.h"
#include "../EngineLib/GUInterface/InterfaceBaseDefine.h"

#include "../RanLogic/Msg/WorldBattleMsg.h"
#include "../RanLogic/Msg/PVEMsg.h"
#include "../RanLogic/Msg/GLContrlCountry.h"
#include "../RanLogic/InstanceSystem/GLInstanceSystem.h"
#include "../RanLogic/PVE/MapTable.h"
#include "../RanLogicClient/TriggerSystem/ClientMapInfoMessage.h"

#include "../RanLogicClient/Country/GLCountryManClient.h"
#include "../RanLogicClient/Country/GLVictoriousCountryManClient.h"
#include "../RanLogicClient/LoadingThread/LoadingDirectClient.h"

struct SDAYDATA;
class PrivateMarketClient;
class ConsignmentSaleClient;
class GLGroupChat;
class EngineDeviceMan;
class GLFriendClient;
class GLPartyClient;
class GLPartyManagerClient;
class GLPartyTenderManagerClient;
class GLPostClient;
class GLSNSClient;
class GLTradeClient;
class GLBaseTutorial;
class DxGlobalStage;
class GLCharacter;
class GLClubClient;
class GLPetClient;
class GLSummonClient;
class GLLandManClient;
class GLCrowClient;
class GLCharClient;
class TaskN;
class SUBPATH;
struct SACTIONTAR;
struct GLQUEST_STEP;
class ClientActor;
enum EMSERVICE_PROVIDER;
struct GLCHARLOGIC;
class GameCommand;
struct FRIEND_GROUP;
struct GLCLUBALLIANCE;
struct GLCLUBBATTLE;
class CharNameDbNumCache;
class SPartyOption;
enum EMFRIEND_FLAGS;
class NotifyClientMan;
class PartyClientCache;
struct PARTY_CACHE_CLIENT;
class GLProductClient;
class GLPVEClient;
class RnAttendanceClient;
struct SATTENDANCEQUESTDATA;
class GLActivityClient;
class CMapReCruitPartyInfo;
class CCostumeStatClient;
class GLCTFlagEffectMan;

namespace Ping
{
	class ManagerClient;
}
namespace InstanceSystem
{
    class ManagerClient;
};
namespace MiniGame
{
	class GLManagerClient;
}
namespace GLEvent
{
	class ManagerClient;
}
namespace DamageRecord
{
	class Manager;
}
namespace clubmark
{
    struct DXDATA;
}
struct FRIEND_GROUP;

namespace pointshop
{
    class PointShopClient;
}

namespace attendance
{
    class GLAttendanceClient;
}

namespace PVP
{
    class GLCaptureTheFieldClient;
    enum ENUM_PVP_TYPE;
    enum ENUM_CTF_TYPE;
}

namespace wb 
{
    class ClubCountryClientMan;
}

namespace ReferChar
{
	class CClient;
}

namespace club
{
    class LogManClient;
    enum EM_LOG_TYPE;
	enum ALIGN_ORDER_TYPE;
	enum ALIGN_ORDER_DES_ASC;
	enum AUTHORITY;
    struct ALIGN_ORDER;
    struct LOG_DATA;
    struct GLCLUBMEMBER_CLIENT;
    struct MEMBER_ON_OFFLINE;
    struct GRADE_FLAG;
}

namespace WishList
{
	class CClient;
}

class ClientScriptManager;

namespace ReferChar
{
	class CClient;
}

namespace private_market
{
	struct sSaleItemSearchandSortName;
	struct sSaleItemSearchandSortBasic;
	struct sPurchaseQueue;
	class GLSearchDelay;
	class GLPrivateMarketPurchseQueue;
}

namespace TexasHoldem
{
	class GLTexasHoldemClient;
}

namespace LottoSystem
{
	class GLLottoSystemManClient;
}

class ClientScriptManager;

namespace MatchingSystem
{
	class GLTouranmentClient;
}

namespace ItemPreview
{
	class GLItemPreviewClient;
}

namespace Faction
{
	class ManagerClient;
}

class PostRnClient;
class CSkillCAIdx;
typedef std::tr1::shared_ptr<PostRnClient> PostRnClientPtr;

class GLGaeaClient : /*public GLMapListClient,*/ public GLCrowTracer, public GLActiveMap
{
public:
    typedef std::map<DWORD, std::tr1::shared_ptr<SDROP_CHAR> > LAND_DROP_CHAR;
    typedef LAND_DROP_CHAR::value_type                      LAND_DROP_CHAR_VALUE;
    typedef LAND_DROP_CHAR::iterator                        LAND_DROP_CHAR_ITER;
    typedef LAND_DROP_CHAR::const_iterator                  LAND_DROP_CHAR_CITER;

public:
    GLGaeaClient(
        DxGlobalStage* pGlobalStage,
        EngineDeviceMan* pEngineDevice,
        EMSERVICE_PROVIDER ServiceProvider,
        SUBPATH* pPath,
        bool bToolMode);
    virtual ~GLGaeaClient();
    //static GLGaeaClient& GetInstance();

protected:
    EngineDeviceMan* m_pEngineDevice;
    EMSERVICE_PROVIDER m_ServiceProvider;
    SUBPATH* m_pPath;

	char						m_szAppPath[MAX_PATH];
	HWND						m_hWnd;

	int							m_nServerGroup;
	int							m_nChannel;

	float						m_fAge;
	float						m_fElapsedTime;
	LPDIRECT3DDEVICEQ			m_pd3dDevice;
	GLLandManClient*			m_pLandMClient;

    GLClubClient*               m_pMyClub;
	GLCharacter*				m_pCharacter;
	GLPetClient*				m_pPet;

	VEC_SK_TAR					m_vecDetectID;
	D3DXVECTOR3					m_vCharPos;

	bool						m_bPKServer;		// PK ����

	bool						m_bSCHOOL_FREEPK;	// �п��� ���� PK
	bool						m_bBRIGHTEVENT;		// ���� �̺�Ʈ
	
	bool						m_bCLUBBATTLE;		//������ ���� ���� ���� �ִ°�?
    std::vector<float>  		m_vecClubGuidBattleTimer; //������ ���� �ð�
    std::vector<std::string>	m_vecClubGuidBattleName;  //�������� ������ �̸�

	// �δ� ��Ͽ� ���� �����ν� �ɸ��Ͱ� ���� �ϰ� �Ǹ� �����κ��� �޾ƿ�;	
	InstanceSystem::InstanceInformationMap m_instanceInformationMap;

	bool						m_bClubDeathMatch;
	float						m_fClubDeathMatchTimer;

	CTime						m_cServerTime;			// ������ ���� �ð�
	int							m_nServerTimeBias;		// ���� TimeZone ( +Hour )
	char						m_szShopInfoFile[MAX_PATH];

	DWORD						m_dwRTTAgent;
	DWORD						m_dwRTTField;

    DWORD m_MaxClientNetworkProtocolNum; //! �ִ� Client network protocol number
    DWORD m_MaxServerNetworkProtocolNum; //! �ִ� Server network protocol number

    int m_ServerGroup; // ���� ������ ���� �׷�

	bool						m_bUseCustomChaseCamera;	// ����ٴϴ� ī�޶� ���
	DWORD						m_dwChaseCameraTarget;		// ����ٴ� �༮
	STARGETID					m_sChaseCameraTarget;

    DxGlobalStage* m_pGlobalStage;
    GLFriendClient*                 m_pFriendClient;                //! Friend ģ��
	MiniGame::GLManagerClient*      m_pMiniGameManager;             //! �̴� ����;
	GLEvent::ManagerClient*				m_pEventManager;				//! �̺�Ʈ ������;
    GLPartyManagerClient*           m_pPartyManagerClient;          //! Party ��Ƽ	
	GLPartyTenderManagerClient*     m_pPartyTenderManagerClient;    //! ������ ����(��Ƽ/������);
    GLGroupChat*                    m_pChatGroup;                   //!�׷� ä��
	PrivateMarketClient*            m_pPrivateMarketSearchBuyData;  //! ���λ��� �˻��ý��� ������
    ConsignmentSaleClient*          m_pConsignmentSale;             //! ���λ��� ��Ź�Ǹ�
    GLPostClient*                   m_pPostClient;                  //! Post ����
    GLSNSClient*                    m_pSNSClient;                   //! SNS sns
    GLTradeClient*                  m_pTradeClient;                 //! Trade �ŷ�
    GLBaseTutorial*                 m_pBaseTutorial;                //! Tutorial
    pointshop::PointShopClient*     m_pPointShop;                   //! Point Shop
    attendance::GLAttendanceClient* m_pAttendance;                  //! Attendance
	GLActivityClient*				m_pActivity;					//! Activity
    PVP::GLCaptureTheFieldClient*   m_pCaptureTheField;             //! Captured The Field
	ReferChar::CClient*             m_pReferChar;
	WishList::CClient*              m_pWishList;
    GameCommand*                    m_pGameCommand;                 //! Game Command
    wb::ClubCountryClientMan*       m_pClubCountryMan;              //! World battle country/club
    wb::EM_COUNTRY                  m_UserCountry;                  //! User country info
	int								m_Use2ndPass;					//! 2������� ����ϴ��� ���� 0 : ������ 1: ����� 2: �űԼ��� �ʿ�
    club::LogManClient*             m_pClubLog;
	DamageRecord::Manager*          m_pDamageRecord;                //! Damage Log Record;
    GLProductClient*                m_pProductClient;               //hsshin ���۽ý��� Ŭ����
	GLPVEClient*                    m_pPVEClient;
	RnAttendanceClient*             m_pRnAttendanceClient;
    InstanceSystem::ManagerClient* m_pInstanceManager; // �ű� �ν��Ͻ� ���� �Ŵ���;
	ItemPreview::GLItemPreviewClient* m_pItemPreviewClient;
	GLCTFlagEffectMan*				m_pCTFlagEffect;				// ����� ����Ʈ

	Ping::ManagerClient* m_pPingManager;

	private_market::GLSearchDelay* m_pPrivateMarketSearchDelay;
	private_market::GLSearchDelay* m_pPrivateMarketLogDelay;
	private_market::GLPrivateMarketPurchseQueue* m_pPrivateMarketPurchseQueue;

	TexasHoldem::GLTexasHoldemClient* m_pTexasHoldemMgr;

    LAND_DROP_CHAR m_TempLandDropChar;
    wb::NET_REQ_DROP_CHAR_CLUB_NAME_CA m_TempClubReqSendData;

    ClientScriptManager* m_pClientScript;
    GLMapListClient* m_pMapList;
    NotifyClientMan* m_pNotify;

    CharNameDbNumCache* m_pChaDbNumNameCache; //! Character DB number and name cache

    PartyClientCache* m_pPartyCache; //! Other party info cache
	
	MatchingSystem::GLTouranmentClient* m_pTournament;

	// Country System
	Country::SCOUNTRY_INFO m_sCountryInfo;
	Country::GLCountryManClient* m_pCountryMan;
	Country::GLVictoriousCountryManClient* m_pVCountryMan;

	// Lotto System
	LottoSystem::GLLottoSystemManClient* m_pLottoSystemMan;
	BOOL				m_bGateOpen;

    // Club Name Timer
    float m_fClubNameTimer;

	PostRnClientPtr m_spPostRnClient;

	CCostumeStatClient* m_pCostumeClient;

	LoadingDirectClient* m_pLodingDirect;

public:
	CDM_RANK_VEC m_vecCdmRank;
	SCDM_RANK_EX m_sMyCdmRank;

public:
    inline DxGlobalStage* GetGlobalStage() { return m_pGlobalStage; }
    inline EngineDeviceMan* GetEngineDevice() { return m_pEngineDevice; }
    inline SUBPATH* GetSubPath() const { return m_pPath; }
    GLFriendClient* GetFriendClient();
	MiniGame::GLManagerClient* const GetMiniGameManager(void) const;
	GLEvent::ManagerClient* const GetEventManager(void);
	InstanceSystem::ManagerClient* const GetInstanceManager(void) const;
	GLPartyManagerClient* const GetPartyManager(void);
	GLPartyTenderManagerClient* GetPartyTenderManager(void);
    GLPartyClient* const GetMyPartyClient(void);
	GLGroupChat* GetChatGroup();
	PrivateMarketClient* GetPrivateMarketClient();
    ConsignmentSaleClient* GetConsignmentSale();
    GLPostClient* GetPostClient();
    GLSNSClient* GetSNSClient();
    GLTradeClient* GetTradeClient();
    GLBaseTutorial* GetTutorial();
    pointshop::PointShopClient* GetPointShopClient();
	const DamageRecord::Manager* const GetDamageRecord(void) const;
	ReferChar::CClient* GetReferChar() { return m_pReferChar; }
	WishList::CClient* GetWishList() { return m_pWishList; }
    inline EMSERVICE_PROVIDER GetServiceProvider() const { return m_ServiceProvider; }
    GLClubClient* GetMyClub() { return m_pMyClub; }
    NotifyClientMan* GetNotifyClientMan() { return m_pNotify; }
    GLProductClient* GetProductClient() { return m_pProductClient; } //hsshin
	RnAttendanceClient* GetRnAttendanceClient() { return m_pRnAttendanceClient; }
	TexasHoldem::GLTexasHoldemClient* GetTexasHoldemManager() { return m_pTexasHoldemMgr; }
	PostRnClientPtr GetPostRenewClient() { return m_spPostRnClient; }
	GLActivityClient* GetActivityClient() { return m_pActivity; }
	ItemPreview::GLItemPreviewClient* GetItemPreviewClient() { return m_pItemPreviewClient; }
	GLCTFlagEffectMan* GetCTFlagEffectMan() { return m_pCTFlagEffect; }

	CCostumeStatClient*	GetCostumeClient() { return m_pCostumeClient; }

	Faction::ManagerClient* GetFactionClient ();
	LottoSystem::GLLottoSystemManClient* GetLottoSystemMan () { return m_pLottoSystemMan; }
	
	bool IsPKServer()			{ return m_bPKServer; }
	bool IsSchoolFreePk ()		{ return m_bSCHOOL_FREEPK; }
	bool IsPKTime()		{ return (m_bPKServer || m_bSCHOOL_FREEPK || m_bClubDeathMatch || m_bCLUBBATTLE || m_bBRIGHTEVENT); }
	bool IsBRIGHTEVENT ()		{ return m_bBRIGHTEVENT; }
	bool IsClubDeathMatch()		{ return m_bClubDeathMatch; }
	bool IsClubBattle()			{ return m_bCLUBBATTLE; }

    bool CanRecoveryExp();
    bool CanRecoveryExpNpc();

    //! ���� ������ ���� �׷�
    //! �ʱ���� -1
    int GetServerGroup() const { return m_ServerGroup; }
    void SetServerGroup(int ServerGroup);

	void SetConnServerGroup( int nServerGroup ) { m_nServerGroup = nServerGroup; }
	void SetConnChannel( int nChannel ) { m_nChannel = nChannel; }

	int GetConnServerGroup() { return m_nServerGroup; }
	int GetConnChannel() { return m_nChannel; }

    BOOL IsEmulator() const;

	BOOL GetUsedMaterialSystem() const;

	Country::GLCountry*				GetClientCountry () const;
	inline Country::SCOUNTRY_INFO	GetCountryInfo () const { return m_sCountryInfo; }
	void							SetCountryInfo ( Country::SCOUNTRY_INFO sCountryInfo );

	bool IsSelectCountry();
	DWORD GetSelectedCountryID();
	std::string GetSelectedCountryName();

	bool IsCharPrison();

public:
	HRESULT OneTimeSceneInit ( const char* szAppPath, HWND hWnd );
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ();

	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

	HRESULT FrameMove ( float fTime, float fElapsedTime, const CLIPVOLUME &sCV, BOOL bNotRendering );
	
	HRESULT Render ( CLIPVOLUME &CV, LPDIRECT3DSURFACEQ pColorBuffer );
	HRESULT RenderSoftAlpha ( CLIPVOLUME &CV, LPDIRECT3DSURFACEQ pColorBuffer );
	HRESULT RenderSingleEff ( CLIPVOLUME &CV );
	//void	RenderPickAlpha();

	HRESULT RenderStarGlow();
    HRESULT Render_LightShafts();

	BOOL	IsInstantMap();
	BOOL	IsTutorialMap();// by luxes.

	const bool	IsNewInstanceMap ( void );
	const bool	IsOldInstanceMap ( void );

protected:
	GLLandManClient* CreateLandMClient ( const MapID& mapID, D3DXVECTOR3 &vBasicPos );

protected:
	HRESULT MoveActiveMap(const MapID& mapID, D3DXVECTOR3 &vPos );
	HRESULT CreateInstantMap(const MapID& mapID, D3DXVECTOR3 &vBasicPos );

public:
//	HRESULT SetActiveMap ( char* szLandFileName, D3DXVECTOR3 &vBasicPos );
	HRESULT SetActiveMap ( const MapID& mapID, D3DXVECTOR3 &vBasicPos );
	
    GLLandManClient* GetActiveMap();
    GLLandManClient* GetLandManClient();

	const MapID GetActiveMapID(void) const;

	void ShowLogFactionInfo ();

	//! ���� ���� ����;
	void JoinWorldBattleServer ();

    //! ��������
    void FirstJoin();
    void SendProtocolInfo();
    void MsgProtocolInfo(NET_MSG_GENERIC* pMsg);
    void PrintProtocolInfo();
	void NotifyReferCharPublic();

	void CloseConnect();

	// Country;
	void ChangeCountry ( Country::SCOUNTRY_INFO sCountryInfo );
	void MsgVCountryAddFAC ( NET_MSG_GENERIC* pMsg );
	void MsgVCountryResetFAC ( NET_MSG_GENERIC* pMsg );

	//! Set Country Mark;
	void MsgNameDisplayShowCountryMark ( NET_MSG_GENERIC* pMsg);

	void MsgMoveActiveMap ( NET_MSG_GENERIC *nmg );
	void ReqCreateInstantMap ( NET_MSG_GENERIC *nmg );
	void ReqCreateTutorialMap ( NET_MSG_GENERIC *nmg ); // by luxes.
	void DropOutCrow ( NET_MSG_GENERIC *nmg );
    void MsgResurrect(NET_MSG_GENERIC* nmg);
    void MsgResurrectInfo(NET_MSG_GENERIC* nmg);
    void MsgResurrectFB(NET_MSG_GENERIC* nmg);
	void MoveMapResultAFB(NET_MSG_GENERIC* nmg);
	void ReLvMustLeaveFB(NET_MSG_GENERIC* nmg);
	void ReqBusFB(NET_MSG_GENERIC* nmg);
	void ReqTaxiFB(NET_MSG_GENERIC* nmg);
	void TeleportFB(NET_MSG_GENERIC* nmg);

	void MsgInstanceSystem(NET_MSG_GENERIC* nmg);
    void MsgInstanceSystemMsgPack(NET_MSG_GENERIC* nmg);
    void MsgInstanceSystemContents(NET_MSG_GENERIC* nmg);
    void MsgInstanceSystemContentsMsgPack(NET_MSG_GENERIC* nmg);

	void MsgInstanceSystemProc ( NET_MSG_GENERIC* pMsg );

    void MsgFactionSystem(NET_MSG_GENERIC* nmg);

	void MsgPing(NET_MSG_GENERIC* nmg);

	void MsgChangeJoinWorldBattleState(NET_MSG_GENERIC* nmg);
	void MsgTransToWorldBattleAns(NET_MSG_GENERIC* nmg);
	void MsgTransToWorldBattleDBAns(NET_MSG_GENERIC* nmg);
	
	// �������
	void MsgGmRanMobileCommandResponse(NET_MSG_GENERIC* nmg);

	// Land User Count 
	void MsgGmResponseUserCountLandFC ( NET_MSG_GENERIC* _pMsg );

	// ĳ���� ����;
	void MsgResponseLockCharSlotFC ( NET_MSG_GENERIC* _pNetMsg );
	void MsgResponseUnlockCharSlotFC ( NET_MSG_GENERIC* _pNetMsg );
	void MsgResponseChangeCharSlotFC ( NET_MSG_GENERIC* _pNetMsg );

public:
	HRESULT ReqClubDeathMatchInfo();
	void ReqClubDeathMatchOnOff(DWORD CDMId, int nStateFlag);

public:
	void clearInstanceInformation(void);
	void addInstanceInformation(const InstanceSystem::InstanceInformationClient& _instanceInformation);
	void showInstanceType(void);
	const InstanceSystem::InstanceInformationClient* const getInstanceInformation(const SNATIVEID& _keyMapID) const;

public:
	const EMCROWCOLORTYPE GetCrowIdentifyColorType ( const STARGETID& sTargetID );
    const EMCROWCOLORTYPE GetCrowIdentifyColorTypeNonFaction(const STARGETID& sTargetID);
	DWORD GetMobNameColor ( DWORD dwGlobID );
	BOOL IsMapCollsion( D3DXVECTOR3& vTargetPt, D3DXVECTOR3& vFromPt, EMCOLLMODE emCullMode );

public:
	ClientActor* GetCopyActor ( const STARGETID &sTARID )	{ return GetCopyActor ( sTARID.emCrow, sTARID.GaeaId ); }
	ClientActor* GetCopyActor ( const EMCROW emCrow, const DWORD dwID );
	ClientActor* GetCopyActor ( const std::string &strName );
	GLCharacter* GetCharacter();
	const D3DXVECTOR3 &GetCharacterPos();
	SCHARDATA2& GetCharacterData();
	GLCHARLOGIC& GetCharacterLogic();

	VOID AllCharClientUpdateSuit();


	std::tr1::shared_ptr<GLCharClient> GetChar(DWORD dwID);
	std::tr1::shared_ptr<GLCharClient> GetCharByDBnum(DWORD dwID);

	BOOL ValidCheckTarget ( const STARGETID & sTargetID );
	VEC_SK_TAR* DetectCrowDie ( const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTarget );
	VEC_SK_TAR* DetectCrow ( DWORD emCrow, const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTarget );
	VEC_SK_TAR* DetectCrow ( DWORD emCrow, const D3DXVECTOR3 &vTarPos, const float fLength );
	VEC_SK_TAR* DetectCrowAll ( DWORD emCrow, const D3DXVECTOR3 &vTarPos, const float fLength );
	VEC_SK_TAR* DetectReactionableCrowDie ( const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTarget, BOOL bEnemy );
	VEC_SK_TAR* DetectReactionableCrow ( DWORD emCrow, const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTarget, BOOL bEnemy );
	VEC_SK_TAR* DetectCrowWithCylinder( DWORD emCrow, const D3DXVECTOR3 &vFromPt, const D3DXVECTOR3 &vTargetPt, float fRadius );
	BOOL IsCollisionMobToPoint ( const D3DXVECTOR3 &vPoint, const WORD wBodyRadius );
	const bool IsCollision(const D3DXVECTOR3& vPoint0, const D3DXVECTOR3& vPoint1, EMCOLLMODE emCullMode) const;

	D3DXVECTOR3* FindCharPos (const std::string& strName );
	D3DXVECTOR3* FindCharHeadPos (const std::string& strName );
	D3DXVECTOR3* FindCharHeadPos ( DWORD dwGaeaID );
	D3DXVECTOR3* FindMaterialHeadPos ( DWORD dwGaeaID );

	const CTime& GetCurrentTime() const { return m_cServerTime; }
	void SetCurrentTime(__time64_t tServerTime) { m_cServerTime = tServerTime; }
	void GetConvertServerTime( CTime& cTime );
	void SetServerTimeBias( int nTimeBias )	{ m_nServerTimeBias = nTimeBias; }

	// PET
	GLPetClient* GetPetClient () { return m_pPet; }
	HRESULT		 CreatePET(NET_MSG_GENERIC* nmg);
	void		 PetSkinPackApplyEffect();

	// SUMMON
	HRESULT		    CreateSummon(NET_MSG_GENERIC* nmg);
	void		    ReqUseSummon( SNATIVEID sSummonID );
    std::tr1::shared_ptr<GLSummonClient> GetSummon(DWORD dwID);

	void ReqSummonEventSkill ( UINT iIdx );

	// Reload AI FSM
	void ReloadAIFSM();

	// Change Basic Hire Summon
	void ChangeBasicHireSummon( WORD wMainID, WORD wSubID );

	// Show Faction Information
	void ShowFactionInformation ( );

	// Show Land UserCount
	void ShowLandUserCount ( Faction::EMFACTION_TYPE _emFactionType, int _nInstanceMapID );

	// Character Slot
	void LockCharSlot ( const WORD _wCharSlotNum );
	void UnlockCharSlot ( const WORD _wCharSlotNum );
	void ChangeCharSlot ( const WORD _wCharSlotNum );

	// QUEST
	// ũ�ο� ID�� ���� NPC�� ã�Ƽ� questNpc ���¸� ��ȭ��Ų��.
	std::tr1::shared_ptr<GLCrowClient> GETQUSETNPC(const GLQUEST_STEP& step);
	// �Է��� STEP�� �ٸ� ����Ʈ���� ��� ���� ���
	std::vector<DWORD> CHECKQUESTNPCSTATE(std::tr1::shared_ptr<GLCrowClient> spCrow, const GLQUEST_STEP *pStep = NULL, const GLQUESTPROG* pProg = NULL, const INT iStepNumber = 0);

	//! DamageRecord;	
	void turnOnDamageRecord(const STARGETID& ActorID); // DPS ���ͱ� ��� �˸�;	
	void logDamageRecord(const STARGETID& ActorID, const DWORD nDamage, const SNATIVEID& SkillID = SNATIVEID(false), const WORD wSkillLevel = 0, const DWORD dwDamageFlag = 0);	 // ������ ���;
	void addActorDamageRecord(const STARGETID& ActorID); // ������ ��� ��� ���;	
	void dropActorDamageRecord(const DWORD nIndex); // ������ ��� ��� ����;	
	void resetActorDamageRecord(const DWORD nIndex); // ������ ��� ����;
	void saveCSVActorDamageRecord(void); // ������ ��� ����;

public:
	// ���콺 ������ Ÿ�������� ������� �ϴ��� ����
	const bool IsVisibleToTargetInfoMouseOver( const SACTIONTAR &sCursorTar );
    //! ������ Log �� ���� ������ ������ reload �϶�� ����Ѵ�.
    void LogItemGmReload();
    //! UserDB, LogDB �� ���� ���� ������(nType �� ���� ������, ����Ʈ, ��ų) ������Ʈ �Ѵ�.
    void ServerGameDataUpdate( int nType );

public:
    //! class GLActiveMap
	virtual DxLandMan* getactivemap() override;

public:
    //! class GLCrowTracer
	virtual BOOL IsVisibleCV(const STARGETID & sTargetID) override;
	virtual D3DXVECTOR3 GetTargetPos(const STARGETID &sTargetID) override;
	virtual DxSkinChar* GetSkinChar(const STARGETID &sTargetID) override;
	virtual float GetCrowDir(const STARGETID &sTargetID) override;
	virtual D3DXMATRIX GetTargetTransMatrix(const STARGETID &sTargetID) override;
	ClientActor*	GetCrow( const EMCROW emCrow, const DWORD dwGaeaId );
	CSkillCAIdx* GetTargetCA ( const STARGETID &sTargetID );

    // Interface
public:
    //! NS_UITEXTCOLOR::DEFAULT
    void PrintMsgText(D3DCOLOR dwColor, const std::string& strText);
    void PrintConsoleText(const std::string& strText);
    void PrintMsgTextDlg(D3DCOLOR dwColor, const std::string& strText );
    void PrintConsoleTextDlg(const std::string& strText);
    void PrintMsgDlg(D3DCOLOR dwColor, const std::string& strText);
	void PrintAddString(D3DCOLOR dwColor, const std::string& strText);
	void PrintMessageBox(DWORD dwType, const std::string& strText);

    void DoModalOutInterface(
        const std::string& strText,
        int nModalTitle = UI::MODAL_NOTITLE,
        int nModalType = UI::OK,
        UIGUID CallerID = NO_ID,
        BOOL bHide = FALSE);
    
    bool ExecuteCommand(const std::string& Command);

    // Network
public:
    void NETSENDTOAGENT(NET_MSG_GENERIC* pMsg);
    void NETSENDTOAGENT(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);

    void NETSENDTOFIELD(NET_MSG_GENERIC* pMsg);
    void NETSENDTOFIELD(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);

	void NETSENDTOMYSELF(NET_MSG_GENERIC* pMsg);
	void NETSENDTOMYSELF(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);

	DWORD GETMYDBNUM();
    DWORD GETMYGAEAID();
    DWORD GETMYCLIENTID();

public:
    //! -----------------------------------------------------------------------
    //! Point Shop
    void PointShopGmReload();
    void PointShopGmView();
    void SendPointShopUpdateDate();
    //! ������ ���۵ɶ� ȣ���
    void PointShopCacheLoad();    
    void PointShopAddPoint(__int64 AddPoint);
    //! Point Shop ���� Item ����
    bool PointShopBuyItem(const SNATIVEID& ItemId);
    
    //! Point Shop ���� ���Ű����� Character Point
    __int64 PointShopChaPoint() const;

    //! Point Card ������û
    void ReqInvenDrugPointCard(WORD PosX, WORD PosY);

    //! Field->Client:Character Point ������
    void MsgPointShopAddPointFC(NET_MSG_GENERIC* pMsg);

    void MsgPointShopDataStartAC(NET_MSG_GENERIC* pMsg);
    void MsgPointShopDataAC(NET_MSG_GENERIC* pMsg);
    void MsgPointShopEndAC(NET_MSG_GENERIC* pMsg);
    void MsgPointShopCharPointAC(NET_MSG_GENERIC* pMsg);
    void MsgPointShopBuyItemCF(NET_MSG_GENERIC* pMsg);

public:
    //! -----------------------------------------------------------------------
    //! Attendance
    attendance::GLAttendanceClient* GetAttendance() { return m_pAttendance; }
    void        AttendanceUIReload();
    void        AttendanceUILoad();
    std::string AttendanceUIGetStringCaption_1 ( TaskN* pTask );
    std::string AttendanceUIGetStringCaption_2 ( TaskN* pTask );
    std::string AttendanceUIGetStringProgress  ( TaskN* pTask );
    const UINT  AttendanceGetProgressDays ();
    const UINT  AttendanceGetItemTakeAbleCount ();

    void MsgAttendanceSync(NET_MSG_GENERIC* pMsg);
    void MsgAttendanceReset(NET_MSG_GENERIC* pMsg);
    void MsgAttendanceUpgrade(NET_MSG_GENERIC* pMsg);
    void MsgAttendanceTaskUpdate(NET_MSG_GENERIC* pMsg);
    void MsgAttendanceTaskReset(NET_MSG_GENERIC* pMsg);
    void MsgAttendanceTaskCompleted(NET_MSG_GENERIC* pMsg);
    void MsgAttendancePopup(NET_MSG_GENERIC* pMsg);
    void MsgAttendanceTakeReward(NET_MSG_GENERIC* pMsg);
    void MsgAttendanceDonePointshopItem(NET_MSG_GENERIC* pMsg);
	void MsgAttendanceLoadEnd(NET_MSG_GENERIC* pMsg);

    void ReqAttendanceTakeReward (const UINT nTaskIndex);
    void ReqAttendanceDonePointshopItem(const UINT nTaskIndex);
	
	//! Rnattendance
	std::vector<SATTENDANCEQUESTDATA>GetAttendanceQuestData();
	void GetAttendanceCompleted();
	void MsgRnattendanceProcess(NET_MSG_GENERIC* pMsg);
	int GetAttendanceCount();
	void SetAttendanceQuestData();
	void ClearAttendanceQuestIdServerToClient();
	int GetDayCount();
	void SetSelectDay(int nDayofWeek, 
					  int nDays, 
					  int nMonth, 
					  int nYear,
					  int nAnyDay);
	SDAYDATA GetSelectDay();

	void MsgPremiumMapExit( NET_MSG_GENERIC* pMsg );

public:
	void MsgLottoSystem( NET_MSG_GENERIC* pMsg );

	// Country
	bool LoadCountryFile ();

public:
    //! -----------------------------------------------------------------------
    //! Captured The Field
    PVP::GLCaptureTheFieldClient* GetCaptureTheField() { return m_pCaptureTheField; }

    const bool IsCTFPlayer();
    const bool IsCTFMap();
	const bool IsCTFMap_PVP ( void ) const;
	const bool IsCTFMap_PVE ( void ) const;

    void MsgCaptureTheFieldReady(NET_MSG_GENERIC* pMsg);
    void MsgCaptureTheFieldJoinFB(NET_MSG_GENERIC* pMsg);
    void MsgCaptureTheFieldUpdateQueue(NET_MSG_GENERIC* pMsg);
    void MsgCaptureTheFieldWarning4Drop(NET_MSG_GENERIC* pMsg);
    void MsgCaptureTheFieldStartToAll(NET_MSG_GENERIC* pMsg);
	void MsgCaptureTheFieldStartToPlayers(NET_MSG_GENERIC* pMsg);
	void MsgCaptureTheFieldDoneToAll(NET_MSG_GENERIC* pMsg);
    void MsgCaptureTheFieldReward(NET_MSG_GENERIC* pMsg);
    void MsgCaptureTheFieldStop(NET_MSG_GENERIC* pMsg);
    void MsgCaptureTheFieldAuthed(NET_MSG_GENERIC* pMsg);
    void MsgCaptureTheFieldSyncStatus(NET_MSG_GENERIC* pMsg);
	void MsgCaptureTheFieldUpdatePlayersPosition(NET_MSG_GENERIC* pMsg);
	void MsgCaptureTheFieldStatistics(NET_MSG_GENERIC* pMsg);
	void MsgCaptureTheFieldRewardToAll(NET_MSG_GENERIC* pMsg);
	void MsgCaptureTheFieldRankingToAll(NET_MSG_GENERIC* pMsg);    
    
    void ReqCaptureTheFieldJoin(const PVP::ENUM_CTF_TYPE emCTFType);
    void ReqCaptureTheFieldCancelJoin(const PVP::ENUM_CTF_TYPE emCTFType);
    void ReqCaptureTheFieldQuit();

public:
	//! -----------------------------------------------------------------------
	//! ControlType
	void KeySettingLoad();

public:
    // Product-----------------------------------------------------------------
    void MsgProductProcess(NET_MSG_GENERIC* pMsg);

public:
    //! �ѱ� �˴ٿ� Shutdown
    void MsgKorShutdown(NET_MSG_GENERIC* pMsg);

public:
    //! -----------------------------------------------------------------------
    //! World battle

    //! ������ ����/Club���� ������ ��û�Ѵ�
    void ReqClubCountryInfo();
    //! ������ ����/Club ������ ��û�Ѵ�
    void ReqClubCountryInfo(wb::EM_COUNTRY emCountry);
    void MsgClubCountryCountStartAC(NET_MSG_GENERIC* pMsg);
    void MsgClubCountryCountAC(NET_MSG_GENERIC* pMsg);
    void MsgClubCountryCountEndAC(NET_MSG_GENERIC* pMsg);
    void MsgClubCountryDataAC(const json_spirit::mObject& Obj);
    //! �ش� ������ Club������ �޾���
    void MsgClubCountryDataEndAC(const json_spirit::mObject& Obj);
    //! Club ĳ���� ���� ����
    //void MsgClubChaLevelChangeAC(NET_MSG_GENERIC* pMsg);

    //! ���� ������ Club �⺻������ �����´�.
    void SearchClubCountry(const std::string& SearchStr, const std::vector<wb::search::OrderBy>& Order, std::vector<wb::CLUB_INFO_BASE>& vInfo);

    //! ���������� �˻��Ѵ�
    void wbSearch(const std::string& SearchWord, std::vector<wb::COUNTRY_INFO>& Result);
    //! ������ ��ü������ �����´�
    void wbSearch(std::vector<wb::COUNTRY_INFO>& Result);
    bool wbIsExistCountry(wb::EM_COUNTRY CountryCode) const;

    //! ���� ������ �����´�
    const wb::COUNTRY_INFO& wbGetData(wb::EM_COUNTRY CountryCode) const;

    //! ���� �̹���
    const std::string& wbGetFlagTexture(wb::EM_COUNTRY CountryCode) const;
    
    //! ���� �̹���. ���� ��/����
    WORD wbGetFlagWidth() const;
    WORD wbGetFlagHeight() const;

    //! ���� �̹���. ���� ��ġ
    const wb::FLAG_TEXTURE_POSITION& wbGetFlagTexturePos(wb::EM_COUNTRY CountryCode) const;

    //! ����/������ Club����
    void wbSetCountryInfo(wb::EM_COUNTRY CountryCode, WORD ClubCount);
    
    //! User country info
    wb::EM_COUNTRY GetUserCountry() const { return m_UserCountry; }
    void SetUserCountry(wb::EM_COUNTRY Country) { m_UserCountry=Country; }

	void SetUse2ndPass(int nUse2ndPass) { m_Use2ndPass = nUse2ndPass; }
	int GetUse2ndPass() const { return m_Use2ndPass; }
    
    //! ���������û
    void ChangeUserCountry(wb::EM_COUNTRY NewCountry);

    //! �������� ��û ���oiu	`
    void MsgUserChangeCountryAC(NET_MSG_GENERIC* pMsg);

	//! �¸��� Ŭ���� ���⸦ �ʿ� �����Ѵ�.
	//! ���� ������ ���� 4���� �ִ�.
	//! �� ������ ���� ID(GuidanceID) �� �¸��� Ŭ���� ���⸦ �־��ָ� �ȴ�.
	//! ����� 4���� �����Ƿ� 4�� ȣ���� �ʿ��ϴ�.
	//void SetFlagWinnerClubCountry( DWORD dwGuidanceID,  wb::EM_COUNTRY emCountry );

	// ������ ���� ID �� ���� �ڵ尡 �Ѿ� �´�. SetFlagWinnerClubCountry ȣ����.
	void MsgWbCbSetFlagFACBRD(NET_MSG_GENERIC* pMsg);

    void ReqGuidanceInfo();
    void ReqGuidanceChange(DWORD GuidId, DWORD ClubDbNum);
	void ReqGuidanceOnOff(DWORD GuidId, int nStateFlag);

	void ToggleJoinWorldBattleButton(bool bOn);

public:
    struct GLGaeaClientPacketFunc
    {        
        boost::function<void (NET_MSG_GENERIC* pMsg)> Msgfunc;
        GLGaeaClientPacketFunc()
        {
            Msgfunc = NULL;
        }
    };
    GLGaeaClientPacketFunc m_GaeaClientMsgFunc[NET_MSG_GCTRL_MAX];

    void initPacketFunc();

    void MsgProcess(NET_MSG_GENERIC* pMsg);
    //! Json message 2012-02-16 jgkim
    void MsgJson(NET_MSG_GENERIC* pMsg);
    bool MsgJsonParser(const std::string& JsonStr);

private:
    struct TEMP_PACKET
    {
        BYTE Buffer[NET_DATA_BUFSIZE];
        void SetData(void* pBuffer, size_t Size)
        {
            memcpy(Buffer, pBuffer, Size);
        }
    };

    std::vector<TEMP_PACKET> m_TempPacket;

public:
    void MsgProcess2(NET_MSG_GENERIC* pMsg);
    void MsgClubCommissionRvBrd(NET_MSG_GENERIC* pMsg);
    void MsgClubCommissionBrd(NET_MSG_GENERIC* pMsg);
    void MsgServerClubDeathMatchInfo(NET_MSG_GENERIC* pMsg);
    void MsgServerClubGuidBattleInfo(NET_MSG_GENERIC* pMsg);
    void MsgServerBrightEventInfo(NET_MSG_GENERIC* pMsg);
    void MsgServerInfo(NET_MSG_GENERIC* pMsg);
    void MsgServerPkEventInfo(NET_MSG_GENERIC* pMsg);
    
    //������
    void MsgClubGuidBattleStartBrd(NET_MSG_GENERIC* pMsg);  //������ ����
    void MsgClubGuidBattleRemainBrd(NET_MSG_GENERIC* pMsg); //������ ������� ���� �ð�
    void MsgClubGuidBattleEndBrd(NET_MSG_GENERIC* pMsg);    //������ ����
    
    void MsgClubCdCertyfingBrd(NET_MSG_GENERIC* pMsg);
    void MsgClubCdCertifyBrd(NET_MSG_GENERIC* pMsg);
    void MsgClubDeathMatchRankingUpdate(NET_MSG_GENERIC* pMsg);
    void MsgClubDeathMatchMyRankUpdate(NET_MSG_GENERIC* pMsg);
    void MsgClubDeathMatchPointUpdate(NET_MSG_GENERIC* pMsg);
    void MsgClubDeathMatchRemainBrd(NET_MSG_GENERIC* pMsg);
    void MsgClubDeathMatchEndBrd(NET_MSG_GENERIC* pMsg);
    void MsgclubDeathMatchStartBrd(NET_MSG_GENERIC* pMsg);
    void MsgClubMarkInfoFb(NET_MSG_GENERIC* pMsg);

    void MsgGmViewWorkEventFb(NET_MSG_GENERIC* pMsg);
    void MsgGmViewAllPlayerFldFb(NET_MSG_GENERIC* pMsg);
    void MsgGmShopInfoFb(NET_MSG_GENERIC* pMsg);
    void MsgGmFreePkBrd(NET_MSG_GENERIC* pMsg);
    void MsgGmBigHandBrd(NET_MSG_GENERIC* pMsg);
    void MsgGmBigHeadBrd(NET_MSG_GENERIC* pMsg);
    void MsgGmEventGradeEndFb(NET_MSG_GENERIC* pMsg);
    void MsgGmEventItemGenEndFb(NET_MSG_GENERIC* pMsg);
    void MsgGmEventGradeFb(NET_MSG_GENERIC* pMsg);
    void MsgGmEventItemGenFb(NET_MSG_GENERIC* pMsg);
    void MsgGmEventExpEnd(NET_MSG_GENERIC* pMsg);
    void MsgCyberCafeClassUpdate(NET_MSG_GENERIC* pMsg);

    void MsgLimitTimeOver(NET_MSG_GENERIC* pMsg);
    void MsgDuplicateCloseGame(NET_MSG_GENERIC* pMsg);
    void MsgEtnryFailed(NET_MSG_GENERIC* pMsg);
    void MsgGmWhisperStateFb(NET_MSG_GENERIC* pMsg);
    void MsgGmNonConfrontModeViewFb(NET_MSG_GENERIC* pMsg);
    void MsgGmNonConfrontMode(NET_MSG_GENERIC* pMsg);
    void MsgChangeNameMap(NET_MSG_GENERIC* pMsg);
    void MsgGmWhereNpcFb(NET_MSG_GENERIC* pMsg);
    void MsgGmWhereNpcAllFb(NET_MSG_GENERIC* pMsg);
    void MsgGmPrintCrowListFb(NET_MSG_GENERIC* pMsg);
    void MsgGmWherePcMapFb(NET_MSG_GENERIC* pMsg);
    void MsgGmWherePcPosFb(NET_MSG_GENERIC* pMsg);
    void MsgGmMoveToCharFb(NET_MSG_GENERIC* pMsg);
    void MsgGmCheckInstantNumFb(NET_MSG_GENERIC* pMsg);
    void MsgGmChatBlockFb(NET_MSG_GENERIC* pMsg);
	void MsgGmChatBlockNotify(NET_MSG_GENERIC* pMsg);
    void MsgGmCharInfoAgtFb(NET_MSG_GENERIC* pMsg);
    
    void MsgUserCharInfoAgtFb(NET_MSG_GENERIC* pMsg);
    void MsgUserCharInfoFldFb(NET_MSG_GENERIC* pMsg);
    void MsgGmWarningBrd(NET_MSG_GENERIC* pMsg);
    void MsgInstantMapReEntryCheckFb(NET_MSG_GENERIC* pMsg);
    void MsgGmCountDownBrd(NET_MSG_GENERIC* pMsg);
    void MsgServerStopFb(NET_MSG_GENERIC* pMsg);
    void MsgServicerTimeBrd(NET_MSG_GENERIC* pMsg);
    
    void MsgPetReqUseCardFb(NET_MSG_GENERIC* pMsg);
    void MsgPetSkinPackOpenFb(NET_MSG_GENERIC* pMsg);
    void MsgPetReqUnUseCardFb(NET_MSG_GENERIC* pMsg);
    void MsgPetReqGiveFoodFb(NET_MSG_GENERIC* pMsg);
    void MsgPetReqPetChardInfoFb(NET_MSG_GENERIC* pMsg);
    void MsgPetReqPetRevivedInfoFb(NET_MSG_GENERIC* pMsg);
    void MsgPetReqReviveFb(NET_MSG_GENERIC* pMsg);
    void MsgPetNotEnoughInven(NET_MSG_GENERIC* pMsg);
    void MyPetMsgProcess(NET_MSG_GENERIC* pMsg);
    void MsgPetDropPet(NET_MSG_GENERIC* pMsg);
    
    void MsgCreateAnyPet(NET_MSG_GENERIC* pMsg);
    void PetMsgProcess(NET_MSG_GENERIC* pMsg);
    void MsgReqUseSummonFb(NET_MSG_GENERIC* pMsg);
	void MsgReqHireSummonFC(NET_MSG_GENERIC* pMsg);
    void SummonMsgProcess(NET_MSG_GENERIC* pMsg);
    void MsgDropSummon(NET_MSG_GENERIC* pMsg);
	void MsgFailedDropSummon(NET_MSG_GENERIC* pMsg);
    void MsgLandInfo(NET_MSG_GENERIC* pMsg);
    void MsgGmClassEvent(NET_MSG_GENERIC* pMsg);
    void CrowMsgProcess(NET_MSG_GENERIC* pMsg);
    void ActorMsgProcess(NET_MSG_GENERIC* pMsg);
    void MyCharMsgProcess(NET_MSG_GENERIC* pMsg);
    void CharMsgProcess(NET_MSG_GENERIC* pMsg);
	
	void MsgAutoLevelList(NET_MSG_GENERIC* pMsg);
	void MsgAutoLevelResult(NET_MSG_GENERIC* pMsg);
	void MsgAutoLevelInformation(NET_MSG_GENERIC* pMsg);

	void MsgMobLevList(NET_MSG_GENERIC* pMsg);
	void MsgMapLayerNewNotify(NET_MSG_GENERIC* pMsg);
	void MsgMapLayerDelNotify(NET_MSG_GENERIC* pMsg);
	void MsgMapLayerErrorNotify(NET_MSG_GENERIC* pMsg);
	void MsgLand(NET_MSG_GENERIC* pMsg);
	void MsgGMWhySoSeriousFB(NET_MSG_GENERIC* pMsg);
    
    void MsgDropMaterial(NET_MSG_GENERIC* pMsg); 
    void MsgDropCrow(NET_MSG_GENERIC* pMsg); 
    void MsgDropPc_Base(NET_MSG_GENERIC* pMsg); 
    void MsgDropPc_StateBlow_SkillFact(NET_MSG_GENERIC* pMsg);
    void MsgDropPc_PutOnItems(NET_MSG_GENERIC* pMsg);
    void MsgDropPc_End(NET_MSG_GENERIC* pMsg);
    void MsgUpdateDropCharClubNameAC(NET_MSG_GENERIC* pMsg);
    void MsgDropMoney(NET_MSG_GENERIC* pMsg);
    void MsgDropItem(NET_MSG_GENERIC* pMsg);
    
    void MsgMapWhimsical(NET_MSG_GENERIC* pMsg);
    void MsgWhimsical(NET_MSG_GENERIC* pMsg);
    void MsgMapWeather(NET_MSG_GENERIC* pMsg); 
    void MsgWeather(NET_MSG_GENERIC* pMsg);
    void MsgPeriod(NET_MSG_GENERIC* pMsg);
    
    void MsgPostOpenTakeFb(NET_MSG_GENERIC* pMsg);
    void MsgPostDeleteFb(NET_MSG_GENERIC* pMsg);
    void MsgPostReturnFb(NET_MSG_GENERIC* pMsg);
    void MsgPostSendFb(NET_MSG_GENERIC* pMsg);
    void MsgPostReqChaNumFb(NET_MSG_GENERIC* pMsg);
    void MsgPostGetStateFb(NET_MSG_GENERIC* pMsg);
    void MsgPostGetSendStateFb(NET_MSG_GENERIC* pMsg);
    void MsgPostGetRecvStateFb(NET_MSG_GENERIC* pMsg);
    void MsgPostGetDataFb(NET_MSG_GENERIC* pMsg);
    void MsgPostGetInfoFb(NET_MSG_GENERIC* pMsg);
    
    void MsgReceivePostNotify(NET_MSG_GENERIC* pMsg);
    
    void MsgSnsTwUpdateBr(NET_MSG_GENERIC* pMsg);
    void MsgSnsTwGetUidFb(NET_MSG_GENERIC* pMsg);
    void MsgSnsTwUnAuthedFb(NET_MSG_GENERIC* pMsg);
    void MsgSnsTwAuthedFb(NET_MSG_GENERIC* pMsg);
    void MsgSnsUpdateBrFb(NET_MSG_GENERIC* pMsg);
    void MsgSnsFriendAdd(NET_MSG_GENERIC* pMsg);
    void MsgSnsUnAuthFb(NET_MSG_GENERIC* pMsg);
    void MsgSnsAuthFb(NET_MSG_GENERIC* pMsg);
    void MsgSnsAuthInfoMsg(NET_MSG_GENERIC* pMsg);
    void LandEventMsgProcess(NET_MSG_GENERIC* pMsg);
    void MsgGmCharInfoFldFb(NET_MSG_GENERIC* pMsg);
    void MsgActivitySync(NET_MSG_GENERIC* pMsg);

	void MsgCountRecorderInfoFB(NET_MSG_GENERIC* pMsg);

	void MsgCharValidation(NET_MSG_GENERIC* pMsg);

    //mjeon
    //Process Ping Msg
    void	ProcessPingMsg(NET_MSG_GENERIC* nmg);
    void	ProcessPingTraceMsg(NET_MSG_GENERIC* nmg);
	void	ProcessAttendanceMaintenanceFB(NET_MSG_GENERIC* nmg);
	void	ProcessCTFMoveStateByGMFB(NET_MSG_GENERIC* nmg);

	void MsgReferChar( NET_MSG_GENERIC* nmg );

		
	void MsgMapSearchNameAck1(NET_MSG_GENERIC* nmg);
	void MsgMapSearchNameAck2(NET_MSG_GENERIC* nmg);


	void MsgMapSearchNameDetailInfo(NET_MSG_GENERIC* nmg);
	void MsgMapSearchNamePosition(NET_MSG_GENERIC* nmg);

    //! Party -----------------------------------------------------------------
	void MsgMapPartyInfoListBoard_1(NET_MSG_GENERIC* nmg);
	void MsgMapPartyInfoListBoard_2(NET_MSG_GENERIC* nmg);

	void MsgMapPartyInfoDetail_1(NET_MSG_GENERIC* nmg);
	void MsgMapPartyInfoDetail_2(NET_MSG_GENERIC* nmg);

    std::tr1::shared_ptr<PARTY_CACHE_CLIENT> GetOtherPartyInfo(const GLPartyID& PartyID);
    void PartyInfoReq(const GLPartyID& PartyID);

    void MsgPartyInfoAC(NET_MSG_GENERIC* pMsg);

    LONGLONG CharacterInvenMoney() const;

	void MyCSStatsMsgProcess(NET_MSG_GENERIC* pMsg);
	void MsgDrugFbProcess(NET_MSG_GENERIC* pMsg);
	void MyCSStatsReleaseMsgProcess(NET_MSG_GENERIC* pMsg);

	//! Script
public:
    void ScriptStop();
    void ScriptResume();
    void ScriptDestroy();
    void ScriptFrameMove(float fTime, float ElapsedTime);
    void ScriptExec(const std::string& FileName);
    void ScriptExec(const std::wstring& FileName);

    //! Map List/Map Node
    SMAPNODE* FindMapNode(const SNATIVEID& sNativeID);
    const char* GetMapName(const SNATIVEID& sNativeID);	
    std::string GetMapNameStr(const SNATIVEID& MapID) const;
    HRESULT LoadMapList(
        BOOL bTool=FALSE,
        F_SERVER_INFO* pServerInfo=NULL,
        int nServerNum=0);
    GLMapList::FIELDMAP& GetMapList();
    std::tr1::shared_ptr<GLMapAxisInfo> GetAxisInfo(const std::string& FileName);
    void ConvertWorldToMap(const MapID& mapID, const D3DXVECTOR3& vPos, int& nPosX, int& nPosY);
    void ConvertMapToWorld(const MapID& mapID, int PosX, int PosY, D3DXVECTOR3& vPos);

	//! -----------------------------------------------------------------------
    //! Mini Game OddEven
	void MsgMiniGame(NET_MSG_GENERIC* pMsg);

	//! -----------------------------------------------------------------------
	//! Event-Message;
	void MsgEvent(NET_MSG_GENERIC* _pMessage);
    
    void MsgCharOnlineAC(NET_MSG_GENERIC* pMsg);
    void MsgCharOfflineAC(NET_MSG_GENERIC* pMsg);
    void MsgCharPositionAC(NET_MSG_GENERIC* pMsg);
    void MsgOtherCharPartyChangeAC(NET_MSG_GENERIC* pMsg);
	
	//! -----------------------------------------------------------------------
	//! Party
	void MsgParty(NET_MSG_GENERIC* pMsg);	
	void MsgPartyTender(NET_MSG_GENERIC* pMsg);	

    //! -----------------------------------------------------------------------
    //! Club

    //! Club ���� ����
    size_t GetClubAuthDescSize() const;

    //! Club ���� ���� ���ڿ�
    std::string GetClubAuthDesc(size_t Index) const;

    //! Club ��� ����
    size_t GetClubGradeSize() const;

    //! Club ��� ���� ���ڿ�
    std::string GetClubGradeName(size_t Index) const;

	DWORD GetClubMasterDBNum(void) const;
	std::string GetClubMasterName();
    //! Club Member ��� ����
    bool SetClubMemberGrade(DWORD MemberDbNum, size_t Index);

    void MsgClubAuthFC(NET_MSG_GENERIC* pMsg);

protected:
    //! Club �α׸� ��û�Ѵ�.
    //! EM_LOG_TYPE �α�����
    //! Page ���ϴ� ������
    //! PageSize ���ϴ� �������� data ����
    void ReqClubLog(club::EM_LOG_TYPE LogType, WORD Page, WORD PageSize);
public:
    bool GetClubLog(club::EM_LOG_TYPE LogType, WORD Page, WORD PageSize, std::vector<club::LOG_DATA>& Result);
    void ClubLogPrint(WORD Page, WORD PageSize);

    //! Club log
    void MsgClubLogAC(NET_MSG_GENERIC* pMsg);

    //! Club log
    void MsgClubLogAc_End(NET_MSG_GENERIC* pMsg);

    //! Club member list 
    //! bool bOnline : online �� ����� ������ ���ΰ�>
    void GetClubMember(
        std::vector<club::GLCLUBMEMBER_CLIENT>& vResult,
        const std::vector<club::ALIGN_ORDER>& Align,
        bool bOnline);

    //! Club member online/offline ����
    void GetClubMemberOnOffline(club::MEMBER_ON_OFFLINE& Result) const;

    //! Club member Ż���Ŵ
    bool ReqClubMemberDel(DWORD MemberDbNum);

    //! Agent->Client Club member grade change
    void MsgClubMemberGradeChangeAC2(NET_MSG_GENERIC* pMsg);

    //! Client->Agent Club member ������ CD �������� �ο�
    //! bCD true : cd ���������� �ο��Ѵ�.
    //!     false : cd ���������� �����´�. Master�� ����
    void ReqClubMemberCd(DWORD MemberDbNum, bool bCD);
    void MsgClubCdCertifierChangeAC(NET_MSG_GENERIC* pMsg);

    //! Club master ���� ��û
    bool ReqClubMasterChange(DWORD MemberDbNum);
    void MsgClubMasterChangeAC(NET_MSG_GENERIC* pMsg);

    //! Ŭ�� ���Կ�û, ���ӳ�����
    bool ReqClubJoin(DWORD dwGaeaID);
	bool ReqClubJoinByCharname(const char* szCharName);

    //! Ŭ�� ���Կ�û, Lobby ����
    void JoinClubLobby(DWORD ClubDbNum, DWORD ChaDbNum, const std::string& ChaName, EMCHARCLASS ChaClass);
    void ClubMemberReqFB(NET_MSG_GENERIC* pMsg);

    //! Ŭ�� ���� ��û Lobby
    void MakeNewClubLobby(DWORD ChaDbNum, const std::string& ClubName, const std::string& ChaName);

    //! Ŭ�� ���� ���� ��û
    bool ReqClubNotice(const std::string& ClubNotice);

    void MsgClubNewFB(NET_MSG_GENERIC* nmg);
    //void MsgClubMemberState(NET_MSG_GENERIC* nmg);
    void MsgClubInfo(NET_MSG_GENERIC* nmg);
    HRESULT ReqClubMarkInfo(DWORD dwClubID, DWORD dwMarkVer);
    void MsgClubDel(NET_MSG_GENERIC* nmg);
    void MsgClubInfoDissolution(NET_MSG_GENERIC* nmg);
    void MsgClubMember(NET_MSG_GENERIC* nmg);
    void MsgClubMemberReqAsk(NET_MSG_GENERIC* nmg);
    
    void MsgClubRank(NET_MSG_GENERIC* nmg);
    void MsgClubRankFB(NET_MSG_GENERIC* nmg);
    
    //void MsgClubMemberPos(NET_MSG_GENERIC* nmg);
    void MsgClubDissolutionFB(NET_MSG_GENERIC* nmg);
    void MsgClubMemberDel(NET_MSG_GENERIC* nmg);
    void MsgClubMemberDelFB(NET_MSG_GENERIC* nmg);
    void MsgClubMemberSecedeFB(NET_MSG_GENERIC* nmg);
    void MsgClubAuthorityReqAsk(NET_MSG_GENERIC* nmg);
    void MsgClubAuthorityReqFB(NET_MSG_GENERIC* nmg);
    void MsgClubAutority(NET_MSG_GENERIC* nmg);
    void MsgClubCD_CertifyFB(NET_MSG_GENERIC* nmg);
    void MsgClubCommissionFB(NET_MSG_GENERIC* nmg);
    void MsgClubStorageReset(NET_MSG_GENERIC* nmg);
    void MsgClubStorageSendEnd(NET_MSG_GENERIC* nmg);
    
    void MsgClubGetStorageItem(NET_MSG_GENERIC* nmg);
    void MsgClubStorageInsert(NET_MSG_GENERIC* nmg);
    void MsgClubStorageDelete(NET_MSG_GENERIC* nmg);
    void MsgClubStorageDelIns(NET_MSG_GENERIC* nmg);
    void MsgClubStorageUpdateItem(NET_MSG_GENERIC* nmg);
    void MsgClubNoticeFB(NET_MSG_GENERIC* nmg);
    void MsgClubNotice(NET_MSG_GENERIC* nmg);
    void MsgClubMemberRename(NET_MSG_GENERIC* nmg);
    void MsgClubSubMasterFb(NET_MSG_GENERIC* nmg);

    void MsgClubAllianceDis(NET_MSG_GENERIC* nmg);
    void MsgClubAllianceDel(NET_MSG_GENERIC* nmg);
    void MsgClubAllianceAdd(NET_MSG_GENERIC* nmg);
    void MsgClubAllianceReqAsk(NET_MSG_GENERIC* nmg);
    void MsgClubAllianceDisFB(NET_MSG_GENERIC* nmg);
    void MsgClubAllianceSecFB(NET_MSG_GENERIC* nmg);
    void MsgClubAllianceDelFB(NET_MSG_GENERIC* nmg);    
    void MsgClubAllianceReqFB(NET_MSG_GENERIC* nmg);

    void MsgClubBattlePointUpdate(NET_MSG_GENERIC* nmg);
    void MsgClubBattleKillUpdate(NET_MSG_GENERIC* nmg); 
        
    void MsgClubBattleArmisticeReqFB(NET_MSG_GENERIC* nmg); 
    void MsgClubBattleOver(NET_MSG_GENERIC* nmg); 
    void MsgClubBattleBegin(NET_MSG_GENERIC* nmg);
    void MsgClubBattleBegin2(NET_MSG_GENERIC* nmg);
    void MsgClubBattleReqAsk(NET_MSG_GENERIC* nmg); 
    void MsgClubBattleReqFB(NET_MSG_GENERIC* nmg); 

    void MsgClubAllianceBattleSubmissionReqFB(NET_MSG_GENERIC* nmg); 
    
    void MsgClubAllianceBattleReqAsk(NET_MSG_GENERIC* nmg); 
    void MsgClubAllianceBattleReqFB(NET_MSG_GENERIC* nmg);

	void MsgClubStorageUseFB( NET_MSG_GENERIC* pMsg );
	void MsgClubStorageMoneyUseFB( NET_MSG_GENERIC* pMsg );

    //! Ŭ�� ������ ���� ���� �亯
    void ReqClubAuthorityAns(bool bOK);

    //! CD ������û
    void ReqCDCertify(DWORD dwNpcGlobID);

    //! Club Rank up ��û/��ũ��, Ŭ��
    void ReqClubRank(DWORD dwNpcGlobID);

    //! Ŭ�� ���� �ۼ� ��¥
    std::string GetClubNoticeDate() const;

    //! Ŭ�� ���� �ۼ� ĳ����
    std::string GetClubNoticeChaName() const;

    //! Ŭ�� ����/Club notice
    std::string GetClubNotice() const;

    //! Ŭ�� â����
    std::string GetClubCreateDate() const;

    //! Ŭ�� ��ũ
    std::string GetClubRank() const;

    //! Ŭ�� Ư��Ȱ�� ����
    DWORD GetClubActivityPoint() const;

    //! Ŭ�� ������ġ ���� ����-ų-����/ club death match
    void GetClubDeathMatchRecord(DWORD& Ranking, DWORD& Kill, DWORD& Death) const;

    //! Ŭ�� ��Ʋ ���� ��-��-��/ club battle
    void GetClubBattleRecord(DWORD& Win, DWORD& Draw, DWORD& Lost) const;

    //! Ŭ�� World battle ����, ����
    void GetClubWorldBattleRecord(DWORD& Point, DWORD& Rank) const;

    //! Ŭ�� ����Ŭ�� ��
    void GetClubGuidanceMap(std::vector<std::string>& vMap);

	//! ���� Ŭ���� ����
	bool IsChief() const;

	// ���� Ŭ���� ID ���� [12/11/2012 gbgim]
	const DWORD GetChiefID() const;

    //! ���� Ŭ�� ����
    void GetClubAlliance(std::vector<GLCLUBALLIANCE>& vData) const;

    //! Ŭ�� ���� ���� ��û.
	bool ReqClubAllianceDel_Targetname(const char* szClubName);
    bool ReqClubAllianceDel(DWORD dwClubID);

	//! Ŭ�� �������� �ƴ� ��� Ż�� ��û.
	bool ReqClubAllianceWithdraw(DWORD dwClubID);

    //! Ŭ�� ���� Ż�� ��û.
    bool ReqClubAllianceSec();

    //! Ŭ�� ��Ʋ/club battle
    void GetClubBattleData(std::vector<GLCLUBBATTLE>& vData) const;

    //! Ŭ�� ���� ������Ʈ ��û
    //bool ReqClubInfoUpdate(bool bUpdate);

    //! Ŭ�� ���� ��ü ��û.
    bool ReqClubAllianceDis();

    //! Ŭ�� ��Ʋ ��û.
    bool ReqClubBattle(DWORD dwGaeaID, DWORD dwTime);
	bool ReqClubBattle( const std::string& strCharName, WORD wHour, WORD wMin );

    //! Ŭ�� ��Ʋ ��û �亯.
    bool ReqClubBattleAns(DWORD dwChiefCharID, bool bOK);

	//! Ŭ�� ��Ʋ ���� ��û CA
	bool ReqClubBattleArmistice_TargetName(const char* szClubName);

    //! Ŭ�� ��Ʋ ���� ��û Client->Agent
    bool ReqClubBattleArmistice(DWORD dwCLUBID);

    //! Ŭ�� ��Ʋ ������û Agent->Client
    void MsgClubBattleArmisticeReqAsk(NET_MSG_GENERIC* nmg); 

    //! Ŭ�� ��Ʋ ���� �亯 Client->Agent
    bool ReqClubBattleArmisticeAns(DWORD dwCLUBID, DWORD ReqChaDbNum, bool bOK);

	//! Ŭ�� ��Ʋ �׺� ��û Client->Agent
    bool ReqClubBattleSubmission_TargetName(const char* szClubName);
    
	//! Ŭ�� ��Ʋ �׺� ��û Client->Agent
    bool ReqClubBattleSubmission(DWORD dwCLUBID);

    //! Ŭ�� ��Ʋ �׺� Agent->Client
    void MsgClubBattleSubmissionReqFB(NET_MSG_GENERIC* nmg); 

    //! Ŭ�� ���� ��Ʋ ��û.
    bool ReqAllianceBattle(DWORD dwGaeaID, DWORD dwTime);
	bool ReqAllianceBattle( const std::string& strCharName, WORD wHour, WORD wMin );

    //! Ŭ�� ���� ��Ʋ ��û �亯.
    bool ReqAllianceBattleAns(DWORD dwChiefCharID, bool bOK);

    //! Ŭ�� ���� ��Ʋ ���� ��û
    bool ReqAllianceBattleArmistice(DWORD dwCLUBID);

    //! Ŭ�� ���� ��Ʋ ���� �亯 Client->Agent
    bool ReqAllianceBattleArmisticeAns(DWORD dwCLUBID, DWORD ReqChaDbNum, bool bOK);

    //! Ŭ�� ���� ��Ʋ ���� ��û
    void MsgClubAllianceBattleArmisticeReqAsk(NET_MSG_GENERIC* nmg); 
    
    void MsgClubAllianceBattleArmisticeReqFB(NET_MSG_GENERIC* nmg); 

    //! Ŭ�� ���� ��Ʋ �׺� ��û Client->Agent
    bool ReqAllianceBattleSubmission(DWORD dwCLUBID);

    //! Ŭ�� ����(����) ��û.
	bool ReqClubAlliance_PToPTarget();
    bool ReqClubAlliance(DWORD dwGaeaID);

    //! Ŭ�� ���� ��û �亯.
    bool ReqClubAllianceAns(DWORD dwChiefCharID, bool bOK);    
    //! Ŭ�� ��� ���� ���� ��û
    bool ReqClubNick(const char* szNickName);
    void MsgClubMemberNickFB(NET_MSG_GENERIC* nmg);

    std::string GetClubNickName() const;

    //! Ŭ����ũ
    const clubmark::DXDATA& GetClubMark(DWORD ClubDbNum, DWORD MarkVersion);

    //! Ŭ�� ��� �̸� ���� ��û Client->Agent
    bool SetClubGradeName(size_t Index, const std::string& GradeName);

    //! Ŭ�� ��� �̸� ���� ��û ��� Agent->Client
    void MsgClubGradeNameChangeAC_Result(NET_MSG_GENERIC* pMsg);
    void MsgClubGradeNameChangeAC(NET_MSG_GENERIC* pMsg);

    //! Ŭ�� ����
    DWORD ClubGradeFlagGet(size_t Index) const;

    //! Ŭ�� ���� ���� ��û
    bool ClubGradeFlagChangeCA(const std::vector<club::GRADE_FLAG>& Data);
    void MsgClubGradeFlagChangeAC(NET_MSG_GENERIC* pMsg);

    //! Ŭ�� ���� ���� ����, club::EM_PUBLIC_TYPE ����
    bool ClubPublicTypeReq(DWORD PublicFlag);
    void MsgClubPublicFlagChangeAC(NET_MSG_GENERIC* nmg);

    //! Ŭ�� �ڵ� Ż�� ���� ����
    bool ClubAutoKickReq(bool AutoKickUse, size_t GradeIndex, WORD Day);
    void MsgClubAutoKickAC(NET_MSG_GENERIC* nmg);

    //! Ŭ�� Ż�� ��û
    bool ReqClubSecede();

    //! �� Ŭ�� ��ũ�� ���� ��û.
    bool ReqClubMarkChange(const char* szFileName);
    void MsgClubMarkChangeFB(NET_MSG_GENERIC* nmg);
    void MsgClubMarkChange(NET_MSG_GENERIC* nmg);

    //! ���Ի� �Ұ���
    std::string ClubNewbieNotice() const;
    bool ClubNewbieNoticeReq(const std::string& Notice);
    void MsgClubNewbieNoticeAC(NET_MSG_GENERIC* nmg);

    //! Ŭ�� â��
    //! Ŭ�� â�� ������ ������ ��û.
    bool ReqGetClubStorage();

    //! Ŭ�� â�� ������ �鶧, ������, ��ȯ�Ҷ�, ��ĥ��.
    //bool ReqClubStorageTo(DWORD dwChannel, WORD wPosX, WORD wPosY);

    bool ClubStorageCheck( DWORD dwChannel );

    //! ��� �ִ� �������� Ŭ��â�� �ִ� �����۰� ��ȯ
    bool ReqClubItemExchangeToHoldItem(DWORD dwChannel, WORD wPosX, WORD wPosY);
    
    //! Ŭ��â�� �ִ� �������� �տ� �鶧
    bool ReqClubItemToHoldItem(DWORD dwChannel, WORD wPosX, WORD wPosY);

    //! ����ִ� �������� Ŭ��â�� ������
    bool ReqInvenItemToClub(DWORD dwChannel, WORD wPosX, WORD wPosY);

    //! Ŭ�� â�� ��ħ ������ �и�.
    //bool ReqClubStorageSplit(DWORD dwChannel, WORD wPosX, WORD wPosY, WORD wSplitNum);
    //bool IsClubStorageSplitItem(DWORD dwChannel, WORD wPosX, WORD wPosY);

    LONGLONG ClubStorageMoney() const;
    LONGLONG ClubIncomeMoney() const;

    void MsgClubStorageUpdateMoney(NET_MSG_GENERIC* nmg);

	//! Ŭ���� ���� ���� ��û.
    bool ClubIncomeMoneyToStorageMoney();   

    //! Ŭ�� â�� �� �ֱ�.
    bool ReqClubStorageSaveMoney(LONGLONG lnMoney);

    //! Ŭ�� â�� �� ������.
    bool ReqClubStorageDrawMoney(LONGLONG lnMoney);

    bool IsKEEP_CLUB_STORAGE(DWORD dwCHANNEL);

    //! Ŭ�� �ػ�
    bool ReqClubDissolution();

    //! Ŭ�� �ػ� ���
    bool ReqClubDissolutionCancel();

    //! Ŭ�� ���� ��û�� ���� �����ϱ�.
    bool ReqClubJoinAns(DWORD dwMaster, bool bOK);

    //! Ŭ�� �������� �ð� ����
    void ResetClubRejoinTime(const std::string& ChaName);
    void MsgClubJoinTimeResetAC(NET_MSG_GENERIC* pMsg);
    
    //! Ŭ�� ���� ���� ���� �ð� ����
    void ResetClubAllianceTime(const std::string& ClubName);
    void MsgClubAllianceTimeResetAC(NET_MSG_GENERIC* pMsg);

	void MsgClubId2MasterNameAnsAC(NET_MSG_GENERIC* pMsg);

    //! Ŭ���̸� ��û
    void ClubNameReq();

	// �ٸ� ĳ���� Ŭ�� �̸�
	std::string GetOhterCharClubName( const char* szCharName );
	
	void MsgClubRename( NET_MSG_GENERIC* nmg );
	void MsgClubRenameAlliance( NET_MSG_GENERIC* nmg );
	void PrintClubRenameFB( int nResult );
	void MsgClubRenameFB( NET_MSG_GENERIC* nmg );

    //! -----------------------------------------------------------------------
    //! Friend Group
    bool FriendAddGroup(const FRIEND_GROUP& Info);
    bool FriendDelGroup(WORD GroupNum);
    
    bool FriendIsExistGroup(WORD GroupNum) const;
    bool FriendIsExistGroup(const std::string& GroupName) const;
    WORD FriendGetFreeGroupNum() const;
    std::string FriendGetGroupName(WORD GroupNum) const;
    void FriendGroup(std::vector<FRIEND_GROUP>& GroupInfo) const;

    //! �ű� ģ�� �׷� ���� ��û/new friend group
    bool FriendGroupNewCA(const std::string& GroupName);
    void MsgFriendGroupNewAC(NET_MSG_GENERIC* pMsg);
    
    //! ģ�� �׷� ���� ��û/delete friend group
    bool FriendGroupDelReq(WORD GroupNum);
    void MsgFriendGroupDelAC(NET_MSG_GENERIC* pMsg);

    //! ģ���� �׷쿡 ���� ��û
    bool FriendGroupSetFriend(DWORD FriendDbNum, WORD Group);
    void MsgFriendSetGroupAC(NET_MSG_GENERIC* pMsg);

    //! ģ�� �׷� �̸� ���� ��û    
    bool FriendRenameGroup(WORD GroupNum, const std::string& NewName);
    void MsgFriendGroupNameChangeAC(NET_MSG_GENERIC* pMsg);
    
    void ReqFriendGroup();
    void ReqFriendList();
    void MsgFriendGroupReqAC(NET_MSG_GENERIC* pMsg);
    void MsgOtherCharLevelChangeAC(NET_MSG_GENERIC* pMsg);
    void MsgFriendMapChangeAC(NET_MSG_GENERIC* pMsg);

    //! ģ�� ���¸� �����´�.
    //! �˼� �������� ������ ��û�ϰ� �ȴ�. ���ϰ� false
    //! MainChaDbNum ĳ���Ͱ� SubChaDbNum �� � �����ΰ�?
    bool GetFriendState(DWORD MainChaDbNum, DWORD SubChaDbNum, EMFRIEND_FLAGS& State);
    void MsgFriendStateReqAC(NET_MSG_GENERIC* pMsg);
    
    //! Game Notify/�˸� ------------------------------------------------------
    void ReqNotifyCA();
    void NotifyClear();
    void MsgNotifyDataAc(NET_MSG_GENERIC* pMsg);
    void MsgNotifyDataAcEnd(NET_MSG_GENERIC* pMsg);

    //! Game Command/GM Command -----------------------------------------------
    void DbUnlockUser(DWORD UserDbNum);

    //! ĳ���� �̸����� ĳ���� DB ��ȣ�� ��û
    //! ��û�ϱ� ���� client �� cache �Ǿ� �ִ��� Ȯ���� ������.
    //! FindChaNameAtCache �Լ��� �̿��ϸ� �˴ϴ�.
    //! return �Ǵ� ���� ��û ���� ��ȣ�̴�.
    //! 0 �� ���ϵǸ� ����
    //! ��û�Ҷ� �׻� ���ϵǴ� ��û��ȣ�� ����� �ξ��ٰ� 
    //! ������ ������ �� ��ȣ�� ��û�� �ĺ��ؾ� �Ѵ�.
    DWORD ReqChaDbNum(const std::string& ChaName);
    void MsgCharDbNumReqAC(NET_MSG_GENERIC* pMsg);

    void SaveCrowListToFile();
    void MsgPutonUpdateBrd(NET_MSG_GENERIC* pMsg);

    //! Character db number and name cache
    void AddCharacterCache(DWORD ChaDbNum, const std::string& ChaName);
    DWORD FindChaDbNumAtCache(const std::string& ChaName);
    std::string FindChaNameAtCache(DWORD ChaDbNum);
    void DisplayChaChche();

    void ReqGuidanceMapName();
    void MsgGuidanceMapInfoAC(NET_MSG_GENERIC* pMsg);
    void MsgGmGuidanceInfoAC(NET_MSG_GENERIC* pMsg);

	void MsgCatchDelay( NET_MSG_GENERIC* pMsg );

	//! -----------------------------------------------------------------------
	//! ���λ���
	
	//! �⺻�������� �˻���û
	HRESULT ReqPrivateMarketItemSearch( private_market::sSaleItemSearchandSortBasic* pBasic );

	//! ������ �̸����� �˻���û
	HRESULT ReqPrivateMarketItemSearchByName( private_market::sSaleItemSearchandSortName* pName );

	void MsgPrivateMarketItemNumTotalFC( NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketItemNumFC( NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketItemFC( NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketItemEndFC( NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketCommonFB( NET_MSG_GENERIC* pMsg );
	void PrivateMarketError( int nFeedback, UUID* pItemGUID = NULL );
	void PrivateMarketItemStateText( const char* szItemStateText, UUID pItemGUID );

	HRESULT ReqPrivateMarketItemBuy( std::vector< private_market::sPurchaseQueue >& vecPurchase );
	HRESULT ProcessPrivateMarketBuyQueue();
	HRESULT ReqPrivateMarketLog();

	void MsgPrivateMarketLogNumFC( NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketLogFC( NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketLogEndFC( NET_MSG_GENERIC* pMsg );

	void MsgPrivateMarketStateFC( NET_MSG_GENERIC* pMsg );

    //! ���λ��� ����Ʈ �ŷ�
    void MsgConSignmentSaleListAck( NET_MSG_GENERIC* pMsg );
    void MsgConSignmentSaleRegistAck( NET_MSG_GENERIC* pMsg );
    void MsgConSignmentSaleSendBackAck( NET_MSG_GENERIC* pMsg );
    void MsgConSignmentSalePurchaseAck( NET_MSG_GENERIC* pMsg );

    void MsgConSignmentSaleSlotExpirationDate( NET_MSG_GENERIC* pMsg );

   

	void PrivateMarketDelayLoad();
	DWORD GetRemainSearchLimitTime();

public:
    // ������ ����
    void MsgItemRepairAck( NET_MSG_GENERIC* pMsg );
    void MsgItemRepairCardUse( NET_MSG_GENERIC* pMsg );

    // PVE
	GLPVEClient* GetPVEClient() { return m_pPVEClient; }
    void ReqMapTable();
    void ReqPVEEntrance(const DWORD dwCharDbNum, const DWORD dwMapID, const DWORD dwGateID, const int nLevel);
    void ReqPVEEntranceConfirm(bool bOk);
    void ReqPVERetry(bool bOk);
    void ReqPVEExit();
    void ReqPVEClearOfBelonging(DWORD dwCharDbNum, DWORD dwMapID, int nLevel);
    void GetPVEStageResult(GLMSG::StageResult& results, int& nStage, bool& bLast, int& nTimer);
    bool GetPVEEntranceMapInfo(TriggerSystem::SClientTriggerMapInfo*& info);
	void PVERebirth(DWORD dwGaeaID);
	void SendTriggerMessage(TriggerSystem::SMessage* msg);
	int GetThisStage();
	
	// New Instance System;
	void ReqNewInstanceRetry ( bool bOk, InstanceSystem::InstanceMapID nInstanceID, SNATIVEID sKeyMapID );

    // Char State
    void MsgCharState(NET_MSG_GENERIC* pMsg);

    void AckAcquireEXP(NET_MSG_GENERIC* pMsg);
    void MsgAcquireEXP(NET_MSG_GENERIC* pMsg);

	// Tournament
	void MsgTournamentClient(NET_MSG_GENERIC* pMsg);
	void MsgPackTournamentClient(NET_MSG_GENERIC* pMsg);
	void MsgMatchingProc(NET_MSG_GENERIC* pMsg);

	MatchingSystem::GLTouranmentClient* GetTournamentClient() { return m_pTournament; }
	void SetChaseCam(const STARGETID& sTarget, bool isChase)	{	m_bUseCustomChaseCamera = isChase;	m_sChaseCameraTarget = sTarget;	}
	// void SetFlyCameraChaseCam(const STARGETID& sTarget);
	// void SetChaseCam(const STARGETID& sTarget, bool isChase)	{	SetFlyCameraChaseCam(sTarget);	}

	// TexasHoldem
	void MsgTexasHoldem( NET_MSG_GENERIC* pMsg );

	// FlyCameraControl;
	void MsgCameraControlFB(NET_MSG_GENERIC* pMsg);

	void MsgInvenItemDecomposeFb(NET_MSG_GENERIC* pMsg);

	// Apply Skill Effect
	void MsgApplySkillEffectFC ( NET_MSG_GENERIC* pMsg );

	void MsgNpcExchangeItem(NET_MSG_GENERIC* pMsg);

	void FlyCameraControlOnOff( const bool bFlyCameraOn = false );
	float GetFlyCameraControl_MoveVelo() const;
	float GetFlyCameraControl_RotateVelo() const;
	void SetFlyCameraControl_MoveVelo( float fVelo ) const;
	void SetFlyCameraControl_RotateVelo( float fVelo ) const;
	void SetFlyCameraControl_MoveInterTime( float fInterTime ) const;
	void SetFlyCameraControl_RotateInterTime( float fInterTime ) const;
	void SetFlyCameraControl_TargetInterTime( float fInterTime ) const;

	void SetFlyCameraPos( const D3DXVECTOR3& vPos );

	void MsgMapSearchName( NET_MSG_GENERIC* pMsg );

	// Level
	void MsgLevelLayerFC( NET_MSG_GENERIC* pMsg );

public:
	D3DXVECTOR3 m_vMacroTracePos;

public:
	D3DXVECTOR3& GetMacroTraceTargetPos(void) { return m_vMacroTracePos; }
	void MsgMacroOnofFB(NET_MSG_GENERIC* pMsg);
	void MsgMacroCloseFC(NET_MSG_GENERIC* pMsg);
	void MsgMacroStateFC(NET_MSG_GENERIC* pMsg);
	void MsgMacroNearestCrowInfoFC(NET_MSG_GENERIC* pMsg);

	void MsgForceresetSkillStatFC ( NET_MSG_GENERIC* pMsg );

	void MsgGame( NET_MSG_GENERIC* pMsg );
	void MsgReqTakeFB( NET_MSG_GENERIC* pMsg );
	void MsgServerGeneralChat( NET_MSG_GENERIC* pMsg );

public:
	// PostRenew
	void MsgPostProcess(NET_MSG_GENERIC* pMsg);

	void FlyCamera_AddMsg( DWORD dwType, DWORD dwMsg );

	void ReqSetNotifyMoney( const LONGLONG& amount );
	void MsgSetNotifyMoney( NET_MSG_GENERIC* pMsg );

	void ReqSearchCharacter( const char* szName, bool bCurMap, DWORD dwCurMap );
	void ReqSearchCharacterDetailInfo( const char* szName );
	void ReqSearchCharacterPosition( const char* szName );
	LuaTable GetMemberInfoList( club::ALIGN_ORDER_TYPE emType, club::ALIGN_ORDER_DES_ASC emDesc, bool bOnline );
	std::string GetMyClubMasterGradeName();
	lua_tinker::table GetClubLogText(club::EM_LOG_TYPE LogType, WORD Page, WORD PageSize);
	lua_tinker::table GetClubBattleRecords();
	lua_tinker::table GetCDMRecord();
	lua_tinker::table GetAllianceClubInfo();
	lua_tinker::table GetHostileClubInfo();
	lua_tinker::table GetClubMarkInfo( const char* szClubName, bool bAllorHostile );
	bool ReqClubNoticeChange(const char* _Noice);
	const char* GetClubNickName_char();
	void ReqChangeGradeName(int _index, const char* _GradeName);
	void ReqAuthorityChange(lua_tinker::table _tbAuthData);
	club::AUTHORITY GetFlagByAuthIndex( size_t nAuthorityIndex );
	lua_tinker::table GetClubAuthority();
	lua_tinker::table CheckAuthorities( size_t nGradeIndex );
	LuaTable GetDisolutiontime();
	DWORD GetMemberDBNum(const char* _Name);
	void DelClubMember(const char* _Name);
	void ClubMasterChange(int nDBNum);
	void ClubCDAuth( const char* _Name );
	bool ClubCDAuthCheck( DWORD dwDBNum );
	void RequestClubJoin();
	const char* GetCTFProgressTime(const bool bCTFStart);
	int GetCTFState();
	void CheckGateMove();

	void SetGateOpen(BOOL bGate) { m_bGateOpen = bGate; }
	BOOL GetGateOpen() { return m_bGateOpen; }

	void PartyMemberAddBuff( DWORD dwGaeaID, DWORD dwSkillID, DWORD dwSlot );
	void PartyMemberRemoveBuff( DWORD dwGaeaID, DWORD dwSkillID, DWORD dwSlot );
	void PartyMemberRemoveAllBuff( DWORD dwGaeaID );

	// sf
	LuaTable SfReqPartyMemberGetSkillFactInfo(DWORD dwGaeaID);

	LuaTable SfPartyDistribution_GetItemInfo(void);
	void SfPartyDistribution_AbadonItem(void);
	void SfPartyDistribution_SelectItem(DWORD dwSelectIdx);
	void SfPartyDistribution_Bidding(void);
	LuaTable SfPartyDistribution_GetMemberInfo(void);
	void SfResetSelectAll();
	void SfPartyDistribution_SelectMember(DWORD dwSelectIdx);
	void SfPartyDistribution_ResetSelectByItemID( DWORD dwSelectIdx );
	void SfPartyDistribution_TransferItem(void);
	void SfPartyDistribution_JunkItem(void);
	

	bool CanComposeSetItem( const STARGETID& sTargetID, DWORD& dwCharClass, SNATIVEID* pPutonItem );
	bool IsWoreSetItem( EMSUIT emSuit, const SNATIVEID& sNID, const SNATIVEID* sPutOnItemCollect );
	void SetItemGroupComposition( const SNATIVEID& sItemID );

	lua_tinker::table SfGetCurrentMapGuidClubInfo(void);
	void ReqPetSkin( DWORD dwItemID, WORD wPosX, WORD wPosY, WORD wHoldPosX, WORD wHoldPosY );

public :
	//void SetCursorTargetInfo( const STARGETID& sTargetID );

	void MsgWithVehicleTarget(NET_MSG_GENERIC* pMsg);
	void VehicleWithAns(bool bOk);
	void Vehicle_GetoffwithVehicle();
	void SfTournamentTeamTextInfoRecall();
private:
	// ž�� ��û�� ���� ID;
	DWORD m_dwVehicleRidingRequestTarget;

/*
 * redmine : #1161 Randombox Ȯ������ �ý��� ó��
 * created : sckim, 2015.12.15
 * modified : 
 */
public:
    void MsgGmSetRandomboxChanceReqAC(NET_MSG_GENERIC* pMsg);
    void MsgGmGetRandomboxChanceListAC(NET_MSG_GENERIC* pMsg);
    void MsgGmDelRandomboxChanceAC(NET_MSG_GENERIC* pMsg);
    void MsgRandomboxWinnerNotifyAC(NET_MSG_GENERIC* pMsg);

public:
	void SfReqPeriodExtension(WORD wHoldPosX, WORD wHoldPosY, WORD wPosX, WORD wPosY);
	void SfPeriodExtensionReconfirm(WORD wHoldPosX, WORD wHoldPosY, WORD wPosX, WORD wPosY);

public:
	void MsgJackpotNotify( NET_MSG_GENERIC* pMsg );

	bool SfIsWorldBattleStage(void);

public:
	const int GetOpenCharacterSlotCount ( void );

};

