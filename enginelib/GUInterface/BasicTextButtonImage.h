//	기본 정보 표시
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.11.27]
//			@ 작성
//

#pragma	once

#include "../EngineLib/GUInterface/UIGroup.h"

class	CBasicTextButtonImage : public CUIGroup
{
public:
	CBasicTextButtonImage (EngineDeviceMan* pEngineDevice);
	virtual	~CBasicTextButtonImage ();

public:
	void CreateButtonImage(
        const char* szLeft,
        const char* szCenter,
        const char* szRight);
};