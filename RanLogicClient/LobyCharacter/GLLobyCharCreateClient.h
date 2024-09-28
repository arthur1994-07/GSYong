#pragma once

#include "../../RanLogic/s_NetGlobal.h"
#include "../Widget/GLWidgetScript.h"

class GLLobyCharCreateClient
{
private :
	GLLobyCharCreateClient(void);
	GLLobyCharCreateClient( const GLLobyCharCreateClient& vallue );
	~GLLobyCharCreateClient(void);

public :
	static GLLobyCharCreateClient& GetInstance();

public :
	int m_nSelectSchool;
	int m_nSelectSex;
	int m_nSelectClass;

public :
	void MsgProcess( NET_MSG_GENERIC* nmg );

public :
	// ĳ���� �б� ����
	void SelectCharSchool( int nSchool );
	// ĳ���� Ŭ���� ����
	void SelectCharClass( int nClass );
	// ĳ���� �� ����
	void SelectCharFace( int nFace );
	// ĳ���� ��� ����
	void SelectCharHair( int nHair );
	// ĳ���� ��
	void ShiftCharFace( bool bRight );
	// ĳ���� ���
	void ShiftCharHair( bool bRight );
	// �ϼ��� ĳ���� ���̱�
	void SetStyleStep( bool bStyle );
	// ĳ���� ȸ��
	void RotateChar( bool bRight, float fDegree );
	// ĳ���� ����
	int CreateChar( const char* strName );
	// ĳ���� ����;
	LuaTable GetCharacterRect(void);
};
