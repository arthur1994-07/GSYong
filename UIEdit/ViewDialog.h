#pragma once

#include "../EngineLib/GUInterface/UIDataType.h"
#include "../EngineLib/DxTools/TextureManager.h"

class CViewDialog : public CDialog
{
	DECLARE_DYNAMIC(CViewDialog)

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_VIEW_DLG };

protected:
	TextureResource m_textureRes;
	CRect				m_cRect;
	CRect				m_cLimitRect;
	int					m_nSTART_POS_X;
	int					m_nSTART_POS_Y;
	int					m_nTEXTURE_START_POS_X;
	int					m_nTEXTURE_START_POS_Y;
	int					m_nTEXTURE_SIZE_WIDTH;
	int					m_nTEXTURE_SIZE_HEIGHT;
	float				m_fMultply;
	D3DCOLOR			m_BgColor;

public:
	CViewDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CViewDialog();

protected:
	virtual VOID DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	VOID Render();
	VOID DrawSprite( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID DrawLimitRect( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID DrawRect( LPDIRECT3DDEVICEQ pd3dDevice );

	VOID CloseWindow();

public:
	VOID SetStartPos(int nX, int nY);
	VOID SetTextureStartPos(int nX, int nY);
	VOID SetTextureSize(int nWidth, int nHeight);
	VOID SetMultiply(float fMultply);
	VOID SetTexture( TextureResource textureRes )	{ m_textureRes = textureRes; }
	VOID SetRect( CRect cRect )						{ m_cRect = cRect; }
	VOID SetLimitRect( CRect cRect )				{ m_cLimitRect = cRect; }
	VOID SetBgColor(UINT r, UINT g, UINT b);
	D3DCOLOR GetBgColor();

	VOID SetVertexPos( 
		UIVERTEX* pVertexArray, 
		FLOAT LEFT, 
		FLOAT TOP, 
		FLOAT SIZEX, 
		FLOAT SIZEY );

	VOID SetTexturePos( 
		UIVERTEX* pVertexArray, 
		FLOAT LEFT, 
		FLOAT TOP, 
		FLOAT RIGHT, 
		FLOAT BOTTOM );

};
