#ifndef CSettingIni_H
#define CSettingIni_H

/**
	설정을 ini 파일에 저장, 로드 클래스
	
	생성시 파일과 키를 설정하고
    생성 후에는 파일과 키는 고정시키고 카테고리로 로딩, 저장 하도록 의도, 제작 했다.
    하지만 부득이 한 경우 수정할 수도 있다.

 */
class CSettingIni
{
public:
		CSettingIni(const TCHAR* pFile, const TCHAR* pKey);
		~CSettingIni();

		void				SetFile(const TCHAR* pFile)							{ m_strFile = pFile; }
		void				SetKey(const TCHAR* pKey)							{ m_strKey = pKey; }

		bool				Save(const TCHAR* pCatagory, const TCHAR* pFormat, ...);
		int                 Load(const TCHAR* pCatagory, const TCHAR* pFormat, ...);

    	/**
			입력되는 윈도우의 위치, 크기 정보를 저장, 로딩 한다.
			자주 사용되는 기능이므로 내장 한다.

		 */
		void				SavePosition(CWnd* pWnd);
		void				LoadPosition(CWnd* pWnd);

private:
		/**
			ini 파일의 경로를 출력 한다.
		   
			\param pPath 출력할 경로 주소
		 */
		void				PathGetIni(TCHAR* pPath);

		// ini 에 접근할 키, 반드시 Save, Load 전에 SetKey로 설정 하여야 한다.
		TSTRING				m_strKey;

		// ini 파일명
		TSTRING				m_strFile;
};

#endif