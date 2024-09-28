#include "pch.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

InterfaceBase::InterfaceBase(GLGaeaClient* pGaeaClient)
    : m_pGaeaClient(pGaeaClient)
{
}

InterfaceBase::~InterfaceBase()
{
}

namespace UI
{
	void StringSplit( const std::string& strOrigin, const std::string& strKey, std::vector<std::string>& vecStrResult )
	{
		bool bLoof = true;
		std::basic_string<char>::size_type Start = 0, End;
		std::basic_string<char>::size_type npos = -1;

		while( bLoof )
		{
			End = strOrigin.find( strKey, Start );

			if( End != npos )
			{
				vecStrResult.push_back( strOrigin.substr( Start, End - Start ) );
				Start = End + strKey.size();
			}
			else
			{
				vecStrResult.push_back( strOrigin.substr( Start, strOrigin.size() ) );
				bLoof = false;
			}
		}
	}
}