#pragma once

#include "../Util/UIPage.h"

class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;

/**
	전장 UI 의 공통 텝 페이지 처리를 수행 한다.

	전장 UI 는 현재 점령전과 PVE 로 이뤄져 있지만 향후 여러개가 추가될 수 있도록
	구조적으로 CCompetitionPage 를 상속 받아 탭 페이지 처리를 구현 한다.

	그러므로 모든 탭 페이지에서 사용되는 공통 인터페이스가 이곳에 구현되어야 한다.

 */
class CCompetitionPage
	: public CUIPage
{
public:
		CCompetitionPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual ~CCompetitionPage() = 0 {}

		/**
			외부에서 전장 관련 상태가 변경 되면 호출되어
			UI 의 내용을 갱신 해 주는 역할을 수행 한다.

		 */
virtual void Update() = 0;
};
