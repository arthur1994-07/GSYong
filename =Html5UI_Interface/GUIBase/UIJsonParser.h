#pragma once

#include "json/json.h"

namespace GUIBase
{

class CUIJsonParser
{
public:
	static const std::string Write( Json::Value& root );

	static bool Read( const std::string& readString, Json::Value& root );	
};

}