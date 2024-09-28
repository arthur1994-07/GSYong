//#include "OutPutStream.h"

#ifndef	__C_DEBUGSET__
#define	__C_DEBUGSET__

#define _ON_DEBUGSET

#include "../Common/gassert.h"
#include <string>

class CD3DFontPar;
class SUBPATH;

namespace CDebugSet
{
	#define ToViewP(arg) (ToView(PSTREAM(arg)))

#ifdef _ON_DEBUGSET
	//	Note	:	�������� ȭ�鿡 �Ѹ�
	//	Usage	:	printf () ������ ����		
	void ToView ( const char *szFormat, ... );
	
	//	Note	:	�������� �Ѹ���, ������ ������
	//				������ 0�����κ��� ������
	void ToView ( int line, const char *szFormat, ... );
	void ToView ( int nChannel, int line, const char *szFormat, ... );
	void ToListView ( const char *szFormat, ... );

	void ToPos ( float x, float y, const char *szFormat, ... );
#else
	inline void ToView ( const char *szFormat, ... ) {}
	inline void ToView ( int line, const char *szFormat, ... ) {}
	inline void ToView ( int nChannel, int line, const char *szFormat, ... ) {}
	inline void ToListView ( const char *szFormat, ... ) {}
	inline void ToPos ( float x, float y, const char *szFormat, ... ) {}
#endif // _ON_DEBUGSET

	//! ���Ϸ� ���
    void ToLogFile(std::string& Message, BOOL bSaveCSV=TRUE);    

	// Ʃ�丮�� �� ���� �α�
	void ToTutorialMapFile( const char *szFormat, ... );
	// �ν���Ʈ ���� ���� �α�
	void ToInstantMapFile( const char *szFormat, ... );
	// ���� �α׸� ���涧 �����
	void ToTracingFile ( const char *szAccountName, const char *szFormat, ... );
	// ��ŷ�� �ǽɵ� ��� ���� �α�
	void ToHackingFile( const char *szFormat, ... );
	// 10���̻� �� ������Ʈ�� �α׸� ����
	void ToBillionUpdateFile( const char *szFormat, ... );
	// Ŭ�� ���� ����� �α׸� ����
	void ToClubNoticeFile( const char *szFormat, ... );

	// �� �α�
	void ToBlockDetectFile( const char *szFormat, ... );
	// Ŭ�� ���� �α�
	void ToClubIncomeFile( const char *szFormat, ... );
	// CDM �α�
	void ToCDMLogFile( const char *szFormat, ... );
	// �������ϸ� �α�
	void ToProFileLogFile( const char *szFormat, ... );
	// �������ϸ� �޼����α�
	void ToProFileMsgFile( const char *szFormat, ... );
	// Ȯ�� �������� �α׿�
	void ToProfileExLogFile( const char *szFormat, ... );

	//Game Ŭ���̾�Ʈ ���� üũ
	void ToStateCheckFile ( const char *szFormat, ... );

	void ToFile ( const char *szFileName, const char *szFormat, ... );
	void ToFileWithTime ( const char* szFileName, const char* szFormat, ... );
	void ClearFile ( const char *szFileName );
	
	const char* GetPath ();
	std::string GetCurTime ();

	//! ����������� ���Ϸ� ����� ��, �����θ� �����ؾ� �Ѵ�.
	//! ȭ������ ������ ����ϱ� ����, Font�� �����Ѵ�.
	HRESULT OneTimeSceneInit(const SUBPATH* pPath, bool bLogFileFinalOpen/*=false*/);
	HRESULT	InitDeviceObjects ( CD3DFontPar* pFont );
	HRESULT FinalCleanup();


    void CleanLogFile();
	void SetLogFileFinalOpen( bool bUse );
	void SetSubjectName_ERROR( BOOL bUse );

	void FrameMove ( float fTime, float fETime );
	void Render ();

	// �ӽ� �׽�Ʈ��
	void TempOnOff(bool bOn, const SUBPATH* pPath, const char* szFileName); //= NULL);
	void TempToFile( const char* szFormat, ... );
}

namespace CLOCKCHECK
{
	extern DWORD dwShi, dwSlo, dwEhi, dwElo;
	inline void CLOCKCHK_ST ()
	{
		__asm
		{
			rdtsc                      ;Ŭ��ī��Ʈ�� edx:eax�������Ϳ� ����
			mov     dwShi, edx       ;���� dword �޸𸮿� ���
			mov     dwSlo, eax       ;���� dword �޸𸮿� ���       
		}
	}

	inline void CLOCKCHK_ED ( DWORD &dwHigh, DWORD &dwLow )
	{
		__asm
		{
			rdtsc                      ; Ŭ��ī��Ʈ�� edx:eax�������Ϳ� ����
			mov     dwElo, eax       ; ���� dword �� ����
			mov     dwEhi, edx       ; ���� dword�� ����
		}

		dwHigh = dwEhi - dwShi;
		dwLow = dwElo - dwSlo;
	}
}

#endif	//	__C_DEBUGSET__