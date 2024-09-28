#pragma once
//
//mjeon
//

#include <string>

class IScriptMan
{
public:
	IScriptMan()
		: m_bLoaded(FALSE)
	{
	}

public:
	virtual void LoadScript(const char *szFilePath) = 0;

	virtual BOOL IsLoaded()							= 0;

protected:
	virtual void ToDo()								= 0;	// User-defined-method called by LoadScript

	std::string m_strFilePath;
	std::string m_strError;
	BOOL		m_bLoaded;

public:
	std::string GetError() { return m_strError; }
};
