#pragma once

enum SIMPLE_COLLISION_RESULT
{
	COLL_FALSE,			// 충돌되지 않음.
	COLL_TRUE,			// 이동라인은 충돌되지만 시작점은 충돌되지 않음.
	COLL_TRUE_START_IN,	// 시작점이 충돌되고 있음.
};
