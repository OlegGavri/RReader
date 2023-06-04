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
