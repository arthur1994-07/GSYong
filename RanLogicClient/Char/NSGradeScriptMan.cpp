#include "pch.h"

#include <boost/tr1/memory.hpp>

#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../EngineLib/DxTools/DxVertexFVF.h"
#include "../../EngineLib/Common/SUBPATH.h"
#include "../../RanLogic/Script/SquirrelMan.h"
#include "../../RanLogic/Item/GLItemDef.h"

#include "NSGradeScriptMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace NSGradeScriptMan
{
    // 전역 정의
    enum
    {
        GRADE_UP   = 0,
        GRADE_DOWN = 1,
        GRADE_HAND = 2,
        GRADE_FOOT = 3,
        GRADE_SIZE = 4,
    };
    TSTRING g_strEmpty(_T(""));


    // Grade 클래스
    class Grade : public Sqrat::SqObjectType
    {
    private:
        VEC_TSTRING m_vecstrGrade[2][ GRADE_SIZE ]; // 2 - 남자여자 / GRADE_SIZE - 연마단계

        void SetGradePart( Sqrat::Object& myObject, int nSex, int nPart )
        {
            SqStdVector<Sqrat::Object> testVec;
 		    GetVectorFromArray( myObject,&testVec );
		    for (size_t i = 0; i < testVec.size(); ++i)
		    {
                m_vecstrGrade[nSex][ nPart ].push_back( testVec[i].Cast<Sqrat::string>().c_str() );
		    }
        }

    public:
        void SetGradeUpMan( Sqrat::Object& myObject )
        {
            SetGradePart( myObject, 1, GRADE_UP );
        }

        void SetGradeDownMan( Sqrat::Object& myObject )
        {
            SetGradePart( myObject, 1, GRADE_DOWN );
        }

        void SetGradeHandMan( Sqrat::Object& myObject )
        {
            SetGradePart( myObject, 1, GRADE_HAND );
        }

        void SetGradeFootMan( Sqrat::Object& myObject )
        {
            SetGradePart( myObject, 1, GRADE_FOOT );
        }

        void SetGradeUpWoman( Sqrat::Object& myObject )
        {
            SetGradePart( myObject, 0, GRADE_UP );
        }

        void SetGradeDownWoman( Sqrat::Object& myObject )
        {
            SetGradePart( myObject, 0, GRADE_DOWN );
        }

        void SetGradeHandWoman( Sqrat::Object& myObject )
        {
            SetGradePart( myObject, 0, GRADE_HAND );
        }

        void SetGradeFootWoman( Sqrat::Object& myObject )
        {
            SetGradePart( myObject, 0, GRADE_FOOT );
        }

        // Grade Cps 를 얻음
        const TSTRING& GetGradeCps( int nSex, int nPart, size_t nLevel )
        {
            if ( nSex >= 2 )
                return g_strEmpty;

            if ( nLevel >= m_vecstrGrade[nSex][nPart].size() )
                return g_strEmpty;

            return m_vecstrGrade[nSex][nPart][nLevel];
        }

    public:	
	    Grade()
        {
        }

        ~Grade()
        {
        }
    };

    // GradeScriptMan 클래스
    class GradeScriptMan : public SquirrelMan
    {
    private:
        Grade m_sGrade;

    public:
	    GradeScriptMan()
        {
            Sqrat::Class<Grade> sGrade(m_vm);
        	
	        sGrade.Func( "SetGradeUpMan",	    &Grade::SetGradeUpMan );
	        sGrade.Func( "SetGradeDownMan",     &Grade::SetGradeDownMan );
            sGrade.Func( "SetGradeHandMan",	    &Grade::SetGradeHandMan );
	        sGrade.Func( "SetGradeFootMan",     &Grade::SetGradeFootMan );
            sGrade.Func( "SetGradeUpWoman",	    &Grade::SetGradeUpWoman );
	        sGrade.Func( "SetGradeDownWoman",   &Grade::SetGradeDownWoman );
            sGrade.Func( "SetGradeHandWoman",	&Grade::SetGradeHandWoman );
	        sGrade.Func( "SetGradeFootWoman",   &Grade::SetGradeFootWoman );

	        Sqrat::RootTable(m_vm).Bind( "Grade", sGrade );
        }

	    virtual ~GradeScriptMan()
        {
        }

	    virtual void ToDo()
        {
            Sqrat::Function func = Sqrat::RootTable(m_vm).GetFunction( _T("GradeFunc") );
 		    func( &m_sGrade );
        }

        // Grade Cps 를 얻음
        const TSTRING& GetGradeCps( int nSex, int nPart, int nLevel )
        {
            return m_sGrade.GetGradeCps( nSex, nPart, nLevel );
        }
    };


    // 전역정의
    std::tr1::shared_ptr<GradeScriptMan> g_spSquirrelMan = std::tr1::shared_ptr<GradeScriptMan>( new GradeScriptMan );


    // 함수
    void LoadScript( SUBPATH* pPath )
    {
        std::string strScriptPath( sc::getAppPath() );
	    strScriptPath.append( pPath->GLogicPath() );    //SUBPATH::GLogicServerPath());
	    strScriptPath.append( "scripts\\" );
        strScriptPath.append( _T("GrideCharCps.nut") );

        g_spSquirrelMan->LoadScript( strScriptPath.c_str() );
    }

    // Grade Cps 를 얻음
    const TSTRING& GetGradeCps( int nSex, int nSlot, int nLevel )
    {
        int nPart(0);
        switch( nSlot )
        {
        case SLOT_UPPER:
            nPart = GRADE_UP;
            break;
        case SLOT_LOWER:
            nPart = GRADE_DOWN;
            break;
        case SLOT_HAND:
            nPart = GRADE_HAND;
            break;
        case SLOT_FOOT:
            nPart = GRADE_FOOT;
            break;
        default:
            return g_strEmpty;
        };

        if ( nPart >= GRADE_SIZE )
            return g_strEmpty;

        nLevel -= 1;
        if ( nLevel < 0 )
            return g_strEmpty;

        return g_spSquirrelMan->GetGradeCps( nSex, nPart, nLevel );
    }
};