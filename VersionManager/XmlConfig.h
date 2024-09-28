#ifndef _VERSION_MAMANGER_XML_CONFIG_H_
#define _VERSION_MAMANGER_XML_CONFIG_H_

#pragma once

#include "IVersionManConfig.h"

namespace ticpp
{
    class Document;
    class Exception;
}

class XmlConfig : public IVersionManConfig
{
public:
    XmlConfig();
    virtual ~XmlConfig();

protected:
    ticpp::Document* m_pXml;
    

protected:
    bool Parse();

public:
    virtual bool Load(const char* filename) override;
};

#endif // _VERSION_MAMANGER_XML_CONFIG_H_