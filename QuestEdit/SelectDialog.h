#pragma once

#include <map>
#include <vector>

#include "./Resource.h"
// CSelectDialog ��ȭ �����Դϴ�.

class CSelectDialog : public CDialog
{
    DECLARE_DYNAMIC(CSelectDialog)

public:
    CSelectDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
    virtual ~CSelectDialog();

    // ��ȭ ���� �������Դϴ�.
    enum { IDD = IDD_LIST_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

    DECLARE_MESSAGE_MAP()

public:
    typedef std::map<DWORD,std::string>	MAP_STR_LIST;
    typedef MAP_STR_LIST::iterator		MAP_STR_LIST_ITER;

    MAP_STR_LIST	m_mapStrList;

    int m_nSelect;
    DWORD m_dwData;

    CListBox m_SelectList;

    void	SetStringList( std::string *strTexts, int nNum );
    void	SetStringList( std::vector<std::string>& vecQuestArea );
    void	DelStringList( int dwData );

public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
};