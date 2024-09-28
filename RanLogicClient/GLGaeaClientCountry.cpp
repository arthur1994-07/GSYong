#include "pch.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include "../RanLogic/Msg/GLContrlCountry.h"

#include "./Country/GLCountryManClient.h"
#include "./Country/GLVictoriousCountryManClient.h"
#include "./Char/CharNameDbNumCache.h"
#include "./Land/GLLandManClient.h"

#include "./GfxInterfaceBase.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaClient::SetCountryInfo ( Country::SCOUNTRY_INFO sCountryInfo )
{
	// �������� �ʴ� ������� �������� �ʴ´�;
	if ( !Country::GLCountryManClient::Instance()->ExistCountry(
		sCountryInfo.wContinentID,
		sCountryInfo.wCountryID ) )
	{
		m_sCountryInfo.RESET();

		return;
	}

	// ����;
	m_sCountryInfo = sCountryInfo;
}

Country::GLCountry*	GLGaeaClient::GetClientCountry () const
{
	Country::GLCountry* pReturnCountry = NULL;

	Country::GLCountryManClient* pCountryMan =
		Country::GLCountryManClient::Instance();
	if ( pCountryMan )
	{
		pReturnCountry =
			pCountryMan->GetCountry(
			m_sCountryInfo.wContinentID,
			m_sCountryInfo.wCountryID );
	}

	return pReturnCountry;
}

bool GLGaeaClient::IsSelectCountry()
{
	return (!m_sCountryInfo.ISNONCOUNTRY());
}

DWORD GLGaeaClient::GetSelectedCountryID()
{
	if( true == m_sCountryInfo.ISNONCOUNTRY() )
		return -1;

	return m_sCountryInfo.wCountryID;
}

std::string GLGaeaClient::GetSelectedCountryName()
{
	Country::GLCountryManClient* pCountryMan = Country::GLCountryManClient::Instance();
	if( NULL == pCountryMan )
		return "";

	Country::GLCountry* pCountry = pCountryMan->GetCountry(
		m_sCountryInfo.wContinentID, m_sCountryInfo.wCountryID );
	if( NULL == pCountry )
		return "";

	return pCountry->GetCountryName();
}

bool GLGaeaClient::LoadCountryFile ()
{
	if ( m_pCountryMan )
		return m_pCountryMan->LoadData();

	return false;
}

void GLGaeaClient::MsgVCountryAddFAC ( NET_MSG_GENERIC* pMsg )
{
	Country::GLVictoriousCountryManClient* pVCountryMan =
		Country::GLVictoriousCountryManClient::Instance();

	if ( pVCountryMan )
		pVCountryMan->MsgVCountryADD( pMsg );

	// ���籺�� �¸����� ������ ��� �̰����� �����Ѵ�;
	GLMSG::NET_VCOUNTRY_ADD_DAF* pAddMsg =
		static_cast< GLMSG::NET_VCOUNTRY_ADD_DAF* >( pMsg );
	if ( pAddMsg )
	{
		if ( pAddMsg->m_sVCountry.emFlag == Country::CAPTURE_THE_ISLANDS )
		{
			Country::GLVictoriousCountryManClient* pVCountryMan =
				Country::GLVictoriousCountryManClient::Instance();
			if ( pVCountryMan )
			{
				const TSTRING strFlagName =
					pVCountryMan->GetVCountryFileNameFirst(
					Country::CAPTURE_THE_ISLANDS );

				if ( false == strFlagName.empty() &&
					m_pLandMClient )
				{
					m_pLandMClient->SetWorldBattleFlag(
						strFlagName.c_str() );
				}

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_CTI_UpdateVictoriousCountry,
					"-n, -s",
					pAddMsg->m_sVCountry.sCountryInfo.wCountryID,
					pVCountryMan->GetVCountryNameFirst( Country::CAPTURE_THE_ISLANDS ).c_str() );
			}
		}
	}
}

void GLGaeaClient::MsgVCountryResetFAC ( NET_MSG_GENERIC* pMsg )
{
	Country::GLVictoriousCountryManClient* pVCountryMan =
		Country::GLVictoriousCountryManClient::Instance();

	if ( pVCountryMan )
		pVCountryMan->MsgVCountryReset( pMsg );

	// ���籺�� �¸����� �ʱ�ȭ �� ��� �̰����� �����Ѵ�;	
	if ( m_pLandMClient )
		m_pLandMClient->SetWorldBattleFlag( NULL );

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_CTI_UpdateVictoriousCountry,
		"-n, -s", -1, "" );
}

void GLGaeaClient::MsgNameDisplayShowCountryMark ( NET_MSG_GENERIC* pMsg )
{
	GLMSG::NET_SHOW_COUNTRY_MARK_FC* pCountryMsg =
		static_cast< GLMSG::NET_SHOW_COUNTRY_MARK_FC* >( pMsg );

	bool bShowCountry = true;
	
	if ( pCountryMsg )
		bShowCountry = pCountryMsg->m_bShowCountryMark;

	GfxInterfaceBase::GetInstance()->SetDisplayCountryMark( bShowCountry );
}
