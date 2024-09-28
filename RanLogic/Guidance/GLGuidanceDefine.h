#ifndef _GL_GUIDANCE_DEFINE_H_
#define _GL_GUIDANCE_DEFINE_H_

#pragma once

#include <vector>
#include "../Character/GLCharDefine.h"

struct GLGUID_DB
{
    DWORD	m_dwID;				//	선도 지역 ID.
    DWORD	m_dwCLUBID;			//	선도 클럽 ID.
    float	m_fCommissionPer;	//	수수료율.

    GLGUID_DB()
        : m_dwID(UINT_MAX)
        , m_dwCLUBID(CLUB_NULL)
        , m_fCommissionPer(0)
    {
    }
};

typedef std::vector<GLGUID_DB> VECGUID_DB;

enum EMCHECKCERTIFY
{
    EMCHECKCERTIFY_FAIL			= 0,
    EMCHECKCERTIFY_OK			= 1,
    EMCHECKCERTIFY_DOING		= 2,
    EMCHECKCERTIFY_OTHERDOING	= 3,
    EMCHECKCERTIFY_NOTBATTLE	= 4,
};

#endif // _GL_GUIDANCE_DEFINE_H_