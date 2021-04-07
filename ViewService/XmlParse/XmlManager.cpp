#include "XmlManager.h"

XmlManager::XmlManager()
{
}

XmlManager::~XmlManager()
{
}

ViewInfoPtr XmlManager::GetViewInfo(const std::string keyName)
{
    ViewInfoPtr viewInfo = NULL;
    for (auto iter = m_configInfoMap.begin(); iter != m_configInfoMap.end(); iter++)
    {
        if (iter->first == keyName)
        {
            viewInfo = iter->second;
            break;
        }
    }

    return viewInfo;
}

void XmlManager::XmlParse(const std::string &filename)
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(filename.c_str()))
    {
        ErrPrint("Failed to load config file [%s]!!\n", filename.c_str());
        exit(1);
    }

    TiXmlElement *rootElement = xmlDoc.RootElement();

    TiXmlElement *pEle = rootElement;
    for (TiXmlElement *stuElement = pEle->FirstChildElement();
         stuElement != NULL;
         stuElement = stuElement->NextSiblingElement())
    {
        TiXmlAttribute *pAttr = stuElement->FirstAttribute();

        while (NULL != pAttr)
        {
            pAttr = pAttr->Next();
        }

        for (TiXmlElement *sonElement = stuElement->FirstChildElement();
             sonElement != NULL;
             sonElement = sonElement->NextSiblingElement())
        {
            TiXmlAttribute *pChildAttr = sonElement->FirstAttribute();

            ViewInfoPtr viewInfo = std::make_shared<ViewInfo>();
            while (NULL != pChildAttr)
            {
                XmlAttrParse(viewInfo, pChildAttr);
                pChildAttr = pChildAttr->Next();
            }
            if (!viewInfo->GetName().empty() && !viewInfo->GetExtraInfo().empty())
            {
                m_configInfoMap.insert(std::make_pair((viewInfo->GetName() + viewInfo->GetExtraInfo()), viewInfo));
            }
        }
    }
}

void XmlManager::XmlAttrParse(ViewInfoPtr viewInfo, TiXmlAttribute *pAttr)
{
    if (strcmp(pAttr->Name(), "name") == 0)
    {
        viewInfo->SetName(pAttr->Value());
        viewInfo->SetExtraInfo(pAttr->Value());
    }
    else if (strcmp(pAttr->Name(), "pid") == 0)
    {
        viewInfo->SetPid(pAttr->Value());
    }
    else if (strcmp(pAttr->Name(), "repeat") == 0)
    {
        viewInfo->SetRepeatStatus(pAttr->Value());
    }
    else if (strcmp(pAttr->Name(), "loop") == 0)
    {
        char tmpLoopBuff[strlen(pAttr->Value()) + 1];
        memset(tmpLoopBuff, 0, sizeof(tmpLoopBuff));
        strcpy(tmpLoopBuff, pAttr->Value());

        int loopValue = this->GetIntNum(tmpLoopBuff);
        viewInfo->SetLoop(loopValue);
    }
    else if (strcmp(pAttr->Name(), "prority") == 0)
    {
        char tmpPriorityBuff[strlen(pAttr->Value()) + 1];
        memset(tmpPriorityBuff, 0, sizeof(tmpPriorityBuff));
        strcpy(tmpPriorityBuff, pAttr->Value());

        int priorityValue = this->GetIntNum(tmpPriorityBuff);
        viewInfo->SetPriority(priorityValue);
    }
    else if (strcmp(pAttr->Name(), "type") == 0)
    {
        viewInfo->SetType(pAttr->Value());
    }
    else if (strcmp(pAttr->Name(), "notifyType") == 0)
    {
        viewInfo->SetNotifyType(pAttr->Value());
    }
    else if (strcmp(pAttr->Name(), "mutex") == 0)
    {
        viewInfo->SetMutexStatus(pAttr->Value());
    }
    else if (strcmp(pAttr->Name(), "power") == 0)
    {
        viewInfo->SetPower(pAttr->Value());
    }
    else if (strcmp(pAttr->Name(), "audiobind") == 0)
    {
        viewInfo->SetAudioBindInfo(pAttr->Value());
    }
    else if (strcmp(pAttr->Name(), "least") == 0)
    {
        char tmpLeastBuff[strlen(pAttr->Value()) + 1];
        memset(tmpLeastBuff, 0, sizeof(tmpLeastBuff));
        strcpy(tmpLeastBuff, pAttr->Value());

        int leastValue = this->GetIntNum(tmpLeastBuff);
        viewInfo->SetLeast(leastValue);
    }
}

int XmlManager::GetIntNum(char a[])
{
    int i = 0, num = 0;
    while (a[i] != '\0')
    {
        if (a[i] >= '0' && a[i] <= '9')
        {
            num = num * 10 + a[i] - '0';
        }
        i++;
    }
    return num;
}
float XmlManager::GetFloatNum(char a[])
{
    int i = 0, num_freq = 0;
    float num = 0;
    while (a[i] != '\0')
    {
        if (a[i] >= '0' && a[i] <= '9')
        {
            num = num * 10 + a[i] - '0';
            num_freq = num_freq * 10;
        }
        i++;
        if (a[i] == '.')
        {
            num_freq = 1;
        }
    }
    if (num_freq != 0)
    {
        num = num / num_freq;
    }
    return num;
}