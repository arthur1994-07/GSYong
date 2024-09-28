#include "../pch.h"

#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Item/GLItem.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Msg/GLContrlBaseMsg.h"

#include "./GLItemGrindingScript.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

SQGrindingItem::SQGrindingItem()
	: m_pItem(NULL)
	, m_pItemcustom(NULL)
{
	//Blank
}

SQGrindingItem::SQGrindingItem( SITEMCUSTOM* _ItemCustom )
	: m_pItem(NULL)
	, m_pItemcustom(_ItemCustom)
{
	if ( _ItemCustom )
	{
		m_pItem = GLItemMan::GetInstance().GetItem(_ItemCustom->GetNativeID());
	}
}

SQGrindingItem::SQGrindingItem( SITEMCUSTOM& _ItemCustom )
	: m_pItem(NULL)
	, m_pItemcustom(&_ItemCustom)
{
	m_pItem = GLItemMan::GetInstance().GetItem(_ItemCustom.GetNativeID());
}

SQGrindingItem::SQGrindingItem( const SITEMCUSTOM* _ItemCustom )
	: m_pItem(NULL)
	, m_pItemcustom(const_cast<SITEMCUSTOM*>(_ItemCustom))
{
	if ( _ItemCustom )
	{
		m_pItem = GLItemMan::GetInstance().GetItem(_ItemCustom->GetNativeID());
	}
}

SQGrindingItem::SQGrindingItem( const SITEMCUSTOM& _ItemCustom )
	: m_pItem(NULL)
	, m_pItemcustom(const_cast<SITEMCUSTOM*>(&_ItemCustom))
{
	m_pItem = GLItemMan::GetInstance().GetItem(_ItemCustom.GetNativeID());
}

/*EMGRINDING_TYPE*/const UINT SQGrindingItem::GetGrindingType()
{
	if ( !m_pItem )
	{
		return EMGRINDING_NUNE;
	}

	return static_cast<UINT>(m_pItem->sGrindingOp.emTYPE);
}

/*EMGRINDER_TYPE*/const UINT SQGrindingItem::GetGrindingLevel()
{
	if ( !m_pItem )
	{
		return EMGRINDER_SIZE;
	}

	return static_cast<UINT>(m_pItem->sGrindingOp.emGRINDER_TYPE);
}


/*EMGRINDING_CLASS*/const UINT SQGrindingItem::GetGrindingPurpose()
{
	if ( !m_pItem )
	{
		return EMGRINDING_CLASS_SIZE;
	}

	return static_cast<UINT>(m_pItem->sGrindingOp.emCLASS);
}

/*EMGRINDING_CLASS*/const UINT SQGrindingItem::GetSuitType2Purpose()
{
	if ( !m_pItem )
	{
		return EMGRINDING_CLASS_SIZE;
	}

	if ( m_pItem->GetAttackDamage() == GLPADATA(0,0) )
	{
		return EMGRINDING_CLASS_CLOTH;
	}

	return EMGRINDING_CLASS_ARM;
}

/*EMSUIT*/const UINT SQGrindingItem::GetSuitType()
{
	if ( !m_pItem )
	{
		return SUIT_NSIZE;
	}

	return EMSUIT(m_pItem->sSuitOp.emSuit);
}

const UINT SQGrindingItem::GetDefaultGrindingConsumeCount( const UINT nIndex )
{
	if ( NULL == m_pItemcustom )
		return 0;

	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( m_pItemcustom->GetItemLevel() );

	if ( GRADE_LIMIT_MAX-GRADE_HIGH <= nIndex )
	{
		sc::writeLogError( sc::string::format(
			"SQGrindingItem::GetDefaultGrindingConsumeNum - GRADE_LIMIT_MAX-GRADE_HIGH <= nIndex - %1%"
			,nIndex
			)	
		);
		return 0;
	}

	//return static_cast<UINT>(GLCONST_CHAR::wUSE_GRADE_NUM[nIndex]);
	return static_cast<UINT>(sInfoByItemLevel.wArrUseItemCountByHighGrade[nIndex]);
}

const float SQGrindingItem::GetDefaultGrindingRate( const UINT nIndex )
{
	if ( NULL == m_pItemcustom )
		return 0.f;

	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( m_pItemcustom->GetItemLevel() );

	if ( GRADE_LIMIT_MAX <= nIndex )
	{
		sc::writeLogError( sc::string::format(
			"SQGrindingItem::GetDefaultGrindingRate - GRADE_LIMIT_MAX <= nIndex - %1%"
			,nIndex
			)	
		);

		return 0.0f;
	}

	//return GLCONST_CHAR::fGRADE_RATE[nIndex];
	return sInfoByItemLevel.fArrSuccessRateByGrade[nIndex];
}

const float SQGrindingItem::GetDefaultGrindingResetRate( const UINT nIndex )
{
	if ( NULL == m_pItemcustom )
		return 0.f;

	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( m_pItemcustom->GetItemLevel() );

	if ( GRADE_LIMIT_MAX <= nIndex )
	{
		sc::writeLogError( sc::string::format(
			"SQGrindingItem::GetDefaultGrindingResetRate - GRADE_LIMIT_MAX <= nIndex - %1%"
			,nIndex
			)	
		);

		return 0.0f;
	}

	//return GLCONST_CHAR::fGRADE_RESET_RATE[nIndex];
	return sInfoByItemLevel.fArrResetRateByGrade[nIndex];
}

const float SQGrindingItem::GetDefaultGrindingTerminateRate( const UINT nIndex )
{
	if ( NULL == m_pItemcustom )
		return 0.f;

	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( m_pItemcustom->GetItemLevel() );

	if ( GRADE_LIMIT_MAX <= nIndex )
	{
		sc::writeLogError( sc::string::format(
			"SQGrindingItem::GetDefaultGrindingTerminateRate - GRADE_LIMIT_MAX <= nIndex - %1%"
			,nIndex
			)	
		);

		return 0.0f;
	}

	//return GLCONST_CHAR::fGRADE_TERMINATE_RATE[nIndex];
	return sInfoByItemLevel.fArrTerminateRateByGrade[nIndex];
}

const UINT SQGrindingItem::GetDefaultGrindingAntiResetCount( const UINT nIndex )
{
	if ( NULL == m_pItemcustom )
		return 0.f;

	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( m_pItemcustom->GetItemLevel() );

	if ( GRADE_LIMIT_MAX <= nIndex )
	{
		sc::writeLogError( sc::string::format(
			"SQGrindingItem::GetDefaultGrindingAnitResetCount - GRADE_LIMIT_MAX <= nIndex - %1%"
			,nIndex
			)	
		);

		return 1;
	}

	//return static_cast<UINT>(GLCONST_CHAR::nGRADE_ITEMOPTION_ANTIRESET[nIndex]);
	return static_cast<UINT>(sInfoByItemLevel.fArrItemOptionAntiResetRateByGrade[nIndex]);
}

const float SQGrindingItem::GetDefaultGrindingIncreaseRate( const UINT nIndex )
{
	if ( NULL == m_pItemcustom )
		return 0.f;

	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( m_pItemcustom->GetItemLevel() );

	if ( GRADE_LIMIT_MAX <= nIndex )
	{
		sc::writeLogError( sc::string::format(
			"SQGrindingItem::GetDefaultGrindingIncreaseRate - GRADE_LIMIT_MAX <= nIndex - %1%"
			,nIndex
			)	
		);

		return 0.0f;
	}

	//return GLCONST_CHAR::fGRADE_ITEMOPTION_INC_GRINDINGRATE[nIndex];
	return sInfoByItemLevel.fArrItemOptionIncreaseRateByGrade[nIndex];
}

const UINT SQGrindingItem::GetTurnNum()
{
	if ( !m_pItemcustom )
	{
		return 0;
	}

	return static_cast<UINT>(m_pItemcustom->wTurnNum);	
}

const UINT SQGrindingItem::GetGrade( const UINT nType )
{
	if ( !m_pItemcustom )
	{
		return 0;
	}

	return static_cast<UINT>(m_pItemcustom->GETGRADE( static_cast<EMGRINDING_TYPE>(nType) ));	
}

const UINT SQGrindingItem::GetGradeLimit(/*EMGRINDING_TYPE*/const UINT nType)
{
	if ( NULL == m_pItemcustom )
		return 0;

	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( m_pItemcustom->GetItemLevel() );

	if ( nType == EMGRINDING_DAMAGE || nType == EMGRINDING_DEFENSE )
	{
		return /*GLCONST_CHAR::wGRADE_MAX*/sInfoByItemLevel.wMaxGrade;
	}
	
	return /*GLCONST_CHAR::wGRADE_MAX_REGI*/sInfoByItemLevel.wMaxResist;
}

void SQGrindingItem::SetGrade( const UINT nType, const UINT nValue )
{
	if ( !m_pItemcustom )
	{
		return;
	}

	if ( GRADE_LIMIT_MAX <= nValue )
	{
		sc::writeLogError( sc::string::format(
			"SQGrindingItem::SetGrade - GRADE_LIMIT_MAX <= nValue - %1%"
			,nValue
			)	
		);

		return;
	}

	switch ( nType )
	{
	case EMGRINDING_DAMAGE:			m_pItemcustom->cDAMAGE		  = static_cast<BYTE>(nValue);	break;
	case EMGRINDING_DEFENSE:		m_pItemcustom->cDEFENSE		  = static_cast<BYTE>(nValue);	break;
	case EMGRINDING_RESIST_FIRE:	m_pItemcustom->cRESIST_FIRE	  = static_cast<BYTE>(nValue);	break;
	case EMGRINDING_RESIST_ICE:		m_pItemcustom->cRESIST_ICE	  = static_cast<BYTE>(nValue);	break;
	case EMGRINDING_RESIST_ELEC:	m_pItemcustom->cRESIST_ELEC	  = static_cast<BYTE>(nValue);	break;
	case EMGRINDING_RESIST_POISON:	m_pItemcustom->cRESIST_POISON = static_cast<BYTE>(nValue);	break;
	case EMGRINDING_RESIST_SPIRIT:	m_pItemcustom->cRESIST_SPIRIT = static_cast<BYTE>(nValue);	break;
	};
}

void SQGrindingItem::DecGrade( /*EMGRINDING_TYPE*/const UINT nType, const UINT nValue )
{
	if ( !m_pItemcustom )
	{
		return;
	}

	m_pItemcustom->DecreaseGrind( EMGRINDING_TYPE(nType), nValue );
}

void SQGrindingItem::ResetGrade( const UINT nType )
{
	if ( !m_pItemcustom )
	{
		return;
	}

	m_pItemcustom->ResetGrind(static_cast<EMGRINDING_TYPE>(nType));
}

//-----------------------------------------------------------------------------

CGrindingScript::CGrindingScript()
	: m_sqTbOption(NULL)
	, m_sqTbLogic(NULL)
	, m_sqFnEntry(NULL)
	, m_sqFnCondition(NULL)
	, m_sqFnConsume(NULL)
{
	Bind();
}

CGrindingScript::~CGrindingScript()
{
	Clear();
}

void CGrindingScript::Bind()
{
	using namespace Sqrat;

	Sqrat::ClassType<SQGrindingItem>::Initialized() = false;

	Sqrat::RootTable(m_vm).Bind( "SQGrindingItem", Sqrat::Class<SQGrindingItem>(m_vm)
		.Func( "GetGrindingType", &SQGrindingItem::GetGrindingType )
		.Func( "GetGrindingLevel", &SQGrindingItem::GetGrindingLevel )
		.Func( "GetGrindingPurpose", &SQGrindingItem::GetGrindingPurpose )
		.Func( "GetSuitType2Purpose", &SQGrindingItem::GetSuitType2Purpose )
		.Func( "GetDefaultGrindingConsumeCount", &SQGrindingItem::GetDefaultGrindingConsumeCount )
		.Func( "GetDefaultGrindingRate", &SQGrindingItem::GetDefaultGrindingRate )
		.Func( "GetDefaultGrindingResetRate", &SQGrindingItem::GetDefaultGrindingResetRate )
		.Func( "GetDefaultGrindingTerminateRate", &SQGrindingItem::GetDefaultGrindingTerminateRate )
		.Func( "GetDefaultGrindingAntiResetCount", &SQGrindingItem::GetDefaultGrindingAntiResetCount )
		.Func( "GetDefaultGrindingIncreaseRate", &SQGrindingItem::GetDefaultGrindingIncreaseRate )
		.Func( "GetSuitType", &SQGrindingItem::GetSuitType )
		.Func( "GetTurnNum", &SQGrindingItem::GetTurnNum )
		.Func( "GetGradeLimit", &SQGrindingItem::GetGradeLimit )
		.Func( "GetGrade", &SQGrindingItem::GetGrade )
		.Func( "SetGrade", &SQGrindingItem::SetGrade )
		.Func( "DecGrade", &SQGrindingItem::DecGrade )
		.Func( "ResetGrade", &SQGrindingItem::ResetGrade )
		);

	Sqrat::ConstTable(m_vm)
		.Const( "EMSUIT_HEADGEAR",				SUIT_HEADGEAR )
		.Const( "EMSUIT_UPPER",					SUIT_UPPER )
		.Const( "EMSUIT_LOWER",					SUIT_LOWER )
		.Const( "EMSUIT_HAND",					SUIT_HAND )
		.Const( "EMSUIT_FOOT",					SUIT_FOOT )
		.Const( "EMSUIT_HANDHELD",				SUIT_HANDHELD )
		.Const( "EMSUIT_NECK",					SUIT_NECK )
		.Const( "EMSUIT_WRIST",					SUIT_WRIST )
		.Const( "EMSUIT_FINGER",				SUIT_FINGER )
		.Const( "EMSUIT_PET_A",					SUIT_PET_A )
		.Const( "EMSUIT_PET_B",					SUIT_PET_B )
		.Const( "EMSUIT_VEHICLE",				SUIT_VEHICLE )
		.Const( "EMSUIT_VEHICLE_SKIN",			SUIT_VEHICLE_SKIN )
		.Const( "EMSUIT_VEHICLE_PARTS_A",		SUIT_VEHICLE_PARTS_A )
		.Const( "EMSUIT_VEHICLE_PARTS_B",		SUIT_VEHICLE_PARTS_B )
		.Const( "EMSUIT_VEHICLE_PARTS_C",		SUIT_VEHICLE_PARTS_C )
		.Const( "EMSUIT_VEHICLE_PARTS_D",		SUIT_VEHICLE_PARTS_D )
		.Const( "EMSUIT_VEHICLE_PARTS_E",		SUIT_VEHICLE_PARTS_E )
		.Const( "EMSUIT_VEHICLE_PARTS_F",		SUIT_VEHICLE_PARTS_F )
		.Const( "EMSUIT_VEHICLE_PARTS_SIZE",	SUIT_VEHICLE_PARTS_SIZE )
        .Const( "EMSUIT_SUIT_WAISTBAND",	    SUIT_WAISTBAND )
        .Const( "EMSUIT_SUIT_EARRING",	        SUIT_EARRING )
        .Const( "EMSUIT_SUIT_ACCESSORIES",	    SUIT_ACCESSORIES )
		.Const( "EMSUIT_NSIZE",					SUIT_NSIZE );

	Sqrat::ConstTable(m_vm)
		.Const( "EMGRINDING_PURPOSE_ARM",	EMGRINDING_CLASS_ARM )
		.Const( "EMGRINDING_PURPOSE_CLOTH",	EMGRINDING_CLASS_CLOTH )
		.Const( "EMGRINDING_PURPOSE_SIZE",	EMGRINDING_CLASS_SIZE );

	Sqrat::ConstTable(m_vm)
		.Const( "EMGRINDING_TYPE_NONE",				EMGRINDING_NUNE )
		.Const( "EMGRINDING_TYPE_DAMAGE",			EMGRINDING_DAMAGE )
		.Const( "EMGRINDING_TYPE_DEFENSE",			EMGRINDING_DEFENSE )
		.Const( "EMGRINDING_TYPE_RESIST_FIRE",		EMGRINDING_RESIST_FIRE )
		.Const( "EMGRINDING_TYPE_RESIST_ICE",		EMGRINDING_RESIST_ICE )
		.Const( "EMGRINDING_TYPE_RESIST_ELEC",		EMGRINDING_RESIST_ELEC )
		.Const( "EMGRINDING_TYPE_RESIST_POISON",	EMGRINDING_RESIST_POISON )
		.Const( "EMGRINDING_TYPE_RESIST_SPIRIT",	EMGRINDING_RESIST_SPIRIT )
		.Const( "EMGRINDING_TYPE_NSIZE",			EMGRINDING_NSIZE );


	Sqrat::ConstTable(m_vm)
		.Const( "EMGRINDING_LEVEL_NORMAL",	EMGRINDER_NORMAL )
		.Const( "EMGRINDING_LEVEL_HIGH",	EMGRINDER_HIGH )
		.Const( "EMGRINDING_LEVEL_TOP",		EMGRINDER_TOP )
		.Const( "EMGRINDING_LEVEL_SIZE",	EMGRINDER_SIZE );

	Sqrat::ConstTable(m_vm)
		.Const( "EMGRINDING_CONDITION_FAILED",		EMGRINDINGCONDITION_FAILED )
		.Const( "EMGRINDING_CONDITION_SUCCESS",		EMGRINDINGCONDITION_SUCCESS )
		.Const( "EMGRINDING_CONDITION_NOITEM",		EMGRINDINGCONDITION_NOITEM )
		.Const( "EMGRINDING_CONDITION_MAX",			EMGRINDINGCONDITION_MAX )
		.Const( "EMGRINDING_CONDITION_NOTBEST",		EMGRINDINGCONDITION_NOTBEST )
		.Const( "EMGRINDING_CONDITION_NOTHIGH",		EMGRINDINGCONDITION_NOTHIGH )
		.Const( "EMGRINDING_CONDITION_NOTBESTITEM",	EMGRINDINGCONDITION_NOTBESTITEM )
		.Const( "EMGRINDING_CONDITION_NUM",			EMGRINDINGCONDITION_NOTNUM )
		.Const( "EMGRINDING_CONDITION_DEFPURPOSE",	EMGRINDINGCONDITION_DEFCLASS );

	Sqrat::ConstTable(m_vm)
		.Const("GRADE_NORMAL",		GRADE_NORMAL)
		.Const("GRADE_HIGH",		GRADE_HIGH)
		.Const("GRADE_LIMIT_MAX",	GRADE_LIMIT_MAX);

	Sqrat::ConstTable(m_vm)
		.Const( "ATTR_NULL",				EMGRINDING_ATTRIBUTE_NULL )
		.Const( "ATTR_RANDOM",				EMGRINDING_ATTRIBUTE_RANDOM )
		.Const( "ATTR_NODEGRATION",			EMGRINDING_ATTRIBUTE_NODEGRATION )
		.Const( "ATTR_IGNORE_PROTECTED",	EMGRINDING_ATTRIBUTE_IGNOREPROTECTED )
		.Const( "ATTR_IGNORE_RATEINC",		EMGRINDING_ATTRIBUTE_IGNORERATEINC )
		.Const( "ATTR_IGNORE_DEFAULTREQ",	EMGRINDING_ATTRIBUTE_IGNOREDEFAULTREQ )
		.Const( "ATTR_ADDED",				EMGRINDING_ATTRIBUTE_ADDED );

	Sqrat::ConstTable(m_vm)
		.Const( "OPTION_NONE",				EMGRINDINGOPTION_NONE )
		.Const( "OPTION_ANTIDISAPPEAR",		EMGRINDINGOPTION_ANTIDISAPPEAR )
		.Const( "OPTION_INC_GRINDINGRATE",	EMGRINDINGOPTION_INC_GRINDINGRATE )
		.Const( "OPTION_ANTIRESET",			EMGRINDINGOPTION_ANTIRESET );

	Sqrat::ConstTable(m_vm)
		.Const( "RESULT_FAILED",				EMGRINDINGRESULT_FAILED )
		.Const( "RESULT_SUCCESS",				EMGRINDINGRESULT_SUCCESS )
		.Const( "RESULT_USE_ANTIDISAPPEAR",		EMGRINDINGRESULT_ANTIDISAPPEAR )
		.Const( "RESULT_USE_INC_GRINDINGRATE",	EMGRINDINGRESULT_INC_GRINDINGRATE )
		.Const( "RESULT_USE_ANTIRESET",			EMGRINDINGRESULT_ANTIRESET )
		.Const( "RESULT_RESET",					EMGRINDINGRESULT_RESET )
		.Const( "RESULT_TERMINATE",				EMGRINDINGRESULT_TERMINATE );
}

void CGrindingScript::Clear()
{
	SAFE_DELETE(m_sqTbOption);
	SAFE_DELETE(m_sqTbLogic);
	SAFE_DELETE(m_sqFnEntry);
	SAFE_DELETE(m_sqFnCondition);
	SAFE_DELETE(m_sqFnConsume);
}

CGrindingScript::TYPE_ENTRY_RESULT CGrindingScript::DoEntry( IN const TYPE_ENTRY_ARG1& dwGrindingFlag, IN const TYPE_ENTRY_ARG2& srcGrinder, IN OUT const TYPE_ENTRY_ARG3& dstItem )
{
	if ( !m_sqFnEntry )
	{
		sc::writeLogError( sc::string::format(
			"CGrindingScript::DoEntry() - m_sqFnEntry is null (%1%)",
			m_strFilePath)
		);

		return TYPE_ENTRY_RESULT(NULL);
	}

	return TYPE_ENTRY_RESULT(m_sqFnEntry->Evaluate<TYPE_ENTRY_RESULT, TYPE_ENTRY_ARG1, TYPE_ENTRY_ARG2, TYPE_ENTRY_ARG3>(
		dwGrindingFlag,
		srcGrinder,
		dstItem
	));
}

CGrindingScript::TYPE_CONDITION_RESULT CGrindingScript::DoCondition( IN const TYPE_CONDITION_ARG1& srcGrinder, IN const TYPE_CONDITION_ARG2& dstItem )
{
	if ( !m_sqFnCondition )
	{
		sc::writeLogError( sc::string::format(
			"CGrindingScript::DoCondition() - m_sqFnCondition is null (%1%)",
			m_strFilePath)
			);

		return TYPE_CONDITION_RESULT(NULL);
	}

	return TYPE_CONDITION_RESULT(m_sqFnCondition->Evaluate<TYPE_CONDITION_RESULT, TYPE_CONDITION_ARG1, TYPE_CONDITION_ARG2>(
		srcGrinder,
		dstItem
		));
}

CGrindingScript::TYPE_CONSUME_RESULT CGrindingScript::DoConsume( IN const TYPE_CONSUME_ARG1& srcGrinder, IN const TYPE_CONSUME_ARG2& dstItem )
{
	if ( !m_sqFnConsume )
	{
		sc::writeLogError( sc::string::format(
			"CGrindingScript::DoConsume() - m_sqFnConsume is null (%1%)",
			m_strFilePath)
			);

		return TYPE_CONSUME_RESULT(NULL);
	}

	return TYPE_CONSUME_RESULT(m_sqFnConsume->Evaluate<TYPE_CONSUME_RESULT, TYPE_CONSUME_ARG1, TYPE_CONSUME_ARG2>(
		srcGrinder,
		dstItem
		));
}

void CGrindingScript::ToDo()
{
	using namespace Sqrat;

	try
	{
		Sqrat::Object ObjLogic = RootTable(m_vm).GetSlot( "GrindingLogic" );

		if ( !ObjLogic.IsNull() )
		{
			m_sqTbLogic		= new Sqrat::Table(ObjLogic);
			m_sqFnEntry		= new Sqrat::Function(m_sqTbLogic->GetFunction( "Entry" ));
			m_sqFnCondition	= new Sqrat::Function(m_sqTbLogic->GetFunction( "Condition" ));
			m_sqFnConsume	= new Sqrat::Function(m_sqTbLogic->GetFunction( "Consume" ));

			if ( m_sqFnEntry->IsNull() )	 
				SAFE_DELETE(m_sqFnEntry);

			if ( m_sqFnCondition->IsNull() ) 
				SAFE_DELETE(m_sqFnCondition);

			if ( m_sqFnConsume->IsNull() )	 
				SAFE_DELETE(m_sqFnConsume);
		}

		Sqrat::Object ObjOption = RootTable(m_vm).GetSlot( "GrindingOption" );

		if ( !ObjOption.IsNull() )
		{
			m_sqTbOption = new Sqrat::Table(ObjOption);
		}

		if ( ObjLogic.IsNull() && ObjOption.IsNull() )
		{
			sc::writeLogError( sc::string::format( 
				"CGrindingScript::ToDo() - At least 'GrindingLogic' table or 'GrindingOption tables must exist (%1%)", 
				m_strFilePath) 
			);

			return;
		}
	}
	catch( Sqrat::Exception ex )
	{
		sc::writeLogError( static_cast<std::string>(ex.Message()) );
	}

}
