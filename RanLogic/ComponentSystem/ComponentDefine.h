#pragma once

namespace ComponentSystem
{
	// Component�� �����ϱ� ���� ID;
	// Component�� �߰��ϱ� ���ؼ��� ID�� �߰��ؾ� �Ѵ�;
	enum EMCOMP_ID
	{
		EMCOMP_NONE,				// ���� �� ���� ( Assert );
		EMCOMP_MOB,
		EMCOMP_SUMMON,
		EMCOMP_AI,
		EMCOMP_SIZE
	};

	// ���� �� �� ���� Component�� �����ϱ� ���� Family ID;
	enum EMFAMILY_ID
	{
		EMFAMILY_NONE,				// ���� �� ���� ( Assert );
		EMFAMILY_CROW,				// Crow�� Summon�� �����Ϸ� �Ҷ� ����ؾ� �Ѵ�;
		EMFAMILY_AI,
		EMFAMILY_SIZE
	};
}