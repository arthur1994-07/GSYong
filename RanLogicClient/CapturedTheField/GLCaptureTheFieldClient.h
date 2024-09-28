#pragma once

//--------------------------------------------------------------------------------------------//

#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/GLCompetition.h"
#include "../Widget/GLWidgetScript.h"

//--------------------------------------------------------------------------------------------//

#include <map>
#include <vector>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/tr1/memory.hpp>

//--------------------------------------------------------------------------------------------//

class GLGaeaClient;
class GLCharacter;
class GLLevelFileClient;

//--------------------------------------------------------------------------------------------//

namespace PVP
{
    struct SALLIANCEINFO
    {
        SALLIANCEINFO(const D3DXVECTOR3& _vPos)
            : vPos(_vPos)
            , bNull(false)
			, bMyInfo(false)
        {
        }

        const bool IsNull()  { return bNull; }
        void SetNull() { bNull = true;  }

        D3DXVECTOR3 vPos;
        bool        bNull;
		bool		bMyInfo;
    };

    struct SAUTHENTICATORINFO
    {
        SAUTHENTICATORINFO(const SNATIVEID& sNativeID)
            : sID(sNativeID)
            , emAuthSchool(SCHOOL_NONE)
        {
        }

        const bool Init();
        const std::string& GetName() const { return strName; }
        const bool IsNull() { return sID==SNATIVEID(false); }

        SNATIVEID   sID;
        std::string strName;
        ENUM_SCHOOL emAuthSchool;
        D3DXVECTOR3 vPos;
    };

    class GLCaptureTheFieldClient : private boost::noncopyable
    {

    public:
        typedef std::vector<SALLIANCEINFO>      VEC_ALLIANCEINFO;
        typedef VEC_ALLIANCEINFO::iterator      VEC_ALLIANCEINFO_ITER;

        typedef std::vector<SAUTHENTICATORINFO> VEC_AUTHENTICATORINFO;
        typedef VEC_AUTHENTICATORINFO::iterator VEC_AUTHENTICATORINFO_ITER;

    public: 
        //! ���� CTF���� ����
        struct CTFGLOBALSTATE
        {
            ENUM_CTF_TYPE			emTYPE;
            ENUM_CTF_STATE		emSTATE;
            bool				bWAITJOIN;
            UINT				nREMAINMINUTES;
            UINT				nQUEUE;
			__time64_t			tSTARTTIME;		  // CTF�� ���۵� or ���۵� �ð�
			__time64_t			tSTOPTIME;		  // CTF�� ���� �ð�
			__time64_t			tPLAYTIME;		  // CTF �÷��̽ð�
			__time64_t			tEXTRATIME;		  // CTF Reward->Stop �ð�
			__time64_t			tCYCLETIME;		  // CTF ����Ŭ
			WORD				wLEVELLIMIT;	  // ���� ����
			UINT				nMAXPLAYERS;	  // ���� �� ����
			ENUM_SCHOOL			emAUTHSCHOOL;	  // ���� �п�
            SNATIVEID			sPVPMAPID;		  // ������ PVP ��
            SNATIVEID			sPVEMAPID;		  // ������ PVE ��
			//���� ������ ���ɿ��� 
			BOOL		bisConsecutively;		  // ������ ���Ӽ�
			BOOL		bConsecutivelyCapturel;	  // ������ ���� ���� ����
			DWORD		dwConsecutivelyNumber;    // ������ ���� ����

			std::tr1::shared_ptr<GLLevelFileClient>  spPVPMAPLEVELDATA; // ������ PVP ��
			std::tr1::shared_ptr<GLLevelFileClient>  spPVEMAPLEVELDATA; // ������ PVE ��

            VEC_AUTHENTICATORINFO vecAUTHENTICATOR; // ������ ����
           
            CTFGLOBALSTATE()
                : emTYPE(ENUM_CTF_TYPE_NULL)
                , emSTATE(CTF_STATE_STOP)
                , bWAITJOIN(false)
                , nREMAINMINUTES(0)
                , nQUEUE(0)
                , tSTARTTIME(0)
				, tSTOPTIME(0)
                , tPLAYTIME(0)
				, tEXTRATIME(0)
				, tCYCLETIME(0)
				, wLEVELLIMIT(0)
				, nMAXPLAYERS(0)
                , sPVPMAPID(SNATIVEID(false))
                , sPVEMAPID(SNATIVEID(false))
                , emAUTHSCHOOL(SCHOOL_NONE)
				, bisConsecutively(FALSE)
				, bConsecutivelyCapturel(TRUE)
				, dwConsecutivelyNumber(0)
            {
            }

            CTFGLOBALSTATE( const ENUM_CTF_TYPE _emTYPE, const ENUM_CTF_STATE _emSTATE )
                : emTYPE(_emTYPE)
                , emSTATE(_emSTATE)
                , bWAITJOIN(false)
                , nREMAINMINUTES(0)
                , nQUEUE(0)
                , tSTARTTIME(0)
				, tPLAYTIME(0)
				, wLEVELLIMIT(0)
				, nMAXPLAYERS(0)
                , sPVPMAPID(SNATIVEID(false))
				, sPVEMAPID(SNATIVEID(false))
                , emAUTHSCHOOL(SCHOOL_NONE)
				, bisConsecutively(FALSE)
				, bConsecutivelyCapturel(TRUE)
				, dwConsecutivelyNumber(0)
            {
            }

            bool IsNULL  () const { return (emTYPE == ENUM_CTF_TYPE_NULL); }
            bool IsSTOP  () const { return (emSTATE == CTF_STATE_STOP);  }
            bool IsREADY () const { return (emSTATE == CTF_STATE_READY); }
            bool IsSTART () const { return (emSTATE == CTF_STATE_START); }
            bool IsREWARD () const { return (emSTATE == CTF_STATE_REWARD); }
			//bool IsCANNOTJOIN () const { return ((emSTATE == CTF_STATE_DONE) || (emSTATE == CTF_STATE_REWARD) || (emSTATE == CTF_STATE_STOP)) || (bisCanJoinSchoolAuthed == FALSE); }
			bool IsCANNOTJOIN (ENUM_SCHOOL myschool ) const { return ((emSTATE == CTF_STATE_DONE) || (emSTATE == CTF_STATE_REWARD) || (emSTATE == CTF_STATE_STOP)); }
			bool IsCANNOTJOIN (WORD myschool ) const { return IsCANNOTJOIN(ENUM_SCHOOL(myschool)); }
            bool IsSAMEMAP ( const SNATIVEID sMapID ) const { return (sPVPMAPID == sMapID || sPVEMAPID == sMapID) ? true : false; }
			const bool IsPVPMap ( const SNATIVEID& _sToMapID ) const { return ( sPVPMAPID == _sToMapID ); }
			const bool IsPVEMap ( const SNATIVEID& _sToMapID ) const { return ( sPVEMAPID == _sToMapID ); }
			bool IsJOINDENIED ( GLCharacter* pCharacter ) const;

            void RESETAUTH();
			void INIT( GLGaeaClient* pGaeaClient );

            SAUTHENTICATORINFO& FindAuthenticator( const SNATIVEID& sID );
        };
        
        typedef std::map<ENUM_CTF_TYPE,CTFGLOBALSTATE>  CTFGLOBALSTATE_MAP;
        typedef std::pair<ENUM_CTF_TYPE,CTFGLOBALSTATE> CTFGLOBALSTATE_PAIR;
        typedef CTFGLOBALSTATE_MAP::iterator        CTFGLOBALSTATE_MAP_ITER;
		typedef CTFGLOBALSTATE_MAP::const_iterator		CTFGLOBALSTATE_MAP_CITER;

	public: 
		//! ���� CTF����
		struct CTFPREVSTATE
		{
			ENUM_CTF_TYPE			emTYPE;
			__time64_t			tSTARTTIME;		  // ���۵� �ð�
			__time64_t			tENDTIME;		  // ����� �ð�
			ENUM_SCHOOL			emAUTHSCHOOL;	  // ���� �п�
			//���� ������ ���ɿ��� 
			BOOL		bisConsecutively;		  // ������ ���Ӽ�
			BOOL		bConsecutivelyCapturel;	  // ������ ���� ���� ����
			DWORD		dwConsecutivelyNumber;    // ������ ���� ����


			VCTF_REWARD_BUFF	vecREWARDBUFF;	 // ���� ����
			VEC_CTF_RANKER		vecRESULTDATA;   // ��� ����

			CTFPREVSTATE()
				: emTYPE(ENUM_CTF_TYPE_NULL)
				, tSTARTTIME(0)
				, tENDTIME(0)
				, emAUTHSCHOOL(SCHOOL_NONE)
			{
			}

			bool IsNULL() const { return (emTYPE == ENUM_CTF_TYPE_NULL); }
		};

		typedef std::map<ENUM_CTF_TYPE,CTFPREVSTATE>  CTFPREVSTATE_MAP;
		typedef std::pair<ENUM_CTF_TYPE,CTFPREVSTATE> CTFPREVSTATE_PAIR;
		typedef CTFPREVSTATE_MAP::iterator        CTFPREVSTATE_MAP_ITER;

    public:
        GLCaptureTheFieldClient  ( GLGaeaClient* pGaeaClient );
        ~GLCaptureTheFieldClient ();

    public:
        void                Clear ();
        HRESULT     	    FrameMove ( float fTime, float fElapsedTime );

    public:
        void                SetGlobalState ( const CTFGLOBALSTATE& sCTFState );
        CTFGLOBALSTATE&     GetGlobalState ( const ENUM_CTF_TYPE emTYPE );
        CTFGLOBALSTATE&     GetGlobalState ( const SNATIVEID& sMapID );
        CTFGLOBALSTATE&     GetCurrentGlobalState ();

		void                SetPrevState ( const CTFPREVSTATE& sPrevState );
		CTFPREVSTATE&		GetPrevState ( const ENUM_CTF_TYPE emTYPE );

    public:
        const bool          IsPlayer() { return !(m_emProgressCTF == ENUM_CTF_TYPE_NULL); }
        void                SetProgressCTF ( const ENUM_CTF_TYPE& emProgressCTF )    { m_emProgressCTF = emProgressCTF; }
        ENUM_CTF_TYPE           GetProgressCTF ()                                    { return m_emProgressCTF; }

    public:
        const bool          IsCTFMap( const SNATIVEID& sMapID );
		const bool			IsCTFMap_PVP ( const SNATIVEID& _sToMapID ) const;
		const bool			IsCTFMap_PVE ( const SNATIVEID& _sToMapID ) const;

    public:
        void                AddAllianceInfo( SALLIANCEINFO& sInfo )                  { m_vecAllianceInfo.push_back(sInfo); }
        void                SetAllianceInfo( UINT nIndex, SALLIANCEINFO& sInfo )     { m_vecAllianceInfo[nIndex] = sInfo; }
        VEC_ALLIANCEINFO&   GetAllianceInfo()                                        { return m_vecAllianceInfo; }
        SALLIANCEINFO&      GetAllianceInfo( UINT nIndex )                           { return m_vecAllianceInfo[nIndex]; }
        const UINT          GetAllianceInfoSize()                                    { return (UINT)m_vecAllianceInfo.size(); }

	public:
		void				AddResultData( const SCTFRESULTDATA& sData );
		VEC_RESULTDATA&		GetResultDataTotal() { return m_vecResultData; }
		VEC_RESULTDATA&		GetResultDataSM() { return m_vecResultDataSM; }
		VEC_RESULTDATA&		GetResultDataHA() { return m_vecResultDataHA; }
		VEC_RESULTDATA&		GetResultDataBH() { return m_vecResultDataBH; }
		SCTFRESULTDATA&		GetMyResultData() { return m_MyResultData; }
		const UINT		    GetMyResultRanking() { return m_MyResultRanking; }
		const UINT		    GetMyResultRankingSchool() { return m_MyResultRankingSchool; }

	public:
		GLLevelFileClient*	GetPVPMapLevelData( const ENUM_CTF_TYPE& emProgressCTF );

    public:
		void                Reset(); //! �������� ����
		void                ClearAllianceInfo();
		void                ClearResultData();

    private:
		CTFGLOBALSTATE_MAP  m_mGlobalState;    //! CTF ���� ����
		CTFPREVSTATE_MAP	m_mPrevState;	   //! CTF ���� ����(���� ����)

        VEC_ALLIANCEINFO    m_vecAllianceInfo; //! ������ ���� (��ġ)
		VEC_RESULTDATA		m_vecResultData;   //! ��� ����
		VEC_RESULTDATA		m_vecResultDataSM; //! ��� ���� �����п�
		VEC_RESULTDATA		m_vecResultDataHA; //! ��� ���� �����п�
		VEC_RESULTDATA		m_vecResultDataBH; //! ��� ���� ��Ȳ�п�
		SCTFRESULTDATA		m_MyResultData;	   //! �� ��� ����
		UINT				m_MyResultRanking; //! �� ��ŷ ����
		UINT				m_MyResultRankingSchool; //! �� �п� ��ŷ ����
        ENUM_CTF_TYPE           m_emProgressCTF;   //! ���� �������� CTF
	
	//Scaleform 
	public:
		DWORD GetCTFCaptureSchool(void);
		lua_tinker::table GetResultData(void) const;
		lua_tinker::table GetResultSM(void) const;
		lua_tinker::table GetResultHA(void) const;
		lua_tinker::table GetResultBA(void) const;
		lua_tinker::table GetMyResult(void) const;
		void			  ReqCTFieldQuit(void) const;

    private:
        GLGaeaClient*       m_pGaeaClient;



    };
};
