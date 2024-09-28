#ifndef CTrueColorToolBar_H
#define CTrueColorToolBar_H

/**
	Ʈ���÷� ���ٸ� ���� �Ѵ�.
    �⺻������ 16��Ʈ ���� �ۿ� �������� �����Ƿ�
    256 �̻� ���ٸ� ����Ϸ��� ���� ����� �־�� �Ѵ�.
	
 */
class CTrueColorToolBar
	: public CToolBar
{
public:

		/**
			���ٸ� �ε� �Ѵ�.
			�ݵ�� ���ٸ� ����(Create �Լ� �Ǵ� CreateEx�Լ�) �� �� ȣ�� �Ͽ��� �Ѵ�.

			���������� MFC �� ���� �ε带 ������ ��Ʈ�� �ε� �κ��� LoadBitmap() �Լ���
			ȣ�� �ϵ��� �Ѵ�.

			\param unToolBarID ���� ���ҽ� ID
			\param unBitmapID ��� ��Ʈ�� ���ҽ� ID
			\return �ε� ���� ����
		 */
		BOOL LoadToolBar(UINT unToolBarID, UINT unBitmapID);

protected:
private:
		/**
			���ٿ��� ����� Ʈ���÷� ��Ʈ���� �ε� �Ѵ�.

			\param lpszResourceName ���ҽ���
			\return �ε� ���� ����
		 */
		BOOL LoadBitmap(LPCTSTR lpszResourceName);
};

#endif