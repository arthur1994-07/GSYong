

#ifndef _mrcRegisterScriptObject_h_
#define _mrcRegisterScriptObject_h_

#include "mrcConfig.h"

namespace mrc
{

	// �ܺο��� ��ũ��Ʈ ��ü�� ����ϴ� �Լ� ���
	void registerScriptObject( funcRegisterScriptObject func );

	// �ܺο��� �����  ��ũ��Ʈ ��ü�� ����ϴ� �Լ���  ����
	void runRegisteredScriptObject();





}


#endif