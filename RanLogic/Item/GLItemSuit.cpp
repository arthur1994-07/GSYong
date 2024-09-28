#include "../pch.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "./GLItemSuit.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ITEM
{
    GLITEM_ATT assign_att ( glold::GLITEM_ATT_102 emOld )
    {
        GLITEM_ATT emAttack;
        switch ( emOld )
        {
        case glold::ITEMATT_NOTHING:
            emAttack = ITEMATT_NOTHING;
            break;

        case glold::ITEMATT_SWORD:
            emAttack = ITEMATT_SWORD;
            break;
        case glold::ITEMATT_REV00:
            emAttack = ITEMATT_SWORD;	//	sword 로 강제 변환.
            break;
        case glold::ITEMATT_DAGGER:
            emAttack = ITEMATT_DAGGER;
            break;
        case glold::ITEMATT_SPEAR:
            emAttack = ITEMATT_SPEAR;
            break;
        case glold::ITEMATT_BOW:
            emAttack = ITEMATT_BOW; 
            break;
        case glold::ITEMATT_THROW:
            emAttack = ITEMATT_THROW;
            break;

        case glold::ITEMATT_NOCARE:
            emAttack = ITEMATT_NOCARE;
            break;

        default:
            emAttack = ITEMATT_NOTHING;
            break;
        };

        return emAttack;
    }

    GLITEM_ATT assign_att ( glold_103::GLITEM_ATT_103 emOld )
    {
        GLITEM_ATT emAttack;
        switch ( emOld )
        {
        case glold_103::ITEMATT_NOTHING:
            emAttack = ITEMATT_NOTHING;
            break;

        case glold_103::ITEMATT_SWORD:
            emAttack = ITEMATT_SWORD;
            break;
        case glold_103::ITEMATT_SABER:
            emAttack = ITEMATT_SABER;
            break;
        case glold_103::ITEMATT_DAGGER:
            emAttack = ITEMATT_DAGGER;
            break;
        case glold_103::ITEMATT_SPEAR:
            emAttack = ITEMATT_SPEAR;
            break;

        case glold_103::ITEMATT_STICK:
            emAttack = ITEMATT_STICK;
            break;

        case glold_103::ITEMATT_BOW:
            emAttack = ITEMATT_BOW; 
            break;
        case glold_103::ITEMATT_THROW:
            emAttack = ITEMATT_THROW;
            break;

        case glold_103::ITEMATT_NOCARE:
            emAttack = ITEMATT_NOCARE;
            break;

        default:
            emAttack = ITEMATT_NOTHING;
            break;
        };

        return emAttack;
    }


    GLITEM_ATT assign_att ( glold_104::GLITEM_ATT_104 emOld )
    {
        GLITEM_ATT emAttack;
        switch ( emOld )
        {
        case glold_104::ITEMATT_NOTHING:
            emAttack = ITEMATT_NOTHING;
            break;

        case glold_104::ITEMATT_SWORD:
            emAttack = ITEMATT_SWORD;
            break;
        case glold_104::ITEMATT_SABER:
            emAttack = ITEMATT_SABER;
            break;

        case glold_104::ITEMATT_DAGGER:
            emAttack = ITEMATT_DAGGER;
            break;
        case glold_104::ITEMATT_SPEAR:
            emAttack = ITEMATT_SPEAR;
            break;

        case glold_104::ITEMATT_STICK:
            emAttack = ITEMATT_STICK;
            break;

        case glold_104::ITEMATT_GWON:
            emAttack = ITEMATT_GWON;
            break;

        case glold_104::ITEMATT_BOW:
            emAttack = ITEMATT_BOW; 
            break;

        case glold_104::ITEMATT_THROW:
            emAttack = ITEMATT_THROW;
            break;

        case glold_104::ITEMATT_NOCARE:
            emAttack = ITEMATT_NOCARE;
            break;

        default:
            emAttack = ITEMATT_NOTHING;
            break;
        };

        return emAttack;
    }

	GLITEM_ATT assign_att ( glold_105::GLITEM_ATT_105 emOld )
    {
        GLITEM_ATT emAttack;
        switch ( emOld )
        {
        case glold_105::ITEMATT_NOTHING:
            emAttack = ITEMATT_NOTHING;
            break;

        case glold_105::ITEMATT_SWORD:
            emAttack = ITEMATT_SWORD;
            break;
        case glold_105::ITEMATT_SABER:
            emAttack = ITEMATT_SABER;
            break;

        case glold_105::ITEMATT_DAGGER:
            emAttack = ITEMATT_DAGGER;
            break;
        case glold_105::ITEMATT_SPEAR:
            emAttack = ITEMATT_SPEAR;
            break;

        case glold_105::ITEMATT_STICK:
            emAttack = ITEMATT_STICK;
            break;

        case glold_105::ITEMATT_GWON:
            emAttack = ITEMATT_GWON;
            break;

        case glold_105::ITEMATT_BOW:
            emAttack = ITEMATT_BOW; 
            break;

        case glold_105::ITEMATT_THROW:
            emAttack = ITEMATT_THROW;
            break;

		case glold_105::ITEMATT_PISTOL:
			emAttack = ITEMATT_PISTOL;
			break;
				
		case glold_105::ITEMATT_RAILGUN:
			emAttack = ITEMATT_RAILGUN;
			break;
				
		case glold_105::ITEMATT_PORTALGUN:
			emAttack = ITEMATT_PORTALGUN;
			break;

		case glold_105::ITEMATT_HAMMER:
			emAttack = ITEMATT_SIDE;
			break;
				
		case glold_105::ITEMATT_DUALSPEAR:
			emAttack = ITEMATT_DUALSPEAR;
			break; 
				
		case glold_105::ITEMATT_SIDE:
			emAttack = ITEMATT_THROWING_KNIFE;
			break;

        case glold_105::ITEMATT_NOCARE:
            emAttack = ITEMATT_NOCARE;
            break;

        default:
            emAttack = ITEMATT_NOTHING;
            break;
        };

        return emAttack;
    }

	GLITEM_ATT assign_att ( glold_108::GLITEM_ATT_108 emOld )
	{
		GLITEM_ATT emAttack;
		switch ( emOld )
		{
		case glold_108::ITEMATT_NOTHING:
			emAttack = ITEMATT_NOTHING;
			break;

		case glold_108::ITEMATT_SWORD:
			emAttack = ITEMATT_SWORD;
			break;
		case glold_108::ITEMATT_SABER:
			emAttack = ITEMATT_SABER;
			break;

		case glold_108::ITEMATT_DAGGER:
			emAttack = ITEMATT_DAGGER;
			break;
		case glold_108::ITEMATT_SPEAR:
			emAttack = ITEMATT_SPEAR;
			break;

		case glold_108::ITEMATT_STICK:
			emAttack = ITEMATT_STICK;
			break;

		case glold_108::ITEMATT_GWON:
			emAttack = ITEMATT_GWON;
			break;

		case glold_108::ITEMATT_BOW:
			emAttack = ITEMATT_BOW; 
			break;

		case glold_108::ITEMATT_THROW:
			emAttack = ITEMATT_THROW;
			break;

		case glold_108::ITEMATT_PISTOL:
			emAttack = ITEMATT_PISTOL;
			break;

		case glold_108::ITEMATT_RAILGUN:
			emAttack = ITEMATT_RAILGUN;
			break;

		case glold_108::ITEMATT_PORTALGUN:
			emAttack = ITEMATT_PORTALGUN;
			break;

		case glold_108::ITEMATT_HAMMER:
			emAttack = ITEMATT_SIDE;
			break;

		case glold_108::ITEMATT_DUALSPEAR:
			emAttack = ITEMATT_DUALSPEAR;
			break; 

		case glold_108::ITEMATT_SIDE:
			emAttack = ITEMATT_THROWING_KNIFE;
			break;

		case glold_108::ITEMATT_EXTREME_GLOVE:
			emAttack = ITEMATT_EXTREME_GLOVE;
			break;
		case glold_108::ITEMATT_TRICK_STICK:
			emAttack = ITEMATT_TRICK_STICK;
			break;
		case glold_108::ITEMATT_TRICK_BOX:
			emAttack = ITEMATT_TRICK_BOX;
			break;
		case glold_108::ITEMATT_TRICK_WHIP:
			emAttack = ITEMATT_TRICK_WHIP;
			break;
		case glold_108::ITEMATT_NOCARE:
			emAttack = ITEMATT_NOCARE;
			break;

		default:
			emAttack = ITEMATT_NOTHING;
			break;
		};

		return emAttack;
	}

    void SSUIT_114::Assign ( SSUIT_100 &Suit100 )
    {
        emSuit			= Suit100.emSuit;
        SETBOTHHAND ( cast_bool(Suit100.bBothHand) );
        SETBIG ( false );
        emHand			= Suit100.emHand;

        emAttack		= assign_att ( Suit100.emAttack );

        wAttRange		= Suit100.wAttRange;

        nHitRate		= Suit100.nHitRate;
        nAvoidRate		= Suit100.nAvoidRate;

        gdDamage		= Suit100.gdDamage;
        nDefense		= Suit100.nDefense;

        sResist			= Suit100.sResist;

        wReModelNum		= Suit100.wReModelNum;

        //	특수 부가 효과들.
        memcpy( sADDON, Suit100.sADDON, sizeof(SADDON)*ADDON_SIZE);

        sVARIATE		= Suit100.sVARIATE;
        sBLOW.emTYPE	= Suit100.sBLOW.emTYPE;
        sBLOW.fRATE		= Suit100.sBLOW.fRATE;
        sBLOW.fLIFE		= Suit100.sBLOW.fLIFE;
        sBLOW.fVAR1		= Suit100.sBLOW.fVALUE;
        sBLOW.fVAR2		= 0.0f;
    }

    void SSUIT_114::Assign ( SSUIT_101 &Suit101 )
    {
        emSuit			= Suit101.emSuit;

        SETBOTHHAND ( cast_bool(Suit101.bBothHand) );
        SETBIG ( false );

        emHand			= Suit101.emHand;

        emAttack		= assign_att ( Suit101.emAttack );
        if ( emAttack==glold::ITEMATT_REV00 ) // different enum types are compared
            emAttack = ITEMATT_SWORD;

        wAttRange		= Suit101.wAttRange;

        nHitRate		= Suit101.nHitRate;
        nAvoidRate		= Suit101.nAvoidRate;

        gdDamage		= Suit101.gdDamage;
        nDefense		= Suit101.nDefense;

        sResist			= Suit101.sResist;

        wReModelNum		= Suit101.wReModelNum;

        //	특수 부가 효과들.
        memcpy( sADDON, Suit101.sADDON, sizeof(SADDON)*ADDON_SIZE);

        sVARIATE		= Suit101.sVARIATE;
        sBLOW			= Suit101.sBLOW;
    }

    void SSUIT_114::Assign ( SSUIT_102 &Suit102 )
    {
        emSuit			= Suit102.emSuit;
        SETBOTHHAND ( Suit102.bBothHand );
        SETBIG ( Suit102.bBig );

        emHand			= Suit102.emHand;

        emAttack		= assign_att ( Suit102.emAttack );
        wAttRange		= Suit102.wAttRange;
        wReqSP			= Suit102.wReqSP;

        nHitRate		= Suit102.nHitRate;
        nAvoidRate		= Suit102.nAvoidRate;

        gdDamage		= Suit102.gdDamage;
        nDefense		= Suit102.nDefense;

        sResist			= Suit102.sResist;

        wReModelNum		= Suit102.wReModelNum;

        for ( int i=0; i<SSUIT_102::ADDON_SIZE; ++i )		sADDON[i] = Suit102.sADDON[i];
        sVARIATE		= Suit102.sVARIATE;
        sBLOW			= Suit102.sBLOW;
    }

    void SSUIT_114::Assign ( SSUIT_103 &Suit103 )
    {
        emSuit			= Suit103.emSuit;
        SETBOTHHAND ( Suit103.bBothHand );
        SETBIG ( Suit103.bBig );

        emHand			= Suit103.emHand;

        emAttack		= assign_att ( Suit103.emAttack );
        wAttRange		= Suit103.wAttRange;
        wReqSP			= Suit103.wReqSP;

        nHitRate		= Suit103.nHitRate;
        nAvoidRate		= Suit103.nAvoidRate;

        gdDamage		= Suit103.gdDamage;
        nDefense		= Suit103.nDefense;

        sResist			= Suit103.sResist;

        wReModelNum		= Suit103.wReModelNum;

        for ( int i=0; i<SSUIT_103::ADDON_SIZE; ++i )		sADDON[i] = Suit103.sADDON[i];
        sVARIATE		= Suit103.sVARIATE;
        sBLOW			= Suit103.sBLOW;
    }

    void SSUIT_114::Assign ( SSUIT_104 &Suit104 )
    {
        emSuit = Suit104.emSuit;
        SETBOTHHAND ( Suit104.bBothHand );
        SETBIG ( Suit104.bBig );

        emHand = Suit104.emHand;

        emAttack = assign_att ( Suit104.emAttack );
        wAttRange = Suit104.wAttRange;
        wReqSP = Suit104.wReqSP;

        nHitRate = Suit104.nHitRate;
        nAvoidRate = Suit104.nAvoidRate;

        gdDamage = Suit104.gdDamage;
        nDefense = Suit104.nDefense;

        sResist.Assign ( Suit104.sResist );

        wReModelNum = Suit104.wReModelNum;

        //	특수 부가 효과들.
        for ( int i=0; i<SSUIT_104::ADDON_SIZE; ++i )		sADDON[i] = Suit104.sADDON[i];
        sVARIATE = Suit104.sVARIATE;
        sBLOW = Suit104.sBLOW;
    }

    void SSUIT_114::Assign ( SSUIT_105 &Suit105 )
    {
        emSuit = Suit105.emSuit;
        SETBOTHHAND ( Suit105.bBothHand );
        SETBIG ( Suit105.bBig );

        emHand = Suit105.emHand;

        emAttack = assign_att ( Suit105.emAttack );
        wAttRange = Suit105.wAttRange;
        wReqSP = Suit105.wReqSP;

        nHitRate = Suit105.nHitRate;
        nAvoidRate = Suit105.nAvoidRate;

        gdDamage = Suit105.gdDamage;
        nDefense = Suit105.nDefense;

        sResist = Suit105.sResist;

        wReModelNum = Suit105.wReModelNum;

        for ( int i=0; i<SSUIT_105::ADDON_SIZE; ++i )		sADDON[i] = Suit105.sADDON[i];
        sVARIATE = Suit105.sVARIATE;
        sBLOW = Suit105.sBLOW;
    }

    void SSUIT_114::Assign ( SSUIT_106 &sSuitOld ) // By 경대
    {
        emSuit = sSuitOld.emSuit;
        //SETBOTHHAND ( sSuitOld.bBothHand );
        //SETBIG ( sSuitOld.bBig );
        dwHAND = sSuitOld.dwHAND;

        emHand = sSuitOld.emHand;

        emAttack = assign_att ( sSuitOld.emAttack );
        wAttRange = sSuitOld.wAttRange;
        wReqSP = sSuitOld.wReqSP;

        nHitRate = sSuitOld.nHitRate;
        nAvoidRate = sSuitOld.nAvoidRate;

        gdDamage = sSuitOld.gdDamage;
        nDefense = sSuitOld.nDefense;

        sResist = sSuitOld.sResist;

        wReModelNum = sSuitOld.wReModelNum;

        for ( int i=0; i<SSUIT_106::ADDON_SIZE; ++i )		sADDON[i] = sSuitOld.sADDON[i];
        sVARIATE = sSuitOld.sVARIATE;
        sBLOW = sSuitOld.sBLOW;
    }

    void SSUIT_114::Assign ( SSUIT_107 &sSuitOld ) // By 경대
    {
        emSuit = sSuitOld.emSuit;
        //SETBOTHHAND ( sSuitOld.bBothHand );
        //SETBIG ( sSuitOld.bBig );
        dwHAND = sSuitOld.dwHAND;

        emHand = sSuitOld.emHand;

        emAttack = assign_att ( sSuitOld.emAttack );
        wAttRange = sSuitOld.wAttRange;
        wReqSP = sSuitOld.wReqSP;

        nHitRate = sSuitOld.nHitRate;
        nAvoidRate = sSuitOld.nAvoidRate;

        gdDamage = sSuitOld.gdDamage;
        nDefense = sSuitOld.nDefense;

        sResist = sSuitOld.sResist;

        wReModelNum = sSuitOld.wReModelNum;

        for ( int i=0; i<SSUIT_107::ADDON_SIZE; ++i )		sADDON[i] = sSuitOld.sADDON[i];
        sVARIATE = sSuitOld.sVARIATE;
        sBLOW = sSuitOld.sBLOW;
        sVOLUME = sSuitOld.sVOLUME;					
    }

    void SSUIT_114::Assign ( SSUIT_108 &sSuitOld ) // By 경대
    {
        emSuit = sSuitOld.emSuit;
        dwHAND = sSuitOld.dwHAND;
        emHand = sSuitOld.emHand;

        gdDamage = sSuitOld.gdDamage;
        nDefense = sSuitOld.nDefense;

        nHitRate = sSuitOld.nHitRate;
        nAvoidRate = sSuitOld.nAvoidRate;

        sResist = sSuitOld.sResist;

        emAttack = assign_att ( sSuitOld.emAttack );

        wAttRange = sSuitOld.wAttRange;
        wReqSP = sSuitOld.wReqSP;

        sVARIATE = sSuitOld.sVARIATE;
        sVOLUME = sSuitOld.sVOLUME;					
        sBLOW = sSuitOld.sBLOW;
        for ( int i=0; i<SSUIT_108::ADDON_SIZE; ++i )		sADDON[i] = sSuitOld.sADDON[i];

        wReModelNum = sSuitOld.wReModelNum;
    }

    void SSUIT_114::Assign ( SSUIT_109 &sSuitOld ) // by luxes.
    {
        emSuit = sSuitOld.emSuit;
        dwHAND = sSuitOld.dwHAND;
        emHand = sSuitOld.emHand;

        gdDamage = sSuitOld.gdDamage;
        nDefense = sSuitOld.nDefense;

        nHitRate = sSuitOld.nHitRate;
        nAvoidRate = sSuitOld.nAvoidRate;

        sResist = sSuitOld.sResist;

        emAttack = sSuitOld.emAttack;

        wAttRange = sSuitOld.wAttRange;
        wReqSP = sSuitOld.wReqSP;

        sVARIATE = sSuitOld.sVARIATE;
        sVOLUME = sSuitOld.sVOLUME;					
        sBLOW = sSuitOld.sBLOW;
        for ( int i=0; i<SSUIT_109::ADDON_SIZE; ++i )		sADDON[i] = sSuitOld.sADDON[i];

        wReModelNum = sSuitOld.wReModelNum;

        //bChangeColor = false;
    }

    void SSUIT_114::Assign( SSUIT_110 &sSuitOld )
    {
        emSuit = sSuitOld.emSuit;
        dwHAND = sSuitOld.dwHAND;
        emHand = sSuitOld.emHand;

        gdDamage = sSuitOld.gdDamage;
        nDefense = sSuitOld.nDefense;

        nHitRate = sSuitOld.nHitRate;
        nAvoidRate = sSuitOld.nAvoidRate;

        sResist = sSuitOld.sResist;

        emAttack = sSuitOld.emAttack;

        wAttRange = sSuitOld.wAttRange;
        wReqSP = sSuitOld.wReqSP;

        sVARIATE = sSuitOld.sVARIATE;
        sVOLUME = sSuitOld.sVOLUME;					
        sBLOW = sSuitOld.sBLOW;
        for ( int i=0; i<SSUIT_110::ADDON_SIZE; ++i )		sADDON[i] = sSuitOld.sADDON[i];

        wReModelNum = sSuitOld.wReModelNum;

        nItemSuitState += (sSuitOld.bChangeColor ? EMITEM_SUIT_CHANGE_COLOR : EMITEM_SUIT_NULL);

        wDurabilityMax = 0;
        dwRepairPrice = 0;
    }

	void SSUIT_114::Assign ( SSUIT_111 &sSuitOld )
	{
		emSuit = sSuitOld.emSuit;
		dwHAND = sSuitOld.dwHAND;
		emHand = sSuitOld.emHand;

		gdDamage = sSuitOld.gdDamage;
		nDefense = sSuitOld.nDefense;

		nHitRate = sSuitOld.nHitRate;
		nAvoidRate = sSuitOld.nAvoidRate;

		sResist = sSuitOld.sResist;

		emAttack = sSuitOld.emAttack;

		wAttRange = sSuitOld.wAttRange;
		wReqSP = sSuitOld.wReqSP;

		sVARIATE = sSuitOld.sVARIATE;
		sVOLUME = sSuitOld.sVOLUME;					
		sBLOW = sSuitOld.sBLOW;
		for ( int i=0; i<SSUIT_110::ADDON_SIZE; ++i )		sADDON[i] = sSuitOld.sADDON[i];

		wReModelNum = sSuitOld.wReModelNum;

		nItemSuitState += (sSuitOld.bChangeColor ? EMITEM_SUIT_CHANGE_COLOR : EMITEM_SUIT_NULL);

		wDurabilityMax = sSuitOld.wDurabilityMax;
		dwRepairPrice = sSuitOld.dwRepairPrice;
	}

	void SSUIT_114::Assign ( SSUIT_112 &sSuitOld )
	{
		emSuit = sSuitOld.emSuit;
		dwHAND = sSuitOld.dwHAND;
		emHand = sSuitOld.emHand;

		gdDamage = sSuitOld.gdDamage;
		nDefense = sSuitOld.nDefense;

		nHitRate = sSuitOld.nHitRate;
		nAvoidRate = sSuitOld.nAvoidRate;

		sResist = sSuitOld.sResist;

		emAttack = assign_att ( sSuitOld.emAttack );

		wAttRange = sSuitOld.wAttRange;
		wReqSP = sSuitOld.wReqSP;

		sVARIATE = sSuitOld.sVARIATE;
		sVOLUME = sSuitOld.sVOLUME;					
		sBLOW = sSuitOld.sBLOW;
		for ( int i=0; i<SSUIT_112::ADDON_SIZE; ++i )		sADDON[i] = sSuitOld.sADDON[i];

		wReModelNum = sSuitOld.wReModelNum;

		nItemSuitState = sSuitOld.nItemSuitState;

		wDurabilityMax = sSuitOld.wDurabilityMax;
		dwRepairPrice = sSuitOld.dwRepairPrice;
	}

	void SSUIT_114::Assign ( SSUIT_113& sSuitOld )
	{
		emSuit = sSuitOld.emSuit;
		dwHAND = sSuitOld.dwHAND;
		emHand = sSuitOld.emHand;

		gdDamage = sSuitOld.gdDamage;
		nDefense = sSuitOld.nDefense;

		nHitRate = sSuitOld.nHitRate;
		nAvoidRate = sSuitOld.nAvoidRate;

		sResist = sSuitOld.sResist;

		emAttack = sSuitOld.emAttack;

		wAttRange = sSuitOld.wAttRange;
		wReqSP = sSuitOld.wReqSP;

		sVARIATE = sSuitOld.sVARIATE;
		sVOLUME = sSuitOld.sVOLUME;					
		sBLOW = sSuitOld.sBLOW;
		for ( int i=0; i<SSUIT_113::ADDON_SIZE; ++i )		sADDON[i] = sSuitOld.sADDON[i];

		wReModelNum = sSuitOld.wReModelNum;

		nItemSuitState = sSuitOld.nItemSuitState;

		wDurabilityMax = sSuitOld.wDurabilityMax;
		dwRepairPrice = sSuitOld.dwRepairPrice;
	}

	void SSUIT::Assign ( SSUIT_100 &sSuitOld )
	{
		SSUIT_114 sSuit;
		sSuit.Assign(sSuitOld);
		Assign(sSuit);
	}

	void SSUIT::Assign ( SSUIT_101 &sSuitOld )
	{
		SSUIT_114 sSuit;
		sSuit.Assign(sSuitOld);
		Assign(sSuit);
	}

	void SSUIT::Assign ( SSUIT_102 &sSuitOld )
	{
		SSUIT_114 sSuit;
		sSuit.Assign(sSuitOld);
		Assign(sSuit);
	}

	void SSUIT::Assign ( SSUIT_103 &sSuitOld )
	{
		SSUIT_114 sSuit;
		sSuit.Assign(sSuitOld);
		Assign(sSuit);
	}

	void SSUIT::Assign ( SSUIT_104 &sSuitOld )
	{
		SSUIT_114 sSuit;
		sSuit.Assign(sSuitOld);
		Assign(sSuit);
	}

	void SSUIT::Assign ( SSUIT_105 &sSuitOld )
	{
		SSUIT_114 sSuit;
		sSuit.Assign(sSuitOld);
		Assign(sSuit);
	}

	void SSUIT::Assign ( SSUIT_106 &sSuitOld )
	{
		SSUIT_114 sSuit;
		sSuit.Assign(sSuitOld);
		Assign(sSuit);
	}

	void SSUIT::Assign ( SSUIT_107 &sSuitOld )
	{
		SSUIT_114 sSuit;
		sSuit.Assign(sSuitOld);
		Assign(sSuit);
	}

	void SSUIT::Assign ( SSUIT_108 &sSuitOld )
	{
		SSUIT_114 sSuit;
		sSuit.Assign(sSuitOld);
		Assign(sSuit);
	}

	void SSUIT::Assign ( SSUIT_109 &sSuitOld )
	{
		SSUIT_114 sSuit;
		sSuit.Assign(sSuitOld);
		Assign(sSuit);
	}

	void SSUIT::Assign ( SSUIT_110 &sSuitOld )
	{
		SSUIT_114 sSuit;
		sSuit.Assign(sSuitOld);
		Assign(sSuit);
	}

	void SSUIT::Assign ( SSUIT_111 &sSuitOld )
	{
		SSUIT_114 sSuit;
		sSuit.Assign(sSuitOld);
		Assign(sSuit);
	}

	void SSUIT::Assign ( SSUIT_112 &sSuitOld )
	{
		SSUIT_114 sSuit;
		sSuit.Assign(sSuitOld);
		Assign(sSuit);
	}

	void SSUIT::Assign ( SSUIT_113& sSuitOld )
	{
		SSUIT_114 sSuit;
		sSuit.Assign(sSuitOld);
		Assign(sSuit);
	}

	void SSUIT::Assign ( SSUIT_114& sSuitOld )
	{
		emSuit = sSuitOld.emSuit;
		dwHAND = sSuitOld.dwHAND;
		emHand = sSuitOld.emHand;

		gdDamage = sSuitOld.gdDamage;
		nDefense = sSuitOld.nDefense;

		nHitRate = sSuitOld.nHitRate;
		nAvoidRate = sSuitOld.nAvoidRate;

		sResist = sSuitOld.sResist;

		emAttack = sSuitOld.emAttack;

		wAttRange = sSuitOld.wAttRange;
		wReqSP = sSuitOld.wReqSP;

		sBLOW = sSuitOld.sBLOW;

		wReModelNum = sSuitOld.wReModelNum;

		nItemSuitState = sSuitOld.nItemSuitState;

		wDurabilityMax = sSuitOld.wDurabilityMax;
		dwRepairPrice = sSuitOld.dwRepairPrice;

		nBasicAttackDamageIndex = sSuitOld.nBasicAttackDamageIndex;
		nBasicDefenceIndex = sSuitOld.nBasicDefenceIndex;

		bEditableBasicAttackDamage = sSuitOld.bEditableBasicAttackDamage;
		bEditableBasicDefence = sSuitOld.bEditableBasicDefence;

		// sVARIATE = sSuitOld.sVARIATE;
		// sVOLUME = sSuitOld.sVOLUME;	
		for ( int i=0; i<SSUIT_114::ADDON_SIZE; ++i )		
			sADDON[i] = sSuitOld.sADDON[i];

		// 115 버전 이후로는 SADDON, SVAR, SVOL을 SADDON하나로 합친다;
		// convert;
		{
			INT nAddonSpareIdx = 0;
			for ( ; sADDON[nAddonSpareIdx].emTYPE != EMADD_NONE; ++nAddonSpareIdx )	{}

			// 기존의 EMITEM_ADDON과 EMITEM_VAR 차이는 17이다;
			// 기존에 SVAR와 SVOL는 하나의 타입 EMITEM_VAR를 사용해왔다;
			// 특이점이 일부는 공용으로 사용하고 일부는 따로사용하는데;
			// EMVAR_CRUSHING_BLOW까지는 공용, 이후는 따로 사용한다;
			// 꽤 복잡한데 이 컨버팅이 잘되면 이부분은 볼필요없으므로 더 이상 설명은 생략한다;
			// 컨버팅이 끝났을때 제발 문제없기를 제발;
			if ( sSuitOld.sVARIATE.emTYPE != EMVAR_NONE )
			{
				if ( sSuitOld.sVARIATE.emTYPE <= EMVAR_CRUSHING_BLOW )
				{
					sADDON[nAddonSpareIdx].emTYPE = static_cast<EMITEM_ADDON>(sSuitOld.sVARIATE.emTYPE + EMADDEX_INCR_NONE);
					sADDON[nAddonSpareIdx].fVALUE = sSuitOld.sVARIATE.fVariate;
				}
				else
				{
					sADDON[nAddonSpareIdx].emTYPE = static_cast<EMITEM_ADDON>(sSuitOld.sVARIATE.emTYPE + EMADDEX_INCR_IGNORE_BOSSDAMAGE);
					sADDON[nAddonSpareIdx].fVALUE = sSuitOld.sVARIATE.fVariate;
				}
				++nAddonSpareIdx;
			}
			if ( sSuitOld.sVOLUME.emTYPE != EMVAR_NONE )
			{
				if ( sSuitOld.sVARIATE.emTYPE <= EMVAR_CRUSHING_BLOW )
				{
					sADDON[nAddonSpareIdx].emTYPE = static_cast<EMITEM_ADDON>(sSuitOld.sVOLUME.emTYPE + EMADDEX_INC_NONE);
					sADDON[nAddonSpareIdx].fVALUE = sSuitOld.sVOLUME.fVolume;
				}
				else
				{
					sADDON[nAddonSpareIdx].emTYPE = static_cast<EMITEM_ADDON>(sSuitOld.sVOLUME.emTYPE + EMADDEX_INCR_IGNORE_BOSSDAMAGE);
					sADDON[nAddonSpareIdx].fVALUE = sSuitOld.sVOLUME.fVolume;
				}
			}
		}
	}

	void SSUIT::Assign ( SSUIT_115& sSuitOld )
	{
		emSuit = sSuitOld.emSuit;
		dwHAND = sSuitOld.dwHAND;
		emHand = sSuitOld.emHand;

		gdDamage = sSuitOld.gdDamage;
		nDefense = sSuitOld.nDefense;

		nHitRate = sSuitOld.nHitRate;
		nAvoidRate = sSuitOld.nAvoidRate;

		sResist = sSuitOld.sResist;

		emAttack = sSuitOld.emAttack;

		wAttRange = sSuitOld.wAttRange;
		wReqSP = sSuitOld.wReqSP;

		sBLOW = sSuitOld.sBLOW;
		for ( int i=0; i<SSUIT_115::ADDON_SIZE; ++i )		
			sADDON[i] = sSuitOld.sADDON[i];
		strcpy( szRandomAddonScript, sSuitOld.szRandomAddonScript );

		wReModelNum = sSuitOld.wReModelNum;

		nItemSuitState = sSuitOld.nItemSuitState;

		wDurabilityMax = sSuitOld.wDurabilityMax;
		dwRepairPrice = sSuitOld.dwRepairPrice;

		nBasicAttackDamageType = -1;
		nBasicAttackDamageIndex = sSuitOld.nBasicAttackDamageIndex;

		nBasicDefenceType = -1;
		nBasicDefenceIndex = sSuitOld.nBasicDefenceIndex;

		bEditableBasicAttackDamage = sSuitOld.bEditableBasicAttackDamage;
		bEditableBasicDefence = sSuitOld.bEditableBasicDefence;
	}

    VOID SSUIT::SaveCsvHead ( std::fstream &SFile )
    {
        SFile << "emSuit" << ",";        
        SFile << "dwHAND" << ",";
        SFile << "emHand" << ",";

        SFile << "emAttack" << ",";
        SFile << "wAttRange" << ",";
        SFile << "wReqSP" << ",";

        SFile << "nHitRate" << ",";
        SFile << "nAvoidRate" << ",";

        SFile << "gdDamage wLow" << ",";
        SFile << "gdDamage wHigh" << ",";
        SFile << "nDefense" << ",";

        SFile << "sResist nFire" << ",";
        SFile << "sResist nIce" << ",";
        SFile << "sResist nElectric" << ",";
        SFile << "sResist nPoison" << ",";
        SFile << "sResist nSpirit" << ",";

        SFile << "wReModelNum" << ",";

        for( int i=0 ;i<ADDON_SIZE; ++i )
        {
            SFile << "sADDON " << i << " emTYPE" << ",";
            SFile << "sADDON " << i << " nVALUE" << ",";
        }

        SFile << "sBLOW emTYPE" << ",";
        SFile << "sBLOW fRATE" << ",";
        SFile << "sBLOW fLIFE" << ",";
        SFile << "sBLOW fVAR1" << ",";
        SFile << "sBLOW fVAR2" << ",";
		SFile << "Random Addon Script" << ",";

		SFile << "bChangeColor" << ",";
		SFile << "bUniqueEquip" << ",";

        SFile << "dwDurability Max" << ",";
        SFile << "dwRepair Price" << ",";

		SFile << "nBasicAttackDamageType" << ",";
		SFile << "nBasicAttackDamageIndex" << ",";
		SFile << "nBasicDefenceType" << ",";
		SFile << "nBasicDefenceIndex" << ",";

		SFile << "bEditableBasicAttackDamage" << ",";
		SFile << "bEditableBasicDefence" << ",";
    }

    VOID SSUIT::SaveCsv ( std::fstream &SFile )
    {
        SFile << emSuit << ",";        
        SFile << dwHAND << ",";
        SFile << emHand << ",";

        SFile << emAttack << ",";
        SFile << wAttRange << ",";
        SFile << wReqSP << ",";

        SFile << nHitRate << ",";
        SFile << nAvoidRate << ",";

        SFile << gdDamage.wLow << ",";
        SFile << gdDamage.wHigh << ",";
        SFile << nDefense << ",";

        SFile << sResist.nFire << ",";
        SFile << sResist.nIce << ",";
        SFile << sResist.nElectric << ",";
        SFile << sResist.nPoison << ",";
        SFile << sResist.nSpirit << ",";

        SFile << wReModelNum << ",";

        for( int i=0 ;i<ADDON_SIZE; ++i )
        {
            SFile << sADDON[i].emTYPE << ",";

			const int nValue = GetAddonValue_CompareExchange< const int >( i, ITEM::bAddonValueInteger[sADDON[i].emTYPE] );
			const float fValue = GetAddonValue_CompareExchange< const float >( i, ITEM::bAddonValueInteger[sADDON[i].emTYPE] );

			if ( ITEM::bAddonValueInteger[sADDON[i].emTYPE] )
				SFile << nValue << ",";
			else
				SFile << fValue << ",";
        }

        SFile << sBLOW.emTYPE << ",";
        SFile << sBLOW.fRATE << ",";
        SFile << sBLOW.fLIFE << ",";
        SFile << sBLOW.fVAR1 << ",";
        SFile << sBLOW.fVAR2 << ",";

		STRUTIL::OutputStrCsv( SFile, std::string( szRandomAddonScript ) );
		//SFile << szRandomAddonScript << ",";

		SFile << nItemSuitState[EMITEM_SUIT_CHANGE_COLOR] << ","; // by luxes.
		SFile << nItemSuitState[EMITEM_SUIT_UNIQUE] << ","; // by luxes.

        SFile << wDurabilityMax << ",";
        SFile << dwRepairPrice << ",";

		SFile << nBasicAttackDamageType << ",";
		SFile << nBasicAttackDamageIndex << ",";
		SFile << nBasicDefenceType << ",";
		SFile << nBasicDefenceIndex << ",";

		SFile << bEditableBasicAttackDamage << ",";
		SFile << bEditableBasicDefence << ",";
    }

    VOID SSUIT::LoadCsv ( CStringArray &StrArray, int& iCsvCur )
    {
        emSuit = (EMSUIT)atoi( StrArray[ iCsvCur++ ] );        
        dwHAND = (DWORD)atol( StrArray[ iCsvCur++ ] );
        emHand = (EMITEM_HAND)atoi( StrArray[ iCsvCur++ ] );

        emAttack = (GLITEM_ATT)atoi( StrArray[ iCsvCur++ ] );
        wAttRange = (WORD)atoi( StrArray[ iCsvCur++ ] );
        wReqSP = (WORD)atoi( StrArray[ iCsvCur++ ] );

        nHitRate = (short)atoi( StrArray[ iCsvCur++ ] );
        nAvoidRate = (short)atoi( StrArray[ iCsvCur++ ] );

        gdDamage.wLow = (WORD)atoi( StrArray[ iCsvCur++ ] );
        gdDamage.wHigh = (WORD)atoi( StrArray[ iCsvCur++ ] );
        nDefense = (short)atoi( StrArray[ iCsvCur++ ] );

        sResist.nFire = (short)atoi( StrArray[ iCsvCur++ ] );
        sResist.nIce = (short)atoi( StrArray[ iCsvCur++ ] );
        sResist.nElectric = (short)atoi( StrArray[ iCsvCur++ ] );
        sResist.nPoison = (short)atoi( StrArray[ iCsvCur++ ] );
        sResist.nSpirit = (short)atoi( StrArray[ iCsvCur++ ] );

        wReModelNum = (WORD)atoi( StrArray[ iCsvCur++ ] );

        for( int i=0 ;i<ADDON_SIZE; ++i )
        {
            sADDON[i].emTYPE = (EMITEM_ADDON)atoi( StrArray[ iCsvCur++ ] );
			if ( ITEM::bAddonValueInteger[sADDON[i].emTYPE] )
				sADDON[i].nVALUE = (int)atoi( StrArray[ iCsvCur++ ] );
			else
				sADDON[i].fVALUE = atof( StrArray[ iCsvCur++ ] );
        }

//         sVARIATE.emTYPE = (EMITEM_VAR)atoi( StrArray[ iCsvCur++ ] );
//         sVARIATE.fVariate = (float)atof( StrArray[ iCsvCur++ ] );
//         sVOLUME.emTYPE = (EMITEM_VAR)atoi( StrArray[ iCsvCur++ ] );
//         sVOLUME.fVolume = (float)atof( StrArray[ iCsvCur++ ] );
        sBLOW.emTYPE = (EMSTATE_BLOW)atoi( StrArray[ iCsvCur++ ] );
        sBLOW.fRATE = (float)atof( StrArray[ iCsvCur++ ] );
        sBLOW.fLIFE = (float)atof( StrArray[ iCsvCur++ ] );
        sBLOW.fVAR1 = (float)atof( StrArray[ iCsvCur++ ] );
        sBLOW.fVAR2 = (float)atof( StrArray[ iCsvCur++ ] );
		//STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], szRandomAddonScript );
		strncpy_s(szRandomAddonScript, StrArray[ iCsvCur++ ], STRING_LENGTH );

		nItemSuitState += (1 != atoi(StrArray[ iCsvCur++ ]) ? EMITEM_SUIT_NULL : EMITEM_SUIT_CHANGE_COLOR); // by luxes.
		nItemSuitState += (1 != atoi(StrArray[ iCsvCur++ ]) ? EMITEM_SUIT_NULL : EMITEM_SUIT_UNIQUE); // by luxes.

        wDurabilityMax = (WORD)atoi( StrArray[ iCsvCur++ ] );
        dwRepairPrice = (DWORD)atol( StrArray[ iCsvCur++ ] );

		nBasicAttackDamageType = atoi( StrArray[ iCsvCur++ ] );
		nBasicAttackDamageIndex = atoi( StrArray[ iCsvCur++ ] );
		nBasicDefenceType = atoi( StrArray[ iCsvCur++ ] );
		nBasicDefenceIndex = atoi( StrArray[ iCsvCur++ ] );

		bEditableBasicAttackDamage = atoi( StrArray[ iCsvCur++ ] ) == 1 ? true : false;
		bEditableBasicDefence = atoi( StrArray[ iCsvCur++ ] ) == 1 ? true : false;
    }

	const bool SSUIT::IsStatEnable() const
	{
		if( gdDamage != GLPADATA(0, 0) ) return true;
		if( nDefense   > 0 ||
			nHitRate   > 0 ||
			nAvoidRate > 0 )
			return true;
		if( sResist != SRESIST(0,0,0,0,0)) return true;
		
		for( int i = 0; i < ADDON_SIZE; ++i )
		{
			if( !sADDON[i].IsNoneType() ) return true;
		}

		return false;
	}

	const bool SSUIT::IsHaveAddon( IN const EMITEM_ADDON emAddon, OUT int& nIdx ) const
	{
		for ( int i = 0; i < ADDON_SIZE; ++i )
		{
			if ( sADDON[i].emTYPE == emAddon )
			{
				nIdx = i;
				return true;
			}
		}
		return false;
	}

	const bool SSUIT::GetHaveAddonValue_float( IN const EMITEM_ADDON emAddon, OUT float& fResult ) const
	{
		bool bRes = false;
		fResult = 0.0f;
		for ( int i = 0; i < ADDON_SIZE; ++i )
		{
			if ( sADDON[i].emTYPE == emAddon )
			{
				// 해당 타입의 Integer 상태를 확인을 해야만한다;
				// 이런식으로 예외처리를 하지 않는것은 좋지 않은 코드이다;
				//fResult += sADDON[i].fVALUE;
				fResult += GetAddonValue<float>(i, ITEM::bAddonValueInteger[emAddon]);

				bRes = true;
			}
		}
		return bRes;
	}

	const bool SSUIT::GetHaveAddonValue_int( IN const EMITEM_ADDON emAddon, OUT INT& nResult ) const
	{
		bool bRes = false;
		nResult = 0.0f;
		for ( int i = 0; i < ADDON_SIZE; ++i )
		{
			if ( sADDON[i].emTYPE == emAddon )
			{
				// 해당 타입의 Integer 상태를 확인을 해야만한다;
				// 이런식으로 예외처리를 하지 않는것은 좋지 않은 코드이다;
				//nResult += sADDON[i].nVALUE;
				nResult += GetAddonValue<int>(i, ITEM::bAddonValueInteger[emAddon]);

				bRes = true;
			}
		}
		return bRes;
	}

} // namespace ITEM