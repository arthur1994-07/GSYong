#ifndef DXGLOBALSTAGE_H_
#define DXGLOBALSTAGE_H_

#pragma once

#include "../../EngineLib/G-Logic/DxMsgProcessor.h"
#include "../../RanLogic/EmulBase.h"

#include "../../EngineLib/GUInterface/InterfaceBaseDefine.h"
//#include "../InterfaceBaseDefine.h"
#include "../Network/s_NetClient.h"
#include "./DxGameStage.h"
#include "./DxLobyStage.h"
#include "./DxWBStage.h"

#include "../../RanGfxUI/GfxDef.h"

#define _PRELOAD_BONE	"preload_skeleton.lst"
#define _PRELOAD_ANI	"preload_ani.lst"
#define _PRELOAD_SKIN	"preload_skin.lst"

//#ifdef _RELEASED
//#define TEST_INTRO_VIDEO
//#endif

class EngineDeviceMan;
class GfxInterfaceBase;
class GLGaeaClient;

typedef struct _AUTH_DATA
{
	GG_AUTH_DATA gg_Auth_Data;
	UINT		 Auth_Msg;
	BOOL bPass;
	int packetSize;
	int packetSeq;
	BYTE packet[4096];

	_AUTH_DATA()
        : Auth_Msg(0)
	{
		bPass = false;
		packetSize = 0;
		packetSeq = 0;
		memset(packet, 0, sizeof(BYTE)*4096);
	}
} AUTH_DATA, *P_AUTH_DATA;

class DxGlobalStage : public DxMsgProcessor
{
public:
	enum EMSTAGE
	{
		EM_STAGE_NULL	= 0,
		EM_STAGE_LOBY	= 1,
		EM_STAGE_GAME	= 2,
		EM_STAGE_LOBY_WORLDBATTLE	= 3,
	};

	enum EMSTATE
	{
		EM_CHANGE				= 0x0001,
		EM_CLOSE				= 0x0002,

		EM_REQ_CONNECT_WORLDBATTLE_LOGIN_SVR	= 0x0010,
		EM_REQ_CONNECT_WORLDBATTLE_GAME_SVR	= 0x0020,
		EM_REQ_CONNECT_WORLDBATTLE_CHA_SELECT	= 0x0040,
		EM_REQ_CONNECT_WORLDBATTLE_JOIN_GAME	= 0x0080,

		EM_REQCONNECT_LOGINSVR	= 0x1000,

	};

public:
	typedef std::vector<PBYTE>			MSGBUFFERLIST;
	typedef MSGBUFFERLIST::iterator		MSGBUFFERLIST_ITER;

private:
	typedef std::queue<P_AUTH_DATA>		GG_AUTH_QUEUE;
	GG_AUTH_QUEUE	m_GGAuthBuffer;
	CRITICAL_SECTION m_CS; 
public:
	UINT GetGGAuthData( PGG_AUTH_DATA pGG );
	UINT GetGGAuth30Data( P_AUTH_DATA pAD );

	DWORD				m_dwNProtect30ServerNumber;
public:
    DxGlobalStage(EngineDeviceMan* pEngineDevice, SUBPATH* pPath, bool bToolMode, bool bLoginProcessLog = false);
    virtual ~DxGlobalStage(void);
    
protected:
    EngineDeviceMan* m_pEngineDevice;
    GLGaeaClient* m_pGaeaClient;
    SUBPATH* m_pPath;
    EMSERVICE_PROVIDER m_ServiceProvider;

	HWND				m_hWnd;
	WORD				m_wWidth;
	WORD				m_wHeight;
	char				m_szFullPath[MAX_PATH];
	INT					m_nChannel; // 서버의 채널 번호

	BOOL				m_bEmulate;
	DWORD				m_dwState;
	EMSTAGE				m_emChangeStage;
	EMSTAGE				m_emThisStage;

	DxStage*			m_pActiveStage;
	DxLobyStage*		m_pLobyStage;
	DxWBStage*			m_pWBStage;
	DxGameStage*		m_pGameStage;
    
	GfxInterfaceBase*	m_pGfxInterface;

	CD3DApplication*	m_pD3DApp;
	D3DCAPSQ			m_d3dCaps;
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	CNetClient				m_NetClient; // 통신객체

	MSGBUFFERLIST*			m_pMsgActive;
	MSGBUFFERLIST*			m_pMsgReceive;
	
	MSGBUFFERLIST			m_MsgBufferA;
	MSGBUFFERLIST			m_MsgBufferB;

	static CRITICAL_SECTION	m_CSMsgProcLock;

	DWORD				m_dwBLOCK_PROG_COUNT;
	DWORD				m_dwBLOCK_PROG_IMAGE;
	bool				m_bBLOCK_PROG_DETECTED;
	bool				m_bDETECTED_REPORT;
	float				m_fBLOCK_PROG_TIMER;

	EmulBase* m_pEmulMsgField;

	CString m_strWorldBattleID;
	CString m_strPWDForWB;
public:
	bool m_bLoginProcessLog;

public:
	void SetWorldBattleID(CString strID) { m_strWorldBattleID = strID; }
	CString GetWorldBattleID() { return m_strWorldBattleID; }
	void SetPWDForWB(CString strPWD) { m_strPWDForWB = strPWD; }
	CString GetPWDForWB() { return m_strPWDForWB; }


public:
	void SetEmulMsgField( EmulBase* pEmulMsg );
    inline GLGaeaClient* GetGaeaClient() { return m_pGaeaClient; }
    inline EngineDeviceMan* GetEngineDevice() { return m_pEngineDevice; }
    inline SUBPATH* GetSubPath() { return m_pPath; }
    inline EMSERVICE_PROVIDER GetServiceProvider() const { return m_ServiceProvider; }

public:
	void SetProgDetected ( DWORD dwID );
	void SetDetectedReport ()			{ m_bDETECTED_REPORT = true; }
	bool GetDetectedReport ()			{ return m_bDETECTED_REPORT; }

protected:
	void MsgQueueFlip ();
	MSGBUFFERLIST& GetActiveMsgList ()			{ return *m_pMsgActive; }
	MSGBUFFERLIST& GetReceivedMsgList ()		{ return *m_pMsgReceive; }

public:
    struct DxGlobalStagePacketFunc
    {
        (void) (DxGlobalStage::*Msgfunc)(NET_MSG_GENERIC* pMsg);
        DxGlobalStagePacketFunc()
        {
            Msgfunc = NULL;
        }
    };
    DxGlobalStagePacketFunc m_GlobalStageMsgFunc[NET_MSG_GCTRL_MAX];

    void initPacketFunc();

	void MsgProcessFrame();
	void MsgProcessFrame(NET_MSG_GENERIC* pMsg);
    void LobyMsgProcessFrame(NET_MSG_GENERIC* pMsg);
    void GameMsgProcessFrame(NET_MSG_GENERIC* pMsg);
    void GaeaClientMsgProcessFrame(NET_MSG_GENERIC* pMsg);
    void GaeaClientMsgProcessDirect(NET_MSG_GENERIC* pMsg);
    void GaeaClientPingTrace(NET_MSG_GENERIC* pMsg);
	void GaeaClientAttendanceMaintenanceFB(NET_MSG_GENERIC* pMsg);
	void GaeaClientCTFMoveStateByGMFB(NET_MSG_GENERIC* pMsg);    
    
	void GaeaClientMsgChatGroup(NET_MSG_GENERIC* pMsg);
    void GaeaClientMsgFriend(NET_MSG_GENERIC* pMsg);

    void MsgGmKickUserProc(NET_MSG_GENERIC* pMsg);
    void MsgGameGuardAuth(NET_MSG_GENERIC* pMsg);
    void MsgGameGuardAuth1(NET_MSG_GENERIC* pMsg);
    void MsgGameGuardAuth2(NET_MSG_GENERIC* pMsg);
    void MsgApexData(NET_MSG_GENERIC* pMsg);

public:
	void MsgPostRenew( NET_MSG_GENERIC* pMsg );

public:
	// Country
	void MsgUpdateUserCountryAC ( NET_MSG_GENERIC* pMsg );
	void MsgSyncVictoriousCountry ( NET_MSG_GENERIC* pMsg );

	// Use Features 동기화;
	// Use Features 동기화가 이루어진 후;
	// 게임의 데이터 정보를 다시 초기화한다;
	void MsgSyncUseFeatures ( NET_MSG_GENERIC* pMsg );
	void MsgSyncUseFeaturesJoinWorldBattle ( NET_MSG_GENERIC* pMsg );

	void MsgRanMobileCommandResponse( NET_MSG_GENERIC* pMsg );

public:
	bool GetBlockDetectState() const { return m_bDETECTED_REPORT; }
	CNetClient* GetNetClient()	{ return &m_NetClient; }

	void NetSend(NET_MSG_GENERIC* nmg);
    void NetSend(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);

	void NetSendToField(NET_MSG_GENERIC* nmg);
    void NetSendToField(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);
    
	//void NetSendEventLottery ( const char* szLotteryName );
	
	virtual void MsgProcess(NET_MSG_GENERIC* nmg);

public:
	void OnInitialize();
	void UpdateJoinWorldBattle(bool bJoinWorldBattle);
	void OnInitLogin();

	BOOL IsEmulator() const { return m_bEmulate; }

	BOOL IsSTATE(DWORD dwState) const { return m_dwState&dwState; }
	void SetSTATE ( DWORD dwState )		{ m_dwState |= dwState; }
	void ReSetSTATE ( DWORD dwState )	{ m_dwState &= ~dwState; }

public:
	DxLobyStage* GetLobyStage ()			{ return m_pLobyStage; }
	DxGameStage* GetGameStage ()			{ return m_pGameStage; }
	DxStage* GetActiveStage ()				{ return m_pActiveStage; }
	GfxInterfaceBase* GetGfxInterfaece()	{ return m_pGfxInterface; }

	EMSTAGE GetStageType() { return m_emThisStage; }

protected:
	HRESULT ChangeStage ( WORD wWidth, WORD wHeight );	//	스테이지 변경 구동.

public:
	HRESULT ToChangeStage ( EMSTAGE ChangeStage );	//	스테이지 변경 요청.
	void CloseGame ( LPCTSTR lpszMsg = NULL );

	HRESULT GameToLobbyStage (bool bWorldBattle = false);

public:
	void SetD3DApp( CD3DApplication * pD3DApp );

	CD3DApplication* GetD3DApp ();
	LPDIRECT3DDEVICEQ GetD3dDevice ()			{ return m_pd3dDevice; }

	const char* GetAppPath()					{ return m_szFullPath; }

public:
	bool LoadMapList ();

public:
    HRESULT OneTimeSceneInitPrev( const char* szAppPath, HWND hWnd, BOOL bEmulate=FALSE, LPDIRECT3DDEVICEQ pd3dDevice = NULL );
    HRESULT OneTimeSceneInitNext(
        WORD wWidth/*=0*/,
        WORD wHeight/*=0*/,
        const char* szMapList/*=NULL*/,
        bool bToolMode);

public:
	HRESULT OneTimeSceneInit(
        const char* szAppPath,
        HWND hWnd,
        BOOL bEmulate/*=FALSE*/,
        WORD wWidth/*=0*/,
        WORD wHeight/*=0*/,
        const char* szMapList/*=NULL*/,
        bool bToolMode);
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ();
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();
	HRESULT FinalCleanup();

public:
	HRESULT FrameMove ( float m_fTime, float m_fElapsedTime, BOOL bNotRendering );
	HRESULT Render ( const D3DXCOLOR& cClearColor );

public:
	bool MsgProc ( MSG* pMsg );
	LRESULT OnNetNotify(WPARAM wParam, LPARAM lParam);

	HRESULT ReSizeWindow ( WORD wWidth, WORD wHeight );

public:
	HRESULT ChangeWndMode ();

public:
	HRESULT ToFullSize ();
	HRESULT ToRestoreSize ();
	HRESULT ToChangeWndStyle();

public:
	BOOL GetUsedMaterialSystem();

public:
	// 대만 서버의 채널 관련 함수
	void SetChannel( INT nChannel ) { m_nChannel = nChannel; }
	INT GetChannel() { return m_nChannel; }	// 채널은 0부터 시작, 초기값은 -1

    void SetGameJoin( BOOL bBOOL ) { m_pLobyStage->m_bGameJoin = bBOOL; }

    void SetUserCountry(wb::EM_COUNTRY Country);
    void ShowCountryClubCountInSelectCountryWindow();
    void ReloadClubListInSelectClubWindow();

	EMGFX_FONT_LANG GetTextLanguage();

    void DoModalOutInterface(const std::string& strText, int nModalTitle = UI::MODAL_NOTITLE, int nModalType = UI::OK, UIGUID CallerID = NO_ID, BOOL bHide = FALSE);
};

#endif // DXGLOBALSTAGE_H_
