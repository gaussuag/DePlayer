#ifndef MEDIAINFOREADER_H
#define MEDIAINFOREADER_H

///编码不同使用不同char类型转换
#ifdef UNICODE
    #define toMediaString(qString) qString.toStdWString()
    #define fromMediaString(mediaString) QString::fromStdWString(mediaString)
#else
    #define toMediaString(qstring) qstring.toStdString()
    #define fromMediaString(mediaString) QString::fromStdString(mediaString)
#endif // !UNICODE


#include <QObject>

/*!
 * \brief The MediaInfoReader class 基于Qt数据类型的mediainfoDLL的封装类，提供一些Qt数据类型的接口来读取视频文件的一些信息
 */
class MediaInfoReader : public QObject
{
    Q_OBJECT
public:
    explicit MediaInfoReader(QObject *parent = nullptr);

    static bool getMediaInfoProperty(const QString &filePath, const QString &property, QString &value);
private:

};

#endif // MEDIAINFOREADER_H
