#include "stdafx.h"
#include "UIJsonParser.h"

namespace GUIBase
{
	const std::string CUIJsonParser::Write( Json::Value& root )
	{
		static Json::StyledWriter writer;
		
		return writer.write( root );
	}

	bool CUIJsonParser::Read( const std::string& readString, Json::Value& root )
	{
		static Json::Reader reader;

		bool parsingSuccessful = reader.parse( readString, root );
		if( !parsingSuccessful )
			return false;

		return true;
	}
}