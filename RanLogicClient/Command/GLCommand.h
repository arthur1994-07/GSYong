#pragma once

#include <boost/function.hpp>
#include <boost/bind.hpp>

typedef std::vector< std::string >	VEC_STRING;
typedef VEC_STRING::iterator		VEC_STRING_ITER;
typedef VEC_STRING::const_iterator	VEC_STRING_CITER;

class GLCommandFunc
{
public :
	GLCommandFunc(void)
	{
		CallbackFunc = NULL;
	}

public :
	boost::function< std::string( const VEC_STRING& value ) > CallbackFunc;
};
typedef std::map< std::string, GLCommandFunc >	MAP_COMMAND;
typedef MAP_COMMAND::iterator					MAP_COMMAND_ITER;

class GLCommand
{
private :
	GLCommand(void);
	GLCommand( const GLCommand& value );
	~GLCommand(void);

public :
	static GLCommand& GetInstance();

private :
	MAP_COMMAND m_mapCommand;

private :
	// 공백 단위로 명령어와 파라미터 분리 (명령어 리턴)
	std::string ParseCommand( const char* strCommand, VEC_STRING& vecParam );

public :
	// 명령 실행 (콘솔에 띄울 로그 리턴)
	std::string DoCommand( const char* strCommand );

	// 루아 명령 실행
	std::string Excute( const VEC_STRING& vecParam );
	// 스크립트, 스케일폼 리로드
	std::string ReloadWidget( const VEC_STRING& vecParam );
	// UI 열기
	std::string OpenWidget( const VEC_STRING& vecParam );
	// UI 닫기
	std::string CloseWidget( const VEC_STRING& vecParam );
	// UI 언로드
	std::string UnloadWidget( const VEC_STRING& vecParam );
	// 이름 디스플레이 활성/비활성
	std::string DisplayName( const VEC_STRING& vecParam );
	// UI 프로퍼티 저장/로드
	std::string SaveProp( const VEC_STRING& vecParam );
};
