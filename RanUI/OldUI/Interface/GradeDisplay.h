//	'ESC' 키 눌렀을때 나오는 메뉴
//
//	최초 작성자 : 성기엽
//	이후 수정자 :
//	로그
//		[2003.12.4]
//			@ 최초 작성

#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CGradeDisplay : public CUIGroup
{
public:
    CGradeDisplay(EngineDeviceMan* pEngineDevice);
	virtual	~CGradeDisplay ();

public:
	void	CreateSubControl ();	
};