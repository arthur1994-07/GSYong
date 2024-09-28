
#pragma once

#include "./ItemPeriodExtensionDefine.h"

struct SINVENITEM;

// 아이템 최초생성시 연장가능 카운트를 기록후 사용시 차감하는 방식이 좋으나;
// 현재는 더하는 형식으로 되어있음;
namespace ItemPeriodExtension
{
	// 툴을 제외하고 클라, 서버일 경우 검색 속도를 위해 다음과 같은 추가작업을 함;
	// 기간 연장 아이템인 경우를 제외하고;
	// SITEM->sBasicOp.strItemPeriodExtension에 기간 연장 리스트를 등록해둔다;
	// SITEM->sBasicOp.strItemPeriodExtension는 기간 연장 아이템만 사용한다;
	extern HRESULT LoadFile(GLogicFile* const pFileLogic, bool bTool = false );

	typedef std::vector<SNATIVEID> VEC_ITEMLIST;
	typedef VEC_ITEMLIST::iterator VEC_ITEMLIST_ITR;
	class Entity
	{
	public:
		char			m_szName[FILENAME_LENGTH];
		INT				m_nPeriodExtensionCount;
		VEC_ITEMLIST	m_vecItemList;
		
	public:
		Entity(void);
		virtual ~Entity(void);

	public:
		HRESULT LoadFile(std::string& strFileName, bool bTool);

	public:
		bool IsRegItem(const SNATIVEID& nidCheck) const;
	};

	typedef std::map<std::string, Entity>	MAP_ENTITYLIST;
	typedef MAP_ENTITYLIST::iterator		MAP_ENTITYLIST_ITR;
	class Manager
	{
	private:
		MAP_ENTITYLIST	m_mapEntityList;

	private:
		Manager(void);

	public:
		virtual ~Manager(void);
		static Manager& GetInstance(void);

		static const char* _FileName;
	public:
		HRESULT Insert(std::string strFileName, bool bTool);

	public:
		//HRESULT IsExtendableItem(const SINVENITEM* pExtensionInvenItem, const SINVENITEM* pCheckInvenItem);
		HRESULT IsExtendableItem(const SNATIVEID& nidExtension, const SNATIVEID& nidCheckItem, OUT INT& nMaxCount);

		bool IsRegItem(const SNATIVEID& nidCheck, OUT INT& nCount);
		bool IsRegItem(const SNATIVEID& nidExtension, const SNATIVEID& nidCheck);
		bool IsRegItem(const SNATIVEID& nidExtension, const SNATIVEID& nidCheck, OUT INT& nCount);

		DWORD DoTest(const SITEMCUSTOM& sExtensionItem, const SITEMCUSTOM& sCheckItem, __time64_t& tPrevTime, __time64_t& tNextTime);
	};
}