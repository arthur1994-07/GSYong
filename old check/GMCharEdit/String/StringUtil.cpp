#include "StdAfx.h"
#pragma warning(disable:4995)
#include <strsafe.h>
#include <oledb.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/crc.hpp>
#include <boost/filesystem.hpp> 
#include <boost/scoped_array.hpp>
#include <boost/lexical_cast.hpp>

#include "../../=CryptoPP/hex.h"
#include "../../=CryptoPP/filters.h"
#include "../../=CryptoPP/base64.h"
#include "../../=CryptoPP/sha.h"
#include "../../=CryptoPP/iterhash.h"
#include "../../=CryptoPP/crc.h"
//#include "../CoreCommon.h"
#include "../Hash/CHash.h"
#include "./StringFormat.h"
#include "./StringUtil.h"

#include "../DebugInclude.h"

namespace sc {
    namespace string {

//! Convert string to wstring
void ansiToUnicode(const char* szAscii, wchar_t* szUnicode, int nLen)
{	
	MultiByteToWideChar(
		CP_ACP,
		0,
		szAscii,
		-1,
		szUnicode,
		nLen );
}

//! Convert a Unicode string to an ASCII string
void unicodeToAnsi(const wchar_t* szUnicode, char* szAscii, int nLen)
{	
	WideCharToMultiByte(
		CP_ACP,
		0,
		szUnicode,
		-1,
		szAscii,
		nLen,
		NULL,
		NULL );
}

//! Convert UTF8 to Unicode
void utf8ToUnicode(const char* szUtf8, wchar_t* szUnicode, int nLen)
{
	MultiByteToWideChar(
		CP_UTF8,
		0,
		szUtf8,
		-1,
		szUnicode,
		nLen );
}

//! Convert Unicode to UTF8
void unicodeToUtf8(const wchar_t* szUnicode, char* szUtf8, int nLen)
{
	WideCharToMultiByte(
		CP_UTF8,
		0,
		szUnicode,
		-1,
		szUtf8,
		nLen,
		NULL,
		NULL );
}

//! Convert multibyte to Unicode
std::wstring multiToWide(const char* str, int len, UINT CodePage)
{
	// 필요버퍼크기를 구한다.
	int bufSize = MultiByteToWideChar( CodePage, 0, str, len, NULL, 0 );
	int strSize = bufSize;
	// len 이 -1 일 경우 마지막 터미널 문자의 개수까지 포함되기때문에
	// 실제 문자열 길이는 1을 뺀다.
	if ( len == -1 )
		strSize = bufSize-1;
	// 실제 문자열 길이가 0 보다 작을 경우 그냥 종료
	if ( strSize <= 0 )
		return std::wstring();
	std::vector<wchar_t> wsv(bufSize, 0);
	if ( MultiByteToWideChar(CodePage, 0, str,len, &wsv[0], bufSize) == 0 )
	{
		return std::wstring();
	}
	else
	{
		return std::wstring(wsv.begin(), wsv.begin()+strSize);
	}
}

std::string wideToMulti(const wchar_t* str, int len, UINT CodePage)
{
	// 필요버퍼크기를 구한다.
	int bufSize = WideCharToMultiByte( CodePage, 0, str, len, NULL, 0, NULL, NULL );
	int strSize = bufSize;
	// len 이 -1 일 경우 마지막 터미널 문자의 개수까지 포함되기때문에
	// 실제 문자열 길이는 1을 뺀다.
	if( len == -1 )
		strSize = bufSize-1;
	// 실제 문자열 길이가 0 보다 작을 경우 그냥 종료
	if( strSize <= 0 )
		return std::string();
	std::vector<char> msv(bufSize, 0);
	if ( WideCharToMultiByte( CodePage, 0, str, len, &msv[0], bufSize, NULL, NULL ) == 0 )
	{
		return std::string();
	}
	else
	{
		return std::string(msv.begin(), msv.begin()+strSize);
	}
}

int getUUID(char* pszUUID, size_t cchUUID)
{
	if (pszUUID == NULL)
        return UUID_ERROR;

	UUID id;
	if (RPC_S_OK == ::UuidCreateSequential(&id))
	{
		TCHAR* tmpid = NULL;
#ifdef UNICODE
		RPC_STATUS rpcResult = ::UuidToString(&id, reinterpret_cast<unsigned short**>(&tmpid));
		if (RPC_S_OUT_OF_MEMORY != rpcResult)
		{
			StringCchCopy(pszUUID, cchUUID, tmpid);
			::RpcStringFree(reinterpret_cast<unsigned short**>(&tmpid));
			return UUID_SUCCESS;
		}
#else
		RPC_STATUS rpcResult = ::UuidToString(&id, reinterpret_cast<unsigned char**>(&tmpid));
		if (RPC_S_OUT_OF_MEMORY != rpcResult)
		{
			StringCchCopyA(pszUUID, cchUUID, tmpid);
			::RpcStringFree(reinterpret_cast<unsigned char**>(&tmpid));
			return UUID_SUCCESS;
		}
#endif // UNICODE
	}	
	return UUID_ERROR;
}

int getUUID(UUID &_uuid)
{
	if (RPC_S_OK == ::UuidCreateSequential(&_uuid))
		return UUID_SUCCESS;
	else
		return UUID_ERROR;
}

int uuidToString(UUID &_uuid, TCHAR* pszUUID)
{
	if (!pszUUID)
		return UUID_ERROR;

	TCHAR* tmpid = NULL;
#ifdef UNICODE
	RPC_STATUS rpcResult = ::UuidToString(&_uuid, reinterpret_cast<unsigned short**>(&tmpid));
	if (RPC_S_OUT_OF_MEMORY != rpcResult)
	{
		StringCchCopy(pszUUID, UUID_LENGTH, tmpid);
		::RpcStringFree(reinterpret_cast<unsigned short**>(&tmpid));
		return UUID_SUCCESS;
	}
#else
	RPC_STATUS rpcResult = ::UuidToString(&_uuid, reinterpret_cast<unsigned char**>(&tmpid));
	if (RPC_S_OUT_OF_MEMORY != rpcResult)
	{
		StringCchCopy(pszUUID, UUID_LENGTH, tmpid);
		::RpcStringFree(reinterpret_cast<unsigned char**>(&tmpid));
		return UUID_SUCCESS;
	}
#endif // UNICODE
	return UUID_ERROR;
}

std::string uuidToString(UUID& _uuid)
{
	char* tmpid = NULL;
	char szUUID[UUID_LENGTH] = {0};

	RPC_STATUS rpcResult = ::UuidToString(&_uuid, reinterpret_cast<unsigned char**>(&tmpid));
	if (RPC_S_OUT_OF_MEMORY != rpcResult)
	{
		StringCchCopyA(szUUID, UUID_LENGTH, tmpid);
		::RpcStringFree(reinterpret_cast<unsigned char**>(&tmpid));
		return std::string(szUUID);
	}
	else
	{
		return std::string("");
	}
}

int stringToUUID(char* pszUUID, UUID &_uuid)
{	
#ifdef UNICODE
	if (RPC_S_OK == ::UuidFromString(reinterpret_cast<unsigned short*>(pszUUID), &_uuid))
	{
		return UUID_SUCCESS;
	}
#else
	std::string StrUuid(pszUUID);
	sc::string::trim(StrUuid, "{}");

	char szUuid[UUID_LENGTH] = {0};
	StringCchCopy(szUuid, UUID_LENGTH, StrUuid.c_str());

	if (RPC_S_OK == ::UuidFromString(reinterpret_cast<unsigned char*>(szUuid), &_uuid))
	{
		return UUID_SUCCESS;
	}
#endif
	return UUID_ERROR;	
}

// Generate Random String
// 사용하지 마시오! 너무 느림.
bool generateRandomString(
	CString& str,
	int Length,
	int Small,
	int Capital,
	int Numbers)
{
	CString SmallCharacters = _T("abcdefghijklmnopqrstuvwxyz");
	CString CapitalCharacters = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	CString NumbersSet = _T("0123456789");
	
	int Number = 26;
	//int Number2 = 10;
	int Choice = 4;
	int Flag = 0;
	int i = 0;

	if ((Small) || (Capital) || (Numbers))
	{		
		for (i=1;i<=Length; ++i)
		{
			while ((Choice>=4) || (Choice==0))
			{
				Flag = 0;
				while (Flag==0)
				{
					Choice = int (rand()/1000);
					if (Choice==Small)
					{
						Flag=1;
						break;
					}
					if (Choice==Capital)
					{
						Flag=1;
						break;
					}
					if (Choice==Numbers)
					{
						Flag=1;
						break;
					}
				}
			}
			switch (Choice)
			{
			case 1:
				{
					while (Number>=26) 
						Number  = int(rand()/1000);
					str.Insert(0,SmallCharacters.GetAt(Number));
					Number=26;
					Choice=4;
					break;
				}
			case 2:
				{
					while (Number>=26) 
						Number  = int(rand()/1000);
					str.Insert(0,CapitalCharacters.GetAt(Number));
					Number=26;
					Choice=4;
					break;
				}
			case 3:
				{
					while (Number>=10) 
						Number  = int(rand()/1000);
					str.Insert(0,NumbersSet.GetAt(Number));
					Number=26;
					Choice=4;
				}
			}
		}
		
	}
	else // exception
	{
		return false;
	}
	return true;
}

bool checkAdult(CString& strCheck, int nCheckAge)
{
	if ( strCheck.GetLength() < 6 )
		return false;

	CTime tNow( CTime::GetCurrentTime() ); // 현재시간

	int nYear, nMonth, nDay, nAge=0;

	// 2000 년 이후 출생자는 주민번호 첫번째 숫자가 0
	if ( _tstoi( strCheck.Mid(0, 1).GetString() ) == 0 )
	{	
		nYear  = 2000 + _tstoi( strCheck.Mid(0, 2).GetString() );
	}
	else // 2000 년 이전 출생자
	{
		nYear  = 1900 + _tstoi( strCheck.Mid(0, 2).GetString() );
	}

	nAge = (int) tNow.GetYear() - nYear;

	if (nAge >= nCheckAge) // 체크하려는 나이 이상일때
	{
		return true;
	}
	else if (nAge == (nCheckAge-1)) // 체크하려는 나이보다 작지만 생일이 지났는지 체크
	{
		nMonth = _tstoi(strCheck.Mid(2, 2).GetString());
		if (nMonth == 0) 
			return false;
		nDay   = _tstoi(strCheck.Mid(4, 2).GetString());
		if (nDay == 0) 
			return false;

		if ((nMonth <= tNow.GetMonth()) && (nDay <= tNow.GetDay()))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else // 체크하려는 나이보다 작음
	{
		return false;
	}
}

//! 주민등록번호 앞자리로 나이를 구한다.
WORD SsnHeadToAge(const std::string& SsnHead)
{
    if (SsnHead.length() < 6)
    {
        // 잘못된 주민등록번호는 15세로 간주한다.
        return 15;
    }

    CTime CurrentDate(CTime::GetCurrentTime()); // 현재시간
    
    std::string Year  = SsnHead.substr(0, 2);
    std::string Month = SsnHead.substr(2, 2);
    std::string Day   = SsnHead.substr(4, 2);

    int nYear  = boost::lexical_cast<int> (Year);
    int nMonth = boost::lexical_cast<int> (Month);
    int nDay   = boost::lexical_cast<int> (Day);

    if (nMonth < 1 || nMonth > 12)
        return 15;

    if (nDay < 1 || nDay > 31)
        return 15;

    // 주민등록번호 앞자리 만으로는 정확하게 체크할 수 없다.
    // 정확하게 체크하기 위해서는 주민등록번호 뒷자리가 필요하다.
    // 20 이하일때는 2000 년 이후에 출생한 것으로 생각한다.
    if (nYear < 20)
        nYear += 2000;
    else
        nYear += 1900;

	if (nYear < 1970)
		nYear = 1971;

	if (nYear >= 3000)
		nYear = 2999;

    CTime BirthDate(nYear, nMonth, nDay, 12, 30, 30);
    if (BirthDate >= CurrentDate)
    {
        // 잘못된 주민등록번호는 15세로 간주한다.
        return 15;
    }

	// 현재 나이에서 무조건 1살 뺀 다음에
	// 생일 지난 사람은 1살 더해준다.
	int LiveYear = CurrentDate.GetYear() - BirthDate.GetYear() - 1;

	if( BirthDate.GetMonth() < CurrentDate.GetMonth() )
	{
		LiveYear += 1;
	}
	else if( BirthDate.GetMonth() == CurrentDate.GetMonth() )
	{
		if( BirthDate.GetDay() <= CurrentDate.GetDay() )
			LiveYear += 1;
	}


	/*
	// 지금 기준으로 올해 지난 날짜
	CTime PassedDate(CurrentDate.GetYear(), 1, 1, 1, 1, 1);
	CTimeSpan passed_ts = CurrentDate - PassedDate;
	LONGLONG passed_days = passed_ts.GetDays();
    // Subtract 2 CTimes
    CTimeSpan ts = CurrentDate - BirthDate;
    LONGLONG LiveDays = ts.GetDays();

    //if (BirthDate.GetMonth() >= CurrentDate.GetMonth() && BirthDate.GetDay() > CurrentDate.GetDay())
        //LiveYear -= 1;

	LONGLONG LiveYear = ( LiveDays - passed_days ) / 365;
   */
    return static_cast<WORD> (LiveYear);   
}

EM_CHAR_NATION checkWCharNation(wchar_t wChar)
{
	// 0030 ~ 0039 숫자 
	if (wChar >= 0x0030 && wChar <= 0x0039 ) {
		return CHAR_NUMBER;
	}

	// 0041 ~ 005A 영문 대소문자
	// 0061 ~ 007A
	if ((wChar >= 0x0041 && wChar <= 0x005A ) ||
		(wChar >= 0x0061 && wChar <= 0x007A ) ) {
		return CHAR_ENGLISH;
	}

	// 0E00 ~ 0E7F Thai 타이어
	if ( wChar >= 0x0E00 && wChar <= 0x0E7F ) {
		return CHAR_THAI;
	}

	// 1950 ~ 197F Tai Le 타이 레 문자
	if ( wChar >= 0x1950 && wChar <= 0x197F ) {
		return CHAR_THAI;
	}

	// 1980 ~ 19DF New Tai Lue 새 타이 루에
	if ( wChar >= 0x1980 && wChar <= 0x19DF ) {
		return CHAR_THAI;
	}

	// 20000 ~ 2A6DF CJK Unified Ideographs Extension B 한중일 통합 한자 확장-B
	
	// 2190 21FF Arrows / 특수문자
	// 2500 257F Box Drawing / 특수문자
	// 2580 259F Block Elements / 특수문자
	// 25A0 25FF Geometric Shapes / 특수문자
	// 2600 26FF Miscellaneous Symbols / 특수문자
	// 2700 27BF Dingbats / 특수문자
	// 2800 28FF Braille Patterns / 특수문자
	// 2B00 2BFF Miscellaneous Sysbols and Arrows / 특수문자
	if ( (wChar >= 0x2190 && wChar <= 0x21FF) ||
		 (wChar >= 0x2500 && wChar <= 0x257F) ||
		 (wChar >= 0x2580 && wChar <= 0x259F) ||
		 (wChar >= 0x25A0 && wChar <= 0x25FF) ||
		 (wChar >= 0x2600 && wChar <= 0x26FF) ||
		 (wChar >= 0x2700 && wChar <= 0x27BF) ||
		 (wChar >= 0x2800 && wChar <= 0x28FF) ||
		 (wChar >= 0x2B00 && wChar <= 0x2BFF) )
	{
		return CHAR_SYMBOL;
	}

	// 2F800 ~ 2FA1F CJK Compatibility Ideographs Supplement 한중일 호환용 한자 보충

	// 2E80 ~ 2EFF CJK Radicals Supplement 한중일 부수 보충
	if ( wChar >= 0x2E80 && wChar <= 0x2EFF ) {
		return CHAR_CHINA;
	}
	
	// 3040 ~ 309F 일본어 히라가나
	// 30A0 ~ 30FF 일본어 카타가나
	if ( (wChar >= 0x3040 && wChar <= 0x309F) || 
		 (wChar >= 0x30A0 && wChar <= 0x30FF) ) {
		return CHAR_JAPAN;
	}

	// 3200 32FF Enclosed CJK Letters and Months / 특수문자
	// 3300 33FF CJK Compatibility / 특수문자
	if ( (wChar >= 0x3200 && wChar <= 0x32FF) ||
		 (wChar >= 0x3300 && wChar <= 0x3fFF) )
	{
		return CHAR_SYMBOL;
	}

	// 4E00 ~ 9FAF 한중일 통합 한자
	if ( wChar >= 0x4E00 && wChar <= 0x9FAF ) {
		return CHAR_CHINA;
	}	
	
	// AC00 ~ D7AF 한글
	if ( wChar >= 0xAC00 && wChar <= 0xD7AF ) {
		return CHAR_HANGUL;
	}

	// F900 ~ FAFF CJK Compatibility Ideographs 한중일 호환용 한자
	if ( wChar >= 0xF900 && wChar <= 0xFAFF ) {
		return CHAR_CHINA;
	}		

	// '.' 002E
	if ( wChar == 0x002E ) {
		return CHAR_DOT;
	}
	// '_' 005F
	if ( wChar == 0x005F )	{
		return CHAR_LOWLINE;
	}
	return CHAR_UNKNOW;
}

//! 문자열이 숫자와 영어로만 구성되어 있는지 체크한다.
bool checkEnglishOrNumber(const std::string& ChkStr)
{
    std::wstring strUnicode = ansiToUnicode(ChkStr);
    return checkEnglishOrNumber(strUnicode);
}

bool checkEnglishOrNumber(const std::wstring& ChkStr)
{
	return checkEnglishOrNumber(ChkStr.c_str(), ChkStr.length());
}

bool checkEnglishOrNumber(const wchar_t* pChar, size_t nLength)
{
	EM_CHAR_NATION emChar;
	for ( size_t n=0; n<nLength; ++n )
	{
		emChar = checkWCharNation(*(pChar+n));
		if (CHAR_ENGLISH != emChar && CHAR_NUMBER != emChar)
			return false;
	}
	return true;
}

bool checkKoreanOrNumber(const std::string& ChkStr)
{
    std::wstring strUnicode = ansiToUnicode(ChkStr);
    return checkKoreanOrNumber(strUnicode);
}

bool checkKoreanOrNumber(const std::wstring& ChkStr)
{
    return checkKoreanOrNumber(ChkStr.c_str(), ChkStr.length());
}

bool checkKoreanOrNumber(const wchar_t* pChar, size_t nLength)
{
    EM_CHAR_NATION emChar;
    for ( size_t n=0; n<nLength; ++n )
    {
        emChar = checkWCharNation(*(pChar+n));
        if (CHAR_HANGUL != emChar && CHAR_NUMBER != emChar)
            return false;
    }
    return true;
}

bool checkEnglish(const std::wstring& ChkStr)
{
	return checkEnglish( ChkStr.c_str(), ChkStr.length() );
}

bool checkEnglish(const wchar_t* pChar, size_t nLength)
{	
	for ( size_t n=0; n<nLength; ++n ) {
		if ( (CHAR_ENGLISH != checkWCharNation(*(pChar+n))) ) {
			return false;
		}
	}
	return true;
}

//! 태국에서 허용하는 문자인지 검사한다.
bool checkThaiName(const std::wstring& ChkStr)
{
    return checkThaiName(ChkStr.c_str(), ChkStr.length());
}

bool checkThaiName(const wchar_t* pChar, size_t nLength)
{
    for (size_t n=0; n<nLength; ++n)
    {
        EM_CHAR_NATION emChar = checkWCharNation(*(pChar+n));
        if (emChar != CHAR_NUMBER &&
            emChar != CHAR_ENGLISH &&
            emChar != CHAR_THAI)
        {
            return false;
        }
    }
    return true;
}

bool checkNumber( const std::wstring& ChkStr )
{
	return checkNumber( ChkStr.c_str(), ChkStr.length() );
}

bool checkNumber( const wchar_t* pChar, size_t nLength )
{
	for (size_t n=0; n<nLength; ++n)
    {
		if (CHAR_NUMBER != checkWCharNation(*(pChar+n)))
			return false;
	}
	return true;
}

//! 특수문자 . , 등이 포함되어 있는가?
bool haveSymbolCha( const std::wstring& strName )
{
	return haveSymbolCha( strName.c_str(), strName.length() );
}

bool haveSymbolCha( const wchar_t* pChar, size_t nLength )
{
	for (size_t n=0; n<nLength; ++n)
	{
		EM_CHAR_NATION emChar = checkWCharNation(*(pChar+n));
		if ( (CHAR_SYMBOL  == emChar) ||
			 (CHAR_DOT     == emChar) ||
			 (CHAR_LOWLINE == emChar) )
		{
			return true;
		}
	}
	return false;
}

//! 사용자 ID 가 올바른지 체크한다.
//! 1. 영문/숫자 로만 구상되어야 한다.
bool goodIdName( const wchar_t* pChar, size_t nLength )
{
	return checkEnglishOrNumber( pChar, nLength );
}

//! 파일이름이 올바른지 검사한다.
//! 파일이름 규칙
//! 1. 영문, 숫자, _, . 으로만 구성되어야 한다.
//! 2. 소문자로만 구성되어야 한다.
bool goodFileName( const wchar_t* pChar, int nLength )
{
	for (int n=0; n<nLength; ++n) {
		EM_CHAR_NATION emChar = checkWCharNation(*(pChar+n));
		if ( (CHAR_ENGLISH  == emChar) ||
			 (CHAR_NUMBER   == emChar) ||
			 (CHAR_LOWLINE  == emChar) || 
			 (CHAR_DOT      == emChar) ) 
		{
		}
		else
		{
			return false;
		}
	}
	return true;
}

//! 한국에 맞는 캐릭터 이름인지 검사한다.
//! 캐릭터 이름 규칙
//! 1. 한글, 영문, 숫자
bool goodChaNameKorea(const std::string& strName)
{
    std::wstring UnicodeStr = ansiToUnicode(strName);
    return goodChaNameKorea(UnicodeStr);
}

bool goodChaNameKorea(const std::wstring& strName)
{
	return goodChaNameKorea(strName.c_str(), (int) strName.length());
}

bool goodChaNameKorea(const wchar_t* pChar, int nLength)
{
    if (checkEnglishOrNumber(pChar, nLength))
        return true;

    if (checkKoreanOrNumber(pChar, nLength))
        return true;

    return false;	
}

bool goodChaNameThailand(const std::wstring& strName)
{
    return goodChaNameThailand(strName.c_str(), (int) strName.length());
}

bool goodChaNameThailand(const wchar_t* pChar, int nLength)
{
    for (int n=0; n<nLength; ++n)
    {
        EM_CHAR_NATION emChar = checkWCharNation(*(pChar+n));
        if ((CHAR_THAI    == emChar) ||
            (CHAR_ENGLISH == emChar) ||
            (CHAR_NUMBER  == emChar))
        {
        }
        else
        {
            return false;
        }
    }
    return true;
}

size_t lengthForGame( const std::wstring& strName )
{
	return lengthForGame( strName.c_str(), (int) strName.length() );
}

size_t lengthForGame( const wchar_t* pChar, int nLength )
{
	size_t Length=0;
	for (int n=0; n<nLength; ++n)
	{
		EM_CHAR_NATION emChar = checkWCharNation(*(pChar+n));
		if ( (CHAR_ENGLISH  == emChar) || (CHAR_NUMBER   == emChar) )
			Length++;
		else
			Length += 2;
	}
	return Length;
}

void getFilePathAndName(
	const std::wstring& strFullPath IN,
	std::wstring& strPath OUT,
	std::wstring& strFullFileName OUT,
	std::wstring& strFileExt OUT)
{
    // 4가지 경우 모두 제대로 동작하도록 검정했음.
    //std::wstring FullPath(L"c:\\game\\MarMarOnlineAlpha\\data\\animation\\test.bin");
    //std::wstring FullPath(L"c:\\game\\MarMarOnlineAlpha\\data\\animation\\");
    //std::wstring FullPath(L"c:\\game\\MarMarOnlineAlpha\\data\\animation");
    //std::wstring FullPath(L"test.bin");

    boost::filesystem::wpath p(strFullPath);

    strFullFileName = p.filename().wstring();
    // 파일 이름이 없을경우 . 만 리턴된다. 이럴경우 clear 한다.
    if (strFullFileName == L".")
        strFullFileName.clear();

    strFileExt = p.extension().wstring();
    // 확장자에 . 이 포함된다. 따라서 제거한다.
    boost::algorithm::trim_if(strFileExt, boost::algorithm::is_any_of(L"."));
    
    // 전체 path 에서 파일이름을 지운다.
    p.remove_filename();
    strPath = p.wstring();
    strPath = boost::algorithm::trim_right_copy_if(strPath, boost::algorithm::is_any_of(L"\\"));   
}

void getFilePathAndName(
    const std::string& strFullPath IN,
    std::string& strPath OUT,
    std::string& strFullFileName OUT,
    std::string& strFileExt OUT)
{
    // 4가지 경우 모두 제대로 동작하도록 검정했음.
    //std::string FullPath(L"c:\\game\\MarMarOnlineAlpha\\data\\animation\\test.bin");
    //std::string FullPath(L"c:\\game\\MarMarOnlineAlpha\\data\\animation\\");
    //std::string FullPath(L"c:\\game\\MarMarOnlineAlpha\\data\\animation");
    //std::string FullPath(L"test.bin");

    boost::filesystem::path p(strFullPath);

    strFullFileName = p.filename().string();
    // 파일 이름이 없을경우 . 만 리턴된다. 이럴경우 clear 한다.
    if (strFullFileName == ".")
        strFullFileName.clear();

    strFileExt = p.extension().string();
    // 확장자에 . 이 포함된다. 따라서 제거한다.
    boost::algorithm::trim_if(strFileExt, boost::algorithm::is_any_of("."));

    // 전체 path 에서 파일이름을 지운다.
    p.remove_filename();
    strPath = p.string();
    strPath = boost::algorithm::trim_right_copy_if(strPath, boost::algorithm::is_any_of("\\"));   
}

void getFilePathAndName(
	const CString& strFullPath IN,
	CString& strPath OUT,
	CString& strFullFileName OUT,
	CString& strFileExt OUT )
{
	// 먼저 \\ 를 찾아서 path 의 끝을 찾아낸다.
	DWORD dwPathFind = strFullPath.ReverseFind( _T('\\') );
	if (dwPathFind != -1)
	{
		// path 정보가 있다.
		// path 정보 세팅
		strPath = strFullPath.Left( dwPathFind );
		// file 이름 세팅 파일명+ '.' + 확장자
		strFullFileName = strFullPath.Mid(
							dwPathFind+1,
							strFullPath.GetLength() - dwPathFind-1 );
	}
	else
	{
		// path 정보가 없다.
		// path 정보 세팅
		strPath = _T("");
		strFullFileName = strFullPath;
	}

	// 확장자를 알아낸다.
	DWORD dwExtFind = strFullFileName.ReverseFind( _T('.') );
	if (dwExtFind != -1)
	{
		// 확장자가 있다.
		// 확장자를 알아낸다.
		strFileExt = strFullFileName.Mid(
			dwExtFind+1, strFullFileName.GetLength()-dwExtFind-1);
	}
	else
	{
		// 확장자가 없다.
		strFileExt = _T("");
	}
}


//! 전체 경로를 주면
//! 확장자를 포함한 파일 이름만 돌려준다
void getFullFileName(
	const std::string& strFullPath IN,
	std::string& strFullFileName OUT )
{
	std::string strPath;
	std::string strFileExt;
	getFilePathAndName(
		strFullPath,
		strPath,
		strFullFileName,
		strFileExt );
}

//! 전체 경로를 주면
//! 확장자를 포함한 파일 이름만 돌려준다
void getFullFileName(
	const CString& strFullPath IN,
	CString& strFullFileName OUT )
{
	CString strPath;
	CString strFileExt;
	getFilePathAndName(
		strFullPath,
		strPath,
		strFullFileName,
		strFileExt );
}

//! 파일 확장자를 돌려준다.
CString getFileExt( const LPCTSTR szFileName )
{
	CString strFullFileName( szFileName );
	CString strFileExt;
	// 확장자를 알아낸다.
	DWORD dwExtFind = strFullFileName.ReverseFind( _T('.') );
	if (dwExtFind != -1)
	{
		// 확장자가 있다.
		// 확장자를 알아낸다.
		strFileExt = strFullFileName.Mid(
			dwExtFind+1,
			strFullFileName.GetLength()-dwExtFind-1);
	}
	else
	{
		// 확장자가 없다.
		strFileExt = _T("");
	}

	return strFileExt;
}

std::wstring utf8ToUnicode( const std::string& Utf8 )
{
	return utf8ToUnicode( Utf8.c_str(), (int) Utf8.length() );
}

//! Convert UTF8 to Unicode
//! 여기서 유니코드는 MS OS 에서 말하는 유니코드이다. UTF16
std::wstring utf8ToUnicode( const char* szUtf8, int nLen )
{
	// 필요버퍼크기를 구한다.
	int bufSize = MultiByteToWideChar( CP_UTF8, 0, szUtf8, nLen, NULL, 0 );
	int strSize = bufSize;
	// len 이 -1 일 경우 마지막 터미널 문자의 개수까지 포함되기때문에
	// 실제 문자열 길이는 1을 뺀다.
	if ( nLen == -1 )
		strSize = bufSize-1;
	// 실제 문자열 길이가 0 보다 작을 경우 그냥 종료
	if ( strSize <= 0 )
		return std::wstring();
	std::vector<wchar_t> wsv(bufSize);
	if ( MultiByteToWideChar(CP_UTF8, 0, szUtf8, nLen, &wsv[0], bufSize) == 0 )
	{
		return std::wstring();
	}
	else
	{
		return std::wstring(wsv.begin(), wsv.begin()+strSize);
	}
	//return multiToWide( szUtf8,	nLen, CP_UTF8 );
}

std::string unicodeToUtf8( const std::wstring& StrW )
{
	return unicodeToUtf8( StrW.c_str(), (int) StrW.length() );
}

std::string unicodeToUtf8( const wchar_t* szUnicode, int nLen )
{
	// 필요버퍼크기를 구한다.
	int bufSize = WideCharToMultiByte( CP_UTF8, 0, szUnicode, nLen, NULL, 0, NULL, NULL );
	int strSize = bufSize;
	// len 이 -1 일 경우 마지막 터미널 문자의 개수까지 포함되기때문에
	// 실제 문자열 길이는 1을 뺀다.
	if( nLen == -1 )
		strSize = bufSize-1;
	// 실제 문자열 길이가 0 보다 작을 경우 그냥 종료
	if( strSize <= 0 )
		return std::string();
	std::vector<char> msv(bufSize);
	if( WideCharToMultiByte( CP_UTF8, 0, szUnicode, nLen, &msv[0], bufSize, NULL, NULL ) == 0 )
	{
		return std::string();
	}
	else
	{
		return std::string(msv.begin(), msv.begin()+strSize);
	}
	//return wideToMulti( szUnicode, nLen, CP_UTF8 );
}

//! Convert string to wstring
std::wstring ansiToUnicode( const std::string& StrW )
{
	return ansiToUnicode( StrW.c_str(), (int) StrW.length() );
}

//! Convert string to wstring
//! 여기서 유니코드는 MS OS 에서 말하는 유니코드이다. UTF16
std::wstring ansiToUnicode( const char* szAscii, int nLen )
{
	// 필요버퍼크기를 구한다.
	int bufSize = MultiByteToWideChar( CP_ACP, 0, szAscii, nLen, NULL, 0 );
	int strSize = bufSize;
	// len 이 -1 일 경우 마지막 터미널 문자의 개수까지 포함되기때문에
	// 실제 문자열 길이는 1을 뺀다.
	if ( nLen == -1 )
		strSize = bufSize-1;
	// 실제 문자열 길이가 0 보다 작을 경우 그냥 종료
	if ( strSize <= 0 )
		return std::wstring();
	std::vector<wchar_t> wsv(bufSize);
	if ( MultiByteToWideChar(CP_ACP, 0, szAscii, nLen, &wsv[0], bufSize) == 0 )
		return std::wstring();
	else
		return std::wstring(wsv.begin(), wsv.begin()+strSize);
	/*
	size_t lenAscii = strlen(szAscii);

	if ( lenAscii <= 0 )
		return std::wstring();

	wchar_t* str = new wchar_t[lenAscii + 1];

	ZeroMemory( (void *) str, sizeof(wchar_t) * (lenAscii + 1));

	if ( 0 == MultiByteToWideChar(
				CP_ACP,
				MB_PRECOMPOSED,
				szAscii,
				-1,
				str,
				(int) lenAscii + 1 ) )
	{
		delete str;
		return std::wstring();
	}
	else
	{
		std::wstring result(str);
		delete str;
		return result;
	}
	*/
}

//! Convert a Unicode string to an ASCII string
std::string unicodeToAnsi( const std::wstring& strW )
{
	return unicodeToAnsi( strW.c_str(), (int) strW.length() );
}

//! Convert string to wstring
//! 여기서 유니코드는 MS OS 에서 말하는 유니코드이다. UTF16
std::string unicodeToAnsi(
	const wchar_t* szUnicode,
	int nLen )
{
	// 필요버퍼크기를 구한다.
	int bufSize = WideCharToMultiByte( CP_ACP, 0, szUnicode, nLen, NULL, 0, NULL, NULL );
	int strSize = bufSize;
	// len 이 -1 일 경우 마지막 터미널 문자의 개수까지 포함되기때문에
	// 실제 문자열 길이는 1을 뺀다.
	if( nLen == -1 )
		strSize = bufSize-1;
	// 실제 문자열 길이가 0 보다 작을 경우 그냥 종료
	if( strSize <= 0 )
		return std::string();
	std::vector<char> msv(bufSize);
	if( WideCharToMultiByte( CP_ACP, 0, szUnicode, nLen, &msv[0], bufSize, NULL, NULL ) == 0 )
		return std::string();
	else
		return std::string(msv.begin(), msv.begin()+strSize);
	
	/*
	size_t lenUnicode = wcslen( szUnicode );
	if ( lenUnicode <= 0 )
		return std::string();
	
	char* str = new char[lenUnicode + 1];

	ZeroMemory( (void *) str, lenUnicode + 1 );
	mbstowcs
	if ( 0 == WideCharToMultiByte(
				CP_ACP,
				0,
				szUnicode,
				-1,
				str,
				(int) lenUnicode + 1,
				NULL,
				NULL ) )
	{
		delete str;
		return std::string();
	}
	else
	{
		std::string result(str);
		delete str;
		return result;
	}
	*/
}



//
//mjeon.utf-8
//
std::string ansi_to_utf8(std::string& ansi)
{
	int len = (int)ansi.size() + 1;
	
	boost::scoped_array<WCHAR> unicode(new WCHAR[len]); // WCHAR * len = char * len * 2;
	boost::scoped_array<char>  utf8(new char[len*2]);	// result of utf8 will not exceed the twice of the size of ansi(multibyte).

	::MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), -1, unicode.get(), len/*cch*/);
	::WideCharToMultiByte(CP_UTF8, 0, unicode.get(), -1, utf8.get(), len*2/*cb*/, NULL, NULL);
	
	return std::string(utf8.get());
}


std::string utf8_to_ansi(std::string &utf8)
{
	int len = (int)utf8.size() + 1;

	boost::scoped_array<WCHAR> unicode(new WCHAR[len]);	// WCHAR * len = char * len * 2;
	boost::scoped_array<char>  ansi(new char[len*2]);	// result of ansi(multibyte) will not exceed the twice of the size of utf8.

	::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, unicode.get(), len/*cch*/);
	::WideCharToMultiByte(CP_ACP, 0, unicode.get(), -1, ansi.get(), len*2/*cb*/, NULL, NULL);

	return std::string(ansi.get());
}


std::string utf8_to_ansi(char *utf8, int len)	//Overloaded function uses char-array
{
	len = len + 1; //for NULL character for the case of the len is just string length except NULL.

	boost::scoped_array<WCHAR> unicode(new WCHAR[len]);	// WCHAR * len = char * len * 2;
	boost::scoped_array<char>  ansi(new char[len*2]);	// result of ansi(multibyte) will not exceed the twice of the size of utf8.
	
	::MultiByteToWideChar(CP_UTF8, 0, utf8, len, unicode.get(), len/*cch*/);
	::WideCharToMultiByte(CP_ACP, 0, unicode.get(), -1, ansi.get(), len*2/*cb*/, NULL, NULL);

	return std::string(ansi.get());
}



//! Byte Buffer 를 Hex Encoding 한다.
std::string HexEncodeA( BYTE* pBuffer, size_t nLength )
{
	CryptoPP::HexEncoder encoder;
	std::string output;
	encoder.Attach( new CryptoPP::StringSink( output ) );
	encoder.Put(
		pBuffer,
		nLength );
	encoder.MessageEnd();
	return output;		
}

std::wstring HexEncodeW( BYTE* pBuffer, size_t nLength )
{
	std::string Encoded = HexEncodeA( pBuffer, nLength );
	return ansiToUnicode( Encoded );
}

std::string HexDecodeA( const std::string& strSrc )
{
	// Cipher Text Sink
	std::string DecodeText;

	// Hex Decode
	CryptoPP::HexDecoder decoder;
	decoder.Attach( new CryptoPP::StringSink( DecodeText ) );
	decoder.Put(
		(byte*) strSrc.c_str(),
		strSrc.size() );
	decoder.MessageEnd();

	return DecodeText;
}

std::wstring HexDecodeW( const std::wstring& strSrc )
{
	std::string strSrcA = unicodeToAnsi( strSrc.c_str(), (int) strSrc.size() );

	// Cipher Text Sink
	std::string DecodeText;

	// Hex Decode
	CryptoPP::HexDecoder decoder;
	decoder.Attach( new CryptoPP::StringSink( DecodeText ) );
	decoder.Put(
		(byte*) strSrcA.c_str(),
		strSrcA.size() );
	decoder.MessageEnd();

	return ansiToUnicode( DecodeText );
}

std::string Base64EncodeA( BYTE* pBuffer, size_t BufSize )
{
	std::string Encoded;
	CryptoPP::StringSource(
		pBuffer,
		BufSize,
		true,
		new CryptoPP::Base64Encoder(
		new CryptoPP::StringSink( Encoded ) )  );
	return Encoded;
}

std::wstring Base64EncodeW( BYTE* pBuffer, size_t BufSize )
{
	std::string Encoded = Base64EncodeA( pBuffer, BufSize );
	return ansiToUnicode( Encoded );
}

std::string Base64DecodeA( BYTE* pBuffer, size_t BufSize )
{
	std::string Decoded;
	CryptoPP::StringSource(
		pBuffer,
		BufSize,
		true,
		new CryptoPP::Base64Decoder(
		new CryptoPP::StringSink( Decoded ) )  );
	return Decoded;
}

std::wstring Base64DecodeW( BYTE* pBuffer, size_t BufSize )
{
	std::string Decoded = Base64DecodeA( pBuffer, BufSize );
	return ansiToUnicode( Decoded );
}

void trim( std::wstring& strSrc, const wchar_t* TrimStr )
{
	boost::algorithm::trim_if(
		strSrc,
		boost::algorithm::is_any_of(TrimStr) );
}

void trimLeft( std::wstring& strSrc, const wchar_t* TrimStr )
{
	boost::algorithm::trim_left_if(
		strSrc,
		boost::algorithm::is_any_of(TrimStr) );
}

void trimRight( std::wstring& strSrc, const wchar_t* TrimStr )
{
	boost::algorithm::trim_right_if(
		strSrc,
		boost::algorithm::is_any_of(TrimStr) );
}

void trim( std::string& strSrc, const char* TrimStr )
{
	boost::algorithm::trim_if(
		strSrc,
		boost::algorithm::is_any_of(TrimStr) );
}

void trimLeft( std::string& strSrc, const char* TrimStr )
{
	boost::algorithm::trim_left_if(
		strSrc,
		boost::algorithm::is_any_of(TrimStr) );
}

void trimRight( std::string& strSrc, const char* TrimStr )
{
	boost::algorithm::trim_right_if(
		strSrc,
		boost::algorithm::is_any_of(TrimStr) );
}

void replace(std::wstring& strSrc, const std::wstring& SearchStr, const std::wstring& NewStr)
{
    replace_all(strSrc, SearchStr, NewStr);
}

void replace(std::string& strSrc, const std::string& SearchStr, const std::string& NewStr)
{
    replace_all(strSrc, SearchStr, NewStr);
}

void replace_all( std::wstring& strSrc, const std::wstring& SearchStr, const std::wstring& NewStr )
{
	// std::wstring strTemp = abcd abcd efg;
	// replace_all( strTemp, L"abcd", "min" );
	// abcd abcd xkxkxkxkdfsdf -> min min efg
	boost::replace_all( strSrc, SearchStr, NewStr );
}

void replace_all(std::string& strSrc, const std::string& SearchStr, const std::string& NewStr)
{
    // std::string strTemp = abcd abcd efg;
    // replace_all( strTemp, L"abcd", "min" );
    // abcd abcd xkxkxkxkdfsdf -> min min efg
    boost::replace_all(strSrc, SearchStr, NewStr);
}

void replace_first( std::wstring& strSrc, const std::wstring& SearchStr, const std::wstring& NewStr )
{
	// std::wstring strTemp = abcd abcd efg;
	// replace_firat( strTemp, L"abcd", "min" );
	// abcd abcd xkxkxkxkdfsdf -> min abcd efg
	boost::replace_first( strSrc, SearchStr, NewStr );
}

void replace_first(std::string& strSrc, const std::string& SearchStr, const std::string& NewStr)
{
    // std::string strTemp = abcd abcd efg;
    // replace_firat( strTemp, L"abcd", "min" );
    // abcd abcd xkxkxkxkdfsdf -> min abcd efg
    boost::replace_first(strSrc, SearchStr, NewStr);
}

void replace_last( std::wstring& strSrc, const std::wstring& SearchStr, const std::wstring& NewStr )
{
	// std::wstring strTemp = abcd abcd efg;
	// replace_last( strTemp, L"abcd", "min" );
	// abcd abcd xkxkxkxkdfsdf -> abcd min efg
	boost::replace_last( strSrc, SearchStr, NewStr );	
}

void replace_last(std::string& strSrc, const std::string& SearchStr, const std::string& NewStr)
{
    // std::string strTemp = abcd abcd efg;
    // replace_last( strTemp, L"abcd", "min" );
    // abcd abcd xkxkxkxkdfsdf -> abcd min efg
    boost::replace_last(strSrc, SearchStr, NewStr);
}

void replaceForDb(std::wstring& strSrc)
{
    replace_all(strSrc, L"'", L"''");
}

void replaceForDb(std::string& strSrc)
{
    replace_all(strSrc, "'", "''");
}

std::string GetCurrentTime()
{
    CTime t(CTime::GetCurrentTime());
    DBTIMESTAMP ts;
    t.GetAsDBTIMESTAMP(ts);
    std::string ReturnStr(
        sc::string::format(
            "%1%-%2%-%3% %4%:%5%:%6%",
            ts.year, ts.month, ts.day, ts.hour, ts.minute, ts.second));
    return ReturnStr;
}

//! Unicode string -> Clipboard
bool stringToClipboard(const std::string& strSrc)
{
	SIZE_T MemoryLength = sizeof(char) * (strSrc.size()+1);
	HGLOBAL hGLOBAL = ::GlobalAlloc( GMEM_ZEROINIT|GMEM_MOVEABLE, MemoryLength );
	if ( hGLOBAL == NULL )
	{
		::AfxMessageBox("Clipboard GlobalAlloc error");
		return false;
	}

	LPVOID pBUFFER = ::GlobalLock( hGLOBAL );
	if ( pBUFFER )
	{		
		::StringCchCopyA((char*) pBUFFER, strSrc.size()+1, strSrc.c_str() );
		::GlobalUnlock(hGLOBAL);
	}
	// ------------------------------------------------------------------------

	if ( !OpenClipboard(NULL) )
	{
		::AfxMessageBox("Cannot open the Clipboard");
		::GlobalFree(hGLOBAL);
		return false;
	}

	// Remove the current Clipboard contents  
	if( !::EmptyClipboard() )
	{
		::AfxMessageBox("Cannot empty the Clipboard");
		::CloseClipboard();
		::GlobalFree(hGLOBAL);
		return false;
	}

	//if ( ::SetClipboardData( CF_UNICODETEXT, hGLOBAL ) == NULL )
    if (::SetClipboardData(CF_TEXT, hGLOBAL) == NULL)
	{
		::AfxMessageBox("Unable to set Clipboard data");    
		::CloseClipboard();
		::GlobalFree(hGLOBAL);
		return false;
	}
	::CloseClipboard();
	return true;
}

bool stringToClipboard(const std::wstring& strSrc)
{
	if( TRUE == OpenClipboard( NULL ) )
	{
		if( FALSE == EmptyClipboard() )
		{
			CloseClipboard();
			return false;
		}

		size_t nLength = wcslen( strSrc.c_str() );
		size_t nByteOfBuffer = (nLength + 1) * sizeof( wchar_t );

		HGLOBAL hGlobal = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT,  nByteOfBuffer );
		if( NULL == hGlobal )
		{
			CloseClipboard();
			return false;
		}

		wchar_t* pBuffer = (wchar_t*) GlobalLock( hGlobal );
		if( NULL == pBuffer )
		{
			GlobalFree( hGlobal );
			hGlobal = NULL;
		}

		wcscpy_s( pBuffer, nLength + 1, strSrc.c_str() );
		SetClipboardData( CF_UNICODETEXT, hGlobal );

		GlobalUnlock( hGlobal );
		CloseClipboard();

		return true;
	}

	return false;
}

//! Clipboard -> Unicode string
bool clipboardToString(HWND hWnd, std::string& strTar)
{
	HANDLE hResult = NULL;

	if (::OpenClipboard(hWnd))
	{
		//hResult = ::GetClipboardData( CF_UNICODETEXT );
		hResult = ::GetClipboardData(CF_TEXT);
		if (hResult == NULL)
		{
			//_showLastError("GetClipboardData");
			::CloseClipboard();
			return false;
		}		


	}

	LPVOID pBUFFER = ::GlobalLock(hResult);
	if (!pBUFFER)
	{
		::CloseClipboard();
		return false;
	}

	char szClipboard[MAX_PATH] = {0};

	::StringCchCopyA(szClipboard, MAX_PATH, (const char*) pBUFFER);

	::GlobalUnlock(hResult);
	::GlobalFree(hResult);    
	::CloseClipboard();

	strTar = szClipboard;

	return true;
}

//! Clipboard -> string
bool clipboardToString2(HWND hWnd, std::string& strTar)
{
    if (::OpenClipboard(hWnd))
    {
        if (::IsClipboardFormatAvailable(CF_TEXT) || ::IsClipboardFormatAvailable(CF_OEMTEXT))
        {
            HANDLE hResult = ::GetClipboardData(CF_TEXT);            
            char* pBUFFER = (char*) ::GlobalLock(hResult);
            CString Text = pBUFFER;
            strTar = Text.GetString();
            ::GlobalUnlock(hResult);            
        }
        ::CloseClipboard();
        return true;
    }
    return false;    
}

bool clipboardToString(HWND hWnd, std::wstring& strTar)
{
	if (::OpenClipboard(hWnd))
	{
		if (::IsClipboardFormatAvailable(CF_UNICODETEXT) || ::IsClipboardFormatAvailable(CF_OEMTEXT))
		{
			HANDLE hResult = ::GetClipboardData(CF_UNICODETEXT);            
			wchar_t* pBUFFER = (wchar_t*) ::GlobalLock(hResult);
			strTar = pBUFFER;
			::GlobalUnlock(hResult);            
		}
		::CloseClipboard();
		return true;
	}
	return false;    
}

//! 문자열을 소문자로 변환한다.
void lowerString( std::wstring& strConvert )
{
	std::transform(strConvert.begin(), strConvert.end(), strConvert.begin(), towlower);
}
void lowerString( std::string& strConvert )
{
	std::transform(strConvert.begin(), strConvert.end(), strConvert.begin(), towlower);
}

std::wstring lowerString2(const std::wstring& strConvert)
{
    std::wstring LowerStr(strConvert);
    std::transform(LowerStr.begin(), LowerStr.end(), LowerStr.begin(), towlower);
    return LowerStr;
}

//! 문자열을 대문자로 변환한다.
void upperString( std::wstring& strConvert )
{
	std::transform(
		strConvert.begin(),
		strConvert.end(),
		strConvert.begin(),
		towupper );
}
void upperString( std::string& strConvert )
{
	std::transform(
		strConvert.begin(),
		strConvert.end(),
		strConvert.begin(),
		towupper );
}

//! 문자열이 동일한지 검사한다. 대소문자 구분함
//! 문자열이 같으면 true, 틀리면 false
bool equal( const std::wstring& strL, const std::wstring& strR )
{
	if ( 0 == strL.compare( strR ) ) {
		return true;
	} else {
		return false;
	}
}

//! 문자열이 동일한지 검사한다. 대소문자 구분안함
//! 문자열이 같으면 true, 틀리면 false
bool equalNoCase( const std::wstring& strL, const std::wstring& strR )
{
	std::wstring strTempL( strL ); lowerString( strTempL );
	std::wstring strTempR( strR ); lowerString( strTempR );
	return equal( strTempL, strTempR );
}

TCHAR* lower_bound(
	TCHAR *pFirst, 
	DWORD dwCount, 
	TCHAR cFIND )
{
	for (; 0 < dwCount; )
	{
		// divide and conquer, find half that contains answer
		DWORD dwCount2 = dwCount / 2;
		TCHAR *pMiddle = pFirst;
		pMiddle += dwCount2;

		if ( *pMiddle < cFIND )
			pFirst = ++pMiddle, dwCount -= dwCount2 + 1;
		else
			dwCount = dwCount2;
	}

	return (pFirst);
}

bool binary_search( TCHAR *pFirst, DWORD dwCount, TCHAR cFIND )
{
	const TCHAR *pFound = lower_bound( pFirst, dwCount, cFIND );
	return (pFound!=(pFirst+dwCount) && !(cFIND < *pFound));
}

//! time 값을 string 으로 만든다.
std::string DateTimeFormat(const __time64_t& Time, bool bTimeInclude, bool bSecInclude)
{
	/*
	tm TimeTemp;
	_localtime64_s( &TimeTemp, &Time );
	wchar_t TempDest[MAX_PATH] = {0};

	if ( true == bTimeInclude )
		wcsftime( TempDest, MAX_PATH, L"%Y-%m-%d %H:%M:%S", &TimeTemp );
	else
		wcsftime( TempDest, MAX_PATH, L"%Y-%m-%d", &TimeTemp );

	return std::wstring( TempDest );
	*/

	/*
	COleDateTime ConvTime( Time );	
	CString strDate = ConvTime.Format(VAR_DATEVALUEONLY);
	if ( false == bTimeInclude )
		return std::wstring( strDate.GetString() );

	CString strTime = ConvTime.Format(VAR_TIMEVALUEONLY);

	CString strDateTime = strDate + L" " + strTime;

	return std::wstring( strDateTime.GetString() );
	*/

	CTime ConvTime( Time );
	if ( false == bTimeInclude )
	{
        return sc::string::format(
				"%1%-%2$02d-%3$02d",
				ConvTime.GetYear(),
				ConvTime.GetMonth(),
				ConvTime.GetDay() );
	}
	
	if ( false == bSecInclude )
	{
        return sc::string::format(
				"%1%-%2$02d-%3$02d %4$02d:%5$02d",
				ConvTime.GetYear(),
				ConvTime.GetMonth(),
				ConvTime.GetDay(),
				ConvTime.GetHour(),
				ConvTime.GetMinute() );
	}

    return sc::string::format(
			"%1%-%2$02d-%3$02d %4$02d:%5$02d:%6$02d",
			ConvTime.GetYear(),
			ConvTime.GetMonth(),
			ConvTime.GetDay(),
			ConvTime.GetHour(),
			ConvTime.GetMinute(),
			ConvTime.GetSecond());	
}
/*
std::wstring D3DCOLOR_TO_HTML( DWORD ColorA )
{
	// 메모리에
	// B -> R
	// G -> G
	// R -> B
	// A -> A
	DWORD ColorB = ( ((ColorA & 0x00FF0000) >> 16) | ((ColorA & 0x000000FF) << 16) | (ColorA & 0xFF00FF00) );

	// A 는 빼버리고 RGB 만 문자열로 바꾼다.
	return HexEncodeW( (BYTE*) &ColorB, sizeof(DWORD)-1 );
}
*/

//! 문자열을 html 로 변환한다. 특수기호 처리는 하지않는다.
/*
void ToHtml( const std::wstring& strTextIn, DWORD dwSize, DWORD dwColor, std::wstring& strTextOut, WORD wAddLine )
{
	if( strTextIn.empty() )
		return;

	strTextOut.append( CORE_COMMON::StringUtil::format(
		L"<font size='%1%' color='#%2%'>%3%</font>",
		dwSize,
		CORE_COMMON::StringUtil::D3DCOLOR_TO_HTML( dwColor ),
		strTextIn ) );

	for( WORD i = 0; i < wAddLine; ++i )
	{
		strTextOut.append( L"<BR>" );
	}
}
*/
//! 문자열을 html 로 변환, 특수기호 처리
//! " -> &quot;  & -> &amp; < -> &lt; > -> &gt;
void StringToHtml( std::wstring& strSrc )
{
	//boost::algorithm::replace_all( strSrc, L"\"", L"&quot;" );
	boost::algorithm::replace_all( strSrc, L"&", L"&amp;" );
	boost::algorithm::replace_all( strSrc, L"<", L"&lt;" );
	boost::algorithm::replace_all( strSrc, L">", L"&gt;" );
}

//! MD5 를 구한다.
std::string getMd5A( const std::string& strSrc )
{
	//trim( strSrc );
	HASH::CHash passHash;
	passHash.SetHashOperation( HASH::STRING_HASH );	
	passHash.SetHashAlgorithm( HASH::MD5 );
	passHash.SetHashString   ( strSrc.c_str() );
	CStringA strMd5Hash = passHash.DoHash();
	return std::string( strMd5Hash.GetString() );
}

std::wstring getMd5W( const std::wstring& strSrc )
{
	//trim( strSrc );

	HASH::CHashW passHash;
	passHash.SetHashOperation( HASH::STRING_HASH );	
	passHash.SetHashAlgorithm( HASH::MD5 );
	passHash.SetHashString   ( strSrc.c_str() );
	CStringW strMd5Hash = passHash.DoHash();
	return std::wstring( strMd5Hash.GetString() );
}

//! SHA1
std::string getSha1A( const std::string& strSrc )
{
	//trim( strSrc );

	HASH::CHash passHash;
	passHash.SetHashOperation( HASH::STRING_HASH );	
	passHash.SetHashAlgorithm( HASH::SHA1 );
	passHash.SetHashString   ( strSrc.c_str() );
	CStringA strSHA1 = passHash.DoHash();
	return std::string( strSHA1.GetString() );
}

std::wstring getSha1W( const std::wstring& strSrc )
{
	//trim( strSrc );

	HASH::CHashW passHash;
	passHash.SetHashOperation( HASH::STRING_HASH );	
	passHash.SetHashAlgorithm( HASH::SHA1 );
	passHash.SetHashString   ( strSrc.c_str() );
	CStringW strSHA1 = passHash.DoHash();
	return std::wstring( strSHA1.GetString() );
}

//! SHA2
std::string getSha2A( const std::string& strSrc )
{
	//trim( strSrc );

	HASH::CHash passHash;
	passHash.SetHashOperation( HASH::STRING_HASH );
	passHash.SetHashAlgorithm( HASH::SHA2 );
	passHash.SetHashString   ( strSrc.c_str() );
	CStringA strSHA2 = passHash.DoHash();
	return std::string( strSHA2.GetString() );
}
std::wstring getSha2W( const std::wstring& strSrc )
{
	//trim( strSrc );

	HASH::CHashW passHash;
	passHash.SetHashOperation( HASH::STRING_HASH );	
	passHash.SetHashAlgorithm( HASH::SHA2 );
	passHash.SetHashString   ( strSrc.c_str() );
	CStringW strSHA2 = passHash.DoHash();
	return std::wstring( strSHA2.GetString() );
}

//! SHA256
//! 반환되는 hash 값은 대문자이다.
//! 64 개의 문자열 + 1 (공백문자) = 65
//! Hash 값이 제대로 되었있는지는 아래 URL 에서 확인가능하다.
//! http://www.xorbin.com/tools/sha256-hash-calculator
std::string getSha256(const std::string& strSrc)
{
    CryptoPP::SHA256 Sha256;    
    BYTE* OUTPUT = new BYTE[Sha256.DigestSize()];
    Sha256.CalculateDigest(OUTPUT, (const byte*) strSrc.c_str(), strSrc.size());
    std::string strHash(HexEncodeA(OUTPUT, Sha256.DigestSize()));
    delete [] OUTPUT;
    return strHash;
}

//! CRC 32
std::string getCrc32A(const std::string& strSrc)
{
	//trim( strSrc );

	HASH::CHash passHash;
	passHash.SetHashOperation(HASH::STRING_HASH);
	passHash.SetHashAlgorithm(HASH::CRC32);
	passHash.SetHashString   (strSrc.c_str());
	CStringA strCRC = passHash.DoHash();
	return std::string(strCRC.GetString());
}

std::wstring getCrc32W(const std::wstring& strSrc)
{
	// trim( strSrc );

	HASH::CHashW passHash;
	passHash.SetHashOperation(HASH::STRING_HASH);
	passHash.SetHashAlgorithm(HASH::CRC32);
	passHash.SetHashString   (strSrc.c_str());
	CStringW strCRC = passHash.DoHash();
	return std::wstring( strCRC.GetString());
}

// unsigned int getCrc32_2(void const *buffer, std::size_t byte_count)
// {
//     if (byte_count < 0)
//         return 0;
// 
//     CryptoPP::CRC32 Sha256;    
//     BYTE* OUTPUT = new BYTE[Sha256.DigestSize()];
//     Sha256.CalculateDigest(OUTPUT, (const byte*) strSrc.c_str(), strSrc.size());
//     std::string strHash(HexEncodeA(OUTPUT, Sha256.DigestSize()));
//     delete [] OUTPUT;
//     return strHash;
// }

unsigned int getCrc32(void const *buffer, std::size_t byte_count)
{
    if (byte_count == 0)
        return 0;

    boost::crc_32_type computer;
    computer.process_bytes(buffer, byte_count);
    return computer.checksum();
}

unsigned short getCrc16(void const *buffer, std::size_t byte_count)
{
    if (byte_count == 0)
        return 0;

    boost::crc_16_type computer;
    computer.process_bytes(buffer, byte_count);
    return computer.checksum();
}

std::string getAppPathA()
{
	CStringA strFullPath;
	CStringA strCommandLine = GetCommandLineA();

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

	if (strFullPath.GetLength() < 4)
	{
		char szTest[MAX_PATH] = {0};
		::GetCurrentDirectoryA(MAX_PATH, szTest);
		return std::string( szTest );		
	}
	else
	{
		return std::string( strFullPath.GetString() );
	}
}

std::wstring getAppPathW()
{
	CStringW strFullPath;
	CStringW strCommandLine = GetCommandLineW();

	if ( !strCommandLine.IsEmpty() )
	{
		DWORD dwFind = strCommandLine.ReverseFind( '\\' );
		if ( dwFind != -1 )
		{
			strFullPath = strCommandLine.Left( dwFind );

			if ( !strFullPath.IsEmpty() )
				if ( strFullPath.GetAt(0) == '"' )
					strFullPath = strFullPath.Right( strFullPath.GetLength() - 1 );
		}
	}

	if (strFullPath.GetLength() < 4)
	{
		wchar_t szTest[MAX_PATH] = {0};
		::GetCurrentDirectoryW(MAX_PATH, szTest);
		return std::wstring( szTest );		
	}
	else
	{
		return std::wstring( strFullPath.GetString() );
	}
}

int HexPairValue(const char * code)
{
	int value = 0;
	const char * pch = code;
	for (;;) {
		int digit = *pch++;
		if (digit >= '0' && digit <= '9') {
			value += digit - '0';
		}
		else if (digit >= 'A' && digit <= 'F') {
			value += digit - 'A' + 10;
		}
		else if (digit >= 'a' && digit <= 'f') {
			value += digit - 'a' + 10;
		}
		else {
			return -1;
		}
		if (pch == code + 2)
			return value;
		value <<= 4;
	}
}

int UrlDecode(const char *source, char *dest)
{
	char * start = dest;

	while (*source) {
		switch (*source) {
	case '+':
		*(dest++) = ' ';
		break;
	case '%':
		if (source[1] && source[2]) {
			int value = HexPairValue(source + 1);
			if (value >= 0) {
				*(dest++) = value;
				source += 2;
			}
			else {
				*dest++ = '?';
			}
		}
		else {
			*dest++ = '?';
		}
		break;
	default:
		*dest++ = *source;
		}
		source++;
	}

	*dest = 0;
	return int(dest - start);
}  

int UrlEncode(const char *source, char *dest, unsigned max)  
{
	static const char *digits = "0123456789ABCDEF";
	unsigned char ch;
	unsigned len = 0;
	char *start = dest;

	while (len < max - 4 && *source)
	{
		ch = (unsigned char)*source;
		if (*source == ' ') {
			*dest++ = '+';
		}
		else if (isalnum(ch) || strchr("-_.!~*'()", ch)) {
			*dest++ = *source;
		}
		else {
			*dest++ = '%';
			*dest++ = digits[(ch >> 4) & 0x0F];
			*dest++ = digits[       ch & 0x0F];
		}  
		source++;
	}
	*dest = 0;
	return int(start - dest);
}

std::string UrlDecodeString(const std::string & encoded)
{
	const char * sz_encoded = encoded.c_str();
	size_t needed_length = encoded.length();
	for (const char * pch = sz_encoded; *pch; pch++) {
		if (*pch == '%')
			needed_length += 2;
	}
	needed_length += 10;
	char stackalloc[64];
	char * buf = needed_length > sizeof(stackalloc)/sizeof(*stackalloc) ?
		(char *)malloc(needed_length) : stackalloc;
	UrlDecode(encoded.c_str(), buf);
	std::string result(buf);
	if (buf != stackalloc) {
		free(buf);
	}
	return result;
}

std::string UrlEncodeString(const std::string & decoded)
{
	//const char * sz_decoded = decoded.c_str();
	size_t needed_length = decoded.length() * 3 + 3;
	char stackalloc[64];
	char * buf = needed_length > sizeof(stackalloc)/sizeof(*stackalloc) ?
		(char *)malloc(needed_length) : stackalloc;
	UrlEncode( decoded.c_str(), buf, (unsigned int) needed_length );
	std::string result(buf);
	if (buf != stackalloc) {
		free(buf);
	}
	return result;
}

std::wstring UrlDecodeStringW(const std::wstring& encoded)
{
	return utf8ToUnicode(
			UrlDecodeString(
				unicodeToUtf8( encoded ) ) );
}

std::wstring UrlEncodeStringW(const std::wstring& decoded)
{
	return utf8ToUnicode(
			UrlEncodeString(
				unicodeToUtf8( decoded ) ) );
}

/*
 * Shift-Or Algorithm 이용;
 */
/*
*/
const unsigned int FindWord(const std::string& strTarget IN, const std::string& strSource IN)
{
	const char* const _cSource(strSource.c_str());
	const unsigned int _sizeSource(strSource.length());
	if ( _sizeSource < 32 )
	{
		unsigned int _bitPattern[256];
		::memset(_bitPattern, 0x0L, sizeof(unsigned int) * 256);
		for ( unsigned int _index(_sizeSource); _index--; )
		{
			const unsigned char _indexPattern((unsigned char)_cSource[_index]);
			_bitPattern[_indexPattern] |= (0x1L << _index);
		}

		unsigned int _bitResult(0x1L);
		const unsigned int _bitCompare(0x1L << _sizeSource);
		const unsigned int _sizeTarget(strTarget.length());
		const char* const _cTarget(strTarget.c_str());
		for ( unsigned int _index(0); _index < _sizeTarget; ++_index )
		{
			const unsigned char _indexPattern((unsigned char)_cTarget[_index]);
			_bitResult &= _bitPattern[_indexPattern];
			_bitResult = (_bitResult << 1) | 0x1L;			

			if ( _bitResult & _bitCompare )
				return _index - _sizeSource + 1;
		}
	}
	else if ( _sizeSource < 128 )
	{
		sc::BitField128 _bitPattern[256];
		for ( unsigned int _index(_sizeSource); _index--; )
		{
			const unsigned char _indexPattern((unsigned char)_cSource[_index]);
			_bitPattern[_indexPattern] |= _index;
		}

		sc::BitField128 _bitResult(_sizeSource, 0x1L);
		const unsigned int _sizeTarget(strTarget.length());
		const char* const _cTarget(strTarget.c_str());
		for ( unsigned int _index(0); _index < _sizeTarget; ++_index )
		{
			const unsigned char _indexPattern((unsigned char)_cTarget[_index]);
			_bitResult &= _bitPattern[_indexPattern];
			_bitResult <<= 1;
			_bitResult |= 0;

			if ( (_bitResult & _sizeSource) == true )
				return _index - _sizeSource + 1;
		}
	}
	return unsigned int(-1);
}

/*
LONGLONG ConvertLongLong(const char* szText)
{
	std::string strTemp(szText);
	std::wstring strReturn = utf8ToUnicode(strTemp);

	return (LONGLONG)_tstoi64(strReturn.c_str());
}
*/

void SqlInjectionCheck(std::string& Sql)
{
    replace_all(Sql, "'", "''");
    replace_all(Sql, ";", "");
//     replace_all(Sql, "--", " ");
//     replace_all(Sql, "select", " ");
//     replace_all(Sql, "insert", " ");
//     replace_all(Sql, "update", " ");
//     replace_all(Sql, "delete", " ");
//     replace_all(Sql, "drop", " ");
//     replace_all(Sql, "union", " ");
//     replace_all(Sql, "and", " ");
//     replace_all(Sql, "or", " ");
//     replace_all(Sql, "1=1", " ");
//     replace_all(Sql, "sp_", " ");
//     replace_all(Sql, "xp_", " ");
//     replace_all(Sql, "@variable", " ");
//     replace_all(Sql, "@@variable", " ");
//     replace_all(Sql, "exec", " ");
//     replace_all(Sql, "sysobject", " ");
}

std::string GetLastError(DWORD ErrorCode)
{
    HLOCAL hLocal = NULL;
    DWORD SystemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
    BOOL fOK = FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL,
        ErrorCode,
        SystemLocale,
        (PTSTR) &hLocal,
        0,
        NULL);
    if (!fOK)
    {
        // 네트워크와 관련된 에러인가?
        HMODULE hDll = LoadLibraryEx(TEXT("netmsg.dll"), NULL, DONT_RESOLVE_DLL_REFERENCES);
        if (hDll)
        {
            fOK = FormatMessage(
                FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                hDll,
                ErrorCode,
                SystemLocale,
                (PTSTR) &hLocal,
                0,
                NULL);
            FreeLibrary(hDll);
        }
    }

    std::string ErrorMsg;
    if (fOK && hLocal)
    {
        ErrorMsg = (PCTSTR) LocalLock(hLocal);
        LocalFree(hLocal);
    }
    else
    {
        ErrorMsg = "Unknown error";
    }

    std::string ReturnStr(
        sc::string::format(
            "%1% %2%",
            ErrorMsg,
            ErrorCode));

    return ReturnStr;
}

void ParseMark(const std::string& OriginalStr, char StartMark, char EndMark, std::vector<PARSE_RESULT>& SplitStr)
{
    bool bMark = false;
    std::string SubStr;
    std::string::const_iterator iterator1 = OriginalStr.begin();    
    while (iterator1 != OriginalStr.end()) 
    {
        if (*iterator1 == StartMark && bMark == false)
        {
            if (!SubStr.empty())
            {
                PARSE_RESULT Result;
                Result.m_Mark = false;
                Result.m_Data = SubStr;
                SplitStr.push_back(Result);
            }
            bMark = true;
            SubStr.clear();
            SubStr.append(1, *iterator1);
        }
        else if (*iterator1 == EndMark && bMark == true)
        {
            SubStr.append(1, *iterator1);
            if (!SubStr.empty())
            {
                PARSE_RESULT Result;
                Result.m_Mark = true;
                Result.m_Data = SubStr;
                SplitStr.push_back(Result);
            }
            SubStr.clear();
            bMark = false;
        }
        else
        {
            SubStr.append(1, *iterator1);
        }
        ++iterator1;
    }
    if (!SubStr.empty())
    {
        PARSE_RESULT Result;
        Result.m_Mark = bMark;
        Result.m_Data = SubStr;
        SplitStr.push_back(Result);        
    }  
}

void AddCommaToNum(const char *str, char *buf) 
{ 
   int len = strlen(str); 

   switch ((len - 1) % 3) 
   {
   case 3:
      while (*str) { 
                 *buf++ = ','; 
		case 2: *buf++ = *str++; 
		case 1: *buf++ = *str++; 
		case 0: *buf++ = *str++; 
      } 
   }
   *buf = '\0'; 
}


    } // namespace string
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BitField128::BitField128(void)
		: sizeUse(sc::BitField128::BIT_FLAG_NSIZE)
	{
		::memset(this->arrayBit, 0x0L, sizeof(unsigned int) * sc::BitField128::BIT_FLAG_NSIZE);
	}
	BitField128::BitField128(const unsigned int _sizeBitUse, const unsigned int _bitLow4Byte)
		: sizeUse((_sizeBitUse >> 5) + 1)
	{
		::memset(this->arrayBit, 0x0L, sizeof(unsigned int) * (sc::BitField128::BIT_FLAG_NSIZE - 1));
		this->arrayBit[sc::BitField128::BIT_FLAG_NSIZE - 1] = _bitLow4Byte;
	}
	BitField128::BitField128(const unsigned int _sizeBitUse)
		: sizeUse((_sizeBitUse >> 5) + 1)
	{
		::memset(this->arrayBit, 0x0L, sizeof(unsigned int) * sc::BitField128::BIT_FLAG_NSIZE);
	}
	BitField128::BitField128(const sc::BitField128& _rhs)
		: sizeUse(_rhs.sizeUse)
	{
		::memcpy(this->arrayBit, _rhs.arrayBit, sizeof(unsigned int ) * sc::BitField128::BIT_FLAG_NSIZE);
	}

} // namespace sc