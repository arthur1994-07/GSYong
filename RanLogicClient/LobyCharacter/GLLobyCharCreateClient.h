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
	// 캐릭터 학교 선택
	void SelectCharSchool( int nSchool );
	// 캐릭터 클래스 선택
	void SelectCharClass( int nClass );
	// 캐릭터 얼굴 선택
	void SelectCharFace( int nFace );
	// 캐릭터 헤어 선택
	void SelectCharHair( int nHair );
	// 캐릭터 얼굴
	void ShiftCharFace( bool bRight );
	// 캐릭터 헤어
	void ShiftCharHair( bool bRight );
	// 완성된 캐릭터 보이기
	void SetStyleStep( bool bStyle );
	// 캐릭터 회전
	void RotateChar( bool bRight, float fDegree );
	// 캐릭터 생성
	int CreateChar( const char* strName );
	// 캐릭터 영역;
	LuaTable GetCharacterRect(void);
};
