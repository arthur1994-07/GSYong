#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include <share.h>
#include "CCfg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VersionManCfg::VersionManCfg()
{
	SetDefault();
}

VersionManCfg::VersionManCfg(const char* filename)
{
	SetDefault();
	Load(filename);
}

VersionManCfg::~VersionManCfg()
{
}

void VersionManCfg::SetDefault()
{
}

CString	VersionManCfg::GetAppPath()
{
	CString strFullPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			strFullPath = strCommandLine.Left ( dwFind );
			
			if ( !strFullPath.IsEmpty() )
			if ( strFullPath.GetAt(0) == '"' )
				strFullPath = strFullPath.Right ( strFullPath.GetLength() - 1 );
		}
	}

	return strFullPath;
}

bool VersionManCfg::Load(const char* filename)
{
	// Data type problem
	// program directory + cfg + filename	
	CString strAppPath;
	char szAppPath[MAX_PATH] = {0};

	strAppPath = GetAppPath ();
	strcpy(szAppPath, strAppPath.GetString());
	strcat(szAppPath, "\\");
	strcat(szAppPath, filename);

	FILE *oFile;
	char line[300];

	// Open for read 
    if ((oFile = ::_fsopen(szAppPath, "r", _SH_DENYNO )) == NULL)
	{
		return false;
	}	

	// Read a line and process
	while (::fgets(line, 200, oFile))
	{
		Process(line);
	}
	
	// Close file
	if (::fclose( oFile ))
		return false;
    else
	    return true;
}

int VersionManCfg::Process(char* sLine)
{
	// 토큰으로 분리할 분리자...
	// space, comma, tab, new line
	char seps[]   = " ,\t\n";
	char *token;

	// Establish string and get the first token
	token = ::strtok(sLine, seps);
	while (token != NULL)
	{	
		// 주석일때는 무시...
		if (::strcmp(token, "//") == 0) 
		{
			return 0;
		} 
		//////////////////////////////////////////////////////////////////////
		// database setting value 
		else if (::strcmp(token, "odbc_name") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				m_OdbcName = token;
			else
				return 0;
		}
		else if (::strcmp(token, "odbc_db_name") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				m_DbName = token;
			else
				return 0;
		}
		else if (::strcmp(token, "odbc_user_name") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				m_DbUserId = token;
			else
				return 0;
		}
		else if (::strcmp(token, "odbc_password") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				m_DbUserPassword = token;
			else
				return 0;
		}
		//////////////////////////////////////////////////////////////////////////////
		// FTP setting
		else if (::strcmp(token, "ftp_ip") == 0) 
		{
			token = ::strtok(NULL, seps );
			while (token != NULL)
			{
				m_vFtpIP.push_back(token);
				token = ::strtok(NULL, seps );
			}
		}
		else if (::strcmp(token, "ftp_port") == 0) 
		{
			token = ::strtok(NULL, seps);
			if (token)
				m_nFtpPort = atoi(token);
			else
				return 0;
		}
		else if (::strcmp(token, "ftp_user_name") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				m_FtpUserName = token;
			else
				return 0;
		}
		else if (::strcmp(token, "ftp_password") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				m_FtpPassword = token;
			else
				return 0;
		}
		// ////////////////////////////////////////////////////////////////////////////
		// Target path
		else if (::strcmp(token, "target_path") == 0) 
		{			
			token = ::strtok(NULL, seps );
			if (token)
				m_TargetPath = token;
			else
				return 0;
		}
		//
		else
		{
			
		}
		// Get next token
		token = ::strtok( NULL, seps );
	}
	return 0;
}