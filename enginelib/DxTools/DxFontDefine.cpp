#include "pch.h"
#include "DxFontDefine.h"

namespace language
{
	char strNationKeyWord[NATION_SIZE][10] = 
	{
		"kr",
		"GS",
		"eu",
		"us",
		"tw",
		"id",
		"th",
		"me",
		"ml",
		"pe",
		"cn",
	};

	TCHAR strNationFont[NATION_SIZE][20] =
	{
		_T("돋움체"),		// KR
		_T("MingLiU"),		// Global Service
		_T("MingLiU"),		// Europe
		_T("MingLiU"),		// America
		_T("MingLiU"),		// Taiwan
		_T("Tahoma"),		// Indonesia
		_T("Arial"),		// Thailand
		_T("MingLiU"),		// Malaysia EN
		_T("MingLiU"),		// Malaysia CN
		_T("MingLiU"),		// Philippines
		_T("NSimSun"),		// Chinese
	};

	const char* const	LanguageToStrNation( const LANGFLAG emLang )
	{
		// 없는 곳은 kr리턴;
		switch ( emLang )
		{
			// 		case TRADITIONAL_CHINESE:	return strNationKeyWord[NATION_TAIWAN];
		case KOREAN:				return strNationKeyWord[NATION_KOREA];
		case SIMPLIFIED_CHINESE:	return strNationKeyWord[NATION_KOREA];
		case THAILAND:				return strNationKeyWord[NATION_THAILAND];
		case VIETNAM:				return strNationKeyWord[NATION_GLOBALSERVICE];
		}
		static CString strLangTemp;
		return strLangTemp;
	}

	const LANGFLAG NationToLanguage( const SERVICENATION emNation )
	{
		switch ( emNation )
		{
		case NATION_KOREA :					return KOREAN;
		case NATION_GLOBALSERVICE :
		case NATION_EUROPE :			// 유럽;
		case NATION_AMERICA :			// 미국;
		case NATION_MALAYSIA_ENGLISH :
		case NATION_INDONESIA :
		case NATION_PHILIPPINES :
			return DEFAULT;
		case NATION_MALAYSIA_CHINESE :
		case NATION_TAIWAN :				return TRADITIONAL_CHINESE;
		case NATION_THAILAND :				return THAILAND;
		case NATION_CHINESE :				return SIMPLIFIED_CHINESE;
		}
		return DEFAULT;
	}

	const SERVICENATION StrNationToEMNation( const char* szNation )
	{
		for ( UINT i = 0; i < NATION_SIZE; ++i )
		{
			if ( strcmp( szNation, strNationKeyWord[i] ) == 0 )
				return (SERVICENATION)i;
		}
		return NATION_SIZE;
	}

	const TEXTTOOL_COUNTRYNUM NationToCountrynum( const SERVICENATION langFlag )
	{
		switch ( langFlag )
		{
		case NATION_KOREA:				return COUNTRYNUM_KR;
		case NATION_GLOBALSERVICE:		return COUNTRYNUM_GS;
		case NATION_EUROPE:				return COUNTRYNUM_EU;
		case NATION_AMERICA:			return COUNTRYNUM_US;
		case NATION_TAIWAN:				return COUNTRYNUM_TW;
		case NATION_INDONESIA:			return COUNTRYNUM_IN;
		case NATION_THAILAND:			return COUNTRYNUM_TH;
		case NATION_MALAYSIA_ENGLISH:	return COUNTRYNUM_ME;
		case NATION_MALAYSIA_CHINESE:	return COUNTRYNUM_ML;
		case NATION_PHILIPPINES:		return COUNTRYNUM_PE;
		case NATION_CHINESE:			return COUNTRYNUM_CN;
		}
		return COUNTRYNUM_COMMON;
	}

	//////////////////////////////////////////////////////////////////////////
	TCHAR strProvideLanguage[LANGFLAG_TOTAL_NUM][8] =
	{
		_T("kr"),		//! Korea
		_T("tw"),		//! Taiwan
		_T("cn"),		//! China								// Not Used
		_T("jp"),		//! Japan								// Not Used
		_T("pe"),		//! Philippines							
		_T("th"),		//! Thailand
		_T("gsp"),		//! Global Service (GSP)				// Not Used
		_T("ml"),		//! Malaysia
		_T("GS"),		//! Global Service
		_T("id"),		//! Indonesia
		_T("me"),		//! Malaysia English
		_T("vn"),		//! Vietnam								// Not Used
		_T("hk"),		//! HongKong							// Not Used
		_T("wb"),		//! World Battle,						// Not Used
		_T("eu"),		//! EU, Games Masters
		_T("us"),		//! US, GameSamba
	};

	const LANGFLAG ProvidelangToLangflag( const EMPROVIDE_LANGFLAG emServiceLang )
	{
		switch (emServiceLang)
		{
		case LANGFLAG_KOREA:			return KOREAN;
		case LANGFLAG_TAIWAN:
		case LANGFLAG_MALAYSIA:			return TRADITIONAL_CHINESE;
		case LANGFLAG_HONGKONG:		
		case LANGFLAG_CHINA:			return SIMPLIFIED_CHINESE;
		case LANGFLAG_JAPAN:			return JAPANESE;
		case LANGFLAG_THAILAND:			return THAILAND;
		case LANGFLAG_INDONESIA:
		case LANGFLAG_PHILIPPINES:		
		case LANGFLAG_GLOBAL:
		case LANGFLAG_GS:
		case LANGFLAG_MALAYSIA_EN:
		case LANGFLAG_VIETNAM:
		case LANGFLAG_WORLD_BATTLE:
		case LANGFLAG_EU:
		case LANGFLAG_US:
		case LANGFLAG_TOTAL_NUM:		return DEFAULT;
			//default :						return LANG_NUM;
		}		
		return DEFAULT;
	}

	const LANGFLAG StringToLangflag( const char* szServiceLang )
	{
		return ProvidelangToLangflag(StringToProvidelang(szServiceLang));
	}

	const EMPROVIDE_LANGFLAG StringToProvidelang( const char* szServiceLang )
	{
		for ( UINT i = 0; i < LANGFLAG_TOTAL_NUM; ++i )
		{
			if ( strcmp( szServiceLang, strProvideLanguage[i] ) == 0 )
				return (EMPROVIDE_LANGFLAG)i;
		}
		return LANGFLAG_TOTAL_NUM;
	}

	const TEXTTOOL_COUNTRYNUM ProvidelangToTexttoolcountrynum( const EMPROVIDE_LANGFLAG emServiceLang )
	{
		switch ( emServiceLang )
		{
		case LANGFLAG_KOREA:			return COUNTRYNUM_KR;		// 대한민국;
		case LANGFLAG_TAIWAN:			return COUNTRYNUM_TW;		// 대만;
		case LANGFLAG_CHINA:			return COUNTRYNUM_CN;		// 중쿡;				//Not Used
		case LANGFLAG_JAPAN:			return COUNTRYNUM_JP;		// 일본;				//Not Used
		case LANGFLAG_PHILIPPINES:		return COUNTRYNUM_PE;		// 필리핀;
		case LANGFLAG_THAILAND:			return COUNTRYNUM_TH;		// 태국;
		case LANGFLAG_GLOBAL:			return COUNTRYNUM_GSP;		// 글로벌 서비스;		//Not Used
		case LANGFLAG_MALAYSIA:			return COUNTRYNUM_ML;		// 말레이시아-중문;
		case LANGFLAG_GS:				return COUNTRYNUM_GS;		// 글로벌 서비스;
		case LANGFLAG_INDONESIA:		return COUNTRYNUM_ID;		// 인도네시아;
		case LANGFLAG_MALAYSIA_EN:		return COUNTRYNUM_ME;		// 말레이시아-영문;
		case LANGFLAG_VIETNAM:			return COUNTRYNUM_VN;		// 베트남;				//Not Used
		case LANGFLAG_HONGKONG:			return COUNTRYNUM_HK;		// 홍콩;				//Not Used
		case LANGFLAG_WORLD_BATTLE:		return COUNTRYNUM_WB;		// 월드배틀;			//Not Used
		case LANGFLAG_EU:				return COUNTRYNUM_EU;		// 유럽;
		case LANGFLAG_US:				return COUNTRYNUM_US;		// 미국;
		case LANGFLAG_TOTAL_NUM:		return COUNTRYNUM_COMMON;
		default:						return COUNTRYNUM_COMMON;
		}
	}
	const std::string CountryNumToNationStr(TEXTTOOL_COUNTRYNUM emCountryNum)
	{
		switch (emCountryNum)
		{
		case COUNTRYNUM_KR:					return strProvideLanguage[LANGFLAG_KOREA];		
		case COUNTRYNUM_TW:					return strProvideLanguage[LANGFLAG_TAIWAN];		
		case COUNTRYNUM_CN:					return strProvideLanguage[LANGFLAG_CHINA];		
		case COUNTRYNUM_JP:					return strProvideLanguage[LANGFLAG_JAPAN];		
		case COUNTRYNUM_PE:					return strProvideLanguage[LANGFLAG_PHILIPPINES];	
		case COUNTRYNUM_TH:					return strProvideLanguage[LANGFLAG_THAILAND];		
		case COUNTRYNUM_GSP:				return strProvideLanguage[LANGFLAG_GLOBAL];		
		case COUNTRYNUM_ML:					return strProvideLanguage[LANGFLAG_MALAYSIA];		
		case COUNTRYNUM_GS:					return strProvideLanguage[LANGFLAG_GS];			
		case COUNTRYNUM_ID:					return strProvideLanguage[LANGFLAG_INDONESIA];	
		case COUNTRYNUM_ME:					return strProvideLanguage[LANGFLAG_MALAYSIA_EN];	
		case COUNTRYNUM_VN:					return strProvideLanguage[LANGFLAG_VIETNAM];		
		case COUNTRYNUM_HK:					return strProvideLanguage[LANGFLAG_HONGKONG];		
		case COUNTRYNUM_WB:					return strProvideLanguage[LANGFLAG_WORLD_BATTLE];	
		case COUNTRYNUM_EU:					return strProvideLanguage[LANGFLAG_EU];			
		case COUNTRYNUM_US:					return strProvideLanguage[LANGFLAG_US];			
		case COUNTRYNUM_COMMON:				return strProvideLanguage[LANGFLAG_TOTAL_NUM];	
		default:	return strProvideLanguage[LANGFLAG_TOTAL_NUM];
		}
	}
}
