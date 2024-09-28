#pragma once

#include <map>
#include <vector>
#include <string>
#include "../Crow/ItemGen.h"
#include "../SigmaCore/String/StringUtils.h"


/*******************************************************************************/
/***************************SpecID는 반드시 -1로 초기화해야함************************/
/*******************************************************************************/


namespace GenItem
{
	enum LoadCondition
	{
		ALL_DATA = 0,
		SELECT_DATA = 1,
	};

	enum MessageGenItem
	{
		LOAD_DATA = -1,
		FIND_DATA = -2,
		FIND_DATA_FROM_PART = -3,
		END_TOOL = -4,
	};

	enum FindFromWordCase
	{
		CROW_NAME_WORD = 100,
		GENITEMFILE_NAME_WORD = 101,
		ITEM_NAME_WORD = 102,
		NONE_NAME_WORD = -1
	};

	struct ItemInfo
	{
		SNATIVEID			m_ItemID;
		std::string				m_ItemName;
		int					m_SpecID;

		ItemInfo()
			: m_SpecID(-1), m_ItemID(65535, 65535)
		{
			m_ItemName.erase();
		}

		ItemInfo(const ItemInfo& _ItemInfo)
		:	m_ItemID(_ItemInfo.m_ItemID.wMainID, _ItemInfo.m_ItemID.wSubID),
			m_ItemName(_ItemInfo.m_ItemName),
			m_SpecID(_ItemInfo.m_SpecID)
		{}

		ItemInfo& operator =(const ItemInfo &_Ref)
		{
			if(this != &_Ref)
			{			
				m_ItemID = _Ref.m_ItemID;
				m_ItemName = _Ref.m_ItemName;
				m_SpecID = _Ref.m_SpecID;
			}

			return *this;
		}
	}; //Item.isf 파일 정보들에서 필요한 것들을 담기위한 구조체 GenItemTool용

	struct SpecIDInfo
	{
		std::vector<int>		m_ItemIndex;
		std::vector<int>		m_GenItemNameKey;

		SpecIDInfo operator =(const SpecIDInfo &_Ref)
		{
			SpecIDInfo _SpecIDInfo;

			for(unsigned int i = 0; i < _Ref.m_ItemIndex.size(); ++i)
			{				
				_SpecIDInfo.m_ItemIndex.push_back(_Ref.m_ItemIndex[i]);
			}

			for(unsigned int i = 0; i < _Ref.m_GenItemNameKey.size(); ++i)
			{
				_SpecIDInfo.m_GenItemNameKey.push_back(_Ref.m_GenItemNameKey[i]);
			}

			return _SpecIDInfo;
		}

	};	//SpecID관련 정보, Map컨테이너에 담음, m_GenItemNameKey는 이름만 따로 모아둔 스트링 컨테이너의 index

	struct GenItemInfo
	{
		enum GenType
		{
			GEN_MONEY = 0, 
			GEN_ITEM,
			GEN_ITEM_WITH_SKILL,
			GEN_ITEM_QBOX,
			GEN_NONE,
		};

		struct SpIDnRate
		{
			int				m_SpecID;
			float				m_Rate;
			GenType			m_GenType;
			SNATIVEID		m_SkillID;

			SpIDnRate()
			:	m_SpecID(-1), 
				m_Rate(0),
				m_GenType(GEN_NONE),
				m_SkillID(65535, 65535)
			{}

			void Clear()
			{
				m_SpecID = -1;
				m_Rate = 0;
				m_GenType = GEN_NONE;
				m_SkillID.wMainID = 65535;
				m_SkillID.wMainID = 65535;
			}
		};

		std::vector<SpIDnRate>	m_SpIDnRate;
		int					m_GenNum;
		float					m_GenRate;
		int					m_GenItemFileNameindex;
		

		GenItemInfo()
		:	m_GenRate(0), m_GenNum(0)
		{}

		GenItemInfo& operator =(const GenItemInfo& _Ref)
		{
			if(this == &_Ref)
				return *this;			

			for(unsigned int i = 0; i< _Ref.m_SpIDnRate.size(); ++i)
			{
				m_SpIDnRate.push_back(_Ref.m_SpIDnRate[i]);
			}

			m_GenItemFileNameindex = _Ref.m_GenItemFileNameindex;
			m_GenNum = _Ref.m_GenNum;
			m_GenRate = _Ref.m_GenRate;			

			return *this;
		}
	}; 
	//.genitem 의 확장자를 가진 파일들의 정보를 담기위한 구조체, GenItemTool 용

	struct CrowInfo
	{
		SNATIVEID			 m_CrowID;
		std::string				 m_CrowName;
		std::vector<int>		 m_GenItemFileNameindex;

		CrowInfo()
			: m_CrowID(65535, 65535)
		{
			m_CrowName.erase();
		}


		CrowInfo(const CrowInfo& _CrowInfo)
		:	m_CrowID(_CrowInfo.m_CrowID.wMainID, _CrowInfo.m_CrowID.wSubID),
			m_CrowName(_CrowInfo.m_CrowName)		
		{
			for(unsigned int i =0; i < _CrowInfo.m_GenItemFileNameindex.size(); ++i)
			{
				m_GenItemFileNameindex.push_back(_CrowInfo.m_GenItemFileNameindex[i]);
			}
		}

		CrowInfo& operator=(const CrowInfo&_Ref)
		{
			if(this == &_Ref) return *this;
			
			m_CrowID = _Ref.m_CrowID;
			m_CrowName = _Ref.m_CrowName;
			
			for(unsigned int i = 0; i < _Ref.m_GenItemFileNameindex.size(); ++i)
			{
				m_GenItemFileNameindex.push_back(_Ref.m_GenItemFileNameindex[i]);
			}

			return *this;
		}

	}; 
	//crow파일의 정보를 담기위한 구조체, m_GenItemFileNameindex은 말그대로 인덱스임
	//이름이 저장된 string 컨테이너의 index를 저장함		

	struct SearchData
	{
		std::vector<SNATIVEID*> m_vecCrowID;

		std::string	  m_CrowID_MID;
		std::string  m_CrowID_SID;
		std::string  m_CrowName;

		std::string  m_GenItemFileName;
		std::string	  m_SpecID;

		std::vector<SNATIVEID*> m_vecItemID;

		std::string  m_ItemID_MID;
		std::string  m_ItemID_SID;
		std::string  m_ItemName;

		SearchData()
		{
			m_SpecID.erase();
			m_CrowName.erase();
			m_GenItemFileName.erase();			
			m_ItemName.erase();
		}

		~SearchData()
		{
			unsigned int _CrowSize = m_vecCrowID.size();
			for(unsigned int _index = 0; _index < _CrowSize; ++_index)
			{
				delete m_vecCrowID[_index];
			}

			unsigned int _ItemSize = m_vecItemID.size();
			for(unsigned int _index = 0; _index < _ItemSize; ++_index)
			{
				delete m_vecItemID[_index];
			}
		}

		void clear()
		{
			
			m_CrowName.erase();
			m_GenItemFileName.erase();
			m_SpecID.erase();			
			m_ItemName.erase();
		}	
	};

	//툴에서 검색어 정보를 저장할 구조체, 여기에 담긴 데이터들을 기준으로 해서 검색을 진행함

	struct LinkLostData
	{
		enum TypeData
		{
			ITEM_DATA = 300,
			SPECID_DATA,
			GENITEMFILE_DATA ,
			CROW_DATA,
			NONE = -1
		};	
		
		TypeData m_Type;
		std::string m_StateMessage;	
		std::string m_ItemName;
		SNATIVEID m_ItemID;					
		int m_SpecID;
		std::string m_GenItemFileName;
		SNATIVEID m_CrowID;
		std::string	  m_CrowName;
	

		LinkLostData()
		:	m_Type(NONE), m_StateMessage(""), m_ItemName(""), m_SpecID(-1), m_GenItemFileName(""),
			m_CrowName(""), m_CrowID(65535, 65535), m_ItemID(65535, 65535)
		{			
		}

		static VOID SaveCsvHead(std::fstream &_sFile)
		{
			_sFile << "Type" << ",";
			_sFile << "Message" << ",";
			
			_sFile << "ItemID wMainID" << ",";
			_sFile << "ItemID wSubID" << ",";
			_sFile << "ItemName" << ",";
			
			_sFile << "SpecID" << ",";
			
			_sFile << "GenItemFileName" << ",";
			
			_sFile << "CrowID wMainID" << ",";
			_sFile << "CrowId wSubID" << ",";
			_sFile << "CrowName" << ",";

			_sFile << std::endl;
		}

		VOID SaveCsv(std::fstream &_sFile)
		{
			_sFile << m_Type << ",";
			_sFile << m_StateMessage << ",";

			_sFile << m_ItemID.wMainID << ",";
			_sFile << m_ItemID.wSubID << ",";
			_sFile << m_ItemName << ",";

			_sFile << m_SpecID << ",";
			
			_sFile << m_GenItemFileName << ",";

			_sFile << m_CrowID.wMainID << ",";
			_sFile << m_CrowID.wSubID << ",";
			_sFile << m_CrowName << ",";

			_sFile << std::endl;
		}
	};
	//데이터 정렬이 끝난 후 데이터 간의 연결이 끊긴 데이터들을 저장할 구조체
	//Crow -> GenitemFile Lost, 
	//GenItemFile -> Crow or SpecID Lost, 
	//SpecID -> GenItemFile or Item Lost,
	//Item -> SpecID Lost
	//네 종류의 데이터 연결 끊어짐 종류

	struct GenItemData
	{
		int nSpecID; // GenItem 그룹 ID;

		SNATIVEID skillID; // 아이템이 드랖 되기 위해 필요한 스킬(걸려 있는 스킬);

		SNATIVEID itemID; // 아이템 ID;
		std::string stringItemName; // 아이템 이름;	

		SNATIVEID crowID; // 이 아이템을 드랖하는 Crow의 ID;
		std::string stringCrowName; // Crow의 이름;

		std::string stringGenItemFileName; // GenItem 그룹을 가진 파일 이름;

		int sizeGenNum; // GenItem 그룹 내에서 드랖 될 수량;

		float rateSpec; // GenItem 그룹 내에서 nSpecID에 속한 아이템이 드랖 될 확률;
		float rateGen; // GenItem으로 인해 아이템이 발생할 확률;
		float rateTotal; // 위 두 확률의 곱(nSpecID가 선택 될 최종 확률);

		GenItemData()
		:	nSpecID(-1), sizeGenNum(0), rateSpec(0), rateGen(0), rateTotal(0),
			skillID(65535, 65535), itemID(65535, 65535), crowID(65535, 65535)
		{
			stringItemName.erase();
			stringCrowName.erase();
			stringGenItemFileName.erase();
		}		

		static VOID SaveCsvHead(std::fstream &_SFile)
		{
			_SFile << "SPECID" << ",";

			_SFile << "ItemID wMainID" << ",";
			_SFile << "ItemID wSubID" << ",";
			_SFile << "ItemName" << ",";

			_SFile << "CrowID wMainID" << ",";
			_SFile << "CrowID wSubID" << ",";
			_SFile << "CrowName" << ",";

			_SFile << "GenItemFileName" << ",";

			_SFile << "GenNum" << ",";

			_SFile << "RateSpec(%)" << ",";
			_SFile << "RateGen(%)" << ",";
			_SFile << "RateTotal(%)" << ",";

			_SFile << "SkillID wMainID" << ",";
			_SFile << "SkillID wSubID" << ",";


			_SFile << std::endl;
		}

		VOID SaveCsv(std::fstream &_SFile)
		{
			_SFile << nSpecID << ",";			
			
			_SFile << itemID.wMainID << ",";
			_SFile << itemID.wSubID << ",";
			_SFile << stringItemName << ",";

			_SFile << crowID.wMainID << ",";
			_SFile << crowID.wSubID << ",";
			_SFile << stringCrowName << ",";

			_SFile << stringGenItemFileName << ",";

			_SFile << sizeGenNum << ",";

			_SFile << rateSpec*100 << ",";
			_SFile << rateGen*100 << ",";
			_SFile << rateTotal*100 << ",";

			_SFile << skillID.wMainID << ",";
			_SFile << skillID.wSubID << ",";


			_SFile << std::endl;
		}
	};

	struct GenItemEditData
	{
		enum GenFileEditType
		{
			FILEGENRATE = 5,
			NUMRATE,
			SPIDNRATE_ITEMSPEC,
			SPIDNRATE_SKILLFACT,			
			NONE 
		};

		struct NumRate
		{
			GenFileEditType  m_Type;
			int			  m_Num;
			float			  m_NumRate;
			std::string		  m_StrName;

			NumRate()
			:m_Type(NONE), m_Num(-1), m_NumRate(0.0f)
			{}

			NumRate(NumRate &_Ref)
			:m_Type(_Ref.m_Type), m_Num(_Ref.m_Num), m_NumRate(_Ref.m_NumRate)
			{}
		};

		struct SpIDnRate
		{
			GenFileEditType	m_Type;
			int				m_SpecID;
			float				m_Rate;			
			SNATIVEID		m_SkillID;
			std::string		  m_StrName;
			std::string		  m_StrAnnotation;

			SpIDnRate()
			:m_Type(NONE), m_SpecID(-1), m_Rate(0.0f), m_SkillID(65535, 65535), m_StrAnnotation("")
			{}

			SpIDnRate(SpIDnRate &_Ref)
			:m_Type(_Ref.m_Type), m_SpecID(_Ref.m_SpecID), 
			m_Rate(_Ref.m_Rate), m_SkillID(_Ref.m_SkillID.wMainID, _Ref.m_SkillID.wSubID)
			{}
		};

		struct FileGenRate
		{
			GenFileEditType m_Type;
			float			  m_Rate;
			std::string		  m_StrName;

			FileGenRate()
			:m_Type(NONE), m_Rate(0.0f)
			{}

			FileGenRate(FileGenRate &_Ref)
			:m_Type(_Ref.m_Type), m_Rate(_Ref.m_Rate)
			{}
		};		
		
	
		std::list<SpIDnRate*> m_listSpIDnRate;
		std::list<NumRate*> m_listNumRate;
		FileGenRate m_FileGenRate;		

		void erase()
		{
			std::list<SpIDnRate*>::iterator _Spbegin = m_listSpIDnRate.begin();
			std::list<SpIDnRate*>::iterator _Spend = m_listSpIDnRate.end();

			for(;_Spbegin != _Spend;)
			{
				if(m_listSpIDnRate.empty()) break;

				delete *_Spbegin;
				m_listSpIDnRate.erase(_Spbegin++);
			}

			std::list<NumRate*>::iterator _NumBegin = m_listNumRate.begin();
			std::list<NumRate*>::iterator _NumEnd = m_listNumRate.end();

			for(;_NumBegin != _NumEnd;)
			{
				if(m_listNumRate.empty()) break;

				delete *_NumBegin;
				m_listNumRate.erase(_NumBegin++);
			}
		}
	};
}