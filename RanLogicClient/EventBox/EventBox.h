#pragma once

#include <queue>

namespace EventBox
{
	/* �̺�Ʈ �ڽ�;
	 * �̺�Ʈ �ڽ��� ��ӹ޴� Ŭ������ �ܺο�;
	 * �����ΰ� �߻���Ű�� �̺�Ʈ�� �˸� �� �ִ�; */
	class CEventBox
	{
	public:
		// ID�� �����ϴ� Event;
		void AddEvent( int addEvent )
		{
			m_queue.push( addEvent );
		}

		// Json�������� �� Data�� �Բ� �����ϴ� Event;
		void AddJsonEvent( const std::string& addEvent )
		{
			m_jsonQueue.push( addEvent );
		}

	public:
		int NextEvent()
		{
			if( m_queue.empty() )
				return 0;

			int curEvent = m_queue.front();
			m_queue.pop();
			return curEvent;
		}

		std::string NextJsonEvent()
		{
			if( m_jsonQueue.empty() )
				return std::string();

			std::string curEvent = m_jsonQueue.front();
			m_jsonQueue.pop();
			return curEvent;
		}
		
	private:
		std::queue<int> m_queue;
		std::queue<std::string> m_jsonQueue;
	};
}