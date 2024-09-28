
#pragma once

struct SBONESCALE
{
	D3DXVECTOR3 vScale;
	std::string strBoneName;

	SBONESCALE() 
		: vScale(1.0f,1.0f,1.0f)
		, strBoneName("")
	{
	}
};

typedef std::deque<SBONESCALE>  DEQ_BONESCALE;
typedef DEQ_BONESCALE::iterator DEQ_BONESCALE_ITER;