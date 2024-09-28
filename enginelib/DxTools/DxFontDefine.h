#ifndef _DX_FONT_DEFINE_H_
#define _DX_FONT_DEFINE_H_

#pragma once

namespace language
{
	// tool.ran-online.co.kr 에서 사용하는 CountryNum이다;
	enum TEXTTOOL_COUNTRYNUM
	{
		COUNTRYNUM_KR = 1,				// 대한민국;
		COUNTRYNUM_TW = 6,				// 대만;
		COUNTRYNUM_JP = 7,				// 일본;
		COUNTRYNUM_COMMON = 9,			// COMMON;
		COUNTRYNUM_HK = 10,				// 홍콩;
		COUNTRYNUM_PE = 11,				// 필리핀;
		COUNTRYNUM_ML = 12,				// 말레이시아 <중문>;
		COUNTRYNUM_ME = 13,				// 말레이시아 <영문>;
		COUNTRYNUM_TH = 14,				// 태국 <태국어>;
		COUNTRYNUM_TE = 15,				// 태국 <영문>;
		COUNTRYNUM_CN = 16,				// 중국;
		COUNTRYNUM_ID = 18,				// 인도네시아;
		COUNTRYNUM_VN = 19,				// 베트남;
		COUNTRYNUM_GSP= 21,				// 글로벌;
		COUNTRYNUM_TS = 22,				// 테스트;
		COUNTRYNUM_IN = 23,				// 인도;
		COUNTRYNUM_GS = 24,				// 글로벌;
		COUNTRYNUM_WB = 27,				// 월드배틀;
		COUNTRYNUM_EU = 31,				// 유럽;
		COUNTRYNUM_US = 32,				// 북미;
	};

	//! 언어 설정
	enum LANGFLAG           
	{
		DEFAULT				= 0,
		TRADITIONAL_CHINESE,
		JAPANESE,
		KOREAN,
		SIMPLIFIED_CHINESE,
		THAILAND,
		VIETNAM,
//		INDONESIA,
		LANG_NUM,
	};

	// 서비스 중인 국가만 설정한다; GS 제외;
	enum SERVICENATION
	{
		NATION_KOREA = 0,
		NATION_GLOBALSERVICE,	
		NATION_EUROPE,			// 유럽;
		NATION_AMERICA,			// 미국;
		NATION_TAIWAN,
		NATION_INDONESIA,
		NATION_THAILAND,		// 태국;
		NATION_MALAYSIA_ENGLISH,
		NATION_MALAYSIA_CHINESE,
		NATION_PHILIPPINES,
		NATION_CHINESE,

		NATION_SIZE,
	};


	extern char strNationKeyWord[NATION_SIZE][10];
	extern TCHAR strNationFont[NATION_SIZE][20];

	const LANGFLAG		NationToLanguage( const SERVICENATION emNation );
	const SERVICENATION StrNationToEMNation( const char* szNation );
	const TEXTTOOL_COUNTRYNUM NationToCountrynum( const SERVICENATION langFlag );

	//  [5/23/2014 gbgim];
	// - 같은 언어를 사용한다고 해서 하나의 키워드를 사용하지않는다;
	//   국가별 문화에 따라 다르게, 다른 의미로 사용해야하기 때문에 국가별 분리가 꼭 필요하다;
	//   ex) 영국 영어와 미국 영어가 완전하게 똑같지 않은 것 처럼;
	// Service Provider에 대한 언어선택 플래그;
	enum EMPROVIDE_LANGFLAG
	{
		LANGFLAG_KOREA        =  0,	//! Korea
		LANGFLAG_TAIWAN       =  1,	//! Taiwan
		LANGFLAG_CHINA        =  2,	//! China								// Not Used
		LANGFLAG_JAPAN        =  3,	//! Japan								// Not Used
		LANGFLAG_PHILIPPINES  =  4,	//! Philippines							
		LANGFLAG_THAILAND     =  5,	//! Thailand
		LANGFLAG_GLOBAL       =  6,	//! Global Service (GSP)				// Not Used
		LANGFLAG_MALAYSIA     =  7,	//! Malaysia
		LANGFLAG_GS			  =  8,	//! Global Service
		LANGFLAG_INDONESIA    =  9,	//! Indonesia
		LANGFLAG_MALAYSIA_EN  = 10,	//! Malaysia English
		LANGFLAG_VIETNAM      = 11,	//! Vietnam								// Not Used
		LANGFLAG_HONGKONG     = 12,	//! HongKong							// Not Used
		LANGFLAG_WORLD_BATTLE = 13,	//! World Battle,						// Not Used
		LANGFLAG_EU           = 14,	//! EU, Games Masters
		LANGFLAG_US           = 15,	//! US, GameSamba
		LANGFLAG_TOTAL_NUM    = 16,
	};

	// Param dwLangSet, Xml 문자열 참조 키워드;
	extern TCHAR strProvideLanguage[LANGFLAG_TOTAL_NUM][8];
	
	const LANGFLAG ProvidelangToLangflag( const EMPROVIDE_LANGFLAG emServiceLang );
	const LANGFLAG StringToLangflag( const char* szServiceLang );
	const EMPROVIDE_LANGFLAG StringToProvidelang( const char* szServiceLang );
	const TEXTTOOL_COUNTRYNUM ProvidelangToTexttoolcountrynum( const EMPROVIDE_LANGFLAG emServiceLang );
	const std::string CountryNumToNationStr(TEXTTOOL_COUNTRYNUM emCountryNum);
} // namespace language
#endif // _DX_FONT_DEFINE_H_