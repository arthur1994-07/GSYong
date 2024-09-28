#ifndef _SC_REPORT_FILES_XML_H_
#define _SC_REPORT_FILES_XML_H_

#pragma once

#include <string>
#include <vector>

namespace ticpp
{
    class Document;
    class Exception;
}

namespace sc
{
    class ReportFilesXml
    {
    public:
        ReportFilesXml();
        ~ReportFilesXml();

    protected:
        ticpp::Document* m_pXml;
        std::vector<std::string> m_BanFiles;

    public:
        bool Load(const std::string& FileName);
        const std::vector<std::string>& GetBanFiles() const { return m_BanFiles; }

    protected:
        bool Parse();
    };

} // namespace sc

#endif // _SC_REPORT_FILES_XML_H_