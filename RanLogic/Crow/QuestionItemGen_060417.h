#ifndef _QUESTION_ITEM_GEN_060417_H_
#define _QUESTION_ITEM_GEN_060417_H_

#pragma once

#include "../../EngineLib/G-Logic/GLDefine.h"

//! Question ������ �߻��� ó�� ���� by ���
struct SQTGENITEM_060417
{
    enum { EMQTMAX = 20 };

    struct ITEM
    {
        SNATIVEID	m_sGenItemID;
        float		m_fGenRate;

        ITEM()
            : m_sGenItemID(false)
            , m_fGenRate(0)
        {
        }
    };

    ITEM	m_sItem[EMQTMAX];
    int		m_nItemNum;

    SQTGENITEM_060417()
        : m_nItemNum(0)
    {
    }

    BOOL LOADFILE(const char * szFileName);
    SNATIVEID SELECTITEM(); // ������ �߻�
};

#endif // _QUESTION_ITEM_GEN_060417_H_