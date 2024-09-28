#pragma once

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/Character/GLCharData.h"
#include "../../RanLogic/GLogicData.h"

#include "../../RanLogic/Script/SquirrelMan.h"

#include <map>

// ----------------------------------------------------------------------------

namespace buffmanager
{
    struct SQBuff : public Sqrat::SqObjectType
    {
        SQBuff()
            : sID(SNATIVEID(false))
            , wLEVEL(0)
            , fAGE(_SKILLFACT_DEFAULTTIME)
        {
        }

        SQBuff( const WORD _wMid, const WORD _wSid )
            : sID(_wMid,_wSid)
            , wLEVEL(0)
            , fAGE(_SKILLFACT_DEFAULTTIME)
            , bEXPIRED(false)
        {
        }

        SQBuff( const WORD _wMid, const WORD _wSid, const WORD _wLEVEL )
            : sID(_wMid,_wSid)
            , wLEVEL(_wLEVEL)
            , fAGE(_SKILLFACT_DEFAULTTIME)
            , bEXPIRED(false)
        {
        }

        SQBuff( const WORD _wMid, const WORD _wSid, const WORD _wLEVEL, const float _fAGE, const bool _bEXPIRED )
            : sID(_wMid,_wSid)
            , wLEVEL(_wLEVEL)
            , fAGE(_fAGE)
            , bEXPIRED(_bEXPIRED)
        {
        }


        SQBuff( const DWORD _dwID, const WORD _wLEVEL )
            : sID(_dwID)
            , wLEVEL(_wLEVEL)
            , fAGE(_SKILLFACT_DEFAULTTIME)
            , bEXPIRED(false)
        {
        }

        SQBuff( const DWORD _dwID, const WORD _wLEVEL, const float _fAGE, const bool _bEXPIRED )
            : sID(_dwID)
            , wLEVEL(_wLEVEL)
            , fAGE(_fAGE)
            , bEXPIRED(_bEXPIRED)
        {
        }

        const bool operator < ( const SQBuff& rhs ) const
        {
            if ( sID.dwID < rhs.sID.dwID )
				return true;
			else if( rhs.sID.dwID < sID.dwID )
				return false;

            return  wLEVEL < rhs.wLEVEL;
        }

        const bool operator == ( const SQBuff& rhs ) const
        {
            if ( sID != rhs.sID )
            {
                return false;
            }

            if ( wLEVEL != rhs.wLEVEL )
            {
                return false;
            }

            return true;
        }

        SSKILLFACT::STIME GetTime( __time64_t tCurrentTime )
        {
            if ( bEXPIRED )
            {
                UINT nEXPIREDTIME = static_cast<UINT>(tCurrentTime) + static_cast<UINT>(fAGE);
                return SSKILLFACT::STIME(nEXPIREDTIME);
            }

            return SSKILLFACT::STIME(fAGE);
        }

        SSKILLFACT::STIME Make_Time( __time64_t tExpiredTime )
        {
            if ( bEXPIRED )
            {
                UINT nEXPIREDTIME = static_cast<UINT>(tExpiredTime);
                return SSKILLFACT::STIME(nEXPIREDTIME);
            }

            return SSKILLFACT::STIME(fAGE);
        }

        SNATIVEID sID;
        WORD      wLEVEL;
        float     fAGE;
        bool      bEXPIRED;
    };

    // ----------------------------------------------------------------------------

    struct SBuffBinder : public Sqrat::ForEachFunction<std::string, Sqrat::Object>
    {
    private:
        typedef std::map<std::string,Sqrat::Function> MFUNCTION;
        typedef MFUNCTION::iterator                   MFUNCTION_ITER;

        typedef std::map<std::string,SQInteger>       MINTEGER;
        typedef MINTEGER::iterator                    MINTEGER_ITER;

    public:
        virtual void operator()( const std::string& key, const Sqrat::Object& value )
        {
            std::string strkey = m_strCurrentKey;

            if ( strkey.size() != 0 )
            {
                strkey += "::";
            }

            strkey += key;

            switch( value.GetType() )
            {
            case OT_TABLE:
                {
                    std::string strParentkey = m_strCurrentKey;
                    m_strCurrentKey = strkey;

                    Sqrat::ForEach(value, (*this));

                    m_strCurrentKey = strParentkey;
                }
                break;

            case OT_CLOSURE:
                {

                    Sqrat::Function sqFunc = value.Cast<Sqrat::Function>();
                    m_mFunction.insert( std::make_pair(strkey, sqFunc) );
                }
                break;

            case OT_INTEGER:
                {
                    SQInteger sqInt = value.Cast<SQInteger>();
                    m_mInteger.insert( std::make_pair(strkey, sqInt) );
                }
                break;
            };
        }

    public:
        SBuffBinder( HSQUIRRELVM hvm ) 
            : m_hvm(hvm)
        {}

        ~SBuffBinder()
        {
            Clear();
        }

    public:
        void Clear()
        {
            m_strCurrentKey.clear();
            m_mFunction.clear();
            m_mInteger.clear();
        }

    public:
        Sqrat::Function& GetFunction ( const std::string& strKey )
        {
            MFUNCTION_ITER finditer = m_mFunction.find(strKey);

            if ( finditer == m_mFunction.end() )
            {
                sc::writeLogError( sc::string::format("SBuffBinder::GetFunction - not found key '%1%' ", strKey) );
                GASSERT( !"SBuffBinder::GetFunction - not found key" );
            }

            return finditer->second;
        }

        SQInteger& GetInteger ( const std::string& strKey )
        {
            MINTEGER_ITER finditer = m_mInteger.find(strKey);

            if ( finditer == m_mInteger.end() )
            {
                sc::writeLogError( sc::string::format("SBuffBinder::GetInteger - not found key '%1%' ", strKey) );
                GASSERT( !"SBuffBinder::GetInteger - not found key" );
            }

            return finditer->second;
        }

    private:
        HSQUIRRELVM   m_hvm;
        MFUNCTION     m_mFunction;
        MINTEGER      m_mInteger;

    private:
        std::string   m_strCurrentKey;
    };

    // ----------------------------------------------------------------------------

    class GLBuffManager : public SquirrelMan
    {
    public:
        enum EMCTFBUFFTYPE
        {
            ENUM_BUFF = 30,
        };

    public:
        typedef SqStdVector<Sqrat::Array>                       VECSQARRAY;
        typedef VECSQARRAY::iterator                            VECSQARRAY_ITER;

        typedef SqStdVector<SQBuff>                             VECBUFF;
        typedef VECBUFF::iterator                               VECBUFF_ITER;

        typedef std::map<SQBuff, EMCTF_REWARD_BUFF_TYPE>        MAPCTFBUFFTYPE;
        typedef MAPCTFBUFFTYPE::iterator                        MAPCTFBUFFTYPE_ITER;

    public:
        GLBuffManager();
        virtual ~GLBuffManager();

    public:
        virtual void ToDo();
        virtual void ToDo_BindTrigger() = 0;

    public:
        EMCTF_REWARD_BUFF_TYPE GetCTFRewardBuffType( const SQBuff& sqBuff );
		WORD GetCTFBuffMID() { return m_wCTFBuffMID; }

    public:
        void Clear();

    protected:
         SBuffBinder        m_sBinder;

    protected:
        //! CTF
        WORD                m_wCTFBuffMID;
		WORD				m_wCTFBuffRebirthNum;
        MAPCTFBUFFTYPE      m_mCTFBuffTypes;
		Sqrat::Function     m_fnSQGetCTFBuffRebirth;		
        Sqrat::Function     m_fnSQGetCTFBuffReward;
        Sqrat::Function     m_fnSQGetCTFBuffBalance;

    protected:
        VECBUFF             m_vecCTFBuffBalance[SCHOOL_NUM];

    };
};

