#ifndef CClientMapInfoTrigger_H
#define CClientMapInfoTrigger_H

#include "../../RanLogic/TriggerSystem/MapInfoTrigger.h"
#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
	�� ������ ������.
	�̸� ���� �ܺο��� ���� ���� �Ѵ�.
	
 */
class CClientMapInfoTrigger
	: public CClientTrigger<CMapInfoTrigger>
{
public:
		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType		GetType() const { return TRIGGER_TYPE_MAPINFO; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void			Bind(CTriggerScript* pScript);

		/**
			���ε� �������̽� �Լ���

		 */
		void			SetMapName(TSTRING name) { m_mapname = name; }
		const TSTRING	GetMapName() const { return m_mapname; }

		void			SetObject(TSTRING Object) { m_mapObject = Object; }//�������� UI��
		const TSTRING	GetObject() const { return m_mapObject; }

		void			SetMapInfo(TSTRING info) { m_mapinfo = info; }
		const TSTRING	GetMapInfo() const { return m_mapinfo; }

		void			SetMapInfoRn(TSTRING infoRn) { m_mapinfoRn = infoRn; } // �������� UI��
		const TSTRING	GetMapInfoRn() const { return m_mapinfoRn; }


		void			SetMapReward(TSTRING reward) { m_mapreward = reward; }
		const TSTRING	GetMapReward() const { return m_mapreward; }

protected:
		TSTRING			m_mapname;		//@ �δ���(XML)
		TSTRING			m_mapObject;	//@ �ʸ�ǥ(XML) �������� UI��
		TSTRING			m_mapinfo;		//@ ������(XML)
		TSTRING			m_mapinfoRn;	//@ ������(XML) �������� UI��
		TSTRING			m_mapreward;	//@ �ʺ���(XML)
};

} // end namespace

#endif