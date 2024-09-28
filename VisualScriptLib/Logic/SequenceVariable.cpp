#include "stdafx.h"

#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

#include "../../InternalCommonLib/Core/CommonUtil.h"
#include "../../InternalCommonLib/Interface/RenderInterface.h"
#include "../../InternalCommonLib/Interface/PlayerInterface.h"

#include "./Sequence.h"
#include "./SequenceVariable.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace vs
{
    CSequenceVariable::CSequenceVariable()
    {
        strName = _T( "Variable" );
        nColor = D3DCOLOR_XRGB( 0, 0, 0 );
        bDrawLast = TRUE;
    }

    VOID CSequenceVariable::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyString* pProperty = ic::CPropertyFactory::New< ic::CPropertyString >();
            pProperty->SetPtr( &strVarName );
            pProperty->SetCall( boost::bind( boost::mem_fn( &CSequenceVariable::OnChangedVarName ), this ) );
            InsertProperty( _T( "Var Name" ), pProperty );
        }
    }

    VOID CSequenceVariable::DrawObject(
        ic::IRenderInterface* pRI,
        BOOL bSelected,
        BOOL bMouseOver,
        INT nMouseOverConnType,
        INT nMouseOverConnIndex )
    {
        const INT CircleSides = 16;

        if( pRI->IsHitTesting() )
            pRI->SetHitProxy( new ic::SLinkedObjProxy( this ) );

        DWORD nBorderColor = GetBorderColor( bSelected, bMouseOver );

	    if( !strComment.empty() )
	    {
            SIZE sTextSize = { 0, 0 };
            pRI->GetTextExtent(
                sTextSize,
                strComment,
                pRI->DefaultFontName(),
                pRI->DefaultFontSize(),
                pRI->DefaultFontFlags() );

            pRI->DrawText(
                FLOAT( sPos.x + 2 ),
                FLOAT( sPos.y - sTextSize.cy - 2 ),
                strComment,
                D3DCOLOR_XRGB( 64, 64, 192 ),
                pRI->DefaultFontName(),
                pRI->DefaultFontSize(),
                pRI->DefaultFontFlags() );
	    }

        ic::Int2 sCircleCenter;
        sCircleCenter.x = sPos.x + INT( ic::MinShapeSize * 0.5f );
        sCircleCenter.y = sPos.y + INT( ic::MinShapeSize * 0.5f );

        sDrawSize.x = sCircleCenter.x - sPos.x;
        sDrawSize.y = sCircleCenter.y - sPos.y;

	    ic::CLinkedObjDrawUtils::DrawNGon(
            pRI,
            sCircleCenter,
            nBorderColor,
            CircleSides,
            INT( ic::MinShapeSize * 0.5f ) );

        ic::CLinkedObjDrawUtils::DrawNGon(
            pRI,
            sCircleCenter,
            D3DCOLOR_XRGB( 140, 140, 140 ),
            CircleSides,
            INT( ic::MinShapeSize * 0.5f ) - 1 );

        DrawExtraInfo( pRI, sCircleCenter );

        TSTRING strValue = GetValueText();
        if( !strValue.empty() )
        {
            SIZE sTextSize = { 0, 0 };
            pRI->GetTextExtent(
                sTextSize,
                strValue,
                pRI->DefaultFontName(),
                pRI->DefaultFontSize(),
                pRI->DefaultFontZoom() );

            if( !bSelected &&
                strValue.size() > 8 &&
                sTextSize.cx > ic::MinShapeSize &&
                !IsA( CSeqVar_Named::Rtti() ) )
            {
                CString strText = strValue.c_str();
                strValue = strText.Left( 4 ) + _T( ".." ) + strText.Right( 4 );

                pRI->GetTextExtent(
                    sTextSize,
                    strValue,
                    pRI->DefaultFontName(),
                    pRI->DefaultFontSize(),
                    pRI->DefaultFontZoom() );
            }

            pRI->DrawText(
                FLOAT( sCircleCenter.x - INT( sTextSize.cx * 0.5f ) ),
                FLOAT( sCircleCenter.y - INT( sTextSize.cy * 0.5f ) ),
                strValue,
                D3DCOLOR_XRGB( 255, 255, 255 ),
                pRI->DefaultFontName(),
                pRI->DefaultFontSize(),
                pRI->DefaultFontZoom() );
        }

	    if( !strVarName.empty() )
	    {
            SIZE sTextSize = { 0, 0 };
            pRI->GetTextExtent(
                sTextSize,
                strVarName,
                pRI->DefaultFontName(),
                pRI->DefaultFontSize(),
                pRI->DefaultFontZoom() );

            pRI->DrawText(
                FLOAT( sCircleCenter.x - INT( sTextSize.cx * 0.5f ) ),
                FLOAT( sPos.y + ic::MinShapeSize + 2 ),
                strVarName,
                D3DCOLOR_XRGB( 255, 64, 64 ),
                pRI->DefaultFontName(),
                pRI->DefaultFontSize(),
                pRI->DefaultFontZoom() );
	    }

	    if( pRI->IsHitTesting() )
            pRI->SetHitProxy( NULL );
    }

    ic::Int4 CSequenceVariable::GetBoundingBox() const
    {
        return ic::Int4(
            sPos.x,
            sPos.y,
            sPos.x + ic::MinShapeSize,
            sPos.y + ic::MinShapeSize );
    }

    ic::Int2 CSequenceVariable::GetConnectionLocation()
    {
        return ic::Int2( sPos.x + sDrawSize.x, sPos.y + sDrawSize.y );
    }

    VOID CSequenceVariable::OnChangedVarName()
    {
        CSequence* pRoot = GetRootSequence();
        __gassert_( pRoot );

        pRoot->UpdateNamedVarStatus();
    }

    ic::CProperty* CSequenceVariable::GetProperty()
    {
        return VProperty( _T( "Value" ) );
    }

    CSeqVar_Bool::CSeqVar_Bool()
        : bValue( FALSE )
    {
        strName = _T( "Bool" );
        nColor = D3DCOLOR_XRGB( 255, 0, 0 );
    }

    VOID CSeqVar_Bool::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyBool* pProperty = ic::CPropertyFactory::New< ic::CPropertyBool >();
            pProperty->SetPtr( &bValue );
            InsertProperty( _T( "Value" ), pProperty );
        }
    }

    TSTRING CSeqVar_Bool::GetValueText()
    {
        CString strValue;
        strValue.Format( _T( "%s" ), bValue ? _T( "True" ) : _T( "False" ) );

        return strValue.GetString();
    }

    CSeqVar_Double::CSeqVar_Double()
        : nValue( 0.0f )
    {
        strName = _T( "Double" );
        nColor = D3DCOLOR_XRGB( 0, 0, 255 );
    }

    VOID CSeqVar_Double::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyDouble* pProperty = ic::CPropertyFactory::New< ic::CPropertyDouble >();
            pProperty->SetPtr( &nValue );
            InsertProperty( _T( "Value" ), pProperty );
        }
    }

    TSTRING CSeqVar_Double::GetValueText()
    {
        CString strValue;
        strValue.Format( _T( "%.3f" ), nValue );

        return strValue.GetString();
    }

    CSeqVar_String::CSeqVar_String()
    {
        strName = _T( "String" );
        nColor = D3DCOLOR_XRGB( 0, 255, 0 );
    }

    VOID CSeqVar_String::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyString* pProperty = ic::CPropertyFactory::New< ic::CPropertyString >();
            pProperty->SetPtr( &strValue );
            InsertProperty( _T( "Value" ), pProperty );
        }
    }

    CSeqVar_Vector3::CSeqVar_Vector3()
        : vValue( 0.0f, 0.0f, 0.0f )
    {
        strName = _T( "Vector3" );
        nColor = D3DCOLOR_XRGB( 127, 127, 0 );
    }

    VOID CSeqVar_Vector3::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyVector3* pProperty = ic::CPropertyFactory::New< ic::CPropertyVector3 >();
            pProperty->SetPtr( &vValue );
            InsertProperty( _T( "Value" ), pProperty );
        }
    }

    TSTRING CSeqVar_Vector3::GetValueText()
    {
        CString strValue;
        strValue.Format( _T( "X=%.3f Y=%.3f Z=%.3f" ), vValue.x, vValue.y, vValue.z );

        return strValue.GetString();
    }

    CSeqVar_Object::CSeqVar_Object()
        : pValue( NULL )
    {
        strName = _T( "Object" );
        nColor = D3DCOLOR_XRGB( 255, 0, 255 );
    }

    VOID CSeqVar_Object::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyObject* pProperty = ic::CPropertyFactory::New< ic::CPropertyObject >();
            pProperty->SetPtr( &pValue );
            InsertProperty( _T( "Value" ), pProperty );
        }

        if( pValue )
            pValue->VRegistProperty();
    }

    TSTRING CSeqVar_Object::GetValueText()
    {
        CString strValue;
        strValue.Format( _T( "%s" ), pValue ? pValue->GetClassName().c_str() : _T( "???" ) );

        return strValue.GetString();
    }

    CSeqVar_Player::CSeqVar_Player()
    {
        strName = _T( "Player" );

        // 임시
        pValue = this;
    }

    VOID CSeqVar_Player::PostNew()
    {
        // 임시
        size_t nSize = ic::CRttiMgr::Count();
        for( size_t i=0; i<nSize; ++i )
        {
            ic::CBaseObject* pStatic = ic::CRttiMgr::FindStatic( i );
            if( !pStatic->IsA( ic::IPlayer::Rtti() ) &&
                pStatic->IsChildOf( ic::IPlayer::Rtti() ) )
            {
                pValue = pStatic;
            }
        }
    }

    VOID CSeqVar_Player::RegistProperty()
    {
        Super::RegistProperty();

        // 임시
        {
            static INT s_nTest = 7;

            ic::CPropertyInt* pProperty = ic::CPropertyFactory::New< ic::CPropertyInt >();
            pProperty->SetPtr( &s_nTest );
            InsertProperty( _T( "Variable" ), pProperty );
        }
        {
            ic::CPropertyFnArgs* pProperty = ic::CPropertyFactory::New< ic::CPropertyFnArgs >();
            pProperty->SetFunction( boost::bind( boost::mem_fn( &CSeqVar_Player::TestFunction ), this, _1 ) );
            pProperty->PushArgType( ic::CPropertyFnArgs::ArgType( ic::EBool, _T( "Bool" ) ) );
            pProperty->PushArgType( ic::CPropertyFnArgs::ArgType( ic::EInt, _T( "Int" ) ) );
            pProperty->PushArgType( ic::CPropertyFnArgs::ArgType( ic::EFloat, _T( "Float" ) ) );
            InsertProperty( _T( "Function" ), pProperty );
        }
    }

    TSTRING CSeqVar_Player::GetValueText()
    {
        CString strValue;
        strValue.Format( _T( "Player" ) );

        return strValue.GetString();
    }

    VOID CSeqVar_Player::TestFunction( ic::IPropertyValueVec& vecArgs )
    {
        // 임시
        {
            BOOL tBool = *vecArgs[ 0 ];
            INT tInt = *vecArgs[ 1 ];
            FLOAT tFloat = *vecArgs[ 2 ];

            *vecArgs[ 0 ] = BOOL( !tBool );
            *vecArgs[ 1 ] = INT( tInt + 1 );
            *vecArgs[ 2 ] = FLOAT( tFloat + 0.1f );
        }
    }

    CSeqVar_Named::CSeqVar_Named()
        : pExpected( NULL )
        , bStatusIsOk( FALSE )
        , pLinkVar( NULL )
    {
        strName = _T( "Named Variable" );
    }

    VOID CSeqVar_Named::RegistProperty()
    {
        Super::RegistProperty();

        {
            std::vector< ic::CBaseObject* > vecObject;
            for( size_t i=0; i<ic::CRttiMgr::Count(); ++i )
            {
                ic::CBaseObject* pStatic = ic::CRttiMgr::FindStatic( i );
                CSequenceVariable* pVariable = dynamic_cast< CSequenceVariable* >( pStatic );
                if( !pVariable )
                    continue;
                if( pVariable->IsA( CSequenceVariable::Rtti() ) )
                    continue;
                vecObject.push_back( pStatic );
            }
            std::sort( vecObject.begin(), vecObject.end(), ic::CompBaseObject() );

            ic::CPropertySelect* pProperty = ic::CPropertyFactory::New< ic::CPropertySelect >();
            pProperty->SetPtr( reinterpret_cast< DWORD* >( &pExpected ) );
            BOOST_FOREACH( ic::CBaseObject* pStatic, vecObject )
            {
                pProperty->PushData( 
                    ic::CPropertySelect::DataPair( pStatic->GetIdentity(), pStatic->GetClassName() ) );
            }
            pProperty->SetCall( boost::bind( boost::mem_fn( &CSeqVar_Named::OnChangedClass ), this ) );
            InsertProperty( _T( "Class" ), pProperty );
        }
        {
            ic::CPropertyString* pProperty = ic::CPropertyFactory::New< ic::CPropertyString >();
            pProperty->SetPtr( &strFindVarName );
            pProperty->SetCall( boost::bind( boost::mem_fn( &CSeqVar_Named::OnChangedFindVarName ), this ) );
            InsertProperty( _T( "Find Name" ), pProperty );
        }
    }

    VOID CSeqVar_Named::PostLoad()
    {
        OnChangedFindVarName();
    }

    VOID CSeqVar_Named::OnLinkFrom( CSequenceObject* pObject, INT nConnIndex )
    {
        if( bStatusIsOk )
            return;

        CSequenceOp* pOp = dynamic_cast< CSequenceOp* >( pObject );
        if( !pOp )
            return;

        SSeqVarLink& sLink = pOp->vecVariableLinks[ nConnIndex ];

        SeqVariableVecIter fi = std::find(
            sLink.vecLinks.begin(),
            sLink.vecLinks.end(),
            this );

        if( fi != sLink.vecLinks.end() )
        {
            pExpected = sLink.pExpected;

            OnChangedClass();
        }
    }

    VOID CSeqVar_Named::DrawExtraInfo( ic::IRenderInterface* pRI, const ic::Int2& sCenter )
    {
        DrawStatusMark( pRI, sCenter, bStatusIsOk );
    }

    VOID CSeqVar_Named::OnChangedClass()
    {
        OnChangedFindVarName();

        CSequence* pRoot = GetRootSequence();
        __gassert_( pRoot );

        pRoot->UpdateNamedVarStatus();
    }

    VOID CSeqVar_Named::OnChangedFindVarName()
    {
        bStatusIsOk = FALSE;
        pLinkVar = NULL;
        nColor = D3DCOLOR_XRGB( 0, 0, 0 );

        if( !pExpected )
            return;

        CSequenceVariable* pVar = dynamic_cast< CSequenceVariable* >( pExpected );
        nColor = pVar->nColor;

        if( strFindVarName.empty() )
            return;

        CSequence* pRoot = GetRootSequence();
        __gassert_( pRoot );

        SeqVariableVec vecVars;
        pRoot->FindNamedVariable( strFindVarName, FALSE, vecVars );

        if( vecVars.size() != 1 )
            return;

        if( !vecVars[ 0 ]->IsChildOf( pExpected ) )
            return;

        bStatusIsOk = TRUE;
        pLinkVar = vecVars[ 0 ];
        nColor = pLinkVar->nColor;
    }

    ic::CProperty* CSeqVar_Named::GetProperty()
    {
        return ( pLinkVar ) ? pLinkVar->GetProperty() : NULL;
    }

    TSTRING CSeqVar_Named::GetValueText()
    {
        if( strFindVarName.empty() )
            return _T( "???" );

        return strFindVarName;
    }
}