#ifndef FileNotifier_H
#define FileNotifier_H

#include "Param.h"

/**
	������ ���� �Ǿ������� üũ �Ѵ�.

 */
class CFileNotifier
{
public:
		CFileNotifier();

        enum
        {
            RETURNCODE_FAIL     = 0x00000001,	//@ ���� ����
            RETURNCODE_SUCCESS  = 0x00000002,	//@ ���� ����
            RETURNCODE_EXIT     = 0x00000004,	//@ ���� üũ ����
        };

		/**
			�ʱ�ȭ�� �����Ѵ�.
			üũ�� ���ϸ�� �ʱ� �ð��� ������.
		 */
virtual	bool			Init();

		/**
			������ ���ű� ������ üũ �Ѵ�.
		 */
virtual	bool			Classification(CParam& kParam);

		/**
			���� ������ üũ �Ѵ�.

			\return ���� ����
		 */
		int				Notify();

        /**
			���� �Ǿ��� ��� ó�� �Ѵ�.
			
			\return ���� ó�� �ڵ�
		 */
virtual int		        OnNotify() = 0;

        /**
			�Ķ����� ����ȯ�� ������
			�������� �����͸� ��� �� �� �ִ�.
		 */
		CParam&         Param() { return m_param; }

		/**
			������ ����Ǿ��� ��� ȣ�� �ȴ�.
		 */
virtual	void			OnChangeFile(CParam& kParam) {}

		/**
			���ϸ��� ������ �� �Ѵ�.
		 */
static	bool			IsSameFile(const TCHAR* pFile1, const TCHAR* pFile2);

protected:
	
		/**
			Ǯ ��� ���ϸ��� �Է� �Ѵ�.
		 */
		bool			SetFile(TSTRING& strFile);

		/**
			���� ���ϸ��� ��� �Ѵ�.
		 */
		const TCHAR*	GetFileName();

		/**
			������ ���� �������� üũ �Ѵ�.
			���� ���� ���� ������ �ȵǹǷ� �� �� üũ������ ��� �Ѵ�.
		 */
		bool			IsFileOpen();

        CParam	        m_param;    //@ variable values
		TSTRING			m_strFile;	//@ ���� ���
		time_t			m_time;		//@ ���� ���� �ð�        
};

#endif