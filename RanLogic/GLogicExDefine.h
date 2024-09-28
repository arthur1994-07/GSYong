#ifndef _GLOGIC_EX_DEFINE_H_
#define _GLOGIC_EX_DEFINE_H_

#pragma once

#include <map>
#include "./Skill/GLSkill.h"

class STARDIST
{
public:
    float		m_fDIST;
    STARGETID	m_sTARGETID;

public:
    STARDIST()
    {
    }

    STARDIST(const float fDIST, const STARGETID& sTARID)
        : m_fDIST(fDIST)
        , m_sTARGETID(sTARID)
    {
    }

    bool operator < (const STARDIST& rvalue)
    {
        return m_fDIST < rvalue.m_fDIST;
    }
};

//typedef std::vector<STARDIST> TARGETMAP;
//typedef TARGETMAP::iterator	TARGETMAP_ITER;
typedef std::vector<STARDIST> TARDISTMAP;
typedef TARDISTMAP::iterator  TARDISTMAP_ITER;

typedef std::vector<STARGETID>						VEC_SK_TAR;
typedef VEC_SK_TAR::iterator							VEC_SK_ITER;
typedef std::pair<VEC_SK_ITER,VEC_SK_ITER>	VEC_SK_ITER_RANGE;

class STAR_ORDER
{
public:
    bool operator() ( const STARGETID &lvalue, const STARGETID &rvalue )
    {
        return lvalue.emCrow < rvalue.emCrow;
    }

    bool operator() ( const STARGETID &lvalue, const EMCROW emCROW )
    {
        return lvalue.emCrow < emCROW;
    }

    bool operator() ( const EMCROW emCROW, const STARGETID &rvalue )
    {
        return emCROW < rvalue.emCrow;
    }
};

//	Àû´ëÀÚ.
struct SPLAYHOSTILE
{
    BOOL	bBAD;
    float	fTIME;

    SPLAYHOSTILE ()
    {
        bBAD     = FALSE;
        fTIME    = 0;
    }
};

typedef std::map<DWORD, SPLAYHOSTILE*> MAPPLAYHOSTILE;
typedef MAPPLAYHOSTILE::iterator	   MAPPLAYHOSTILE_ITER;

#endif // _GLOGIC_EX_DEFINE_H_