#pragma once

#include <string>
#include <map>
#include <vector>
#include <boost/tr1/memory.hpp>

namespace sc
{
    class AssetData
    {
    public:
        AssetData(const std::string& Parent);
        AssetData(const std::string& Parent, const std::string& Child);
        ~AssetData();

    protected:
        std::string m_Parent;
        std::vector<std::string> m_Child;
        
    public:
        void AddChild(const std::string& Child);
    };

    class AssetList
    {
    public:
        typedef std::map<std::string, std::tr1::shared_ptr<AssetData> > ASSET_DATA;
        typedef ASSET_DATA::iterator       ASSET_DATA_ITER;
        typedef ASSET_DATA::const_iterator ASSET_DATA_CITER;
        typedef ASSET_DATA::value_type     ASSET_DATA_VALUE;

        AssetList();
        ~AssetList();
        static AssetList& GetInstance();

    protected:
        ASSET_DATA m_Data;

    public:
        void Add(const std::string& Parent, const std::string& Child);
        void Clear();
    };

} // namespace sc