#pragma once

#include "../../RanLogic/WorldBattle/WorldBattleDefine.h"
#include "../Widget/GLWidgetScript.h"

class GLLoginClient
{
private :
	GLLoginClient(void);
	GLLoginClient( const GLLoginClient& value );
	~GLLoginClient(void);

public :
	static GLLoginClient& GetInstance();

private :
	bool			m_bChaRemainExtreme;
	bool			m_bChaRemainActor;
	USHORT			m_uChaRemain;
	wb::EM_COUNTRY	m_Country;

public :
	int m_nSelectServerIndex;
	int m_nServerGroup;
	int m_nServerChannel;

public :
	void MsgProcess( LPVOID pBuffer );

private :
	BOOL LoadClientVersion( int& nPatchVer, int& nGameVer );

public :
	void SetCharRemainEx( const bool bChaRemain );
	const bool GetCharRemainEx() const;

	void SetCharRemainActor ( const bool bChaRemain );
	const bool GetCharRemainActor () const;

	void SetCountry( wb::EM_COUNTRY Country );
	wb::EM_COUNTRY GetCountry() const { return m_Country; }

	void SetCharRemain( const USHORT uChaRemain )		{ m_uChaRemain = uChaRemain; }
	const USHORT GetCharRemain() const					{ return m_uChaRemain; }

public :
	// �¶��� �����ΰ�?
	bool IsOnline();
	// ���� ���� ������ ��� �޾ҳ�?
	bool IsGameServerInfoEnd();
	// ���� ���� ����Ʈ ��������
	lua_tinker::table GetGameServerList();
	// �α��� ���� ���� �õ�
	bool ConnectLoginServer();
	// �������� ��ȣȭ Ű �޾����� �α��� �õ�
	bool WaitKeyAndTryLogin();

	// ä�� ����Ʈ ��������
	lua_tinker::table GetServerChannelList( int nGroupIndex );
	// ������ ä�ο� ����
	bool ConnectServerChannel( int nServerIndex, int nServerGroup, int nChannelIndex );
	// �α��� �ϱ� ���� ���� �ε��� ����
	bool SetServerNumber( int nServerIndex );
	// ���� �ڵ� �ڸ��� ��������
	int GetRandomPassNumber();

	// ����� ���� ���̵�
	std::string SaveUserID();
	// ���� ���̵� ���� ���� ����
	void SetSaveUserID( bool bSave );
	// ���� ���̵� ���� ����
	bool IsSaveUserID();
	// �α���
	void Login( const char* strID, const char* strPW, const char* strRP, int nServerChannel );
	// �������� �α��� W.B.
	BOOL LoginWorldBattle();
};
