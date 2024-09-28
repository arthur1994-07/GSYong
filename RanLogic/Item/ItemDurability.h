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
    // ������ ��� ���� ( default.charclass ���� )
    bool IsUse( const EMSLOT& emSLOT ) const;
    bool IsUse( const SNATIVEID& sID ) const;
    bool IsUse( const SITEMCUSTOM& sitemCustom ) const;
    bool IsUse( const EMSUIT& emSuit ) const;

	bool IsUseWeapon( const SITEM* pItem ) const;

    // ���� ������
    WORD Get( const SITEMCUSTOM& sitemCustom ) const;

    // ���� ������
    WORD GetMax( const SNATIVEID& sID ) const;
    WORD GetMax( const SITEMCUSTOM& sitemCustom ) const;

    // ���� ���� ������ �ۼ�Ʈ
    WORD  GetPercent( const SITEMCUSTOM& sitemCustom ) const;

    // ��ǰ ������
    LONGLONG GetRepairPriceAllPoint(const SITEMCUSTOM& sitemCustom ) const;

    // ���� �������� ���� �ؽ�Ʈ �÷�( Ŭ�� ��� )
    DWORD GetTextColor( const SITEMCUSTOM& sitemCustom, DWORD dwOriginalColor ) const;

    float GetPerformance( const SITEMCUSTOM& sitemCustom  ) const;

private:
    // 1p ������
    DWORD GetRepairPrice1Point( const SNATIVEID& sID ) const;
    DWORD GetRepairPrice1Point( const SITEMCUSTOM& sitemCustom  ) const;

    // ���� ���� ������ ����
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
