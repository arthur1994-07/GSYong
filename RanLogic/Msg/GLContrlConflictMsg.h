#pragma once

#include "GLContrlBaseMsg.h"

namespace GLMSG
{
	#pragma pack(1)

	//---------------------------------------------------------------------------NET
    struct SNETPC_REQ_CONFRONT : public NET_MSG_GENERIC
	{
		EMCONFT_TYPE		emTYPE;
		DWORD				dwID;

		SCONFT_OPTION		sOption;

		SNETPC_REQ_CONFRONT ( EMCONFT_TYPE _emType, DWORD _dwID, SCONFT_OPTION _sOption ) 
			: emTYPE(_emType)
			, dwID(_dwID)
            , sOption (_sOption)
		{
			dwSize = sizeof(SNETPC_REQ_CONFRONT);
			nType = NET_MSG_GCTRL_CONFRONT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_CONFRONT)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_CONFRONT_TAR : public NET_MSG_GENERIC
	{
		EMCONFT_TYPE		emTYPE;
		DWORD				dwID;
		DWORD				dwTargetID;

		char				szMasterName[CHAR_SZNAME];
		EMVEHICLE_WHETHER	emVehicleState;
		SCONFT_OPTION		sOption;

		SNETPC_REQ_CONFRONT_TAR ( EMCONFT_TYPE _emType, DWORD _dwID, DWORD _dwTargetID, const std::string& _strMasterName, SCONFT_OPTION _sOption ) 
			: emTYPE(_emType)
			, dwID(_dwID)
			, dwTargetID(_dwTargetID)
			, emVehicleState(EMVEHICLE_OFF)
            , sOption(_sOption)
		{
			StringCchCopy(szMasterName, CHAR_SZNAME, _strMasterName.c_str());

			dwSize = sizeof(SNETPC_REQ_CONFRONT_TAR);
			nType = NET_MSG_GCTRL_CONFRONT_TAR;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_CONFRONT_TAR)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_CONFRONT_ANS : public NET_MSG_GENERIC
	{
		EMCONFT_TYPE		emTYPE;
		DWORD				dwID;
		EMCONFRONT_FB		emFB;

		SCONFT_OPTION		sOption;

		SNETPC_REQ_CONFRONT_ANS ( EMCONFT_TYPE _emType, DWORD _dwID, EMCONFRONT_FB _emFB, SCONFT_OPTION _sOption ) 
			: emTYPE(_emType)
			, dwID(_dwID)
			, emFB(_emFB)
            , sOption(_sOption)
		{
			dwSize = sizeof(SNETPC_REQ_CONFRONT_ANS);
			nType = NET_MSG_GCTRL_CONFRONT_ANS;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_CONFRONT_ANS)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_CONFRONT_FB : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		EMCONFT_TYPE		emTYPE;
		DWORD				dwID;
		EMCONFRONT_FB		emFB;
		float				fTIME;

		SNETPC_REQ_CONFRONT_FB (EMCONFT_TYPE _emTYPE, DWORD _dwID, EMCONFRONT_FB _emFB, float _fTIME) 
			: emTYPE(_emTYPE)
			, dwID(_dwID)
			, emFB(_emFB)
			, fTIME(_fTIME)
		{
			dwSize = sizeof(SNETPC_REQ_CONFRONT_FB);
			nType = NET_MSG_GCTRL_CONFRONT_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_CONFRONT_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CONFRONT_START2_FLD : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		EMCONFT_TYPE		emTYPE;
		DWORD				dwTARID;

		SCONFT_OPTION		sOption;

		SNETPC_CONFRONT_START2_FLD ( EMCONFT_TYPE _emTYPE, DWORD _dwTARID, SCONFT_OPTION _sOption ) 
			: emTYPE(_emTYPE)
			, dwTARID(_dwTARID)
            , sOption(_sOption)
		{
			dwSize = sizeof(SNETPC_CONFRONT_START2_FLD);
			nType = NET_MSG_GCTRL_CONFRONT_START2_FLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONT_START2_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CONFRONT_START2_CLT : public NET_MSG_GENERIC
	{
		EMCONFT_TYPE		emTYPE;
		DWORD				dwTARID;

		D3DXVECTOR3			vPosition;
		SCONFT_OPTION		sOption;

		char				szMasterName[CHAR_SZNAME];

		SNETPC_CONFRONT_START2_CLT (EMCONFT_TYPE _emType, DWORD _dwTARID, D3DXVECTOR3 _vPosition, SCONFT_OPTION _sOption, const std::string& _strMasterName ) 
			: emTYPE(_emType)
			, dwTARID(_dwTARID)
			, vPosition(_vPosition)
            , sOption(_sOption)
		{
			StringCchCopy(szMasterName, CHAR_SZNAME, _strMasterName.c_str());

			dwSize = sizeof(SNETPC_CONFRONT_START2_CLT);
			nType = NET_MSG_GCTRL_CONFRONT_START2_CLT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONT_START2_CLT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CONFRONT_FIGHT2_CLT : public NET_MSG_GENERIC
	{
		SNETPC_CONFRONT_FIGHT2_CLT ()
		{
			dwSize = sizeof(SNETPC_CONFRONT_FIGHT2_CLT);
			nType = NET_MSG_GCTRL_CONFRONT_FIGHT2_CLT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONT_FIGHT2_CLT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CONFRONT_END2_FLD : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		EMCONFRONT_END		emEND;

		SNETPC_CONFRONT_END2_FLD( EMCONFRONT_END _emEND ) 
			: emEND(_emEND)
		{
			dwSize = sizeof(SNETPC_CONFRONT_END2_FLD);
			nType = NET_MSG_GCTRL_CONFRONT_END2_FLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONT_END2_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CONFRONT_END2_CLT : public NET_MSG_GENERIC
	{
		EMCONFRONT_END		emEND;

		SNETPC_CONFRONT_END2_CLT( EMCONFRONT_END _emEND ) 
			: emEND(_emEND)
		{
			dwSize = sizeof(SNETPC_CONFRONT_END2_CLT);
			nType = NET_MSG_GCTRL_CONFRONT_END2_CLT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONT_END2_CLT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CONFRONT_END2_CLT_MBR : public NET_MSG_GENERIC
	{
		DWORD				dwID;

		SNETPC_CONFRONT_END2_CLT_MBR( DWORD _dwID ) 
			: dwID(_dwID)
		{
			dwSize = sizeof(SNETPC_CONFRONT_END2_CLT_MBR);
			nType = NET_MSG_GCTRL_CONFRONT_END2_CLT_MBR;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONT_END2_CLT_MBR)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CONFRONT_END2_CLT_BRD : public SNETPC_BROAD
	{
		EMCONFRONT_END		emEND;

		SNETPC_CONFRONT_END2_CLT_BRD(EMCONFRONT_END _emEND) 
            : emEND(_emEND)
		{
			dwSize = sizeof(SNETPC_CONFRONT_END2_CLT_BRD);
			nType = NET_MSG_GCTRL_CONFRONT_END2_CLT_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONT_END2_CLT_BRD)<=NET_DATA_BUFSIZE);
		}   
	};

	struct SNETPC_CONFRONT_END2_AGT : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		EMCONFRONT_END		emEND;

		SNETPC_CONFRONT_END2_AGT(EMCONFRONT_END _emEND) 
			: emEND(_emEND)
		{
			dwSize = sizeof(SNETPC_CONFRONT_END2_AGT);
			nType = NET_MSG_GCTRL_CONFRONT_END2_AGT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONT_END2_AGT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CONFRONTPTY_CHECKMBR2_FLD : public NET_MSG_GENERIC
	{
		SNATIVEID			sMapID;

		GLPartyID			PartyID_A;
		GLPartyID			PartyID_B;

		SCONFT_OPTION		sOption;

		SNETPC_CONFRONTPTY_CHECKMBR2_FLD( SNATIVEID _sMapID, GLPartyID _PartyID_A, GLPartyID _PartyID_B, SCONFT_OPTION _sOption ) 
			: sMapID(_sMapID)
			, PartyID_A(_PartyID_A)
			, PartyID_B(_PartyID_B)
            , sOption(_sOption)
		{
			dwSize = sizeof(SNETPC_CONFRONTPTY_CHECKMBR2_FLD);
			nType = NET_MSG_GCTRL_CONFRONTPTY_CHECKMBR_FLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONTPTY_CHECKMBR2_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CONFRONTPTY_CHECKMBR2_AGT : public NET_MSG_GENERIC
	{
		SNATIVEID			sMapID;

		D3DXVECTOR3		vPosition;
		GLPartyID			PartyID_A;
		GLPartyID			PartyID_B;

		DWORD				dwPARTY_A_MEMBER[MAXPARTY];
		DWORD				dwPARTY_B_MEMBER[MAXPARTY];

		SCONFT_OPTION		sOption;

		SNETPC_CONFRONTPTY_CHECKMBR2_AGT( SNATIVEID _sMapID, D3DXVECTOR3 _vPosition, GLPartyID _PartyID_A, GLPartyID _PartyID_B, SCONFT_OPTION _sOption ) 
			: sMapID(_sMapID)
			, vPosition(_vPosition)
			, PartyID_A(_PartyID_A)
			, PartyID_B(_PartyID_B)
            , sOption(_sOption)
		{
			dwSize = sizeof(SNETPC_CONFRONTPTY_CHECKMBR2_AGT);
			nType = NET_MSG_GCTRL_CONFRONTPTY_CHECKMBR_AGT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONTPTY_CHECKMBR2_AGT)<=NET_DATA_BUFSIZE);

			memset(dwPARTY_A_MEMBER, 0xff, sizeof(DWORD)*MAXPARTY );
			memset(dwPARTY_B_MEMBER, 0xff, sizeof(DWORD)*MAXPARTY );
		}
	};
 
	struct SNETPC_CONFRONTPTY_START2_FLD : public NET_MSG_GENERIC
	{
		SNATIVEID			sMapID;

		GLPartyID		PartyID_A;
		GLPartyID		PartyID_B;

		DWORD				dwPARTY_A_MEMBER[MAXPARTY];
		DWORD				dwPARTY_B_MEMBER[MAXPARTY];

		D3DXVECTOR3			vPos;
		SCONFT_OPTION		sOption;

		SNETPC_CONFRONTPTY_START2_FLD ( SNATIVEID _sMapID, GLPartyID _PartyID_A, GLPartyID _PartyID_B, D3DXVECTOR3 _vPosition, SCONFT_OPTION _sOption ) 
			: sMapID(_sMapID)
			, PartyID_A(_PartyID_A)
			, PartyID_B(_PartyID_B)
			, vPos(_vPosition)
            , sOption(_sOption)
		{
			dwSize = sizeof(SNETPC_CONFRONTPTY_START2_FLD);
			nType = NET_MSG_GCTRL_CONFRONTPTY_START2_FLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONTPTY_START2_FLD)<=NET_DATA_BUFSIZE);

			memset(dwPARTY_A_MEMBER, 0xff, sizeof(DWORD)*MAXPARTY );
			memset(dwPARTY_B_MEMBER, 0xff, sizeof(DWORD)*MAXPARTY );
		}
	};

	struct SNETPC_CONFRONTPTY_START2_CLT : public NET_MSG_GENERIC
	{
		CHAR				szName[CHR_ID_LENGTH+1];

		GLPartyID			ConfrontPartyID;
		DWORD				dwCONFRONT_MY_MEMBER[MAXPARTY];
		DWORD				dwCONFRONT_TAR_MEMBER[MAXPARTY];

		D3DXVECTOR3			vPosition;
		SCONFT_OPTION		sOption;

		SNETPC_CONFRONTPTY_START2_CLT ( GLPartyID _ConfrontPartyID, D3DXVECTOR3 _vPosition, SCONFT_OPTION _sOption ) 
			: ConfrontPartyID(_ConfrontPartyID)
			, vPosition(_vPosition)
            , sOption(_sOption)
		{
			dwSize = sizeof(SNETPC_CONFRONTPTY_START2_CLT);
			nType = NET_MSG_GCTRL_CONFRONTPTY_START2_CLT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONTPTY_START2_CLT)<=NET_DATA_BUFSIZE);

            memset(szName, 0, sizeof(CHAR) * (CHR_ID_LENGTH+1));
        }
	};

	struct SNETPC_CONFRONTPTY_END2_FLD : public NET_MSG_GENERIC
	{
		GLPartyID		PartyID_A;
		GLPartyID		PartyID_B;

		EMCONFRONT_END		emEND_A;
		EMCONFRONT_END		emEND_B;

		SNETPC_CONFRONTPTY_END2_FLD( GLPartyID _PartyID_A, GLPartyID _PartyID_B, EMCONFRONT_END _emEND_A, EMCONFRONT_END _emEND_B ) 
			: PartyID_A(_PartyID_A)
			, PartyID_B(_PartyID_B)
			, emEND_A(_emEND_A)
			, emEND_B(_emEND_B)
		{
			dwSize = sizeof(SNETPC_CONFRONTPTY_END2_FLD);
			nType = NET_MSG_GCTRL_CONFRONTPTY_END2_FLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONTPTY_END2_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CONFRONTPTY_END2_CLT : public NET_MSG_GENERIC
	{
		EMCONFRONT_END		emEND;

		SNETPC_CONFRONTPTY_END2_CLT( EMCONFRONT_END _emEND ) 
			: emEND(_emEND)
		{
			dwSize = sizeof(SNETPC_CONFRONTPTY_END2_CLT);
			nType = NET_MSG_GCTRL_CONFRONTPTY_END2_CLT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONTPTY_END2_CLT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CONFRONT_RECOVE : public NET_MSG_GENERIC
	{
		WORD				wRECOVER;

		SNETPC_CONFRONT_RECOVE( WORD _wRECOVER ) 
			: wRECOVER(_wRECOVER)
		{
			dwSize = sizeof(SNETPC_CONFRONT_RECOVE);
			nType = NET_MSG_GCTRL_CONFRONT_RECOVE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONT_RECOVE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CONFRONTCLB_CHECKMBR_FLD : public NET_MSG_GENERIC
	{
		SNATIVEID			sMapID;

		DWORD				dwCLUB_A;
		DWORD				dwCLUB_B;

		SCONFT_OPTION		sOption;

		SNET_CONFRONTCLB_CHECKMBR_FLD ( SNATIVEID _sMapID, DWORD _dwCLUB_A,	DWORD _dwCLUB_B, SCONFT_OPTION _sOption ) 
			: sMapID(_sMapID)
            , dwCLUB_A(_dwCLUB_A)
			, dwCLUB_B(_dwCLUB_B)
            , sOption(_sOption)
		{
			dwSize = sizeof(SNET_CONFRONTCLB_CHECKMBR_FLD);
			nType = NET_MSG_GCTRL_CONFRONTCLB_CHECKMBR_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CONFRONTCLB_CHECKMBR_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CONFRONTCLB_CHECKMBR_AGT : public NET_MSG_GENERIC
	{
		SNATIVEID			sMapID;

		D3DXVECTOR3			vPosition;
		DWORD				dwCLUB_A;
		DWORD				dwCLUB_B;

		DWORD				dwCLUB_A_NUM;
		DWORD				dwCLUB_A_MEMBER[EMMAX_CLUB_NUM];
		
		DWORD				dwCLUB_B_NUM;
		DWORD				dwCLUB_B_MEMBER[EMMAX_CLUB_NUM];

		SCONFT_OPTION		sOption;

		SNET_CONFRONTCLB_CHECKMBR_AGT( SNATIVEID _sMapID, D3DXVECTOR3 _vPosition, DWORD _dwCLUB_A, DWORD _dwCLUB_B, SCONFT_OPTION _sOption ) 
			: sMapID(_sMapID) 
            , vPosition(_vPosition)
			, dwCLUB_A(_dwCLUB_A)
			, dwCLUB_B(_dwCLUB_B)
			, dwCLUB_A_NUM(0)
			, dwCLUB_B_NUM(0)
            , sOption(_sOption)
		{
			dwSize = sizeof(SNET_CONFRONTCLB_CHECKMBR_AGT);
			nType = NET_MSG_GCTRL_CONFRONTCLB_CHECKMBR_AGT;
			MIN_STATIC_ASSERT(sizeof(SNET_CONFRONTCLB_CHECKMBR_AGT)<=NET_DATA_BUFSIZE);
			memset(dwCLUB_A_MEMBER, 0, sizeof(DWORD) * EMMAX_CLUB_NUM);
			memset(dwCLUB_B_MEMBER, 0, sizeof(DWORD) * EMMAX_CLUB_NUM);
		}
	};

	struct SNETPC_CONFRONTCLB_START2_FLD : public NET_MSG_GENERIC
	{
		SNATIVEID			sMapID;

		DWORD				dwCLUB_A;
		DWORD				dwCLUB_B;

		DWORD				dwCLUB_A_NUM;
		DWORD				dwCLUB_A_MEMBER[EMMAX_CLUB_NUM];
		
		DWORD				dwCLUB_B_NUM;
		DWORD				dwCLUB_B_MEMBER[EMMAX_CLUB_NUM];

		D3DXVECTOR3			vPos;
		SCONFT_OPTION		sOption;

		SNETPC_CONFRONTCLB_START2_FLD( SNATIVEID _sMapID, DWORD _dwCLUB_A, DWORD _dwCLUB_B, D3DXVECTOR3 _vPosition, SCONFT_OPTION _sOption ) 
			: sMapID(_sMapID)
			, dwCLUB_A(_dwCLUB_A)
			, dwCLUB_B(_dwCLUB_B)
			, dwCLUB_A_NUM(0)
			, dwCLUB_B_NUM(0)   
            , vPos(_vPosition)
            , sOption(_sOption)

		{
			dwSize = sizeof(SNETPC_CONFRONTCLB_START2_FLD);
			nType = NET_MSG_GCTRL_CONFRONTCLB_START2_FLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONTCLB_START2_FLD)<=NET_DATA_BUFSIZE);

			memset(dwCLUB_A_MEMBER, 0, sizeof(DWORD) * EMMAX_CLUB_NUM);
			memset(dwCLUB_B_MEMBER, 0, sizeof(DWORD) * EMMAX_CLUB_NUM);
		}
	};

	struct SNETPC_CONFRONTCLB_START2_CLT : public NET_MSG_GENERIC
	{
		CHAR				szName[CHR_ID_LENGTH+1];

		DWORD				dwconftCLUB;

		DWORD				dwCLUB_MY_NUM;
		DWORD				dwCONFRONT_MY_MEMBER[EMMAX_CLUB_NUM];

		DWORD				dwCLUB_TAR_NUM;
		DWORD				dwCONFRONT_TAR_MEMBER[EMMAX_CLUB_NUM];

		D3DXVECTOR3			vPosition;
		SCONFT_OPTION		sOption;

		SNETPC_CONFRONTCLB_START2_CLT( DWORD _dwconftCLUB, D3DXVECTOR3 _vPosition, SCONFT_OPTION _sOption )
			: dwconftCLUB(_dwconftCLUB)
			, vPosition(_vPosition)
            , sOption(_sOption)
			, dwCLUB_MY_NUM(0)
			, dwCLUB_TAR_NUM(0)
		{
			dwSize = sizeof(SNETPC_CONFRONTCLB_START2_CLT);
			nType = NET_MSG_GCTRL_CONFRONTCLB_START2_CLT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONTCLB_START2_CLT)<=NET_DATA_BUFSIZE);
			memset(szName, 0, sizeof(CHAR) * (CHR_ID_LENGTH+1));
			memset(dwCONFRONT_MY_MEMBER, 0, sizeof(DWORD) * EMMAX_CLUB_NUM);
			memset(dwCONFRONT_TAR_MEMBER, 0, sizeof(DWORD) * EMMAX_CLUB_NUM);
		}
	};

	struct SNETPC_CONFRONTCLB_END2_FLD : public NET_MSG_GENERIC
	{
		DWORD				dwCLUB_A;
		DWORD				dwCLUB_B;

		EMCONFRONT_END		emEND_A;
		EMCONFRONT_END		emEND_B;

		SNETPC_CONFRONTCLB_END2_FLD( DWORD _dwCLUB_A, DWORD _dwCLUB_B, EMCONFRONT_END _emEND_A, EMCONFRONT_END _emEND_B ) 
			: dwCLUB_A(_dwCLUB_A)
			, dwCLUB_B(_dwCLUB_B)
			, emEND_A(_emEND_A)
			, emEND_B(_emEND_B)
		{
			dwSize = sizeof(SNETPC_CONFRONTCLB_END2_FLD);
			nType = NET_MSG_GCTRL_CONFRONTCLB_END2_FLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONTCLB_END2_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CONFRONTCLB_END2_CLT : public NET_MSG_GENERIC
	{
		EMCONFRONT_END		emEND;

		SNETPC_CONFRONTCLB_END2_CLT( EMCONFRONT_END _emEND ) 
			: emEND(_emEND)
		{
			dwSize = sizeof(SNETPC_CONFRONTCLB_END2_CLT);
			nType = NET_MSG_GCTRL_CONFRONTCLB_END2_CLT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CONFRONTCLB_END2_CLT)<=NET_DATA_BUFSIZE);
		}
	};

	// Revert to default structure packing
	#pragma pack()
};