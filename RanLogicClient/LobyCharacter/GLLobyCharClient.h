#pragma once

#include "../../RanLogic/Msg/LoginMsg.h"
#include "../Widget/GLWidgetScript.h"

class GLLobyCharClient
{
private :
	GLLobyCharClient(void);
	GLLobyCharClient( const GLLobyCharClient& value );
	~GLLobyCharClient(void);

public :
	static GLLobyCharClient& GetInstance();

public :
	void MsgProcess( NET_MSG_GENERIC* nmg );

public :
	// 캐릭터 정보 얻기
	lua_tinker::table GetLobyCharInfoTable();
	// 캐릭터 선택
	void SelectLobyChar( int nSelectPage, int nSelectNum, int nIndex );
	// 게임 접속
	bool JoinGame( int nIndex );
	// 게임 접속 2차 비번
	bool JoinGame2ndPassword( const char* szPassword );
	// 서버 접속 끊기
	void DisconnectServer();
	// 캐릭터 삭제
	void DeleteChar( int nIndex, const char* strPassNum );
	// 2차비번 타입
	int Get2ndPassType();
};
