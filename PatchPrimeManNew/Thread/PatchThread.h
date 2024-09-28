#pragma once

#include <queue>

#include "../Define.h"
#include "./EventThread.h"
#include "../Cabinet.hpp"
#include "../Cabinet/Extract.hpp"

class CHttpPatch;

class PatchThread : EventThread
{
public:
	PatchThread( DWORD nDlgThreadID, CHttpPatch* pHttpPatch );
	virtual ~PatchThread();

protected:
	std::queue< PatchFile > m_Job;
	CHttpPatch* m_pHttpPatch;

public:
	static FILEVECTOR m_ExtractedFile;

public:
	void AddJob( PatchFile& File );
	bool Empty();
	bool ErrorOccurred() { return m_bError; }

	static BOOL OnBeforeCopySingleFile( Cabinet::CExtract::kCabinetFileInfo* pk_Info, void* p_Param );

public:	
	//! Virtual Function	
	virtual int process()	override;
	virtual void start()	override;
	virtual void end()		override;
	virtual void endHook()	override;

protected:
	int DownloadFile();

};