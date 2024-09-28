#ifndef STL_FUNC_H_
#define STL_FUNC_H_

#pragma once

#include "../EngineLib/G-Logic/GLDefine.h"
#include "./GLogicEx.h"
#include "./Summon/GLSummon.h"

namespace std_afunc
{	
	class CCompareStarDist
	{
	public:
		CCompareStarDist()
		{
		}

	public:
		bool operator() (const STARDIST& StarA, const STARDIST& StarB)
		{
			return StarA.m_fDIST < StarB.m_fDIST;
		}
	};

	class CCompareTargetDist
	{
	public:
		D3DXVECTOR3 vFromPt;

	public:
		CCompareTargetDist(const D3DXVECTOR3& vFrom) : vFromPt(vFrom) {}

	public:
		bool operator() (const STARGETID& TarA, const STARGETID& TarB)
		{
			D3DXVECTOR3 vLengA = TarA.vPos - vFromPt;
			D3DXVECTOR3 vLengB = TarB.vPos - vFromPt;
			//float fLengA = D3DXVec3Length(&vLengA);
			//float fLengB = D3DXVec3Length(&vLengB);
            float fLengA = D3DXVec3LengthSq(&vLengA);
            float fLengB = D3DXVec3LengthSq(&vLengB);			
			return fLengA < fLengB;
		}

		bool operator() (const STARGETID* pTarA, const STARGETID* pTarB)
		{
			D3DXVECTOR3 vLengA = pTarA->vPos - vFromPt;
			D3DXVECTOR3 vLengB = pTarB->vPos - vFromPt;
			//float fLengA = D3DXVec3Length(&vLengA);
			//float fLengB = D3DXVec3Length(&vLengB);
            float fLengA = D3DXVec3LengthSq(&vLengA);
            float fLengB = D3DXVec3LengthSq(&vLengB);			
			return fLengA < fLengB;
		}
	};

	class CCompareSelectTargetDist
	{
	public:
		TARIDRC_SET& setTargetList;
		D3DXVECTOR3 vFromPt;

	public:
		CCompareSelectTargetDist( TARIDRC_SET& _setTargetList, const D3DXVECTOR3& vFromPt) 
			: setTargetList(_setTargetList)
			, vFromPt(vFromPt) {}

	public:
		bool operator() (const STARGETID& TarA, const STARGETID& TarB)
		{
			TARIDRC_SET_ITER findA = setTargetList.find(TarA);
			TARIDRC_SET_ITER findB = setTargetList.find(TarB);

			if ( findA == setTargetList.end() || findB == setTargetList.end() )
			{
				if ( findA != setTargetList.end() )
				{
					return false;
				}

				if ( findB != setTargetList.end() )
				{
					return true;
				}
			}
			else
			{
				const STARGETIDREFCOUNTER& sTargetA = (*findA);
				const STARGETIDREFCOUNTER& sTargetB = (*findB);

				if ( sTargetA.nRefCount != sTargetB.nRefCount )
				{
					return sTargetA.nRefCount < sTargetB.nRefCount;
				}
			}

			D3DXVECTOR3 vLengA = TarA.vPos - vFromPt;
			D3DXVECTOR3 vLengB = TarB.vPos - vFromPt;
			float fLengA = D3DXVec3LengthSq(&vLengA);
			float fLengB = D3DXVec3LengthSq(&vLengB);			
			return fLengA < fLengB;
		}
	};
}

#endif // STL_FUNC_H_