#ifndef CharStateMachine_H
#define CharStateMachine_H

class CAssetDlg;

/**
	캐릭터 툴에는 몇가지의 큰 탭(탭페이지)이 있다.
	예를들면 캐릭터, 조각, 어태치본, 어태치본링크 등이다.

    아래는 현재 상태 머신을 통해 구현된 기능을 정의 한다.

    1. 파일 변경 감시
     - 현재 뷰 되는 모델의 텍스처, 애니, 메쉬, 본 파일의 변경을 감지 변경시 적용한다.

    2. 탭 동기화
     - 캐릭터 탭과 조각 탭의 동기화

    3. 아카이브 기능
    - 아카이브는 3ds max 의 아카이브 처럼 현재 신의 요소들을 추출하는 기능이다.

    4. 어셋 뷰 처리
    - 어셋 처리를 외부에서 수행하려면 각 페이지마다 처리를 해야 하므로 복잡하다.
      리로딩 기능과 연계하면 쉽게 처리할 수 있다.
    
 */
namespace CharStateMachine
{
	/**
		초기화 및 파괴를 수행 한다.
		프로그램 실행 중 단 1번만 호출 되어야 한다.
		
	 */
	void	InitRefresher();
	void	DestroyRefresher();

    /**
 		현재 포커스를 설정 한다.
 		포커스란 현재 활성화된 텝 페이지을 의미 한다.
 
 		\param nOld 이전 활성화 탭 페이지
		\param nNew 현재 활성화 탭 페이지
 	 */
	void	SetFocus(int nOld, int nNew);

	/**
		현 상태에서 모든 데이터들을 재로딩 한다.
		예를들어 캐릭터 상태라면 현재 캐릭터를 완전 삭제 후 재로딩 한다.

		\param nAtvIndex 현재 활성화된 상태 인덱스
	 */
	void	Reload(int nAtvIndex);

	/**
		아카이브를 처리 한다.

		\param pRootFolder 아카이브 할 폴더
	 */
	void	Archive(const TCHAR* pRootFolder);

	/**
		파일 갱신기를 삽입 한다.

		\param nType 파일 갱신기 그룹 타입
					 -1 일 경우 이전에 저장된 타입을 사용
	 */
	void	InsertFileNotifier(int nType = -1);

	/**
		파일 갱신기를 제거 한다.

		\param nType 상태넘버
					 -1 일 경우 이전에 저장된 타입을 사용
	 */
	void	ClearFileNotifier(int nType = -1);

	/**
		파일이 강제로 변경 되는 경우의 처리
		예를들어 현재 조각텝 테이지에서 텍스처를 변경할 수 있다.
		이럴경우 사용 된다.

		\param pFile 경로 제외 순수 변경전 파일명(ex).abc.x)
		\param pChangeFile 경로 제외 순수 변경된 파일명(ex). abc.x)
	 */
	void	ChangeFIle(const TCHAR* pFile, const TCHAR* pChangeFile);

    /**
        현재 활성화 뷰의 어셋을 어셋 다이얼로그에 추가 한다.
                
     */
    void    Asset();

	/**
		파일 변경을 체크 한다.
		파일에 수정 시간을 저장해 놓고
		변경 되었는지 체크 하여 변경 되었을 경우 갱신 한다.
			
	 */
	void	Update();

	/** <내부 사용 함수>
		액티브 탭 테이지의 그룹을 지정 한다.

		예를들어 조각 파일 세팅 안에 이펙트와 재질 설정 탭이 포함 된다.

		\param nAtvIndex 현재 활성화된 탭 넘버(enum CHAR_TAB_PAGE)
                         -1 일 경우 이전에 저장된 활성화 탭 넘버를 출력
	 */
	int		GetActiveGroup(int nAtvIndex = -1);

	/** <내부 사용 함수>
		업데이트 갱신 주기를 출력 한다.
		1000분에 1초 단위

	 */
	DWORD	GetUpdateTime();

	/** <내부 사용 함수>
		강제로 화면을 렌더링 한다.
		만약 x파일이나 텍스처를 수정했을 경우 외부에서 수정했기 때문에
		현재 툴 화면에는 갱신이 되지 않는다.(갱신하려면 Active시켜야함) 이를
		타개 하기 위해 변경 되었을 경우 강제로 렌더링을 걸어 준다.

	 */
	void	ForceRender();

}; // end namespace

#endif