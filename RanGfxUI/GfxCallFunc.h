#pragma once

class GfxCallbackFuncValue
{
public :
	GfxCallbackFuncValue( const GFx::Value* _args, const int _argCount )
		: args( _args )
		, argCount( _argCount )
	{
	}

public :
	const GFx::Value* args;
	const int argCount;
};

class GfxCallbackFunc
{
public :
	GfxCallbackFunc(void)
	{
		CallbackFunc = NULL;
	}

public :
	boost::function< void( const GfxCallbackFuncValue& value ) > CallbackFunc;
};

typedef std::map< std::string, GfxCallbackFunc >	MAP_CALLBACK_FUNC;
typedef MAP_CALLBACK_FUNC::iterator					MAP_CALLBACK_FUNC_ITER;