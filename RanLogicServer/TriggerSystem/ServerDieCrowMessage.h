#ifndef SServerDieCrowMessage_H
#define SServerDieCrowMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../enginelib/G-Logic/GLDefine.h"

namespace TriggerSystem
{

/**	
	�� ũ�� ��� �޽���
	ũ�簡 �ʿ��� ������� �� ȣ�� �ȴ�.    
	
 */
struct SServerDieCrowMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD   dwCrowGaeaID;   //@ ũ�� �ʵ弭�� ���̾� ID, ũ��� DB ID �� ����.
    EMCROW  eCromType;      //@ ũ�� Ÿ��
    DWORD   dwKillerDbBum;  //@ �׾��� �� �������� DB ID
};

} // end namespace

#endif