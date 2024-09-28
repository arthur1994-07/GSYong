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
	// ĳ���� ���� ���
	lua_tinker::table GetLobyCharInfoTable();
	// ĳ���� ����
	void SelectLobyChar( int nSelectPage, int nSelectNum, int nIndex );
	// ���� ����
	bool JoinGame( int nIndex );
	// ���� ���� 2�� ���
	bool JoinGame2ndPassword( const char* szPassword );
	// ���� ���� ����
	void DisconnectServer();
	// ĳ���� ����
	void DeleteChar( int nIndex, const char* strPassNum );
	// 2����� Ÿ��
	int Get2ndPassType();
};
