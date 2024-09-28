#ifndef _STL_FUNCTION_SERVER_H_
#define _STL_FUNCTION_SERVER_H_

#pragma once

namespace std_afunc
{
    struct CHARDIST
    {
        float	fLeng;
        PGLCHAR	pChar;

        CHARDIST() {}
        CHARDIST(D3DXVECTOR3 vCenter, PGLCHAR _pChar)
            : pChar(_pChar)
        {
            D3DXVECTOR3 vLeng = pChar->GetPosition() - vCenter;
            fLeng = D3DXVec3LengthSq(&vLeng);
        }

        CHARDIST(float _fLeng, PGLCHAR _pChar)
            : fLeng(_fLeng)
            , pChar(_pChar)
        {
        }
    };

    class CCompareCharDist
    {
    public:
        CCompareCharDist ()
        {
        }

    public:
        bool operator() ( const CHARDIST &CharA, const CHARDIST &CharB )
        {
            return CharA.fLeng < CharB.fLeng;
        }
    };

    struct CROWDIST
    {
        float	fLeng;
        PGLCROW	pCrow;

        CROWDIST () {}
        CROWDIST ( D3DXVECTOR3 vCenter, PGLCROW _pCrow ) :
        pCrow(_pCrow)
        {
            D3DXVECTOR3 vLeng = pCrow->GetPosition() - vCenter;
            fLeng = D3DXVec3LengthSq ( &vLeng );
        }

        CROWDIST ( float _fLeng, PGLCROW _pCrow ) :
        fLeng(_fLeng),
            pCrow(_pCrow)
        {
        }
    };

    class CCompareCrowDist
    {
    public:
        CCompareCrowDist ()
        {
        }

    public:
        bool operator() ( const CROWDIST &CharA, const CROWDIST &CharB )
        {
            return CharA.fLeng < CharB.fLeng;
        }
    };

    struct SUMMONDIST
    {
        float	        fLeng;
        PGLSUMMONFIELD	pSummon;

        SUMMONDIST () {}
        SUMMONDIST ( D3DXVECTOR3 vCenter, PGLSUMMONFIELD _pSummon ) :
        pSummon(_pSummon)
        {
            D3DXVECTOR3 vLeng = _pSummon->GetPosition() - vCenter;
            fLeng = D3DXVec3LengthSq ( &vLeng );
        }

        SUMMONDIST ( float _fLeng, PGLSUMMONFIELD _pSummon ) :
        fLeng(_fLeng),
            pSummon(_pSummon)
        {
        }
    };

    class CCompareSummonDist
    {
    public:
        CCompareSummonDist ()
        {
        }

    public:
        bool operator() ( const SUMMONDIST &SummonA, const SUMMONDIST &SummonB )
        {
            return SummonA.fLeng < SummonB.fLeng;
        }
    };

} // namespace std_afunc

#endif // _STL_FUNCTION_SERVER_H_