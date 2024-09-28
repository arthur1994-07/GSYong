#pragma once

#include <string>
#include <vector>

struct lua_State;
class SUBPATH;

//----------------------------------------------------------------------------------------

class GLWidget
{
public :
	GLWidget(void);
	~GLWidget(void);

public :
	WORD			m_wWidgetID;			// 위젯 ID
	bool			m_bWindow;				// 윈도우인가?
	bool			m_bModal;				// 모달인가?
	bool			m_bEscCloseable;		// esc키로 닫기 가능한가?
	bool			m_bSaveProp;			// 프로퍼티 저장 기능 사용?
	bool			m_bSnapping;			// 스냅핑 사용할건가?
	bool			m_bCloseToDelete;		// 닫을 경우 메모리에서 삭제할 것인가?

	WORD			m_wDeleteTime;			// 닫힌 후 메모리에서 삭제될 때까지 대기 시간

	std::string		m_strFileName;			// swf/gfx 파일 이름
	std::string		m_strWindowFile;		// 윈도우 클래스 이름
	std::wstring	m_strTitleName;			// 타이틀
	std::string		m_strLoadCompleteCall;	// 로드 완료 후 호출할 함수 이름 
	std::string		m_strOpenCall;			// UI 오픈되면 호출할 함수 이름

	WORD			m_wPosX;				// 초기 x 좌표
	WORD			m_wPosY;				// 초기 y 좌표

	WORD			m_wLayer;				// 레이어 위치

	WORD			m_wAlignType;			// 정렬 타입
	int				m_nOffsetX;				// x 오프셋
	int				m_nOffsetY;				// y 오프셋
};

typedef std::map< WORD, GLWidget >		MAP_WIDGET;
typedef MAP_WIDGET::iterator			MAP_WIDGET_ITER;
typedef MAP_WIDGET::const_iterator		MAP_WIDGET_CITER;

//----------------------------------------------------------------------------------------

class GLWidgetsList
{
private :
	GLWidgetsList(void);
	GLWidgetsList( const GLWidgetsList& value );
	~GLWidgetsList(void);

public :
	static GLWidgetsList& GetInstance();

private :
	MAP_WIDGET m_mapWidget;

public :
	bool LoadFile( lua_State* pLuaState, const SUBPATH* pPath );

	// zip 내부에 있는 WidgetsList lua 파일을 읽도록 한다.
	// strPathWSF : zip 파일 이름.
	bool LoadBufferZIP( lua_State* pLuaState, const std::string& strPathWSF );

	void Clear();

private:
	void SetWidgetList( lua_State* pLuaState );

public :
	GLWidget* GetWidget( WORD wWidgetID );

	const MAP_WIDGET& GetList() { return m_mapWidget; }
	int GetSize() { return m_mapWidget.size(); }
};
