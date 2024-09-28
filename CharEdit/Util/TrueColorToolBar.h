#ifndef CTrueColorToolBar_H
#define CTrueColorToolBar_H

/**
	트루컬러 툴바를 지원 한다.
    기본적으로 16비트 툴바 밖에 지원하지 않으므로
    256 이상 툴바를 사용하려면 따로 만들어 주어야 한다.
	
 */
class CTrueColorToolBar
	: public CToolBar
{
public:

		/**
			툴바를 로드 한다.
			반드시 툴바를 생성(Create 함수 또는 CreateEx함수) 한 후 호출 하여야 한다.

			내부적으로 MFC 의 툴바 로드를 따르되 비트맵 로딩 부분을 LoadBitmap() 함수를
			호출 하도록 한다.

			\param unToolBarID 툴바 리소스 ID
			\param unBitmapID 사용 비트맵 리소스 ID
			\return 로드 성공 유무
		 */
		BOOL LoadToolBar(UINT unToolBarID, UINT unBitmapID);

protected:
private:
		/**
			툴바에서 사용할 트루컬러 비트맵을 로딩 한다.

			\param lpszResourceName 리소스명
			\return 로드 성공 유무
		 */
		BOOL LoadBitmap(LPCTSTR lpszResourceName);
};

#endif