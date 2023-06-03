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

    QSettings settings;
    bool ok;

    qreal scale = settings.value(scaleKey).toReal(&ok);
    if(ok)
    {
        documentSettings.scale = scale;
    }

    return documentSettings;
}

void Settings::SetDocumentSettings(
    const QString documentName,
    const DocumentSettings documentSettings)
{
    const QString scaleKey = QString("documents/%1/scale").arg(documentName);

    QSettings settings;

    optional<qreal> scale = documentSettings.scale;

    if(scale.has_value())
    {
        settings.setValue(scaleKey, scale.value());
    }
}
