
#pragma once

#include "./ItemPeriodExtensionDefine.h"

struct SINVENITEM;

// ������ ���ʻ����� ���尡�� ī��Ʈ�� ����� ���� �����ϴ� ����� ������;
// ����� ���ϴ� �������� �Ǿ�����;
namespace ItemPeriodExtension
{
	// ���� �����ϰ� Ŭ��, ������ ��� �˻� �ӵ��� ���� ������ ���� �߰��۾��� ��;
	// �Ⱓ ���� �������� ��츦 �����ϰ�;
	// SITEM->sBasicOp.strItemPeriodExtension�� �Ⱓ ���� ����Ʈ�� ����صд�;
	// SITEM->sBasicOp.strItemPeriodExtension�� �Ⱓ ���� �����۸� ����Ѵ�;
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