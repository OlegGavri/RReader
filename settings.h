#ifndef SETTINGS_H
#define SETTINGS_H

#include <optional>

#include <QString>

//
// Application settings
// Settings about documents is saved with keys 'documents/<docname>/<key>
//

struct DocumentSettings
{
    std::optional<qreal> scale;

    // Page on which document was opened
    std::optional<int> page;
};

class Settings
{
    Settings() = delete;
public:
    // Initialize application settings
    static void Init();

    // Get settings for a specific document.
    // Document name - file name with path
    static DocumentSettings GetDocumentSettings(const QString documentName);

    // Set settings for document
    // Document name - file name with path
    static void SetDocumentSettings(const QString documentName, const DocumentSettings);

    // Save in settins open documents list.
    // Get list of filepath
    static void SetDocumentsList(QList<QString>);

    // Get from settings list of documents opended in last session
    // Return list of filepath
    static QList<QString> GetDocumentList();
};

#endif // SETTINGS_H
