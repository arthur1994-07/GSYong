#include "../pch.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "./GLItemRandomBox.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ITEM
{

BOOL SRANDOMBOX::LOAD ( sc::BaseStream &SFile )
{
    DWORD dwVER(0), dwSIZE(0);
    SFile >> dwVER;
    SFile >> dwSIZE;

    switch (dwVER)
    {
    case 0x0100:
        {
            DWORD dwNUM(0);
            SRANDOMITEM_100 sITEM_Old;
            SRANDOMITEM sITEM;

            SFile >> dwNUM;
            for ( DWORD i=0; i<dwNUM; ++i )
            {
                SFile.ReadBuffer ( &sITEM_Old, sizeof(SRANDOMITEM_100) );

                sITEM.Assign( sITEM_Old );

                vecBOX.push_back ( sITEM );
            }

        }
        break;

    case VERSION:
        {
            DWORD dwNUM(0);
            SRANDOMITEM sITEM;

            SFile >> dwNUM;
            for ( DWORD i=0; i<dwNUM; ++i )
            {
                SFile.ReadBuffer ( &sITEM, sizeof(SRANDOMITEM) );
                vecBOX.push_back ( sITEM );
            }
        }
        break;

    default:
        SFile.SetOffSet ( SFile.GetfTell()+dwSIZE );
        MessageBox ( NULL, "item/randombox unknown data version.", "ERROR", MB_OK );
        break;
    };

    return TRUE;
}

BOOL SRANDOMBOX::SAVE ( sc::SerialFile &SFile )
{
    SFile << DWORD(VERSION);
    SFile.BeginBlock();
    {
        SFile << (DWORD) vecBOX.size();
        if ( vecBOX.size() > 0 )
        {
            SFile.WriteBuffer ( &(vecBOX[0]), DWORD(sizeof(SRANDOMITEM)*vecBOX.size()) );
        }
    }
    SFile.EndBlock();

    return TRUE;
}

VOID SRANDOMBOX::SaveCsvHead ( std::fstream &SFile )
{
    const int iMaxSize = 30;

    for( int i=0; i<iMaxSize; ++i )
    {
        SFile << "vecBOX " << i << " fRATE" << ",";
        SFile << "vecBOX " << i << " nidITEM wMainID" << ",";
        SFile << "vecBOX " << i << " nidITEM wSubID" << ",";
    }
}

VOID SRANDOMBOX::SaveCsv ( std::fstream &SFile )
{
    const int iMaxSize = 30;
    int iSize = (int)vecBOX.size();

    if( iSize > iMaxSize )
    {
        iSize = iMaxSize;
        MessageBox( NULL, _T("최대 30개까지만 저장합니다."), _T("ERROR"), MB_OK );
    }

    for( int i=0; i<iMaxSize; ++i )
    {
        if( i < iSize )
        {
            SFile << vecBOX[i].fRATE << ",";
            SFile << vecBOX[i].nidITEM.wMainID << ",";
            SFile << vecBOX[i].nidITEM.wSubID << ",";
        }
        else
        {
            SFile << "0" << ",";
            SFile << "0" << ",";
            SFile << "0" << ",";
        }
    }
}

VOID SRANDOMBOX::LoadCsv ( CStringArray &StrArray, int& iCsvCur )
{
    const int iMaxSize = 30;

    vecBOX.clear();

    for( int i=0; i<iMaxSize; ++i )
    {
        SRANDOMITEM stRandomItem;

        stRandomItem.fRATE = (float)atof( StrArray[ iCsvCur++ ] );
        stRandomItem.nidITEM.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
        stRandomItem.nidITEM.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

        if( stRandomItem.fRATE != 0.0f )
            //				stRandomItem.nidITEM.wMainID != 0 &&
            //				stRandomItem.nidITEM.wSubID != 0 )
            vecBOX.push_back( stRandomItem );
    }
}

void SRANDOMBOX::CLEAR ()
{
    vecBOX.clear();
}

bool SRANDOMBOX::VALID () const
{
    return !vecBOX.empty();
}

bool SRANDOMBOX::INSERT ( const SNATIVEID &nidITEM, float fRATE )
{
    SRANDOMITEM sITEM;
    sITEM.nidITEM = nidITEM;
    sITEM.fRATE = fRATE;

    vecBOX.push_back ( sITEM );

    return true;
}

bool SRANDOMBOX::DEL ( int nIndex )
{
    if ( nIndex < 0 )					return false;
    if ( int(vecBOX.size()) < nIndex )	return false;

    vecBOX.erase ( vecBOX.begin()+nIndex );

    return true;
}

} // namespace ITEM