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
	// 온라인 상태인가?
	bool IsOnline();
	// 게임 서버 정보를 모두 받았나?
	bool IsGameServerInfoEnd();
	// 게임 서버 리스트 가져오기
	lua_tinker::table GetGameServerList();
	// 로그인 서버 접속 시도
	bool ConnectLoginServer();
	// 서버에서 복호화 키 받았으면 로그인 시도
	bool WaitKeyAndTryLogin();

	// 채널 리스트 가져오기
	lua_tinker::table GetServerChannelList( int nGroupIndex );
	// 선택한 채널에 접속
	bool ConnectServerChannel( int nServerIndex, int nServerGroup, int nChannelIndex );
	// 로그인 하기 위해 서버 인덱스 설정
	bool SetServerNumber( int nServerIndex );
	// 보안 코드 자릿수 가져오기
	int GetRandomPassNumber();

	// 저장된 유저 아이디
	std::string SaveUserID();
	// 유저 아이디 저장 여부 설정
	void SetSaveUserID( bool bSave );
	// 유저 아이디 저장 여부
	bool IsSaveUserID();
	// 로그인
	void Login( const char* strID, const char* strPW, const char* strRP, int nServerChannel );
	// 통합전장 로그인 W.B.
	BOOL LoginWorldBattle();
};
