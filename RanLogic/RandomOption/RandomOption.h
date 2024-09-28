#pragma once

#include "./RandomOptionDefine.h"

namespace RandomOption
{
	extern HRESULT LoadFile(const RandomOption::LOAD_TYPE _typeLoad, GLogicFile* const _pFileLogic = 0, const bool _bTool = true);
	extern HRESULT LoadFileOld(const bool _bConvert);



	class Value
	{
	public:
		const bool saveFile(sc::SerialFile& _fileStreamServer) const;
		const bool saveCsv(std::fstream& SFile, bool bInteger) const;
		const bool loadCsv(CStringArray& StrArray, bool bInteger);
		const bool loadFile(sc::BaseStream& _fileStream);
		const bool loadFile_old(gltexfile& _fileText, const TCHAR* const _cOptSEFull, RandomOption::ValueTemporary& _valueTemporary, const bool _bValueInteger);

		__forceinline const float getRateChoose(void) const;
		__forceinline const float getValue(const bool _bValueInteger) const;
		__forceinline const float getValueFloatHigh(void) const;
		__forceinline const float getValueFloatLow(void) const;
		__forceinline const int getValueIntegerHigh(void) const;
		__forceinline const int getValueIntegerLow(void) const;

		__forceinline void setRateChoose(const float _fRateChoose);
		__forceinline void setValueHigh(const int _valueIntegerHigh);
		__forceinline void setValueHigh(const float _valueFloatHigh);
		__forceinline void setValueLow(const int _valueIntegerLow);
		__forceinline void setValueLow(const float _valueFloatLow);

	public:
		__forceinline const RandomOption::Value& operator =(const RandomOption::Value& _rhs);
		Value(const RandomOption::Value& _rhs);
		Value(const float _fRateChoose, const int _valueIntegerLow, const int _valueIntegerHigh);
		Value(const float _fRateChoose, const float _valueFloatLow, const float _valueFloatHigh);
		Value(void);

	private:
		float fRateChoose; // ���õ� Ȯ��;
		union
		{
			struct
			{
				int valueIntegerHigh;
				int valueIntegerLow;
			};
			struct
			{
				float valueFloatHigh;
				float valueFloatLow;
			};
		};
	};
	typedef std::vector<RandomOption::Value> ValueVector;
	typedef ValueVector::const_iterator ValueVectorConstIterator;
	typedef ValueVector::iterator ValueVectorIterator;



	class ValueBunch
	{
	public:
		const bool saveFileServer(sc::SerialFile& _fileStreamServer) const;
		const bool saveCsv(std::fstream& SFile) const;
		const bool loadCsv(CStringArray& StrArray);
		const bool saveFileClient(sc::SerialFile& _fileStreamClient) const;
		const bool loadFileClient(sc::BaseStream& _fileStream);
		const bool loadFileServer(sc::BaseStream& _fileStream);
		const bool loadFile_old(gltexfile& _fileText, const std::string& _stringNameFile, const TCHAR* const _cOptionFull, const TCHAR* const _cOptSE, const RandomOption::GENERATE _typeGenerate, const bool _bConvert);
		
		void addValue(const unsigned int _indexInsert, const RandomOption::Value& _value = RandomOption::Value());
		void removeValue(const unsigned int _indexValue);
		void refreshValue(void);

		__forceinline const float getRateChoose(void) const;
		__forceinline const int getValueIntegerHigh(void) const;
		__forceinline const int getValueIntegerLow(void) const;
		__forceinline const float getValueFloatHigh(void) const;
		__forceinline const float getValueFloatLow(void) const;
		__forceinline const RandomOption::RANDOM_OPTION getTypeRandomOption(void) const;		
		__forceinline const RandomOption::ValueVector& getValueVector(void) const;
		__forceinline RandomOption::ValueVector& getValueVector(void);
		__forceinline const RandomOption::Value& getValue(void) const;		

	private:
		__forceinline RandomOption::Value* const _createValue(void);
		void _rebalance(void);

	public:
		__forceinline const bool operator ==(const RandomOption::RANDOM_OPTION _typeRandomOption) const;
		__forceinline const RandomOption::ValueBunch& operator =(const RandomOption::ValueBunch& _rhs);
		ValueBunch(const RandomOption::ValueBunch& _rhs);
		ValueBunch(const float _fRateChoose, const RandomOption::RANDOM_OPTION _typeRandomOption);
		ValueBunch(void);

	private:
		float fRateChoose;
		RandomOption::RANDOM_OPTION typeRandomOption;
		union
		{
			struct
			{
				int valueIntegerHigh;
				int valueIntegerLow;
			};
			struct
			{
				float valueFloatHigh;
				float valueFloatLow;
			};
		};		
		RandomOption::ValueVector vectorValue;
	};
	typedef std::vector<RandomOption::ValueBunch> ValueBunchVector;
	typedef ValueBunchVector::const_iterator ValueBunchVectorConstIterator;
	typedef ValueBunchVector::iterator ValueBunchVectorIterator;



	// ���� �ɼ��� �߻� �� �� �ִ� �ִ� ������ ���� 4�̴�;
	// �� ��ġ�� �ø��� ���ؼ� SCUSTOMITEM�� ������ ���� �� �ʿ䰡 �ִ�;
	class Entity
	{
	public:		
		const bool saveFileServer(sc::SerialFile& _fileStreamServer) const;
		const bool saveCsv(std::fstream& SFile) const;
		const bool loadCsv(CStringArray& StrArray);
		const bool loadFileServer(sc::BaseStream& _fileStream);
		const bool loadFile_old(gltexfile& _fileText, const std::string& _stringNameFile, const bool _bConvert);

		const bool getInformation(const RandomOption::GENERATE _typeGenerate, const RandomOption::RANDOM_OPTION _typeRandomOption, RandomOption::Information& _informationReference) const;
		const bool getInformation(const RandomOption::GENERATE _typeGenerate, const RandomOption::RANDOM_OPTION _typeRandomOption, const unsigned int _indexValue, RandomOption::Information& _informationReference) const;

		const bool setInformation(const RandomOption::GENERATE _typeGenerate, const RandomOption::RANDOM_OPTION _typeRandomOption, RandomOption::Information& _information);
		const bool setInformation(const RandomOption::GENERATE _typeGenerate, const RandomOption::RANDOM_OPTION _typeRandomOption, const unsigned int _indexValue, RandomOption::Information& _information);

		const bool removeValueBunch(const RandomOption::GENERATE _typeGenerate, const RandomOption::RANDOM_OPTION _typeRandomOption);
		const bool removeValueBunch(const RandomOption::GENERATE _typeGenerate, const unsigned int _indexValueBunch);
		const bool removeValue(const RandomOption::GENERATE _typeGenerate, const RandomOption::RANDOM_OPTION _typeRandomOption, const unsigned int _indexValue);
		const bool removeValue(const RandomOption::GENERATE _typeGenerate, const unsigned int _indexValueBunch, const unsigned int _indexValue);

		void refreshValueBunch(const RandomOption::GENERATE _typeGenerate, const RandomOption::RANDOM_OPTION _typeRandomOption);

		__forceinline const char* const getName(void) const;

		__forceinline const float getRateRandomOptionGenerate(void) const;
		__forceinline const float getRateRandomOptionDestroy(void) const;
		__forceinline const float getRateMoneySpend(void) const;
		__forceinline const unsigned int getSizeValueBunch(const RandomOption::GENERATE _typeGenerate) const;
		__forceinline const unsigned int getSizeRandomOptionGenerate(const RandomOption::GENERATE _typeGenerate) const;
		__forceinline const RandomOption::ValueBunchVector* const getVectorValueBunch(const RandomOption::GENERATE _typeGenerate) const;

		const WORD GetMinGain(void) const { return wMinGain; }
		const WORD GetMaxGain(void) const { return wMaxGain; }
	private:
		const bool loadFile_001(sc::BaseStream& _fileStream);
		const bool loadFile_002(sc::BaseStream& _fileStream);

	private:
		__forceinline RandomOption::ValueBunch* const _createValueBunch(const RandomOption::GENERATE _typeGenerate);

	public:
		__forceinline const bool operator ==(const char* const _cName) const;
		__forceinline const RandomOption::Entity& operator =(const RandomOption::Entity& _rhs);
		Entity(const RandomOption::Entity& _rhs);
		Entity(const RandomOption::Entity& _rhs, const char* const _cName);
		Entity(const char* const _cName);
		Entity(void);

	private:
		char cName[RandomOption::LENGTH_NAME];
		float fRateRandomOptionGenerate; // ���� �ɼ� �߻� Ȯ��;
		float fRateRandomOptionDestroy; // �������� �ı� �� Ȯ��;
		float fRateMoneySpend;	// ���� �� �ʿ��� �ݾ�(item�� �ܰ� �� rateMoneyGenerate);
		WORD wMinGain;			// �ּ� ȹ�� ��;
		WORD wMaxGain;			// �ִ� ȹ�� ��;
		unsigned int sizeRandomOptionGenerate[RandomOption::GENERATE_NSIZE]; // ���� �ɼ��� �߻� �� �� �ִ� �ִ� ����;
		RandomOption::ValueBunchVector vectorValueBunchDrop; // ���� �ɼ� ���̺�(drop);
		RandomOption::ValueBunchVector vectorValueBunchRemodel; // ���� �ɼ� ���̺�(����);
	};
	typedef std::list<RandomOption::Entity> EntityList;
	typedef EntityList::const_iterator EntityListConstIterator;
	typedef EntityList::iterator EntityListIterator;

	typedef std::map<unsigned int, RandomOption::EntityList> EntityListMap;
	typedef EntityListMap::const_iterator EntityListMapConstIterator;
	typedef EntityListMap::iterator EntityListMapIterator;
	typedef EntityListMap::value_type EntityListValue;



	// standard-library���� hash-map�� �����Ƿ� ���� ���� �� hash-map���� ���� �Ǿ� �ִ�;
	class Manager
	{
	public:
		enum
		{ // RandomOption::Entity ���� �׸�鿡 ���� ���� ������ ������ ����ٸ� VERSION�� �÷� �־�� �Ѵ�;
			VERSION = 0x0002L,
		};
		static const char* _FILEHEADSERVER;
		static const char* _FILEHEADCLIENT;
	public:
		void calcSize(void);
		const bool	saveFile(void);
		void		saveCsvHead(std::fstream& SFile) const;
		const bool	saveCsv(CWnd* pWnd);
		const bool	loadCsv(CWnd* pWnd);
		const bool loadFileServer(GLogicFile* const _pFileLogic, const bool _bTool);
		const bool loadFileClient(GLogicFile* const _pFileLogic);

		//  [2/23/2016 gbgim]
		// - Client, Server Data�а��� �ʿ��������;
		// - Client Data - �����ɼ� ����;
		//   Server Data - �����ɼ�, ����ɼ� ����;
		//   �̷��� ������ �ʿ�������;
		//   OpItemCheck�� ���� Client������ Drop������ �˾ƾ��Ѵ�;
		const bool loadFile(GLogicFile* const _pFileLogic, const bool _bTool);
		const bool loadFileTool(void);
		const bool loadFile_old(gltexfile& _fileText, const std::string& _stringNameFile, const bool _bConvert);

		void remove(const char* const _cName);

		const RandomOption::Entity* const find(const char* const _cName) const;
		RandomOption::Entity* const find(const char* const _cName);
		RandomOption::Entity* const create(const char* const _cName);
		RandomOption::Entity* const insert(const RandomOption::Entity& _entity);
		RandomOption::Entity* const rename(const char* const _cNameTarget, const char* const _cNameSource);

		__forceinline void clear(void);
		__forceinline const unsigned int size(void) const;		
		__forceinline const RandomOption::EntityListMap& getMapEntityList(void) const;
		__forceinline RandomOption::EntityListMap& getMapEntityList(void);

	private:
		const bool _loadFileServer(sc::BaseStream& _fileStream, const std::string& _stringNameFileRandomOption, const std::string& _stringPathFileRandomOption);		
		const bool BinExtract(void);

	public:
		__forceinline const RandomOption::Manager& operator =(const RandomOption::Manager&){};
		~Manager(void);
		static RandomOption::Manager& getInstance(void);		

	private:
		Manager(void);

	private:
		RandomOption::EntityListMap mapEntityList;
		unsigned int sizeEntity;
	};
}

#include "./RandomOption.hpp"
