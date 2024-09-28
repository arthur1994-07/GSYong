#include "../pch.h"
#include <string>
#include <LIST>
#include <map>
#include ".\cplmanager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL SIPInfo::CheckConnectionIP( const std::string& strIP )
{

	CString strTemp;
	BYTE checkIP;
	for( BYTE i = 0; i < 4; i++ )
	{
		AfxExtractSubString( strTemp, strIP.c_str(), i, _T('.') );
		checkIP = atoi(strTemp.GetString());

		if( checkIP < beginIP[i] || checkIP > endIP[i] ) return FALSE;
	}	

	
	return TRUE;
}

CCPLManager::CCPLManager(void)
{
}

CCPLManager::~CCPLManager(void)
{
}

CString CCPLManager::GetAppPath ()
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


BOOL CCPLManager::LoadCPLFile()
{

	CString strAppPath;	

	strAppPath = GetAppPath();
	strAppPath += _T("\\cfg\\");
	strAppPath += "ConnectList.cpl";


	FILE *fp = _fsopen( strAppPath.GetString(), "rb", _SH_DENYNO );
	if( !fp ) return FALSE;

	LIST_IPLIST listConnectIP;
	m_mapIPv1List.clear();

	
	// 전체 갯수
	int	 nMaxCount;
	fread( &nMaxCount, sizeof(int), 1, fp );

	for( int i = 0; i < nMaxCount; i++ )
	{
		SIPInfo sIpInfo;

		char sz1[24] = {0};
		char sz2[24] = {0};
		int	 leng1 = 0, leng2 = 0;

		// 아이피1 저장
		fread( &leng1, sizeof(int), 1, fp );
		fread( sz1, sizeof(char), leng1, fp );

		// 아이피2 저장
		fread( &leng2, sizeof(int), 1, fp );
		fread( sz2, sizeof(char), leng2, fp );

		sIpInfo.setIpInfo( sz1, sz2 );

		listConnectIP.push_back(sIpInfo);
	}
	LIST_IPLIST_ITER iter = listConnectIP.begin();
	CString str[4];
	for( ; iter != listConnectIP.end(); ++iter )
	{
		for( int i = iter->beginIP[0]; i <= iter->endIP[0]; i++ )
		{
			LIST_IPLIST ipList = m_mapIPv1List[i];
			ipList.push_back(*iter);
			m_mapIPv1List[i] = ipList;
		}
	}


	fclose( fp );

	return TRUE;
}

BOOL CCPLManager::CheckConnectionIP( const std::string& strIP )
{
	BYTE beginIP;
	CString strTemp;
	AfxExtractSubString( strTemp, strIP.c_str(), 0, _T('.') );
	beginIP = atoi(strTemp.GetString());

	MAP_IPv1LIST_ITER iterIPv1 = m_mapIPv1List.find(beginIP);
	if( iterIPv1 == m_mapIPv1List.end() ) return FALSE;

	LIST_IPLIST		 ipList = iterIPv1->second;
	LIST_IPLIST_ITER iter	= ipList.begin();

	CString str[4];
	for( ; iter != ipList.end(); ++iter )
	{
		if( iter->CheckConnectionIP(strIP) ) return TRUE;
	}

	return FALSE;
}

