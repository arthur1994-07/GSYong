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
        //! 현재 CTF들의 상태
        struct CTFGLOBALSTATE
        {
            ENUM_CTF_TYPE			emTYPE;
            ENUM_CTF_STATE		emSTATE;
            bool				bWAITJOIN;
            UINT				nREMAINMINUTES;
            UINT				nQUEUE;
			__time64_t			tSTARTTIME;		  // CTF가 시작될 or 시작된 시간
			__time64_t			tSTOPTIME;		  // CTF가 끝날 시간
			__time64_t			tPLAYTIME;		  // CTF 플레이시간
			__time64_t			tEXTRATIME;		  // CTF Reward->Stop 시간
			__time64_t			tCYCLETIME;		  // CTF 사이클
			WORD				wLEVELLIMIT;	  // 레벨 제한
			UINT				nMAXPLAYERS;	  // 참여 수 제한
			ENUM_SCHOOL			emAUTHSCHOOL;	  // 점령 학원
            SNATIVEID			sPVPMAPID;		  // 점령전 PVP 맵
            SNATIVEID			sPVEMAPID;		  // 점령전 PVE 맵
			//연속 점령전 가능여부 
			BOOL		bisConsecutively;		  // 점령전 연속성
			BOOL		bConsecutivelyCapturel;	  // 점령전 연속 가능 여부
			DWORD		dwConsecutivelyNumber;    // 점령전 연속 갯수

			std::tr1::shared_ptr<GLLevelFileClient>  spPVPMAPLEVELDATA; // 점령전 PVP 맵
			std::tr1::shared_ptr<GLLevelFileClient>  spPVEMAPLEVELDATA; // 점령전 PVE 맵

            VEC_AUTHENTICATORINFO vecAUTHENTICATOR; // 인증기 정보
           
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
		//! 이전 CTF정보
		struct CTFPREVSTATE
		{
			ENUM_CTF_TYPE			emTYPE;
			__time64_t			tSTARTTIME;		  // 시작된 시간
			__time64_t			tENDTIME;		  // 종료된 시간
			ENUM_SCHOOL			emAUTHSCHOOL;	  // 점령 학원
			//연속 점령전 가능여부 
			BOOL		bisConsecutively;		  // 점령전 연속성
			BOOL		bConsecutivelyCapturel;	  // 점령전 연속 가능 여부
			DWORD		dwConsecutivelyNumber;    // 점령전 연속 갯수


			VCTF_REWARD_BUFF	vecREWARDBUFF;	 // 보상 버프
			VEC_CTF_RANKER		vecRESULTDATA;   // 결과 정보

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
		void                Reset(); //! 진행정보 리셋
		void                ClearAllianceInfo();
		void                ClearResultData();

    private:
		CTFGLOBALSTATE_MAP  m_mGlobalState;    //! CTF 들의 상태
		CTFPREVSTATE_MAP	m_mPrevState;	   //! CTF 들의 상태(이전 정보)

        VEC_ALLIANCEINFO    m_vecAllianceInfo; //! 동맹의 정보 (위치)
		VEC_RESULTDATA		m_vecResultData;   //! 결과 정보
		VEC_RESULTDATA		m_vecResultDataSM; //! 결과 정보 성문학원
		VEC_RESULTDATA		m_vecResultDataHA; //! 결과 정보 현암학원
		VEC_RESULTDATA		m_vecResultDataBH; //! 결과 정보 봉황학원
		SCTFRESULTDATA		m_MyResultData;	   //! 내 결과 정보
		UINT				m_MyResultRanking; //! 내 랭킹 정보
		UINT				m_MyResultRankingSchool; //! 내 학원 랭킹 정보
        ENUM_CTF_TYPE           m_emProgressCTF;   //! 내가 진행중인 CTF
	
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
