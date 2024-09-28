#include "../pch.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "./GLItemQuestion.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ITEM
{

    BOOL SQUESTIONITEM::LOAD( sc::BaseStream &SFile ) // by 경대
    {
        DWORD dwVer(0), dwSize(0);
        SFile >> dwVer;
        SFile >> dwSize;

        if ( dwVer==ITEM::SQUESTIONITEM::VERSION )	//	데이터 포맷 변경 진행
        {
            GASSERT(sizeof(ITEM::SQUESTIONITEM)==dwSize);
            int nItem(0);
            SFile >> nItem;
            emType = static_cast<EMITEM_QUESTION>(nItem);
            SFile >> wParam1;
            SFile >> wParam2;
            SFile >> fTime;
            SFile >> fExp; // 경험치 획득 0.04~0.1%
        }
        else if ( dwVer==0x0100 )	
        {
            int nItem(0);
            SFile >> nItem;
            emType = static_cast<EMITEM_QUESTION>(nItem);
            SFile >> fTime;	
            SFile >> fExp; // 경험치 획득 0.04~0.1%
            SFile >> wParam1;
            SFile >> wParam2;

        }
        else
        {
            MessageBox ( NULL, "SITEM::LoadFile(), ITEM::SQUESTION unknown data version.", "ERROR", MB_OK );
            SFile.SetOffSet ( SFile.GetfTell()+dwSize );

            return FALSE;
        }

        return TRUE;
    }

    BOOL SQUESTIONITEM::SAVE( sc::SerialFile &SFile )
    {
        SFile << static_cast<int>(ITEM::SQUESTIONITEM::VERSION);
        SFile.BeginBlock(); // 구조체 사이즈를 저장한다.
        {
            SFile << static_cast<int>(emType);
            SFile << wParam1;
            SFile << wParam2;
            SFile << fTime;
            SFile << fExp; // 경험치 획득 0.04~0.1%
        }
        SFile.EndBlock();

        return TRUE;
    }

    VOID SQUESTIONITEM::SaveCsvHead ( std::fstream &SFile )
    {
        SFile << "emType" << ",";
        SFile << "fTime" << ",";
        SFile << "fExp" << ",";
        SFile << "wParam1" << ",";
        SFile << "wParam2" << ",";
    }

    VOID SQUESTIONITEM::SaveCsv ( std::fstream &SFile )
    {
        SFile << emType << ",";
        SFile << fTime << ",";
        SFile << fExp << ",";
        SFile << wParam1 << ",";
        SFile << wParam2 << ",";
    }

    VOID SQUESTIONITEM::LoadCsv ( CStringArray &StrArray, int& iCsvCur )
    {
        emType = (EMITEM_QUESTION)atoi( StrArray[ iCsvCur++ ] );
        fTime = (float)atof( StrArray[ iCsvCur++ ] );
        fExp = (float)atof( StrArray[ iCsvCur++ ] );
        wParam1 = (WORD)atoi( StrArray[ iCsvCur++ ] );
        wParam2 = (WORD)atoi( StrArray[ iCsvCur++ ] );
    }

} // namespace ITEM