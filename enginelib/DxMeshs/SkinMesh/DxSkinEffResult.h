#pragma once

struct SKINEFF_LEVEL_DATA
{
    BOOL m_bLevel;
    BOOL m_bLevelFlow;
    D3DXVECTOR3 m_vLevelColor;
    D3DXVECTOR3 m_vLevelFlowColor;

    SKINEFF_LEVEL_DATA()
        : m_bLevel(FALSE)
        , m_bLevelFlow(FALSE)
    {
    }
};

struct SKINEFFDATA
{
	BOOL				    m_bNewHeadDRAW;		// 자신이 렌더 될 수도 있고, 다른 곳에서 렌더 할 수도 있다.
	LPDIRECT3DTEXTUREQ	    m_rFaceTex;			// 얼굴 텍스쳐를 바꿀 경우 이것으로 바꾼다.
	DWORD				    m_dwEdgeColor;		// 툰의 외각선 색.
	DWORD				    m_dwCTShadeColor;	// 툰 용 쉐이드 색.
	LPDIRECT3DTEXTUREQ	    m_rToonShadeTex;	// 툰 용 텍스쳐.
    SKINEFF_LEVEL_DATA*     m_rLevelData;       // Level에서 쓰이는 데이터 - 참조

	SKINEFFDATA()
        : m_bNewHeadDRAW(FALSE)
		, m_rFaceTex(NULL)
		, m_dwEdgeColor(0L)
		, m_dwCTShadeColor(0xffffffff)
		, m_rToonShadeTex(NULL)
        , m_rLevelData(NULL)
	{
	}

	void Reset()
	{
		m_bNewHeadDRAW = FALSE;
		m_rFaceTex = NULL;
		m_dwEdgeColor = 0L;
		m_dwCTShadeColor = 0xffffffff;
		m_rToonShadeTex = NULL;
	}
};