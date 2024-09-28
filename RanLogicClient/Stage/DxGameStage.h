#ifndef _LOGIC_CLIENT_DXGAMESTAGE_H_
#define _LOGIC_CLIENT_DXGAMESTAGE_H_

#pragma once

#include "./DxStage.h"
#include "../../EngineLib/DxCommon/D3DApp.h"
#include "../Char/GLCharacter.h"

class GLClubClient;

struct DxCharJoinData
{
	bool				m_bLoginProcessLog;				// �α��� ���μ��� �α� ��� ����
	bool				m_bCharJoin;
	DWORD				m_dwCharID;				//	�ް� �ִ� �ɸ����� ID.

	DWORD				m_dwClientID;			//	Ŭ���̾�Ʈ ID
	DWORD				m_dwGaeaID;				//	���� �޸� �ε�����.
	MapID				m_sMapID;				//	�� ID.
	D3DXVECTOR3		m_vPos;					//	�� ���� ��ġ.
	bool					m_bCullByObjectMap; // ������Ʈ�� ���� �ø� on/off;

	//	Note : �߰��� �޾ƾ� �� ��� ����.
	//
	DWORD				m_dwPutOnItems;
	DWORD				m_dwNumInvenItem;		//	�ι꿡 ����ִ� ������ ����.
	DWORD				m_dwNumVNInvenItem;		//	��Ʈ�� �κ��� ����ִ� ������ ����
	DWORD				m_dwNumSkill;			//	��� ��ų ���.
	DWORD				m_dwNumProductBook;		//	���ռ� ���� ���
	DWORD				m_dwNumQuestProc;		//	�������� ����Ʈ.
	DWORD				m_dwNumQuestEnd;		//	�Ϸ�� ����Ʈ.
	DWORD				m_dwChargeItem;			//	���� ������ ���.
	DWORD				m_dwClubMember;			//	Ŭ�� ������.
	DWORD				m_dwClubAlliance;		//	Ŭ�� ����.
	DWORD				m_dwClubBattle;			//	Ŭ�� ��Ʋ.
	DWORD				m_dwItemCoolTime;		//	������ ��Ÿ��

	//	Note : ������ ��� ����.
	DWORD				m_dwReceiveInvenItem;	//	�ι꿡 ����ִ� ������ ����.
	DWORD				m_dwReceiveVNInvenItem;	//	��Ʈ�� �κ��� ����ִ� ������ ����
	DWORD				m_dwReceiveSkill;		//	��� ��ų ���.
	DWORD				m_dwReceiveProductBook;	//	���ռ� ���� ���
	DWORD				m_dwReceiveQuestProc;	//	�������� ����Ʈ.
	DWORD				m_dwReceiveQuestEnd;	//	�Ϸ�� ����Ʈ.
	DWORD				m_dwReceiveChargeItem;	//	���� ������ ���.
	DWORD				m_dwReceiveClubMember;	//	Ŭ�� �ɹ�.
	DWORD				m_dwReceiveClubAlliance;//	Ŭ�� ����.
	DWORD				m_dwReceiveClubBattle;	//	Ŭ�� ��Ʋ
	DWORD				m_dwReceiveItemCoolTime; //	������ ��Ÿ��

	bool				m_bReceiveClubInfo;	

	GLClubClient*		m_pMyClub;
	SCHARDATA2			m_CharData2;			//	ĳ�� �⺻ ������.
	//bool				m_bTutorial;			//	Ʃ�丮�� ���� ����.( true = ����, false = ������� ) by luxes.

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
	// ����Ʈ �ϳ��� ũ�Ⱑ �����̻��ΰ�� �������� Ŭ���̾�Ʈ�� �������� ���ϴ� ���� �߻� ( ���� ũ�� �̻��� ������ return )
	// ������ �ϸ� ������ �����ص� ���߿� �����ص� �뷮�� �Ѿ�� ��� �� ������ �߻���.
	// ��Ŷ�� �̹� �����ؼ� ������ ������ ������ �����͸� �����ص� ȿ���� ������...
	// �������� Ŭ���̾�Ʈ�� ����Ʈ ������ ������ ��ũ�����尡 �Ѱ��̹Ƿ� ���ÿ� ���� ����Ʈ�� ���ҵǾ� �ü� �����Ƿ� ���� ���ͳ� ���� ������� ����
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
