#ifndef _CROW_ACTION_H_
#define _CROW_ACTION_H_

#pragma once

#include <string>
#include "../../EngineLib/DxMeshs/SkinMesh/SAnimationDefine.h"
#include "../Character/GLCharDefine.h"
#include "./GLCrowDefine.h"

namespace sc
{
    class BaseStream;
    class SerialFile;
}

struct SCROWACTION_100
{
    WORD				m_wBodyRadius;					//	몸체원통 반경.
    char				m_szSkinObj[ACF_SZNAME];		//	스킨 파일.
    char				m_szTalkFile[ACF_SZNAME];		//	대화 파일.

    BOOL				m_bTrade;						//	거래가능한지.
    char				m_szSaleFile[ACF_SZNAME];		//	Item 판매 목록.

    BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
    char				m_szFallingEffect[ACF_SZNAME];	//	죽는 순간에 나오는 효과.
    char				m_szBlowEffect[ACF_SZNAME];		//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

    DWORD				m_dwActionUP;					//	행동 성향 HP 50%이상.
    DWORD				m_dwActionDN;					//	행동 성향 HP 50%이하.

    EMMOVETYPE			m_emMoveType;					//	이동 타입.
    float				m_fDriftHeight;					//	부양 높이.

    float				m_fWalkVelo;					//	이동 속도.
    BOOL				m_bRun;							//	뛰기 가능.
    float				m_fRunVelo;						//	뛰기 속도.
};

struct SCROWACTION_101
{
    WORD				m_wBodyRadius;					//	몸체원통 반경.
    char				m_szSkinObj[ACF_SZNAME];		//	스킨 파일.
    char				m_szTalkFile[ACF_SZNAME];		//	대화 파일.

    DWORD				m_dwActFlag;					//	각종 행동.
    char				m_szSaleFile[ACF_SZNAME];		//	Item 판매 목록.

    BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
    char				m_szBirthEffect[ACF_SZNAME];	//	탄생시 나오는 효과.
    char				m_szFallingEffect[ACF_SZNAME];	//	죽는 순간에 나오는 효과.
    char				m_szBlowEffect[ACF_SZNAME];		//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

    EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
    EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

    EMMOVETYPE			m_emMoveType;					//	이동 타입.
    float				m_fDriftHeight;					//	부양 높이.

    float				m_fWalkVelo;					//	이동 속도.
    BOOL				m_bRun;							//	뛰기 가능.
    float				m_fRunVelo;						//	뛰기 속도.
};

struct SCROWACTION_102
{
    enum { SALENUM = 3 };
    WORD				m_wBodyRadius;					//	몸체원통 반경.
    char				m_szSkinObj[ACF_SZNAME];		//	스킨 파일.
    char				m_szTalkFile[ACF_SZNAME];		//	대화 파일.

    DWORD				m_dwActFlag;					//	각종 행동.
    char				m_szSaleFile[SALENUM][ACF_SZNAME];		//	Item 판매 목록.

    BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
    char				m_szBirthEffect[ACF_SZNAME];	//	탄생시 나오는 효과.
    char				m_szFallingEffect[ACF_SZNAME];	//	죽는 순간에 나오는 효과.
    char				m_szBlowEffect[ACF_SZNAME];		//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

    EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
    EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

    EMMOVETYPE			m_emMoveType;					//	이동 타입.
    float				m_fDriftHeight;					//	부양 높이.

    float				m_fWalkVelo;					//	이동 속도.
    BOOL				m_bRun;							//	뛰기 가능.
    float				m_fRunVelo;						//	뛰기 속도.
};

struct SCROWACTION_103
{
    enum { VERSION = 0x0103, SALENUM = 3 };

    WORD				m_wBodyRadius;					//	몸체원통 반경.
    std::string			m_strSkinObj;					//	스킨 파일.
    std::string			m_strTalkFile;					//	대화 파일.

    DWORD				m_dwActFlag;					//	각종 행동.
    std::string			m_strSaleFile[SALENUM];			//	Item 판매 목록.

    BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
    std::string			m_strBirthEffect;				//	탄생시 나오는 효과.
    std::string			m_strFallingEffect;				//	죽는 순간에 나오는 효과.
    std::string			m_strBlowEffect;					//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

    EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
    EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

    EMMOVETYPE			m_emMoveType;					//	이동 타입.
    float				m_fDriftHeight;					//	부양 높이.

    float				m_fWalkVelo;					//	이동 속도.
    BOOL				m_bRun;							//	뛰기 가능.
    float				m_fRunVelo;						//	뛰기 속도.
};

struct SCROWACTION_104
{
    enum { VERSION = 0x0104, SALENUM = 3 };

    WORD				m_wBodyRadius;					//	몸체원통 반경.
    std::string			m_strSkinObj;					//	스킨 파일.
    std::string			m_strTalkFile;					//	대화 파일.

    DWORD				m_dwActFlag;					//	각종 행동.
    std::string			m_strSaleFile[SALENUM];			//	Item 판매 목록.

    BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
    std::string			m_strBirthEffect;				//	탄생시 나오는 효과.
    std::string			m_strFallingEffect;				//	죽는 순간에 나오는 효과.
    std::string			m_strBlowEffect;					//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

    EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
    EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

    EMMOVETYPE			m_emMoveType;					//	이동 타입.
    float				m_fDriftHeight;					//	부양 높이.

    float				m_fWalkVelo;					//	이동 속도.
    BOOL				m_bRun;							//	뛰기 가능.
    float				m_fRunVelo;						//	뛰기 속도.

    float				m_fLiveTime;					//  활동가능한 시간(EMCROWACT_AUTODROP 플래그일 경우)
};

struct SCROWACTION_105
{
    enum { VERSION = 0x0105, SALENUM = 3 };

    WORD				m_wBodyRadius;					//	몸체원통 반경.
    std::string			m_strSkinObj;					//	스킨 파일.
    std::string			m_strTalkFile;					//	대화 파일.

    DWORD				m_dwActFlag;					//	각종 행동.
    std::string			m_strSaleFile[SALENUM];			//	Item 판매 목록.

    BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
    std::string			m_strBirthEffect;				//	탄생시 나오는 효과.
    std::string			m_strFallingEffect;				//	죽는 순간에 나오는 효과.
    std::string			m_strBlowEffect;					//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

    EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
    EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

    EMMOVETYPE			m_emMoveType;					//	이동 타입.
    float				m_fDriftHeight;					//	부양 높이.

    float				m_fWalkVelo;					//	이동 속도.
    BOOL				m_bRun;							//	뛰기 가능.
    float				m_fRunVelo;						//	뛰기 속도.

    float				m_fLiveTime;					//  활동가능한 시간(EMCROWACT_AUTODROP 플래그일 경우)
    float				m_fActionDNRate;				// 행동 성향 이하 %
};


struct SCROWACTION_106
{
    enum { VERSION = 0x0106, SALENUM = 3 };

    WORD				m_wBodyRadius;					//	몸체원통 반경.
    std::string			m_strSkinObj;					//	스킨 파일.
    std::string			m_strTalkFile;					//	대화 파일.

    DWORD				m_dwActFlag;					//	각종 행동.
    std::string			m_strSaleFile[SALENUM];			//	Item 판매 목록.

    BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
    std::string			m_strBirthEffect;				//	탄생시 나오는 효과.
    std::string			m_strFallingEffect;				//	죽는 순간에 나오는 효과.
    std::string			m_strBlowEffect;					//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

    EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
    EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

    EMMOVETYPE			m_emMoveType;					//	이동 타입.
    float				m_fDriftHeight;					//	부양 높이.

    float				m_fWalkVelo;					//	이동 속도.
    BOOL				m_bRun;							//	뛰기 가능.
    float				m_fRunVelo;						//	뛰기 속도.

    float				m_fLiveTime;					//  활동가능한 시간(EMCROWACT_AUTODROP 플래그일 경우)
    float				m_fActionDNRate;				// 행동 성향 이하 %

    BOOL				m_bMobLink;						// 해당몬스터가 죽으면 연결되어있는 몬스터를 호출할지 여부.
    SNATIVEID			m_sMobLinkID;					// 연결된 몬스터의 ID
};

struct SCROWACTION_107
{
    enum { VERSION = 0x0107, SALENUM = 3, PATTERNNUM = 10, ATTACKSETNUM = 4 };

    WORD				m_wBodyRadius;					//	몸체원통 반경.
    std::string			m_strSkinObj;					//	스킨 파일.
    std::string			m_strTalkFile;					//	대화 파일.

    DWORD				m_dwActFlag;					//	각종 행동.
    std::string			m_strSaleFile[SALENUM];			//	Item 판매 목록.

    BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
    std::string			m_strBirthEffect;				//	탄생시 나오는 효과.
    std::string			m_strFallingEffect;				//	죽는 순간에 나오는 효과.
    std::string			m_strBlowEffect;					//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

    EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
    EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

    EMMOVETYPE			m_emMoveType;					//	이동 타입.
    float				m_fDriftHeight;					//	부양 높이.

    float				m_fWalkVelo;					//	이동 속도.
    BOOL				m_bRun;							//	뛰기 가능.
    float				m_fRunVelo;						//	뛰기 속도.

    float				m_fLiveTime;					//  활동가능한 시간(EMCROWACT_AUTODROP 플래그일 경우)
    float				m_fActionDNRate;				// 행동 성향 이하 %

    BOOL				m_bMobLink;						// 해당몬스터가 죽으면 연결되어있는 몬스터를 호출할지 여부.
    SNATIVEID			m_sMobLinkID;					// 연결된 몬스터의 ID

    SCROWPATTERN		m_Pattern;						
    std::vector< SCROWPATTERN >	m_vecPatternList;
};

struct SCROWACTION_108
{
    enum { VERSION = 0x0108, SALENUM = 3, PATTERNNUM = 10, ATTACKSETNUM = 4 };

    WORD				m_wBodyRadius;					//	몸체원통 반경.
    std::string			m_strSkinObj;					//	스킨 파일.
    std::string			m_strTalkFile;					//	대화 파일.

    DWORD				m_dwActFlag;					//	각종 행동.
    std::string			m_strSaleFile[SALENUM];			//	Item 판매 목록.

    BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
    std::string			m_strBirthEffect;				//	탄생시 나오는 효과.
    std::string			m_strFallingEffect;				//	죽는 순간에 나오는 효과.
    std::string			m_strBlowEffect;					//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

    EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
    EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

    EMMOVETYPE			m_emMoveType;					//	이동 타입.
    float				m_fDriftHeight;					//	부양 높이.

    float				m_fWalkVelo;					//	이동 속도.
    BOOL				m_bRun;							//	뛰기 가능.
    float				m_fRunVelo;						//	뛰기 속도.

    float				m_fLiveTime;					//  활동가능한 시간(EMCROWACT_AUTODROP 플래그일 경우)
    float				m_fActionDNRate;				// 행동 성향 이하 %

    BOOL				m_bMobLink;						// 해당몬스터가 죽으면 연결되어있는 몬스터를 호출할지 여부.
    SNATIVEID			m_sMobLinkID;					// 연결된 몬스터의 ID
    float				m_fMobLinkDelay;				// 연결된 몬스터 리젠 딜레이
    float				m_fMobLinkScale;				// 연결된 몬스터 몸체 크기

    SCROWPATTERN		m_Pattern;						
    std::vector< SCROWPATTERN >	m_vecPatternList;
};

struct SCROWACTION
{
    // Version 변경내용 : SCROWPATTERN::RANDOMPATTERNNUM -> 10으로 수정
    enum { VERSION = 0x0110, SALENUM = 3, PATTERNNUM = 10, ATTACKSETNUM = 4 };	

    DWORD				m_dwActFlag;					//	각종 행동. EMCROW_NPCACT

    EMMOVETYPE			m_emMoveType;					//	이동 타입.
    float				m_fDriftHeight;					//	부양 높이.

    float				m_fWalkVelo;					//	이동 속도.
    BOOL				m_bRun;							//	뛰기 가능.
    float				m_fRunVelo;						//	뛰기 속도.

    WORD				m_wBodyRadius;					//	몸체원통 반경.
    std::string			m_strSkinObj;					//	스킨 파일.
    std::string			m_strTalkFile;					//	대화 파일.

    BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
    std::string			m_strBirthEffect;				//	탄생시 나오는 효과.
    std::string			m_strFallingEffect;				//	죽는 순간에 나오는 효과.
    std::string			m_strBlowEffect;				//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

    std::string			m_strSaleFile[SALENUM];			//	Item 판매 목록.

    float				m_fLiveTime;					//  활동가능한 시간(EMCROWACT_AUTODROP 플래그일 경우)

    BOOL				m_bMobLink;						// 해당몬스터가 죽으면 연결되어있는 몬스터를 호출할지 여부.
    SNATIVEID			m_sMobLinkID;					// 연결된 몬스터의 ID
    float				m_fMobLinkDelay;				// 연결된 몬스터 리젠 딜레이
    float				m_fMobLinkScale;				// 연결된 몬스터 몸체 크기

    SCROWPATTERN		m_Pattern;												
    std::vector< SCROWPATTERN >	m_vecPatternList;


    SCROWACTION();
    SCROWACTION(const SCROWACTION& value);

    SCROWACTION& operator= ( const SCROWACTION_100 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_101 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_102 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_103 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_104 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_105 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_106 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_107 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_108 &OldData );	//	데이터 포맷 변경 진행

    SCROWACTION& operator= ( const SCROWACTION &value );

    bool LOAD103 ( sc::BaseStream &SFile );
    bool LOAD104 ( sc::BaseStream &SFile );
    bool LOAD105 ( sc::BaseStream &SFile );
    bool LOAD106 ( sc::BaseStream &SFile );
    bool LOAD107 ( sc::BaseStream &SFile );
    bool LOAD108 ( sc::BaseStream &SFile );	//	데이터 포맷 변경 진행
    bool LOAD109 ( sc::BaseStream &SFile );	//	SCROWPATTERN::RANDOMPATTERNNUM -> 10으로 수정

    bool LOAD ( sc::BaseStream &SFile );
    bool SAVE ( sc::SerialFile &SFile );

    static VOID SaveCsvHead(std::fstream& SFile);
    VOID SaveCsv(std::fstream& SFile);
    VOID LoadCsv(CStringArray& StrArray, int& iCsvCur);

    //! 각종 행동
    inline DWORD GetActFlag() const { return m_dwActFlag; }
};

#endif // _CROW_ACTION_H_