#ifndef _XML_MANAGER_H_
#define _XML_MANAGER_H_

#include "tinyxml.h"
#include "Util.h"
#include "ViewInfo.hpp"
#include "Singleton.hpp"

typedef std::shared_ptr<ViewInfo> ViewInfoPtr;
typedef std::unordered_map<std::string, ViewInfoPtr> ConfigInfoMap;

class XmlManager : public Singleton<XmlManager>
{
public:
    XmlManager();
    ~XmlManager();

    ViewInfoPtr GetViewInfo(const std::string keyName);
    void XmlParse(const std::string &filename);

private:
    void XmlAttrParse(ViewInfoPtr viewInfo, TiXmlAttribute *pAttr);
    int GetIntNum(char a[]);
    float GetFloatNum(char a[]);

    /* data */
    std::mutex configMutex;
    ViewInfoPtr m_viewInfo;
    ConfigInfoMap m_configInfoMap; //< name : ViewInfo* >
};

#endif // !_XML_PARSE_HPP_