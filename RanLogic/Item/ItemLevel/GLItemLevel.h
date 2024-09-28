#pragma once

#include "../../../SigmaCore/Util/GLSingletone.h"

#include "../../GLogicData.h"

struct SINFO_BYITEMLEVEL
{
	WORD wMaxGrade;
	WORD wDamageByGrade;
	WORD wDefenceByGrade;
	float fDamageRateByGrade;
	float fDefenceRateByGrade;

	WORD wMaxResist;
	WORD wFireResistByGrade;
	WORD wIceResistByGrade;
	WORD wElectricResistByGrade;
	WORD wPoisonResistByGrade;
	WORD wSpiritResistByGrade;

	WORD wArrUseItemCountByHighGrade[ GRADE_LIMIT_MAX-GRADE_HIGH ];
	float fArrDamageRateByHighGrade[ GRADE_LIMIT_MAX-GRADE_HIGH ];
	float fArrDefenceRateByHighGrade[ GRADE_LIMIT_MAX-GRADE_HIGH ];

	float fArrSuccessRateByGrade[ GRADE_LIMIT_MAX ];
	float fArrResetRateByGrade[ GRADE_LIMIT_MAX ];
	float fArrTerminateRateByGrade[ GRADE_LIMIT_MAX ];
	float fArrItemOptionIncreaseRateByGrade[ GRADE_LIMIT_MAX ];
	float fArrItemOptionAntiResetRateByGrade[ GRADE_LIMIT_MAX ];

	bool bTerminateItem;

	SINFO_BYITEMLEVEL ( void )
		: wMaxGrade ( 0 )
		, wDamageByGrade ( 0 )
		, wDefenceByGrade ( 0 )
		, fDamageRateByGrade ( 0.f )
		, fDefenceRateByGrade ( 0.f )
		, wMaxResist ( 0 )
		, wFireResistByGrade ( 0 )
		, wIceResistByGrade ( 0 )
		, wElectricResistByGrade ( 0 )
		, wPoisonResistByGrade ( 0 )
		, wSpiritResistByGrade ( 0 )
	{
		ZeroMemory( wArrUseItemCountByHighGrade, sizeof( wArrUseItemCountByHighGrade ) );
		ZeroMemory( fArrDamageRateByHighGrade, sizeof( fArrDamageRateByHighGrade ) );
		ZeroMemory( fArrDefenceRateByHighGrade, sizeof( fArrDefenceRateByHighGrade ) );

		ZeroMemory( fArrSuccessRateByGrade, sizeof( fArrSuccessRateByGrade ) );
		ZeroMemory( fArrResetRateByGrade, sizeof( fArrResetRateByGrade ) );
		ZeroMemory( fArrTerminateRateByGrade, sizeof( fArrTerminateRateByGrade ) );
		ZeroMemory( fArrItemOptionIncreaseRateByGrade, sizeof( fArrItemOptionIncreaseRateByGrade ) );
		ZeroMemory( fArrItemOptionAntiResetRateByGrade, sizeof( fArrItemOptionAntiResetRateByGrade ) );
	}
};

class GLItemLevelMan : public CSingletone< GLItemLevelMan >
{
protected:
	friend CSingletone;


public:
	bool LoadData ();


public:
	const SINFO_BYITEMLEVEL& GetInfoByItemLevel ( const EMITEMLEVEL _emItemLevel ) const;


private:
	SINFO_BYITEMLEVEL m_sArrInfoByItemLevel[ EMITEM_LEVEL_NSIZE ];


private:
	GLItemLevelMan () { }
	virtual ~GLItemLevelMan () { }
};