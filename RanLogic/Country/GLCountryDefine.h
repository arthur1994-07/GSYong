#pragma once

#include <string>

#include "../../enginelib/G-Logic/TypeDefine.h"

#include "../Network/NetLogicDefine.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/gassert.h"

#include "../../SigmaCore/MsgPack/MinMsgPack.h"

namespace Country
{
	typedef		WORD	COUNTRY_ID;
	typedef		WORD	CONTINENT_ID;

	const COUNTRY_ID	UNKNOWN_COUNTRY			= 0;
	const CONTINENT_ID	UNKNOWN_CONTINENT		= 0;

	struct SCOUNTRY_INFO
	{
		union
		{
			struct
			{
				DWORD dwID;
			};

			struct
			{
				CONTINENT_ID	wContinentID;
				COUNTRY_ID		wCountryID;
			};
		};

		MSGPACK_DEFINE( dwID );

		SCOUNTRY_INFO ()
			: wCountryID ( 0 )
			, wContinentID ( 0 )
		{

		}

		SCOUNTRY_INFO ( CONTINENT_ID _wContinentID
			, COUNTRY_ID _wCountryID )
			: wContinentID ( _wContinentID )
			, wCountryID ( _wCountryID )
		{

		}

		void RESET ()
		{
			wContinentID = 0;
			wCountryID = 0;
		}

		bool ISNONCOUNTRY ()
		{
			if ( ( wContinentID == UNKNOWN_CONTINENT ) &&
				( wCountryID == UNKNOWN_COUNTRY ) )
				return true;

			return false;
		}

		const bool operator == ( const SCOUNTRY_INFO& _rhs ) const
		{
			return ( wContinentID == _rhs.wContinentID )
				&& ( wCountryID == _rhs.wCountryID );
		}
	};

	const DWORD			UNKNOWN_COUNTRY_INFO		= SCOUNTRY_INFO().dwID;
	const std::string	UNKNOWN_COUNTRY_FILENAME	= "UCT.dds";
	const std::string	UNKNOWN_COUNTRY_IMAGEID		= "COUNTRY_UCT";
	const std::string	UNKNOWN_COUNTRY_MINIIMAGEID	= "MINI_COUNTRY_UCT";


	// 국가 시스템 이용 컨텐츠 플래그;
	enum EMCOUNTRY_SYSTEM_FLAG
	{
		CAPTURE_THE_ISLANDS,
		EMCOUNTRY_SYSTEM_FLAG_SIZE
	};

	// 승리한 나라 정보;
	struct SVICTORIOUS_COUNTRY 
	{
		EMCOUNTRY_SYSTEM_FLAG	emFlag;				// 전쟁 Flag;
		SCOUNTRY_INFO			sCountryInfo;		// 승리한 나라 정보;
		SNATIVEID				sMapID;				// 점령한 영토;

		const bool operator == ( const SVICTORIOUS_COUNTRY& _rhs ) const
		{
			return ( emFlag == _rhs.emFlag )
				&& ( sCountryInfo == _rhs.sCountryInfo )
				&& ( sMapID == _rhs.sMapID );
		}
	};

	typedef std::vector< SVICTORIOUS_COUNTRY >		VCOUNTRY_VEC;
	typedef VCOUNTRY_VEC::iterator					VCOUNTRY_VEC_ITER;
	typedef VCOUNTRY_VEC::const_iterator			VCOUNTRY_VEC_CITER;
}