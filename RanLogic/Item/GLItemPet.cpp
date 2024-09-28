#include "../pch.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "./GLItemPet.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ITEM
{
    BOOL SPETSKINPACKITEM::LOAD ( sc::BaseStream &SFile )
	{
		DWORD dwVER(0), dwSIZE(0);
		SFile >> dwVER;
		SFile >> dwSIZE;

		switch (dwVER)
		{
		case 0x0100:
			{
				DWORD dwNUM(0);
				SPETSKINPACKITEMDATA sITEM;

				SFile >> dwPetSkinTime;
				SFile >> dwNUM;
				for ( DWORD i=0; i<dwNUM; ++i )
				{
					SFile.ReadBuffer ( &sITEM, sizeof(SPETSKINPACKITEMDATA) );
					vecPetSkinData.push_back ( sITEM );
				}

			}
			break;
		case VERSION:	//	데이터 포맷 변경 진행
			{
				DWORD dwNUM(0);
				SPETSKINPACKITEMDATA sITEM;

				SFile >> dwNUM;
				for ( DWORD i=0; i<dwNUM; ++i )
				{
					SFile.ReadBuffer ( &sITEM, sizeof(SPETSKINPACKITEMDATA) );
					vecPetSkinData.push_back ( sITEM );
				}

				SFile >> dwPetSkinTime;
			}
			break;

		default:
			SFile.SetOffSet ( SFile.GetfTell()+dwSIZE );
			MessageBox ( NULL, "item/SPETSKINPACKITEM unknown data version.", "ERROR", MB_OK );
			break;
		};

		return TRUE;
	}

	BOOL SPETSKINPACKITEM::SAVE ( sc::SerialFile &SFile )
	{
		SFile << DWORD(VERSION);
		SFile.BeginBlock();
		{
			SFile << (DWORD) vecPetSkinData.size();
			if ( vecPetSkinData.size() > 0 )
			{
				SFile.WriteBuffer ( &(vecPetSkinData[0]), DWORD(sizeof(SPETSKINPACKITEMDATA)*vecPetSkinData.size()) );
			}

			SFile << dwPetSkinTime;
		}
		SFile.EndBlock();

		return TRUE;
	}

	VOID SPETSKINPACKITEM::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "PetSkinTime" << ",";

		for( int i=0; i<SKINPACK_MAX; ++i )
		{
			SFile << "PetSkin " << i << " fRATE" << ",";
			SFile << "PetSkin " << i << " fSCALE" << ",";
			SFile << "PetSkin " << i << " nidMOB wMainID" << ",";
			SFile << "PetSkin " << i << " nidMOB wSubID" << ",";			
		}
	}

	VOID SPETSKINPACKITEM::SaveCsv ( std::fstream &SFile )
	{
		int iSize = (int)vecPetSkinData.size();

		SFile << dwPetSkinTime << ",";

		if( iSize > SKINPACK_MAX )
		{
			iSize = SKINPACK_MAX;
		}

		for( int i=0; i<SKINPACK_MAX; ++i )
		{
			if( i < iSize )
			{
				SFile << vecPetSkinData[i].fRate << ",";
				SFile << vecPetSkinData[i].fScale << ",";
				SFile << vecPetSkinData[i].sMobID.wMainID << ",";
				SFile << vecPetSkinData[i].sMobID.wSubID << ",";
			}
			else
			{
				SFile << "0" << ",";
				SFile << "0" << ",";
				SFile << "0" << ",";
				SFile << "0" << ",";
			}
		}
	}

	VOID SPETSKINPACKITEM::LoadCsv ( CStringArray &StrArray )
	{
		vecPetSkinData.clear();

		dwPetSkinTime  = atoi( StrArray[ iCsvCur++ ] );

		for( int i=0; i<SKINPACK_MAX; ++i )
		{
			SPETSKINPACKITEMDATA stPetSkinPack;

			stPetSkinPack.fRate  = (float)atof( StrArray[ iCsvCur++ ] );
			stPetSkinPack.fScale = (float)atof( StrArray[ iCsvCur++ ] );
			stPetSkinPack.sMobID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
			stPetSkinPack.sMobID.wSubID  = (WORD)atoi( StrArray[ iCsvCur++ ] );

			if( stPetSkinPack.fRate != 0.0f && stPetSkinPack.fScale != 0.0f )
				vecPetSkinData.push_back( stPetSkinPack );
		}
	}

	void SPETSKINPACKITEM::CLEAR ()
	{
		vecPetSkinData.clear();
	}

	bool SPETSKINPACKITEM::VALID () const
	{
		return !vecPetSkinData.empty();
	}

	bool SPETSKINPACKITEM::INSERT ( const SNATIVEID &sMobID, float fRate, float fScale )
	{
		if ( SKINPACK_MAX <= vecPetSkinData.size() )
		{
			MessageBox ( NULL, "PetSkinPack-item count is max", "ERROR", MB_OK );
			return false;
		}
		SPETSKINPACKITEMDATA sPetSkinPack;
		sPetSkinPack.sMobID = sMobID;
		sPetSkinPack.fRate  = fRate;
		sPetSkinPack.fScale = fScale;

		vecPetSkinData.push_back ( sPetSkinPack );

		return true;
	}

	bool SPETSKINPACKITEM::DEL ( int nIndex )
	{
		if ( nIndex < 0 )					return false;
		if ( (int)vecPetSkinData.size() < nIndex )	return false;

		vecPetSkinData.erase ( vecPetSkinData.begin()+nIndex );

		return true;
	}

    BOOL SPET::LOAD( sc::BaseStream &SFile )	// PetData
    {
        DWORD dwVer( 0 ), dwSize( 0 );
        SFile >> dwVer;
        SFile >> dwSize;

        switch( dwVer )
        {
        case 0x0100:
            {
                SFile >> dwPetID;
            }
            break;



        case 0x0101:
            {
                int nType(0);
                SFile >> dwPetID;
                SFile >> nType;
                emType = static_cast<ACCESSORYTYPE>(nType);
            }
            break;

        case 0x0102:
            {

                int nType(0);
                SFile >> dwPetID;
                SFile >> nType;
                emType = static_cast<ACCESSORYTYPE>(nType);
                SFile >> nType;
                emPetType = static_cast<int>(nType);
                SFile >> sPetID.dwID;
            }
            break;
        case VERSION:	//	데이터 포맷 변경 진행
            {
                GASSERT( sizeof( ITEM::SPET ) == dwSize );

                int nType(0);
                SFile >> nType;
                emPetType = static_cast<int>(nType);
                SFile >> dwPetID;

                SFile >> nType;
                emType = static_cast<ACCESSORYTYPE>(nType);

                SFile >> sPetID.dwID;
            }
            break;

        default:
            {
                MessageBox( NULL, "SITEM::LoadFile(), ITEM::SPET unknown data version.", "ERROR", MB_OK );
                SFile.SetOffSet( SFile.GetfTell() + dwSize );
                return FALSE;
            }
            break;
        }

        return TRUE;
    }

    BOOL SPET::SAVE( sc::SerialFile &SFile )	// PetData
    {
        SFile << static_cast<int>( ITEM::SPET::VERSION );
        SFile.BeginBlock(); // 구조체 사이즈를 저장한다.
        {
            SFile << emPetType;
            SFile << dwPetID;
            SFile << emType;
            SFile << sPetID.dwID;
        }
        SFile.EndBlock();

        return TRUE;
    }

    VOID SPET::SaveCsvHead ( std::fstream &SFile )	// PetData
    {
        SFile << "dwPetID" << ",";
        SFile << "emType" << ",";
        SFile << "emPetType" << ",";
        SFile << "sPetID[MID]" << ",";
        SFile << "sPetID[SID]" << ",";
    }

    VOID SPET::SaveCsv ( std::fstream &SFile )	// PetData
    {
        SFile << dwPetID << ",";
        SFile << emType << ",";
        SFile << emPetType << ",";
        SFile << sPetID.wMainID << ",";
        SFile << sPetID.wSubID << ",";
    }

    VOID SPET::LoadCsv ( CStringArray &StrArray, int& iCsvCur )	// PetData
    {
        ++iCsvCur;
        dwPetID = (DWORD)atol( StrArray[ iCsvCur++ ] );
        emType = (ACCESSORYTYPE)atoi( StrArray[ iCsvCur++ ] );
        emPetType = (int)atoi( StrArray[ iCsvCur++ ] );
        sPetID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
        sPetID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
    }

} // namespace ITEM