#pragma once

class CLPatchThread : public CWinThread
{
	DECLARE_DYNCREATE(CLPatchThread)

protected:
	CLPatchThread();

public:
	virtual ~CLPatchThread();
	virtual void Delete();

protected:
	virtual void ThreadMain(){}
	virtual BOOL InitInstance();

	void SetComplete(){ m_bComplete = TRUE; }

public:
	void KillThread();
	void ForceKillThread();

	BOOL IsFail () { return m_bFail; }
	void SetFail () { m_bFail = TRUE; }

	BOOL IsComplete() { return m_bComplete; }

	void Pause();
	void Resume();

	BOOL IsRunning ();
	BOOL IsForceTerminate ();
	BOOL IsPause();

	BOOL IsExtractError () { return m_bExtractError; }

	void WatingForResume();

protected:
	BOOL m_bFail;
	BOOL m_bComplete;
	BOOL m_bExtractError;

public:
	HANDLE m_hEventResume;
	HANDLE m_hEventKill;
	HANDLE m_hEventDead;

public:
	int m_nDownloadFileCount;
	int m_nDownloadedFileCount;

protected:
	DECLARE_MESSAGE_MAP()
};