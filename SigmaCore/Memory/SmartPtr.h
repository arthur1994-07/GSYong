#ifndef _CTYPE_MEM_SMART_PTR_H_
#define _CTYPE_MEM_SMART_PTR_H_

#include <boost/tr1/memory.hpp>

/*  
	@class c언어 스타일의 포인터를 자동으로 관리하는 스마트 포인터 객체
    @brief  반드시 생성자나 대입연산자에 malloc으로 할당된 메모리를 넘겨야 한다 
    그리고 참조 카운팅을 유지 하려면 객체 자체를 복사해서 쓰면 된다 ( 복사 해도 spVoid 멤버 하나뿐이므로 가볍다)
	참고로 부스트 스마트 포인터는 참조카운팅을 atomic 연산을 함으로 쓰레드에 안전 

	shared_ptr에 대한 자세한 정보는 boost.org 의 문서 참조
*/

class CTypeSmartPtr
{
	typedef std::tr1::shared_ptr<void> spVoid;

public:
	CTypeSmartPtr(void* pMemory = NULL) { Init(pMemory); }
	BYTE* operator=(void* pMemory) { return (BYTE*)Init(pMemory); }
	
	// 포인터를 캐스팅할 경우 묵시적으로 잘못 변환될수 있다 막아두자
//	operator BYTE*() const { return (BYTE*)m_spMemory.get(); }
//	operator void*() const { return m_spMemory.get(); }
//	operator bool() const { return get() != NULL; }

	bool operator==(void* pMemory) const { return m_spMemory.get() == pMemory; }
	bool operator!=(void* pMemory) const { return m_spMemory.get() != pMemory; }

	void* get() const { return ToVoidPtr(); }
	void* ToVoidPtr() const {return m_spMemory.get();}
	BYTE* ToBytePtr() const {return (BYTE*)m_spMemory.get();}

	long use_count() { return m_spMemory.use_count(); }

	spVoid& GetSPMemory() { return m_spMemory; }
private:
	void* Init(void* pMemory) { m_spMemory = spVoid(pMemory,free); return m_spMemory.get(); }

private:
	spVoid m_spMemory;

};

inline bool operator==(void* pMemory,CTypeSmartPtr& CTypeMemory) { return pMemory == CTypeMemory.get(); }
inline bool operator!=(void* pMemory,CTypeSmartPtr& CTypeMemory) { return pMemory != (void*)CTypeMemory.get(); }

#endif //#ifndef _CTYPE_MEM_SMART_PTR_H_
