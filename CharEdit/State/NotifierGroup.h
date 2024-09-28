#ifndef NotifierGroup_H
#define NotifierGroup_H

#include <list>
#include "Param.h"
#include <boost/smart_ptr/shared_ptr.hpp>

class CFileNotifier;

/**
	�������� ���ű⸦ ���� �Ѵ�.

 */
class CNotifierGroup
{
public:
		CNotifierGroup();
		~CNotifierGroup();

		/**
			�׷��� Ÿ���� ���� �Ѵ�.

		 */
		void									Init(int nType);
		void									Clear();

		/**
			�ش� ���ϸ��� ������ ���ű⸦ ã�´�.

		 */
		CFileNotifier*							FindNotifier(CParam& kParam);

		/**
			���ű⸦ ���� ���� �Ѵ�.

		 */
		void 									InsertNotifier(CFileNotifier* pNotifier);
		void 									DeleteNotifier(CParam& kParam);

		/**
			���ű���� ���� �Ѵ�.

			\return ���� �ڵ�
		 */	
		int										Notify();

protected:
private:
		int										m_nType;

		typedef std::tr1::shared_ptr<CFileNotifier>SpCNotifier;
		typedef std::list<SpCNotifier>			NotifierContainer;
		typedef NotifierContainer::iterator 	NotifierContainerIterator;
		NotifierContainer						m_kNotifierContainer;		
};

#endif