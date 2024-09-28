#ifndef _GL_ITEM_GENERATE_H_
#define _GL_ITEM_GENERATE_H_

#pragma once

namespace ITEM
{

    struct SGENERATE_100	//	������ ���� ���� ����
    {
        DWORD dwSpecID;			//	������ �з� ��.
        DWORD dwLimitTime;		//	���� ���� �ð�.
        DWORD dwLimitTimeGen;	//	�ð��� ���� ���� ����.			
    };

    struct SGENERATE
    {
        enum { VERSION = 0x0101, SPECID_NULL = 0, LIMIT_NOUSE = 0 };

        DWORD dwSpecID;			//	������ �з� ��.
        DWORD dwLimitTimeGen;	//	�ð��� ���� ���� ����.
        DWORD dwLimitTime;		//	���� ���� �ð�.

        SGENERATE()
            : dwSpecID(SPECID_NULL)
            , dwLimitTime(LIMIT_NOUSE)
            , dwLimitTimeGen(LIMIT_NOUSE)
        {
        }

        void Assign(SGENERATE_100& sOld);

        static VOID SaveCsvHead(std::fstream& SFile);
        VOID SaveCsv(std::fstream& SFile);
        VOID LoadCsv(CStringArray &StrArray, int& iCsvCur);
    };

} // namespace ITEM
#endif // _GL_ITEM_GENERATE_H_