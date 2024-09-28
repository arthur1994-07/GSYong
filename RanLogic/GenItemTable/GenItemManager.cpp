#include "pch.h"
#include "GenItemManager.h"

#include "./GenItemTable.h"
#include "./GenItemToolUtill.h"
#include "../SigmaCore//String/StringUtil.h"
#include "../GLogicData.h"
#include "../enginelib/G-Logic/GLogic.h"
#include "../GenItemTool/ExcelExportGenItem.h"


GenItem::GenItemManager::GenItemManager(void)
{	
	m_InstanceGenItemTool = new GenItemTool;
}

GenItem::GenItemManager::~GenItemManager(void)
{
	DeleteVecGenItemData();
	delete m_InstanceGenItemTool;
}

void GenItem::GenItemManager::DeleteVecGenItemData()
{
	unsigned int _Size(m_vecGenItemData.size());
	
	for(unsigned int _index = 0; _index < _Size; ++_index)
	{
		if(m_vecGenItemData[_index] != NULL)
		delete m_vecGenItemData[_index];
	}
	
	std::vector<GenItem::GenItemData*> temobj;
	temobj.swap(m_vecGenItemData);
}

void GenItem::GenItemManager::SeperateStringID(std::vector<SNATIVEID*> &_Vec, const std::string &_StrMID, const std::string &_StrSID, const std::string _StrSource)
{
	std::vector<SNATIVEID*> _vecTempID;
	std::vector<int> _vecMid;
	std::vector<int> _vecSid;
	char* _StrMIDOrigin = new char[100];
	memcpy(_StrMIDOrigin, _StrMID.c_str(), sizeof(_StrMID.c_str()));	
	char* _StrMIDToken(NULL);
	_StrMIDToken = strtok(_StrMIDOrigin, _StrSource.c_str());
	for(; _StrMIDToken != NULL; _StrMIDToken = strtok(NULL, _StrSource.c_str()))
	{
		int _Mid = atoi(_StrMIDToken);
		_vecMid.push_back(_Mid);		
	}
	
	char* _StrSIDOrigin= new char[100];	
	memcpy(_StrSIDOrigin, _StrSID.c_str(), sizeof(_StrSID.c_str()));
	char* _StrSIDToken("");
	_StrSIDToken = strtok(_StrSIDOrigin, _StrSource.c_str());
	for(; _StrSIDToken != NULL; _StrSIDToken = strtok(NULL, _StrSource.c_str()))
	{
		int _Sid = atoi(_StrSIDToken);
		_vecSid.push_back(_Sid);
	}

	unsigned int _Msize = _vecMid.size();
	unsigned int _Ssize = _vecSid.size();
	for(unsigned int _Mindex(0); _Mindex < _Msize; ++_Mindex)
	{
		if(_Ssize != 0)
		{		
			for(unsigned int _Sindex(0); _Sindex < _Ssize; ++_Sindex)
			{
				SNATIVEID* _ID = new SNATIVEID;
				_ID->wMainID = _vecMid[_Mindex];
				_ID->wSubID = _vecSid[_Sindex];
				_vecTempID.push_back(_ID);
			}
		}
		else
		{
			SNATIVEID* _ID = new SNATIVEID;
			_ID->wMainID = _vecMid[_Mindex];
			_ID->wSubID = 65535;
			_vecTempID.push_back(_ID);
		}
	}

	unsigned int _Size = _vecTempID.size();	
	if(_Size == 0)
	{
		SNATIVEID* _tempID = new SNATIVEID;	
		_tempID->wMainID = 65535;
		_tempID->wSubID = 65535;
		_vecTempID.push_back(_tempID);
	}

	_vecTempID.swap(_Vec);
	unsigned int _tempSize = _vecTempID.size();

	if(_tempSize != 0)
	{
		for(unsigned int _index = 0; _index < _tempSize; ++_index)
		{
			delete _vecTempID[_index];
		}
	}

	delete _StrSIDOrigin;
	delete _StrMIDOrigin;	
}
//���� �˻��� ���ؼ� ID���� ���ڷ� ó����, ���� ���ڸ� �ٽ� ���ڷ� ��ȯ����� �ϱ⶧����
//ID�� ���ڷ� �ٲ��ִ� �Լ��� ����, ���ڰ� ����ִٸ� ID�� �ϳ� ���� ��������
//- (NULL = "") ���� ��- 

bool GenItem::GenItemManager::FindDataCondition(const SearchData& _SearchData)
{
	if(!FindCrow(_SearchData)) 
	{
		if(_SearchData.m_CrowID_MID != "" || _SearchData.m_CrowName != "")
			return false;
	}
	if(!FindGenItemFile(_SearchData))
	{
		if(_SearchData.m_GenItemFileName != "")
			return false;
	}
	if(!FindSpecID(_SearchData))
	{
		if(_SearchData.m_SpecID != "")
			return false;
	}
	if(!FindItem(_SearchData)) 
	{
		if(_SearchData.m_ItemID_MID != "" || _SearchData.m_ItemName != "")
			return false;
	}
	//���������� �Լ� ������ ��(�˻��� �켱 ������� ������ ��ġ�Ǿ� ����)
	return true;
}
//�˻����� ���º� ������ �˻�
//Case�� ������ �ƴ϶� �켱������ �����ؼ� ������ �� �����شٸ� ���ϴ� ��� �˻��� ���� ����

bool GenItem::GenItemManager::FindCrow(const SearchData& _SearchData)
{		
	if(_SearchData.m_CrowID_MID != "" || _SearchData.m_CrowID_SID != "" || _SearchData.m_CrowName != "")
	{
		if(m_vecGenItemData.size() == 0)
		{
			unsigned int _Size = _SearchData.m_vecCrowID.size();
			for(unsigned int _index = 0; _index < _Size; ++_index)
			{
				std::string _CrowOrigin = _SearchData.m_CrowName;
				char* _StrTemp = const_cast<char*>(_CrowOrigin.c_str());
				char* _CrowName(NULL);			
				_CrowName = strtok(_StrTemp, ";");			

				if(_SearchData.m_vecCrowID[_index]->wMainID != 65535 && _SearchData.m_vecCrowID[_index]->wSubID != 65535 && _CrowName == NULL)
					m_InstanceGenItemTool->SetDataStartCrow_NativeID(&m_vecGenItemData, *_SearchData.m_vecCrowID[_index]);
				else if(_SearchData.m_vecCrowID[_index]->wMainID != 65535 && _SearchData.m_vecCrowID[_index]->wSubID == 65535 && _CrowName == NULL)
					m_InstanceGenItemTool->SetDataStrartCrow_MID(&m_vecGenItemData, *_SearchData.m_vecCrowID[_index]);

				for(; _CrowName != NULL; _CrowName = strtok(NULL, ";"))
				{		
					_CrowOrigin = _CrowName; //���� ��Ȱ��
					if(_SearchData.m_vecCrowID[_index]->wMainID != 65535)
					{	
						SNATIVEID _CrowID; //�ӽþ��̵�

						if(!m_InstanceGenItemTool->JudgeSame(*_SearchData.m_vecCrowID[_index], _CrowOrigin, _CrowID, GenItemTool::CROW_ID))
							continue;		
						else
							m_InstanceGenItemTool->SetDataStartCrow_NativeID(&m_vecGenItemData, _CrowID);
					}
					else
						m_InstanceGenItemTool->SetDataStartCrow_Name(&m_vecGenItemData, _CrowOrigin);								
				}
			}
		}

		else
		{
			unsigned int _Size = _SearchData.m_vecCrowID.size();
			for(unsigned int _index = 0; _index < _Size; ++_index)
			{
				if(_SearchData.m_vecCrowID[_index]->wMainID != 65535 || _SearchData.m_vecCrowID[_index]->wSubID != 65535)
					m_InstanceGenItemTool->SetFindFromSearchWord(m_vecGenItemData, &_SearchData, GenItemTool::CROW_ID);
				else
					m_InstanceGenItemTool->SetFindFromSearchWord(m_vecGenItemData, &_SearchData, GenItemTool::CROW_NAME);				
			}
		}
	}

	if(m_vecGenItemData.size() == 0)
		return false;
	else
		return true;
}
//�̸��� ���� �˻��� �����ѵ� �̶� �̸��� ';'���ڸ� �������� �и��˻��ϴ� ���� ���⿡ �������
//�˻� ����� ��� �����̳ʰ� ����ִٸ� ������ ������ ���� �̷�� ����, 
//�����̳ʰ� �ִٸ� ���ο��� ��ġ�ϴ� ���θ� �˻�������
//����� Crow�� �ֿ켱 �˻������� ������ ������ �ƴ� ��ġ �˻�� �̷������ ������
//�˻� ���� ������ �Ͼ ��츦 ����ؼ� ��ġ �˻絵 �߰��� ����
//�˻�� ��� ����ü���� ID�����̳ʰ� ��� �ִٸ� �˻��� ������� ����(���ǿ��) default: 65535/65535

bool GenItem::GenItemManager::FindGenItemFile(const SearchData& _SearchData)
{	
	if(_SearchData.m_GenItemFileName != "")
	{
		if(m_vecGenItemData.size() == 0)
		{
			std::string _GenItemOrigin = _SearchData.m_GenItemFileName;
			char* _StrTemp = const_cast<char*>(_GenItemOrigin.c_str());
			char* _GenItemName(NULL);
			_GenItemName = strtok(_StrTemp, ";");	
		
			for(;_GenItemName != NULL; _GenItemName = strtok(NULL, ";"))
			{	
				m_InstanceGenItemTool->SetDataStartGenItem(&m_vecGenItemData, _GenItemName);									
			}
		}
		else
			m_InstanceGenItemTool->SetFindFromSearchWord(m_vecGenItemData, &_SearchData, GenItemTool::GENITEMFILENAME);
	}

	if(m_vecGenItemData.size() == 0)
		return false;
	else
		return true;
}

//���� �̸� ���߰˻��� �����ϱ� ������ ���ڸ� ������ �����̳� ���º� ������ ���� or ��ġ�˻� ����

bool GenItem::GenItemManager::FindSpecID(const SearchData& _SearchData)
{
	if(_SearchData.m_SpecID != "")
	{
		if(m_vecGenItemData.size() == 0)
		{
			std::string _SpecOrigin = _SearchData.m_SpecID;
			char* _StrTemp = const_cast<char*>(_SpecOrigin.c_str());
			char* _SpecIDstr(NULL);
			_SpecIDstr = strtok(_StrTemp, ";");				

			for(;_SpecIDstr != NULL; _SpecIDstr = strtok(NULL, ";"))
			{					
				int _SpecID = atoi(_SpecIDstr);
				if(_SpecID > GLItemMan::SPECID_NUM) continue;
				m_InstanceGenItemTool->SetDataStartSpecID(&m_vecGenItemData, _SpecID);					
			}
		}
		else		
			m_InstanceGenItemTool->SetFindFromSearchWord(m_vecGenItemData, &_SearchData, GenItemTool::SPEC_ID);		
	}

	if(m_vecGenItemData.size() == 0)
		return false;
	else
		return true;
}

//GenItemFile �˻� �Լ��� �뵿������

bool GenItem::GenItemManager::FindItem(const SearchData& _SearchData)
{
	if(_SearchData.m_ItemID_MID != "" || _SearchData.m_ItemID_SID != "" || _SearchData.m_ItemName != "")
	{
		if(m_vecGenItemData.size() == 0)
		{
			unsigned int _Size = _SearchData.m_vecItemID.size();
			for(unsigned int _index = 0; _index < _Size; ++_index)
			{
				std::string _ItemNameOrigin = _SearchData.m_ItemName;
				char* _StrTemp = const_cast<char*>(_ItemNameOrigin.c_str());
				char* _ItemName(NULL);
				_ItemName = strtok(_StrTemp, ";");				

				if(_SearchData.m_vecItemID[_index]->wMainID != 65535 && _SearchData.m_vecItemID[_index]->wSubID != 65535 && _ItemName == NULL)
					m_InstanceGenItemTool->SetDataStartItem_NativeID(&m_vecGenItemData, *_SearchData.m_vecItemID[_index]);
				else if(_SearchData.m_vecItemID[_index]->wMainID != 65535 && _SearchData.m_vecItemID[_index]->wSubID == 65535 && _ItemName == NULL)
					m_InstanceGenItemTool->SetDataStartItem_MID(&m_vecGenItemData, *_SearchData.m_vecItemID[_index]);

				for(;_ItemName != NULL; _ItemName = strtok(NULL, ";"))
				{
					_ItemNameOrigin = _ItemName; //���� ��Ȱ��
					if(_SearchData.m_vecItemID[_index]->wMainID != 65535 && _SearchData.m_vecItemID[_index]->wSubID != 65535)
					{	
						SNATIVEID _ItemID; //�ӽþ��̵�

						if(!m_InstanceGenItemTool->JudgeSame(*_SearchData.m_vecItemID[_index], _ItemNameOrigin, _ItemID, GenItemTool::ITEM_ID))
							continue;
						else
							m_InstanceGenItemTool->SetDataStartItem_NativeID(&m_vecGenItemData, _ItemID);
					}
					else
						m_InstanceGenItemTool->SetDataStartItem_Name(&m_vecGenItemData, _ItemNameOrigin);
				}
			}
		}
		else
		{
			unsigned int _Size = _SearchData.m_vecItemID.size();
			for(unsigned int _index = 0; _index < _Size; ++_index)
			{
				if(_SearchData.m_vecItemID[_index]->wMainID != 65535 || _SearchData.m_vecItemID[_index]->wSubID != 65535)
					m_InstanceGenItemTool->SetFindFromSearchWord(m_vecGenItemData, &_SearchData, GenItemTool::ITEM_ID);
				else
					m_InstanceGenItemTool->SetFindFromSearchWord(m_vecGenItemData, &_SearchData, GenItemTool::ITEM_NAME);
			}
		}
	}	

	if(m_vecGenItemData.size() == 0)
		return false;
	else
		return true;
}

//Crow�˻� �Լ��� ���� ����

bool GenItem::GenItemManager::FindData(const SearchData& _SearchData)
{
	DeleteVecGenItemData();
	if(!FindDataCondition(_SearchData))
		return false;
	else
		return true;
}

void GenItem::GenItemManager::LoadData(LoadCondition _Condition)
{
	m_InstanceGenItemTool->SetGenItemToolInfo();
}

void GenItem::GenItemManager::FindLinkLostData(std::vector<LinkLostData*> &_LinkLostData)
{
	unsigned int _Size = _LinkLostData.size();
	for(unsigned int _index = 0; _index < _Size; ++_index)
	{		
		if(_LinkLostData[_index] != NULL)
			delete _LinkLostData[_index];		
	}	

	if(_LinkLostData.size())
	{
		std::vector<LinkLostData*> tempobj;
		tempobj.swap(_LinkLostData);
	}

	m_InstanceGenItemTool->CheckLinkItem(_LinkLostData);
	m_InstanceGenItemTool->CheckLinkSpecID(_LinkLostData);
	m_InstanceGenItemTool->CheckGenItemUse();
	m_InstanceGenItemTool->CheckLinkGenitem(_LinkLostData);
	m_InstanceGenItemTool->CheckLinkCrow(_LinkLostData);
	//������ �߿�, ������ ���� Count�ϰ� Count�� �� ��¿� �̿���
}

//GenItemToolUtill���� ���ǵ� �Լ����� �̿��� ������� �ʰ� �ִ� ������(GenItemFile, SpecID ��)�� ã�Ƴ�

void GenItem::GenItemManager::FindDatafromWord(std::vector<std::string> &_Vec, const std::string &_Source, GenItem::FindFromWordCase _Case)
{
	m_InstanceGenItemTool->SetFindFromPartWord(_Vec, _Source, _Case);
}

HRESULT GenItem::GenItemManager::SaveExcelFile(CWnd* _pWnd)
{	
	ExcelExportGenItem::GenInstance().ExcelSaveGenItemTool(_pWnd, m_vecGenItemData);
	return S_OK;	
}

HRESULT GenItem::GenItemManager::SaveExcelFile(CWnd* _pWnd, std::vector<LinkLostData*> &_LinkLostData, std::vector<int> &_NoUseSpecID)
{
	ExcelExportGenItem::GenInstance().ExcelSaveGenItemTool(_pWnd, _LinkLostData, _NoUseSpecID);
	return S_OK;
}
//�������� ���� �Լ�

void GenItem::GenItemManager::LoadSelectFile()
{
	m_InstanceGenItemTool->LoadSelectFile();
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void GenItem::GenItemManager::DeleverSearchData(std::vector<GenItem::GenItemData*> &_vecGenItemData)
{
	std::vector<GenItem::GenItemData*> tempobj;
	tempobj.swap(_vecGenItemData); //���� �����͵��� �ּҰ��� ������ ����, delete ���� �ʾƾߵ�

	unsigned int _Size = m_vecGenItemData.size();
	for(unsigned int _index = 0; _index < _Size; ++_index)
	{
		if(m_vecGenItemData[_index] != NULL)
			_vecGenItemData.push_back(m_vecGenItemData[_index]);
	}	
}
//�˻��� ���� �����͵��� �����̳ʿ� ���� �� �����̳��� ������ Dialog�� �����ϱ� ���� �Լ�
//�����̳ʿ��� �ּҰ��鸸 ����־ �ּҰ��� �Ѱ��ִ� ����
//Dialog�ʿ����� �Ѱܹ��� �ּҰ��� ���ؼ� ���� �� ��¸���
//�����Ŀ��� ����ؼ� Manager�ʿ����� �����͵��� ��� �ְ� ���ο� �˻� Ȥ�� ���� ��ư�� ������
//�Ҵ�� �����͵��� �Ҵ� ������

void GenItem::GenItemManager::DeleverNoUseSpecID(std::vector<int> &_vecNoUseSpecID)
{
	unsigned int _Size = (unsigned int)m_InstanceGenItemTool->GetMaxNumofNoUseSpecID();
	for(unsigned int _index(0); _index < _Size; ++_index)
	{
		_vecNoUseSpecID.push_back(m_InstanceGenItemTool->GetNoUseSpecID(_index));
	}
}
//NoUseSpecID�� ������
//LinkLost�����͵���� �ٸ� ������ �����̳ʸ� ���� �޽����� �ʿ��� �ܵ����� ���� ������

void GenItem::GenItemManager::FindGenItemFileData(std::string &_FileName, GenItemEditData &_GenItemEdit)
{
	_GenItemEdit.erase();

	GenItem::GenItemMapIterator _tempIter = GenItem::Table::getInstance().GetGenItemMapIter(_FileName); //������ �����̸����� �ʿ��� ������ ������		
	SGENINFO &_tempInfo = _tempIter->second;	

	_GenItemEdit.m_FileGenRate.m_Rate = _tempInfo.fProbability;
	_GenItemEdit.m_FileGenRate.m_Type = GenItemEditData::FILEGENRATE;
	_GenItemEdit.m_FileGenRate.m_StrName = "GenRate";

	unsigned int _Size = _tempInfo.vecGenNum.size();
	for(unsigned int _index = 0; _index < _Size; ++_index)
	{
		GenItemEditData::NumRate* _NumRate = new GenItemEditData::NumRate;
		_NumRate->m_Num = _tempInfo.vecGenNum[_index].wNum;
		_NumRate->m_NumRate = _tempInfo.vecGenNum[_index].fRate;
		_NumRate->m_Type = GenItemEditData::NUMRATE;
		_NumRate->m_StrName = "GenNum";

		_GenItemEdit.m_listNumRate.push_back(_NumRate);
	}

	_Size = _tempInfo.vecGenItem.size();
	for(unsigned int _index = 0; _index < _Size; ++_index)
	{
		GenItemEditData::SpIDnRate* _SpIDnRate;
		switch(_tempInfo.vecGenItem[_index]->emType)
		{
			case EMGENITEMTYPE_ITEM:
				{
					std::tr1::shared_ptr<SGENITEMSPEC> pTempGen = std::tr1::static_pointer_cast<SGENITEMSPEC>(_tempInfo.vecGenItem[_index]);
					_SpIDnRate = new GenItemEditData::SpIDnRate;
					
					if(pTempGen->dwSpecID != 0 && pTempGen->dwSpecID <= GLItemMan::SPECID_NUM)
						_SpIDnRate->m_SpecID = pTempGen->dwSpecID;

					_SpIDnRate->m_Rate = pTempGen->fRate;
					_SpIDnRate->m_Type = GenItemEditData::SPIDNRATE_ITEMSPEC;
					_SpIDnRate->m_StrName = "ITEMSPEC";

					_GenItemEdit.m_listSpIDnRate.push_back(_SpIDnRate);
					break;
				}
			case EMGENITEMTYPE_ITEMSKILLFACT:
				{
					std::tr1::shared_ptr<SGENITEMSPECSKILLFACT> pTempGen = std::tr1::static_pointer_cast<SGENITEMSPECSKILLFACT>(_tempInfo.vecGenItem[_index]);
					_SpIDnRate = new GenItemEditData::SpIDnRate;
					if(pTempGen->dwSpecID != 0 && pTempGen->dwSpecID <= GLItemMan::SPECID_NUM)
						_SpIDnRate->m_SpecID = pTempGen->dwSpecID;

					_SpIDnRate->m_Rate = pTempGen->fRate;
					_SpIDnRate->m_SkillID.wMainID = pTempGen->sSkillID.wMainID;
					_SpIDnRate->m_SkillID.wSubID = pTempGen->sSkillID.wSubID;
					_SpIDnRate->m_Type = GenItemEditData::SPIDNRATE_SKILLFACT;
					_SpIDnRate->m_StrName = "SKILLFACT";

					_GenItemEdit.m_listSpIDnRate.push_back(_SpIDnRate);
					break;
				}
		}
	}	
}

void GenItem::GenItemManager::GetItemInfo(ItemInfo &_Info, std::string _Name, int _Mid, int _Sid)
{
	if(_Name != "")
		m_InstanceGenItemTool->GetItemInfo(_Info, _Name);
	else if(_Mid != 65535 && _Sid != 65535)
		m_InstanceGenItemTool->GetItemInfo(_Info, _Mid, _Sid);
	else 
		return;
}

void GenItem::GenItemManager::GetItemInfo(ItemInfo &_Info, int _Index)
{
	if(_Index < 0) return;

	m_InstanceGenItemTool->GetItemInfo(_Info, _Index);
}

void GenItem::GenItemManager::GetSpecIDInfo(SpecIDInfo* &_Info, int _SpecID)
{
	if(_SpecID < 0 || _SpecID >= GLItemMan::SPECID_NUM)
		return;

	m_InstanceGenItemTool->GetSpecIDInfo(_Info, _SpecID);
}

void GenItem::GenItemManager::DataChageGenInfo(SGENINFO& _Output, std::vector<std::string> &_annotation, GenItemEditData &_Tooldata)
{	
	_Output.dwVersion = 0;
	_Output.fProbability = _Tooldata.m_FileGenRate.m_Rate;

	std::list<GenItem::GenItemEditData::NumRate*>::iterator _iterBegin(_Tooldata.m_listNumRate.begin());
	std::list<GenItem::GenItemEditData::NumRate*>::iterator _iterEnd(_Tooldata.m_listNumRate.end());	
	
	for(;_iterBegin != _iterEnd; ++_iterBegin)
	{
		SGENNUM sGenNum;

		sGenNum.wNum = (*_iterBegin)->m_Num;
		sGenNum.fRate = (*_iterBegin)->m_NumRate;

		_Output.PushGenNum(sGenNum);
	}	

	std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _iteratorBegin(_Tooldata.m_listSpIDnRate.begin());
	std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _iteratorEnd(_Tooldata.m_listSpIDnRate.end());

	for(; _iteratorBegin != _iteratorEnd; ++_iteratorBegin)
	{
		if((*_iteratorBegin)->m_Type == GenItem::GenItemEditData::SPIDNRATE_ITEMSPEC)
		{	
			std::tr1::shared_ptr<SGENITEMSPEC> spItemSpec( new SGENITEMSPEC );

			spItemSpec->dwSpecID = (*_iteratorBegin)->m_SpecID;
			spItemSpec->fRate = (*_iteratorBegin)->m_Rate;

			_Output.PushGenItem(std::tr1::static_pointer_cast<SGENITEMBASE>(spItemSpec));
			_annotation.push_back((*_iteratorBegin)->m_StrAnnotation);
		}
		else if((*_iteratorBegin)->m_Type == GenItem::GenItemEditData::SPIDNRATE_SKILLFACT)
		{
			std::tr1::shared_ptr<SGENITEMSPECSKILLFACT> spItemSpecSkillfact( new SGENITEMSPECSKILLFACT );

			spItemSpecSkillfact->sSkillID.wMainID = (*_iteratorBegin)->m_SkillID.wMainID;
			spItemSpecSkillfact->sSkillID.wSubID = (*_iteratorBegin)->m_SkillID.wSubID;
			spItemSpecSkillfact->dwSpecID = (*_iteratorBegin)->m_SpecID;
			spItemSpecSkillfact->fRate = (*_iteratorBegin)->m_Rate;

			_Output.PushGenItem(std::tr1::static_pointer_cast<SGENITEMBASE>(spItemSpecSkillfact));
			_annotation.push_back((*_iteratorBegin)->m_StrAnnotation);
		}
	}	

	_Output.emType = EMGENINFOTYPE_NONE;
}

void GenItem::GenItemManager::SaveData(GenItem::GenItemEditData &_data, std::string &_FileName)
{
	SGENINFO _temp;
	std::vector<std::string> _vectmp;
	DataChageGenInfo(_temp, _vectmp, _data);
	if(!GenItem::SaveFile(_temp, _FileName, _vectmp))
		MessageBox(NULL, "Data Save Success", "Revise", MB_OK);
	else
		MessageBox(NULL, "Data Save Fail", "Revise", MB_OK);
}