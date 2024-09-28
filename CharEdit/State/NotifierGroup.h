#ifndef NotifierGroup_H
#define NotifierGroup_H

#include <list>
#include "Param.h"
#include <boost/smart_ptr/shared_ptr.hpp>

class CFileNotifier;

/**
	여러개의 갱신기를 관리 한다.

 */
class CNotifierGroup
{
public:
		CNotifierGroup();
		~CNotifierGroup();

		/**
			그룹의 타입을 설정 한다.

		 */
		void									Init(int nType);
		void									Clear();

		/**
			해당 파일명을 가지는 갱신기를 찾는다.

		 */
		CFileNotifier*							FindNotifier(CParam& kParam);

		/**
			갱신기를 삽입 삭제 한다.

		 */
		void 									InsertNotifier(CFileNotifier* pNotifier);
		void 									DeleteNotifier(CParam& kParam);

		/**
			갱신기들을 갱신 한다.

			\return 갱신 코드
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