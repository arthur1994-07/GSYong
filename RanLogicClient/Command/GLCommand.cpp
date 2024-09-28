#include "pch.h"
#include "GLCommand.h"

#include <cctype>

#include "../GfxInterfaceBase.h"
#include "../Widget/GLWidgetScript.h"
#include "../Widget/GLWidgetsList.h"
#include "../Widget/GLWidgetTexture.h"
#include "../Widget/GLWidgetSaveProp.h"

#define ADD_COMMAND( commandName, fun, class ) {\
	GLCommandFunc commandFunc;\
	commandFunc.CallbackFunc = boost::bind( std::mem_fun( fun ), class, _1 );\
	m_mapCommand[ commandName ] = commandFunc;\
}

GLCommand::GLCommand(void)
{
	ADD_COMMAND( "execute", &GLCommand::Excute, this );
	ADD_COMMAND( "reloadwidget", &GLCommand::ReloadWidget, this );
	ADD_COMMAND( "openwidget", &GLCommand::OpenWidget, this );
	ADD_COMMAND( "closewidget", &GLCommand::CloseWidget, this );
	ADD_COMMAND( "unloadwidget", &GLCommand::UnloadWidget, this );
	ADD_COMMAND( "dspname", &GLCommand::DisplayName, this );
	ADD_COMMAND( "saveprop", &GLCommand::SaveProp, this );
}

GLCommand::GLCommand( const GLCommand& value )
{
}

GLCommand::~GLCommand(void)
{
}

GLCommand& GLCommand::GetInstance()
{
	static GLCommand Instance;
	return Instance;
}

std::string GLCommand::ParseCommand( const char* strCommand, VEC_STRING& vecParam )
{
	std::string strCom = "";
	std::string strPush = "";

	bool bString = false;

	for( ; *strCommand; ++strCommand )
	{
		if( 0 == strncmp( strCommand, "#", 1 ) )
		{
			if( true == bString )
			{
				bString = false;

				if( true == strCom.empty() )
					strCom = strPush;
				else
					vecParam.push_back( strPush );

				strPush = "";
			}
			else
			{
				bString = true;
			}

			continue;
		}
		else if( 0 == strncmp( strCommand, " ", 1 ) && false == bString )
		{
			if( true == strCom.empty() )
				strCom = strPush;
			else
				vecParam.push_back( strPush );

			strPush = "";

			continue;
		}

		strPush += strCommand[ 0 ];
	}

	if( false == strPush.empty() )
	{
		if( true == strCom.empty() )
			strCom = strPush;
		else
			vecParam.push_back( strPush );
	}

	return strCom;
}

std::string GLCommand::DoCommand( const char* strCommand )
{
	VEC_STRING vecParam;
	std::string strCom = ParseCommand( strCommand, vecParam );

	std::transform( strCom.begin(), strCom.end(), strCom.begin(), std::tolower );

	MAP_COMMAND_ITER iter = m_mapCommand.find( strCom );
	if( iter == m_mapCommand.end() )
	{
		std::string strErrorMsg = "Invalid Command '";
		strErrorMsg += strCom;
		strErrorMsg += "'";

		return strErrorMsg;
	}

	return iter->second.CallbackFunc( vecParam );
}

std::string GLCommand::Excute( const VEC_STRING& vecParam )
{
	if( 1 != vecParam.size() )
		return "Invalid Parameters : Excute [#Script#]";

	GLWidgetScript::GetInstance().LuaDoCommand( vecParam[ 0 ].c_str() );

	return "";
}

std::string GLCommand::ReloadWidget( const VEC_STRING& vecParam )
{
	std::string strError = 
		"Invalid Parameters : ReloadWidget [Type]\nType :  Script - 0,  Scaleform - 1,  Script&Scaleform - 2";

	if( 1 != vecParam.size() )
		return strError;

	int nType = atoi( vecParam[ 0 ].c_str() );

	switch( nType )
	{
	case 0 :
		GLWidgetScript::GetInstance().Reload();
		break;

	case 1 :
		GfxInterfaceBase::GetInstance()->Reload();
		break;

	case 2 :
		GLWidgetScript::GetInstance().Reload();
		GfxInterfaceBase::GetInstance()->Reload();
		break;

	default :
		return strError;
	}

	return "Wait Reload";
}

std::string GLCommand::OpenWidget( const VEC_STRING& vecParam )
{
	size_t nSize = vecParam.size();

	if( 1 != nSize && 2 != nSize )
		return "Invalid Parameters : OpenWidget [WidgetID] [SubID = 0]";

	int nWidgetID = atoi( vecParam[ 0 ].c_str() );
	if( 0 > nWidgetID )
		return "Invalid Parameters : Incorrect WidgetID";

	int nSubID = 0;
	if( 2 == nSize )
	{
		nSubID = atoi( vecParam[ 1 ].c_str() );
		if( 0 > nSubID )
			return "Invalid Parameters : Incorrect SubID";
	}

	GfxInterfaceBase::GetInstance()->OpenWidget( nWidgetID, nSubID );

	return "";
}

std::string GLCommand::CloseWidget( const VEC_STRING& vecParam )
{
	size_t nSize = vecParam.size();

	if( 1 != nSize && 2 != nSize )
		return "Invalid Parameters : CloiseWidget [WidgetID] [SubID = 0]";

	int nWidgetID = atoi( vecParam[ 0 ].c_str() );
	if( 0 > nWidgetID )
		return "Invalid Parameters : Incorrect WidgetID";

	int nSubID = 0;
	if( 2 == nSize )
	{
		nSubID = atoi( vecParam[ 1 ].c_str() );
		if( 0 > nSubID )
			return "Invalid Parameters : Incorrect SubID";
	}

	GfxInterfaceBase::GetInstance()->CloseWidget( nWidgetID, nSubID );

	return "";
}

std::string GLCommand::UnloadWidget( const VEC_STRING& vecParam )
{
	size_t nParam = vecParam.size();
	if( 1 != nParam || 2 != nParam )
		return "Invalid Parameters : UnloadWidget [WidgetID] [SubID = 0]";

	if( 0 == strcmp( "all", vecParam[ 0 ].c_str() ) )
	{
		GfxInterfaceBase::GetInstance()->UnloadWidget( -1, 0 );
	}
	else
	{
		int nWidgetID = 0;
		int nSubID = 0;

		if( 1 == nParam )
		{
			nWidgetID = atoi( vecParam[ 0 ].c_str() );
		}
		else if( 2 == nParam )
		{
			nWidgetID = atoi( vecParam[ 0 ].c_str() );
			nSubID = atoi( vecParam[ 1 ].c_str() );
		}
		else
		{
			return "Invalid Parameters : UnloadWidget [WidgetID] [SubID = 0]";
		}

		GfxInterfaceBase::GetInstance()->UnloadWidget( nWidgetID, nSubID );
	}	

	return "";
}

std::string GLCommand::DisplayName( const VEC_STRING& vecParam )
{
	if( 1 != vecParam.size() )
		return "Invalid Parameters : DspName [Visible]\nVisible : On - 1, Off - 0"; 

	bool bValue = atoi( vecParam[ 0 ].c_str() ) == 0 ? false : true;
	GfxInterfaceBase::GetInstance()->SetDisplayOn( bValue );

	return "";
}

std::string GLCommand::SaveProp( const VEC_STRING& vecParam )
{
	if( 1 != vecParam.size() )
		return "Invalid Parameters : SaveProp [Save or Load]\nSave or Load : Save - 1, Load - 2";

	int nValue = atoi( vecParam[ 0 ].c_str() );
	if( 1 == nValue )
	{
		GLWidgetSaveProp::GetInstance().SaveXML();

		return "Save WidgetProp";
	}
	else if( 2 == nValue )
	{
		GLWidgetSaveProp::GetInstance().LoadXML("");

		return "Load WidgetProp";
	}
	else
	{
		return "Nothing occurred";
	}

	return "";
}