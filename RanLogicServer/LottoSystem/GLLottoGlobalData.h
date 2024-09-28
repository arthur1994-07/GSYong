//-----------------------------------------------------------------------
/** @file GLLottoGlobalData.h
	@brief Server Script ���Ͽ��� �����ϴ� ��� Data�� �����ϴ� Class;
*///---------------------------------------------------------------------
#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "./GLLottoDataTypeServer.h"

namespace LottoSystem
{

	// Lotto System�� Data Manager;
	class GLLottoGlobalData : public CSingletone<GLLottoGlobalData>
	{
	protected:
		friend CSingletone;

	public:
		GLLottoGlobalData()
			: m_systemCount( 0 ) { }
		~GLLottoGlobalData() { }

	public:
		// Loading Script File;
		const bool LoadScriptFile ( const std::string& _strFileName );

	public:
		// Getter;
		inline const unsigned int GetLottoSystemNum() const { return m_systemCount; }
		inline const LOTTO_GLOBAL_VALUE GetLottoGlobalValue() const { return m_sLottoGlobalValue; }
		inline const LOTTO_SYSTEM_VALUE GetLottoSystemValue( unsigned int id ) const
		{
			if( id >= 0 && id < MAX_LOTTO_SYSTEM )
				return m_lottoSystemValues[ id ];

			return LOTTO_SYSTEM_VALUE();
		}

	private:
		// �ζ� �ý��� ���� ���� ��;
		LOTTO_GLOBAL_VALUE m_sLottoGlobalValue;

		// �ζ� �ý��� �� ���� ��;
		LOTTO_SYSTEM_VALUE m_lottoSystemValues[ MAX_LOTTO_SYSTEM ];
		unsigned int m_systemCount;
	};

}