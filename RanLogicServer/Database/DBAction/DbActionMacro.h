
#pragma once

#include "DbAction.h"

namespace db
{
	class MacroLogInsert : public db::DbAction
	{
	private:
		DWORD m_dwChaDBNum;
		WORD m_wState;

	public:
		MacroLogInsert(DWORD dwChaDBNum, WORD wState)
			: m_dwChaDBNum(dwChaDBNum), m_wState(wState) {}
		virtual ~MacroLogInsert(void) {}
		virtual int Execute(NetServer* pServer) override;
	};

}