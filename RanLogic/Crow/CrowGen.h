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
    DWORD				m_dwGenMoney;				//	�߻� �ݾ�.
    SNATIVEID			m_sGenItemID;				//	�߻� ������.
    WORD				m_wGenMoney_Rate;			//	�߻� �ݾ� Ȯ��.
    WORD				m_wGenItem_Rate;			//	�߻� ������ Ȯ��.
};

struct SCROWGEN_101
{
    DWORD				m_dwGenMoney;				//	�߻� �ݾ�.
    SNATIVEID			m_sGenItemID;				//	�߻� ������.
    WORD				m_wGenMoney_Rate;			//	�߻� �ݾ� Ȯ��.
    WORD				m_wGenItem_Rate;			//	�߻� ������ Ȯ��.

    char				m_szGenItem[ACF_SZNAME];	//	�߻� ������ ���� ����.
};

struct SCROWGEN
{
    enum { VERSION = 0x0104, };

    //	�߻�������.
    WORD				m_wGenItem_Rate;			//	�߻� ������ Ȯ��.
    WORD				m_wGenMoney_Rate;			//	�߻� �ݾ� Ȯ��.
    SNATIVEID			m_sGenItemID;				//	�߻� ������.	
    DWORD				m_dwGenMoney;				//	�߻� �ݾ�.		

    typedef std::vector<std::string> VEC_GENITEM_FILE;
    typedef VEC_GENITEM_FILE::iterator VEC_GENITEM_FILE_ITER;
    typedef VEC_GENITEM_FILE::const_iterator VEC_GENITEM_FILE_CITER;

    VEC_GENITEM_FILE m_vecGeneralGenItem; // General Item ���� ���� ����Ʈ.
    VEC_GENITEM_FILE m_vecSpecialGenItem; // Special Item ���� ���� ����Ʈ.

    SCROWGEN();
    SCROWGEN(const SCROWGEN& value);
    
    void Assign(SCROWGEN_100& OldCrowGen);
    void Assign(SCROWGEN_101& OldCrowGen);
    SCROWGEN& operator = (const SCROWGEN& value);
    
    bool LOAD(sc::BaseStream& SFile);
    bool SAVE(sc::SerialFile& SFile);

    bool LOAD_0102(sc::BaseStream& SFile);	//	������ ���� ���� ����
    bool LOAD_0103(sc::BaseStream& SFile);	//	������ ���� ���� ����

    static VOID SaveCsvHead(std::fstream& SFile);
    VOID SaveCsv(std::fstream& SFile);
    VOID LoadCsv(CStringArray& StrArray, int& iCsvCur);

    void AddGeneralGenItemFileName( const std::string& strFileName );
    void AddSpecialGenItemFileName( const std::string& strFileName );
};

#endif // _GROW_GEN_H_