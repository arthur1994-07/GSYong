#pragma once

namespace holditem
{
    enum HOLD_PLACE_TYPE
    {
        NONE,
        INVENTORY,
        STORAGE,
        WEAR_SLOT,
        PET_SLOT,
        VEHICLE_SLOT,
        CLUB_STORAGE,
    };

    void HoldItem( HOLD_PLACE_TYPE _HoldPlace, const SITEMCUSTOM& _HoldItem, WORD _PosX, WORD _PosY, bool bReserveDelete = false );
    const SITEMCUSTOM& GetHoldItem();
    void ReleaseHoldItem();

    bool HaveHoldItem();

    bool IsHoldPlace( HOLD_PLACE_TYPE _HoldPlace );
    HOLD_PLACE_TYPE WhenHoldItem();

    WORD GetHoldPosX();
    WORD GetHoldPosY();

    void SetData( DWORD _Data0 = 0, DWORD _Data1 = 0 );
    void GetData( DWORD& _OutData0, DWORD& _OutData1 );
	void SetReserveDelete(bool bReserve);
	bool IsReserveDelete(void);

	void SetHoldSlotBinding( const std::string& strBinding );
	const std::string& GetHoldSlotBinding();
};