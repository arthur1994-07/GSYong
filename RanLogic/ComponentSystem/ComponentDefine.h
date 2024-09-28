#pragma once

namespace ComponentSystem
{
	// Component를 구별하기 위한 ID;
	// Component를 추가하기 위해서는 ID를 추가해야 한다;
	enum EMCOMP_ID
	{
		EMCOMP_NONE,				// 있을 수 없다 ( Assert );
		EMCOMP_MOB,
		EMCOMP_SUMMON,
		EMCOMP_AI,
		EMCOMP_SIZE
	};

	// 같이 들어갈 수 없는 Component를 구별하기 위한 Family ID;
	enum EMFAMILY_ID
	{
		EMFAMILY_NONE,				// 있을 수 없다 ( Assert );
		EMFAMILY_CROW,				// Crow와 Summon을 통합하려 할때 사용해야 한다;
		EMFAMILY_AI,
		EMFAMILY_SIZE
	};
}