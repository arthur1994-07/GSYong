#ifndef _GL_ITEM_GENERATE_H_
#define _GL_ITEM_GENERATE_H_

#pragma once

namespace ITEM
{

    struct SGENERATE_100	//	데이터 포맷 변경 진행
    {
        DWORD dwSpecID;			//	아이템 분류 군.
        DWORD dwLimitTime;		//	생성 기준 시간.
        DWORD dwLimitTimeGen;	//	시간당 생성 제한 갯수.			
    };

    struct SGENERATE
    {
        enum { VERSION = 0x0101, SPECID_NULL = 0, LIMIT_NOUSE = 0 };

        DWORD dwSpecID;			//	아이템 분류 군.
        DWORD dwLimitTimeGen;	//	시간당 생성 제한 갯수.
        DWORD dwLimitTime;		//	생성 기준 시간.

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