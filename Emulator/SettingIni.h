#ifndef CSettingIni_H
#define CSettingIni_H

/**
	������ ini ���Ͽ� ����, �ε� Ŭ����
	
	������ ���ϰ� Ű�� �����ϰ�
    ���� �Ŀ��� ���ϰ� Ű�� ������Ű�� ī�װ��� �ε�, ���� �ϵ��� �ǵ�, ���� �ߴ�.
    ������ �ε��� �� ��� ������ ���� �ִ�.

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
			�ԷµǴ� �������� ��ġ, ũ�� ������ ����, �ε� �Ѵ�.
			���� ���Ǵ� ����̹Ƿ� ���� �Ѵ�.

		 */
		void				SavePosition(CWnd* pWnd);
		void				LoadPosition(CWnd* pWnd);

private:
		/**
			ini ������ ��θ� ��� �Ѵ�.
		   
			\param pPath ����� ��� �ּ�
		 */
		void				PathGetIni(TCHAR* pPath);

		// ini �� ������ Ű, �ݵ�� Save, Load ���� SetKey�� ���� �Ͽ��� �Ѵ�.
		TSTRING				m_strKey;

		// ini ���ϸ�
		TSTRING				m_strFile;
};

#endif