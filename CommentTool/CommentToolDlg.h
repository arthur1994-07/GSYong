// CommentToolDlg.h : 헤더 파일
//

#pragma once

#include <string>
#include <vector>
#include <map>

struct SCommentKeyData 
{
	std::string strSubKey;
	std::vector< std::string > strData;
};

struct SCommentData 
{
	std::string strMainKey;
	std::vector< SCommentKeyData > vecKeyData;

	void RESET()
	{
		strMainKey.clear();
		vecKeyData.clear();
	}
};

// CCommentToolDlg 대화 상자
class CCommentToolDlg : public CDialog
{
	enum
	{
		MAX_EDIT_NUM = 20,	
	};
private:
	typedef std::string							STRING;
	typedef STRING::size_type					STRING_SIZE;

	typedef std::vector<STRING>					VECSTRING;
	typedef VECSTRING::iterator					VECSTRING_ITER;

	typedef std::multimap<STRING,VECSTRING*>	MAPKEY;
	typedef MAPKEY::iterator					MAPKEY_ITER;

	typedef std::pair<MAPKEY_ITER,MAPKEY_ITER>	MAPKEY_PAIR;

	typedef std::map<STRING,MAPKEY*>			MAPSECTION;
	typedef MAPSECTION::iterator				MAPSECTION_ITER;

private:
	STRING m_strFileName;
	STRING m_fieldsep;			// 구분 문자들
	std::vector< SCommentData > m_vecCommentData;

	INT m_nMenuIndex;
	INT m_nTypeIndex;
	INT m_nDataIndex;

protected:
	bool IniFileLoad(const STRING& strFile, bool bDEC, BOOL bPack );
	bool IsNotes( const CString & line );
	bool IsSection( const CString & line, STRING & sec );
	int GetKeyData( const CString & line, STRING & key, VECSTRING * pVec );
	int split( const STRING & line, VECSTRING * pVec );
	int advquoted(const STRING & s, STRING & fld, UINT i);
	int advplain(const STRING & s, STRING & fld, UINT i );
	int findfirstof( const STRING & line, const STRING & sep, UINT i );

	bool IniFileSave( CString& strFile );

	void ResetDlg();
	void Update();

// 생성
public:
	CCommentToolDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_COMMENTTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeMenuList();
	afx_msg void OnFileLoad();
	afx_msg void OnLbnSelchangeTypeList();
	afx_msg void OnLbnSelchangeDataList();
	afx_msg void OnLbnDblclkDataList();
	afx_msg void OnFileSave();
	afx_msg void OnFileIniToLua();
//	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
