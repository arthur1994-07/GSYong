
#pragma once

// 'RandomOption'과 흡사하다;
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
		float fRateChoose;	// 선택될 확률;
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
		// 위 union에 있는 low, high 사이의 값을 반환함;
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
		// vecValue내에 확률 체크후 해당하는 RandomAddon::value 반환함;
		const Value& GetValue(void) const;
	};

	typedef std::vector<Bunch>		VEC_BUNCH;
	typedef VEC_BUNCH::iterator		VEC_BUNCH_ITR;

	class Entity
	{
	public:
		std::string	m_strName;
		float		fRateGenerate;	// 발생 확률;
		VEC_BUNCH	vecBunchDrop;	// 드랍에 의한 랜덤 테이블;
		VEC_BUNCH	vecBunchRemodel;// 개조에 의한 랜덤 테이블;

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