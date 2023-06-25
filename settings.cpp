#include <QSettings>
#include <QDataStream>

#include "settings.h"

using namespace std;

QDataStream & operator << (QDataStream & stream, const DocumentSettings & settings)
{
    stream << settings.page;
    stream << settings.scale;

    return stream;
}

QDataStream & operator >> (QDataStream & stream, DocumentSettings & settings)
{
    stream >> settings.page;
    stream >> settings.scale;
    return stream;
}

Q_DECLARE_METATYPE(DocumentSettings)

void Settings::Init()
{
    qRegisterMetaTypeStreamOperators<DocumentSettings>("DocumentSettings");
}

optional<DocumentSettings> Settings::GetDocumentSettings(const QString documentName)
{
    QSettings settings;
    QVariant variant = settings.value(QString("docsettings/%1").arg(documentName));

    if(!variant.isValid())
        return nullopt;

    DocumentSettings documentSettings = variant.value<DocumentSettings>();
    return documentSettings;
}

void Settings::SetDocumentSettings(
    const QString documentName,
    const DocumentSettings documentSettings)
{
    QSettings settings;
    settings.setValue(QString("docsettings/%1").arg(documentName), QVariant::fromValue(documentSettings));
}

void Settings::SetDocumentsList(QStringList fileList)
{
    QSettings settings;
    settings.setValue("openFilesList", QVariant(fileList));
}

QStringList Settings::GetDocumentList()
{
    //
    // Get from settings QList<QVariant>, convert it to QList<QString>
    //
    QSettings settings;
    QStringList fileList = settings.value("openFilesList").toStringList();
    return fileList;
}

void Settings::SetOpenDocumentNumber(const int num)
{
    QSettings settings;
    settings.setValue("openDocumentNum", num);
}

optional<int> Settings::GetOpenDocumentNumber()
{
    QSettings settings;
    bool ok;
    int num  = settings.value("openDocumentNum").toInt(&ok);

    if(ok)
        return num;
    else
        return nullopt;
}

QStringList Settings::GetRecentDocuments()
{
    QSettings settings;
    QVariant variant = settings.value("recentDocuments");
    QStringList list = variant.toStringList();

    return list;
}

void Settings::SetRecentDocuments(QStringList recentDocs)
{
    QSettings settings;
    QVariant variant = QVariant::fromValue(recentDocs);
    settings.setValue("recentDocuments", variant);
}
