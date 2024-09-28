#pragma once
// CDlgPiecePreView ��ȭ �����Դϴ�.

#include "../WorldEdit/resource.h"

#include "../EngineLib/DxLand/DxPiece/DxPieceView.h"
#include "../EngineLib/DxLand/DxPiece/DxPiece.h"
#include "../EngineLib/DxLand/DxPiece/DxPieceManagerPSF.h"

class CDlgPiecePreView : public CDialog
{
	//DECLARE_DYNAMIC(CDlgPiecePreView)

public:
	CDlgPiecePreView(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgPiecePreView();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_PIECE_PREVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreInitDialog();
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();


// D3DSetting
private:
	HRESULT Cleanup();

public:
	LPDIRECT3DDEVICEQ GetPieceD3DDevice();
	void SetPieceData(const TCHAR *szFile );
	void SetSPTData(const TCHAR *szFile );

public:
	HRESULT Setup();
	HRESULT Update(float timeDelta);
	void	SetLight();
	HRESULT Render();
	void	SetCameraEX();
	void	SetCamera(D3DXVECTOR3 vCenter, float fLenght);
	
	void	SetRotateLeft();
	void	SetRotateRight();
	void	SetRotate(int nAngle);
	void	SetRotateRandom();
    void    SetDefaultRandom();
	void	SetScaleUp();
	void	SetScaleDown();
	void	SetScale(float fScale);
	
	D3DXMATRIX GetPieceMatrix();

	//LPDIRECT3DDEVICEQ		d3ddevice;
	D3DPRESENT_PARAMETERS	d3dpp;

	D3DXVECTOR3		m_vEye;
	D3DXVECTOR3		m_vLookat;
	D3DXVECTOR3		m_vUp;
	D3DXVECTOR3		m_vDir;
	
	D3DXMATRIX		m_matWorld;
	int				m_nPieceAngle;
	float			m_fPieceScale;
    D3DXVECTOR2     m_vRandomScale;
    float			m_fFinalScale;  // RandomScale ���� ����� Scale - ������밪

	DxPieceEdit*	m_pPIECE;

	struct SpeedTreeInfo
	{
		TSTRING			strName;
		float			fLength;
		D3DXVECTOR3		vCenter;
	};

	SpeedTreeInfo	m_pSPT;

private:
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProj;
	CLIPVOLUME m_sCV;
};
