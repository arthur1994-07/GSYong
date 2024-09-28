/**
 * \date	2011/01/27
 * \author	Jun-Hyuk Choi
 * \ref     Unreal3
 */
#pragma once

#include "./SequenceObject.h"

namespace vs
{
    class CSequenceOp;
    class CSequenceEvent;
    class CSequenceVariable;

    typedef std::vector< FLOAT > SeqImpulseVec;
    typedef SeqImpulseVec::iterator SeqImpulseVecIter;

    struct SSeqOpInputLink
    {
        enum { EVersion = 1 };

        TSTRING strLinkDesc;
        TSTRING strLinkAction;
        FLOAT fDelay;
        SeqImpulseVec vecImpulse;
        INT nDrawY;

        SSeqOpInputLink()
            : fDelay( SequenceImpulseEnable )
            , nDrawY( 0 )
        {
        }

        SERIALIZE_DEFAULT;
    };

    struct SSeqOpOutputInputLink
    {
        enum { EVersion = 1 };

        CSequenceOp* pLinkedOp;
        INT nInputLinkIndex;

        SSeqOpOutputInputLink()
            : pLinkedOp( NULL )
            , nInputLinkIndex( 0 )
        {
        }

        SERIALIZE_DEFAULT;
    };

    typedef std::vector< SSeqOpOutputInputLink > SeqOutputInputVec;
    typedef SeqOutputInputVec::iterator SeqOutputInputVecIter;

    struct SSeqOpOutputLink
    {
        enum { EVersion = 1 };

        TSTRING strLinkDesc;
        TSTRING strLinkAction;
        FLOAT fDelay;
        SeqOutputInputVec vecLinks;
        SeqImpulseVec vecImpulse;
        INT nDrawY;

        SSeqOpOutputLink()
            : fDelay( SequenceImpulseEnable )
            , nDrawY( 0 )
        {
        }

        SERIALIZE_DEFAULT;
    };

    typedef std::vector< CSequenceVariable* > SeqVariableVec;
    typedef SeqVariableVec::iterator SeqVariableVecIter;

    struct SSeqVarLink
    {
        enum { EVersion = 1 };

        TSTRING strLinkDesc;
        TSTRING strLinkVar;
        INT nMaxVars;
        BOOL bWriteable;
        ic::CBaseObject* pExpected;
        SeqVariableVec vecLinks;
        INT nDrawX;

        SSeqVarLink()
            : nMaxVars( -1 )
            , nDrawX( 0 )
            , bWriteable( FALSE )
            , pExpected( NULL )
        {
        }

        SERIALIZE_DEFAULT;
    };

    typedef std::vector< CSequenceOp* > SequenceOpVec;
    typedef SequenceOpVec::iterator SequenceOpVecIter;

    class CSequenceOp : public ic::CRttiObject< CSequenceOp, CSequenceObject, 1, ic::EInternal >
    {
    public:
        typedef std::vector< SSeqOpInputLink > InputVec;
        typedef std::vector< SSeqOpOutputLink > OutputVec;
        typedef std::vector< SSeqVarLink > VariableVec;

    public:
        InputVec vecInputLinks;
        OutputVec vecOutputLinks;
        VariableVec vecVariableLinks;

    public:
        CSequenceOp();

    public:
        virtual VOID CheckForErrors();

        virtual VOID ProcessInput();
        virtual VOID ProcessOutput( SequenceOpVec& vecOp );
        virtual VOID Activated( size_t nInput = 0 ) {}
        virtual VOID DeActivated();

        virtual VOID InsertInputImpulse( size_t nIndex = 0 );
        virtual VOID InsertOutputImpulse( size_t nIndex = 0 );
        virtual BOOL UpdateInputImpulse( FLOAT fElapsedTime );
        virtual BOOL UpdateOutputImpulse( FLOAT fElapsedTime );

        virtual ic::Int2 GetConnectionLocation( INT nConnType, INT nConnIndex );

        virtual VOID DrawObject(
            ic::IRenderInterface* pRI,
            BOOL bSelected,
            BOOL bMouseOver,
            INT nMouseOverConnType,
            INT nMouseOverConnIndex );

        virtual VOID DrawLogicLinks( ic::IRenderInterface* pRI );
        virtual VOID DrawVariableLinks( ic::IRenderInterface* pRI );

        INT FindConnectorIndex( const TSTRING& strConnName, INT nConnType );
        VOID CleanupConnections();

        ic::Int2 GetLogicConnectorsSize(
            ic::IRenderInterface* pRI,
            INT* pInputY = NULL,
            INT* pOutputY = NULL );
        ic::Int2 GetVariableConnectorsSize( ic::IRenderInterface* pRI );
        DWORD GetVarConnectorColor( INT nLinkIndex );

        VOID DrawLogicConnectors(
            ic::IRenderInterface* pRI,
            const ic::Int2& sPos_,
            const ic::Int2& sSize,
            INT nMouseOverConnType,
            INT nMouseOverConnIndex );

        VOID DrawVariableConnectors(
            ic::IRenderInterface* pRI,
            const ic::Int2& sPos_,
            const ic::Int2& sSize,
            INT nMouseOverConnType,
            INT nMouseOverConnIndex );

        VOID MakeLinkedObjDrawInfo(
            ic::SLinkedObjDrawInfo& sObjInfo,
            INT nMouseOverConnType = INT_MIN,
            INT nMouseOverConnIndex = INT_MIN );

        VOID LinkFromIdentity( const SequenceObjectMap& mapObjects );

        template< class T >
        VOID GetVariable( std::vector< T* >& vecProperty, INT nIndex );
        template< class T >
        VOID FindVariable( std::vector< T* >& vecProperty, const TSTRING& strDesc );

    private:
        SERIALIZE_DEFAULT;
    };

    template< class T >
    VOID CSequenceOp::GetVariable( std::vector< T* >& vecProperty, INT nIndex )
    {
        if( nIndex < 0 || nIndex >= (INT)vecVariableLinks.size() )
            return;

        SSeqVarLink& sVarLink = vecVariableLinks[ nIndex ];

        BOOST_FOREACH( CSequenceVariable* pVariable, sVarLink.vecLinks )
        {
            if( !pVariable )
                continue;

            T* pProperty = dynamic_cast< T* >( pVariable->GetProperty() );
            if( pProperty )
                vecProperty.push_back( pProperty );
        }
    }

    template< class T >
    VOID CSequenceOp::FindVariable( std::vector< T* >& vecProperty, const TSTRING& strDesc )
    {
        BOOST_FOREACH( const SSeqVarLink& sVarLink, vecVariableLinks )
        {
            if( strDesc.compare( sVarLink.strLinkDesc ) != 0 )
                continue;

            BOOST_FOREACH( CSequenceVariable* pVariable, sVarLink.vecLinks )
            {
                if( !pVariable )
                    continue;

                T* pProperty = dynamic_cast< T* >( pVariable->GetProperty() );
                if( pProperty )
                    vecProperty.push_back( pProperty );
            }
        }
    }
}

SERIALIZE_VERSION( vs::SSeqOpInputLink )
SERIALIZE_VERSION( vs::SSeqOpOutputInputLink )
SERIALIZE_VERSION( vs::SSeqOpOutputLink )
SERIALIZE_VERSION( vs::SSeqVarLink )