#ifndef READUPDATEFILE_H_
#define READUPDATEFILE_H_
#include <string.h>
#include <stdio.h>
#include <list>
#include <map>
#include "tinyxml.h"
#include "sound.h"
using namespace std;

class readXmlFile
{
public:
	readXmlFile(std::string strFilePath, map<std::string, Sound*>& m_map);
	virtual ~readXmlFile();
private:
	void paraseFile(std::string strFilePath, map<std::string, Sound*>& m_map);
	void paraseNode(TiXmlElement* rootElement, map<std::string,Sound*>& m_map);
	int getIntNum(char a[]);
	float getFloatNum(char a[]);
};

#endif /* READUPDATEFILE_H_ */
