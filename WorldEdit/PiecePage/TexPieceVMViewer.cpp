// ./PiecePage/TexPieceVMViewer.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../WorldEdit.h"
#include "TexPieceVMViewer.h"


// CTexPieceVMViewer ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CTexPieceVMViewer, CDialog)

CTexPieceVMViewer::CTexPieceVMViewer(CWnd* pParent /*=NULL*/)
	: CDialog(CTexPieceVMViewer::IDD, pParent)
{

}

CTexPieceVMViewer::~CTexPieceVMViewer()
{
}

void CTexPieceVMViewer::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_TEX_PIECE_VM, m_ctrlViewer);
}


BEGIN_MESSAGE_MAP(CTexPieceVMViewer, CDialog)
END_MESSAGE_MAP()


// CTexPieceVMViewer �޽��� ó�����Դϴ�.

BOOL CTexPieceVMViewer::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
    RECT listRect;
	m_ctrlViewer.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[3] = { int(nListWidth*0.333f), int(nListWidth*0.333f), int(nListWidth*0.333f) };

	m_ctrlViewer.SetExtendedStyle ( m_ctrlViewer.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_ctrlViewer.InsertColumn ( 0, _T("Texture"),	LVCFMT_CENTER, nColWidth[0] );
	m_ctrlViewer.InsertColumn ( 1, _T("Piece"),	LVCFMT_CENTER, nColWidth[1] );
	m_ctrlViewer.InsertColumn ( 2, _T("VisualMaterial"),	LVCFMT_CENTER, nColWidth[2] );

    int i=0;
    SET_DATA_CITER iter=m_setData.begin();
	for ( ; iter!=m_setData.end(); ++iter, ++i )
	{
        m_ctrlViewer.InsertItem( i, (*iter).m_strTexture.c_str() );
		m_ctrlViewer.SetItemText( i, 1, (*iter).m_strPiece.c_str() );
		m_ctrlViewer.SetItemText( i, 2, (*iter).m_strVM.c_str() );
	}

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CTexPieceVMViewer::SetData( const TSTRING& strTexture, const TSTRING& strPiece, const TSTRING& strVM )
{
    DATA sData;
    sData.m_strTexture = strTexture;
    sData.m_strPiece = strPiece;
    sData.m_strVM = strVM;
    m_setData.insert( sData );
}