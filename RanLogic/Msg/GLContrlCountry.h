#pragma once

#include "../Country/GLCountryDefine.h"

#include "GLContrlBaseMsg.h"

namespace GLMSG
{
	struct NET_COUNTRY_MESSAGE_HEADER : public NET_MSG_GENERIC
	{
		NET_COUNTRY_MESSAGE_HEADER ( const DWORD nSize
			, const EMNET_MSG msg )
			: NET_MSG_GENERIC ( msg, nSize )
		{

		}
	};

	// Change User Country Info;
	struct NET_COUNTRY_CHANGE_CA : public NET_COUNTRY_MESSAGE_HEADER
	{
		Country::SCOUNTRY_INFO m_sCountryInfo;

		NET_COUNTRY_CHANGE_CA ( Country::SCOUNTRY_INFO sCountryInfo )
			: NET_COUNTRY_MESSAGE_HEADER ( sizeof( NET_COUNTRY_CHANGE_CA )
			, NET_MSG_GCTRL_COUNTRY_CHANGE_CA )
			, m_sCountryInfo ( sCountryInfo )
		{
		}
	};

	// Setting User Country Info ( Server );
	struct NET_COUNTRY_CHANGE_DA : public NET_COUNTRY_MESSAGE_HEADER
	{
		DWORD m_dwUserDbNum;

		Country::SCOUNTRY_INFO m_sCountryInfo;

		NET_COUNTRY_CHANGE_DA ( Country::SCOUNTRY_INFO sCountryInfo
			, DWORD dwUserDbNum )
			: NET_COUNTRY_MESSAGE_HEADER ( sizeof( NET_COUNTRY_CHANGE_DA )
			, NET_MSG_GCTRL_COUNTRY_CHANGE_DA )
			, m_sCountryInfo ( sCountryInfo )
			, m_dwUserDbNum ( dwUserDbNum )
		{
		}
	};

	// Setting User Country Info ( Client );
	struct NET_COUNTRY_CHANGE_AFC : public NET_COUNTRY_MESSAGE_HEADER
	{
		Country::SCOUNTRY_INFO m_sCountryInfo;

		NET_COUNTRY_CHANGE_AFC ( Country::SCOUNTRY_INFO sCountryInfo )
			: NET_COUNTRY_MESSAGE_HEADER ( sizeof( NET_COUNTRY_CHANGE_AFC )
			, NET_MSG_GCTRL_COUNTRY_CHANGE_AFC )
			, m_sCountryInfo ( sCountryInfo )
		{
		}
	};



	/************************************************************************/
	/* Victorious Country Message                                           */
	/************************************************************************/
	struct NET_VCOUNTRY_ADD_FAC : public NET_COUNTRY_MESSAGE_HEADER
	{
		Country::SVICTORIOUS_COUNTRY m_sVCountry;
		bool m_bTempVCountry;

		NET_VCOUNTRY_ADD_FAC ( Country::SVICTORIOUS_COUNTRY sVCountry )
			: NET_COUNTRY_MESSAGE_HEADER ( sizeof( NET_VCOUNTRY_ADD_FAC )
			, NET_MSG_GCTRL_VCOUNTRY_ADD_FAC )
			, m_sVCountry ( sVCountry )
			, m_bTempVCountry ( false )
		{
		}
	};

	struct NET_VCOUNTRY_ADD_DAF : public NET_COUNTRY_MESSAGE_HEADER
	{
		Country::SVICTORIOUS_COUNTRY m_sVCountry;

		NET_VCOUNTRY_ADD_DAF ( Country::SVICTORIOUS_COUNTRY sVCountry )
			: NET_COUNTRY_MESSAGE_HEADER ( sizeof( NET_VCOUNTRY_ADD_DAF )
			, NET_MSG_GCTRL_VCOUNTRY_ADD_DAF )
			, m_sVCountry ( sVCountry )
		{
		}
	};

	struct NET_VCOUNTRY_ADD_AA : public NET_COUNTRY_MESSAGE_HEADER
	{
		Country::SVICTORIOUS_COUNTRY m_sVCountry;

		NET_VCOUNTRY_ADD_AA ( Country::SVICTORIOUS_COUNTRY sVCountry )
			: NET_COUNTRY_MESSAGE_HEADER ( sizeof( NET_VCOUNTRY_ADD_AA )
			, NET_MSG_GCTRL_VCOUNTRY_ADD_AA )
			, m_sVCountry ( sVCountry )
		{
		}
	};

	struct NET_VCOUNTRY_RESET_FAC : public NET_COUNTRY_MESSAGE_HEADER
	{
		Country::EMCOUNTRY_SYSTEM_FLAG m_emFlag;
		bool m_bTempVCountry;

		NET_VCOUNTRY_RESET_FAC ( Country::EMCOUNTRY_SYSTEM_FLAG emFlag )
			: NET_COUNTRY_MESSAGE_HEADER ( sizeof( NET_VCOUNTRY_RESET_FAC )
			, NET_MSG_GCTRL_VCOUNTRY_RESET_FAC )
			, m_emFlag ( emFlag )
			, m_bTempVCountry ( false )
		{
		}
	};

	struct NET_VCOUNTRY_RESET_DAF : public NET_COUNTRY_MESSAGE_HEADER
	{
		Country::EMCOUNTRY_SYSTEM_FLAG m_emFlag;

		NET_VCOUNTRY_RESET_DAF ( Country::EMCOUNTRY_SYSTEM_FLAG emFlag )
			: NET_COUNTRY_MESSAGE_HEADER ( sizeof( NET_VCOUNTRY_RESET_DAF )
			, NET_MSG_GCTRL_VCOUNTRY_RESET_DAF )
			, m_emFlag ( emFlag )
		{
		}
	};

	struct NET_VCOUNTRY_RESET_AA : public NET_COUNTRY_MESSAGE_HEADER
	{
		Country::EMCOUNTRY_SYSTEM_FLAG m_emFlag;

		NET_VCOUNTRY_RESET_AA ( Country::EMCOUNTRY_SYSTEM_FLAG emFlag )
			: NET_COUNTRY_MESSAGE_HEADER ( sizeof( NET_VCOUNTRY_RESET_AA )
			, NET_MSG_GCTRL_VCOUNTRY_RESET_AA )
			, m_emFlag ( emFlag )
		{
		}
	};

	struct NET_VCOUNTRY_SYNC_AC : public NET_COUNTRY_MESSAGE_HEADER
	{
		Country::SVICTORIOUS_COUNTRY m_sVictoriousCountry;

		NET_VCOUNTRY_SYNC_AC ( Country::SVICTORIOUS_COUNTRY sVictoriousCountry )
			: NET_COUNTRY_MESSAGE_HEADER ( sizeof( NET_VCOUNTRY_SYNC_AC )
			, NET_MSG_GCTRL_VCOUNTRY_SYNC_AC )
			, m_sVictoriousCountry ( sVictoriousCountry )
		{
		}
	};


	/************************************************************************/
	/* Use Initialize Country Item                                          */
	/************************************************************************/
	struct NET_USE_ITEM_INITIALIZE_COUNTRY_CF : public NET_COUNTRY_MESSAGE_HEADER
	{	
		WORD m_wPosX;
		WORD m_wPosY;

		NET_USE_ITEM_INITIALIZE_COUNTRY_CF ()
			: NET_COUNTRY_MESSAGE_HEADER ( sizeof( NET_USE_ITEM_INITIALIZE_COUNTRY_CF )
			, NET_MSG_GCTRL_INITIALIZE_COUNTRY_CF )
			, m_wPosX ( 0 )
			, m_wPosY ( 0 )
		{
		}
	};

	struct NET_USE_ITEM_INITIALIZE_COUNTRY_FA : public NET_COUNTRY_MESSAGE_HEADER
	{	
		DWORD m_dwUserDbNum;

		NET_USE_ITEM_INITIALIZE_COUNTRY_FA ()
			: NET_COUNTRY_MESSAGE_HEADER ( sizeof( NET_USE_ITEM_INITIALIZE_COUNTRY_FA )
			, NET_MSG_GCTRL_INITIALIZE_COUNTRY_FA )
			, m_dwUserDbNum ( 0 )
		{
		}
	};


	/************************************************************************/
	/* Show Country Mark                                                    */
	/************************************************************************/
	struct NET_SHOW_COUNTRY_MARK_FC : public NET_COUNTRY_MESSAGE_HEADER
	{	
		bool m_bShowCountryMark;

		NET_SHOW_COUNTRY_MARK_FC ()
			: NET_COUNTRY_MESSAGE_HEADER ( sizeof( NET_SHOW_COUNTRY_MARK_FC )
			, NET_MSG_GCTRL_SHOW_COUNTRY_MARK_FC )
			, m_bShowCountryMark ( false )
		{
		}
	};
}