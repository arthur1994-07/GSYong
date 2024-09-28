///////////////////////////////////////////////////////////////////////////////
// ListLoader.h
//
// class CListLoader
//
// * History
// 2003.09.09 jgkim Create
//
// Copyright 2002-2004 (c) Mincoms. All rights reserved.                 
// 
// * Note
// Config file load class head file
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef _CLISTLOADER_H_
#define _CLISTLOADER_H_

#include <vector>
#include "DatabaseTable.h"

class CMainFrame;

class CListLoader
{
public:
	CListLoader(CMainFrame* pFrame);
	CListLoader(const char* filename, CMainFrame* pFrame);
	~CListLoader();
	
	void		SetDefault(void);
	int			Process(char* sLine);
	int			Load(const char* filename);		

    CMainFrame* m_pFrame;
	char	m_szSourcePath[256];
    CString m_strCrtPath;
	std::vector<CompressFile> m_vFile;
	std::vector<FolderList> m_vFolderList;

	int		m_nVerIncreaseCount;
public:
	
};

#endif // _CLISTLOADER_H_