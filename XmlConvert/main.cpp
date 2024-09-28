

#define _AFXDLL


#include <afxwin.h>
#include <afxext.h>
#include <afxole.h>
#include <atlbase.h>
#include <atlwin.h>
#include <afxcoll.h>

#include <conio.h>
#include <windows.h>
#include <string>
#include <math.h>
#include <time.h>
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../SigmaCore/File/StringFile.h"
#include "../SigmaCore/File/SerialFile.h"
#include "../SigmaCore/Encrypt/Rijndael.h"
#include "XmlConverter.h"


enum EM_COVERT_SEQUENCE
{
	S_CONV_NO = -1,
	S_CONV_BEGIN,
	S_CONV_TYPE,
	S_CONV_RESULT,
	S_CONV_START,
	S_CONV_CONV,
	S_CONV_END,
};

enum EM_CONVERT_TYPE
{
	CONV_TYPE_NO = -1,
	CONV_TYPE_STRTABLE,
	CONV_TYPE_TIP,
	CONV_TYPE_WORD,
	CONV_TYPE_EX,
	CONV_TYPE_LUA,
	CONV_TYPE_EMOTICON,
	CONV_TYPE_MOBLOGIC,
	CONV_TYPE_COMMENT,
	CONV_TYPE_PETCOMMENT,
	CONV_TYPE_EXCEL,
	CONV_TYPE_SKIP,
	CONV_TYPE_EXCELTOXML,
};

int main()
{
	sc::CRijndael oRijndael;
	oRijndael.Initialize( sc::CRijndael::sm_Version[sc::CRijndael::VERSION-1].c_str(), sc::CRijndael::sm_KeyLength[sc::CRijndael::VERSION-1] );

	sc::CStringFile StrFile;
	if ( !StrFile.Open( "./ConverterText.txt", FALSE ) )
	{
		::MessageBoxA( NULL, "\"ConverterText.txt\" Not Find", "ERROR", MB_OK );
		return 0;
	}

	EM_COVERT_SEQUENCE	emSEQUENCE = S_CONV_NO;
	EM_CONVERT_TYPE		emTYPE = CONV_TYPE_NO;
	
	CString	strLine;
	char	szResultFile[255] = {0};
	XmlConverter* pStrTable = new XmlConverter("1.0", "utf-8");
	while( StrFile.GetNextLine( strLine ) != 0 )
	{
		if ( strcmp (strLine, "GO") == 0 )
		{
			emSEQUENCE = S_CONV_BEGIN;
			continue;
		}
		if ( emSEQUENCE == S_CONV_NO )
			continue;

		INT iPos = 0;
		char szData[255] = {0};
		CString strToken = strLine.Tokenize("\t", iPos);
		strcpy (szData, &strLine.GetString()[strToken.GetLength()+1]);
		printf ("COMMAND LINE FEED(%s)\n", strLine.GetString());
		printf ("\tTOKEN : \"%s\"\n", strToken.GetString());
		printf ("\tDATA : \"%s\"\n", szData);

		char szSeparate[100] = {0};
		strcpy_s(szSeparate, iPos, strToken.GetString());
		if		( strcmp( szSeparate, "TYPE") == 0 )		{emSEQUENCE = S_CONV_TYPE;}
		else if ( strcmp( szSeparate, "RESULT") == 0 )		{emSEQUENCE = S_CONV_RESULT;}
		else if ( strcmp( szSeparate, "START") == 0 )		{emSEQUENCE = S_CONV_START;}
		else if ( strcmp( szSeparate, "END") == 0 )		{emSEQUENCE = S_CONV_END;}

		switch (emSEQUENCE)
		{
		case S_CONV_TYPE:
			{
				if ( strcmp(szData, "NORMAL") == 0 )		emTYPE = CONV_TYPE_STRTABLE;
				else if ( strcmp(szData, "TIP") == 0 )		emTYPE = CONV_TYPE_TIP;
				else if ( strcmp(szData, "WORD") == 0 )		emTYPE = CONV_TYPE_WORD;
				else if ( strcmp(szData, "EX") == 0 )		emTYPE = CONV_TYPE_EX;
				else if ( strcmp(szData, "LUA") == 0 )		emTYPE = CONV_TYPE_LUA;
				else if ( strcmp(szData, "EMOTICON") == 0 )	emTYPE = CONV_TYPE_EMOTICON;
				else if ( strcmp(szData, "MOBLOGIC") == 0 ) emTYPE = CONV_TYPE_MOBLOGIC;
				else if ( strcmp(szData, "COMMENT") == 0 )	emTYPE = CONV_TYPE_COMMENT;
				else if ( strcmp(szData, "PETCOMMENT") == 0 ) emTYPE = CONV_TYPE_PETCOMMENT;
				else if ( strcmp(szData, "EXCEL") == 0 )	emTYPE = CONV_TYPE_EXCEL;
				else if ( strcmp(szData, "SKIP") == 0 )		emTYPE = CONV_TYPE_SKIP;
				else if ( strcmp(szData, "EXCELTOXML") == 0)emTYPE = CONV_TYPE_EXCELTOXML;
			}
			break;
		case S_CONV_RESULT:
			{
				strcpy (szResultFile, szData);
			}
			break;
		case S_CONV_START:
			emSEQUENCE = S_CONV_CONV;
			break;
		case S_CONV_CONV:
			{
				bool bResult;
				if ( emTYPE == CONV_TYPE_STRTABLE )
				{
					bResult = pStrTable->RanTextConvertToXml_Sentence( 
						strToken.GetString(), 
						szResultFile, 
						szData, true );
				}
				else if ( emTYPE == CONV_TYPE_TIP )
				{
					bResult = pStrTable->RanTipTextConvertToXml( 
						strToken.GetString(), 
						szResultFile, 
						szData, true );
				}
				else if ( emTYPE == CONV_TYPE_WORD )
				{
					bResult = pStrTable->RanTextConvertToXml_Word( 
						strToken.GetString(), 
						szResultFile, 
						szData, true );
				}
				else if ( emTYPE == CONV_TYPE_EX )
				{
					bResult = pStrTable->RanTextConvertToXml_EX( 
						strToken.GetString(), 
						szResultFile, 
						szData, true );
				}
				else if ( emTYPE == CONV_TYPE_LUA )
				{
					bResult = pStrTable->RanTextConvert_Lua(//RanTextConvertToXml_CommentLua( 
						strToken.GetString(), 
						szResultFile, 
						szData, true );
				}
				else if ( emTYPE == CONV_TYPE_EMOTICON )
				{
					bResult = pStrTable->RanTextConvert_Emoticon(strToken.GetString(), 
						szResultFile, 
						szData, true);
				}
				else if ( emTYPE == CONV_TYPE_MOBLOGIC)
				{
					bResult = pStrTable->RanTextConvert_MobLogic(strToken.GetString(), 
						szResultFile, 
						szData, true);
				}
				else if ( emTYPE == CONV_TYPE_COMMENT )
				{
					bResult = pStrTable->RanTextConvertToXml_CommentLua_OnlyText(strToken.GetString(), 
						szResultFile, 
						szData, true);
				}
				else if ( emTYPE == CONV_TYPE_PETCOMMENT )
				{
					bResult = pStrTable->RanTextConvertToXml_PetCommentLua_OnlyText(strToken.GetString(), 
						szResultFile, 
						szData, true);
				}
				else if ( emTYPE == CONV_TYPE_EXCEL )
				{
					bResult = pStrTable->RanTextConvert_Excel(strToken.GetString(), 
						szResultFile, 
						szData, true);
				}
				else if ( emTYPE == CONV_TYPE_SKIP )
				{
					bResult = pStrTable->RanTextConvert_Skip(strToken.GetString(), 
						szResultFile, 
						szData, true);
				}
				else if (emTYPE == CONV_TYPE_EXCELTOXML)
				{
					bResult = pStrTable->RanTextConvert_ExcelToXml(strToken.GetString(), 
						szResultFile, 
						szData, true);
				}

				if (!bResult)
				{
					CString strError = strToken;
					strError.Append( " Not Find" );
					::MessageBoxA( NULL, strError.GetString(), "ERROR", MB_OK );
				}
			}
			break;
		}
	}

	StrFile.Close();
	delete pStrTable;

	if ( emSEQUENCE == S_CONV_END )
		::MessageBoxA( NULL, "Convert Complet", "Yes", MB_OK );
	else
	{
		::MessageBoxA( NULL, "Convert ERROR", "ERROR", MB_OK );
		switch ( emTYPE )
		{
		case S_CONV_NO:
			::MessageBoxA( NULL, "\"GO\" 문구를 찾을 수 없습니다", "ERROR", MB_OK );
			break;
		case S_CONV_BEGIN:
			::MessageBoxA( NULL, "\"TYPE\" 문구를 찾을 수 없습니다", "ERROR", MB_OK );
			break;
		case S_CONV_TYPE:
			::MessageBoxA( NULL, "\"RESULT\" 문구를 찾을 수 없습니다", "ERROR", MB_OK );
			break;
		case S_CONV_RESULT:
			::MessageBoxA( NULL, "\"START\" 문구를 찾을 수 없습니다", "ERROR", MB_OK );
			break;
		case S_CONV_START:
			::MessageBoxA( NULL, "\"START, END\" 사이 문구가 잘못되었습니다.", "ERROR", MB_OK );
			break;
		}
	}
	_getch();
	return 0;
}