#pragma once

#include <hash_map>
#include "../enginelib/G-Logic/TypeDefine.h"

class GfxWidgetInstance
{
public :
	GfxWidgetInstance();
	~GfxWidgetInstance();

public :
	typedef stdext::hash_map< std::string, Scaleform::GFx::Value >	MAP_INSTANCE;
	typedef MAP_INSTANCE::iterator									MAP_INSTANCE_ITER;

public :
	// 인스턴스 이름별 컴포넌트 및 무비클립
	MAP_INSTANCE m_mapInstance;
};

//------------------------------------------------------------------------------------------

class GfxWidgetInstanceMan
{
private :
	GfxWidgetInstanceMan();
	GfxWidgetInstanceMan( const GfxWidgetInstanceMan& value );
	~GfxWidgetInstanceMan();

public :
	static GfxWidgetInstanceMan& GetInstance();

public :
	typedef std::map< SNATIVEID, GfxWidgetInstance >	MAP_WIDGET;
	typedef MAP_WIDGET::iterator						MAP_WIDGET_ITER;

	typedef std::map< SNATIVEID, float >				MAP_DELETE_RESERVE;
	typedef MAP_DELETE_RESERVE::iterator				MAP_DELETE_RESERVE_ITER;

private :
	MAP_WIDGET			m_mapWidget;
	MAP_DELETE_RESERVE	m_mapDeleteReserve;

public :
	void Clear();

	void Insert( WORD wWidgetID, WORD wSubID, std::string strInst, Scaleform::GFx::Value& value );
	void Remove( WORD wWidgetID, WORD wSubID );

	Scaleform::GFx::Value* GetWidgetInstance( WORD wWidgetID, WORD wSubID, std::string strInst );
	GfxWidgetInstance* GetWidget( WORD wWidgetID, WORD wSubID );

	void InsertDelete( WORD wWidgetID, WORD wSubID );
	void RemoveDelete( WORD wWidgetID, WORD wSubID );
	MAP_DELETE_RESERVE& GetDeleteReserve() { return m_mapDeleteReserve; }
};