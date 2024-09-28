#pragma once
#include "afxcmn.h"

#include <set>

// CTexPieceVMViewer ��ȭ �����Դϴ�.

class CTexPieceVMViewer : public CDialog
{
	DECLARE_DYNAMIC(CTexPieceVMViewer)

private:
    struct DATA
    {
        TSTRING m_strTexture;
        TSTRING m_strPiece;
        TSTRING m_strVM;

        bool operator < ( const DATA& rhs ) const
        {
            if ( std::make_pair(m_strTexture,m_strPiece) < std::make_pair(rhs.m_strTexture,rhs.m_strPiece) )
                return true;
            else
                return false;
        }
    };

    /*
    struct NAME_LMID
{
    std::string m_strName;      // �ؽ��� �̸�
    DWORD       m_dwLightMapID; // m_strName �Ͱ��� Ʋ�� ����Ʈ�� ID
    
    bool operator < ( const NAME_LMID& rhs ) const
    {
        if ( std::make_pair(m_strName,m_dwLightMapID) < std::make_pair(rhs.m_strName,rhs.m_dwLightMapID) )
            return true;
        else
            return false;
    }

    bool operator != ( const NAME_LMID& rhs ) const
    {
        if ( std::make_pair(m_strName,m_dwLightMapID) != std::make_pair(rhs.m_strName,rhs.m_dwLightMapID) )
            return true;
        else
            return false;
    }

    NAME_LMID( const TSTRING& strName, DWORD dwLightMapID )
        : m_strName(strName)
        , m_dwLightMapID(dwLightMapID)
    {
    }
};
    */

    typedef std::set<DATA>                  SET_DATA;
    typedef std::set<DATA>::const_iterator  SET_DATA_CITER;

    SET_DATA m_setData;

public:
    void SetData( const TSTRING& strTexture, const TSTRING& strPiece, const TSTRING& strVM );

public:
	CTexPieceVMViewer(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTexPieceVMViewer();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_TEX_PIECE_VM_VIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
    CListCtrl m_ctrlViewer;
    virtual BOOL OnInitDialog();
};
