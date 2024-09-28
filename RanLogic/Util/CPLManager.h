#pragma once

/************************************************************************/
/* 에이전트 서버에서 접속 가능한 IP만 찾아 접속하게 하는 함수			*/
/************************************************************************/


struct SIPInfo 
{
	BYTE beginIP[4];
	BYTE endIP[4];

	SIPInfo::SIPInfo()
	{
		ZeroMemory( beginIP, 4 );
		ZeroMemory( endIP, 4 );
	}

	void setIpInfo( const std::string& str1, const std::string& str2 )
	{
		CString strTemp;
		BYTE i;
		for( i = 0; i < 4; i++ )
		{
			AfxExtractSubString( strTemp, str1.c_str(), i, _T('.') );
			beginIP[i] = atoi(strTemp.GetString());

			AfxExtractSubString( strTemp, str2.c_str(), i, _T('.') );
			endIP[i] = atoi(strTemp.GetString());
		}
	}

	BOOL CheckConnectionIP( const std::string& strIP );
	
};


class CCPLManager
{
public:
	CCPLManager(void);
	~CCPLManager(void);

	BOOL LoadCPLFile();
	BOOL CheckConnectionIP( const std::string& strIP );
	CString GetAppPath ();

	typedef std::list<SIPInfo>    LIST_IPLIST;
	typedef LIST_IPLIST::iterator LIST_IPLIST_ITER;

	typedef std::map<BYTE,LIST_IPLIST>	MAP_IPv1LIST;
	typedef MAP_IPv1LIST::iterator		MAP_IPv1LIST_ITER;

private:
	MAP_IPv1LIST m_mapIPv1List;
};
