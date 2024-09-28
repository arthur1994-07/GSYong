#ifndef _ITEM_GEN_060417_H_
#define _ITEM_GEN_060417_H_

#pragma once

//! 다수 아이템 발생시 처리 구조.
struct SGENITEM_060417
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

    SGENITEM_060417()
        : m_fGenRate(0)
    {
    }

    BOOL LOADFILE(const char* szFileName);

    BOOL ISGEN(float fGenRate = 1.0f);	//	발생 확율.
    WORD SELECTNUM();					//	발생 갯수.
    DWORD SELECTSPECID(float fGEN_RATE, bool& bSPECID);	//	발생 군.
};

#endif // _ITEM_GEN_060417_H_