#ifndef GLVEHICLE_H_
#define GLVEHICLE_H_

#pragma once

#include <boost/tr1/unordered_map.hpp>
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxVehicleData.h"

#include "../Item/GLItem.h"

class GLChar;
class DxSkinChar;
class DxVehicle;

struct SVEHICLEITEMINFO
{
	VEHICLE_TYPE	m_emTYPE;
	int				m_nFull;
	DWORD			m_dwBoosterID;
	SITEMCUSTOM		m_PutOnItems[ACCE_TYPE_SIZE];

	SVEHICLEITEMINFO () :
		m_emTYPE(VEHICLE_TYPE_NONE),
		m_nFull(0),
		m_dwBoosterID(0)
	{
	}

	SVEHICLEITEMINFO ( const SVEHICLEITEMINFO& value ) { operator=( value ); }
	SVEHICLEITEMINFO& operator= ( const SVEHICLEITEMINFO& rvalue );

};

typedef std::map<DWORD, SVEHICLEITEMINFO> VEHICLEITEMINFO_MAP;
typedef VEHICLEITEMINFO_MAP::iterator	  VEHICLEITEMINFO_MAP_ITER;

struct SVEHICLE_COLOR
{
public:
	enum { VERSION = 0x0100 };
	enum { COLOR_NUM = ACCE_TYPE_SIZE };

	DWORD dwMainColor;
	DWORD dwSubColor;

	SVEHICLE_COLOR()
	{
		//재정신이 아닌 루프  -  주석
// 		for( INT i = 0 ; i < ACCE_TYPE_SIZE; i++ )
// 		{
			dwMainColor = 0;
			dwSubColor = 0;
//		}
	}

	bool operator==(const SVEHICLE_COLOR& other) const
	{
		return dwMainColor == other.dwMainColor && dwSubColor == other.dwSubColor;
	}

	bool operator!=(const SVEHICLE_COLOR& other) const
	{
		return !operator==(other);
	}

};

struct CLIENT_VEHICLE
{
	VEHICLE_TYPE	m_emTYPE;
	DWORD			m_DbNum; //! 탈것의 GUID (DB 번호)
	SNATIVEID		m_sVehicleID;					// 소환을 요청한 탈것 아이템의 Mid/Sid
	SITEMCLIENT		m_PutOnItems[ACCE_TYPE_SIZE];
	SVEHICLE_COLOR	m_sColor[ ACCE_TYPE_SIZE ];
	DWORD			m_dwBoosterID;
	BOOL			m_bUseBoost;			// 부스트사용여부
	BOOL			m_bUseBoostDelay;		// 부스트딜레이 체크
	float			m_fBoostDelay;			// 부스트 재사용딜레이
	float			m_fBoostTime;			// 부스트 사용시간
	
	CLIENT_VEHICLE() 
		: m_emTYPE ( VEHICLE_TYPE_NONE )
		, m_DbNum ( 0 )
		, m_sVehicleID ( NATIVEID_NULL() )
		, m_dwBoosterID( 0 )
		, m_bUseBoost( FALSE )
		, m_bUseBoostDelay( FALSE )
		, m_fBoostDelay( 0.0f )
		, m_fBoostTime( 0.0f )
	{
	}
};

BOOL SETITEMSCOLOR_BYBUF ( SVEHICLE_COLOR *pVehiclePartsColor, se::ByteStream &ByteStream );


class GLVEHICLE
{
public:
	VEHICLE_TYPE	m_emTYPE;
	DWORD			m_DbNum;						//! 탈것의 GUID (DB 번호)
	DWORD			m_OwnerDbNum;					//! 주인의 Char Db 번호
	SNATIVEID		m_sVehicleID;					//! 소환을 요청한 탈것 아이템의 Mid/Sid
	int				m_nFull;
	SITEMCUSTOM		m_PutOnItems[ACCE_TYPE_SIZE];
	SVEHICLE_COLOR	m_sColor[ACCE_TYPE_SIZE];
	float			m_fSpeedRate;					// 탈것의 속도 비율( 파츠 부분을 합친값 )
	float			m_fSpeedVol;					// 탈것의 속도 절대값( 파츠 부분을 합친값 )
	SNATIVEID		m_sSkinID;						// 탈것의 스킨 적용할 Item ID
	bool			m_bActiveValue;					// 현재 탈것의 정보인지 확인
	DxVehicle*		m_pSkinChar;					// 오토바이 스킨
	DxVehicleData	m_VehicleData;					// 오토바이 데이터

protected:
    DWORD			m_dwPassengerID[MAX_PASSENGER_COUNT]; //! 오토바이 탑승자 ID (Gaea ID)

public:
	DWORD			m_dwBoosterID;
	BOOL			m_bUseBoost;			//! 부스트사용여부
	BOOL			m_bUseBoostDelay;		//! 부스트딜레이 체크
	float			m_fBoostDelay;			//! 부스트 재사용딜레이
	float			m_fBoostTime;			//! 부스트 사용시간
	BOOL			m_bOnceBoost;

    DWORD m_CacheTickCount; //! Cache 된 시간

public:
	// constructor & operator
	GLVEHICLE();
	GLVEHICLE(const GLVEHICLE& value);
	GLVEHICLE& operator = (const GLVEHICLE& rvalue);
    virtual ~GLVEHICLE();
	
	BOOL GETPUTONITEMS_BYBUF(se::ByteStream& ByteStream) const;
	BOOL GETITEMSCOLOR_BYBUF(se::ByteStream& ByteStream) const;

	BOOL GETPUTONITEMS_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItem ) const;
	void SETPUTONITEM( VEHICLE_ACCESSORYTYPE emSlot, const SITEMCUSTOM& sItem );

	void ASSIGN(const GLVEHICLE& sVehicleData);
	void RESET();
	bool IncreaseFull(const WORD volume, const BOOL bRatio = TRUE);

	BOOL IsNotEnoughFull() const { return m_nFull < 10; }

	BOOL IsActiveValue() const { return m_bActiveValue; }
	VOID SetActiveValue(bool bActiveValue) { m_bActiveValue = bActiveValue; }
	SNATIVEID GetSkinID() const { return m_sSkinID; }

	INT GetPassengerNum() const;

	bool GetAccessoryType( EMSUIT emSuit, VEHICLE_ACCESSORYTYPE& Type );

	SITEMCUSTOM GetSlotitembySuittype ( EMSUIT emSuit );
	bool SetSlotItem ( EMSUIT emType, SITEMCUSTOM sCustomItem);
	bool ReSetSlotItem( EMSUIT emType);
	BOOL CheckSlotItem ( SNATIVEID sNativeID, EMSUIT emSUIT ) const;

    void PutOnItemSet(VEHICLE_ACCESSORYTYPE Type, SNATIVEID ItemId);
    void PutOnItemReset(VEHICLE_ACCESSORYTYPE Type);

    SNATIVEID PutOnItemId(VEHICLE_ACCESSORYTYPE Type) const;
    void      PutOnItemIdReset(VEHICLE_ACCESSORYTYPE Type);

    const SITEMCUSTOM& PutOnItemRef(VEHICLE_ACCESSORYTYPE Type) const;
    SITEMCUSTOM PutOnItem(VEHICLE_ACCESSORYTYPE Type) const;
    float PutOnItemMoveSpeedTotal() const;

//    EMPIECECHAR GetPieceFromSlot ( VEHICLE_ACCESSORYTYPE emType );

	DxVehicleData* GetVehicleData()	{ return &m_VehicleData; }

	//! access
    BOOL LoadFile(const std::string& FileName, VEHICLE_TYPE emType);
	void ITEM_UPDATE();

	float GetSpeedRate() const { return m_fSpeedRate; }
	float GetSpeedVol() const { return m_fSpeedVol; }

	void BoostReset();

    //! 소환을 요청한 탈것 아이템의 Mid/Sid
    SNATIVEID Id() const { return m_sVehicleID; }
    void IdSet(const SNATIVEID& VehicleId) { m_sVehicleID=VehicleId; }

    //! 부스트사용여부
    inline BOOL UseBoost() const { return m_bUseBoost; }
    inline BOOL OnceBoost() const { return m_bOnceBoost; }
    inline BOOL* OnceBoostRef() { return &m_bOnceBoost; }
    inline void OnceBoostSet(BOOL bSet) { m_bOnceBoost=bSet; }

    //! 오토바이 스킨
    DxVehicle* SkinChar() { return m_pSkinChar; }

    //! 오토바이 탑승자 ID
    DWORD PassengerId(const size_t Index) const
    {
        if (Index >= MAX_PASSENGER_COUNT)
            return GAEAID_NULL;
        else
            return m_dwPassengerID[Index];
    }

	const WORD GetPassengerIndex( const DWORD GaeaId )
	{
		for ( WORD Index = 0; Index < MAX_PASSENGER_COUNT; ++Index )
		{
			if ( m_dwPassengerID[Index] == GaeaId )
				return Index;
		}
		return -1;
	}

    void PassengerIdSet(const size_t Index, const DWORD GaeaId)
    {
        if (Index < MAX_PASSENGER_COUNT)
            m_dwPassengerID[Index] = GaeaId;
    }

    void PassengerIdReset(const size_t Index)
    {
        if (Index < MAX_PASSENGER_COUNT)
            m_dwPassengerID[Index] = GAEAID_NULL;
    }

	const BOOL IsPassengerID( const DWORD dwGaeaID ) const
	{
		for ( int Index = 0; Index < MAX_PASSENGER_COUNT; ++Index )
		{
			if ( m_dwPassengerID[Index] == dwGaeaID )
				return TRUE;
		}
		return FALSE;
	}

    int Full() const { return m_nFull; }

    VEHICLE_TYPE Type() const { return m_emTYPE; }
    void         TypeSet(VEHICLE_TYPE emType) { m_emTYPE=emType; }

    DWORD BoosterId() const { return m_dwBoosterID; }

    //! 부스트 사용시간
    float BoostTime() const { return m_fBoostTime; }
    
    //! 부스트딜레이 체크
    BOOL BoostUseDelay() const { return m_bUseBoostDelay; }

    //! 부스트 재사용딜레이
    float BoostDelay() const { return m_fBoostDelay; }

    DWORD ColorMain(VEHICLE_ACCESSORYTYPE Type) const;
    DWORD ColorSub(VEHICLE_ACCESSORYTYPE Type) const;
    SVEHICLE_COLOR Color(VEHICLE_ACCESSORYTYPE Type) const;

    //! 주인의 Char Db 번호
    DWORD OwnerDbNum() const { return m_OwnerDbNum; }
    void  OwnerDbNumSet(DWORD ChaDbNum) { m_OwnerDbNum = ChaDbNum; }

    //! 탈것의 GUID (DB 번호)
    inline DWORD DbNum() const { return m_DbNum; }
    inline void  DbNumSet(DWORD dwDbNum) { m_DbNum=dwDbNum; }

    //! Cache 된 시간
    DWORD GetChacheTickCount() const { return m_CacheTickCount; }

	bool IsValidSlot( EMSUIT emSuit );
};

typedef GLVEHICLE* PGLVEHICLE;

namespace COMMENT
{
	extern std::string VEHICLE_TYPE[VEHICLE_TYPE_SIZE];
};

#endif // GLVEHICLE_H_