#include "../pch.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "./GLItemVehecle.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ITEM
{   

    BOOL SVEHICLE::LOAD( sc::BaseStream &SFile )	// VehicleData
    {
        DWORD dwVer( 0 ), dwSize( 0 );
        SFile >> dwVer;
        SFile >> dwSize;

        switch( dwVer )
        {
        case 0x0100:
        case 0x0101:
            {
                int nType ( 0 );
                SFile >> nType;
                emVehicleType = static_cast<VEHICLE_TYPE>(nType);
            }
            break;
        case 0x0102:
            {
                int nType ( 0 );
                SFile >> nType;
                emVehicleType = static_cast<VEHICLE_TYPE>(nType);

                SFile >> nType;
                emPartsType = static_cast<EMPARTS_TYPE>(nType);
            }
            break;

        case VERSION:
            {
                int nType ( 0 );
                SFile >> nType;
                emVehicleType = static_cast<VEHICLE_TYPE>(nType);

                SFile >> nType;
                emPartsType = static_cast<EMPARTS_TYPE>(nType);

                SFile >> bNotUseParts;
            }
            break;
        default:
            {
                MessageBox( NULL, "SITEM::LoadFile(), ITEM::SVEHICLE unknown data version.", "ERROR", MB_OK );
                SFile.SetOffSet( SFile.GetfTell() + dwSize );
                return FALSE;
            }
            break;
        }

        return TRUE;
    }

    BOOL SVEHICLE::SAVE( sc::SerialFile &SFile )	// VehicleData
    {
        SFile << static_cast<int>( ITEM::SVEHICLE::VERSION );
        SFile.BeginBlock(); // 구조체 사이즈를 저장한다.
        {
            SFile << emVehicleType;
            SFile << emPartsType;
            SFile << bNotUseParts;
        }
        SFile.EndBlock();

        return TRUE;
    }

    VOID SVEHICLE::SaveCsvHead ( std::fstream &SFile )	// VehicleData
    {
        SFile << "emVehicleType" << ",";
        SFile << "emPartsType" << ",";
        SFile << "bNotUseParts" << ",";
    }

    VOID SVEHICLE::SaveCsv ( std::fstream &SFile )	// VehicleData
    {
        SFile << emVehicleType << ",";
        SFile << emPartsType << ",";
        SFile << bNotUseParts << ",";
    }

    VOID SVEHICLE::LoadCsv ( CStringArray &StrArray, int& iCsvCur )	// VehicleData
    {
        emVehicleType = (VEHICLE_TYPE)atoi( StrArray[ iCsvCur++ ] );
        emPartsType = (EMPARTS_TYPE)atoi( StrArray[ iCsvCur++ ] );
        bNotUseParts = (BOOL)atoi( StrArray[ iCsvCur++ ] );
    }

} // namespace ITEM

namespace vehicle
{
    //! 탑승상태에서 이동관련 아이템을 사용가능한가?
    bool CanUseTeleportItem(VEHICLE_TYPE Type)
    {
		switch ( Type )
		{
		case VEHICLE_TYPE_BOARD:	return false;
		case VEHICLE_TYPE_BIKE:		return false;
		case VEHICLE_TYPE_CAR:		return false;
		case VEHICLE_TYPE_REARCAR:	return false;
		default:					return true;
		}
    }
} // namespace vehicle