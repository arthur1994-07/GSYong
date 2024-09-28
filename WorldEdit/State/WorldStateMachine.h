#ifndef WorldStateMachine_H
#define WorldStateMachine_H

/**
	월드 에디트 툴의 같이 동작되는 탭 페이지들을 묶어 상태로 정의 한다.
    상태는 GetActiveGroup() 함수 구현부에 정의되어 있다.

    아래는 현재 상태 머신을 통해 구현된 기능을 정의 한다.

    1. 외부 파일 변경에 따른 동기화 처리
     - 현재 프레임 x 파일, 텍스처, pie 파일에 대해 외부에서 파일이 변경되면
       변경을 감지해서 실시간으로 변경을 적용해 준다.
       psf 뿐 아니라 wld 도 적용 됨에 주의 한다. 단 wld 는 프레임 x 파일이 없으므로 이것은 대상이 아니다.

	2. 월드 에디트의 메인 프레임(psf or wld) 리로딩 기능
	 - 현재 어떠한 탭 페이지든 간에 리로딩은 메인 프레임을 재로딩 하게 된다.

	3. 조각 파일 편집 후 메인 프레임 적용 처리
	 - 지형 조각 파일(pie)을 편집 후 저장했을 때 동일한 파일이 메인 프레임에 사용될 경우 실시간 적용

    4. 아카이브 기능
     - 아카이브는 3ds max 의 아카이브 처럼 현재 신의 요소들을 추출하는 기능이다.

    5. 어셋 뷰 처리
     - 현재 신(Scene)에서 사용되는 어셋(자산)에 대한 처리를 수행 한다.
 */
namespace WorldStateMachine
{
	/**
		초기화 및 파괴를 수행 한다.
		프로그램 실행 중 단 1번만 호출 되어야 한다.
		
	 */
	void	InitWorldStateMachine();
	void	DestroyWorldStateMachine();

 	/**
 		현재 포커스를 설정 한다.
 		포커스란 현재 활성화된 텝 페이지을 의미 한다.
 
 		\param nOld 이전 활성화 탭 페이지
		\param nNew 현재 활성화 탭 페이지
 	 */
	void	SetFocus(int nOld, int nNew);

	/**
		현 상태에서 모든 데이터들을 재로딩 한다.		

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

		\param nType 현재 활성화 그룹 타입
                     -1 일 경우 이전에 저장된 타입을 사용
	 */
	void	InsertFileNotifier(int nType = -1);
	void	ClearFileNotifier(int nType = -1);

	/**
		에셋 처리를 수행 한다.
        현재는 메인 파일(wld, psf) 파일 로딩, 재로딩 시에 Asset()을 호출하고
        EditAsset()는 에셋이 삭제, 추가, 선택 될 때 호출 된다.

		\param nType 현재 활성화 그룹 타입
                     -1 일 경우 이전에 저장된 타입을 사용
	 */
	void	Asset(int nType = -1);
    void    EditAsset(const TCHAR* pItem);
    void    SelectAsset(DWORD dwData);

	/**
		파일 변경을 체크 한다.
		파일에 수정 시간을 저장해 놓고
		변경 되었는지 체크 하여 변경 되었을 경우 갱신 한다.
			
	 */
	void	Update();

	/** <내부 사용 함수>
		액티브 탭 테이지의 그룹을 지정 한다.

		그룹을 지어 주는 이유는 현재 뷰 페이지에 따라 여러개의 텝 페이지를 가지므로
		우리는 뷰에 따라 하나의 활성화 텝 넘버를 얻기 위함 이다.

		\param nAtvIndex 현재 활성화된 탭 넘버
                         -1 일 경우 이전에 저장된 타입을 사용
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

    /** <유틸리티 함수>
        현재 윈도우 영역에 마우스가 있는지 살핀다.
        전역적으로 참조할 만한 마땅한데가 없어(아직 코드가 덜 파악될을수도...)
        일딴 이곳에 추가한다;;

     */
    bool    IsInMouse(CWnd* pWnd);

}; // end namespace

#endif