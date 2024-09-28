#ifndef _ITEM_GEN_110722_H_
#define _ITEM_GEN_110722_H_

#pragma once

#include "../../EngineLib/G-Logic/TypeDefine.h"

#include "ItemGenType.h"

#include <string>
#include <vector>
#include <boost/tr1/memory.hpp>
#include <boost/function.hpp>

//! 다수 아이템 발생시 처리 구조.
//! 11-07-22(bjju) 일반 아이템과 QITEM, 금액등 클래스 하나로 통합하였습니다.
//                 Load시에 아이템 파일의 이름을 ;토큰으로 분리하며
//                 각각의 아이템 파일들은 독립적인 확률을 가지고 드랍됩니다.


//! 발생 총정보
struct SGENINFO
{
    typedef std::vector<SGENNUM>                               VEC_SGENNUM;
    typedef VEC_SGENNUM::iterator                              VEC_SGENNUM_ITER;
    typedef VEC_SGENNUM::const_iterator                        VEC_SGENNUM_CITER;

    typedef std::vector<std::tr1::shared_ptr<SGENITEMBASE> >   VEC_SPGENITEM;
    typedef VEC_SPGENITEM::iterator                            VEC_SPGENITEM_ITER;
    typedef VEC_SPGENITEM::const_iterator                      VEC_SPGENITEM_CITER;

    EMGENINFOTYPE  emType;       // 정보 타입
    DWORD          dwVersion;    // 버전
    float          fProbability; // 드롭할 기본 확률
    VEC_SGENNUM    vecGenNum;    // 드롭할 개수 정보
    VEC_SPGENITEM  vecGenItem;   // 드롭할 아이템 정보

    SGENINFO()
        : emType(EMGENINFOTYPE_NONE)
        , dwVersion(0)
        , fProbability(0.0f)
    {
    }

    void PushGenNum   ( const SGENNUM& sGenNum ) { vecGenNum.push_back(sGenNum); }
    void PushGenItem  ( std::tr1::shared_ptr<SGENITEMBASE>& spGenItem )  { vecGenItem.push_back(spGenItem); }
};


class CGENITEM
{

public:
    CGENITEM();
    ~CGENITEM();

public:
    bool LoadFile(const std::string& strFileList);
	bool SaveFile(const SGENINFO& sGenInfo, std::string &_FileName, const std::vector<std::string> &_Annotation);
	//저장할때 주석때문에 스트링 벡터가 추가됨, 크기가 0이라면 추가 하지 않으므로 주석 불필요시 인자만 맞춰주면됨
	bool GetGenInfo(const std::string& ParentsFileName, const std::string& strFileName, SGENINFO& sGenInfo);
    void AddGenInfo(const SGENINFO& sGenInfo);
	void GetGenInfoStr(CString &_Str, const SGENINFO& _GenInfo, const std::vector<std::string> &_Annotation);
    
public:
    bool AddInfoGenItem  ( const float fProb, const SNATIVEID& sItemID );
    bool AddInfoGenMoney ( const float fProb, const DWORD dwMoney );    

public:
	typedef std::vector<SGENINFO>        VEC_SGENINFO;
    typedef VEC_SGENINFO::iterator       VEC_SGENINFO_ITER;
    typedef VEC_SGENINFO::const_iterator VEC_SGENINFO_CITER;
    VEC_SGENINFO m_vecGenInfo; 

/*
private:
    typedef boost::function<float (EMGENINFOTYPE emInfoType, float fProb)>                      FNGENPROB;
    typedef boost::function<float (EMGENITEMTYPE emItemType, float fRate, WORD wGrade)>         FNGENITEMRATE;
    typedef boost::function<void  (EMGENITEMTYPE emItemType, float fRate, SGENRESULT& sResult)> FNGENITEMLOG;
    typedef boost::function<void  (OUT SGENRESULT&)>                                            FNGENRESULT;

    //! 이벤트나 드랍관련 확률을 증가시켜줄때 이 기능을 사용합니다.
    //
    FNGENPROB     m_fnGenProb;   // 드롭할 확률 조절 함수
    FNGENITEMRATE m_fnGenRate;   // 아이템 드롭 비율 조절 함수
    FNGENRESULT   m_fnGenResult; // 결과템 조절 함수
    FNGENITEMLOG  m_fnGenLog;    // 드롭 아이템 로그 함수
	
    void          SetGenProbCallback   ( FNGENPROB     fnCallback ) { m_fnGenProb   = fnCallback; }
    void          SetGenRateCallback   ( FNGENITEMRATE fnCallback ) { m_fnGenRate   = fnCallback; }
    void          SetGenResultCallback ( FNGENRESULT   fnCallback ) { m_fnGenResult = fnCallback; }
    void          SetGenLogCallback    ( FNGENITEMLOG  fnCallback ) { m_fnGenLog    = fnCallback; }
	*/


/*
public:
     //! 발생한 아이템 결과
    typedef std::vector<SGENRESULT> VEC_RESULT;
    typedef VEC_RESULT::iterator    VEC_RESULT_ITER;
    VEC_RESULT m_vecResult;



public:
    typedef VEC_RESULT_ITER iterator;
    iterator Generate ( GLCHARLOGIC* pChar );
    iterator begin () { return m_vecResult.begin(); }
    iterator end   () { return m_vecResult.end();   }

private:
	void       GenerateItem ( GLCHARLOGIC* pChar, SGENINFO::VEC_SPGENITEM& vecGenItem, OUT SGENRESULT& sResult );
*/

//private:
public:
    const WORD GenerateNum(const SGENINFO::VEC_SGENNUM& vecGenNum);
	const WORD GenerateNum_RandomOnce(const SGENINFO::VEC_SGENNUM& vecGenNum);
};

/*
//! 다수 아이템 발생시 처리 구조.
class CGENITEM
{
    enum
    {
        EMGNUM		= 5,
        EMGSPEC		= 30,
        EMSPEC_NULL	= -1
    };

    struct SGNUM	//	발생 갯수별 확율.
    {
        WORD	wNum;
        float	fRateL;
        float	fRateH;

        SGNUM () :
        wNum(0),
            fRateL(100.0f),
            fRateH(100.0f)
        {
        }
    };

    struct SGITEMSPEC	//	발생 아이템군별 확율.
    {
        DWORD	dwSPECID;
        float	fRateL;
        float	fRateH;
        bool	bSPECID;

        SGITEMSPEC () :
        dwSPECID(EMSPEC_NULL),
            fRateL(100.0f),
            fRateH(100.0f),
            bSPECID( false )
        {
        }
    };

    float		m_fGenRate;				//	발생 확율.
    SGNUM		m_sGenNum[EMGNUM];		//	발생 갯수별 확율 분포.
    SGITEMSPEC	m_sItemSpec[EMGSPEC];	//	발생 아이템 군.

    CGENITEM()
        : m_fGenRate(0)
    {
    }

    BOOL LOADFILE(const char* szFileName);

    BOOL ISGEN(float fGenRate = 1.0f);	//	발생 확율.
    WORD SELECTNUM();					//	발생 갯수.
    DWORD SELECTSPECID(float fGEN_RATE, bool& bSPECID);	//	발생 군.
};
*/

#endif // _ITEM_GEN_110722_H_
