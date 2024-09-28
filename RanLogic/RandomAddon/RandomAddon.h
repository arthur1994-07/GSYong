
#pragma once

// 'RandomOption'�� ����ϴ�;
// * different: Fileformat Lua, Save logic not exist;
#include "../Item/GLItemDef.h"
#include "RandomAddonDef.h"

class GLogicFile;

namespace RandomAddon
{
	extern BOOL Lua_LoadDoBuffer(std::string& strPath, std::string& strFile);

	class Value
	{
	public:
		float fRateChoose;	// ���õ� Ȯ��;
		union
		{
			struct { INT	nValueHigh, nValueLow; };
			struct { float	fValueHigh, fValueLow; };
		};

	public:
		Value(void);
		Value(float rateChoose, INT	valueHigh, INT valueLow);
		Value(float rateChoose, float valueHigh, float valueLow);

	public:
		// �� union�� �ִ� low, high ������ ���� ��ȯ��;
		const float GetValue(const bool bInteger) const;
	};
	typedef std::vector<Value>		VEC_VALUE;
	typedef VEC_VALUE::iterator		VEC_VALUE_ITR;

	class Bunch
	{
	public:
		enum EMBUNCHTYPE{ BUNCH_TYPE_DROP = 0, BUNCH_TYPE_GENERATE };
		float fRateChoose;
		EMITEM_ADDON emAddon;
		union
		{
			struct { INT	nValueHigh, nValueLow; };
			struct { float	fValueHigh, fValueLow; };
		};
		VEC_VALUE vecValue;

	public:
		Bunch(void);
		Bunch(float rateChoose, EMITEM_ADDON addon);

	public:
		// vecValue���� Ȯ�� üũ�� �ش��ϴ� RandomAddon::value ��ȯ��;
		const Value& GetValue(void) const;
	};

	typedef std::vector<Bunch>		VEC_BUNCH;
	typedef VEC_BUNCH::iterator		VEC_BUNCH_ITR;

	class Entity
	{
	public:
		std::string	m_strName;
		float		fRateGenerate;	// �߻� Ȯ��;
		VEC_BUNCH	vecBunchDrop;	// ����� ���� ���� ���̺�;
		VEC_BUNCH	vecBunchRemodel;// ������ ���� ���� ���̺�;

	public:
		Entity(void);

	public:
		BOOL LoadFile(std::string& strName);
		const float GetGenerateRate(void) const { return fRateGenerate; }

	private:
		BOOL Load(Bunch::EMBUNCHTYPE emType, VEC_BUNCH& vecStorage);
	};

	//HASH::generateHashKey
	typedef std::map<unsigned int, Entity>	MAP_ENTITY;
	typedef MAP_ENTITY::iterator	MAP_ENTITY_ITR;
	typedef MAP_ENTITY::const_iterator	MAP_ENTITY_CITR;

	class Manager
	{
	private:
		MAP_ENTITY	m_mapEntity;

	private:
		Manager(void);
		virtual ~Manager(void);
		
	public:
		static Manager& GetInstance(void);
		BOOL LoadFile(GLogicFile* const pFileLogic);
		BOOL Find(IN const std::string& strName, IN OUT Entity& sEntity);
	};
};