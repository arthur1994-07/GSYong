#pragma once

#include <vector>
#include <map>

#include "../../../enginelib/G-Logic/TypeDefine.h"

#include "../../../RanLogic/Msg/GLContrlInDunMsg.h"
#include "../../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"

#include "./CertificationMachineMode/InstanceDungeon_CertificationMachine_Define.h"

#include "./InstanceSystemField.h"

namespace InstanceSystem
{

	enum EMINSTANCE_FIELD_MODE
	{
		EMINSTANCE_FIELD_MODE_NONE,
		EMINSTANCE_FIELD_MODE_CERTIFICATION_MACHINE_CLUB,
		EMINSTANCE_FIELD_MODE_CERTIFICATION_MACHINE_COUNTRY
	};


	// Callback Function Parameter;
	struct SCALLBACK_PARAMETER
	{
		enum EMCALLBACK_PARAMETER_TYPE
		{
			EMCALLBACK_PARAMETER_TYPE_INTEGER,
			EMCALLBACK_PARAMETER_TYPE_NUMBER
		};

		union
		{
			int						nValue;
			float					fValue;
		};

		EMCALLBACK_PARAMETER_TYPE	emType;

		SCALLBACK_PARAMETER ( void )
			: emType ( EMCALLBACK_PARAMETER_TYPE_INTEGER )
			, nValue ( 0 )
		{

		}

		SCALLBACK_PARAMETER ( int _nValue )
			: emType ( EMCALLBACK_PARAMETER_TYPE_INTEGER )
			, nValue ( _nValue )
		{

		}

		SCALLBACK_PARAMETER ( float _fValue )
			: emType ( EMCALLBACK_PARAMETER_TYPE_NUMBER )
			, nValue ( _fValue )
		{

		}

		SCALLBACK_PARAMETER ( const SCALLBACK_PARAMETER& _sInfo )
			: emType ( _sInfo.emType )
			, nValue ( _sInfo.nValue )
		{

		}
	};

	typedef		std::vector< SCALLBACK_PARAMETER >			VEC_CALLBACK_PARAMETER;
	typedef		VEC_CALLBACK_PARAMETER::iterator			VEC_CALLBACK_PARAMETER_ITER;
	typedef		VEC_CALLBACK_PARAMETER::const_iterator		VEC_CALLBACK_PARAMETER_CITER;

}