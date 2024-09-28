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
	BOOL				    m_bNewHeadDRAW;		// �ڽ��� ���� �� ���� �ְ�, �ٸ� ������ ���� �� ���� �ִ�.
	LPDIRECT3DTEXTUREQ	    m_rFaceTex;			// �� �ؽ��ĸ� �ٲ� ��� �̰����� �ٲ۴�.
	DWORD				    m_dwEdgeColor;		// ���� �ܰ��� ��.
	DWORD				    m_dwCTShadeColor;	// �� �� ���̵� ��.
	LPDIRECT3DTEXTUREQ	    m_rToonShadeTex;	// �� �� �ؽ���.
    SKINEFF_LEVEL_DATA*     m_rLevelData;       // Level���� ���̴� ������ - ����

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