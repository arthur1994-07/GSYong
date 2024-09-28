#include <stdarg.h>

/**
	vc++ ������ ���� ���� ����Ʈ Scanf �Է� �Լ��� �������� �ʴ´�.
    �� �Լ��� �̸� ������ �Ѵ�.

    ������ ���� ���������� _stscanf_s �� ����ϰ� �����Ƿ�
    ��Ʈ���� ���� ���� ��Ʈ���� ����� �ݵ�� �˷��־�� �Ѵ�.

    ������� (%s %d ...) �������� �о� ���δٸ� 
    strBuff, strBuffSize, int... �������� ��Ʈ�� ������ ũ��(����)�� �Է��� �־�� �Ѵ�.

    ���� ����Ʈ
    http://www.gamedev.net/topic/310888-no-vfscanf-in-visual-studio/

	\param buf ��ĵ�� ���ڿ� ����
	\param format ��ĵ�� ����
	\param argPtr ���� ���� ����Ʈ
	\return ��ĵ�� ���� �� ��
 */
int _vstscanf_s(const TCHAR* buf, const TCHAR* format, va_list argPtr);