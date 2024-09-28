#include "../pch.h"

#include "./GLBuffManager.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/gassert.h"

#include "../../RanLogic/GLUseFeatures.h"

#include <map>
#include <string>
#include <boost/function.hpp>
#include <boost/bind.hpp>

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace buffmanager;
using namespace Sqrat;

// ----------------------------------------------------------------------------

GLBuffManager::GLBuffManager()
: m_sBinder(m_vm)
, m_wCTFBuffMID(USHRT_MAX)
{
    //! 환경 바인딩
	Sqrat::ClassType<SQBuff>::Initialized() = false;
    Sqrat::RootTable(m_vm).Bind( "BUFF", Sqrat::ImprovedClass<SQBuff>(m_vm)
        //.Ctor<const WORD,const WORD>()
        //.Ctor<const WORD,const WORD,const WORD>()
        .Ctor<const WORD,const WORD,const WORD, const float, const bool>()
        );

    Sqrat::ConstTable(m_vm)
        .Const("BUFF_AGE_INFINITY", FLT_MAX)
        .Const("BUFF_AGE_DEFAULT",  _SKILLFACT_DEFAULTTIME);

    Sqrat::ConstTable(m_vm).Enum("ENUM_SCHOOL", Enumeration(m_vm)
        .Const( "SCHOOL_SM", SCHOOL_SM )
        .Const( "SCHOOL_BH", SCHOOL_BH )
        .Const( "SCHOOL_HA", SCHOOL_HA )
        );

    Sqrat::ConstTable(m_vm).Enum("ENUM_BUFF_TYPE", Enumeration(m_vm)
        .Const( "BUFF_TYPE_NORMAL",   EMSKILLFACT_TYPE_NORMAL )
        .Const( "BUFF_TYPE_LUNCHBOX", EMSKILLFACT_TYPE_LUNCHBOX )
        .Const( "BUFF_TYPE_SYSTEM",   EMSKILLFACT_TYPE_SYSTEM )
        );
}

GLBuffManager::~GLBuffManager()
{
    Clear();
}

void GLBuffManager::Clear()
{
    m_sBinder.Clear();

    m_mCTFBuffTypes.clear();
}

void GLBuffManager::ToDo()
{
    try
    {
        Sqrat::Object objTable = RootTable(m_vm).GetSlot( "BuffManagerType" );

        if ( objTable.IsNull() )
        {
            GASSERT( !"GLBuffManagerFD::ToDo() - not found 'BuffManagerType'" );
        }

        //! 함수들을 바인딩
        Sqrat::ForEach(objTable, m_sBinder);

        //! 바인딩된 정보들을 세팅한다.

        //! CTF Buff MID
        {
            m_wCTFBuffMID = WORD(m_sBinder.GetInteger( "CTF::nBuffMID" ));
			m_wCTFBuffRebirthNum = WORD(m_sBinder.GetInteger("CTF::nBuffRebirth_Num"));
        }

        //! CTF Buff Types
        {
			Sqrat::Array arrCTFBuffTypes( m_sBinder.GetFunction( "CTF::GetBuffTypes" ).Evaluate<Sqrat::Object>() );

            VECSQARRAY vecSQArray;
            Sqrat::GetVectorFromArray( arrCTFBuffTypes, &vecSQArray );

            UINT nSize = static_cast<UINT>(vecSQArray.size());

            VECBUFF vecBuff;
            for ( UINT i=0; i<nSize; ++i )
            {
                Sqrat::GetVectorFromArray( vecSQArray[i], &vecBuff );

                VECBUFF_ITER iter = vecBuff.begin();
                for ( ; iter!=vecBuff.end(); ++iter )
                {
                    m_mCTFBuffTypes.insert( std::make_pair((*iter), EMCTF_REWARD_BUFF_TYPE(i+1)) );
                }
            }
        }

        //! CTF Buff Functuon
        {
            m_fnSQGetCTFBuffReward  = m_sBinder.GetFunction( "CTF::GetBuffReward" );
            m_fnSQGetCTFBuffBalance = m_sBinder.GetFunction( "CTF::GetBuffBalance" );
			m_fnSQGetCTFBuffRebirth = m_sBinder.GetFunction( "CTF::GetBuffRebirth" );	
			
        }
    }
    catch ( Sqrat::Exception ex )
    {
        sc::writeLogError( static_cast<std::string>(ex.Message()) );
    }

    //! 트리거를 바인딩한다.
    ToDo_BindTrigger();
}

EMCTF_REWARD_BUFF_TYPE GLBuffManager::GetCTFRewardBuffType( const SQBuff& sqBuff )
{
    MAPCTFBUFFTYPE_ITER finditer = m_mCTFBuffTypes.find( sqBuff );

    if ( finditer == m_mCTFBuffTypes.end() )
    {
		return EMCTF_REWARD_BUFF_TYPE_NULL;
    }

   return (finditer->second);
}

