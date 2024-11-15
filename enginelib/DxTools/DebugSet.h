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
	//	Note	:	동적으로 화면에 뿌림
	//	Usage	:	printf () 사용법과 같음		
	void ToView ( const char *szFormat, ... );
	
	//	Note	:	동적으로 뿌리되, 라인을 지정함
	//				라인은 0번라인부터 시작함
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

	//! 파일로 기록
    void ToLogFile(std::string& Message, BOOL bSaveCSV=TRUE);    

	// 튜토리얼 맵 관련 로그
	void ToTutorialMapFile( const char *szFormat, ... );
	// 인스턴트 던젼 관련 로그
	void ToInstantMapFile( const char *szFormat, ... );
	// 추적 로그를 남길때 사용함
	void ToTracingFile ( const char *szAccountName, const char *szFormat, ... );
	// 해킹이 의심될 경우 남는 로그
	void ToHackingFile( const char *szFormat, ... );
	// 10억이상 돈 업데이트시 로그를 남김
	void ToBillionUpdateFile( const char *szFormat, ... );
	// 클럽 공지 변경시 로그를 남김
	void ToClubNoticeFile( const char *szFormat, ... );

	// 블럭 로그
	void ToBlockDetectFile( const char *szFormat, ... );
	// 클럽 수익 로그
	void ToClubIncomeFile( const char *szFormat, ... );
	// CDM 로그
	void ToCDMLogFile( const char *szFormat, ... );
	// 프로파일링 로그
	void ToProFileLogFile( const char *szFormat, ... );
	// 프로파일링 메세지로그
	void ToProFileMsgFile( const char *szFormat, ... );
	// 확장 프로파일 로그용
	void ToProfileExLogFile( const char *szFormat, ... );

	//Game 클라이언트 상태 체크
	void ToStateCheckFile ( const char *szFormat, ... );

	void ToFile ( const char *szFileName, const char *szFormat, ... );
	void ToFileWithTime ( const char* szFileName, const char* szFormat, ... );
	void ClearFile ( const char *szFileName );
	
	const char* GetPath ();
	std::string GetCurTime ();

	//! 디버그정보를 파일로 출력할 때, 절대경로를 설정해야 한다.
	//! 화면으로 정보를 출력하기 위해, Font를 설정한다.
	HRESULT OneTimeSceneInit(const SUBPATH* pPath, bool bLogFileFinalOpen/*=false*/);
	HRESULT	InitDeviceObjects ( CD3DFontPar* pFont );
	HRESULT FinalCleanup();


    void CleanLogFile();
	void SetLogFileFinalOpen( bool bUse );
	void SetSubjectName_ERROR( BOOL bUse );

	void FrameMove ( float fTime, float fETime );
	void Render ();

	// 임시 테스트용
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
			rdtsc                      ;클럭카운트를 edx:eax레지스터에 저장
			mov     dwShi, edx       ;상위 dword 메모리에 백업
			mov     dwSlo, eax       ;하위 dword 메모리에 백업       
		}
	}

	inline void CLOCKCHK_ED ( DWORD &dwHigh, DWORD &dwLow )
	{
		__asm
		{
			rdtsc                      ; 클럭카운트를 edx:eax레지스터에 저장
			mov     dwElo, eax       ; 하위 dword 값 저장
			mov     dwEhi, edx       ; 상위 dword값 저장
		}

		dwHigh = dwEhi - dwShi;
		dwLow = dwElo - dwSlo;
	}
}

#endif	//	__C_DEBUGSET__