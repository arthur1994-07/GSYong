#ifndef _GL_ITEM_VEHECLE_H_
#define _GL_ITEM_VEHECLE_H_

#pragma once

#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "./GLItemDef.h"

namespace sc
{
    class BaseStream;
    class SerialFile;
}

enum VEHICLE_TYPE
{
    VEHICLE_TYPE_NONE		= -1,
    VEHICLE_TYPE_BOARD		= 0,
    VEHICLE_TYPE_BIKE		= 1,
	VEHICLE_TYPE_CAR		= 2,
	VEHICLE_TYPE_REARCAR	= 3,
    VEHICLE_TYPE_SIZE		= 4,
};

namespace vehicle
{
    //! ž�»��¿��� �̵����� �������� ��밡���Ѱ�?
    bool CanUseTeleportItem(VEHICLE_TYPE Type);
}

enum VEHICLE_ACCESSORYTYPE
{
    ACCE_TYPE_SKIN		= 0, // skin
    ACCE_TYPE_PARTS_A	= 1, // parts
    ACCE_TYPE_PARTS_B	= 2, // parts
    ACCE_TYPE_PARTS_C	= 3, // parts
    ACCE_TYPE_PARTS_D	= 4, // parts
    ACCE_TYPE_PARTS_E	= 5, // parts
    ACCE_TYPE_PARTS_F	= 6, // parts
    ACCE_TYPE_SIZE		= 7,
}; 

namespace ITEM
{
    struct SVEHICLE_101
    {
        VEHICLE_TYPE	emVehicleType; // TYPE
    }; 
    struct SVEHICLE_102
    {
        VEHICLE_TYPE	emVehicleType; // TYPE
        EMPARTS_TYPE	emPartsType;	// ����Ÿ��
    };

    struct SVEHICLE	// PetData
    {
        enum { VERSION = 0x0103 };	//	������ ���� ���� ���� ( ������ �ø� ) 

        VEHICLE_TYPE	emVehicleType; // TYPE
        EMPARTS_TYPE	emPartsType;	// ����Ÿ��
        BOOL			bNotUseParts;	// �������Ұ�

        SVEHICLE() 
            : emVehicleType(VEHICLE_TYPE_SIZE)
            , emPartsType( EMBOARD_HOVER )
            , bNotUseParts( FALSE )
        {
        }

        BOOL LOAD ( sc::BaseStream &SFile );
        BOOL SAVE ( sc::SerialFile &SFile );

        static VOID SaveCsvHead ( std::fstream &SFile );
        VOID SaveCsv ( std::fstream &SFile );
        VOID LoadCsv ( CStringArray &StrArray, int& iCsvCur );

        //! TYPE
        inline VEHICLE_TYPE	Type() const { return emVehicleType; }
    };

} // namespace ITEM
#endif // _GL_ITEM_VEHECLE_H_
