#ifndef GLAROUNDSLOT_H_
#define GLAROUNDSLOT_H_

#pragma once

#include "../../EngineLib/Navimesh/NavigationMesh.h"

#ifndef GLLandMan
	class GLLandMan;
#endif //GLLandMan

#ifndef GLChar
	class GLChar;
	typedef GLChar* PGLCHAR;
#endif

#ifndef GLSummonField
	class GLSummonField;
	typedef GLSummonField* PGLSUMMONFIELD;
#endif

#ifndef GLCrow
	class GLCrow;
	typedef GLCrow* PGLCROW;
#endif

//	Note : 피공격시에 자신의 주변에 몰려들수 있는 위치가 있는지 점검.
//		하고 그 위치를 반환.
//
class GLARoundSlot
{
public:
	enum
	{
		SLOT_SIZE	= 30,
		AROUNDPOS_MINTHETA = 8,
	};

protected:
	PGLCHAR			m_pThisChar;
	PGLSUMMONFIELD	m_pThisSummon;

	DWORD		m_dwAmount; // 현재 캐릭터에 몰려있는 몹의 숫자
	DWORD		m_ARoundSlot[SLOT_SIZE];

	DWORD		m_dwLimitAmount;	// 제한 수; defaultcharclass 영향받는다;
	bool		m_bUseSummon;

public:
	void SetChar ( PGLCHAR pThisChar );
	void SetSummon ( PGLSUMMONFIELD pThisSummon );
	void SetUseSummon ( bool bUseSummon ) { m_bUseSummon = bUseSummon; }
	D3DXVECTOR3 CalculateTracePos(
        const int nDegree,
        const D3DXVECTOR3& vThisPos,
		const float fRange);

protected:
	BOOL ValidPosition ( boost::shared_ptr<NavigationMesh> spNaviMesh, const D3DXVECTOR3 &vTarPos, const DWORD dwTarCell, const D3DXVECTOR3 &vTracePos );

public:
	DWORD GetAmount ()		{ return m_dwAmount; }
	void SetTargetOptSlot (  const DWORD dwAttID, const DWORD dwSlot  );
	void ResetTargetOptSlot ( const DWORD dwAttID, const DWORD dwSlot );

	DWORD TestOptimumSlot ( PGLCROW pCrowAttackor, float fRange, boost::shared_ptr<NavigationMesh> spNaviMesh, D3DXVECTOR3 &vTracePos );
	BOOL ValidOptimumSlot ( boost::shared_ptr<NavigationMesh> spNaviMesh, const D3DXVECTOR3 &vTracePos );

	DWORD ReqOptimumSlot ( PGLCROW pCrowAttackor, float fRange, boost::shared_ptr<NavigationMesh> spNaviMesh, D3DXVECTOR3 &vTracePos );

	void Reset();
	bool Find(const DWORD dwGaeaID);

public:
	void Update ( GLLandMan* pLandMan );

public:
	GLARoundSlot(void);
	~GLARoundSlot(void);
};

#endif // GLAROUNDSLOT_H_