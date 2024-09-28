#pragma once

namespace TexasHoldem
{
	/*
	std::string get_param_string( std::list< std::string >& params, BOOL separate );
	int RequestLogin();
	*/
	std::string GetPostArgument( int ServerGroup, DWORD dwChaDbNum, const std::string& strGUID );
}
