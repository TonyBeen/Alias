/*************************************************************************
    > File Name: XmlConfig.h
    > Author: hsz
    > Brief:
    > Created Time: Thu 12 Jan 2023 10:18:27 AM CST
 ************************************************************************/

#ifndef __CONFIG_XML_CONFIG_H__
#define __CONFIG_XML_CONFIG_H__

#include "typecast.h"
#include <string>
#include <map>

namespace eular {

class XmlConfig
{
public:
    XmlConfig();
    ~XmlConfig();

    bool loadFile(const std::string &xmlFile);
    bool parse(const std::string &xmlText);

    template<typename T>
    T lookup(const std::string &key, const T &defaultVal = T())
    {
        auto it = m_xmlMap.find(key);
        if (it == m_xmlMap.end()) {
            return defaultVal;
        }

        T ret = defaultVal;
        try {
            ret = TypeCast::type_cast<T>(it->second.c_str());
        } catch (...) {
        }

        return ret;
    }

    void foreach();

protected:
    void loadXml(std::string prefix, void *ele);

private:
    void *m_mutex;
    std::map<std::string, std::string> m_xmlMap;
};

} // namespace eular

#endif // __CONFIG_XML_CONFIG_H__
