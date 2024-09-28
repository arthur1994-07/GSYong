#pragma once
#include "afxcmn.h"

#include <set>

// CTexPieceVMViewer 대화 상자입니다.

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
    std::string m_strName;      // 텍스쳐 이름
    DWORD       m_dwLightMapID; // m_strName 것과는 틀린 라이트맵 ID
    
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
	CTexPieceVMViewer(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTexPieceVMViewer();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_TEX_PIECE_VM_VIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    CListCtrl m_ctrlViewer;
    virtual BOOL OnInitDialog();
};
