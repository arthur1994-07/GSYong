#pragma once


#include "GLContrlBaseMsg.h"

enum EMSKILLHOLDRESET
{
	EMSKILLHOLDRESET_EFFECT       = 0,
	EMSKILLHOLDRESET_REQ          = 1,
    EMSKILLHOLDRESET_MOVEFIELDSRV = 2,
    EMSKILLHOLDRESET_SYSTEM       = 3,
};

enum EMSKILL_REVIVE_FB
{
    EMSKILL_REVIVE_FB_OK        = 0, //  성공
    EMSKILL_REVIVE_FB_FAIL      = 1, //  부활 오류
    EMSKILL_REVIVE_FB_NOTVALUE  = 2, //  부활정보 없음
};

namespace GLMSG
{
	#pragma pack(1)

	struct SNETPC_REQ_SKILLQUICK_SET : public NET_MSG_GENERIC
	{
		
		WORD				wSLOT;
		SNATIVEID			skill_id;

		SNETPC_REQ_SKILLQUICK_SET () :
			wSLOT(0)
		{
			dwSize = sizeof(SNETPC_REQ_SKILLQUICK_SET);
			nType = NET_MSG_GCTRL_REQ_SKILLQ_SET;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SKILLQUICK_SET)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_SKILLQUICK_RESET : public NET_MSG_GENERIC
	{
		
		WORD				wSLOT;

		SNETPC_REQ_SKILLQUICK_RESET () :
			wSLOT(0)
		{
			dwSize = sizeof(SNETPC_REQ_SKILLQUICK_RESET);
			nType = NET_MSG_GCTRL_REQ_SKILLQ_RESET;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SKILLQUICK_RESET)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_SKILLQUICK_FB : public NET_MSG_GENERIC
	{
		
		WORD				wSLOT;
		SNATIVEID			skill_id;

		SNETPC_REQ_SKILLQUICK_FB () :
			wSLOT(0)
		{
			dwSize = sizeof(SNETPC_REQ_SKILLQUICK_FB);
			nType = NET_MSG_GCTRL_REQ_SKILLQ_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SKILLQUICK_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_SKILLQUICK_ACTIVE : public NET_MSG_GENERIC
	{		
		WORD wSLOT;
        unsigned int Crc32;

		SNETPC_REQ_SKILLQUICK_ACTIVE(WORD Slot=0)
            : wSLOT(Slot)
            , Crc32(0)
		{
			dwSize = sizeof(SNETPC_REQ_SKILLQUICK_ACTIVE);
			nType = NET_MSG_GCTRL_REQ_SKILLQ_ACTIVE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SKILLQUICK_ACTIVE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_ACTIONQUICK_SET : public NET_MSG_GENERIC
	{
		
		WORD				wSLOT;
		WORD				wACT;

        WORD                wHoldPosX;
        WORD                wHoldPosY;

		SNETPC_REQ_ACTIONQUICK_SET ()
            : wSLOT(0)
			, wACT(EMACT_SLOT_DRUG)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_ACTIONQ_SET;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_ACTIONQUICK_SET)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_ACTIONQUICK_RESET : public NET_MSG_GENERIC
	{
		
		WORD				wSLOT;

		SNETPC_REQ_ACTIONQUICK_RESET () :
			wSLOT(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_ACTIONQ_RESET;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_ACTIONQUICK_RESET)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_ACTIONQUICK_FB : public NET_MSG_GENERIC
	{
		

		WORD				wSLOT;
		SACTION_SLOT		sACT;

		SNETPC_REQ_ACTIONQUICK_FB () :
			wSLOT(0)
		{
			dwSize = sizeof(SNETPC_REQ_ACTIONQUICK_FB);
			nType = NET_MSG_GCTRL_REQ_ACTIONQ_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_ACTIONQUICK_FB)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_SKILL_CANCEL : public NET_MSG_GENERIC
	{
		SNETPC_SKILL_CANCEL ()
		{
			dwSize = sizeof(SNETPC_SKILL_CANCEL);
			nType = NET_MSG_GCTRL_SKILL_CANCEL;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SKILL_CANCEL)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_SKILL_CANCEL_BRD : public SNETPC_BROAD
	{
		SNETPC_SKILL_CANCEL_BRD ()
		{
			dwSize = sizeof(SNETPC_SKILL_CANCEL_BRD);
			nType = NET_MSG_GCTRL_SKILL_CANCEL_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SKILL_CANCEL_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_SKILLUP : public NET_MSG_GENERIC
	{
		
		SNATIVEID			skill_id;

		SNETPC_REQ_SKILLUP () :
			skill_id(NATIVEID_NULL())
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_SKILLUP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SKILLUP)<=NET_DATA_BUFSIZE);
		}
	};

    struct SNETPC_REQ_RNSKILLUP : public NET_MSG_GENERIC
    {
    public:        
        enum
        {
            EMHEAD_SIZE = ( sizeof(NET_MSG_GENERIC) + sizeof( unsigned int ) ),
            EMMAX_SIZE = NET_DATA_BUFSIZE - EMHEAD_SIZE,
            EMMAX_COUNT = EMMAX_SIZE / sizeof( SNATIVEID ),
        };
        
        unsigned int        nCount;
        BYTE                bySkillID[EMMAX_SIZE];
        
        SNETPC_REQ_RNSKILLUP () :
        nCount(0)
        {
            //dwSize = sizeof(EMMAX_SIZE);
            dwSize = sizeof(*this);
            nType = NET_MSG_GCTRL_REQ_RNSKILLUP;
            MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_RNSKILLUP)<=NET_DATA_BUFSIZE);
        }

        bool AddSkillID( SNATIVEID sSkillID )
        {
            WORD wSIZE = sizeof(sSkillID);

            // 추가할 공간이 있는지 검사
            if( EMMAX_SIZE <= wSIZE * nCount )
            {
                return false;
            }
            else
            {
                memcpy( bySkillID + ( wSIZE * nCount ), &sSkillID, wSIZE );
                //dwSize += wSIZE;
                nCount++;
            }
            return true;
        }
        
        unsigned int GetCount()
        {
            return nCount;
        }


        SNATIVEID GetSkillID( unsigned int nIndex )
        {
            SNATIVEID sSkillID = NATIVEID_NULL();
            if( nIndex < nCount )
            {
                WORD wSIZE = sizeof(sSkillID);
                memcpy( &sSkillID, bySkillID + ( wSIZE * nIndex ), wSIZE );
            }
            return sSkillID;
            
        }


    };

	struct SNETPC_REQ_SKILLUP_FB : public NET_MSG_GENERIC
	{
		

		SCHARSKILL			sSkill;
		EMSKILL_LEARNCHECK	emCHECK;

		SNETPC_REQ_SKILLUP_FB ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_SKILLUP_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SKILLUP_FB)<=NET_DATA_BUFSIZE);
		}
	};	

	struct SNETPC_REQ_RNSKILLUP_COMPLETE : public NET_MSG_GENERIC
	{
		enum { FLG_LEARN = 1, FLG_LEVELUP = 2, };
		DWORD dwComplete;
		SNETPC_REQ_RNSKILLUP_COMPLETE()
			: dwComplete(0)
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_GCTRL_REQ_RNSKILLUP_COMPLETE;
			MIN_STATIC_ASSERT( sizeof( SNETPC_REQ_RNSKILLUP_COMPLETE ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_REQ_SKILL : public NET_MSG_GENERIC
	{
		SNATIVEID			skill_id;

		EMCROW				emTarCrow;
		DWORD				dwTarID;
		D3DXVECTOR3			vSelectTargetPosition;

		D3DXVECTOR3			vTargetPosition;
		bool				bDefenseSkill;			// 발동 스킬

		BYTE				btTARNUM;
		BYTE				btSECONDTARNUM;
		STARID				sTARIDS[EMTARGET_NET<<1];



		SNETPC_REQ_SKILL () :
			skill_id(NATIVEID_NULL()),
			vTargetPosition(0,0,0),
			btTARNUM(0),
			btSECONDTARNUM(0),
			bDefenseSkill(false),
			emTarCrow(CROW_PC),
			dwTarID(-1),
			vSelectTargetPosition(0,0,0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_SKILL;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SKILL)<=NET_DATA_BUFSIZE);
		}

		void CALCSIZE ()
		{
			dwSize  = sizeof(*this) - sizeof(STARID) * ( EMTARGET_NET - btTARNUM );
			dwSize += sizeof(STARID) * btSECONDTARNUM;
			//MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SKILL)<=NET_DATA_BUFSIZE);
		}

		BOOL ADDTARGET ( const STARID& starget )
		{
			if( btTARNUM == EMTARGET_NET )
				return FALSE;

			GASSERT( btSECONDTARNUM == 0 && "기본타겟들을 모두 추가한 다음에 second target들을 모두적용해야한다." );

			sTARIDS[btTARNUM++] = starget;
			CALCSIZE();

			return TRUE;
		}

		BOOL ADDSECONDTARGET ( const STARID& starget )
		{
			if( btSECONDTARNUM == EMTARGET_NET )
				return FALSE;

			sTARIDS[btTARNUM + btSECONDTARNUM] = starget;
			btSECONDTARNUM++;
			CALCSIZE();

			return TRUE;
		}


	};

	struct SNETPC_REQ_SKILL_FB : public NET_MSG_GENERIC
	{
		
		EMSKILLCHECK		emSKILL_FB;
		STARGETID			sTargetID;
		SNATIVEID			sSkillID;
		WORD				wLevel;
		
		char				szName[CHAR_SZNAME];

		SNETPC_REQ_SKILL_FB () :
			emSKILL_FB(EMSKILL_OK),
				sSkillID(NATIVEID_NULL())
		{
			ZeroMemory( szName, CHAR_SZNAME );
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_SKILL_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SKILL_FB)<=NET_DATA_BUFSIZE);
		}
	};

    struct SNET_MSG_REQ_SKILL_REVIVE : public NET_MSG_GENERIC
    {
		SNET_MSG_REQ_SKILL_REVIVE () 
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_SKILL_REVIVE;
			MIN_STATIC_ASSERT(sizeof(SNET_MSG_REQ_SKILL_REVIVE)<=NET_DATA_BUFSIZE);
		}
    };

	struct SNETPC_REQ_SKILL_BRD : public NET_MSG_GENERIC
	{
		

		EMCROW				emCrow;
		DWORD				dwID;

		SNATIVEID			skill_id;
		WORD				wLEVEL;

		D3DXVECTOR3			vTARPOS;

		WORD				wTARNUM;
		STARID				sTARIDS[EMTARGET_NET];

		SNETPC_REQ_SKILL_BRD () :
			emCrow(CROW_PC),
			dwID(0),
			skill_id(NATIVEID_NULL()),
			wLEVEL(0),

			vTARPOS(0,0,0),
			wTARNUM(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_SKILL_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SKILL_BRD)<=NET_DATA_BUFSIZE);
		}

		void CALCSIZE ()
		{
			dwSize = sizeof(*this) - sizeof(STARID) * ( EMTARGET_NET - wTARNUM );
			//MIN_STATIC_ASSERT(sizeof()<=NET_DATA_BUFSIZE);
		}

		BOOL ADDTARGET ( EMCROW emCrow, DWORD dwID )
		{
			if ( wTARNUM==EMTARGET_NET )
				return FALSE;

			sTARIDS[wTARNUM].emCrow = emCrow;
			sTARIDS[wTARNUM].dwID = dwID;
			
			wTARNUM++;
			CALCSIZE ();

			return TRUE;
		}
		BOOL ADDTARGET ( const STARID &starget )
		{
			return ADDTARGET(starget.GETCROW(),starget.GETID());
		}
	};

	struct SNETPC_REQ_SKILL_DEALY_ACTION : public NET_MSG_GENERIC
	{


		EMCROW				emCrow;
		DWORD				dwID;

		SNATIVEID			skill_id;
		WORD				wLEVEL;

		D3DXVECTOR3			vTARPOS;

		WORD				wTARNUM;
		STARID				sTARIDS[EMTARGET_NET];

		SNETPC_REQ_SKILL_DEALY_ACTION () :
		emCrow(CROW_PC),
			dwID(0),
			skill_id(NATIVEID_NULL()),
			wLEVEL(0),

			vTARPOS(0,0,0),
			wTARNUM(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_SKILL_DEALY_ACTION;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SKILL_DEALY_ACTION)<=NET_DATA_BUFSIZE);
		}

		void CALCSIZE ()
		{
			dwSize = sizeof(*this) - sizeof(STARID) * ( EMTARGET_NET - wTARNUM );
			//MIN_STATIC_ASSERT(sizeof()<=NET_DATA_BUFSIZE);
		}

		BOOL ADDTARGET ( EMCROW emCrow, DWORD dwID )
		{
			if ( wTARNUM==EMTARGET_NET )
				return FALSE;

			sTARIDS[wTARNUM].emCrow = emCrow;
			sTARIDS[wTARNUM].dwID = dwID;

			wTARNUM++;
			CALCSIZE ();

			return TRUE;
		}
		BOOL ADDTARGET ( const STARID &starget )
		{
			return ADDTARGET(starget.GETCROW(),starget.GETID());
		}
	};

	struct SNETPC_REQ_SKILL_DEALY_ACTION_BRD : public NET_MSG_GENERIC
	{


		EMCROW				emCrow;
		DWORD				dwID;

		SNATIVEID			skill_id;
		WORD				wLEVEL;

		D3DXVECTOR3			vTARPOS;

		WORD				wTARNUM;
		STARID				sTARIDS[EMTARGET_NET];

		SNETPC_REQ_SKILL_DEALY_ACTION_BRD () :
		emCrow(CROW_PC),
			dwID(0),
			skill_id(NATIVEID_NULL()),
			wLEVEL(0),

			vTARPOS(0,0,0),
			wTARNUM(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_SKILL_DEALY_ACTION_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SKILL_DEALY_ACTION_BRD)<=NET_DATA_BUFSIZE);
		}

		void CALCSIZE ()
		{
			dwSize = sizeof(*this) - sizeof(STARID) * ( EMTARGET_NET - wTARNUM );
			//MIN_STATIC_ASSERT(sizeof()<=NET_DATA_BUFSIZE);
		}

		BOOL ADDTARGET ( EMCROW emCrow, DWORD dwID )
		{
			if ( wTARNUM==EMTARGET_NET )
				return FALSE;

			sTARIDS[wTARNUM].emCrow = emCrow;
			sTARIDS[wTARNUM].dwID = dwID;

			wTARNUM++;
			CALCSIZE ();

			return TRUE;
		}
		BOOL ADDTARGET ( const STARID &starget )
		{
			return ADDTARGET(starget.GETCROW(),starget.GETID());
		}
	};

	struct SNETPC_SKILLFACT_BRD : public SNETCROW_BROAD
	{
		STARID			sACTOR;
		SNATIVEID		sSkillID;

		DWORD			dwDamageFlag;
		int					nVAR_HP;
		int					nVAR_MP;
		int					nVAR_SP;
        WORD			wSchool;    //mjeon.CaptureTheField
		WORD			wSkillLevel;


		SNETPC_SKILLFACT_BRD () 
			: dwDamageFlag( DAMAGE_TYPE_NONE )
			, sSkillID(NATIVEID_NULL())
			, nVAR_HP(0)
			, nVAR_MP(0)
			, nVAR_SP(0)
			, wSchool(SCHOOL_NONE)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_SKILLFACT_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SKILLFACT_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SKILLHOLD_BRD : public SNETCROW_BROAD
	{		
		enum ERECVTYPE
		{
			E_REV_NONE  ,
			E_REV_ATTACK, // 공격자
			E_REV_TARGET, // 타켓
		};

		struct stEFFINFO {
			stEFFINFO() : eType( E_REV_NONE ) {
				memset( iRecvEAIdx, 0, sizeof(iRecvEAIdx));
				memset( iRecvSAIdx, 0, sizeof(iRecvSAIdx));
			}

			ERECVTYPE eType;
			BYTE	iRecvEAIdx[SKILL::MAX_ADDON];
			BYTE	iRecvSAIdx[SKILL::MAX_ADDON];
		};

		SNATIVEID			skill_id;
		WORD				wLEVEL;
		WORD				wSELSLOT;
        SSKILLFACT::STIME   sTIME;
        float					fPARAM;
        STARGETID			sID;    //스킬 시전자 ID

		stEFFINFO			sEffInfo;

		SNETPC_SKILLHOLD_BRD () :
			skill_id(NATIVEID_NULL()),
			wLEVEL(0),
			wSELSLOT(0),
            sTIME(_SKILLFACT_DEFAULTTIME),
            fPARAM(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_SKILLHOLD_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SKILLHOLD_BRD)<=NET_DATA_BUFSIZE);			
		}
	};


	struct SNETPC_SKILLHOLD_RS_BRD : public SNETCROW_BROAD
	{
		bool				 bRESET[SKILLFACT_SIZE];
		EMSKILLHOLDRESET	emResetType; 

		SNETPC_SKILLHOLD_RS_BRD() : emResetType( EMSKILLHOLDRESET_EFFECT )
		{
			dwSize	= sizeof(*this);
			nType	= NET_MSG_GCTRL_SKILLHOLD_RS_BRD;
			MIN_STATIC_ASSERT( sizeof( SNETPC_SKILLHOLD_RS_BRD ) <= NET_DATA_BUFSIZE );

			memset( bRESET, 0, sizeof(bool)*SKILLFACT_SIZE );
		} //SNETPC_SKILLHOLD_RS_BRD
	};

	struct SNETPC_REQ_SKILLHOLD_RS : public SNETCROW_BROAD
	{
		SNATIVEID			sSKILLID;
		WORD				wRESETSLOT;

		SNETPC_REQ_SKILLHOLD_RS () :
			sSKILLID(NATIVEID_NULL()),
			wRESETSLOT(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_SKILLHOLD_RS;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SKILLHOLD_RS)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_SKILLHOLD_RS_FB : public SNETCROW_BROAD
	{
		SNATIVEID			sSKILLID;
		WORD				wRESETSLOT;

		SNETPC_REQ_SKILLHOLD_RS_FB () :
			sSKILLID(NATIVEID_NULL()),
			wRESETSLOT(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_SKILLHOLD_RS_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SKILLHOLD_RS_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_STATEBLOW_BRD : public SNETCROW_BROAD
	{
		EMSTATE_BLOW		emBLOW;
		float				fAGE;
		float				fSTATE_VAR1;
		float				fSTATE_VAR2;
        WORD                wSchool;        //mjeon.CaptureTheField

		SNETPC_STATEBLOW_BRD() :
			emBLOW(EMBLOW_NONE),
			fAGE(0),
			fSTATE_VAR1(0),
			fSTATE_VAR2(0),
            wSchool(SCHOOL_NONE)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_STATEBLOW_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_STATEBLOW_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CURESTATEBLOW_BRD : public SNETCROW_BROAD
	{
		DWORD				dwCUREFLAG;

		SNETPC_CURESTATEBLOW_BRD () :
			dwCUREFLAG(NULL)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_CURESTATEBLOW_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CURESTATEBLOW_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SAVESKILLFACT_BRD : public SNETCROW_BROAD
	{
		enum
		{
			NSIZE = (NET_DATA_BUFSIZE - sizeof(SNETCROW_BROAD) - sizeof(DWORD)) / sizeof(SCHARSAVESKILLFACT),
			HEADER_NSIZE = sizeof(SNETCROW_BROAD) + sizeof(DWORD),
		};		
		DWORD nSkillFactSize;
		SCHARSAVESKILLFACT sSAVESKILLFACT[NSIZE];

		const bool addSaveSkillFact(const SCHARSAVESKILLFACT& _sSaveSkillFact)
		{
			if ( nSkillFactSize >= NSIZE )
				return false;

			sSAVESKILLFACT[nSkillFactSize] = _sSaveSkillFact;
			++nSkillFactSize;
			dwSize += sizeof(SCHARSAVESKILLFACT);

			if ( nSkillFactSize >= NSIZE )
				return false;

			return true;
		}
		const bool isEmpty(void) const
		{
			return nSkillFactSize == 0;
		}
		void reset(void)
		{
			dwSize = HEADER_NSIZE;
			nSkillFactSize = 0;
		}
		SNETPC_SAVESKILLFACT_BRD ()
			: nSkillFactSize(0)
		{
			dwSize = HEADER_NSIZE;
			nType  = NET_MSG_GCTRL_SAVESKILLFACT_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SAVESKILLFACT_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SKILLCONSUME_FB : public NET_MSG_GENERIC
	{
		

		WORD		wTurnNum;	// 소모형 아이템의 사용가능 횟수.
		int				nNowHP;
		int				nNowMP;
		int				nNowSP;

		SNETPC_SKILLCONSUME_FB () :
			wTurnNum(0),
			nNowHP(0),
			nNowMP(0),
			nNowSP(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_SKILLCONSUME_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SKILLCONSUME_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_BEGIN_SKILL_FC : public NET_MSG_GENERIC
	{
		SNATIVEID		sSkillID;
		STARGETID		sTargetID;
		WORD			wLevel;

		// sSPEC;
		DWORD			dwFLAG;
		float				fVAR1;
		float				fVAR2;
		float				fLife;

		SNETPC_BEGIN_SKILL_FC()
		{
			dwSize = sizeof(SNETPC_BEGIN_SKILL_FC);
			nType = NET_MSG_GCTRL_BEGIN_SKILL_FC;
			MIN_STATIC_ASSERT(sizeof(SNETPC_BEGIN_SKILL_FC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_END_SKILL_FC : public NET_MSG_GENERIC
	{
		const DWORD		dwSkillfactIndex;
		BOOL			bForceEnd;
		BOOL			bOverLabEnd;

		SNETPC_END_SKILL_FC(const DWORD _dwSkillfactIndex, BOOL bForce, BOOL _bOverLabEnd = false ) : 
		dwSkillfactIndex(_dwSkillfactIndex), bForceEnd( bForce ), bOverLabEnd( _bOverLabEnd )
		{
			dwSize = sizeof(SNETPC_END_SKILL_FC);
			nType = NET_MSG_GCTRL_END_SKILL_FC;
			MIN_STATIC_ASSERT(sizeof(SNETPC_END_SKILL_FC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_END_SKILL_BRD : public SNETCROW_BROAD
	{
		const DWORD		dwSkillfactIndex;	
		BOOL			bForceEnd;

		SNETPC_END_SKILL_BRD(const EMCROW emCrow, const DWORD dwGaeaID, const DWORD _dwSkillfactIndex, BOOL bForce) : 
		SNETCROW_BROAD(emCrow, dwGaeaID), dwSkillfactIndex(_dwSkillfactIndex), bForceEnd( bForce )
		{
			dwSize = sizeof(SNETPC_END_SKILL_BRD);
			nType = NET_MSG_GCTRL_END_SKILL_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_END_SKILL_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SKILL_RESET_CF : public NET_MSG_GENERIC
	{
		SNET_SKILL_RESET_CF()
		{
			dwSize = sizeof( SNET_SKILL_RESET_CF );
			nType = NET_MSG_SKILL_RESET_CF;
			MIN_STATIC_ASSERT( sizeof( SNET_SKILL_RESET_CF )<=NET_DATA_BUFSIZE );
		}
	};

	// Revert to default structure packing
	#pragma pack()
};
