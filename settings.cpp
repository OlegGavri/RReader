#include <QSettings>

#include "settings.h"

using namespace std;

void Settings::Init()
{

}

DocumentSettings Settings::GetDocumentSettings(const QString documentName)
{
    DocumentSettings documentSettings;

    const QString scaleKey = QString("documents/%1/scale").arg(documentName);
    const QString pageKey = QString("documents/%1/page").arg(documentName);

    QSettings settings;
    bool ok;

    qreal scale = settings.value(scaleKey).toReal(&ok);
    if(ok)
    {
        documentSettings.scale = scale;
    }

    int page = settings.value(pageKey).toInt(&ok);
    if(ok)
    {
        documentSettings.page = page;
    }

    return documentSettings;
}

void Settings::SetDocumentSettings(
    const QString documentName,
    const DocumentSettings documentSettings)
{
    const QString scaleKey = QString("documents/%1/scale").arg(documentName);
    const QString pageKey = QString("documents/%1/page").arg(documentName);

    QSettings settings;

    optional<qreal> scale = documentSettings.scale;
    if(scale.has_value())
    {
        settings.setValue(scaleKey, scale.value());
    }

    optional<int> page = documentSettings.page;
    if(page.has_value())
    {
        settings.setValue(pageKey, page.value());
    }
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
