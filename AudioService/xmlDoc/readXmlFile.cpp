#include "readXmlFile.h"

readXmlFile::readXmlFile(std::string strFilePath, map<std::string, Sound *> &m_map)
{
	if (!strFilePath.empty())
	{

		FILE *fp;
		fp = fopen(strFilePath.c_str(), "r");
		if (fp != NULL)
		{
			fclose(fp);
			paraseFile(strFilePath, m_map);
		}
	}
}

readXmlFile::~readXmlFile()
{
}

void readXmlFile::paraseNode(TiXmlElement *rootElement, map<std::string, Sound *> &m_map)
{
	TiXmlElement *stuElement = rootElement->FirstChildElement();

	while (stuElement != NULL)
	{
		TiXmlAttribute *pAttr = stuElement->FirstAttribute();
		Sound *tmpSoud = new Sound();
		while (pAttr != NULL)
		{
			if (strcmp(pAttr->Name(), "name") == 0)
			{
				std::string strName = pAttr->Value();
				tmpSoud->setName(strName);
			}
			else if (strcmp(pAttr->Name(), "chime_id") == 0)
			{
				std::string strChimeId = pAttr->Value();
				tmpSoud->setChimeId(strChimeId);
			}
			else if (strcmp(pAttr->Name(), "freq") == 0)
			{
				std::string strFreq = pAttr->Value();
				tmpSoud->setFreq(strFreq);
			}
			else if (strcmp(pAttr->Name(), "priority") == 0)
			{
				char tmp[strlen(pAttr->Value()) + 1];
				strcpy(tmp, pAttr->Value());
				tmpSoud->setPriority(getIntNum(tmp));
			}
			else if (strcmp(pAttr->Name(), "power") == 0)
			{
				std::string strPowerMode = pAttr->Value();
				tmpSoud->setPowerMode(strPowerMode);
			}
			else if (strcmp(pAttr->Name(), "loop") == 0)
			{
				char tmp[strlen(pAttr->Value()) + 1];
				strcpy(tmp, pAttr->Value());
				int iLoop = atoi(tmp);
				tmpSoud->setLoop(iLoop);
			}
			else if (strcmp(pAttr->Name(), "playmode") == 0)
			{
				std::string strPlayMode = pAttr->Value();
				tmpSoud->setPlayMode(strPlayMode);
			}
			else if (strcmp(pAttr->Name(), "stopmode") == 0)
			{
				std::string strStopMode = pAttr->Value();
				tmpSoud->setStopMode(strStopMode);
			}
			else if (strcmp(pAttr->Name(), "selfcheck") == 0)
			{
				bool bSelfCheck = (strcmp(pAttr->Value(), "delay") == 0) ? true : false;
				tmpSoud->setSelfCheck(bSelfCheck);
			}
			else if (strcmp(pAttr->Name(), "ontime") == 0)
			{
				char tmp[strlen(pAttr->Value()) + 1];
				strcpy(tmp, pAttr->Value());
				int iOnTime = atoi(tmp);
				tmpSoud->setOnTime(iOnTime);
			}
			else if (strcmp(pAttr->Name(), "oninterrupt") == 0)
			{
				bool bOnInterrupt = (strcmp(pAttr->Value(), "true") == 0) ? true : false;
				tmpSoud->setOnInterrupt(bOnInterrupt);
			}
			pAttr = pAttr->Next();
		}
		m_map.insert(map<std::string, Sound *>::value_type(tmpSoud->getName(), tmpSoud));
		stuElement = stuElement->NextSiblingElement();
	}
}
void readXmlFile::paraseFile(std::string strFilePath, map<std::string, Sound *> &m_map)
{
	TiXmlDocument *myDocument = new TiXmlDocument();
	myDocument->LoadFile(strFilePath.c_str());
	TiXmlElement *rootElement = myDocument->RootElement();
	paraseNode(rootElement, m_map);
	if (myDocument != NULL)
		delete myDocument;
}

//
int readXmlFile::getIntNum(char a[])
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

//
float readXmlFile::getFloatNum(char a[])
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
