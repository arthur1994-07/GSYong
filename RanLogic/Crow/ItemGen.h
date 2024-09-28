#ifndef _ITEM_GEN_110722_H_
#define _ITEM_GEN_110722_H_

#pragma once

#include "../../EngineLib/G-Logic/TypeDefine.h"

#include "ItemGenType.h"

#include <string>
#include <vector>
#include <boost/tr1/memory.hpp>
#include <boost/function.hpp>

//! �ټ� ������ �߻��� ó�� ����.
//! 11-07-22(bjju) �Ϲ� �����۰� QITEM, �ݾ׵� Ŭ���� �ϳ��� �����Ͽ����ϴ�.
//                 Load�ÿ� ������ ������ �̸��� ;��ū���� �и��ϸ�
//                 ������ ������ ���ϵ��� �������� Ȯ���� ������ ����˴ϴ�.


//! �߻� ������
struct SGENINFO
{
    typedef std::vector<SGENNUM>                               VEC_SGENNUM;
    typedef VEC_SGENNUM::iterator                              VEC_SGENNUM_ITER;
    typedef VEC_SGENNUM::const_iterator                        VEC_SGENNUM_CITER;

    typedef std::vector<std::tr1::shared_ptr<SGENITEMBASE> >   VEC_SPGENITEM;
    typedef VEC_SPGENITEM::iterator                            VEC_SPGENITEM_ITER;
    typedef VEC_SPGENITEM::const_iterator                      VEC_SPGENITEM_CITER;

    EMGENINFOTYPE  emType;       // ���� Ÿ��
    DWORD          dwVersion;    // ����
    float          fProbability; // ����� �⺻ Ȯ��
    VEC_SGENNUM    vecGenNum;    // ����� ���� ����
    VEC_SPGENITEM  vecGenItem;   // ����� ������ ����

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
	//�����Ҷ� �ּ������� ��Ʈ�� ���Ͱ� �߰���, ũ�Ⱑ 0�̶�� �߰� ���� �����Ƿ� �ּ� ���ʿ�� ���ڸ� �����ָ��
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

    //! �̺�Ʈ�� ������� Ȯ���� ���������ٶ� �� ����� ����մϴ�.
    //
    FNGENPROB     m_fnGenProb;   // ����� Ȯ�� ���� �Լ�
    FNGENITEMRATE m_fnGenRate;   // ������ ��� ���� ���� �Լ�
    FNGENRESULT   m_fnGenResult; // ����� ���� �Լ�
    FNGENITEMLOG  m_fnGenLog;    // ��� ������ �α� �Լ�
	
    void          SetGenProbCallback   ( FNGENPROB     fnCallback ) { m_fnGenProb   = fnCallback; }
    void          SetGenRateCallback   ( FNGENITEMRATE fnCallback ) { m_fnGenRate   = fnCallback; }
    void          SetGenResultCallback ( FNGENRESULT   fnCallback ) { m_fnGenResult = fnCallback; }
    void          SetGenLogCallback    ( FNGENITEMLOG  fnCallback ) { m_fnGenLog    = fnCallback; }
	*/


/*
public:
     //! �߻��� ������ ���
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
//! �ټ� ������ �߻��� ó�� ����.
class CGENITEM
{
    enum
    {
        EMGNUM		= 5,
        EMGSPEC		= 30,
        EMSPEC_NULL	= -1
    };

    struct SGNUM	//	�߻� ������ Ȯ��.
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

    struct SGITEMSPEC	//	�߻� �����۱��� Ȯ��.
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

    float		m_fGenRate;				//	�߻� Ȯ��.
    SGNUM		m_sGenNum[EMGNUM];		//	�߻� ������ Ȯ�� ����.
    SGITEMSPEC	m_sItemSpec[EMGSPEC];	//	�߻� ������ ��.

    CGENITEM()
        : m_fGenRate(0)
    {
    }

    BOOL LOADFILE(const char* szFileName);

    BOOL ISGEN(float fGenRate = 1.0f);	//	�߻� Ȯ��.
    WORD SELECTNUM();					//	�߻� ����.
    DWORD SELECTSPECID(float fGEN_RATE, bool& bSPECID);	//	�߻� ��.
};
*/

#endif // _ITEM_GEN_110722_H_
