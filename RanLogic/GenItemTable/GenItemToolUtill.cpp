#include "pch.h"
#include "./GenItemToolUtill.h"
#include "../Crow/GLCrowData.h"
#include "./GenItemTable.h"
#include "../enginelib/G-Logic/GLogic.h"

GenItem::GenItemTool::GenItemTool(void)
{
}

GenItem::GenItemTool::~GenItemTool(void)
{
	DeleteMapSecond(m_MapSpecID);
	DeleteMapSecond(m_MapGenItemInfo);
}

bool GenItem::GenItemTool::SetItemInfo()
{
	unsigned int _index(0);	
 
	size_t _Mid(GLItemMan::GetInstance().GetMaxMid()), _Sid(GLItemMan::GetInstance().GetMaxSid());

	for(size_t i = 0; i < _Mid; ++ i) //MID의 Max, GLItemMan.h 참조
	{
		for(size_t j = 0; j < _Sid; ++j) //SID의 Max
		{
			CItemNode* TempItemNode(GLItemMan::GetInstance().GetItemNode(i, j));

			if(TempItemNode)
			{
				SNATIVEID _Check = TempItemNode->m_sItem.BasicId();		

				if(_Check.wMainID >= 65535 || _Check.wSubID >= 65535)
					continue;
			}

			else
				continue;			

			ItemInfo _ItemInfo;
			_ItemInfo.m_ItemID.wMainID = TempItemNode->m_sItem.BasicId().wMainID;
			_ItemInfo.m_ItemID.wSubID = TempItemNode->m_sItem.BasicId().wSubID;
			_ItemInfo.m_ItemName = TempItemNode->m_sItem.GetNameStr();

			if(TempItemNode->m_sItem.sGenerateOp.dwSpecID != 0)
			_ItemInfo.m_SpecID = TempItemNode->m_sItem.sGenerateOp.dwSpecID;	

			m_VecItemInfo.push_back(_ItemInfo);
			m_VecItemName.push_back(m_VecItemInfo[_index].m_ItemName); //ItemNameString 테이블					
			
			_index++;
		}
	}


	return true;
}

bool GenItem::GenItemTool::SetCrowInfo()
{
	unsigned int _index(0);

	size_t _CrowMid(GLCrowDataMan::GetInstance().GetCrowMaxMID()), _CrowSid(GLCrowDataMan::GetInstance().GetCrowMaxSID());

	for(size_t i = 0; i < _CrowMid; ++i) //Crow MID의 Max, GLCrowDataMan.h 참조
	{
		for(size_t j = 0; j < _CrowSid; ++j) //Crow SID의 Max
		{
			SCROWDATA* TempCrowNode(GLCrowDataMan::GetInstance().GetCrowData(i, j)); 
			
			if(TempCrowNode == NULL)
				continue;

			CrowInfo _CrowInfo;

			_CrowInfo.m_CrowID.wMainID = i;
			_CrowInfo.m_CrowID.wSubID = j;
			_CrowInfo.m_CrowName =TempCrowNode->GetNameStr();
			
			m_VecCrowInfo.push_back(_CrowInfo);	
			m_VecCrowName.push_back(m_VecCrowInfo[_index].m_CrowName); //CrowNameString 테이블

			unsigned int _MaxFileIndex = TempCrowNode->m_sGenerate.m_vecGeneralGenItem.size();
			for(unsigned int _FileIndex = 0; _FileIndex < _MaxFileIndex; ++_FileIndex)
			{			
				std::string _GenItemFileName = TempCrowNode->m_sGenerate.m_vecGeneralGenItem[_FileIndex];
				int _IndexKey = FindIndexFromGenFileName(_GenItemFileName);		

				if(_IndexKey != -1)
				{					
					m_VecCrowInfo[_index].m_GenItemFileNameindex.push_back(_IndexKey);
				}
			}	

			_index++;
		}
	}
	return true;
}

bool GenItem::GenItemTool::SetGenItemInfo()
{
	GenItem::GenItemMapIterator _tempBegin(GenItem::Table::getInstance().GetGenItemMapIterBegin());
	GenItem::GenItemMapIterator _tempEnd(GenItem::Table::getInstance().GetGenItemMapIterEnd());
	
	for(unsigned int _index(0); _tempBegin != _tempEnd; ++_tempBegin, ++_index)
	{
		std::string _KeyString = GenItem::Table::getInstance().GetVecGenItemName(_index); //0번 인덱스부터 파일이름을 가져옴
		GenItem::GenItemMapIterator _tempIter = GenItem::Table::getInstance().GetGenItemMapIter(_KeyString); //가져온 파일이름으로 맵에서 데이터 가져옴		
		SGENINFO &_tempInfo = _tempIter->second;

		GenItemInfo* _GenIteminfo = new GenItemInfo;
		_GenIteminfo->m_GenItemFileNameindex = _index;
		_GenIteminfo->m_GenRate = _tempInfo.fProbability/100;	
		_GenIteminfo->m_GenNum = _tempInfo.vecGenItem.size();
		int _tempMaxNum = _GenIteminfo->m_GenNum;

		for(int i = 0; i < _tempMaxNum; ++i)
		{
			GenItemInfo::SpIDnRate _SpIDnRate;

			switch(_tempInfo.vecGenItem[i]->emType)
			{
				case EMGENITEMTYPE_ITEM:
					{						
						std::tr1::shared_ptr<SGENITEMSPEC> pTempGen = std::tr1::static_pointer_cast<SGENITEMSPEC>(_tempInfo.vecGenItem[i]);
						
						_SpIDnRate.Clear();

						if(pTempGen->dwSpecID != 0 && pTempGen->dwSpecID <= GLItemMan::SPECID_NUM)
						_SpIDnRate.m_SpecID = pTempGen->dwSpecID;

						_SpIDnRate.m_Rate = pTempGen->fRate/100;
						_SpIDnRate.m_GenType = GenItemInfo::GEN_ITEM;

						
						_GenIteminfo->m_SpIDnRate.push_back(_SpIDnRate);		
						break;
					}

				case EMGENITEMTYPE_ITEMSKILLFACT:
					{
						std::tr1::shared_ptr<SGENITEMSPECSKILLFACT> pTempGen = std::tr1::static_pointer_cast<SGENITEMSPECSKILLFACT>(_tempInfo.vecGenItem[i]);
						
						_SpIDnRate.Clear();

						if(pTempGen->dwSpecID != 0 &&  pTempGen->dwSpecID <= GLItemMan::SPECID_NUM)
						_SpIDnRate.m_SpecID = pTempGen->dwSpecID;

						_SpIDnRate.m_Rate = pTempGen->fRate/100;
						_SpIDnRate.m_SkillID.wMainID = pTempGen->sSkillID.wMainID;
						_SpIDnRate.m_SkillID.wSubID = pTempGen->sSkillID.wSubID;
						_SpIDnRate.m_GenType = GenItemInfo::GEN_ITEM_WITH_SKILL;

						
						_GenIteminfo->m_SpIDnRate.push_back(_SpIDnRate);		
						break;	
					}

				case EMGENITEMTYPE_MONEY:
					{
						std::tr1::shared_ptr<SGENMONEY> pTempGen = std::tr1::static_pointer_cast<SGENMONEY>(_tempInfo.vecGenItem[i]);

						_SpIDnRate.Clear();

						if(pTempGen->dwMoney != 0 &&  pTempGen->dwMoney <= GLItemMan::SPECID_NUM)
						_SpIDnRate.m_SpecID = pTempGen->dwMoney;

						_SpIDnRate.m_Rate = pTempGen->fRate/100;
						_SpIDnRate.m_GenType = GenItemInfo::GEN_MONEY;		
						
						_GenIteminfo->m_SpIDnRate.push_back(_SpIDnRate);		
						break;
					}

				case EMGENITEMTYPE_QITEM:
					{
						std::tr1::shared_ptr<SGENQITEMID> pTempGen = std::tr1::static_pointer_cast<SGENQITEMID>(_tempInfo.vecGenItem[i]);
						
						_SpIDnRate.Clear();

						_SpIDnRate.m_SkillID = pTempGen->sQItemID; //Q아이템 너무 예외적인 존재, 임시로 넣음
						_SpIDnRate.m_Rate = pTempGen->fRate/100;
						_SpIDnRate.m_GenType = GenItemInfo::GEN_ITEM_QBOX;
					
						_GenIteminfo->m_SpIDnRate.push_back(_SpIDnRate);		
						break;
					}
			}
		}
		 
		m_MapGenItemInfo.insert(std::pair<int, GenItemInfo*>(_index, _GenIteminfo) );
	}

	return true;
}

bool GenItem::GenItemTool::SetSpecIDMap()
{
	for(int SpecID = 0; SpecID < GLItemMan::SPECID_NUM; ++SpecID)
	{
		SpecIDInfo* _SpecIDInfo = new SpecIDInfo;
		
		for(unsigned int i = 0; i < m_VecItemInfo.size(); ++i)
		{
			if(m_VecItemInfo[i].m_SpecID == -1) continue;

			if(m_VecItemInfo[i].m_SpecID == SpecID)
				_SpecIDInfo->m_ItemIndex.push_back(i);
		}

		GenItemInfoMapIter _Begin(m_MapGenItemInfo.begin()), _End(m_MapGenItemInfo.end());

		for(; _Begin != _End; ++_Begin)
		{
			GenItemInfo* _GenIteminfo = _Begin->second;
			for(unsigned int k = 0; k < _GenIteminfo->m_SpIDnRate.size(); ++k)
			{		
				if(_GenIteminfo->m_SpIDnRate[k].m_SpecID == SpecID)
				{
					unsigned int _Size = _SpecIDInfo->m_GenItemNameKey.size();
					bool _Check(true);
					for(unsigned int _index(0); _index < _Size; ++_index)
					{
						if(_SpecIDInfo->m_GenItemNameKey[_index] == _GenIteminfo->m_GenItemFileNameindex)
							_Check = false;
					}
					if(_Check)
						_SpecIDInfo->m_GenItemNameKey.push_back(_GenIteminfo->m_GenItemFileNameindex);
				}
			}
		}	
		m_MapSpecID.insert(std::pair<int, SpecIDInfo*>(SpecID, _SpecIDInfo));
	}

	return true;
}

void GenItem::GenItemTool::GetItemInfo(ItemInfo &_Info, std::string _Name)
{
	unsigned int _Size = m_VecItemInfo.size();
	for(unsigned int _index(0); _index < _Size; ++_index)
	{
		if(m_VecItemInfo[_index].m_ItemName == _Name)
			_Info = m_VecItemInfo[_index];
	}
}

void GenItem::GenItemTool::GetItemInfo(ItemInfo &_Info, int _mid, int _sid)
{
	unsigned int _Size = m_VecItemInfo.size();
	for(unsigned int _index(0); _index < _Size; ++_index)
	{
		if(m_VecItemInfo[_index].m_ItemID.wMainID == _mid && m_VecItemInfo[_index].m_ItemID.wSubID == _sid)
			_Info = m_VecItemInfo[_index];
	}
}

void GenItem::GenItemTool::GetItemInfo(ItemInfo &_Info, unsigned int _Index)
{
	if(_Index < 0 || _Index > m_VecItemInfo.size()) return;

	_Info = m_VecItemInfo[_Index];
}

void GenItem::GenItemTool::GetSpecIDInfo(SpecIDInfo* &_Info, int _SpecID)
{
	if(_SpecID < 0 || _SpecID >= GLItemMan::SPECID_NUM) return;
	SpecIDInfoMapIter Iter = m_MapSpecID.find(_SpecID);
	_Info = Iter->second;
}

template <typename T>
bool GenItem::GenItemTool::DeleteMapSecond(std::map<int, T*> &_Map)
{
	std::map<int, T*>::iterator _iterBegin(_Map.begin());
	std::map<int, T*>::iterator _iterEnd(_Map.end());	

	for(; _iterBegin != _iterEnd; _iterBegin++)
	{
		T* _second = _iterBegin->second;
		delete _second;
	}

	return true;
}


bool GenItem::GenItemTool::SetGenItemToolInfo()
{
	bool _Check[4] = {false};
	
	_Check[0] =	SetItemInfo(); 
	_Check[1] =	SetCrowInfo(); 
	_Check[2] =	SetGenItemInfo();
	_Check[3] =	SetSpecIDMap();

	if(_Check[0] == true && _Check[1] == true && _Check[2] == true && _Check[3] == true)
		return true;
	else
		return false;
}

int GenItem::GenItemTool::FindIndexFromGenFileName(std::string _FileName)
{
	unsigned int _Max = GenItem::Table::getInstance().GetVecGenItemNameSize();
	for(unsigned int i =0; i < _Max; ++i)
	{
		std::string _CompareName = GenItem::Table::getInstance().GetVecGenItemName(i);

		if(!strcmp(_FileName.c_str(), _CompareName.c_str()))
		{
			return i;
		}
	}
	return -1;
}
/*데이터 로딩에서 읽어드린 아이템 데이터들 중에서 필요한 것들만 추출해서 검색툴에서 쓰일 형태의 구조체에 담음*/
/********************첫 검색어를 통한 데이터 정렬*****************************/
bool GenItem::GenItemTool::SetDataStartCrow_NativeID(std::vector<GenItemData*> *_Vec, const SNATIVEID &_CrowID)
{
	if(_CrowID.wMainID != 65535 && _CrowID.wSubID != 65535)
	{
		unsigned int _tempCrowNum(m_VecCrowInfo.size());
		for(unsigned int _index = 0; _index < _tempCrowNum; ++_index)
		{
			if(m_VecCrowInfo[_index].m_CrowID.wMainID == _CrowID.wMainID && 
			  m_VecCrowInfo[_index].m_CrowID.wSubID == _CrowID.wSubID)
			{
				unsigned int _tempGenFile(m_VecCrowInfo[_index].m_GenItemFileNameindex.size());
				for(unsigned int _NumGenItemFile = 0; _NumGenItemFile < _tempGenFile; ++_NumGenItemFile)
				{			
					GenItemInfoMapIter _GenIter = m_MapGenItemInfo.find(m_VecCrowInfo[_index].m_GenItemFileNameindex[_NumGenItemFile]);
					GenItemInfo* _tempGenIt = _GenIter->second;

					unsigned int _tempSpecID(_tempGenIt->m_SpIDnRate.size());
					for(unsigned int _NumSpecID = 0; _NumSpecID < _tempSpecID; ++_NumSpecID)
					{		
						if(_tempGenIt->m_SpIDnRate[_NumSpecID].m_SpecID == -1) continue;
						SpecIDInfoMapIter _SpInfoIter = m_MapSpecID.find(_tempGenIt->m_SpIDnRate[_NumSpecID].m_SpecID);
						SpecIDInfo* _tempSpecIDinfo = _SpInfoIter->second;
						
						unsigned int _tempItem(_tempSpecIDinfo->m_ItemIndex.size());
						for(unsigned int _NumItem = 0; _NumItem < _tempItem; ++_NumItem)
						{
							GenItemData* _GenData = new GenItemData; 	
							_GenData->crowID.wMainID = m_VecCrowInfo[_index].m_CrowID.wMainID;
							_GenData->crowID.wSubID = m_VecCrowInfo[_index].m_CrowID.wSubID;				
							_GenData->stringCrowName = m_VecCrowInfo[_index].m_CrowName;

							_GenData->stringGenItemFileName = GenItem::Table::getInstance().GetVecGenItemName(m_VecCrowInfo[_index].m_GenItemFileNameindex[_NumGenItemFile]);
							_GenData->nSpecID = _tempGenIt->m_SpIDnRate[_NumSpecID].m_SpecID;
							
							_GenData->sizeGenNum = _tempGenIt->m_SpIDnRate.size();
							_GenData->rateGen = _tempGenIt->m_GenRate;
							_GenData->rateSpec = _tempGenIt->m_SpIDnRate[_NumSpecID].m_Rate;
							_GenData->rateTotal = _tempGenIt->m_GenRate * _tempGenIt->m_SpIDnRate[_NumSpecID].m_Rate;
							
							if(_tempGenIt->m_SpIDnRate[_NumSpecID].m_GenType == GenItemInfo::GEN_ITEM_WITH_SKILL)
							{							
								_GenData->skillID.wMainID = _tempGenIt->m_SpIDnRate[_NumSpecID].m_SkillID.wMainID;
								_GenData->skillID.wSubID = _tempGenIt->m_SpIDnRate[_NumSpecID].m_SkillID.wSubID;
							}

							_GenData->itemID.wSubID = m_VecItemInfo[_tempSpecIDinfo->m_ItemIndex[_NumItem]].m_ItemID.wSubID;
							_GenData->itemID.wMainID = m_VecItemInfo[_tempSpecIDinfo->m_ItemIndex[_NumItem]].m_ItemID.wMainID;
							_GenData->stringItemName = m_VecItemInfo[_tempSpecIDinfo->m_ItemIndex[_NumItem]].m_ItemName;

							_Vec->push_back(_GenData);							
						}
					}					
				}				
			}
		}
	}

	if(_Vec->size() == 0)
		return false;
	else
		return true;
}

bool GenItem::GenItemTool::SetDataStrartCrow_MID(std::vector<GenItemData*> *_Vec, const SNATIVEID &_CrowID)
{
	if(_CrowID.wMainID != 65535 || _CrowID.wSubID != 65535)
	{
		unsigned int _tempCrowNum(m_VecCrowInfo.size());
		for(unsigned int _index = 0; _index < _tempCrowNum; ++_index)
		{
			if(m_VecCrowInfo[_index].m_CrowID.wMainID == _CrowID.wMainID && _CrowID.wSubID == 65535)
			{
				unsigned int _tempGenFile(m_VecCrowInfo[_index].m_GenItemFileNameindex.size());
				for(unsigned int _NumGenItemFile = 0; _NumGenItemFile < _tempGenFile; ++_NumGenItemFile)
				{			
					GenItemInfoMapIter _GenIter = m_MapGenItemInfo.find(m_VecCrowInfo[_index].m_GenItemFileNameindex[_NumGenItemFile]);
					GenItemInfo* _tempGenIt = _GenIter->second;

					unsigned int _tempSpecID(_tempGenIt->m_SpIDnRate.size());
					for(unsigned int _NumSpecID = 0; _NumSpecID < _tempSpecID; ++_NumSpecID)
					{		
						if(_tempGenIt->m_SpIDnRate[_NumSpecID].m_SpecID == -1) continue;
						SpecIDInfoMapIter _SpInfoIter = m_MapSpecID.find(_tempGenIt->m_SpIDnRate[_NumSpecID].m_SpecID);
						SpecIDInfo* _tempSpecIDinfo = _SpInfoIter->second;

						unsigned int _tempItem(_tempSpecIDinfo->m_ItemIndex.size());
						for(unsigned int _NumItem = 0; _NumItem < _tempItem; ++_NumItem)
						{
							GenItemData* _GenData = new GenItemData; 	
							_GenData->crowID.wMainID = m_VecCrowInfo[_index].m_CrowID.wMainID;
							_GenData->crowID.wSubID = m_VecCrowInfo[_index].m_CrowID.wSubID;				
							_GenData->stringCrowName = m_VecCrowInfo[_index].m_CrowName;

							_GenData->stringGenItemFileName = GenItem::Table::getInstance().GetVecGenItemName(m_VecCrowInfo[_index].m_GenItemFileNameindex[_NumGenItemFile]);
							_GenData->nSpecID = _tempGenIt->m_SpIDnRate[_NumSpecID].m_SpecID;

							_GenData->sizeGenNum = _tempGenIt->m_SpIDnRate.size();
							_GenData->rateGen = _tempGenIt->m_GenRate;
							_GenData->rateSpec = _tempGenIt->m_SpIDnRate[_NumSpecID].m_Rate;
							_GenData->rateTotal = _tempGenIt->m_GenRate * _tempGenIt->m_SpIDnRate[_NumSpecID].m_Rate;

							if(_tempGenIt->m_SpIDnRate[_NumSpecID].m_GenType == GenItemInfo::GEN_ITEM_WITH_SKILL)
							{							
								_GenData->skillID.wMainID = _tempGenIt->m_SpIDnRate[_NumSpecID].m_SkillID.wMainID;
								_GenData->skillID.wSubID = _tempGenIt->m_SpIDnRate[_NumSpecID].m_SkillID.wSubID;
							}

							_GenData->itemID.wSubID = m_VecItemInfo[_tempSpecIDinfo->m_ItemIndex[_NumItem]].m_ItemID.wSubID;
							_GenData->itemID.wMainID = m_VecItemInfo[_tempSpecIDinfo->m_ItemIndex[_NumItem]].m_ItemID.wMainID;
							_GenData->stringItemName = m_VecItemInfo[_tempSpecIDinfo->m_ItemIndex[_NumItem]].m_ItemName;

							_Vec->push_back(_GenData);							
						}
					}
				}
			}
		}
	}

	if(_Vec->size() == 0)
		return false;
	else
		return true;
}

bool GenItem::GenItemTool::SetDataStartCrow_Name(std::vector<GenItemData*> *_Vec, const std::string _CrowName)
{
	if(_CrowName != "")
	{
		for(unsigned int _index = 0; _index < m_VecCrowInfo.size(); ++_index)
		{
			if(!strcmp(_CrowName.c_str(), m_VecCrowInfo[_index].m_CrowName.c_str()))
			{
				unsigned int _tempGenFile(m_VecCrowInfo[_index].m_GenItemFileNameindex.size());
				for(unsigned int _NumGenItemFile = 0; _NumGenItemFile < _tempGenFile; ++_NumGenItemFile)
				{			
					GenItemInfoMapIter _GenIter = m_MapGenItemInfo.find(m_VecCrowInfo[_index].m_GenItemFileNameindex[_NumGenItemFile]);
					GenItemInfo* _tempGenIt = _GenIter->second;

					unsigned int _tempSpecID(_tempGenIt->m_SpIDnRate.size());
					for(unsigned int _NumSpecID = 0; _NumSpecID < _tempSpecID; ++_NumSpecID)
					{
						if(_tempGenIt->m_SpIDnRate[_NumSpecID].m_SpecID == -1) continue;
						SpecIDInfoMapIter _SpInfoIter = m_MapSpecID.find(_tempGenIt->m_SpIDnRate[_NumSpecID].m_SpecID);
						SpecIDInfo* _tempSpecIDinfo = _SpInfoIter->second;

						unsigned int _tempItem(_tempSpecIDinfo->m_ItemIndex.size());
						for(unsigned int _NumItem = 0; _NumItem < _tempItem; ++_NumItem)
						{
							GenItemData* _GenData = new GenItemData; 	
							_GenData->crowID.wMainID = m_VecCrowInfo[_index].m_CrowID.wMainID;
							_GenData->crowID.wSubID = m_VecCrowInfo[_index].m_CrowID.wSubID;				
							_GenData->stringCrowName = m_VecCrowInfo[_index].m_CrowName;

							_GenData->stringGenItemFileName = GenItem::Table::getInstance().GetVecGenItemName(m_VecCrowInfo[_index].m_GenItemFileNameindex[_NumGenItemFile]);
							_GenData->nSpecID = _tempGenIt->m_SpIDnRate[_NumSpecID].m_SpecID;

							_GenData->sizeGenNum = _tempGenIt->m_GenNum;
							_GenData->rateGen = _tempGenIt->m_GenRate;
							_GenData->rateSpec = _tempGenIt->m_SpIDnRate[_NumSpecID].m_Rate;
							_GenData->rateTotal = _tempGenIt->m_GenRate * _tempGenIt->m_SpIDnRate[_NumSpecID].m_Rate;

							if(_tempGenIt->m_SpIDnRate[_NumSpecID].m_GenType == GenItemInfo::GEN_ITEM_WITH_SKILL)
							{							
								_GenData->skillID.wMainID = _tempGenIt->m_SpIDnRate[_NumSpecID].m_SkillID.wMainID;
								_GenData->skillID.wSubID = _tempGenIt->m_SpIDnRate[_NumSpecID].m_SkillID.wSubID;
							}

							_GenData->itemID.wSubID = m_VecItemInfo[_tempSpecIDinfo->m_ItemIndex[_NumItem]].m_ItemID.wSubID;
							_GenData->itemID.wMainID = m_VecItemInfo[_tempSpecIDinfo->m_ItemIndex[_NumItem]].m_ItemID.wMainID;
							_GenData->stringItemName = m_VecItemInfo[_tempSpecIDinfo->m_ItemIndex[_NumItem]].m_ItemName;

							_Vec->push_back(_GenData);										
						}
					}					
				}				
			}
		}
	}

	if(_Vec->size() == 0)
		return false;
	else
		return true;
}

bool GenItem::GenItemTool::SetDataStartGenItem(std::vector<GenItemData*> *_Vec, std::string _GenItemName)
{	
	if(_GenItemName.c_str() == NULL) 
		return false;

	if(_GenItemName != "" && sc::string::FindWord(_GenItemName, ".genitem") == -1)
		_GenItemName.append(".genitem");

	unsigned int _tempNum(GenItem::Table::getInstance().GetVecGenItemNameSize());
	for(unsigned int _NumFileName = 0; _NumFileName < _tempNum; ++_NumFileName)
	{
		if(!strcmp(_GenItemName.c_str(), GenItem::Table::getInstance().GetVecGenItemName(_NumFileName).c_str()))
		{
			GenItemInfoMapIter _GenItemIter = m_MapGenItemInfo.find(_NumFileName);
			GenItemInfo* _GenItemInfo = _GenItemIter->second;

			unsigned int _tempCrow(m_VecCrowInfo.size());
			for(unsigned int _NumCrow = 0; _NumCrow < _tempCrow; ++_NumCrow)
			{
				unsigned int _tempNum(m_VecCrowInfo[_NumCrow].m_GenItemFileNameindex.size());
				for(unsigned int _NumFileindex = 0; _NumFileindex < _tempNum; ++_NumFileindex)
				{
					if(_GenItemInfo->m_GenItemFileNameindex != m_VecCrowInfo[_NumCrow].m_GenItemFileNameindex[_NumFileindex])	
						continue;

					unsigned int _tempSpecID(_GenItemInfo->m_SpIDnRate.size());
					for(unsigned int _NumSpecID = 0; _NumSpecID < _tempSpecID; ++_NumSpecID)
					{
						if(_GenItemInfo->m_SpIDnRate[_NumSpecID].m_SpecID == -1) continue;
						SpecIDInfoMapIter _SpecIDInfoIter = m_MapSpecID.find(_GenItemInfo->m_SpIDnRate[_NumSpecID].m_SpecID);
						SpecIDInfo* _SpecIDInfo = _SpecIDInfoIter->second;

						unsigned int _tempItemNum(_SpecIDInfo->m_ItemIndex.size());
						for(unsigned int _NumItem = 0; _NumItem < _tempItemNum; ++_NumItem)
						{
							GenItemData* _GenData = new GenItemData; 	
							
							_GenData->crowID.wMainID = m_VecCrowInfo[_NumCrow].m_CrowID.wMainID;
							_GenData->crowID.wSubID = m_VecCrowInfo[_NumCrow].m_CrowID.wSubID;
							_GenData->stringCrowName = m_VecCrowInfo[_NumCrow].m_CrowName;

							_GenData->stringGenItemFileName = GenItem::Table::getInstance().GetVecGenItemName(_NumFileName);
							
							_GenData->nSpecID = _GenItemInfo->m_SpIDnRate[_NumSpecID].m_SpecID;
							_GenData->sizeGenNum = _GenItemInfo->m_GenNum;
							_GenData->rateGen = _GenItemInfo->m_GenRate;
							_GenData->rateSpec = _GenItemInfo->m_SpIDnRate[_NumSpecID].m_Rate;
							_GenData->rateTotal = _GenData->rateGen * _GenData->rateSpec;

							if(_GenItemInfo->m_SpIDnRate[_NumSpecID].m_GenType == GenItemInfo::GEN_ITEM_WITH_SKILL)
							{							
								_GenData->skillID.wMainID = _GenItemInfo->m_SpIDnRate[_NumSpecID].m_SkillID.wMainID;
								_GenData->skillID.wSubID = _GenItemInfo->m_SpIDnRate[_NumSpecID].m_SkillID.wSubID;
							}		

							_GenData->itemID.wMainID = m_VecItemInfo[_SpecIDInfo->m_ItemIndex[_NumItem]].m_ItemID.wMainID;
							_GenData->itemID.wSubID = m_VecItemInfo[_SpecIDInfo->m_ItemIndex[_NumItem]].m_ItemID.wSubID;
							_GenData->stringItemName = m_VecItemInfo[_SpecIDInfo->m_ItemIndex[_NumItem]].m_ItemName;

							_Vec->push_back(_GenData);										
						}
					}
				}
			}				
		}		
	}

	if(_Vec->size() == 0)
		return false;
	else
		return true;
}

bool GenItem::GenItemTool::SetDataStartSpecID(std::vector<GenItemData*> *_Vec, const int _SpecID)
{
	if(_SpecID <= 0 || _SpecID >= GLItemMan::SPECID_NUM) return false;	

	SpecIDInfoMapIter _SpInfoIter = m_MapSpecID.find(_SpecID);
	SpecIDInfo* _SpecIDinfo = _SpInfoIter->second;
	std::vector<int> _tempGenItemNameKey;

	unsigned int _tempGenItemNum(_SpecIDinfo->m_GenItemNameKey.size());
	for(unsigned int _NumGenItem = 0; _NumGenItem < _tempGenItemNum; ++_NumGenItem)
	{
		GenItemInfoMapIter _GenItemInfoIter = m_MapGenItemInfo.find(_SpecIDinfo->m_GenItemNameKey[_NumGenItem]);
		GenItemInfo* _GenItemInfo = _GenItemInfoIter->second;

		unsigned int _tempCrowNum(m_VecCrowInfo.size());
		for(unsigned int _NumCrow = 0; _NumCrow < _tempCrowNum; ++_NumCrow)
		{
			unsigned int _tempGenItemNum(m_VecCrowInfo[_NumCrow].m_GenItemFileNameindex.size());
			for(unsigned int _GenItemNum = 0; _GenItemNum < _tempGenItemNum; ++_GenItemNum)
			{
				if(_SpecIDinfo->m_GenItemNameKey[_NumGenItem] != m_VecCrowInfo[_NumCrow].m_GenItemFileNameindex[_GenItemNum])
					continue;	
				
				unsigned int _tempSpecIDNum(_GenItemInfo->m_SpIDnRate.size());
				for(unsigned int _NumSpecID = 0; _NumSpecID < _tempSpecIDNum; ++_NumSpecID)
				{			
					if(_GenItemInfo->m_SpIDnRate[_NumSpecID].m_SpecID != _SpecID)
						continue;

					unsigned int _tempItemNum(_SpecIDinfo->m_ItemIndex.size());
					for(unsigned int _ItemIndex = 0; _ItemIndex < _tempItemNum; ++_ItemIndex)
					{
						GenItemData* _GenData = new GenItemData; 					

						_GenData->crowID.wMainID = m_VecCrowInfo[_NumCrow].m_CrowID.wMainID;
						_GenData->crowID.wSubID = m_VecCrowInfo[_NumCrow].m_CrowID.wSubID;
						_GenData->stringCrowName = m_VecCrowInfo[_NumCrow].m_CrowName;

						_GenData->stringGenItemFileName = GenItem::Table::getInstance().GetVecGenItemName(_SpecIDinfo->m_GenItemNameKey[_NumGenItem]);

						_GenData->nSpecID = _SpecID;
						_GenData->sizeGenNum = _GenItemInfo->m_SpIDnRate.size();
						_GenData->rateGen = _GenItemInfo->m_GenRate;						
						_GenData->rateSpec = _GenItemInfo->m_SpIDnRate[_NumSpecID].m_Rate;
						_GenData->rateTotal = _GenData->rateGen * _GenData->rateSpec;

						if(_GenItemInfo->m_SpIDnRate[_NumSpecID].m_GenType == GenItemInfo::GEN_ITEM_WITH_SKILL )
						{
							_GenData->skillID.wMainID = _GenItemInfo->m_SpIDnRate[_NumSpecID].m_SkillID.wMainID;
							_GenData->skillID.wSubID = _GenItemInfo->m_SpIDnRate[_NumSpecID].m_SkillID.wSubID;
						}
						
						_GenData->itemID.wMainID = m_VecItemInfo[_SpecIDinfo->m_ItemIndex[_ItemIndex]].m_ItemID.wMainID;
						_GenData->itemID.wSubID = m_VecItemInfo[_SpecIDinfo->m_ItemIndex[_ItemIndex]].m_ItemID.wSubID;
						_GenData->stringItemName = m_VecItemInfo[_SpecIDinfo->m_ItemIndex[_ItemIndex]].m_ItemName;

						_Vec->push_back(_GenData);			
					}
				}
			}
		}		
	}

	if(_Vec->size() == 0)
		return false;
	else
		return true;
}

bool GenItem::GenItemTool::SetDataStartItem_NativeID(std::vector<GenItemData*> *_Vec, const SNATIVEID &_ItemID)
{
	if(_ItemID.wMainID != 65535 && _ItemID.wSubID != 65535)
	{	
		unsigned int _tempItemNum(m_VecItemInfo.size());
		for(unsigned int _ItemNum = 0; _ItemNum < _tempItemNum; ++_ItemNum)
		{
			if(m_VecItemInfo[_ItemNum].m_ItemID.wMainID == _ItemID.wMainID && 
				m_VecItemInfo[_ItemNum].m_ItemID.wSubID == _ItemID.wSubID)
			{
				if(m_VecItemInfo[_ItemNum].m_SpecID == -1) continue;
				SpecIDInfoMapIter _SpecIDIter = m_MapSpecID.find(m_VecItemInfo[_ItemNum].m_SpecID);
				SpecIDInfo* _SpecIDInfo = _SpecIDIter->second;				

				unsigned int _tempSpecIDNum(_SpecIDInfo->m_GenItemNameKey.size());
				for(unsigned int _SpecIDNum = 0; _SpecIDNum < _tempSpecIDNum; ++_SpecIDNum)
				{
					GenItemInfoMapIter _GenItemInfoIter = m_MapGenItemInfo.find(_SpecIDInfo->m_GenItemNameKey[_SpecIDNum]);
					GenItemInfo* _GenItemInfo = _GenItemInfoIter->second;

					unsigned int _tempCrowNum(m_VecCrowInfo.size());
					for(unsigned int _CrowNum = 0; _CrowNum < _tempCrowNum; ++_CrowNum)
					{
						unsigned int _tempCrowGenItemNum(m_VecCrowInfo[_CrowNum].m_GenItemFileNameindex.size());
						for(unsigned int _CrowGenItemNum = 0; _CrowGenItemNum < _tempCrowGenItemNum; ++_CrowGenItemNum)
						{
							if(m_VecCrowInfo[_CrowNum].m_GenItemFileNameindex[_CrowGenItemNum] == _GenItemInfo->m_GenItemFileNameindex)
							{							
								unsigned int _tempSpIDnRateNum(_GenItemInfo->m_SpIDnRate.size());
								for(unsigned int _SpIDnRateNum = 0; _SpIDnRateNum < _tempSpIDnRateNum; ++_SpIDnRateNum)
								{
									if(m_VecItemInfo[_ItemNum].m_SpecID == _GenItemInfo->m_SpIDnRate[_SpIDnRateNum].m_SpecID)
									{		
										GenItemData* _GenData = new GenItemData; 								

										_GenData->crowID.wMainID = m_VecCrowInfo[_CrowNum].m_CrowID.wMainID;
										_GenData->crowID.wSubID = m_VecCrowInfo[_CrowNum].m_CrowID.wSubID;
										_GenData->stringCrowName = m_VecCrowInfo[_CrowNum].m_CrowName;

										_GenData->stringGenItemFileName =  GenItem::Table::getInstance().GetVecGenItemName(_GenItemInfo->m_GenItemFileNameindex);

										_GenData->nSpecID = m_VecItemInfo[_ItemNum].m_SpecID;
										_GenData->rateGen = _GenItemInfo->m_GenRate;

										if(_GenItemInfo->m_SpIDnRate[_SpIDnRateNum].m_GenType == GenItemInfo::GEN_ITEM_WITH_SKILL)
										{																				
												_GenData->skillID.wMainID = _GenItemInfo->m_SpIDnRate[_SpIDnRateNum].m_SkillID.wMainID;
												_GenData->skillID.wSubID = _GenItemInfo->m_SpIDnRate[_SpIDnRateNum].m_SkillID.wSubID;
										}	

										_GenData->rateSpec = _GenItemInfo->m_SpIDnRate[_SpIDnRateNum].m_Rate;
										_GenData->sizeGenNum = _GenItemInfo->m_GenNum;

										_GenData->rateTotal = _GenData->rateGen * _GenData->rateSpec;
										_GenData->itemID.wMainID = m_VecItemInfo[_ItemNum].m_ItemID.wMainID;
										_GenData->itemID.wSubID = m_VecItemInfo[_ItemNum].m_ItemID.wSubID;
										_GenData->stringItemName = m_VecItemInfo[_ItemNum].m_ItemName;

										_Vec->push_back(_GenData);										
									}
								}							
							}
						}
					}
				}				
			}
		}
	}

	if(_Vec->size() == 0)
		return false;
	else
		return true;
}

bool GenItem::GenItemTool::SetDataStartItem_MID(std::vector<GenItemData*> *_Vec, const SNATIVEID &_ItemID)
{
	if(_ItemID.wMainID != 65535 || _ItemID.wSubID != 65535)
	{
		unsigned int _tempItemNum(m_VecItemInfo.size());
		for(unsigned int _ItemNum = 0; _ItemNum < _tempItemNum; ++_ItemNum)
		{
			if(m_VecItemInfo[_ItemNum].m_ItemID.wMainID == _ItemID.wMainID && _ItemID.wSubID == 65535)
			{
				if(m_VecItemInfo[_ItemNum].m_SpecID == -1) continue;
				SpecIDInfoMapIter _SpecIDIter = m_MapSpecID.find(m_VecItemInfo[_ItemNum].m_SpecID);
				SpecIDInfo* _SpecIDInfo = _SpecIDIter->second;				

				unsigned int _tempSpecIDNum(_SpecIDInfo->m_GenItemNameKey.size());
				for(unsigned int _SpecIDNum = 0; _SpecIDNum < _tempSpecIDNum; ++_SpecIDNum)
				{
					GenItemInfoMapIter _GenItemInfoIter = m_MapGenItemInfo.find(_SpecIDInfo->m_GenItemNameKey[_SpecIDNum]);
					GenItemInfo* _GenItemInfo = _GenItemInfoIter->second;

					unsigned int _tempCrowNum(m_VecCrowInfo.size());
					for(unsigned int _CrowNum = 0; _CrowNum < _tempCrowNum; ++_CrowNum)
					{
						unsigned int _tempCrowGenItemNum(m_VecCrowInfo[_CrowNum].m_GenItemFileNameindex.size());
						for(unsigned int _CrowGenItemNum = 0; _CrowGenItemNum < _tempCrowGenItemNum; ++_CrowGenItemNum)
						{
							if(m_VecCrowInfo[_CrowNum].m_GenItemFileNameindex[_CrowGenItemNum] == _GenItemInfo->m_GenItemFileNameindex)
							{
								unsigned int _tempSpIDnRateNum(_GenItemInfo->m_SpIDnRate.size());
								for(unsigned int _SpIDnRateNum = 0; _SpIDnRateNum < _tempSpIDnRateNum; ++_SpIDnRateNum)
								{
									if(m_VecItemInfo[_ItemNum].m_SpecID == _GenItemInfo->m_SpIDnRate[_SpIDnRateNum].m_SpecID)
									{
										GenItemData* _GenData = new GenItemData; 								

										_GenData->crowID.wMainID = m_VecCrowInfo[_CrowNum].m_CrowID.wMainID;
										_GenData->crowID.wSubID = m_VecCrowInfo[_CrowNum].m_CrowID.wSubID;
										_GenData->stringCrowName = m_VecCrowInfo[_CrowNum].m_CrowName;

										_GenData->stringGenItemFileName =  GenItem::Table::getInstance().GetVecGenItemName(_GenItemInfo->m_GenItemFileNameindex);

										_GenData->nSpecID = m_VecItemInfo[_ItemNum].m_SpecID;
										_GenData->rateGen = _GenItemInfo->m_GenRate;

										if(_GenItemInfo->m_SpIDnRate[_SpIDnRateNum].m_GenType == GenItemInfo::GEN_ITEM_WITH_SKILL)
										{																					
											_GenData->skillID.wMainID = _GenItemInfo->m_SpIDnRate[_SpIDnRateNum].m_SkillID.wMainID;
											_GenData->skillID.wSubID = _GenItemInfo->m_SpIDnRate[_SpIDnRateNum].m_SkillID.wSubID;										
										}	

										_GenData->rateSpec = _GenItemInfo->m_SpIDnRate[_SpIDnRateNum].m_Rate;
										_GenData->sizeGenNum = _GenItemInfo->m_GenNum;

										_GenData->rateTotal = _GenData->rateGen * _GenData->rateSpec;
										_GenData->itemID.wMainID = m_VecItemInfo[_ItemNum].m_ItemID.wMainID;
										_GenData->itemID.wSubID = m_VecItemInfo[_ItemNum].m_ItemID.wSubID;
										_GenData->stringItemName = m_VecItemInfo[_ItemNum].m_ItemName;

										_Vec->push_back(_GenData);		
									}
								}																
							}
						}
					}
				}				
			}
		}
	}

	if(_Vec->size() == 0)
		return false;
	else
		return true;
}

bool GenItem::GenItemTool::SetDataStartItem_Name(std::vector<GenItemData*> *_Vec, const std::string _ItemName)
{
	if(_ItemName != "")
	{
		unsigned int _tempItemNum = m_VecItemInfo.size();
		for(unsigned int _ItemNum = 0; _ItemNum < _tempItemNum; ++_ItemNum)
		{
			if(!strcmp(_ItemName.c_str(), m_VecItemInfo[_ItemNum].m_ItemName.c_str()))
			{
				if(m_VecItemInfo[_ItemNum].m_SpecID == -1) continue;
				SpecIDInfoMapIter _SpecIDIter = m_MapSpecID.find(m_VecItemInfo[_ItemNum].m_SpecID);
				SpecIDInfo* _SpecIDInfo = _SpecIDIter->second;				
				
				unsigned int _tempSpecIDNum = _SpecIDInfo->m_GenItemNameKey.size();
				for(unsigned int _SpecIDNum = 0; _SpecIDNum < _tempSpecIDNum; ++_SpecIDNum)
				{
					GenItemInfoMapIter _GenItemInfoIter = m_MapGenItemInfo.find(_SpecIDInfo->m_GenItemNameKey[_SpecIDNum]);
					GenItemInfo* _GenItemInfo = _GenItemInfoIter->second;

					unsigned int _tempCrowNum = m_VecCrowInfo.size();					
					for(unsigned int _CrowNum = 0; _CrowNum < _tempCrowNum; ++_CrowNum)
					{
						unsigned int _tempCrowGenItemNum = m_VecCrowInfo[_CrowNum].m_GenItemFileNameindex.size();						
						for(unsigned int _CrowGenItemNum = 0; _CrowGenItemNum < _tempCrowGenItemNum; ++_CrowGenItemNum)
						{
							if(m_VecCrowInfo[_CrowNum].m_GenItemFileNameindex[_CrowGenItemNum] == _GenItemInfo->m_GenItemFileNameindex)
							{
								unsigned int _tempSpIDnRateNum = _GenItemInfo->m_SpIDnRate.size();
								for(unsigned int _SpIDnRateNum = 0; _SpIDnRateNum < _tempSpIDnRateNum; ++_SpIDnRateNum)
								{
									if(m_VecItemInfo[_ItemNum].m_SpecID == _GenItemInfo->m_SpIDnRate[_SpIDnRateNum].m_SpecID)
									{	
										GenItemData* _GenData = new GenItemData; 	

										_GenData->crowID.wMainID = m_VecCrowInfo[_CrowNum].m_CrowID.wMainID;
										_GenData->crowID.wSubID = m_VecCrowInfo[_CrowNum].m_CrowID.wSubID;
										_GenData->stringCrowName = m_VecCrowInfo[_CrowNum].m_CrowName;

										_GenData->stringGenItemFileName =  GenItem::Table::getInstance().GetVecGenItemName(_GenItemInfo->m_GenItemFileNameindex);

										_GenData->nSpecID = m_VecItemInfo[_ItemNum].m_SpecID;
										_GenData->rateGen = _GenItemInfo->m_GenRate;

										if(_GenItemInfo->m_SpIDnRate[_SpIDnRateNum].m_GenType == GenItemInfo::GEN_ITEM_WITH_SKILL)
										{								
											_GenData->skillID.wMainID = _GenItemInfo->m_SpIDnRate[_SpIDnRateNum].m_SkillID.wMainID;
											_GenData->skillID.wSubID = _GenItemInfo->m_SpIDnRate[_SpIDnRateNum].m_SkillID.wSubID;										
										}
										
										_GenData->rateSpec = _GenItemInfo->m_SpIDnRate[_SpIDnRateNum].m_Rate;
										_GenData->sizeGenNum = _GenItemInfo->m_GenNum;	

										_GenData->rateTotal = _GenData->rateGen * _GenData->rateSpec;
										_GenData->itemID.wMainID = m_VecItemInfo[_ItemNum].m_ItemID.wMainID;
										_GenData->itemID.wSubID = m_VecItemInfo[_ItemNum].m_ItemID.wSubID;
										_GenData->stringItemName = m_VecItemInfo[_ItemNum].m_ItemName;

										_Vec->push_back(_GenData);		
									}
								}									
							}
						}
					}
				}				
			}			
		}
	}

	if(_Vec->size() == 0)
		return false;
	else
		return true;
}
// 첫 검색어를 통한 데이터 세팅은 기준이 되는 검색어를 잡아서 그 이후로 타고가는 형식으로 데이터를 세팅함

bool GenItem::GenItemTool::JudgeSame(SNATIVEID _ID, std::string _Name, SNATIVEID &_SameID, Condition _Something)
{
	switch(_Something)
	{
		case CROW_ID:
			{
				unsigned int _Size = m_VecCrowInfo.size();
				for(unsigned int _Index = 0; _Index < _Size; ++_Index)
				{
					if(m_VecCrowInfo[_Index].m_CrowID == _ID)
					{
						if(m_VecCrowInfo[_Index].m_CrowName == _Name)
						{
							_SameID = m_VecCrowInfo[_Index].m_CrowID;
							return true;
						}
					}					
					else if(m_VecCrowInfo[_Index].m_CrowID.wMainID == _ID.wMainID && _ID.wSubID == 65535)
					{
						if(m_VecCrowInfo[_Index].m_CrowName == _Name)
						{
							_SameID = m_VecCrowInfo[_Index].m_CrowID;
							return true;
						}
					}
				}

				return false;
			}

		case ITEM_ID:
			{
				unsigned int _Size = m_VecItemInfo.size();
				for(unsigned int _Index = 0; _Index < _Size; ++_Index)
				{
					if(m_VecItemInfo[_Index].m_ItemID == _ID)
					{
						if(m_VecItemInfo[_Index].m_ItemName == _Name)
						{
							_SameID = m_VecItemInfo[_Index].m_ItemID;
							return true;
						}
							
					}
					else if(m_VecItemInfo[_Index].m_ItemID.wMainID == _ID.wMainID && _ID.wSubID == 65535)
					{
						if(m_VecItemInfo[_Index].m_ItemName == _Name)
						{
							_SameID = m_VecItemInfo[_Index].m_ItemID;
							return true;
						}
					}
				}

				return false;
			}

		default:
			return false;
	}

	return false;
}
//검색시 ID에 우선권을 주기 위해서 이름과 ID의 일치를 확인하는 함수, MID로만으로도 검색이 되기때문에 
//MID로 검색해서 일치하는 경우 이름이 아닌 ID로 검색이 이루어져야해서 일치할 경우 일치하는 ID를 인자로 넣어줌
//이 ID로 정확함 검색을 함

/***********************여기까지가 첫 검색어를 통한 데이터 정렬***************************/
void GenItem::GenItemTool::SetFindFromSearchWord(std::vector<GenItemData*> &_Vec, const SearchData* _SearchWord, Condition _Something)
{
	std::vector<GenItemData*> _vecTempData; 		
	char* _StrTemp(NULL);
	char* _Name[20] = { 0 };
	std::string _Origin("");

	switch(_Something)
	{
		case CROW_ID:
			{	
				bool _CheckPushBack(false);
				unsigned int _Size(_Vec.size()), _CrowSize(_SearchWord->m_vecCrowID.size());
				for(unsigned int _SearchNum = 0; _SearchNum < _Size; ++_SearchNum, _CheckPushBack = false)
				{
					for(unsigned int _index = 0; _index < _CrowSize; ++_index)
					{					
						if(_Vec[_SearchNum]->crowID == *_SearchWord->m_vecCrowID[_index])
						{
							_vecTempData.push_back(_Vec[_SearchNum]);	
							_CheckPushBack = true;
							break;
						}
						else if(_Vec[_SearchNum]->crowID.wMainID == _SearchWord->m_vecCrowID[_index]->wMainID && _SearchWord->m_vecCrowID[_index]->wSubID == 65535)
						{
							_vecTempData.push_back(_Vec[_SearchNum]);	
							_CheckPushBack = true;
							break;
						}						
					}

					if(!_CheckPushBack)
					{	
						delete _Vec[_SearchNum];
						_CheckPushBack = false;
					}
				}					
				break;
			}

		case CROW_NAME:
			{			
				_Origin = _SearchWord->m_CrowName;
				_StrTemp = const_cast<char*>(_Origin.c_str());		
				_Name[0] = strtok(_StrTemp, ";");		
				bool _Check(false);

				for(int _index = 1; _Name[_index - 1] != NULL; )
				{
					_Name[_index] = strtok(NULL, ";");	
					++_index;
				}

				unsigned int _Size(_Vec.size());
				for(unsigned int _SearchNum = 0; _SearchNum < _Size; ++_SearchNum)
				{
					for(int _index = 0; _Name[_index] != NULL; ++_index)
					{
						if(!strcmp(_Vec[_SearchNum]->stringCrowName.c_str(), _Name[_index]))
							_Check = true;
					}

					if(_Check)
					{
						_vecTempData.push_back(_Vec[_SearchNum]);
						_Check = false;
					}
					else
					{	delete _Vec[_SearchNum];	}
				}	
				break;
			}

		case GENITEMFILENAME:
			{		
				_Origin = _SearchWord->m_GenItemFileName;
				_StrTemp = const_cast<char*>(_Origin.c_str());		
				_Name[0] = strtok(_StrTemp, ";");		
				bool _Check(false);

				for(int _index = 1; _Name[_index - 1] != NULL; ++_index)
				{
					if(_Name[_index-1] != NULL && sc::string::FindWord(_Name[_index-1], ".genitem") == -1)
						_Name[_index-1] =  strcat(_Name[_index-1], ".genitem");

					_Name[_index] = strtok(NULL, ";");						
				}

				unsigned int _Size(_Vec.size());
				for(unsigned int _SearchNum = 0; _SearchNum < _Size; ++_SearchNum)
				{
					for(int _index = 0; _Name[_index] != NULL; ++_index)
					{		
						if(!strcmp(_Vec[_SearchNum]->stringGenItemFileName.c_str(), _Name[_index]))
							_Check = true;
					}

					if(_Check)
					{
						_vecTempData.push_back(_Vec[_SearchNum]);
						_Check = false;
					}

					else
					{	delete _Vec[_SearchNum];	}
				}	
				break;
			}

		case SPEC_ID:
			{		
				_Origin = _SearchWord->m_SpecID;
				_StrTemp = const_cast<char*>(_Origin.c_str());		
				_Name[0] = strtok(_StrTemp, ";");		
				bool _Check(false);
				int _SpecIDtemp(-1);

				for(int _index = 1; _Name[_index - 1] != NULL; )
				{
					_Name[_index] = strtok(NULL, ";");	
					++_index;
				}

				unsigned int _Size(_Vec.size());
				for(unsigned int _SearchNum = 0; _SearchNum < _Size; ++_SearchNum)
				{
					for(int _index = 0; _Name[_index] != NULL; ++_index)
					{
						_SpecIDtemp = atoi(_Name[_index]);

						if(_SpecIDtemp == _Vec[_SearchNum]->nSpecID)
							_Check = true;
					}
					
					if(_Check)
					{
						_vecTempData.push_back(_Vec[_SearchNum]);
						_Check = false;
					}
					else
					{	delete _Vec[_SearchNum];	}					
				}	
				break;
			}

		case ITEM_ID:
			{		
				bool _CheckPushBack(false);
				unsigned int _Size(_Vec.size()), _ItemSize(_SearchWord->m_vecItemID.size());
				for(unsigned int _SearchNum = 0; _SearchNum < _Size; ++_SearchNum, _CheckPushBack = false)
				{
					
					for(unsigned int _index = 0; _index < _ItemSize; ++_index)
					{					
						if(_Vec[_SearchNum]->itemID == *_SearchWord->m_vecItemID[_index])
						{
							_vecTempData.push_back(_Vec[_SearchNum]);
							_CheckPushBack = true;
							break;
						}
						else if(_Vec[_SearchNum]->itemID.wMainID == _SearchWord->m_vecItemID[_index]->wMainID && _SearchWord->m_vecItemID[_index]->wSubID == 65535)
						{
							_vecTempData.push_back(_Vec[_SearchNum]);
							_CheckPushBack = true;
							break;
						}						
					}

					if(!_CheckPushBack)
					{
						delete _Vec[_SearchNum];	
						_CheckPushBack = false;
					}
				}	
				break;
			}
		case ITEM_NAME:
			{	
				_Origin = _SearchWord->m_ItemName;
				_StrTemp = const_cast<char*>(_Origin.c_str());		
				_Name[0] = strtok(_StrTemp, ";");		
				bool _Check(false);
				int _SpecIDtemp(-1);

				for(int _index = 1; _Name[_index - 1] != NULL; )
				{
					_Name[_index] = strtok(NULL, ";");	
					++_index;
				}

				unsigned int _Size(_Vec.size());
				for(unsigned int _SearchNum = 0; _SearchNum < _Size; ++_SearchNum)
				{
					for(int _index = 0; _Name[_index] != NULL; ++_index)
					{
						if(!strcmp(_Vec[_SearchNum]->stringItemName.c_str(), _Name[_index]))
							_Check = true;
					}
 
					if(_Check)
					{
						_vecTempData.push_back(_Vec[_SearchNum]);
						_Check = false;
					}
					else
					{	delete _Vec[_SearchNum];	}
				}	
				break;
			}

		default:
			{	return;	}
	}

	_Vec.clear();

	unsigned int _ResultSize(_vecTempData.size());
	for(unsigned int _PushNum = 0; _PushNum < _ResultSize; ++_PushNum)
	{
		_Vec.push_back(_vecTempData[_PushNum]);
	}

	_vecTempData.clear();
}
//첫 검색어를 통한 데이터 세팅이 끝난 데이터들의 안에서 나머지 검색어들의 일치를 찾음

void GenItem::GenItemTool::CheckGenItemUse()
{
	unsigned int _NumGenItem = GenItem::Table::getInstance().GetVecGenItemNameSize();
	std::vector<CheckLinkGenItem> _tempLinkData;

	for(unsigned int _Number = 0; _Number < _NumGenItem; ++_Number)
	{
		CheckLinkGenItem _tempData;
		_tempData.m_index = _Number;
		_tempLinkData.push_back(_tempData);
	}

	unsigned int _NumCrow = m_VecCrowInfo.size();
	for(unsigned int _CrowIndex = 0; _CrowIndex < _NumCrow; ++_CrowIndex )
	{
		unsigned int _NumCrowGenItem = m_VecCrowInfo[_CrowIndex].m_GenItemFileNameindex.size();
		for(unsigned int _GenItemIndex = 0; _GenItemIndex < _NumCrowGenItem; ++_GenItemIndex)
		{
			for(unsigned int _Num = 0; _Num < _NumGenItem; ++_Num)
			{
				if(_tempLinkData[_Num].m_index == m_VecCrowInfo[_CrowIndex].m_GenItemFileNameindex[_GenItemIndex])
				{
					_tempLinkData[_Num].m_Use = true; 					
					break;
				}
			}
		}
	}

	unsigned int _LinkDataNum = _tempLinkData.size();
	for(unsigned int _NumLinkData = 0; _NumLinkData < _LinkDataNum; ++_NumLinkData)
	{
		if(_tempLinkData[_NumLinkData].m_Use == false)
		{
			m_LinkLostGenitem.push_back(_tempLinkData[_NumLinkData]);
		}
	}
}
//GenItemfile을 Crow데이터에서 사용중인지 아닌지를 판별하는데 쓰는 함수

void GenItem::GenItemTool::CheckLinkItem(std::vector<LinkLostData*> &_LinkLostData)
{
	unsigned int _ItemNum = m_VecItemInfo.size();
	for(unsigned int _NumItem = 0; _NumItem < _ItemNum; ++_NumItem)
	{
		if(m_VecItemInfo[_NumItem].m_SpecID == -1)
		{
			LinkLostData* _tempData = new LinkLostData;
			_tempData->m_Type = LinkLostData::ITEM_DATA;
			_tempData->m_StateMessage = "SPECID doesn't exist";
			_tempData->m_ItemID = m_VecItemInfo[_NumItem].m_ItemID;
			_tempData->m_ItemName = m_VecItemInfo[_NumItem].m_ItemName;

			_LinkLostData.push_back(_tempData);
		}
	}
}
//SpecID값이 들어있지 않은 아이템들의 데이터를 찾을때 쓰는 함수

void GenItem::GenItemTool::CheckLinkSpecID(std::vector<LinkLostData*> &_LinkLostData)
{
	SpecIDInfoMapIter _begin = m_MapSpecID.begin();
	SpecIDInfoMapIter _end = m_MapSpecID.end();

	std::vector<int> tempobj;
	tempobj.swap(m_NoUseSpecID);
	tempobj.clear();

	for(; _begin != _end; ++_begin)
	{
		SpecIDInfo* _SpecIDInfo = _begin->second;

		if(_SpecIDInfo->m_GenItemNameKey.size() == 0)
		{
			for(unsigned int _NumItem = 0; _NumItem < _SpecIDInfo->m_ItemIndex.size(); ++_NumItem)
			{
				LinkLostData* _tempData = new LinkLostData;
				_tempData->m_Type = LinkLostData::SPECID_DATA;
				_tempData->m_StateMessage = "It is not included to GenItemFile";
				_tempData->m_SpecID = m_VecItemInfo[_SpecIDInfo->m_ItemIndex[_NumItem]].m_SpecID;
				_tempData->m_ItemID = m_VecItemInfo[_SpecIDInfo->m_ItemIndex[_NumItem]].m_ItemID;
				_tempData->m_ItemName = m_VecItemInfo[_SpecIDInfo->m_ItemIndex[_NumItem]].m_ItemName;

				_LinkLostData.push_back(_tempData);
			}
		}
	
		if(_SpecIDInfo->m_ItemIndex.size() == 0)
		{
			int SpecID = _begin->first;
			if(SpecID != 0)
				m_NoUseSpecID.push_back(SpecID);
		}
	}
}
//파일로 저장되지는 않으나 개념상으로는 분명 존재하는 SpecID라서 데이터 구조가 하나 더 생김
//GenItemFile들을 모두 검사해서 파일에 포함되어 있지 않은 SpecID를 찾을 때 사용
//NoUseSpecID체크를 여기서 진행함, 추가로 함수를 따로 구성할 시 for문을 다시 구동시켜야해서
//되도록 속도를 높이려는 목적으로 합쳐 놓음

void GenItem::GenItemTool::CheckLinkGenitem(std::vector<LinkLostData*> &_LinkLostData)
{
	unsigned int _GenNum = m_LinkLostGenitem.size();
	for(unsigned int _NumGen = 0; _NumGen < _GenNum; ++_NumGen)
	{
		LinkLostData* _tempData = new LinkLostData;
		_tempData->m_Type = LinkLostData::GENITEMFILE_DATA;
		_tempData->m_StateMessage = "GenItemFiles are not used for now";
		_tempData->m_GenItemFileName = GenItem::Table::getInstance().GetVecGenItemName(m_LinkLostGenitem[_NumGen].m_index);	

		_LinkLostData.push_back(_tempData);
	}

	unsigned int _GenSpecNum = GenItem::Table::getInstance().GetVecGenItemNameSize();
	for(unsigned int _NumSpecGen = 0; _NumSpecGen < _GenSpecNum; ++_NumSpecGen)
	{
		GenItemInfoMapIter _GenInfoIter = m_MapGenItemInfo.find(_NumSpecGen);
		GenItemInfo* _GenInfotemp = _GenInfoIter->second;

		if(_GenInfotemp->m_SpIDnRate.size() == 0)
		{
			LinkLostData* _tempData = new LinkLostData;
			_tempData->m_Type = LinkLostData::GENITEMFILE_DATA;
			_tempData->m_StateMessage = "There is no SpecID within GenitemFile";
			_tempData->m_GenItemFileName = GenItem::Table::getInstance().GetVecGenItemName(_NumSpecGen);

			_LinkLostData.push_back(_tempData);
		}
	}
}
//GenItemFile을 사용여부를 체크하는 함수를 실행하면 사용하지 않는 것들의 검출이 되고 그것들을 메시지와 함께 담음
//그리고 File에서 SpecID가 하나도 포함 되어 있지 않은 것들을 찾음(총 2종류의 LinkLost를 찾음)

void GenItem::GenItemTool::CheckLinkCrow(std::vector<LinkLostData*> &_LinkLostData)
{
	unsigned int _CrowNum = m_VecCrowInfo.size();
	for(unsigned int _NumCrow = 0; _NumCrow < _CrowNum; ++_NumCrow)
	{
		if(m_VecCrowInfo[_NumCrow].m_GenItemFileNameindex.size() == 0)
		{
			LinkLostData* _tempData = new LinkLostData;
			_tempData->m_Type = LinkLostData::CROW_DATA;
			_tempData->m_StateMessage = "This Crow doesn't have GenitemFile";
			_tempData->m_CrowID = m_VecCrowInfo[_NumCrow].m_CrowID;
			_tempData->m_CrowName = m_VecCrowInfo[_NumCrow].m_CrowName;

			_LinkLostData.push_back(_tempData);
		}
	}
}
//Crow에서 GenItemFile을 가지고 있지 않은 것들을 찾음

template<typename T>
void GenItem::GenItemTool::DeleteVec(std::vector<T*> &_Vec)
{
	unsigned int _Size = _Vec.size();
	for(unsigned int _Num = 0; _Num < _Size; +_Num)
		delete _Vec[_Num];
	std::vector<T*> tempobj;
	tempobj.swap(_Vec);	
}

void GenItem::GenItemTool::SetFindFromPartWord(std::vector<std::string> &_Vec, const std::string &_strSource, const GenItem::FindFromWordCase _Case)
{	
	std::string _Origin;
	char* _strtemp;
	std::string _tempSource;
	std::string _tempTarget;

	switch(_Case)
	{
		case GenItem::CROW_NAME_WORD:
				{
					unsigned int _Size = m_VecCrowName.size();
					for(unsigned int _index = 0; _index < _Size; ++_index)
					{	
						_Origin = _strSource;
						_strtemp = const_cast<char*>(_Origin.c_str());
						_tempSource = strlwr(_strtemp);
						_Origin = m_VecCrowName[_index];
						_strtemp = const_cast<char*>(_Origin.c_str());
						_tempTarget = strlwr(_strtemp);
						if(sc::string::FindWord(_tempTarget, _tempSource) != -1)
						{
							_Vec.push_back(m_VecCrowName[_index]);
						}
					}
					break;
				}
		case GenItem::ITEM_NAME_WORD:
			{
				unsigned int _Size = m_VecItemName.size();
				for(unsigned int _index = 0; _index < _Size; ++_index)
				{
					_Origin = _strSource;
					_strtemp = const_cast<char*>(_Origin.c_str());
					_tempSource = strlwr(_strtemp);
					_Origin = m_VecItemName[_index];
					_strtemp = const_cast<char*>(_Origin.c_str());
					_tempTarget = strlwr(_strtemp);						
					if(sc::string::FindWord(_tempTarget, _tempSource) != -1)
					{
						_Vec.push_back(m_VecItemName[_index]);
					}
				}
				break;
			}
		case GenItem::GENITEMFILE_NAME_WORD:
			{
				unsigned int _Size = GenItem::Table::getInstance().GetVecGenItemNameSize();
				for(unsigned int _index = 0; _index < _Size ; ++_index)
				{
					_Origin = _strSource;
					_strtemp = const_cast<char*>(_Origin.c_str());
					_tempSource = strlwr(_strtemp);
					_Origin = GenItem::Table::getInstance().GetVecGenItemName(_index);
					_strtemp = const_cast<char*>(_Origin.c_str());
					_tempTarget = strlwr(_strtemp);		
					if(sc::string::FindWord(_tempTarget, _tempSource) != -1)
					{
						_Vec.push_back(GenItem::Table::getInstance().GetVecGenItemName(_index));
					}
				}
				break;
			}
	}
}
//부분문자열 검색함수, 이름을 검색하는데 이름의 종류가 3개라서 3가지의 케이스
//네이버 검색창 처럼 중간단어가 일치해도 검색함

void GenItem::GenItemTool::LoadSelectFile()
{
	CFileDialog dlg(TRUE, ".genitem", NULL, OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FILEMUSTEXIST, 
		"genItem file (*.genitem) | *.genitem");
	char buffer[4096] = {0};
	dlg.m_ofn.lpstrFile = buffer;
	dlg.m_ofn.nMaxFile = 4096;

	CString _FileName;
	CGENITEM _genItem;
	SGENINFO _genItemInfo;

	if(dlg.DoModal() == IDOK)
	{
		POSITION _pos(dlg.GetStartPosition());
		for(; _pos != NULL;)
		{
			_FileName = dlg.GetNextPathName(_pos);
			_FileName.Delete(0, _FileName.ReverseFind('\\')+1);			
			std::string _Name = _FileName;
			if(GLOGIC::UseLogicZipFile())
				GLOGIC::SetGlogicZipFile(FALSE);

			_genItem.GetGenInfo("", _Name, _genItemInfo);
			unsigned int _Key = FindIndexFromGenFileName(_Name);

			if(_Key == -1 || _Key > GenItem::Table::getInstance().GetVecGenItemNameSize()) 
			{
				MessageBox(NULL, "Couldn't Find File [Fail]", "DataLoad", MB_OK);
				return;
			}

			GenItem::GenItemMapIterator _tempIter = GenItem::Table::getInstance().GetGenItemMapIter(_Name); //가져온 파일이름으로 맵에서 데이터 가져옴		
			SGENINFO &_tempInfo = _tempIter->second;	

			std::vector<SGENNUM> _vectempNum;
			std::vector<std::tr1::shared_ptr<SGENITEMBASE>> _vectempBase;

			_tempInfo.dwVersion = _genItemInfo.dwVersion;
			_tempInfo.emType = _genItemInfo.emType;
			_tempInfo.fProbability = _genItemInfo.fProbability;

			_vectempNum.swap(_tempInfo.vecGenNum);
			unsigned int _Size = _genItemInfo.vecGenNum.size();
			for(unsigned int _index(0); _index < _Size; ++_index)
			{
				_tempInfo.PushGenNum(_genItemInfo.vecGenNum[_index]);
			}

			_vectempBase.swap(_tempInfo.vecGenItem);
			_Size = _genItemInfo.vecGenItem.size();
			for(unsigned int _index(0); _index<_Size; ++_index)
			{
				_tempInfo.PushGenItem(_genItemInfo.vecGenItem[_index]);
			}

			GenItemInfoMapIter _Iter = m_MapGenItemInfo.find(_Key);
			GenItemInfo* _GenItemInfo = _Iter->second;
			_GenItemInfo->m_GenRate = _genItemInfo.fProbability/100;	
			_GenItemInfo->m_GenNum = _genItemInfo.vecGenItem.size();
			int _tempMaxNum = _GenItemInfo->m_GenNum;

			if(_GenItemInfo->m_SpIDnRate.size() != 0)
				_GenItemInfo->m_SpIDnRate.clear();

			for(int i = 0; i < _tempMaxNum; ++i)
			{
				GenItemInfo::SpIDnRate _SpIDnRate;
				switch(_genItemInfo.vecGenItem[i]->emType)
				{
					case EMGENITEMTYPE_ITEM:
						{						
							std::tr1::shared_ptr<SGENITEMSPEC> pTempGen = std::tr1::static_pointer_cast<SGENITEMSPEC>(_genItemInfo.vecGenItem[i]);
							_SpIDnRate.Clear();

							if(pTempGen->dwSpecID != 0 && pTempGen->dwSpecID <= 600)
								_SpIDnRate.m_SpecID = pTempGen->dwSpecID;

							_SpIDnRate.m_Rate = pTempGen->fRate/100;
							_SpIDnRate.m_GenType = GenItemInfo::GEN_ITEM;	
							_GenItemInfo->m_SpIDnRate.push_back(_SpIDnRate);	
							break;
						}
					case EMGENITEMTYPE_ITEMSKILLFACT:
						{
							std::tr1::shared_ptr<SGENITEMSPECSKILLFACT> pTempGen = std::tr1::static_pointer_cast<SGENITEMSPECSKILLFACT>(_genItemInfo.vecGenItem[i]);
							_SpIDnRate.Clear();

							if(pTempGen->dwSpecID != 0 &&  pTempGen->dwSpecID <= 600)
								_SpIDnRate.m_SpecID = pTempGen->dwSpecID;

							_SpIDnRate.m_Rate = pTempGen->fRate/100;
							_SpIDnRate.m_SkillID.wMainID = pTempGen->sSkillID.wMainID;
							_SpIDnRate.m_SkillID.wSubID = pTempGen->sSkillID.wSubID;
							_SpIDnRate.m_GenType = GenItemInfo::GEN_ITEM_WITH_SKILL;
							_GenItemInfo->m_SpIDnRate.push_back(_SpIDnRate);		
							break;	
						}
					case EMGENITEMTYPE_MONEY:
						{
							std::tr1::shared_ptr<SGENMONEY> pTempGen = std::tr1::static_pointer_cast<SGENMONEY>(_genItemInfo.vecGenItem[i]);
							_SpIDnRate.Clear();

							if(pTempGen->dwMoney != 0 &&  pTempGen->dwMoney <= 600)
								_SpIDnRate.m_SpecID = pTempGen->dwMoney;

							_SpIDnRate.m_Rate = pTempGen->fRate/100;
							_SpIDnRate.m_GenType = GenItemInfo::GEN_MONEY;		
							_GenItemInfo->m_SpIDnRate.push_back(_SpIDnRate);		
							break;
						}
					case EMGENITEMTYPE_QITEM:
						{
							std::tr1::shared_ptr<SGENQITEMID> pTempGen = std::tr1::static_pointer_cast<SGENQITEMID>(_genItemInfo.vecGenItem[i]);
							_SpIDnRate.Clear();
							_SpIDnRate.m_SkillID = pTempGen->sQItemID; //Q아이템 너무 예외적인 존재, 임시로 넣음
							_SpIDnRate.m_Rate = pTempGen->fRate/100;
							_SpIDnRate.m_GenType = GenItemInfo::GEN_ITEM_QBOX;
							_GenItemInfo->m_SpIDnRate.push_back(_SpIDnRate);		
							break;
						}
				}
			}
		}

		MessageBox(NULL, "Data Load OK [Success]", "DataLoad", MB_OK);
	}
}
//검색툴 이용도중 파일을 수정햇을 경우 rcc내부의 파일들은 수정 할 수 없기때문에 rcc외부에 새로 추가될것
//때문에 이때 데이터 갱신을 위해서 추가한 함수
//다만 새롭게 추가된 파일에 대해서는 갱신이 일어나지 않음, 파일 이름이 일치하는 경우에 한해서 데이터를 교체해줌

