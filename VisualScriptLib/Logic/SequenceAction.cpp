#include "stdafx.h"

#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>
#include <boost/scope_exit.hpp>

#include "../../InternalCommonLib/Interface/DxSoundInterface.h"
#include "../../InternalCommonLib/Interface/RenderInterface.h"

#include "./SequenceEvent.h"
#include "./SequenceVariable.h"
#include "./Sequence.h"

#include "./SequenceAction.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace vs
{
    CSequenceAction::CSequenceAction()
    {
        strName = _T( "Action" );
        nColor = D3DCOLOR_XRGB( 255, 0, 255 );
    }

    CSeqAct_FinishSequence::CSeqAct_FinishSequence()
        : strLabel( _T( "Out" ) )
    {
        strName = _T( "Finish Sequence" );
    }

    VOID CSeqAct_FinishSequence::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_FinishSequence::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyString* pProperty = ic::CPropertyFactory::New< ic::CPropertyString >();
            pProperty->SetPtr( &strLabel );
            pProperty->SetCall( boost::bind( boost::mem_fn( &CSeqAct_FinishSequence::OnChangedLabel ), this ) );
            InsertProperty( _T( "Label" ), pProperty );
        }
    }

    TSTRING CSeqAct_FinishSequence::GetObjectName()
    {
        CString strName_;
        strName_.Format( _T( "%s \"%s\"" ), strName.c_str(), strLabel.c_str() );

        return strName_.GetString();
    }

    VOID CSeqAct_FinishSequence::OnCreated()
    {
        OnChangedLabel();
    }

    VOID CSeqAct_FinishSequence::Activated( size_t nInput )
    {
        CSequence* pSequence = dynamic_cast< CSequence* >( GetParent() );
        if( !pSequence )
            return;

        INT nIndex = 0;

        BOOST_FOREACH( SSeqOpOutputLink& sLink, pSequence->vecOutputLinks )
        {
            if( sLink.strLinkAction.compare( strName ) == 0 )
            {
                pSequence->InsertOutputImpulse( nIndex );
                break;
            }

            ++nIndex;
        }
    }

    VOID CSeqAct_FinishSequence::OnChangedLabel()
    {
        CSequence* pSequence = dynamic_cast< CSequence* >( GetParent() );
        if( pSequence )
            pSequence->UpdateConnectors();
    }

    CSeqAct_ActivateRemoteEvent::CSeqAct_ActivateRemoteEvent()
        : strLabel( _T( "Untitled" ) )
        , bStatusIsOk( FALSE )
    {
        strName = _T( "Activate Remote Event" );
    }

    VOID CSeqAct_ActivateRemoteEvent::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_ActivateRemoteEvent::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyString* pProperty = ic::CPropertyFactory::New< ic::CPropertyString >();
            pProperty->SetPtr( &strLabel );
            pProperty->SetCall( boost::bind( boost::mem_fn( &CSeqAct_ActivateRemoteEvent::OnChangedLabel ), this ) );
            InsertProperty( _T( "Label" ), pProperty );
        }
    }

    TSTRING CSeqAct_ActivateRemoteEvent::GetObjectName()
    {
        CString strName_;
        strName_.Format( _T( "%s \"%s\"" ), strName.c_str(), strLabel.c_str() );

        return strName_.GetString();
    }

    VOID CSeqAct_ActivateRemoteEvent::PostLoad()
    {
        OnChangedLabel();
    }

    VOID CSeqAct_ActivateRemoteEvent::OnCreated()
    {
        OnChangedLabel();

        CSequence* pRoot = GetRootSequence();
        __gassert_( pRoot );

        pRoot->UpdateRemoteEventStatus();
        pRoot->UpdateActivateRemoteEventStatus();
    }

    VOID CSeqAct_ActivateRemoteEvent::Activated( size_t nInput )
    {
        if( strLabel.empty() )
            return;

        CSequence* pRoot = GetRootSequence();
        __gassert_( pRoot );

        SequenceObjectVec vecObjects;
        pRoot->FindRemoteEvent( strLabel, vecObjects );

        BOOST_FOREACH( CSequenceObject* pObject, vecObjects )
        {
            CSeqEvent_RemoteEvent* pEvent = dynamic_cast< CSeqEvent_RemoteEvent* >( pObject );
            if( pEvent )
                pEvent->InsertInputImpulse();
        }

        DeActivated();
    }

    VOID CSeqAct_ActivateRemoteEvent::DrawExtraInfo( ic::IRenderInterface* pRI, const ic::Int2& sCenter )
    {
        DrawStatusMark( pRI, sCenter, bStatusIsOk );
    }

    VOID CSeqAct_ActivateRemoteEvent::OnChangedLabel()
    {
        bStatusIsOk = FALSE;

        if( strLabel.empty() )
            return;

        CSequence* pRoot = GetRootSequence();
        __gassert_( pRoot );

        SequenceObjectVec vecObjects;
        pRoot->FindRemoteEvent( strLabel, vecObjects );

        if( !vecObjects.empty() )
            bStatusIsOk = TRUE;
    }

    CSeqAct_SetBool::CSeqAct_SetBool()
        : bDefaultValue( FALSE )
    {
        strName = _T( "Set Bool" );
    }

    VOID CSeqAct_SetBool::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "Value" );
            sPush.pExpected = CSeqVar_Bool::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Target" );
            sPush.bWriteable = TRUE;
            sPush.pExpected = CSeqVar_Bool::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_SetBool::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyBool* pProperty = ic::CPropertyFactory::New< ic::CPropertyBool >();
            pProperty->SetPtr( &bDefaultValue );
            InsertProperty( _T( "Default Value" ), pProperty );
        }
    }

    VOID CSeqAct_SetBool::Activated( size_t nInput )
    {
        BOOL bValue = TRUE;

        std::vector< ic::CPropertyBool* > vecVars;
        FindVariable( vecVars, _T( "Value" ) );

        size_t nSize = vecVars.size();
        if( nSize > 0 )
        {
            for( size_t i=0; i<nSize; ++i )
                bValue = bValue && vecVars[ i ]->GetValue();
        }
        else
        {
            bValue = bDefaultValue;
        }

        vecVars.clear();
        FindVariable( vecVars, _T( "Target" ) );

        nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            vecVars[ i ]->SetValue( bValue );

        DeActivated();
    }

    CSeqAct_SetDouble::CSeqAct_SetDouble()
        : nDefaultValue( 0.0f )
    {
        strName = _T( "Set Double" );
    }

    VOID CSeqAct_SetDouble::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "Value" );
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Target" );
            sPush.bWriteable = TRUE;
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_SetDouble::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyDouble* pProperty = ic::CPropertyFactory::New< ic::CPropertyDouble >();
            pProperty->SetPtr( &nDefaultValue );
            InsertProperty( _T( "Default Value" ), pProperty );
        }
    }

    VOID CSeqAct_SetDouble::Activated( size_t nInput )
    {
        DOUBLE nValue = 0.0f;

        std::vector< ic::CPropertyDouble* > vecVars;
        FindVariable( vecVars, _T( "Value" ) );

        size_t nSize = vecVars.size();
        if( nSize > 0 )
        {
            for( size_t i=0; i<nSize; ++i )
                nValue += vecVars[ i ]->GetValue();
        }
        else
        {
            nValue = nDefaultValue;
        }

        vecVars.clear();
        FindVariable( vecVars, _T( "Target" ) );

        nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            vecVars[ i ]->SetValue( nValue );

        DeActivated();
    }

    CSeqAct_SetString::CSeqAct_SetString()
    {
        strName = _T( "Set String" );
    }

    VOID CSeqAct_SetString::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "Value" );
            sPush.pExpected = CSeqVar_String::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Target" );
            sPush.bWriteable = TRUE;
            sPush.pExpected = CSeqVar_String::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_SetString::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyString* pProperty = ic::CPropertyFactory::New< ic::CPropertyString >();
            pProperty->SetPtr( &strDefaultValue );
            InsertProperty( _T( "Default Value" ), pProperty );
        }
    }

    VOID CSeqAct_SetString::Activated( size_t nInput )
    {
        TSTRING strValue;

        std::vector< ic::CPropertyString* > vecVars;
        FindVariable( vecVars, _T( "Value" ) );

        size_t nSize = vecVars.size();
        if( nSize > 0 )
        {
            for( size_t i=0; i<nSize; ++i )
                strValue.append( vecVars[ i ]->GetValue() );
        }
        else
        {
            strValue = strDefaultValue;
        }

        vecVars.clear();
        FindVariable( vecVars, _T( "Target" ) );

        nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            vecVars[ i ]->SetValue( strValue );

        DeActivated();
    }

    CSeqAct_SetVector3::CSeqAct_SetVector3()
    {
        strName = _T( "Set Vector3" );
    }

    VOID CSeqAct_SetVector3::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "Value" );
            sPush.pExpected = CSeqVar_Vector3::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Target" );
            sPush.bWriteable = TRUE;
            sPush.pExpected = CSeqVar_Vector3::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_SetVector3::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyVector3* pProperty = ic::CPropertyFactory::New< ic::CPropertyVector3 >();
            pProperty->SetPtr( &vDefaultValue );
            InsertProperty( _T( "Default Value" ), pProperty );
        }
    }

    VOID CSeqAct_SetVector3::Activated( size_t nInput )
    {
        D3DXVECTOR3 vValue( 0.0f, 0.0f, 0.0f );

        std::vector< ic::CPropertyVector3* > vecVars;
        FindVariable( vecVars, _T( "Value" ) );

        size_t nSize = vecVars.size();
        if( nSize > 0 )
        {
            for( size_t i=0; i<nSize; ++i )
                vValue += vecVars[ i ]->GetValue();
        }
        else
        {
            vValue = vDefaultValue;
        }

        vecVars.clear();
        FindVariable( vecVars, _T( "Target" ) );

        nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            vecVars[ i ]->SetValue( vValue );

        DeActivated();
    }

    CSeqAct_SetVector3Components::CSeqAct_SetVector3Components()
    {
        strName = _T( "Set Vector3 Components" );
    }

    VOID CSeqAct_SetVector3Components::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "X" );
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Y" );
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Z" );
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Vector" );
            sPush.bWriteable = TRUE;
            sPush.pExpected = CSeqVar_Vector3::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_SetVector3Components::Activated( size_t nInput )
    {
        D3DXVECTOR3 vValue( 0.0f, 0.0f, 0.0f );

        std::vector< ic::CPropertyDouble* > vecVarsComponents;
        FindVariable( vecVarsComponents, _T( "X" ) );

        size_t nSize = vecVarsComponents.size();
        for( size_t i=0; i<nSize; ++i )
            vValue.x += static_cast< FLOAT >( vecVarsComponents[ i ]->GetValue() );

        vecVarsComponents.clear();
        FindVariable( vecVarsComponents, _T( "Y" ) );

        nSize = vecVarsComponents.size();
        for( size_t i=0; i<nSize; ++i )
            vValue.y += static_cast< FLOAT >( vecVarsComponents[ i ]->GetValue() );

        vecVarsComponents.clear();
        FindVariable( vecVarsComponents, _T( "Z" ) );

        nSize = vecVarsComponents.size();
        for( size_t i=0; i<nSize; ++i )
            vValue.z += static_cast< FLOAT >( vecVarsComponents[ i ]->GetValue() );

        std::vector< ic::CPropertyVector3* > vecVarsVector;
        FindVariable( vecVarsVector, _T( "Vector" ) );

        nSize = vecVarsVector.size();
        for( size_t i=0; i<nSize; ++i )
            vecVarsVector[ i ]->SetValue( vValue );

        DeActivated();
    }

    CSeqAct_GetVector3Components::CSeqAct_GetVector3Components()
    {
        strName = _T( "Get Vector3 Components" );
    }

    VOID CSeqAct_GetVector3Components::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "Vector" );
            sPush.pExpected = CSeqVar_Vector3::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "X" );
            sPush.bWriteable = TRUE;
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Y" );
            sPush.bWriteable = TRUE;
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Z" );
            sPush.bWriteable = TRUE;
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_GetVector3Components::Activated( size_t nInput )
    {
        D3DXVECTOR3 vValue( 0.0f, 0.0f, 0.0f );

        std::vector< ic::CPropertyVector3* > vecVarsVector;
        FindVariable( vecVarsVector, _T( "Vector" ) );

        size_t nSize = vecVarsVector.size();
        for( size_t i=0; i<nSize; ++i )
            vValue += vecVarsVector[ i ]->GetValue();

        std::vector< ic::CPropertyDouble* > vecVarsComponents;
        FindVariable( vecVarsComponents, _T( "X" ) );

        nSize = vecVarsComponents.size();
        for( size_t i=0; i<nSize; ++i )
            vecVarsComponents[ i ]->SetValue( static_cast< DOUBLE >( vValue.x ) );

        vecVarsComponents.clear();
        FindVariable( vecVarsComponents, _T( "Y" ) );

        nSize = vecVarsComponents.size();
        for( size_t i=0; i<nSize; ++i )
            vecVarsComponents[ i ]->SetValue( static_cast< DOUBLE >( vValue.y ) );

        vecVarsComponents.clear();
        FindVariable( vecVarsComponents, _T( "Z" ) );

        nSize = vecVarsComponents.size();
        for( size_t i=0; i<nSize; ++i )
            vecVarsComponents[ i ]->SetValue( static_cast< DOUBLE >( vValue.z ) );

        DeActivated();
    }

    CSeqAct_SetObject::CSeqAct_SetObject()
        : pDefaultObject( NULL )
    {
        strName = _T( "Set Object" );
    }

    VOID CSeqAct_SetObject::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "Value" );
            sPush.pExpected = ic::CBaseObject::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Target" );
            sPush.bWriteable = TRUE;
            sPush.pExpected = ic::CBaseObject::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_SetObject::Activated( size_t nInput )
    {
        ic::CBaseObject* pValue = NULL;

        std::vector< ic::CPropertyObject* > vecVars;
        FindVariable( vecVars, _T( "Value" ) );

        size_t nSize = vecVars.size();
        if( nSize > 0 )
        {
            for( size_t i=0; i<nSize; ++i )
                pValue = vecVars[ i ]->GetValue();
        }
        else
        {
            pValue = pDefaultObject;
        }

        vecVars.clear();
        FindVariable( vecVars, _T( "Target" ) );

        nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            vecVars[ i ]->SetValue( pValue );

        DeActivated();
    }

    CSeqAct_AddDouble::CSeqAct_AddDouble()
    {
        strName = _T( "Math - Add Double" );
    }

    VOID CSeqAct_AddDouble::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "A" );
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "B" );
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Target" );
            sPush.bWriteable = TRUE;
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_AddDouble::Activated( size_t nInput )
    {
        DOUBLE nValueA = 0.0f;
        DOUBLE nValueB = 0.0f;

        std::vector< ic::CPropertyDouble* > vecVars;
        FindVariable( vecVars, _T( "A" ) );

        size_t nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            nValueA += vecVars[ i ]->GetValue();

        vecVars.clear();
        FindVariable( vecVars, _T( "B" ) );

        nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            nValueB += vecVars[ i ]->GetValue();

        nValueA += nValueB;

        vecVars.clear();
        FindVariable( vecVars, _T( "Target" ) );

        nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            vecVars[ i ]->SetValue( nValueA );

        DeActivated();
    }

    CSeqAct_SubtractDouble::CSeqAct_SubtractDouble()
    {
        strName = _T( "Math - Subtract Double" );
    }

    VOID CSeqAct_SubtractDouble::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "A" );
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "B" );
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Target" );
            sPush.bWriteable = TRUE;
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_SubtractDouble::Activated( size_t nInput )
    {
        DOUBLE nValueA = 0.0f;
        DOUBLE nValueB = 0.0f;

        std::vector< ic::CPropertyDouble* > vecVars;
        FindVariable( vecVars, _T( "A" ) );

        size_t nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            nValueA += vecVars[ i ]->GetValue();

        vecVars.clear();
        FindVariable( vecVars, _T( "B" ) );

        nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            nValueB += vecVars[ i ]->GetValue();

        nValueA -= nValueB;

        vecVars.clear();
        FindVariable( vecVars, _T( "Target" ) );

        nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            vecVars[ i ]->SetValue( nValueA );

        DeActivated();
    }

    CSeqAct_MultiplyDouble::CSeqAct_MultiplyDouble()
    {
        strName = _T( "Math - Multiply Double" );
    }

    VOID CSeqAct_MultiplyDouble::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "A" );
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "B" );
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Target" );
            sPush.bWriteable = TRUE;
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_MultiplyDouble::Activated( size_t nInput )
    {
        DOUBLE nValueA = 0.0f;
        DOUBLE nValueB = 0.0f;

        std::vector< ic::CPropertyDouble* > vecVars;
        FindVariable( vecVars, _T( "A" ) );

        size_t nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            nValueA += vecVars[ i ]->GetValue();

        vecVars.clear();
        FindVariable( vecVars, _T( "B" ) );

        nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            nValueB += vecVars[ i ]->GetValue();

        nValueA *= nValueB;

        vecVars.clear();
        FindVariable( vecVars, _T( "Target" ) );

        nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            vecVars[ i ]->SetValue( nValueA );

        DeActivated();
    }

    CSeqAct_DivideDouble::CSeqAct_DivideDouble()
    {
        strName = _T( "Math - Divide Double" );
    }

    VOID CSeqAct_DivideDouble::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "A" );
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "B" );
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Target" );
            sPush.bWriteable = TRUE;
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_DivideDouble::Activated( size_t nInput )
    {
        DOUBLE nValueA = 0.0f;
        DOUBLE nValueB = 0.0f;

        std::vector< ic::CPropertyDouble* > vecVars;
        FindVariable( vecVars, _T( "A" ) );

        size_t nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            nValueA += vecVars[ i ]->GetValue();

        vecVars.clear();
        FindVariable( vecVars, _T( "B" ) );

        nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            nValueB += vecVars[ i ]->GetValue();

        nValueA = nValueB != 0.0f ? nValueA / nValueB : 0.0f;

        vecVars.clear();
        FindVariable( vecVars, _T( "Target" ) );

        nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            vecVars[ i ]->SetValue( nValueA );

        DeActivated();
    }

    CSeqAct_CompareBool::CSeqAct_CompareBool()
    {
        strName = _T( "Compare Bool" );
    }

    VOID CSeqAct_CompareBool::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "True" );
            vecOutputLinks.push_back( sPush );

            sPush = SSeqOpOutputLink();
            sPush.strLinkDesc = _T( "False" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "Bool" );
            sPush.pExpected = CSeqVar_Bool::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_CompareBool::Activated( size_t nInput )
    {
        BOOL bResult = TRUE;

        std::vector< ic::CPropertyBool* > vecVars;
        FindVariable( vecVars, _T( "Bool" ) );

        size_t nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            bResult = bResult && vecVars[ i ]->GetValue();

        InsertOutputImpulse( bResult ? 0 : 1 );
    }

    CSeqAct_CompareDouble::CSeqAct_CompareDouble()
        : nDefaultA( 0.0f )
        , nDefaultB( 0.0f )
    {
        strName = _T( "Compare Double" );
    }

    VOID CSeqAct_CompareDouble::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "A <= B" );
            vecOutputLinks.push_back( sPush );

            sPush = SSeqOpOutputLink();
            sPush.strLinkDesc = _T( "A > B" );
            vecOutputLinks.push_back( sPush );

            sPush = SSeqOpOutputLink();
            sPush.strLinkDesc = _T( "A == B" );
            vecOutputLinks.push_back( sPush );

            sPush = SSeqOpOutputLink();
            sPush.strLinkDesc = _T( "A != B" );
            vecOutputLinks.push_back( sPush );

            sPush = SSeqOpOutputLink();
            sPush.strLinkDesc = _T( "A < B" );
            vecOutputLinks.push_back( sPush );

            sPush = SSeqOpOutputLink();
            sPush.strLinkDesc = _T( "A >= B" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "A" );
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "B" );
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_CompareDouble::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyDouble* pProperty = ic::CPropertyFactory::New< ic::CPropertyDouble >();
            pProperty->SetPtr( &nDefaultA );
            InsertProperty( _T( "Value A" ), pProperty );
        }
        {
            ic::CPropertyDouble* pProperty = ic::CPropertyFactory::New< ic::CPropertyDouble >();
            pProperty->SetPtr( &nDefaultB );
            InsertProperty( _T( "Value B" ), pProperty );
        }
    }

    VOID CSeqAct_CompareDouble::Activated( size_t nInput )
    {
	    DOUBLE nValueA = 0.0f;
        DOUBLE nValueB = 0.0f;

        std::vector< ic::CPropertyDouble* > vecVars;
        FindVariable( vecVars, _T( "A" ) );

        size_t nSize = vecVars.size();
        if( nSize > 0 )
        {
            for( size_t i=0; i<nSize; ++i )
                nValueA += vecVars[ i ]->GetValue();
        }
        else
        {
            nValueA = nDefaultA;
        }

        vecVars.clear();
        FindVariable( vecVars, _T( "B" ) );

        nSize = vecVars.size();
        if( nSize > 0 )
        {
            for( size_t i=0; i<nSize; ++i )
                nValueB += vecVars[ i ]->GetValue();
        }
        else
        {
            nValueB = nDefaultB;
        }

        if( nValueA <= nValueB )
            InsertOutputImpulse( 0 );
        if( nValueA > nValueB )
            InsertOutputImpulse( 1 );
        if( nValueA == nValueB )
            InsertOutputImpulse( 2 );
        if( nValueA != nValueB )
            InsertOutputImpulse( 3 );
        if( nValueA < nValueB )
            InsertOutputImpulse( 4 );
        if( nValueA >= nValueB )
            InsertOutputImpulse( 5 );
    }

    CSeqAct_CompareString::CSeqAct_CompareString()
    {
        strName = _T( "Compare String" );
    }

    VOID CSeqAct_CompareString::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "A == B" );
            vecOutputLinks.push_back( sPush );

            sPush = SSeqOpOutputLink();
            sPush.strLinkDesc = _T( "A != B" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "A" );
            sPush.pExpected = CSeqVar_String::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "B" );
            sPush.pExpected = CSeqVar_String::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_CompareString::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyString* pProperty = ic::CPropertyFactory::New< ic::CPropertyString >();
            pProperty->SetPtr( &strDefaultA );
            InsertProperty( _T( "Value A" ), pProperty );
        }
        {
            ic::CPropertyString* pProperty = ic::CPropertyFactory::New< ic::CPropertyString >();
            pProperty->SetPtr( &strDefaultB );
            InsertProperty( _T( "Value B" ), pProperty );
        }
    }

    VOID CSeqAct_CompareString::Activated( size_t nInput )
    {
        TSTRING strValueA;
        TSTRING strValueB;

        std::vector< ic::CPropertyString* > vecVars;
        FindVariable( vecVars, _T( "A" ) );

        size_t nSize = vecVars.size();
        if( nSize > 0 )
        {
            for( size_t i=0; i<nSize; ++i )
                strValueA.append( vecVars[ i ]->GetValue() );
        }
        else
        {
            strValueA = strDefaultA;
        }

        vecVars.clear();
        FindVariable( vecVars, _T( "B" ) );

        nSize = vecVars.size();
        if( nSize > 0 )
        {
            for( size_t i=0; i<nSize; ++i )
                strValueB.append( vecVars[ i ]->GetValue() );
        }
        else
        {
            strValueB = strDefaultB;
        }

        InsertOutputImpulse( strValueA.compare( strValueB ) == 0 ? 0 : 1 );
    }

    CSeqAct_CompareVector3::CSeqAct_CompareVector3()
        : vDefaultA( 0.0f, 0.0f, 0.0f )
        , vDefaultB( 0.0f, 0.0f, 0.0f )
    {
        strName = _T( "Compare Vector3" );
    }

    VOID CSeqAct_CompareVector3::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "A == B" );
            vecOutputLinks.push_back( sPush );

            sPush = SSeqOpOutputLink();
            sPush.strLinkDesc = _T( "A != B" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "A" );
            sPush.pExpected = CSeqVar_Vector3::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "B" );
            sPush.pExpected = CSeqVar_Vector3::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_CompareVector3::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyVector3* pProperty = ic::CPropertyFactory::New< ic::CPropertyVector3 >();
            pProperty->SetPtr( &vDefaultA );
            InsertProperty( _T( "Value A" ), pProperty );
        }
        {
            ic::CPropertyVector3* pProperty = ic::CPropertyFactory::New< ic::CPropertyVector3 >();
            pProperty->SetPtr( &vDefaultB );
            InsertProperty( _T( "Value B" ), pProperty );
        }
    }

    VOID CSeqAct_CompareVector3::Activated( size_t nInput )
    {
        D3DXVECTOR3 vValueA( 0.0f, 0.0f, 0.0f );
        D3DXVECTOR3 vValueB( 0.0f, 0.0f, 0.0f );

        std::vector< ic::CPropertyVector3* > vecVars;
        FindVariable( vecVars, _T( "A" ) );

        size_t nSize = vecVars.size();
        if( nSize > 0 )
        {
            for( size_t i=0; i<nSize; ++i )
                vValueA += vecVars[ i ]->GetValue();
        }
        else
        {
            vValueA = vDefaultA;
        }

        vecVars.clear();
        FindVariable( vecVars, _T( "B" ) );

        nSize = vecVars.size();
        if( nSize > 0 )
        {
            for( size_t i=0; i<nSize; ++i )
                vValueB += vecVars[ i ]->GetValue();
        }
        else
        {
            vValueB = vDefaultB;
        }

        InsertOutputImpulse( vValueA == vValueB ? 0 : 1 );
    }

    CSeqAct_CompareObject::CSeqAct_CompareObject()
    {
        strName = _T( "Compare Object" );
    }

    VOID CSeqAct_CompareObject::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "A == B" );
            vecOutputLinks.push_back( sPush );

            sPush = SSeqOpOutputLink();
            sPush.strLinkDesc = _T( "A != B" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "A" );
            sPush.pExpected = ic::CBaseObject::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "B" );
            sPush.pExpected = ic::CBaseObject::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_CompareObject::Activated( size_t nInput )
    {
        ic::CBaseObject* pValueA = NULL;
        ic::CBaseObject* pValueB = NULL;

        std::vector< ic::CPropertyObject* > vecVars;
        FindVariable( vecVars, _T( "A" ) );

        size_t nSize = vecVars.size();
        if( nSize > 0 )
        {
            for( size_t i=0; i<nSize; ++i )
                pValueA = vecVars[ i ]->GetValue();
        }

        vecVars.clear();
        FindVariable( vecVars, _T( "B" ) );

        nSize = vecVars.size();
        if( nSize > 0 )
        {
            for( size_t i=0; i<nSize; ++i )
                pValueB = vecVars[ i ]->GetValue();
        }

        InsertOutputImpulse( pValueA == pValueB ? 0 : 1 );
    }

    CSeqAct_Toggle::CSeqAct_Toggle()
    {
        strName = _T( "Toggle" );
    }

    VOID CSeqAct_Toggle::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "Turn On" );
            vecInputLinks.push_back( sPush );

            sPush = SSeqOpInputLink();
            sPush.strLinkDesc = _T( "Turn Off" );
            vecInputLinks.push_back( sPush );

            sPush = SSeqOpInputLink();
            sPush.strLinkDesc = _T( "Toggle" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "Bool" );
            sPush.pExpected = CSeqVar_Bool::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_Toggle::Activated( size_t nInput )
    {
        std::vector< ic::CPropertyBool* > vecVars;
        FindVariable( vecVars, _T( "Bool" ) );

        size_t nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
        {
            if( nInput == 0 )
                vecVars[ i ]->SetValue( TRUE );
            else if( nInput == 1 )
                vecVars[ i ]->SetValue( FALSE );
            else if( nInput == 2 )
                vecVars[ i ]->SetValue( !vecVars[ i ]->GetValue() );
        }

        DeActivated();
    }

    CSeqAct_Switch::CSeqAct_Switch()
        : nLinkCount( 1 )
        , nIncrementAmount( 1 )
    {
        strName = _T( "Switch" );
    }

    VOID CSeqAct_Switch::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Link 1" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "Index" );
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_Switch::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyInt* pProperty = ic::CPropertyFactory::New< ic::CPropertyInt >();
            pProperty->SetPtr( &nLinkCount );
            pProperty->SetCall( boost::bind( boost::mem_fn( &CSeqAct_Switch::OnChangedLinkCount ), this ) );
            InsertProperty( _T( "Link Count" ), pProperty );
        }
        {
            ic::CPropertyInt* pProperty = ic::CPropertyFactory::New< ic::CPropertyInt >();
            pProperty->SetPtr( &nIncrementAmount );
            InsertProperty( _T( "Increment Amount" ), pProperty );
        }
    }

    VOID CSeqAct_Switch::Activated( size_t nInput )
    {
        std::vector< ic::CPropertyDouble* > vecVars;
        FindVariable( vecVars, _T( "Index" ) );

        size_t nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
        {
            size_t nActiveIndex = static_cast< size_t >( vecVars[ i ]->GetValue() );
            InsertOutputImpulse( nActiveIndex - 1 );

            if( nActiveIndex < vecOutputLinks.size() )
                vecVars[ i ]->SetValue( static_cast< DOUBLE >( nActiveIndex + nIncrementAmount ) );
            else
                vecVars[ i ]->SetValue( 1.0f );
        }
    }

    VOID CSeqAct_Switch::OnChangedLinkCount()
    {
        if( nLinkCount <= 0 )
            nLinkCount = 1;

        size_t nLinkCountSizeT = static_cast< size_t >( nLinkCount );

        if( vecOutputLinks.size() < nLinkCountSizeT )
        {
            while( vecOutputLinks.size() < nLinkCountSizeT )
            {
                CString strDesc;
                strDesc.Format( _T( "Link %d" ), vecOutputLinks.size() + 1 );

                SSeqOpOutputLink sPush;
                sPush.strLinkDesc = strDesc.GetString();
                vecOutputLinks.push_back( sPush );
            }
        }
        else if( vecOutputLinks.size() > nLinkCountSizeT )
        {
            while( vecOutputLinks.size() > nLinkCountSizeT )
                vecOutputLinks.pop_back();
        }
    }

    CSeqAct_RandomSwitch::CSeqAct_RandomSwitch()
    {
        strName = _T( "Switch - Random" );
    }

    VOID CSeqAct_RandomSwitch::PostNew()
    {
        Super::PostNew();

        // Variable
        {
            vecVariableLinks.clear();

            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "Active Link" );
            sPush.bWriteable = TRUE;
            sPush.pExpected = CSeqVar_Double::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_RandomSwitch::Activated( size_t nInput )
    {
        INT nOutIndex = rand() % (INT)vecOutputLinks.size();
        InsertOutputImpulse( static_cast< size_t >( nOutIndex ) );

        std::vector< ic::CPropertyDouble* > vecVars;
        FindVariable( vecVars, _T( "Active Link" ) );

        size_t nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            vecVars[ i ]->SetValue( static_cast< DOUBLE >( nOutIndex + 1 ) );
    }

    CSeqAct_PlaySound::CSeqAct_PlaySound()
    {
        strName = _T( "Play Sound" );
    }

    VOID CSeqAct_PlaySound::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "File" );
            sPush.pExpected = CSeqVar_String::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_PlaySound::Activated( size_t nInput )
    {
        std::vector< ic::CPropertyString* > vecVars;
        FindVariable( vecVars, _T( "File" ) );

        size_t nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            ic::IDxSound::GetInstance()->PlaySound( vecVars[ i ]->GetValue() );

        DeActivated();
    }

    CSeqAct_PlayEffect::CSeqAct_PlayEffect()
    {
        strName = _T( "Play Effect" );
    }

    VOID CSeqAct_PlayEffect::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "File" );
            sPush.pExpected = CSeqVar_String::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_PlayEffect::Activated( size_t nInput )
    {
        std::vector< ic::CPropertyString* > vecVars;
        FindVariable( vecVars, _T( "File" ) );

        size_t nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
            ;//ic::IDxEffectInterface::GetInstance()->PlayEffect( vecVars[ i ]->GetValue() );

        DeActivated();
    }

    CSeqAct_OutputString::CSeqAct_OutputString()
        : nTargets( 0 )
    {
        strName = _T( "Output String" );
    }

    VOID CSeqAct_OutputString::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "String" );
            sPush.pExpected = CSeqVar_String::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_OutputString::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyFlags* pProperty = ic::CPropertyFactory::New< ic::CPropertyFlags >();
            pProperty->SetPtr( &nTargets );
            pProperty->PushData( ic::CPropertyFlags::DataPair( EDebug, _T( "Debug" ) ) );
            pProperty->PushData( ic::CPropertyFlags::DataPair( EFile, _T( "File" ) ) );
            pProperty->PushData( ic::CPropertyFlags::DataPair( EChat, _T( "Chat" ) ) );
            pProperty->PushData( ic::CPropertyFlags::DataPair( ENotice, _T( "Notice" ) ) );
            InsertProperty( _T( "Target" ), pProperty );
        }
    }

    VOID CSeqAct_OutputString::Activated( size_t nInput )
    {
        std::vector< ic::CPropertyString* > vecVars;
        FindVariable( vecVars, _T( "String" ) );

        size_t nSize = vecVars.size();
        for( size_t i=0; i<nSize; ++i )
        {
            //ic::IOutputInterface::GetInstance()->Output( vecVars[ i ]->GetValue() );
            if( nTargets & EDebug )
            {
                TSTRING strString( vecVars[ i ]->GetValue() );
                strString.append( _T( "\n" ) );
                OutputDebugString( strString.c_str() );
            }
        }

        DeActivated();
    }

    CSeqAct_SetProperty::CSeqAct_SetProperty()
        : bStatusIsOk( FALSE )
    {
        strName = _T( "Set Property" );
    }

    VOID CSeqAct_SetProperty::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "Target" );
            sPush.nMaxVars = 1;
            sPush.pExpected = ic::CBaseObject::Static();
            sPush.bWriteable = TRUE;
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Value" );
            sPush.nMaxVars = 1;
            sPush.pExpected = CSequenceVariable::Static();
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_SetProperty::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyString* pProperty = ic::CPropertyFactory::New< ic::CPropertyString >();
            pProperty->SetPtr( &strKey );
            pProperty->SetCall( boost::bind( boost::mem_fn( &CSeqAct_SetProperty::OnChangedKey ), this ) );
            InsertProperty( _T( "Key" ), pProperty );
        }
    }

    TSTRING CSeqAct_SetProperty::GetObjectName()
    {
        CString strName_;
        strName_.Format( _T( "%s \"%s\"" ), strName.c_str(), strKey.c_str() );

        return strName_.GetString();
    }

    VOID CSeqAct_SetProperty::PostLoad()
    {
        OnChangedKey();
    }

    VOID CSeqAct_SetProperty::OnLinkVariable( CSequenceObject* pObject, INT nConnIndex )
    {
        if( nConnIndex == 0 )
            OnChangedKey();
    }

    VOID CSeqAct_SetProperty::OnPostBreakVariable( INT nConnIndex )
    {
        if( nConnIndex == 0 )
            OnChangedKey();
    }

    VOID CSeqAct_SetProperty::Activated( size_t nInput )
    {
        CSeqAct_SetProperty* pThis = this;
        BOOST_SCOPE_EXIT( ( &pThis ) )
        {
            pThis->DeActivated();
        }
        BOOST_SCOPE_EXIT_END

        std::vector< ic::CPropertyObject* > vecVarsTarget;
        FindVariable( vecVarsTarget, _T( "Target" ) );

        if( vecVarsTarget.empty() )
            return;

        ic::CBaseObject* pObject = vecVarsTarget[ 0 ]->GetValue();
        if( !pObject )
            return;

        ic::CProperty* pPropertyBase = pObject->VProperty( strKey );
        ic::IPropertyValue* pPropertyValue = dynamic_cast< ic::IPropertyValue* >( pPropertyBase );
        if( !pPropertyValue )
            return;

        ic::IPropertyValueVec vecVars;
        GetVariable( vecVars, 1 );

        if( vecVars.empty() )
            return;

        switch( vecVars[ 0 ]->VType() )
        {
        case ic::Ebool:
            {
                ic::CPropertybool* pProperty = dynamic_cast< ic::CPropertybool* >( vecVars[ 0 ] );
                __gassert_( pProperty );
                *pPropertyValue = pProperty->GetValue();
            }
            break;

        case ic::EBool:
            {
                ic::CPropertyBool* pProperty = dynamic_cast< ic::CPropertyBool* >( vecVars[ 0 ] );
                __gassert_( pProperty );
                *pPropertyValue = pProperty->GetValue();
            }
            break;

        case ic::EDWord:
            {
                ic::CPropertyDWord* pProperty = dynamic_cast< ic::CPropertyDWord* >( vecVars[ 0 ] );
                __gassert_( pProperty );
                *pPropertyValue = pProperty->GetValue();
            }
            break;

        case ic::EFloat:
            {
                ic::CPropertyFloat* pProperty = dynamic_cast< ic::CPropertyFloat* >( vecVars[ 0 ] );
                __gassert_( pProperty );
                *pPropertyValue = pProperty->GetValue();
            }
            break;

        case ic::EInt:
            {
                ic::CPropertyInt* pProperty = dynamic_cast< ic::CPropertyInt* >( vecVars[ 0 ] );
                __gassert_( pProperty );
                *pPropertyValue = pProperty->GetValue();
            }
            break;

        case ic::EString:
            {
                ic::CPropertyString* pProperty = dynamic_cast< ic::CPropertyString* >( vecVars[ 0 ] );
                __gassert_( pProperty );
                *pPropertyValue = pProperty->GetValue();
            }
            break;

        case ic::EVector3:
            {
                ic::CPropertyVector3* pProperty = dynamic_cast< ic::CPropertyVector3* >( vecVars[ 0 ] );
                __gassert_( pProperty );
                *pPropertyValue = pProperty->GetValue();
            }
            break;

        case ic::EWord:
            {
                ic::CPropertyWord* pProperty = dynamic_cast< ic::CPropertyWord* >( vecVars[ 0 ] );
                __gassert_( pProperty );
                *pPropertyValue = pProperty->GetValue();
            }
            break;
        }
    }

    VOID CSeqAct_SetProperty::DrawExtraInfo( ic::IRenderInterface* pRI, const ic::Int2& sCenter )
    {
        DrawStatusMark( pRI, sCenter, bStatusIsOk );
    }

    VOID CSeqAct_SetProperty::OnChangedKey()
    {
        vecVariableLinks[ 1 ].pExpected = CSequenceVariable::Static();
        bStatusIsOk = FALSE;

        if( strKey.empty() )
            return;

        std::vector< ic::CPropertyObject* > vecVarsTarget;
        FindVariable( vecVarsTarget, _T( "Target" ) );

        if( vecVarsTarget.empty() )
            return;

        ic::CBaseObject* pObject = vecVarsTarget[ 0 ]->GetValue();
        if( !pObject )
            return;

        ic::CProperty* pPropertyBase = pObject->VProperty( strKey );
        if( !pPropertyBase )
            return;

        switch( pPropertyBase->VType() )
        {
        case ic::Ebool:
        case ic::EBool:
            {
                vecVariableLinks[ 1 ].pExpected = CSeqVar_Bool::Static();
                bStatusIsOk = TRUE;
            }
            break;

        case ic::EByte:
        case ic::EColor:
        case ic::EDouble:
        case ic::EFlags:
        case ic::EDWord:
        case ic::EFloat:
        case ic::EInt:
        case ic::ELongLong:
        case ic::ESelect:
        case ic::EWord:
            {
                vecVariableLinks[ 1 ].pExpected = CSeqVar_Double::Static();
                bStatusIsOk = TRUE;
            }
            break;

        case ic::EString:
            {
                vecVariableLinks[ 1 ].pExpected = CSeqVar_String::Static();
                bStatusIsOk = TRUE;
            }
            break;

        case ic::EVector3:
            {
                vecVariableLinks[ 1 ].pExpected = CSeqVar_Vector3::Static();
                bStatusIsOk = TRUE;
            }
            break;
        }
    }

    CSeqAct_GetProperty::CSeqAct_GetProperty()
        : bStatusIsOk( FALSE )
    {
        strName = _T( "Get Property" );
    }

    VOID CSeqAct_GetProperty::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush;
            sPush.strLinkDesc = _T( "Target" );
            sPush.nMaxVars = 1;
            sPush.pExpected = ic::CBaseObject::Static();
            vecVariableLinks.push_back( sPush );

            sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Value" );
            sPush.nMaxVars = 1;
            sPush.pExpected = CSequenceVariable::Static();
            sPush.bWriteable = TRUE;
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_GetProperty::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyString* pProperty = ic::CPropertyFactory::New< ic::CPropertyString >();
            pProperty->SetPtr( &strKey );
            pProperty->SetCall( boost::bind( boost::mem_fn( &CSeqAct_GetProperty::OnChangedKey ), this ) );
            InsertProperty( _T( "Key" ), pProperty );
        }
    }

    TSTRING CSeqAct_GetProperty::GetObjectName()
    {
        CString strName_;
        strName_.Format( _T( "%s \"%s\"" ), strName.c_str(), strKey.c_str() );

        return strName_.GetString();
    }

    VOID CSeqAct_GetProperty::PostLoad()
    {
        OnChangedKey();
    }

    VOID CSeqAct_GetProperty::OnLinkVariable( CSequenceObject* pObject, INT nConnIndex )
    {
        if( nConnIndex == 0 )
            OnChangedKey();
    }

    VOID CSeqAct_GetProperty::OnPostBreakVariable( INT nConnIndex )
    {
        if( nConnIndex == 0 )
            OnChangedKey();
    }

    VOID CSeqAct_GetProperty::Activated( size_t nInput )
    {
        CSeqAct_GetProperty* pThis = this;
        BOOST_SCOPE_EXIT( ( &pThis ) )
        {
            pThis->DeActivated();
        }
        BOOST_SCOPE_EXIT_END

        std::vector< ic::CPropertyObject* > vecVarsTarget;
        FindVariable( vecVarsTarget, _T( "Target" ) );

        if( vecVarsTarget.empty() )
            return;

        ic::CBaseObject* pObject = vecVarsTarget[ 0 ]->GetValue();
        if( !pObject )
            return;

        ic::CProperty* pPropertyBase = pObject->VProperty( strKey );
        ic::IPropertyValue* pPropertyValue = dynamic_cast< ic::IPropertyValue* >( pPropertyBase );
        if( !pPropertyValue )
            return;

        ic::IPropertyValueVec vecVars;
        GetVariable( vecVars, 1 );

        if( vecVars.empty() )
            return;

        switch( pPropertyValue->VType() )
        {
        case ic::Ebool:
            {
                ic::CPropertybool* pProperty = dynamic_cast< ic::CPropertybool* >( pPropertyValue );
                __gassert_( pProperty );
                *vecVars[ 0 ] = pProperty->GetValue();
            }
            break;

        case ic::EBool:
            {
                ic::CPropertyBool* pProperty = dynamic_cast< ic::CPropertyBool* >( pPropertyValue );
                __gassert_( pProperty );
                *vecVars[ 0 ] = pProperty->GetValue();
            }
            break;

        case ic::EDWord:
            {
                ic::CPropertyDWord* pProperty = dynamic_cast< ic::CPropertyDWord* >( pPropertyValue );
                __gassert_( pProperty );
                *vecVars[ 0 ] = pProperty->GetValue();
            }
            break;

        case ic::EFloat:
            {
                ic::CPropertyFloat* pProperty = dynamic_cast< ic::CPropertyFloat* >( pPropertyValue );
                __gassert_( pProperty );
                *vecVars[ 0 ] = pProperty->GetValue();
            }
            break;

        case ic::EInt:
            {
                ic::CPropertyInt* pProperty = dynamic_cast< ic::CPropertyInt* >( pPropertyValue );
                __gassert_( pProperty );
                *vecVars[ 0 ] = pProperty->GetValue();
            }
            break;

        case ic::EString:
            {
                ic::CPropertyString* pProperty = dynamic_cast< ic::CPropertyString* >( pPropertyValue );
                __gassert_( pProperty );
                *vecVars[ 0 ] = pProperty->GetValue();
            }
            break;

        case ic::EVector3:
            {
                ic::CPropertyVector3* pProperty = dynamic_cast< ic::CPropertyVector3* >( pPropertyValue );
                __gassert_( pProperty );
                *vecVars[ 0 ] = pProperty->GetValue();
            }
            break;

        case ic::EWord:
            {
                ic::CPropertyWord* pProperty = dynamic_cast< ic::CPropertyWord* >( pPropertyValue );
                __gassert_( pProperty );
                *vecVars[ 0 ] = pProperty->GetValue();
            }
            break;
        }
    }

    VOID CSeqAct_GetProperty::DrawExtraInfo( ic::IRenderInterface* pRI, const ic::Int2& sCenter )
    {
        DrawStatusMark( pRI, sCenter, bStatusIsOk );
    }

    VOID CSeqAct_GetProperty::OnChangedKey()
    {
        vecVariableLinks[ 1 ].pExpected = CSequenceVariable::Static();
        bStatusIsOk = FALSE;

        if( strKey.empty() )
            return;

        std::vector< ic::CPropertyObject* > vecVarsTarget;
        FindVariable( vecVarsTarget, _T( "Target" ) );

        if( vecVarsTarget.empty() )
            return;

        ic::CBaseObject* pObject = vecVarsTarget[ 0 ]->GetValue();
        if( !pObject )
            return;

        ic::CProperty* pPropertyBase = pObject->VProperty( strKey );
        if( !pPropertyBase )
            return;

        switch( pPropertyBase->VType() )
        {
        case ic::Ebool:
        case ic::EBool:
            {
                vecVariableLinks[ 1 ].pExpected = CSeqVar_Bool::Static();
                bStatusIsOk = TRUE;
            }
            break;

        case ic::EByte:
        case ic::EColor:
        case ic::EDouble:
        case ic::EFlags:
        case ic::EDWord:
        case ic::EFloat:
        case ic::EInt:
        case ic::ELongLong:
        case ic::ESelect:
        case ic::EWord:
            {
                vecVariableLinks[ 1 ].pExpected = CSeqVar_Double::Static();
                bStatusIsOk = TRUE;
            }
            break;

        case ic::EString:
            {
                vecVariableLinks[ 1 ].pExpected = CSeqVar_String::Static();
                bStatusIsOk = TRUE;
            }
            break;

        case ic::EVector3:
            {
                vecVariableLinks[ 1 ].pExpected = CSeqVar_Vector3::Static();
                bStatusIsOk = TRUE;
            }
            break;
        }
    }

    CSeqAct_Function::CSeqAct_Function()
        : bStatusIsOk( FALSE )
    {
        strName = _T( "Function" );
    }

    VOID CSeqAct_Function::PostNew()
    {
        Super::PostNew();

        // Input
        {
            SSeqOpInputLink sPush;
            sPush.strLinkDesc = _T( "In" );
            vecInputLinks.push_back( sPush );
        }

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            SSeqVarLink sPush = SSeqVarLink();
            sPush.strLinkDesc = _T( "Target" );
            sPush.nMaxVars = 1;
            sPush.pExpected = ic::CBaseObject::Static();
            sPush.bWriteable = TRUE;
            vecVariableLinks.push_back( sPush );
        }
    }

    VOID CSeqAct_Function::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyString* pProperty = ic::CPropertyFactory::New< ic::CPropertyString >();
            pProperty->SetPtr( &strKey );
            pProperty->SetCall( boost::bind( boost::mem_fn( &CSeqAct_Function::OnChangedKey ), this ) );
            InsertProperty( _T( "Key" ), pProperty );
        }
    }

    TSTRING CSeqAct_Function::GetObjectName()
    {
        CString strName_;
        strName_.Format( _T( "%s \"%s\"" ), strName.c_str(), strKey.c_str() );

        return strName_.GetString();
    }

    VOID CSeqAct_Function::PostLoad()
    {
        OnChangedKey();
    }

    VOID CSeqAct_Function::OnLinkVariable( CSequenceObject* pObject, INT nConnIndex )
    {
        if( nConnIndex == 0 )
            OnChangedKey();
    }

    VOID CSeqAct_Function::OnPostBreakVariable( INT nConnIndex )
    {
        if( nConnIndex == 0 )
            OnChangedKey();
    }

    VOID CSeqAct_Function::Activated( size_t nInput )
    {
        CSeqAct_Function* pThis = this;
        BOOST_SCOPE_EXIT( ( &pThis ) )
        {
            pThis->DeActivated();
        }
        BOOST_SCOPE_EXIT_END

        std::vector< ic::CPropertyObject* > vecVarsTarget;
        FindVariable( vecVarsTarget, _T( "Target" ) );

        if( vecVarsTarget.empty() )
            return;

        ic::CBaseObject* pObject = vecVarsTarget[ 0 ]->GetValue();
        if( !pObject )
            return;

        ic::CProperty* pPropertyBase = pObject->VProperty( strKey );
        if( !pPropertyBase )
            return;

        switch( pPropertyBase->VType() )
        {
        case ic::EFunctionArgs:
            {
                ic::CPropertyFnArgs* pProperty = dynamic_cast< ic::CPropertyFnArgs* >( pPropertyBase );
                __gassert_( pProperty );

                ic::IPropertyValueVec vecArgs;
                INT nIndex = 1;

                ic::CPropertyFnArgs::ArgTypeVec& vecArgType = pProperty->GetArgTypes();
                BOOST_FOREACH( ic::CPropertyFnArgs::ArgType& pairArg, vecArgType )
                {
                    ic::IPropertyValueVec vecVars;
                    GetVariable( vecVars, nIndex );

                    if( vecVars.empty() )
                        return;

                    vecArgs.push_back( vecVars[ 0 ] );
                    ++nIndex;
                }

                pProperty->Call( vecArgs );
            }
            break;

        case ic::EFunctionVoid:
            {
                ic::CPropertyFnVoid* pProperty = dynamic_cast< ic::CPropertyFnVoid* >( pPropertyBase );
                __gassert_( pProperty );

                pProperty->Call();
            }
            break;
        }
    }

    VOID CSeqAct_Function::DrawExtraInfo( ic::IRenderInterface* pRI, const ic::Int2& sCenter )
    {
        DrawStatusMark( pRI, sCenter, bStatusIsOk );
    }

    static ic::CBaseObject* FromPropTypeToVariable( BYTE nType )
    {
        switch( nType )
        {
        case ic::Ebool: return CSeqVar_Bool::Static();
        case ic::EBool: return CSeqVar_Bool::Static();
        case ic::EByte: return CSeqVar_Double::Static();
        case ic::EColor: return CSeqVar_Double::Static();
        case ic::EDouble: return CSeqVar_Double::Static();
        case ic::EDWord: return CSeqVar_Double::Static();
        case ic::EFlags: return CSeqVar_Double::Static();
        case ic::EFloat: return CSeqVar_Double::Static();
        case ic::EInt: return CSeqVar_Double::Static();
        case ic::ELongLong: return CSeqVar_Double::Static();
        case ic::EObject: return CSeqVar_Object::Static();
        case ic::ESelect: return CSeqVar_Double::Static();
        case ic::EString: return CSeqVar_String::Static();
        case ic::EVector3: return CSeqVar_Vector3::Static();
        case ic::EWord: return CSeqVar_Double::Static();
        }

        return CSequenceVariable::Static();
    }

    VOID CSeqAct_Function::OnChangedKey()
    {
        BOOST_SCOPE_EXIT( ( &bStatusIsOk )( &vecVariableLinks ) )
        {
            if( !bStatusIsOk )
            {
                while( vecVariableLinks.size() > 1 )
                    vecVariableLinks.pop_back();
            }
        }
        BOOST_SCOPE_EXIT_END

        bStatusIsOk = FALSE;

        if( strKey.empty() )
            return;

        std::vector< ic::CPropertyObject* > vecVarsTarget;
        FindVariable( vecVarsTarget, _T( "Target" ) );

        if( vecVarsTarget.empty() )
            return;

        ic::CBaseObject* pObject = vecVarsTarget[ 0 ]->GetValue();
        if( !pObject )
            return;

        ic::CProperty* pPropertyBase = pObject->VProperty( strKey );
        if( !pPropertyBase )
            return;

        switch( pPropertyBase->VType() )
        {
        case ic::EFunctionArgs:
            {
                ic::CPropertyFnArgs* pProperty = dynamic_cast< ic::CPropertyFnArgs* >( pPropertyBase );
                __gassert_( pProperty );

                INT nSizeVar = (INT)vecVariableLinks.size();
                INT nIndex = 1;

                ic::CPropertyFnArgs::ArgTypeVec& vecArgType = pProperty->GetArgTypes();
                BOOST_FOREACH( ic::CPropertyFnArgs::ArgType& pairArg, vecArgType )
                {
                    if( nIndex < nSizeVar )
                    {
                        ic::CBaseObject* pExpected = FromPropTypeToVariable( pairArg.first );

                        SSeqVarLink& sPush = vecVariableLinks[ nIndex ];
                        sPush.strLinkDesc = pairArg.second;
                        if( sPush.pExpected != pExpected )
                        {
                            sPush.vecLinks.clear();
                            sPush.pExpected = pExpected;
                        }
                    }
                    else
                    {
                        SSeqVarLink sPush;
                        sPush.strLinkDesc = pairArg.second;
                        sPush.nMaxVars = 1;
                        sPush.pExpected = FromPropTypeToVariable( pairArg.first );
                        sPush.bWriteable = TRUE;
                        vecVariableLinks.push_back( sPush );
                    }

                    ++nIndex;
                }

                while( vecVariableLinks.size() > vecArgType.size() + 1 )
                    vecVariableLinks.pop_back();

                bStatusIsOk = TRUE;
            }
            break;

        case ic::EFunctionVoid:
            {
                bStatusIsOk = TRUE;
            }
            break;
        }
    }
}