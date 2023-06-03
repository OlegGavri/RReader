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
};

#endif // SETTINGS_H
