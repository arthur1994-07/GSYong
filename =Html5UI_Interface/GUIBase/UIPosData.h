#pragma once

#include "UIDataType.h"

namespace GUIBase
{

/// 실제 관리되어지는 위치 정보 ( 컴포넌트 별로 관리 );
struct COMPONENT_POSITION
{
	UIRECT rcGlobalPos;				// Global 좌표;
	UIRECT rcLocalPos;				// Local 좌표;
};

}