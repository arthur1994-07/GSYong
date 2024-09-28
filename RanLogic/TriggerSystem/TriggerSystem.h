#ifndef TriggerSystem_H
#define TriggerSystem_H

#include "TriggerSystemType.h"

/**
	Ʈ���� �ý���
	�̺�Ʈ�� ������� ���� �Ǿ� ���� �ý���
	���� ��� �������� ���� �ϱ� ���� ���� �ý���
	���� �پ��� �������� ��� ����

 */
namespace TriggerSystem
{
	/**
		Ʈ���� �ý����� �ʱ�ȭ �Ѵ�.
		�ܺ� ���� ������ �ε� �Ͽ� ���ӿ��� ����� �ڷ� ������ �����.

		\return �ʱ�ȭ ���� ����
	 */
	bool			InitTriggerSystem();

	/**
		Ʈ���� �ý����� �ı� �Ѵ�.
		������ �ڷᱸ���� �ı� �Ѵ�.

	 */
	void			DestroyTriggerSystem();

	/**
		Ʈ���� �ý��ۿ� �޽����� ���� �Ѵ�.

	 */
	void			SendMessage(struct SSystemMessage* msg);

	/**
		��ũ��Ʈ ���Ϸ� ���� Ʈ���Ÿ� �����Ѵ�.
		�ѹ� �ε��� ��ũ��Ʈ�� ��Ȱ�� �ȴ�.

		\param pName Ʈ���� ���ϸ�
	 */
	template<class T>
	void			CreateScript(const TCHAR* pFile);

	/** <�ý��� ���� ��� �Լ�>
		Ʈ���� ��ũ��Ʈ�� ���� �Ѵ�.
        ���Ե� ��ũ��Ʈ�� ���� �ð����� ��� ��Ȱ�� �ȴ�.
        ���� ��ũ��Ʈ�� ���� ������ Ʈ���Ÿ� ������ ���� �� �� �ִ�.

		\param pFile Ʈ���� ���ϸ�, �����ڷ� ��� �ϹǷ� ��ø���� �ʰ� �ؾ� �Ѵ�.
		\param trigger ������ Ʈ���� ��ũ��Ʈ
	 */
	void			InsertTrigger(const TCHAR* pFile, SpTriggerScript script);

	/** <�ý��� ���� ��� �Լ�>
		�ý��ۿ� ���Ե� ��ũ��Ʈ�� ����
        Ʈ���Ÿ� ã�´�. �̷��� ã�� ��ũ��Ʈ�� FInger �� ����
        Ʈ���Ű� ������ �� ��� �ȴ�.

		\param pFile ��ũ��Ʈ ���ϸ�
        \param pRoot ã�� Ʈ���Ÿ�
		\return ã�� Ʈ����, ������ NULL
	 */
	CTrigger*		FindTrigger(const TCHAR* pFile, const TCHAR* pRoot);

    /** <�ý��� ���� ��� �Լ�>
		�ý��ۿ��� �ԷµǴ� ���ϸ���
        ������ ��ũ��Ʈ�� �ִ��� üũ �Ѵ�.

		\param pFile ��ũ��Ʈ ���ϸ�
		\return ��ũ���� ã�Ҵٸ� true, �ƴ϶�� false
	 */
    bool            FindScript(const TCHAR* pFile);

} // end namespace

#include "TriggerSystem.inl"

#endif