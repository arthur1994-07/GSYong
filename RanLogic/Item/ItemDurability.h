#pragma once

struct SNATIVEID;
struct SITEMCUSTOM;
struct SITEM;
enum EMSLOT;
enum EMSUIT;

class ItemDurability
{
public:
    static ItemDurability& GetInstance();

public:
    // 내구도 사용 여부 ( default.charclass 설정 )
    bool IsUse( const EMSLOT& emSLOT ) const;
    bool IsUse( const SNATIVEID& sID ) const;
    bool IsUse( const SITEMCUSTOM& sitemCustom ) const;
    bool IsUse( const EMSUIT& emSuit ) const;

	bool IsUseWeapon( const SITEM* pItem ) const;

    // 현재 내구도
    WORD Get( const SITEMCUSTOM& sitemCustom ) const;

    // 쵀대 내구도
    WORD GetMax( const SNATIVEID& sID ) const;
    WORD GetMax( const SITEMCUSTOM& sitemCustom ) const;

    // 현재 남은 내구도 퍼센트
    WORD  GetPercent( const SITEMCUSTOM& sitemCustom ) const;

    // 물품 수리비
    LONGLONG GetRepairPriceAllPoint(const SITEMCUSTOM& sitemCustom ) const;

    // 남은 내구도에 따른 텍스트 컬러( 클라 사용 )
    DWORD GetTextColor( const SITEMCUSTOM& sitemCustom, DWORD dwOriginalColor ) const;

    float GetPerformance( const SITEMCUSTOM& sitemCustom  ) const;

private:
    // 1p 수리비
    DWORD GetRepairPrice1Point( const SNATIVEID& sID ) const;
    DWORD GetRepairPrice1Point( const SITEMCUSTOM& sitemCustom  ) const;

    // 현재 남은 내구도 비율
    float GetRate( const SITEMCUSTOM& sitemCustom ) const;   

    float GetPerformance( const float fRate, WORD wBit = 0 ) const;

public:
    bool DurabilityDropHit( SITEMCUSTOM& sItemCustom, unsigned int nDamage, BOOL bPvp );
    bool DurabilityDropUseSkill( SITEMCUSTOM& sItemCustom );
    bool DurabilityDropDeath( SITEMCUSTOM& sItemCustom, BOOL bPvp );
    bool ItemRepairing( SITEMCUSTOM& sItemCustom );

private:
    bool DurabilityCalc( SITEMCUSTOM& sItemCustom );

};
