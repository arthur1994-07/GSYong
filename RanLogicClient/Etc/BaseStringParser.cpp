#include "pch.h"
#include "./BaseStringParser.h"

#include "../../SigmaCore/String/basestring.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/Common/SubPath.h"
#include "../../EngineLib/G-Logic/GLogic.h"

#include "../../RanLogic/GLogicDataMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

bool CBaseStringParser::Parse( const CString& strFileName, std::vector< CString >& vecString )
{
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s( strFileName.GetString(), NULL, 0, NULL, 0, fname, _MAX_FNAME, ext, _MAX_EXT );
	StringCchCat ( fname, _MAX_EXT, ext );

    std::auto_ptr<sc::CBaseString> pBString(
        GLOGIC::openfile_basestring(
            GLOGIC::UseLogicZipFile(), 
            GLogicData::GetInstance().GetGlogicZipFile(),
            strFileName.GetString(), 
            fname,
            false,
            GLOGIC::UseLogicPackFile()));

	if( !pBString.get() )
	{
		sc::writeLogError(
            sc::string::format(
                "CBaseStringParser::Parse(), File Open %1%", fname));
		return false;
	}

	sc::CBaseString &cBString = *pBString;

	CString strLine;
	CString strFullText;
	while ( cBString.GetNextLine(strLine) )
	{
		if ( strLine == ';' )
		{
			int nLength = strFullText.GetLength ();
			if ( 2<nLength && strFullText[nLength-2] == '\r' && strFullText[nLength-1] == '\n' )
				strFullText = strFullText.Left(nLength-2);

			nLength = strFullText.GetLength ();
			if ( nLength )
			{
				vecString.push_back ( strFullText );

				CString strOrigin(strFullText);
				CString strConvert;
				int nSize = strOrigin.GetLength();
				strConvert = strOrigin;

				int nRFindPos(strConvert.Find('\r'))
					, nNFindPos(strConvert.Find('\n'))
					, nRNFindPos(strConvert.Find('\r\n'));

				strFullText.Empty ();
			}
		}
		else
		{			
			strLine += "\r\n";
			strFullText += strLine;
		}
	}

	if ( strFullText.GetLength () )
        vecString.push_back ( strFullText );

	return true;
}

INT CBaseStringParser::RandomSelect( std::vector< CString >& vecString )
{
    INT nSize = static_cast<INT>( vecString.size() );
    if( nSize <= 0 )
        return INT_MIN;

	//srand( (UINT)time(NULL) );

    return rand() % nSize;
}