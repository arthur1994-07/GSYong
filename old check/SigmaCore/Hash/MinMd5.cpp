#include "StdAfx.h"
#include "../String/StringFormat.h"
#include "./MinMd5.h"

// ----------------------------------------------------------------------------
#include "../DebugInclude.h"
// ----------------------------------------------------------------------------

namespace sc
{

MinMd5::MinMd5(void) 
	: m_hCryptProv(NULL)
	, m_dwHashLen(16)
{
	init();
}

MinMd5::~MinMd5(void)
{	
	if (m_hCryptProv)
		CryptReleaseContext(m_hCryptProv, 0);
}

bool MinMd5::init()
{
	// PROV_RSA_FULL / Algorithms Supported
	// Key Exchange : RSA 
	// Signature : RSA 
	// Encryption RC2, RC4 
	// Hashing : MD5,SHA

	// vista 에서 일부 사용자 계정에 문제가 있어서 CRYPT_MACHINE_KEYSET 를 추가한다. 2008-12-24 Jgkim
	if (FALSE == CryptAcquireContext( &m_hCryptProv, "RanOnline", NULL, PROV_RSA_FULL, 0 ) )
	{
		if (FALSE == CryptAcquireContext( &m_hCryptProv, "RanOnline", NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET ) )
		{
			if (FALSE == CryptAcquireContext( &m_hCryptProv, "RanOnline", NULL, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET ) )
			{
				if (FALSE == CryptAcquireContext( &m_hCryptProv, "RanOnline", NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET|CRYPT_MACHINE_KEYSET ) )
				{
					// Error code : http://msdn.microsoft.com/en-us/library/aa379886(VS.85).aspx
					int nRetCode = GetLastError();
                    std::string strMsg = sc::string::format("MinMd5 File System Error:%1%", nRetCode);
					AfxMessageBox(strMsg.c_str(), MB_OK);
					return false;
				}
			}
		}
	}
	return true;
}

bool MinMd5::getMd5(const BYTE* pData, DWORD dwLength, CStringA& strTarget )
{
	HCRYPTHASH hHash = NULL;
	BYTE bHash[0x7f];
	DWORD dwContent = dwLength;

	const BYTE* pbContent = pData;

	strTarget.Empty();
	// Acquire a hash object handle.
	if (!CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &hHash))
	{
		//int nError = GetLastError();
		return false;
	}

	if (!CryptHashData(hHash, pbContent, dwContent, 0))
	{
		if (hHash) CryptDestroyHash(hHash);
		return false;
	}
	
	if (!CryptGetHashParam(hHash, HP_HASHVAL, bHash, &m_dwHashLen, 0))
	{
		if (hHash) CryptDestroyHash(hHash);
		return false;
	}

	CString strTemp;
	for (int i=0; i<16; i++)
	{
		strTemp.Format(_T("%02x"), bHash[i]);
		strTarget += strTemp;
	}
	
	if (hHash)
		CryptDestroyHash(hHash);
	return true;
}

bool MinMd5::getMd5(const BYTE* pData, DWORD dwLength, CStringW& strTarget )
{
    HCRYPTHASH hHash = NULL;
    BYTE bHash[0x7f];
    DWORD dwContent = dwLength;

    const BYTE* pbContent = pData;

    strTarget.Empty();
    // Acquire a hash object handle.
    if (!CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &hHash))
    {
        //int nError = GetLastError();
        return false;
    }

    if (!CryptHashData(hHash, pbContent, dwContent, 0))
    {
        if (hHash) CryptDestroyHash(hHash);
        return false;
    }

    if (!CryptGetHashParam(hHash, HP_HASHVAL, bHash, &m_dwHashLen, 0))
    {
        if (hHash) CryptDestroyHash(hHash);
        return false;
    }

    CStringW strTemp;
    for (int i=0; i<16; i++)
    {
        strTemp.Format(L"%02x", bHash[i]);
        strTarget += strTemp;
    }

    if (hHash)
        CryptDestroyHash(hHash);
    return true;
}

bool MinMd5::getMd5(const CStringA& strSource, CStringA& strTarget)
{
	HCRYPTHASH hHash = NULL;
	BYTE bHash[0x7f];
	DWORD dwContent = strSource.GetLength();

	const BYTE* pbContent = (const BYTE*) strSource.GetString();

	strTarget.Empty();
	// Acquire a hash object handle.
	if (!CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &hHash))
	{
		//int nError = GetLastError();
		return false;
	}

	if (!CryptHashData(hHash, pbContent, dwContent, 0))
	{
		if (hHash)
            CryptDestroyHash(hHash);
		return false;
	}
	
	if (!CryptGetHashParam(hHash, HP_HASHVAL, bHash, &m_dwHashLen, 0))
	{
		if (hHash)
            CryptDestroyHash(hHash);
		return false;
	}

	CString strTemp;
	for (int i=0; i<16; i++)
	{
		strTemp.Format(_T("%02x"), bHash[i]);
		strTarget += strTemp;
	}
	
	if (hHash)
        CryptDestroyHash(hHash);
	return true;
}

bool MinMd5::getMd5(const CStringW& strSource, CStringW& strTarget)
{
	HCRYPTHASH hHash = NULL;
	BYTE bHash[0x7f];
	DWORD dwContent = strSource.GetLength() * 2;

	const BYTE* pbContent = (const BYTE*) strSource.GetString();

	strTarget.Empty();
	// Acquire a hash object handle.
	if (!CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &hHash))
	{
		//int nError = GetLastError();
		return false;
	}

	if (!CryptHashData(hHash, pbContent, dwContent, 0))
	{
		if (hHash)
            CryptDestroyHash(hHash);
		return false;
	}
	
	if (!CryptGetHashParam(hHash, HP_HASHVAL, bHash, &m_dwHashLen, 0))
	{
		if (hHash)
            CryptDestroyHash(hHash);
		return false;
	}

	CString strTemp;
	for (int i=0; i<16; i++)
	{
		strTemp.Format(_T("%02x"), bHash[i]);
		strTarget += strTemp;
	}
	
	if (hHash)
        CryptDestroyHash(hHash);
	return true;
}

void MinMd5::ExciteSetSecretKey(const TCHAR* szSecretKey)
{
	m_strSecretKey = szSecretKey;
}

bool MinMd5::ExciteGetMd5(const TCHAR* szU, const TCHAR* szT, const TCHAR* szMd5)
{
	if (szU == NULL || szT == NULL || szMd5 == NULL)
		return false;

	CString strU = szU;	
	CString strT = szT;
	CString strTemp = strU + strT + m_strSecretKey;
	CString strMd5Source = szMd5;
	CString strMd5Target;		

	if ( getMd5( strTemp, strMd5Target ) == false )
	{
		return false;
	}
	else
	{
		if (strMd5Source.CompareNoCase(strMd5Target) == 0)
		{
			return true;
		}
		else
		{
			return false;
		}			
	}
}

bool MinMd5::ExciteCheckDate(
	CString& strDate )
{
	if (strDate.GetLength() != 14)	
		return false;

	CTime tNow( CTime::GetCurrentTime() ); // 현재시간

	int nYear, nMonth, nDay, nHour, nMin, nSec;

	// ex) 2005^06^30^16^34^53

	nYear  = _tstoi( strDate.Mid( 0, 4).GetString() );
	nMonth = _tstoi( strDate.Mid( 4, 2).GetString() );
	nDay   = _tstoi( strDate.Mid( 6, 2).GetString() );
	nHour  = _tstoi( strDate.Mid( 8, 2).GetString() );
	nMin   = _tstoi( strDate.Mid(10, 2).GetString() );
	nSec   = _tstoi( strDate.Mid(12, 2).GetString() );

	// The upper date limit is January 18, 2038. For a wider range of dates, see COleDateTime.
	// CTime object and January 1, 1970.
	if (nYear  < 1971 || nYear  > 2037) return false;
	if (nMonth < 1    || nMonth > 12)   return false;
	if (nDay   < 1    || nDay   > 31)   return false;
	if (nHour  < 0    || nHour  > 23)   return false;
	if (nMin   < 0    || nMin   > 59)   return false;
	if (nSec   < 0    || nSec   > 59)   return false;

	CTime tFireTime(nYear, nMonth, nDay, nHour, nMin, nSec);

	__time64_t diffTime = tFireTime.GetTime() - tNow.GetTime();

	if (diffTime > 0)
		return true;
	else
		return false;
}

}