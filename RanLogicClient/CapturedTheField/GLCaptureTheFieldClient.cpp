#include "pch.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/Common/SUBPATH.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/Land/GLMapList.h"
#include "../../RanLogicClient/Level/GLLevelFileClient.h"

#include "../GLGaeaClient.h"

#include "../Char/GLCharacter.h"
#include "./GLCaptureTheFieldClient.h"

#include <boost/checked_delete.hpp>
#include <boost/lambda/lambda.hpp>

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace PVP;

//--------------------------------------------------------------------------------------------//

const bool SAUTHENTICATORINFO::Init()
{
    // 몹의 설정 정보를 가져옴.
    SCROWDATA* pCrowdata = GLCrowDataMan::GetInstance().GetCrowData ( sID );

    if( !pCrowdata )
    {
        SNATIVEID sMobID = sID;
        sc::writeLogError(
            sc::string::format(
                "SAUTHENTICATORINFO::Init()-GLCrowDataMan::GetCrowData() [%1%/%2%] 가져오기 실패.", sMobID.wMainID, sMobID.wSubID));
        return false;
    }

    strName = pCrowdata->GetName();

    return true;
}

void GLCaptureTheFieldClient::CTFGLOBALSTATE::INIT( GLGaeaClient* pGaeaClient )
{
	if ( sPVPMAPID != SNATIVEID(false) )
	{
		SMAPNODE *pMapNode = pGaeaClient->FindMapNode ( sPVPMAPID );

		GASSERT_MSG( pMapNode && "GLCaptureTheFieldClient::CTFGLOBALSTATE::INIT()", 
			sc::string::format("FindMapNode is Failed (mid=%1%,sid=%2%)", sPVPMAPID.wMainID, sPVPMAPID.wSubID) );

		if ( pMapNode )
		{
			spPVPMAPLEVELDATA.reset(new GLLevelFileClient(pGaeaClient));

			if( !spPVPMAPLEVELDATA->LoadFile ( pMapNode->m_LevelFile.c_str(), NULL ) )
			{
				GASSERT_MSG( !"GLCaptureTheFieldClient::CTFGLOBALSTATE::INIT()", 
					sc::string::format("PVPMAPLEVELDATA Load Failed (Filename='%1%')", pMapNode->m_LevelFile) );
			}

			spPVPMAPLEVELDATA->SetBaseMapID(sPVPMAPID);
			spPVPMAPLEVELDATA->SetPeaceZone(pMapNode->IsPeaceZone());
			spPVPMAPLEVELDATA->SetPKZone(true);
		}
	}

	if ( sPVEMAPID != SNATIVEID(false) )
	{
		SMAPNODE *pMapNode = pGaeaClient->FindMapNode ( sPVEMAPID );

		GASSERT_MSG( pMapNode && "GLCaptureTheFieldClient::CTFGLOBALSTATE::INIT()", 
			sc::string::format("FindMapNode is Failed (mid=%1%,sid=%2%)", sPVEMAPID.wMainID, sPVEMAPID.wSubID) );

		if ( pMapNode )
		{
			spPVEMAPLEVELDATA.reset(new GLLevelFileClient(pGaeaClient));

			if( !spPVEMAPLEVELDATA->LoadFile ( pMapNode->m_LevelFile.c_str(), NULL ) )
			{
				GASSERT_MSG( !"GLCaptureTheFieldClient::CTFGLOBALSTATE::INIT()", 
					sc::string::format("PVEMAPLEVELDATA Load Failed (Filename='%1%')", pMapNode->m_LevelFile) );
			}

			spPVPMAPLEVELDATA->SetBaseMapID(sPVPMAPID);
			spPVPMAPLEVELDATA->SetPeaceZone(pMapNode->IsPeaceZone());
			spPVPMAPLEVELDATA->SetPKZone(true);
		}
	}
}

void GLCaptureTheFieldClient::CTFGLOBALSTATE::RESETAUTH()
{
    emAUTHSCHOOL = SCHOOL_NONE;

    UINT nSize = (UINT)vecAUTHENTICATOR.size();

    for( UINT i=0; i<nSize; ++i )
    {
        vecAUTHENTICATOR[i].emAuthSchool = SCHOOL_NONE;
    }
}

bool GLCaptureTheFieldClient::CTFGLOBALSTATE::IsJOINDENIED( GLCharacter* pCharacter ) const
{
	if ( !pCharacter )
	{
		return true;
	}

	if ( pCharacter->GETLEVEL() < wLEVELLIMIT )
	{
		return true;
	}

	return false;
}

SAUTHENTICATORINFO& GLCaptureTheFieldClient::CTFGLOBALSTATE::FindAuthenticator( const SNATIVEID& sID )
{
    UINT nSize = (UINT)vecAUTHENTICATOR.size();

    for( UINT i=0; i<nSize; ++i )
    {
        if ( vecAUTHENTICATOR[i].sID == sID )
        {
            return vecAUTHENTICATOR[i];
        }
    }

    static SAUTHENTICATORINFO NULLINFO(SNATIVEID(false));
    return NULLINFO;
}

//--------------------------------------------------------------------------------------------//
GLCaptureTheFieldClient::GLCaptureTheFieldClient( GLGaeaClient* pGaeaClient )
: m_pGaeaClient(pGaeaClient)
, m_emProgressCTF(ENUM_CTF_TYPE_NULL)
, m_MyResultRanking(0)
, m_MyResultRankingSchool(0)
{
}

GLCaptureTheFieldClient::~GLCaptureTheFieldClient()
{
    Clear();
}

void GLCaptureTheFieldClient::Clear()
{
	Reset();
    m_mGlobalState.clear();
	m_mPrevState.clear();
}

HRESULT GLCaptureTheFieldClient::FrameMove ( float fTime, float fElapsedTime )
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return S_OK;
    }

    return S_OK;
}

void GLCaptureTheFieldClient::SetGlobalState ( const CTFGLOBALSTATE& sCTFState )
{
    m_mGlobalState[sCTFState.emTYPE] = sCTFState;
}

GLCaptureTheFieldClient::CTFGLOBALSTATE& GLCaptureTheFieldClient::GetGlobalState ( const ENUM_CTF_TYPE emTYPE )
{
	if ( emTYPE == ENUM_CTF_TYPE_NULL )
	{
		static CTFGLOBALSTATE NULLSTATE;
		return NULLSTATE;
	}

    CTFGLOBALSTATE_MAP_ITER finditer = m_mGlobalState.find( emTYPE );

    if ( finditer == m_mGlobalState.end() )
    {
        //! 여기서 최초에 생성시켜준다.
        m_mGlobalState[emTYPE] = CTFGLOBALSTATE(emTYPE, CTF_STATE_STOP);
        return m_mGlobalState[emTYPE];
    }

    return finditer->second;
}

GLCaptureTheFieldClient::CTFGLOBALSTATE& GLCaptureTheFieldClient::GetGlobalState ( const SNATIVEID& sMapID )
{
    CTFGLOBALSTATE_MAP_ITER iter = m_mGlobalState.begin();

    for ( ; iter!=m_mGlobalState.end(); ++iter )
    {
        CTFGLOBALSTATE& sState = (iter->second);

        if ( sState.IsSAMEMAP( sMapID ) )
        {
            return sState;
        }
    }

    static CTFGLOBALSTATE NULLSTATE;
    return NULLSTATE;
}

GLCaptureTheFieldClient::CTFGLOBALSTATE& GLCaptureTheFieldClient::GetCurrentGlobalState ()
{
    return GetGlobalState( GetProgressCTF() );
}

void GLCaptureTheFieldClient::SetPrevState ( const CTFPREVSTATE& sPrevState )
{
	m_mPrevState[sPrevState.emTYPE] = sPrevState;
}

GLCaptureTheFieldClient::CTFPREVSTATE& GLCaptureTheFieldClient::GetPrevState ( const ENUM_CTF_TYPE emTYPE )
{
	if ( emTYPE == ENUM_CTF_TYPE_NULL )
	{
		static CTFPREVSTATE NULLSTATE;
		return NULLSTATE;
	}

	CTFPREVSTATE_MAP_ITER finditer = m_mPrevState.find( emTYPE );

	if ( finditer == m_mPrevState.end() )
	{
		//! 여기서 최초에 생성시켜준다.
		m_mPrevState[emTYPE] = CTFPREVSTATE();
		m_mPrevState[emTYPE].emTYPE = emTYPE;
		return m_mPrevState[emTYPE];
	}

	return finditer->second;
}

void GLCaptureTheFieldClient::Reset()
{
	m_emProgressCTF = ENUM_CTF_TYPE_NULL;
    ClearAllianceInfo();
	ClearResultData();
}

void GLCaptureTheFieldClient::ClearAllianceInfo()
{
	m_vecAllianceInfo.clear();
}

void GLCaptureTheFieldClient::ClearResultData()
{
	std::for_each( m_vecResultData.begin(), m_vecResultData.end(), boost::checked_deleter<SCTFRESULTDATA>() );
	m_vecResultData.clear();
	m_vecResultDataSM.clear();
	m_vecResultDataHA.clear();
	m_vecResultDataBH.clear();
	m_MyResultData.Reset();

	m_MyResultRanking = 0;
	m_MyResultRankingSchool = 0;
}

void GLCaptureTheFieldClient::AddResultData( const SCTFRESULTDATA& sData )
{
	SCTFRESULTDATA* pNewData = new SCTFRESULTDATA(sData);

	m_vecResultData.push_back( pNewData );

	switch ( pNewData->emSCHOOL )
	{
	case SCHOOL_SM: m_vecResultDataSM.push_back( pNewData ); break;
	case SCHOOL_HA: m_vecResultDataHA.push_back( pNewData ); break;
	case SCHOOL_BH: m_vecResultDataBH.push_back( pNewData ); break;
	};
		
	if ( m_vecResultData.size() > 1 )
	{
		using namespace boost::lambda;
		std::inplace_merge( m_vecResultData.begin(), m_vecResultData.end()-1, m_vecResultData.end(), (*boost::lambda::_1) < (*boost::lambda::_2) );

		switch ( pNewData->emSCHOOL )
		{
		case SCHOOL_SM: std::inplace_merge( m_vecResultDataSM.begin(), m_vecResultDataSM.end()-1, m_vecResultDataSM.end(), (*boost::lambda::_1) < (*boost::lambda::_2) ); break;
		case SCHOOL_HA: std::inplace_merge( m_vecResultDataHA.begin(), m_vecResultDataHA.end()-1, m_vecResultDataHA.end(), (*boost::lambda::_1) < (*boost::lambda::_2) ); break;
		case SCHOOL_BH: std::inplace_merge( m_vecResultDataBH.begin(), m_vecResultDataBH.end()-1, m_vecResultDataBH.end(), (*boost::lambda::_1) < (*boost::lambda::_2) ); break;
		};
	}

	//! 이미 정보가 있다면 랭킹을 구한다
	if ( !m_MyResultData.IsNull() )
	{
		UINT nRankAdd = (sData < m_MyResultData ) ? 1 : 0;

		m_MyResultRanking += nRankAdd;

		if ( pNewData->emSCHOOL == m_MyResultData.emSCHOOL )
		{
			m_MyResultRankingSchool += nRankAdd;
		}
	}
	//! 이름이 같다면 나의 정보에 값을 넣는다.
	else if ( m_pGaeaClient->GetCharacter()->GetName() == pNewData->strNAME )
	{
		m_MyResultData = (*pNewData);

		//! 현재 나의 랭킹을 구함
		using namespace boost::lambda;
		VEC_RESULTDATA_ITER finditer = std::find_if( m_vecResultData.begin(), m_vecResultData.end(), (*boost::lambda::_1) == sData );
		m_MyResultRanking = (UINT)std::distance(m_vecResultData.begin(), finditer);

		switch ( pNewData->emSCHOOL )
		{
		case SCHOOL_SM:
			{
                VEC_RESULTDATA_ITER finditer = std::find_if( m_vecResultDataSM.begin(), m_vecResultDataSM.end(), (*boost::lambda::_1) == sData );
				m_MyResultRankingSchool = (UINT)std::distance(m_vecResultDataSM.begin(), finditer);
			}
			break;

		case SCHOOL_HA:
			{
				VEC_RESULTDATA_ITER finditer = std::find_if( m_vecResultDataHA.begin(), m_vecResultDataHA.end(), (*boost::lambda::_1) == sData );
				m_MyResultRankingSchool = (UINT)std::distance(m_vecResultDataHA.begin(), finditer);
			}
			break;

		case SCHOOL_BH:
			{
				VEC_RESULTDATA_ITER finditer = std::find_if( m_vecResultDataBH.begin(), m_vecResultDataBH.end(), (*boost::lambda::_1) == sData );
				m_MyResultRankingSchool = (UINT)std::distance(m_vecResultDataBH.begin(), finditer);
			}
			break;
		};
	}
}

GLLevelFileClient* GLCaptureTheFieldClient::GetPVPMapLevelData( const ENUM_CTF_TYPE& emProgressCTF )
{
	CTFGLOBALSTATE& sState = GetGlobalState( emProgressCTF );

	if ( sState.IsNULL() )
	{
		return NULL;
	}

	return sState.spPVPMAPLEVELDATA.get();
}

const bool GLCaptureTheFieldClient::IsCTFMap( const SNATIVEID& sMapID )
{
    CTFGLOBALSTATE_MAP_ITER iter = m_mGlobalState.begin();

    for ( ; iter!=m_mGlobalState.end(); ++iter )
    {
        CTFGLOBALSTATE& sState = (iter->second);

        if ( sState.IsSAMEMAP( sMapID ) )
        {
            return true;
        }
    }

    return false;
}

const bool GLCaptureTheFieldClient::IsCTFMap_PVP ( const SNATIVEID& _sToMapID ) const
{
	CTFGLOBALSTATE_MAP_CITER iter = m_mGlobalState.begin();
	for ( ; iter != m_mGlobalState.end(); ++iter )
	{
		const CTFGLOBALSTATE& sState = iter->second;
		if ( sState.IsPVPMap( _sToMapID ) )
		{
			return true;
		}
	}

	return false;
}

const bool GLCaptureTheFieldClient::IsCTFMap_PVE ( const SNATIVEID& _sToMapID ) const
{
	CTFGLOBALSTATE_MAP_CITER iter = m_mGlobalState.begin();

	for ( ; iter!=m_mGlobalState.end(); ++iter )
	{
		const CTFGLOBALSTATE& sState = iter->second;
		if ( sState.IsPVEMap( _sToMapID ) )
		{
			return true;
		}
	}

	return false;
}

DWORD GLCaptureTheFieldClient::GetCTFCaptureSchool(void)
{
	CTFGLOBALSTATE& sCTFState = GetGlobalState( ENUM_CTF_TYPE_POWERSTATION );
	return sCTFState.emAUTHSCHOOL;
}

//Scleform Funtion Start
lua_tinker::table GLCaptureTheFieldClient::GetResultData(void) const
{
	lua_tinker::table tbResult( GLWidgetScript::GetInstance().GetLuaState() );

	for ( UINT i = 0; i < m_vecResultData.size(); ++i )
	{
		lua_tinker::table tbRankInfo( GLWidgetScript::GetInstance().GetLuaState() );
		tbRankInfo.set( "strNAME", m_vecResultData[i]->strNAME );
		tbRankInfo.set( "emSCHOOL", m_vecResultData[i]->emSCHOOL );
		tbRankInfo.set( "emCLASS", m_vecResultData[i]->emCLASS );
		tbRankInfo.set( "nCONTRIBUTION", m_vecResultData[i]->nCONTRIBUTION );
		tbRankInfo.set( "nDAMAGE", m_vecResultData[i]->nDAMAGE );
		tbRankInfo.set( "nHEAL", m_vecResultData[i]->nHEAL );
		tbRankInfo.set( "nKILL", m_vecResultData[i]->nKILL );
		tbRankInfo.set( "nDEATH", m_vecResultData[i]->nDEATH );
		tbRankInfo.set( "nREBIRTH", m_vecResultData[i]->nREBIRTH );

		tbResult.set( i + 1 , tbRankInfo );
	}
	
	return tbResult;
}

lua_tinker::table GLCaptureTheFieldClient::GetResultSM(void) const
{
	lua_tinker::table tbResult( GLWidgetScript::GetInstance().GetLuaState() );

	for ( UINT i = 0; i < m_vecResultDataSM.size(); ++i )
	{
		lua_tinker::table tbRankInfo( GLWidgetScript::GetInstance().GetLuaState() );

		tbRankInfo.set( "strNAME", m_vecResultDataSM[i]->strNAME );
		tbRankInfo.set( "emSCHOOL", m_vecResultDataSM[i]->emSCHOOL );
		tbRankInfo.set( "emCLASS", m_vecResultDataSM[i]->emCLASS );
		tbRankInfo.set( "nCONTRIBUTION", m_vecResultDataSM[i]->nCONTRIBUTION );
		tbRankInfo.set( "nDAMAGE", m_vecResultDataSM[i]->nDAMAGE );
		tbRankInfo.set( "nHEAL", m_vecResultDataSM[i]->nHEAL );
		tbRankInfo.set( "nKILL", m_vecResultDataSM[i]->nKILL );
		tbRankInfo.set( "nDEATH", m_vecResultDataSM[i]->nDEATH );
		tbRankInfo.set( "nREBIRTH", m_vecResultDataSM[i]->nREBIRTH );

		tbResult.set( i + 1 , tbRankInfo );
	}

	return tbResult;
}

lua_tinker::table GLCaptureTheFieldClient::GetResultHA(void) const
{
	lua_tinker::table tbResult( GLWidgetScript::GetInstance().GetLuaState() );

	for ( UINT i = 0; i < m_vecResultDataHA.size(); ++i )
	{
		lua_tinker::table tbRankInfo( GLWidgetScript::GetInstance().GetLuaState() );

		tbRankInfo.set( "strNAME", m_vecResultDataHA[i]->strNAME );
		tbRankInfo.set( "emSCHOOL", m_vecResultDataHA[i]->emSCHOOL );
		tbRankInfo.set( "emCLASS", m_vecResultDataHA[i]->emCLASS );
		tbRankInfo.set( "nCONTRIBUTION", m_vecResultDataHA[i]->nCONTRIBUTION );
		tbRankInfo.set( "nDAMAGE", m_vecResultDataHA[i]->nDAMAGE );
		tbRankInfo.set( "nHEAL", m_vecResultDataHA[i]->nHEAL );
		tbRankInfo.set( "nKILL", m_vecResultDataHA[i]->nKILL );
		tbRankInfo.set( "nDEATH", m_vecResultDataHA[i]->nDEATH );
		tbRankInfo.set( "nREBIRTH", m_vecResultDataHA[i]->nREBIRTH );

		tbResult.set( i + 1 , tbRankInfo );
	}

	return tbResult;
}

lua_tinker::table GLCaptureTheFieldClient::GetResultBA(void) const
{
	lua_tinker::table tbResult( GLWidgetScript::GetInstance().GetLuaState() );

	for ( UINT i = 0; i < m_vecResultDataBH.size(); ++i )
	{
		lua_tinker::table tbRankInfo( GLWidgetScript::GetInstance().GetLuaState() );

		tbRankInfo.set( "strNAME", m_vecResultDataBH[i]->strNAME );
		tbRankInfo.set( "emSCHOOL", m_vecResultDataBH[i]->emSCHOOL );
		tbRankInfo.set( "emCLASS", m_vecResultDataBH[i]->emCLASS );
		tbRankInfo.set( "nCONTRIBUTION", m_vecResultDataBH[i]->nCONTRIBUTION );
		tbRankInfo.set( "nDAMAGE", m_vecResultDataBH[i]->nDAMAGE );
		tbRankInfo.set( "nHEAL", m_vecResultDataBH[i]->nHEAL );
		tbRankInfo.set( "nKILL", m_vecResultDataBH[i]->nKILL );
		tbRankInfo.set( "nDEATH", m_vecResultDataBH[i]->nDEATH );
		tbRankInfo.set( "nREBIRTH", m_vecResultDataBH[i]->nREBIRTH );

		tbResult.set( i + 1 , tbRankInfo );
	}

	return tbResult;
}

lua_tinker::table GLCaptureTheFieldClient::GetMyResult(void) const
{
	lua_tinker::table tbRankInfo( GLWidgetScript::GetInstance().GetLuaState() );

	tbRankInfo.set( "nRANK", m_MyResultRanking );
	tbRankInfo.set( "strNAME", m_MyResultData.strNAME );
	tbRankInfo.set( "emSCHOOL", m_MyResultData.emSCHOOL );
	tbRankInfo.set( "emCLASS", m_MyResultData.emCLASS );
	tbRankInfo.set( "nCONTRIBUTION", m_MyResultData.nCONTRIBUTION );
	tbRankInfo.set( "nDAMAGE", m_MyResultData.nDAMAGE );
	tbRankInfo.set( "nHEAL", m_MyResultData.nHEAL );
	tbRankInfo.set( "nKILL", m_MyResultData.nKILL );
	tbRankInfo.set( "nDEATH", m_MyResultData.nDEATH );
	tbRankInfo.set( "nREBIRTH", m_MyResultData.nREBIRTH );
	
	return tbRankInfo;
}

void GLCaptureTheFieldClient::ReqCTFieldQuit(void) const
{
	m_pGaeaClient->ReqCaptureTheFieldQuit();
}
//Scleform Funtion End