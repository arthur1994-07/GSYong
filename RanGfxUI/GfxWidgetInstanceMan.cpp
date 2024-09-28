#include "stdafx.h"
#include "GfxWidgetInstanceMan.h"

GfxWidgetInstance::GfxWidgetInstance()
{
}

GfxWidgetInstance::~GfxWidgetInstance()
{
	MAP_INSTANCE_ITER iter = m_mapInstance.begin();
	for( ; iter != m_mapInstance.end(); ++iter )
	{
		GFx::Value& val = iter->second;
		val.SetUndefined();
	}

	m_mapInstance.clear();
}

//------------------------------------------------------------------------------------------

GfxWidgetInstanceMan::GfxWidgetInstanceMan()
{
}

GfxWidgetInstanceMan::GfxWidgetInstanceMan( const GfxWidgetInstanceMan& value )
{
}

GfxWidgetInstanceMan::~GfxWidgetInstanceMan()
{
	Clear();
}

GfxWidgetInstanceMan& GfxWidgetInstanceMan::GetInstance()
{
	static GfxWidgetInstanceMan Instance;
	return Instance;
}

void GfxWidgetInstanceMan::Clear()
{
	m_mapWidget.clear();
}

void GfxWidgetInstanceMan::Insert( WORD wWidgetID, WORD wSubID, std::string strInst, GFx::Value& value )
{
	SNATIVEID sID( wWidgetID, wSubID );

	MAP_WIDGET_ITER iter = m_mapWidget.find( sID );
	if( iter == m_mapWidget.end() )
	{
		m_mapWidget.insert( std::make_pair( sID, GfxWidgetInstance() ) );
		iter = m_mapWidget.find( sID );
	}

	GfxWidgetInstance& widgetInst = iter->second;
	widgetInst.m_mapInstance.insert( std::make_pair( strInst, value ) );
}

void GfxWidgetInstanceMan::Remove( WORD wWidgetID, WORD wSubID )
{
	SNATIVEID sID( wWidgetID, wSubID );

	MAP_WIDGET_ITER iter = m_mapWidget.find( sID );
	if( iter == m_mapWidget.end() )
		return;

	m_mapWidget.erase( iter );
}

GFx::Value* GfxWidgetInstanceMan::GetWidgetInstance( WORD wWidgetID, WORD wSubID, std::string strInst )
{
	SNATIVEID sID( wWidgetID, wSubID );

	MAP_WIDGET_ITER iter = m_mapWidget.find( sID );
	if( iter == m_mapWidget.end() )
		return NULL;

	GfxWidgetInstance& widgetInst = iter->second;

	GfxWidgetInstance::MAP_INSTANCE_ITER iterWidgetInst = widgetInst.m_mapInstance.find( strInst );
	if( iterWidgetInst == widgetInst.m_mapInstance.end() )
		return NULL;

	return &(iterWidgetInst->second);
}

GfxWidgetInstance* GfxWidgetInstanceMan::GetWidget( WORD wWidgetID, WORD wSubID )
{
	SNATIVEID sID( wWidgetID, wSubID );

	MAP_WIDGET_ITER iter = m_mapWidget.find( sID );
	if( iter == m_mapWidget.end() )
	{
		m_mapWidget.insert( std::make_pair( sID, GfxWidgetInstance() ) );
		iter = m_mapWidget.find( sID );
	}

	return &(iter->second);
}

void GfxWidgetInstanceMan::InsertDelete( WORD wWidgetID, WORD wSubID )
{
	SNATIVEID sID( wWidgetID, wSubID );

	m_mapDeleteReserve.insert( std::make_pair( sID, 0.0f ) );
}

void GfxWidgetInstanceMan::RemoveDelete( WORD wWidgetID, WORD wSubID )
{
	SNATIVEID sID( wWidgetID, wSubID );

	MAP_DELETE_RESERVE_ITER iter = m_mapDeleteReserve.find( sID );
	if( iter == m_mapDeleteReserve.end() )
		return;

	m_mapDeleteReserve.erase( iter );
}