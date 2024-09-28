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
	WORD			m_wWidgetID;			// ���� ID
	bool			m_bWindow;				// �������ΰ�?
	bool			m_bModal;				// ����ΰ�?
	bool			m_bEscCloseable;		// escŰ�� �ݱ� �����Ѱ�?
	bool			m_bSaveProp;			// ������Ƽ ���� ��� ���?
	bool			m_bSnapping;			// ������ ����Ұǰ�?
	bool			m_bCloseToDelete;		// ���� ��� �޸𸮿��� ������ ���ΰ�?

	WORD			m_wDeleteTime;			// ���� �� �޸𸮿��� ������ ������ ��� �ð�

	std::string		m_strFileName;			// swf/gfx ���� �̸�
	std::string		m_strWindowFile;		// ������ Ŭ���� �̸�
	std::wstring	m_strTitleName;			// Ÿ��Ʋ
	std::string		m_strLoadCompleteCall;	// �ε� �Ϸ� �� ȣ���� �Լ� �̸� 
	std::string		m_strOpenCall;			// UI ���µǸ� ȣ���� �Լ� �̸�

	WORD			m_wPosX;				// �ʱ� x ��ǥ
	WORD			m_wPosY;				// �ʱ� y ��ǥ

	WORD			m_wLayer;				// ���̾� ��ġ

	WORD			m_wAlignType;			// ���� Ÿ��
	int				m_nOffsetX;				// x ������
	int				m_nOffsetY;				// y ������
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

	// zip ���ο� �ִ� WidgetsList lua ������ �е��� �Ѵ�.
	// strPathWSF : zip ���� �̸�.
	bool LoadBufferZIP( lua_State* pLuaState, const std::string& strPathWSF );

	void Clear();

private:
	void SetWidgetList( lua_State* pLuaState );

public :
	GLWidget* GetWidget( WORD wWidgetID );

	const MAP_WIDGET& GetList() { return m_mapWidget; }
	int GetSize() { return m_mapWidget.size(); }
};
