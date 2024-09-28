#pragma once

#include "../../RanLogic/Activity/ActivityBase.h"
#include "../Widget/GLWidgetScript.h"

class GLGaeaClient;
struct NET_MSG_GENERIC;

class GLActivityClient //: public GLProduct
{
private:
	GLGaeaClient* m_pGaeaClient;
public:
	GLActivityClient( GLGaeaClient* pGaeaClent  );
	virtual ~GLActivityClient();

	const DWORD GetActivityCompletedCount( ENUM_ACTIVITY_CLASS emClass );
	const LONGLONG GetActivityPoint( ENUM_ACTIVITY_CLASS emClass );
	lua_tinker::table GetActivityList( UI_ACTIVITY_PAGE emPage, ENUM_ACTIVITY_CLASS emGrade, UI_ACTIVITY_CATEGORY emCategory );
	lua_tinker::table GetActivitySalesItemList( ENUM_ACTIVITY_CLASS emGrade );

	ENUM_ACTIVITY_CATEGORY ConvertCategory( UI_ACTIVITY_CATEGORY emCategory );
};