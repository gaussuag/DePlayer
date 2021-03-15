#include "MediaInfoReader.h"

#include "MediaInfoDLL.h"

using namespace MediaInfoDLL;



MediaInfoReader::MediaInfoReader(QObject *parent) : QObject(parent)
{

}

bool MediaInfoReader::getMediaInfoProperty(const QString & filePath,const QString & property,QString & value)
{
    MediaInfo info;

    auto openFlag = info.Open(toMediaString(filePath));
    if(openFlag == 0)
        return false;

    auto propertyValue = info.Get(Stream_Video,0,toMediaString(property));
    value = fromMediaString(propertyValue);

    return true;
}
