//	[String Utils],JDH (2002.12.02)
//
//	(2002.12.02) StringSeparate () 함수 및 관련 부가 함수 제작.
//		스트링을 받아서 등록된 "구분자"로 구분하여 스트링 어래이에
//		넣어주는 메소드임.
//
#pragma once

#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <boost/format.hpp>

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

namespace STRUTIL
{
	//	Note : "구분자" 등록 메소드.
	void RegisterSeparator(const CString& strSeparator);
	
	//	Note : 등록된 "구분자" 제거 메소드.
	void ClearSeparator ();

	//	Note : 스트링을 "구분자"로 분리하여 스트링 어래이에 넣어서 반환.
	//		"실패"시 "FALSE"를 리턴.
	//		"성공"시 "TRUE"를 리턴.
	//
	BOOL StringSeparate ( CString &Str, CStringArray &StrArray, bool bEmptyInclude = false );
	void StringReplace(const std::string& s, const std::string& oldsub,	const std::string& newsub, bool replace_all, std::string* res);


	// 가변 길이 문자열을 복사할때
    /*
	inline void STRCPY ( PCHAR &szDes, const PCHAR &szSrc )
	{
		SAFE_DELETE_ARRAY(szDes);
		if (!szSrc)
        return;
		size_t nStrLen = strlen(szSrc)+1;
		szDes = new char[nStrLen];
		StringCchCopy(szDes, nStrLen, szSrc);
	}
    */

	CString GetExtName( const CString& strFileName );
	BOOL ChangeExt ( const char* _szSrc, CString &strDes, const char* szDesExt );
	BOOL ChangeExt ( const char* _szSrc, const char* szSrcExt, CString &strDes, const char* szDesExt );

	// CSV 문자열 입출력 함수
	VOID OutputStrCsv( std::fstream &SFile, std::string &Str, bool bMargin = false );
	VOID InputStrCsv( CString &Src, std::string &Dest );
    

    BOOL InitThaiDLL();
    BOOL ReleaseThaiDLL();
    BOOL CheckThaiString(const CString& strChk);
	
    BOOL CheckString(const CString& strChk);
	BOOL CheckVietnamString(const CString& strChk);
	BOOL CheckCharSunjo(const char chText );
	BOOL CheckCharMoum(const char chText );
/*
	template<typename T1>  
	CString format( const char* szFormat, const T1& data1 );
	
	template<typename T1, typename T2>  
	CString format( const char* szFormat, const T1& data1, const T2& data2 );
	
	template<typename T1, typename T2, typename T3>  
	CString format( const char* szFormat, const T1& data1, const T2& data2, const T3& data3 );
	
	template<typename T1, typename T2, typename T3, typename T4>  
	CString format( const char* szFormat, const T1& data1, const T2& data2, const T3& data3, const T4& data4 );

	template<typename T1, typename T2, typename T3, typename T4, typename T5>  
	CString format( const char* szFormat, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5 );

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>  
	CString format( const char* szFormat, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5,
						const T6& data6 );

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>  
	CString format( const char* szFormat, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5,
						const T6& data6, const T7& data7 );

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>  
	CString format( const char* szFormat, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5,
						const T6& data6, const T7& data7, const T8& data8 );

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>  
	CString format( const char* szFormat, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5,
						const T6& data6, const T7& data7, const T8& data8, const T9& data9 );
*/
	// This function converts Formated-CString to specific CTime value.
	// DO NOT USE with NOT-Formatted CString.
	//
	CTime CStringToCTime( CString &cs );
};

class CSEPARATOR
{
protected:
	typedef std::vector<char>		SEP_STR;
	
protected:
	SEP_STR m_Separator;

protected:
	bool IsSeparator ( char cVALUE );

public:
	//	Note : "구분자" 등록 메소드.
	void Register ( char cSEP );
	
	//	Note : 등록된 "구분자" 제거 메소드.
	void Clear ();

	//	Note : 스트링을 "구분자"로 분리하여 스트링 배열에 넣어서 반환.
	//		"실패"시 "false"를 리턴.
	//		"성공"시 "true"를 리턴.
	//
	bool DoSeparate ( const std::string& strSource, std::vector<std::string> &strArray );
};

inline char* bool2sz ( bool bOK )
{
	return bOK?"true":"false";
}

/*
template<typename T1>  
CString STRUTIL::format( const char* szFormat, const T1& data1 )
{
	CString strResult;

	boost::format fmter(szFormat);
	fmter.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit )  );
	fmter % data1;
	strResult = fmter.str().c_str();

	return strResult;
}

template<typename T1, typename T2>  
CString STRUTIL::format( const char* szFormat, const T1& data1, const T2& data2 )
{
	CString strResult;

	boost::format fmter(szFormat);
	fmter.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit )  );
	fmter % data1 % data2;
	strResult = fmter.str().c_str();


	return strResult;
}

template<typename T1, typename T2, typename T3>  
CString STRUTIL::format( const char* szFormat, const T1& data1, const T2& data2, const T3& data3 )
{
	CString strResult;

    boost::format fmter(szFormat);
	fmter.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit )  );
	fmter % data1 % data2 % data3;
	strResult = fmter.str().c_str();

	return strResult;
}

template<typename T1, typename T2, typename T3, typename T4>  
CString STRUTIL::format( const char* szFormat, const T1& data1, const T2& data2, const T3& data3, const T4& data4 )
{
	CString strResult;

    boost::format fmter(szFormat);
	fmter.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit )  );
	fmter % data1 % data2 % data3 % data4;
	strResult = fmter.str().c_str();

	return strResult;
}

template<typename T1, typename T2, typename T3, typename T4, typename T5>
CString STRUTIL::format( const char* szFormat, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5 )
{
	CString strResult;

    boost::format fmter(szFormat);
	fmter.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit )  );
	fmter % data1 % data2 % data3 % data4 % data5;
	strResult = fmter.str().c_str();

	return strResult;
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
CString STRUTIL::format( const char* szFormat, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5,
							 const T6& data6 )
{
	CString strResult;

    boost::format fmter(szFormat);
	fmter.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit )  );
	fmter % data1 % data2 % data3 % data4 % data5 % data6;
	strResult = fmter.str().c_str();

	return strResult;
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
CString STRUTIL::format( const char* szFormat, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5,
							 const T6& data6, const T7& data7 )
{
	CString strResult;

    boost::format fmter(szFormat);
	fmter.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit )  );
	fmter % data1 % data2 % data3 % data4 % data5 % data6 % data7;
	strResult = fmter.str().c_str();

	return strResult;
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
CString STRUTIL::format( const char* szFormat, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5,
							 const T6& data6, const T7& data7, const T8& data8 )
{
	CString strResult;

    boost::format fmter(szFormat);
	fmter.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit )  );
	fmter % data1 % data2 % data3 % data4 % data5 % data6 % data7 % data8;
	strResult = fmter.str().c_str();

	return strResult;
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
CString STRUTIL::format( const char* szFormat, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5,
							 const T6& data6, const T7& data7, const T8& data8, const T9& data9 )
{
	CString strResult;

    boost::format fmter(szFormat);
	fmter.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit )  );
	fmter % data1 % data2 % data3 % data4 % data5 % data6 % data7 % data8 % data9;
	strResult = fmter.str().c_str();

	return strResult;
}
*/
