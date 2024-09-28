#include "../pch.h"
#include "./GLItem.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void SITEM::LoadFileBasic( sc::BaseStream &SFile )
{
   // bool bReturn = true;
    DWORD dwVer, dwSize;
    SFile >> dwVer;
    SFile >> dwSize;

    //if( bPastLoad )
    {
		switch ( dwVer )
		{
		case 0x0100L:
			{
				ITEM::SBASIC_100 sOldData;
				GASSERT(sizeof(sOldData)==dwSize);
				SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );

				sBasicOp.Assign ( sOldData );
			}
			break;
		case 0x0101L:
			{
				ITEM::SBASIC_101 sOldData;
				GASSERT(sizeof(sOldData)==dwSize);
				SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );

				sBasicOp.Assign ( sOldData );
			}
			break;
		case 0x0102L:
			{
				ITEM::SBASIC_102 sOldData;
				GASSERT(sizeof(sOldData)==dwSize);
				SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );

				sBasicOp.Assign ( sOldData );
			}
			break;
		case 0x0103L:
			sBasicOp.LOAD_103(SFile);
			break;
		case 0x0104L:
			sBasicOp.LOAD_104( SFile );
			break;
		case 0x0105L:
			sBasicOp.LOAD_105( SFile );
			break;
		case 0x0106L:
			sBasicOp.LOAD_106( SFile );
			break;
		case 0x0107L:
			sBasicOp.LOAD_107( SFile );
			break;
		case 0x0108L:
			sBasicOp.LOAD_108( SFile );
			break;
		case 0x0109L:
			sBasicOp.LOAD_109( SFile );
			break;
		case 0x0110L:
			sBasicOp.LOAD_110( SFile );
			break;
		case 0x0111L:
			sBasicOp.LOAD_111( SFile );
			break;
		case 0x0112L:
			sBasicOp.LOAD_112(SFile);
			break;
		case 0x0113L:
			sBasicOp.LOAD_113( SFile );
			break;
		case 0x0114L:
			sBasicOp.LOAD_114( SFile );
			break;
		case 0x0115L:
			sBasicOp.LOAD_115( SFile );
			break;
		case 0x0116L:
			sBasicOp.LOAD_116( SFile );
			break;
		case 0x0117L:
			sBasicOp.LOAD_117( SFile );
			break;
		case 0x0118L:
			sBasicOp.LOAD_118( SFile );
			break;
		case 0x0119L:
			sBasicOp.LOAD_119( SFile );
			break;
		case 0x0120L:
			sBasicOp.LOAD_120( SFile );
			break;
		case 0x0121L:
			sBasicOp.LOAD_121( SFile );
			break;
		case 0x0122L:
			sBasicOp.LOAD_122( SFile );
			break;
		case 0x0123L:
			sBasicOp.LOAD_123( SFile );
			break;
		case 0x0124L:
			sBasicOp.LOAD_124( SFile );
			break;
		case 0x0125L:
			sBasicOp.LOAD_125( SFile );
			break;
		case 0x0126L:
			sBasicOp.LOAD_126( SFile );
			break;
		case 0x0127L:
			sBasicOp.LOAD_127( SFile );
			break;
		case 0x0128L:
			sBasicOp.LOAD_128( SFile );
			break;
		case 0x0129L:
			sBasicOp.LOAD_129( SFile );
			break;
		case ITEM::SBASIC::VERSION:
			sBasicOp.LOAD( SFile );
			break;
		default:
			MessageBox ( NULL, "SITEM::LoadFile(), ITEM::SBASIC unknown data version.", "ERROR", MB_OK );
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );
			break;
		}
    }
    //else
    //{
    //    if ( ITEM::SBASIC::VERSION==dwVer )
    //    {
    //        sBasicOp.LOAD( SFile );
    //    }
    //    else
    //    {
    //        MessageBox ( NULL, "SITEM::LoadFile(), ITEM::SBASIC unknown data version.", "ERROR", MB_OK );
    //        SFile.SetOffSet ( SFile.GetfTell()+dwSize );
    //        bReturn = false;
    //    }
    //}
    //return bReturn;
}

void SITEM::LoadFileSuit( sc::BaseStream &SFile )
{
    DWORD dwVer, dwSize;
    SFile >> dwVer;
    SFile >> dwSize;

    if ( dwVer==0x0100 )
    {
        ITEM::SSUIT_100 sSuit100;
        GASSERT(sizeof(ITEM::SSUIT_100)==dwSize);
        SFile.ReadBuffer ( &sSuit100, sizeof(ITEM::SSUIT_100) );

        sSuitOp.Assign ( sSuit100 );
    }
    else if ( dwVer==0x0101 )
    {
        ITEM::SSUIT_101 sSuit101;
        GASSERT(sizeof(ITEM::SSUIT_101)==dwSize);
        SFile.ReadBuffer ( &sSuit101, sizeof(ITEM::SSUIT_101) );

        sSuitOp.Assign ( sSuit101 );
    }
    else if ( dwVer==0x0102 )
    {
        ITEM::SSUIT_102 sSuit102;
        GASSERT(sizeof(ITEM::SSUIT_102)==dwSize);
        SFile.ReadBuffer ( &sSuit102, sizeof(ITEM::SSUIT_102) );

        sSuitOp.Assign ( sSuit102 );
    }
    else if ( dwVer==0x0103 )
    {
        ITEM::SSUIT_103 sSuit103;
        GASSERT(sizeof(ITEM::SSUIT_103)==dwSize);
        SFile.ReadBuffer ( &sSuit103, sizeof(ITEM::SSUIT_103) );

        sSuitOp.Assign ( sSuit103 );
    }
    else if ( dwVer==0x0104 )
    {
        ITEM::SSUIT_104 sOldSuit;
        GASSERT(sizeof(sOldSuit)==dwSize);
        SFile.ReadBuffer ( &sOldSuit, sizeof(sOldSuit) );

        sSuitOp.Assign ( sOldSuit );
    }
    else if ( dwVer==0x0105 )
    {
        ITEM::SSUIT_105 sOldSuit;
        GASSERT(sizeof(sOldSuit)==dwSize);
        SFile.ReadBuffer ( &sOldSuit, sizeof(sOldSuit) );

        sSuitOp.Assign ( sOldSuit );
    }
    else if ( dwVer==0x0106 ) // By 경대
    {
        ITEM::SSUIT_106 sOldSuit;
        GASSERT(sizeof(sOldSuit)==dwSize);
        SFile.ReadBuffer ( &sOldSuit, sizeof(sOldSuit) );

        sSuitOp.Assign ( sOldSuit );
    }
    else if ( dwVer==0x0107 ) // By 경대
    {
        ITEM::SSUIT_107 sOldSuit;
        GASSERT(sizeof(sOldSuit)==dwSize);
        SFile.ReadBuffer ( &sOldSuit, sizeof(sOldSuit) );

        sSuitOp.Assign ( sOldSuit );
    }
    else if ( dwVer==0x0108 ) // By 경대
    {
        ITEM::SSUIT_108 sOldSuit;
        GASSERT(sizeof(sOldSuit)==dwSize);
        SFile.ReadBuffer ( &sOldSuit, sizeof(sOldSuit) );

        sSuitOp.Assign ( sOldSuit );
    }
    else if ( dwVer==0x0109 ) // By 경대
    {
        ITEM::SSUIT_109 sOldSuit;
        GASSERT(sizeof(sOldSuit)==dwSize);
        SFile.ReadBuffer ( &sOldSuit, sizeof(sOldSuit) );

        sSuitOp.Assign ( sOldSuit );
    }
    else if ( dwVer==0x0110 )
    {
        ITEM::SSUIT_110 sOldSuit;
        GASSERT(sizeof(sOldSuit)==dwSize);
        SFile.ReadBuffer ( &sOldSuit, sizeof(sOldSuit) );

        sSuitOp.Assign ( sOldSuit );
    }
	else if ( dwVer==0x0111 )
	{
		ITEM::SSUIT_111 sOldSuit;
		GASSERT(sizeof(sOldSuit)==dwSize);
		SFile.ReadBuffer ( &sOldSuit, sizeof(sOldSuit) );

		sSuitOp.Assign ( sOldSuit );
	}
	else if ( dwVer==0x0112 )
	{
		ITEM::SSUIT_112 sOldSuit;
		GASSERT(sizeof(sOldSuit)==dwSize);
		SFile.ReadBuffer ( &sOldSuit, sizeof(sOldSuit) );

		sSuitOp.Assign ( sOldSuit );
	}
	else if ( dwVer==0x0113 )
	{
		ITEM::SSUIT_113 sOldSuit;
		GASSERT(sizeof(sOldSuit)==dwSize);
		SFile.ReadBuffer ( &sOldSuit, sizeof(sOldSuit) );

		sSuitOp.Assign ( sOldSuit );
	}
	else if ( dwVer==0x0114 )
	{
		ITEM::SSUIT_114 sOldSuit;
		GASSERT(sizeof(sOldSuit)==dwSize);
		SFile.ReadBuffer ( &sOldSuit, sizeof(sOldSuit) );

		sSuitOp.Assign ( sOldSuit );
	}
	else if ( dwVer==0x0115 )
	{
		ITEM::SSUIT_115 sOldSuit;
		GASSERT( sizeof( sOldSuit ) == dwSize );
		SFile.ReadBuffer( &sOldSuit, sizeof( sOldSuit ) );

		sSuitOp.Assign( sOldSuit );
	}
    else if ( dwVer==ITEM::SSUIT::VERSION )
    {
        GASSERT(sizeof(ITEM::SSUIT)==dwSize);
        SFile.ReadBuffer ( &sSuitOp, sizeof(ITEM::SSUIT) );
    }
    else
    {
        MessageBox ( NULL, "SITEM::LoadFile(), ITEM::SSUIT unknown data version.", "ERROR", MB_OK );
        SFile.SetOffSet ( SFile.GetfTell()+dwSize );
    }
}

void SITEM::LoadFileDrug( sc::BaseStream &SFile )
{
    DWORD dwVer, dwSize;
    SFile >> dwVer;
    SFile >> dwSize;

    if ( dwVer==0x0100 )
    {
        ITEM::SDRUG_100 sOLD;
        GASSERT(sizeof(sOLD)==dwSize);
        SFile.ReadBuffer ( &sOLD, sizeof(sOLD) );

        sDrugOp.Assign ( sOLD );
    }
    else if ( dwVer==0x0101 )
    {
        ITEM::SDRUG_101 sOLD;
        GASSERT(sizeof(sOLD)==dwSize);
        SFile.ReadBuffer ( &sOLD, sizeof(sOLD) );

        sDrugOp.Assign ( sOLD );
    }
	else if ( dwVer==0x0102 )
	{
		ITEM::SDRUG_102 sOLD;
		GASSERT(sizeof(sOLD)==dwSize);
		SFile.ReadBuffer ( &sOLD, sizeof(sOLD) );

		sDrugOp.Assign ( sOLD );
	}
	else if ( dwVer==0x0103 )
	{
		ITEM::SDRUG_103 sOLD;
		GASSERT(sizeof(sOLD)==dwSize);
		SFile.ReadBuffer ( &sOLD, sizeof(sOLD) );

		sDrugOp.Assign ( sOLD );
	}
	else if ( dwVer==0x0104 )
	{
		ITEM::SDRUG_104 sOLD;
		GASSERT(sizeof(sOLD)==dwSize);
		SFile.ReadBuffer ( &sOLD, sizeof(sOLD) );

		sDrugOp.Assign ( sOLD );
	}
    else if ( dwVer==ITEM::SDRUG::VERSION )
    {
        GASSERT(sizeof(ITEM::SDRUG)==dwSize);
        SFile.ReadBuffer ( &sDrugOp, sizeof(ITEM::SDRUG) );
    }
    else
    {
        MessageBox ( NULL, "SITEM::LoadFile(), ITEM::SDRUG unknown data version.", "ERROR", MB_OK );
        SFile.SetOffSet ( SFile.GetfTell()+dwSize );
    }
}

void SITEM::LoadFileSkillBook( sc::BaseStream &SFile )
{
    DWORD dwVer, dwSize;
    SFile >> dwVer;
    SFile >> dwSize;

    if ( dwVer==0x0100 )
    {
        GASSERT(sizeof(ITEM::SSKILLBOOK)==dwSize);
        SFile.ReadBuffer ( &sSkillBookOp, sizeof(ITEM::SSKILLBOOK) );
    }
    else if ( dwVer==0x0101 )
    {
        ITEM::SSKILLBOOK_101 sSkillbook;

        GASSERT(sizeof(ITEM::SSKILLBOOK_101)==dwSize);
        SFile.ReadBuffer ( &sSkillbook, sizeof(ITEM::SSKILLBOOK_101) );

        sSkillBookOp.sSkill_ID    = sSkillbook.sSkill_ID;
        sSkillBookOp.wSkill_Level = 0;
    }
    else if ( dwVer==ITEM::SSKILLBOOK::VERSION )
    {
        GASSERT(sizeof(ITEM::SSKILLBOOK)==dwSize);
        SFile.ReadBuffer ( &sSkillBookOp, sizeof(ITEM::SSKILLBOOK) );
    }
    else
    {
        MessageBox ( NULL, "SITEM::LoadFile(), ITEM::SSKILLBOOK unknown data version.", "ERROR", MB_OK );
        SFile.SetOffSet ( SFile.GetfTell()+dwSize );
    }
}

void SITEM::LoadFileGenerate( sc::BaseStream &SFile )
{
    DWORD dwVer, dwSize;
    SFile >> dwVer;
    SFile >> dwSize;

    if ( dwVer== 0x0100 )
    {
        ITEM::SGENERATE_100 sOLD;
        GASSERT(sizeof(sOLD)==dwSize);
        SFile.ReadBuffer ( &sOLD, sizeof(sOLD) );

        sGenerateOp.Assign ( sOLD );
    }
    else if ( dwVer==ITEM::SGENERATE::VERSION )
    {
        GASSERT(sizeof(ITEM::SGENERATE)==dwSize);
        SFile.ReadBuffer ( &sGenerateOp, sizeof(ITEM::SGENERATE) );
    }
    else
    {
        MessageBox ( NULL, "SITEM::LoadFile(), ITEM::SGENERATE unknown data version.", "ERROR", MB_OK );
        SFile.SetOffSet ( SFile.GetfTell()+dwSize );
    }
}
