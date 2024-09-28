#pragma once

namespace SKILL
{
	//	���� �� ���� ����.
	struct SLEARN_LVL_101	//	������ ���� ���� ����
	{
		DWORD				dwSKP;					//	�䱸 ��ų ����Ʈ.
		DWORD				dwLEVEL;				//	�䱸 Level.
		SCHARSTATS			sSTATS;					//	�䱸 Stats.

		DWORD				dwSKILL_LVL;			//	�䱸 ������ų ����.

		SLEARN_LVL_101 (void);
	};

	struct SLEARN_LVL_102	//	������ ���� ���� ����
	{
		DWORD				dwLEVEL;				//	�䱸 Level.
		SCHARSTATS			sSTATS;					//	�䱸 Stats.		
		DWORD				dwSKP;					//	�䱸 ��ų ����Ʈ.
		DWORD				dwSKILL_LVL;			//	�䱸 ������ų ����.

		SLEARN_LVL_102 (void);
	};

	struct SLEARN_LVL_103	//	��ų���� ���� �߰�( �Ѱ��� ���� �� ���� ���� )
	{
		DWORD				dwLEVEL;				//	�䱸 Level.
		SCHARSTATS			sSTATS;					//	�䱸 Stats.		
		DWORD				dwSKP;					//	�䱸 ��ų ����Ʈ.
		DWORD				dwSKILL_LVL;			//	�䱸 ������ų ����.

		SLEARN_LVL_103 (void);
	};

	struct SLEARN_LVL_104	//	��ų���� ���� �߰�( �Ѱ��� ���� �� ���� ���� )
	{
		DWORD				dwLEVEL;				//	�䱸 Level.
		SCHARSTATS			sSTATS;					//	�䱸 Stats.		
		DWORD				dwSKP;					//	�䱸 ��ų ����Ʈ.
		DWORD				dwSKILL_LVL;			//	�䱸 ������ų ����.
		DWORD				dwReqMoney;				//  �䱸 ��;
		SLEARN_LVL_104 (void);
	};

	struct SLEARN_LVL : public SLEARN_LVL_104
	{
		SLEARN_LVL (void);
		void Assign ( SLEARN_LVL_101& sOldData );
		void Assign ( SLEARN_LVL_102& sOldData );
		void Assign ( SLEARN_LVL_103& sOldData );
	};
};