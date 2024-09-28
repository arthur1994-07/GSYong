#ifndef ArchiveType_H
#define ArchiveType_H

#include <boost/tr1/memory.hpp>
#include <boost/function.hpp>
#include <set>

namespace sc
{
    class BaseStream;
    class SerialFile;
    class CFileFindTree;
}

typedef std::tr1::shared_ptr<sc::BaseStream> SPBaseStream;
typedef std::tr1::shared_ptr<sc::SerialFile> SPSerialFile;
typedef std::tr1::shared_ptr<sc::CFileFindTree> SPFileFindTree;
typedef std::set<TSTRING> StrContainer;

#endif