#include "stdafx.h"
#include "../File/FileUtil.h"
#include "./AssetList.h"

#include "../DebugInclude.h"

namespace sc
{
    AssetData::AssetData(const std::string& Parent)        
    {
        CString FileName = sc::file::GetFileOnly(Parent.c_str());
        FileName.MakeLower();        
        m_Parent = FileName.GetString();
    }

    AssetData::AssetData(const std::string& Parent, const std::string& Child)
    {
        CString FileName = sc::file::GetFileOnly(Parent.c_str());
        FileName.MakeLower();        
        m_Parent = FileName.GetString();

        AddChild(Child);
    }
        
    AssetData::~AssetData()
    {       
    }

    void AssetData::AddChild(const std::string& Child)
    {
        CString FileName = sc::file::GetFileOnly(Child.c_str());
        FileName.MakeLower();

        bool bFind = false;
        for (size_t i=0; i<m_Child.size(); ++i)
        {
            if (FileName.GetString() == m_Child[i])
                bFind = true;
        }

        if (!bFind)
            m_Child.push_back(FileName.GetString());
    }

    AssetList::AssetList()
    {
    }

    AssetList::~AssetList()
    {
    }

    AssetList& AssetList::GetInstance()
    {
        static AssetList Instance;
        return Instance;
    }
    
    void AssetList::Add(const std::string& Parent, const std::string& Child)
    {
        CString ParentFileName = sc::file::GetFileOnly(Parent.c_str());
        ParentFileName.MakeLower();

        ASSET_DATA_ITER iter = m_Data.find(ParentFileName.GetString());
        if (iter != m_Data.end())
        {
            std::tr1::shared_ptr<AssetData> spData = iter->second;
            spData->AddChild(Child);
        }
        else
        {
            std::tr1::shared_ptr<AssetData> spData(new AssetData(ParentFileName.GetString(), Child));
            m_Data.insert(ASSET_DATA_VALUE(ParentFileName.GetString(), spData));
        }
    }

    void AssetList::Clear()
    {
        m_Data.clear();
    }

} // namespace sc