#ifndef _GROW_GEN_H_
#define _GROW_GEN_H_

#pragma once

#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../EngineLib/DxMeshs/SkinMesh/SAnimationDefine.h"

namespace sc
{
    class BaseStream;
    class SerialFile;
}


struct SCROWGEN_100
{
    DWORD				m_dwGenMoney;				//	발생 금액.
    SNATIVEID			m_sGenItemID;				//	발생 아이템.
    WORD				m_wGenMoney_Rate;			//	발생 금액 확율.
    WORD				m_wGenItem_Rate;			//	발생 아이템 확율.
};

struct SCROWGEN_101
{
    DWORD				m_dwGenMoney;				//	발생 금액.
    SNATIVEID			m_sGenItemID;				//	발생 아이템.
    WORD				m_wGenMoney_Rate;			//	발생 금액 확율.
    WORD				m_wGenItem_Rate;			//	발생 아이템 확율.

    char				m_szGenItem[ACF_SZNAME];	//	발생 아이템 설정 파일.
};

struct SCROWGEN
{
    enum { VERSION = 0x0104, };

    //	발생아이템.
    WORD				m_wGenItem_Rate;			//	발생 아이템 확율.
    WORD				m_wGenMoney_Rate;			//	발생 금액 확율.
    SNATIVEID			m_sGenItemID;				//	발생 아이템.	
    DWORD				m_dwGenMoney;				//	발생 금액.		

    typedef std::vector<std::string> VEC_GENITEM_FILE;
    typedef VEC_GENITEM_FILE::iterator VEC_GENITEM_FILE_ITER;
    typedef VEC_GENITEM_FILE::const_iterator VEC_GENITEM_FILE_CITER;

    VEC_GENITEM_FILE m_vecGeneralGenItem; // General Item 설정 파일 리스트.
    VEC_GENITEM_FILE m_vecSpecialGenItem; // Special Item 설정 파일 리스트.

    SCROWGEN();
    SCROWGEN(const SCROWGEN& value);
    
    void Assign(SCROWGEN_100& OldCrowGen);
    void Assign(SCROWGEN_101& OldCrowGen);
    SCROWGEN& operator = (const SCROWGEN& value);
    
    bool LOAD(sc::BaseStream& SFile);
    bool SAVE(sc::SerialFile& SFile);

    bool LOAD_0102(sc::BaseStream& SFile);	//	데이터 포맷 변경 진행
    bool LOAD_0103(sc::BaseStream& SFile);	//	데이터 포맷 변경 진행

    static VOID SaveCsvHead(std::fstream& SFile);
    VOID SaveCsv(std::fstream& SFile);
    VOID LoadCsv(CStringArray& StrArray, int& iCsvCur);

    void AddGeneralGenItemFileName( const std::string& strFileName );
    void AddSpecialGenItemFileName( const std::string& strFileName );
};

#endif // _GROW_GEN_H_