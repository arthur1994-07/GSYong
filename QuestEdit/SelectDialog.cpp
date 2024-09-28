// SelectDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "./SelectDialog.h"


// CSelectDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSelectDialog, CDialog)
CSelectDialog::CSelectDialog(CWnd* pParent /*=NULL*/)
: CDialog(CSelectDialog::IDD, pParent)
, m_nSelect( -1 )
, m_dwData ( 0 )
{
}

CSelectDialog::~CSelectDialog()
{
}

void CSelectDialog::DoDataExchange(CDataExchange* pDX)
{
    DDX_Control(pDX, IDC_SELECT_LIST, m_SelectList);
}


BEGIN_MESSAGE_MAP(CSelectDialog, CDialog)
END_MESSAGE_MAP()

BOOL CSelectDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_SelectList.ResetContent();

    //	문자열 초기화
    MAP_STR_LIST_ITER pos = m_mapStrList.begin();
    MAP_STR_LIST_ITER pos_end = m_mapStrList.end();

    for (; pos != pos_end; ++pos )
    {
        std::string strText = pos->second;
        DWORD dwData = pos->first;
        int nIndex = m_SelectList.AddString( strText.c_str() );
        m_SelectList.SetItemData( nIndex, dwData );
    }

    if ( m_nSelect >= 0 || m_nSelect  < m_SelectList.GetCount() )
        m_SelectList.SetCurSel( m_nSelect );

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSelectDialog::OnOK()
{
    m_nSelect = m_SelectList.GetCurSel();
    if ( m_nSelect == -1 )
    {
        CDialog::OnCancel(); 
        return;
    }

    m_dwData = (DWORD) m_SelectList.GetItemData ( m_nSelect );

    CDialog::OnOK();
}

void CSelectDialog::SetStringList( std::vector<std::string>& vecQuestArea )
{
    m_mapStrList.clear();

    for ( int i = 0; i < static_cast<int>(vecQuestArea.size()); ++i )
    {
        m_mapStrList.insert( std::make_pair ( i, vecQuestArea.at( i ) ) );
    }
}

void CSelectDialog::SetStringList( std::string *strTexts, int nNum )
{
    m_mapStrList.clear();

    for ( int i = 0; i < nNum; ++i )
    {
        m_mapStrList.insert( std::make_pair (i,strTexts[i]) );
    }
}

void CSelectDialog::DelStringList( int dwData )
{
    MAP_STR_LIST_ITER pos = m_mapStrList.find( dwData );
    if ( pos != m_mapStrList.end() )
    {
        m_mapStrList.erase( pos );
    }
}


