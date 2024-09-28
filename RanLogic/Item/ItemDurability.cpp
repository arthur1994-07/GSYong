#include "pch.h"
#include "ItemDurability.h"


#include "../../enginelib/G-Logic/TypeDefine.h"
#include "../../enginelib/GUInterface/UITextControl.h"

#include "../GLUseFeatures.h"

#include "../GLogicData.h"
#include "../GLogicDataMan.h"
#include "GLItemDef.h"
#include "GLItem.h"

ItemDurability& ItemDurability::GetInstance()
{
    static ItemDurability sInstance;
    return sInstance;
}

bool ItemDurability::IsUseWeapon( const SITEM* pItem ) const
{
	if( pItem == NULL ) return false;
	
	if( pItem->sSuitOp.wDurabilityMax == 0 )
		return false;

	if(!IsUse( pItem->sSuitOp.emSuit )) return false;

	if( pItem->sBasicOp.emItemType != ITEM_SUIT ) return false;

	if( pItem->sSuitOp.emSuit != SUIT_HANDHELD ) return false;

	return true;
}

bool ItemDurability::IsUse( const EMSLOT& emSLOT ) const
{
    // 현재 장비하고있는 장비의 부위
    EMSUIT emSuit = SLOT_2_SUIT( emSLOT );
    return IsUse( emSuit );
}

bool ItemDurability::IsUse( const SNATIVEID& sID )  const
{
    const SITEM* pItem = GLogicData::GetInstance().GetItem( sID );
    if( !pItem )
        return false;

    if( pItem->sSuitOp.wDurabilityMax == 0 )
        return false;

    return IsUse( pItem->sSuitOp.emSuit );
}

bool ItemDurability::IsUse( const SITEMCUSTOM& sitemCustom ) const
{
    return IsUse( sitemCustom.GetNativeID() );
}

bool ItemDurability::IsUse( const EMSUIT& emSuit ) const
{
    if ( GLUseFeatures::GetInstance().IsUsingItemDurability() == false )
        return false;

    // 내구도기능을 사용할지 여부
    return GLCONST_CHAR::bDurabilityEmsuit[emSuit];
}

WORD ItemDurability::Get( const SITEMCUSTOM& sitemCustom ) const
{
    const SITEM* pItem = GLogicData::GetInstance().GetItem( sitemCustom.GetNativeID() );
    if( !pItem )
        return 0;

    return GetMax( sitemCustom ) - sitemCustom.wDurability;
}

WORD ItemDurability::GetMax( const SNATIVEID& sID ) const
{
    const SITEM* pItem = GLogicData::GetInstance().GetItem( sID );
    if( !pItem )
        return 0;

    return pItem->sSuitOp.wDurabilityMax;
}

WORD ItemDurability::GetMax( const SITEMCUSTOM& sitemCustom ) const
{
    return GetMax( sitemCustom.GetNativeID() );
}

WORD ItemDurability::GetPercent( const SITEMCUSTOM& sitemCustom ) const
{
    return static_cast<WORD>( GetRate( sitemCustom ) * 100 );
}

LONGLONG ItemDurability::GetRepairPriceAllPoint( const SITEMCUSTOM& sitemCustom ) const
{
    WORD wNow = Get( sitemCustom );
    WORD wMax = GetMax( sitemCustom );
    if( wMax == 0 )
        return 0;

    return static_cast<LONGLONG>( (wMax - wNow) * GetRepairPrice1Point(sitemCustom) );
}

DWORD ItemDurability::GetTextColor( const SITEMCUSTOM& sitemCustom, DWORD dwOriginalColor  ) const
{
    float fDurabilityRate = GetRate( sitemCustom );

    int nPercent = static_cast<int>( fDurabilityRate*100 );

    int nRemainder = nPercent%10;
    int nRaising = (nPercent/10) * 10;
    if( 0 < nRemainder )
        nRaising += 10;

    fDurabilityRate = nRaising * 100.0f;

    BYTE byA = static_cast<BYTE>(((dwOriginalColor)&0xFF000000)>>24);
    BYTE byR = static_cast<BYTE>(((dwOriginalColor)&0x00FF0000)>>16);
    BYTE byG = static_cast<BYTE>(((dwOriginalColor)&0x0000FF00)>>8);
    BYTE byB = static_cast<BYTE>(((dwOriginalColor)&0x000000FF));

    byR = static_cast<BYTE>( byR * fDurabilityRate );
    byR = static_cast<BYTE>( byR + 0xFF * ( 1 - fDurabilityRate ) );
    byG = static_cast<BYTE>( byG * fDurabilityRate );
    byB = static_cast<BYTE>( byB * fDurabilityRate );

    return D3DCOLOR_ARGB( byA, byR, byG, byB );
}

float ItemDurability::GetPerformance( const SITEMCUSTOM& sitemCustom ) const
{
    if( IsUse( sitemCustom.GetNativeID() ) == false )
        return 1.0f;

	SITEM* pItem = GLogicData::GetInstance().GetItem( sitemCustom.GetNativeID() );
	if( !pItem )
		return 1.0f;

	WORD dwBit = 0;

	EMSUIT eSuit = pItem->sSuitOp.emSuit;

	switch( eSuit )
	{
	case SUIT_HANDHELD :
		{			
			SCHARSTATS& rItemStats = pItem->sBasicOp.sReqStats;

			for( int i = 0; i < EMSIZE; ++i )
			{
				if( rItemStats.GET( (EMSTATS)i ) ) {
					if( dwBit > 0 )
					{
						dwBit = 0xffff;
						break;
					}
					dwBit |= (1 << i);
				}
			}
		}
		break;
	}

    return GetPerformance( GetRate(sitemCustom), dwBit );
}

DWORD ItemDurability::GetRepairPrice1Point( const SNATIVEID& sID ) const
{
    const SITEM* pItem = GLogicData::GetInstance().GetItem( sID );
    if( !pItem )
        return 0;

    return pItem->sSuitOp.dwRepairPrice;
}

DWORD ItemDurability::GetRepairPrice1Point( const SITEMCUSTOM& sitemCustom ) const
{
    return GetRepairPrice1Point( sitemCustom.GetNativeID() );
}

float ItemDurability::GetRate( const SITEMCUSTOM& sitemCustom ) const
{
    WORD wNow = Get( sitemCustom );
    WORD wMax = GetMax( sitemCustom );

    if( wMax == 0 && wNow == 0 )
        return 1.0f;

    return static_cast<float>(wNow) / wMax ;
}

float ItemDurability::GetPerformance( const float fRate, WORD wBit ) const
{
	const DWORD dwRate = static_cast<DWORD>(fRate * 100.0f);
    for( int i=0 ; i < DURABILITY_RATE_COUNT ; i++ )
    {
        // 내구도비율에 따라 성능이 감소한다.
        if( (DWORD)GLCONST_CHAR::nDurabilityRateTable[i] <= dwRate )
        {
			switch( wBit )
			{
			case 0xffff : return GLCONST_CHAR::nWMixDurRatePerformance[i]/100.0f;
			case 0x0001 : return GLCONST_CHAR::nWPowDurRatePerformance[i]/100.0f;
			case 0x0004 : return GLCONST_CHAR::nWSpiDurRatePerformance[i]/100.0f;
			case 0x0008 : return GLCONST_CHAR::nWDexDurRatePerformance[i]/100.0f;			
			default : return GLCONST_CHAR::nDurabilityRatePerformance[i]/100.0f;
			}			
        }
    }

    sc::writeLogError( "Plz. Check Your 'nDurabilityRateTable' Subject in 'default.charclass' File");
    return 1.0f;

}

bool ItemDurability::DurabilityDropHit( SITEMCUSTOM& sItemCustom, unsigned int nDamage, BOOL bPvp )
{
    if( sItemCustom.GetNativeID() == NATIVEID_NULL() ) 
        return false;

    if( ItemDurability::GetInstance().IsUse( sItemCustom.GetNativeID() ) == false )
        return false;

    if( Get(sItemCustom) <= 0)
        return false;

    if( bPvp )
        sItemCustom.dwDurabilityCount += GLCONST_CHAR::dwDurabilityDropHitPvP * nDamage;
    else
        sItemCustom.dwDurabilityCount += GLCONST_CHAR::dwDurabilityDropHitPvE * nDamage;

    return DurabilityCalc( sItemCustom );
}

bool ItemDurability::DurabilityDropUseSkill( SITEMCUSTOM& sItemCustom )
{
    if( sItemCustom.GetNativeID() == NATIVEID_NULL() ) 
        return false;

    if( ItemDurability::GetInstance().IsUse( sItemCustom.GetNativeID() ) == false )
        return false;

    if( Get(sItemCustom) <= 0)
        return false;

    sItemCustom.dwDurabilityCount += GLCONST_CHAR::dwDurabilityDropSkillUse;

    return DurabilityCalc( sItemCustom );
}

bool ItemDurability::DurabilityDropDeath( SITEMCUSTOM& sItemCustom, BOOL bPvp )
{
    if( sItemCustom.GetNativeID() == NATIVEID_NULL() ) 
        return false;

    if( ItemDurability::GetInstance().IsUse( sItemCustom.GetNativeID() ) == false )
        return false;

    if( Get(sItemCustom) <= 0)
        return false;


    if(  bPvp )
        sItemCustom.wDurability += GLCONST_CHAR::wDurabilityDropDeathPvP;
    else
        sItemCustom.wDurability += GLCONST_CHAR::wDurabilityDropDeathPvE;

    return DurabilityCalc( sItemCustom );
}

bool ItemDurability::ItemRepairing( SITEMCUSTOM& sItemCustom )
{
    if( sItemCustom.GetNativeID() == NATIVEID_NULL() ) 
        return false;

    if( ItemDurability::GetInstance().IsUse( sItemCustom.GetNativeID() ) == false )
        return false;

    if( Get(sItemCustom) == GetMax( sItemCustom ) )
        return false;

    //내구도를 회복시킨다.
    sItemCustom.wDurability = 0;
    sItemCustom.dwDurabilityCount = 0;

    return true;
}

bool ItemDurability::DurabilityCalc( SITEMCUSTOM& sItemCustom )
{
    WORD& wDurability = sItemCustom.wDurability;
    DWORD& dwDurabilityCount = sItemCustom.dwDurabilityCount;

    int nSub = 0;

    // 장비의 카운트가 리미트를 넘어 선경우
    if( GLCONST_CHAR::dwDurabilityCountLimit <= dwDurabilityCount )
    {
        // 리미트를 넘은 만큼 값을 뺀다.
        const DWORD& dwLimit = GLCONST_CHAR::dwDurabilityCountLimit;

        nSub = dwDurabilityCount / dwLimit; // 나눈다
        dwDurabilityCount %= dwLimit;   // 나머지를 넣고
    }    


    // 실제 내구도에서 증가( 실제 내구도 감소 )
    const WORD& wMaxDurability = GetMax(sItemCustom);

    if( 0 < nSub )
    {
        if( wMaxDurability <= wDurability + nSub )
            wDurability = wMaxDurability;
        else
            wDurability += nSub;

        return true;
    }
    else
    {
        if( wMaxDurability <= wDurability )
            wDurability = wMaxDurability;

        return false;
    }
}
