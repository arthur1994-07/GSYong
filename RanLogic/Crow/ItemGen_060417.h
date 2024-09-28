#ifndef _ITEM_GEN_060417_H_
#define _ITEM_GEN_060417_H_

#pragma once

//! �ټ� ������ �߻��� ó�� ����.
struct SGENITEM_060417
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

    SGENITEM_060417()
        : m_fGenRate(0)
    {
    }

    BOOL LOADFILE(const char* szFileName);

    BOOL ISGEN(float fGenRate = 1.0f);	//	�߻� Ȯ��.
    WORD SELECTNUM();					//	�߻� ����.
    DWORD SELECTSPECID(float fGEN_RATE, bool& bSPECID);	//	�߻� ��.
};

#endif // _ITEM_GEN_060417_H_