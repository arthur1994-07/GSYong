#pragma once

#include "../../RanLogic/Script/SquirrelMan.h"
#include "../../RanLogic/Item/GLItemDef.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"

#include <boost/type_traits/is_enum.hpp> 

struct SITEM;
struct SITEMCUSTOM;

class SQGrindingItem : public Sqrat::SqObjectType
{

public:
	SQGrindingItem();
	SQGrindingItem( SITEMCUSTOM* _ItemCustom );
	SQGrindingItem( SITEMCUSTOM& _ItemCustom );
	SQGrindingItem( const SITEMCUSTOM* _ItemCustom );
	SQGrindingItem( const SITEMCUSTOM& _ItemCustom );

public:

public:
	/*EMGRINDING_TYPE*/	const UINT	GetGrindingType();
	/*EMGRINDER_TYPE*/	const UINT	GetGrindingLevel();
	/*EMGRINDING_CLASS*/const UINT	GetGrindingPurpose();
	/*EMGRINDING_CLASS*/const UINT	GetSuitType2Purpose();
	/*EMSUIT*/			const UINT	GetSuitType();

public:
	const UINT  GetDefaultGrindingConsumeCount( const UINT nIndex );
	const float GetDefaultGrindingRate( const UINT nIndex );
	const float GetDefaultGrindingResetRate( const UINT nIndex );
	const float GetDefaultGrindingTerminateRate( const UINT nIndex );
	const UINT  GetDefaultGrindingAntiResetCount( const UINT nIndex );
	const float GetDefaultGrindingIncreaseRate( const UINT nIndex );


public:
	const UINT	GetTurnNum();
	const UINT  GetGradeLimit(/*EMGRINDING_TYPE*/const UINT nType);
	const UINT	GetGrade( /*EMGRINDING_TYPE*/const UINT nType );
	void		SetGrade( /*EMGRINDING_TYPE*/const UINT nType, const UINT nValue );
	void		DecGrade( /*EMGRINDING_TYPE*/const UINT nType, const UINT nValue );
	void		ResetGrade( /*EMGRINDING_TYPE*/const UINT nType );

public:
	const SITEM* m_pItem;
	SITEMCUSTOM* m_pItemcustom;

};

class CGrindingScript : public SquirrelMan
{
private:
	template<
		typename T, 
		typename boost::is_enum<T>::value_type IS_ENUM=boost::is_enum<T>::value
	> struct TypeTraits
	{     
		typedef T type; 
		static const bool IS_ENUM = IS_ENUM;
	} ;

public:
	typedef DWORD				TYPE_ENTRY_RESULT;	// 결과 값 플래그
	typedef	DWORD				TYPE_ENTRY_ARG1;	// 연마 옵션 플래그
	typedef SQGrindingItem		TYPE_ENTRY_ARG2;	// 연마제
	typedef	SQGrindingItem		TYPE_ENTRY_ARG3;	// 대상아이템

	typedef DWORD				TYPE_CONDITION_RESULT;	// 조건 검사 결과
	typedef SQGrindingItem		TYPE_CONDITION_ARG1;	// 연마제
	typedef SQGrindingItem		TYPE_CONDITION_ARG2;	// 대상아이템

	typedef DWORD				TYPE_CONSUME_RESULT;	// 소모량 결과
	typedef SQGrindingItem		TYPE_CONSUME_ARG1;		// 연마제
	typedef SQGrindingItem		TYPE_CONSUME_ARG2;		// 대상아이템

public:
	CGrindingScript();
	virtual ~CGrindingScript();

public:
	void Bind();
	void Clear();

public:
	const bool IsExistOption()	  { return m_sqTbOption ? true : false; }
	const bool IsExistEntry()	  { return m_sqFnEntry ? true : false; }
	const bool IsExistCondition() { return m_sqFnCondition ? true : false; }
	const bool IsExistConsume()	  { return m_sqFnConsume ? true : false; }

public:
	TYPE_ENTRY_RESULT DoEntry( IN const TYPE_ENTRY_ARG1& dwGrindingFlag, IN const TYPE_ENTRY_ARG2& srcGrinder, IN OUT const TYPE_ENTRY_ARG3& dstItem );
	TYPE_CONDITION_RESULT DoCondition( IN const TYPE_CONDITION_ARG1& srcGrinder, IN const TYPE_CONDITION_ARG2& dstItem );
	TYPE_CONSUME_RESULT DoConsume( IN const TYPE_CONSUME_ARG1& srcGrinder, IN const TYPE_CONSUME_ARG2& dstItem );

public:
	template<typename T>
	const bool GetOptionSlot( const std::string& strSlotName, OUT T& value )
	{
		return GetOptionSlot(strSlotName,value,TypeTraits<T>());
	}

private:
	template<typename T,typename U>
	const bool GetOptionSlot( const std::string& strSlotName, OUT T& value, U);

	template<typename T>
	const bool GetOptionSlot( const std::string& strSlotName, OUT T& value, TypeTraits<T,true> type )
	{
		if ( !m_sqTbOption )
		{
			return false;
		}

		Sqrat::Object obj = m_sqTbOption->GetSlot(strSlotName.c_str());

		if ( obj.IsNull() )
		{
			return false;
		}

		try {
			value = T(obj.Cast<UINT>());
		}
		catch ( Sqrat::Exception ex )
		{
			sc::writeLogError( sc::string::format(
				"CGrindingScript::GetOptionSlot() - '%1%' => %2%",
				m_strFilePath,
				ex.Message())
			);

			return false;
		}

		return true; 		
	}

	template<typename T>
	const bool GetOptionSlot( const std::string& strSlotName, OUT T& value, TypeTraits<T,false> type )
	{
		if ( !m_sqTbOption )
		{
			return false;
		}

		Sqrat::Object obj = m_sqTbOption->GetSlot(strSlotName.c_str());

		if ( obj.IsNull() )
		{
			return false;
		}

		try {
			value = obj.Cast<T>();
		}
		catch ( Sqrat::Exception ex )
		{
			sc::writeLogError( sc::string::format(
				"CGrindingScript::GetOptionSlot() - '%1%' => %2%",
				m_strFilePath,
				ex.Message())
				);

			return false;
		}

		return true; 		
	}

	template<>
	const bool GetOptionSlot( const std::string& strSlotName, OUT SGRINDING_OPT::MAP_CHANCE& value, TypeTraits<SGRINDING_OPT::MAP_CHANCE> )
	{
		if ( !m_sqTbOption )
		{
			return false;
		}

		Sqrat::Object sqObject = m_sqTbOption->GetSlot(strSlotName.c_str());

		if ( sqObject.IsNull() )
		{
			return false;
		}

		try {
			Sqrat::Array sqArr(sqObject);

			value.clear();

			SqStdVector<Sqrat::Object> vecSQArray;
			Sqrat::GetVectorFromArray( sqArr, &vecSQArray );

			if ( vecSQArray.empty() )
			{
				return false;
			}

			float fTotalRate = 0.0f;

			UINT nSize = static_cast<UINT>(vecSQArray.size());
			for ( UINT i=0; i<nSize; ++i )
			{
				Sqrat::Table sqTable(vecSQArray[i]);
				
				Sqrat::Object objGrade   = sqTable.GetSlot("Grade");
				Sqrat::Object objPercent = sqTable.GetSlot("Percent");

				if ( objGrade.IsNull() )
				{
					sc::writeLogError( sc::string::format(
						"CGrindingScript::GetOptionSlot() - '%1%' => %2%",
						m_strFilePath,
						"''Grade' Key is not exists")
						);

					value.clear();
					return false;
				}

				if ( objPercent.IsNull() )
				{
					sc::writeLogError( sc::string::format(
						"CGrindingScript::GetOptionSlot() - '%1%' => %2%",
						m_strFilePath,
						"''Percent' Key is not exists")
						);

					value.clear();
					return false;
				}

				WORD  wGrade   = objGrade.Cast<SGRINDING_OPT::MAP_CHANCE::key_type>();
				float fPercent = objPercent.Cast<SGRINDING_OPT::MAP_CHANCE::mapped_type>();

				value[wGrade] = fPercent;

				fTotalRate += fPercent;
			}
		}
		catch ( Sqrat::Exception ex )
		{
			sc::writeLogError( sc::string::format(
				"CGrindingScript::GetOptionSlot() - '%1%' => %2%",
				m_strFilePath,
				ex.Message())
				);

			return false;
		}

		return true; 		
	}

public:
	virtual void ToDo();

private:
	Sqrat::Table*	m_sqTbOption;
	Sqrat::Table*	m_sqTbLogic;

private:
	Sqrat::Function* m_sqFnEntry;		// 스크립트 Entry(실제 연마 진행)
	Sqrat::Function* m_sqFnCondition;	// 스크립트 연마 조건
	Sqrat::Function* m_sqFnConsume;		// 스크립트 연마제 소모량

};
