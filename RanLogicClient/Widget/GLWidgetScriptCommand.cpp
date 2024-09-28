#include "pch.h"
#include "GLWidgetScript.h"

#include "../../RanGfxUI/GfxInterface.h"
#include "../GLGaeaClient.h"
#include "../Command/GLCommand.h"

std::string GLWidgetScript::DoCommand( const char* strCommand )
{
	NULL_INTERFACE_RETURN_VALUE( "" );

	if( 0 == strcmp( strCommand, "" ) )
		return "";

	return GLCommand::GetInstance().DoCommand( strCommand );
}