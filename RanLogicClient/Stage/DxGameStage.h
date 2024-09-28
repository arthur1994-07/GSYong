#ifndef _LOGIC_CLIENT_DXGAMESTAGE_H_
#define _LOGIC_CLIENT_DXGAMESTAGE_H_

#pragma once

#include "./DxStage.h"
#include "../../EngineLib/DxCommon/D3DApp.h"
#include "../Char/GLCharacter.h"

class GLClubClient;

struct DxCharJoinData
{
	bool				m_bLoginProcessLog;				// 로그인 프로세스 로그 기록 여부
	bool				m_bCharJoin;
	DWORD				m_dwCharID;				//	받고 있는 케릭터의 ID.

	DWORD				m_dwClientID;			//	클라이언트 ID
	DWORD				m_dwGaeaID;				//	생성 메모리 인덱스용.
	MapID				m_sMapID;				//	맵 ID.
	D3DXVECTOR3		m_vPos;					//	맵 상의 위치.
	bool					m_bCullByObjectMap; // 오브젝트에 의한 컬링 on/off;

	//	Note : 추가로 받아야 할 목록 개수.
	//
	DWORD				m_dwPutOnItems;
	DWORD				m_dwNumInvenItem;		//	인밴에 들어있는 아이템 갯수.
	DWORD				m_dwNumVNInvenItem;		//	베트남 인벤에 들어있는 아이템 갯수
	DWORD				m_dwNumSkill;			//	배운 스킬 목록.
	DWORD				m_dwNumProductBook;		//	조합서 습득 목록
	DWORD				m_dwNumQuestProc;		//	진행중인 퀘스트.
	DWORD				m_dwNumQuestEnd;		//	완료된 퀘스트.
	DWORD				m_dwChargeItem;			//	구입 아이템 목록.
	DWORD				m_dwClubMember;			//	클럽 구성원.
	DWORD				m_dwClubAlliance;		//	클럽 동맹.
	DWORD				m_dwClubBattle;			//	클럽 배틀.
	DWORD				m_dwItemCoolTime;		//	아이템 쿨타임

	//	Note : 수신한 목록 개수.
	DWORD				m_dwReceiveInvenItem;	//	인밴에 들어있는 아이템 갯수.
	DWORD				m_dwReceiveVNInvenItem;	//	베트남 인벤에 들어있는 아이템 갯수
	DWORD				m_dwReceiveSkill;		//	배운 스킬 목록.
	DWORD				m_dwReceiveProductBook;	//	조합서 습득 목록
	DWORD				m_dwReceiveQuestProc;	//	진행중인 퀘스트.
	DWORD				m_dwReceiveQuestEnd;	//	완료된 퀘스트.
	DWORD				m_dwReceiveChargeItem;	//	구입 아이템 목록.
	DWORD				m_dwReceiveClubMember;	//	클럽 맴버.
	DWORD				m_dwReceiveClubAlliance;//	클럽 동맹.
	DWORD				m_dwReceiveClubBattle;	//	클럽 배틀
	DWORD				m_dwReceiveItemCoolTime; //	아이템 쿨타임

	bool				m_bReceiveClubInfo;	

	GLClubClient*		m_pMyClub;
	SCHARDATA2			m_CharData2;			//	캐릭 기본 데이터.
	//bool				m_bTutorial;			//	튜토리얼 진행 여부.( true = 진행, false = 진행안함 ) by luxes.

	DWORD				m_dwCountryID;

	DxCharJoinData(GLClubClient* pMyClub);
    virtual ~DxCharJoinData();

	void ResetData();
	bool CheckReceive( bool bLoginProcessLog );
    inline GLClubClient* GetClub() const { return m_pMyClub; }
};

class GLGaeaClient;
class DxGlobalStage;

class DxGameStage : public DxStage
{
public:
    DxGameStage(DxGlobalStage* pGlobalStage, GLGaeaClient* pGaeaClient, bool bLoginProcessLog);
    virtual ~DxGameStage(void);

protected:
    GLGaeaClient* m_pGaeaClient;
    DxGlobalStage* m_pGlobalStage;
	D3DCAPSQ			m_d3dCaps;
	D3DSURFACE_DESC		m_d3dsdBackBuffer;

	DxCharJoinData*		m_pCharJoin;

	// QUEST_BUFFER_OVER
	// 퀘스트 하나의 크기가 일정이상인경우 서버에서 클라이어트로 전송하지 못하는 문제 발생 ( 일정 크기 이상은 무조건 return )
	// 압축을 하면 되지만 압축해도 나중에 압축해도 용량이 넘어서는 경우 또 문제가 발생됨.
	// 패킷을 이미 압축해서 보내기 때문에 압축한 데이터를 압축해도 효과가 없어짐...
	// 서버에서 클라이언트로 퀘스트 정보를 보내는 워크스레드가 한개이므로 동시에 여러 퀘스트가 분할되어 올수 없으므로 굳이 벡터나 맵을 사용하지 않음
	se::ByteStream	m_sQuestByteStream;
	int				m_QuestByteStreamID;

public :
	BOOL			m_bUseInterface;
	BOOL			m_bLoginProcessLog;

public:
	HRESULT Test () override;
	HRESULT Test2 () override;

	void OnInitLogin ();
	
	void ResetCharJoinData ();

	DWORD GetChaNum() const	{ return m_pCharJoin->m_dwCharID; }
	DWORD GetGaeaID() const { return m_pCharJoin->m_dwGaeaID; }
	DWORD GetClientID() const { return m_pCharJoin->m_dwClientID; }
	const DxCharJoinData* GetCharJoinData() const { return m_pCharJoin; }

	void SetSelectChar(SCHARINFO_LOBBY* pCharInfo);

private:
	HRESULT RenderWorldCharEffect();

public:
	virtual HRESULT OneTimeSceneInit ( HWND hWnd, WORD wWidth, WORD wHeight, const char* szAppPath );
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ();
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

public:
	virtual HRESULT FrameMove ( float m_fTime, float m_fElapsedTime, BOOL bNotRendering );
	virtual HRESULT Render ();

	virtual BOOL	GetUsedMaterialSystem();
    
    virtual HRESULT ReSizeWindow(WORD wWidth, WORD wHeight);

	void LobbyGameComplete();

	virtual void MsgProcess(NET_MSG_GENERIC* nmg);

    void MsgLobbyClubBattle(NET_MSG_GENERIC* nmg);
    void MsgLobbyClubAlliance(NET_MSG_GENERIC* nmg);
    //void MsgLobbyClubMember(NET_MSG_GENERIC* nmg);
    void MsgLobbyClubInfo(NET_MSG_GENERIC* nmg);
    void MsgLobbyClubInfoEx(NET_MSG_GENERIC* nmg);
    void MsgLobbyItemCoolTime(NET_MSG_GENERIC* nmg);
	void MsgLobbyInstanceList(NET_MSG_GENERIC* nmg);
    void MsgLobbyChargeItem(NET_MSG_GENERIC* nmg);
    void MsgLobbyQuestProg(NET_MSG_GENERIC* nmg);
    void MsgLobbyQuestEnd(NET_MSG_GENERIC* nmg);
	void MsgLobbyCharDefault(NET_MSG_GENERIC* nmg);
    void MsgLobbyCharSkill(NET_MSG_GENERIC* nmg);
    void MsgLobbyCharVietnamItem(NET_MSG_GENERIC* nmg);
    void MsgLobbyCharItem(NET_MSG_GENERIC* nmg);
    void MsgLobbyCharPutonEx(NET_MSG_GENERIC* nmg);
    void MsgLobbyCharJoin(NET_MSG_GENERIC* nmg);
    void MsgLobbyClubMemberMsgPack(NET_MSG_GENERIC* nmg);
    void MsgLobbyClubMemberMsgPackEnd();

	void MsgGCTRLConfront(NET_MSG_GENERIC* nmg);
	void MsgGCTRTradeTar(NET_MSG_GENERIC* nmg);
	void MsgDropOutForced(NET_MSG_GENERIC* nmg);

	void MsgLobbyQuestProgItem( NET_MSG_GENERIC* nmg );
};

#endif // _LOGIC_CLIENT_DXGAMESTAGE_H_
