#ifndef SETTINGS_H
#define SETTINGS_H

#include <optional>

#include <QStringList>

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
    static void SetDocumentsList(QStringList);

    // Get from settings list of documents opended in last session
    // Return list of filepath
    static QStringList GetDocumentList();

    // Save in settins number of currenlty open documents(in documents list)
    static void SetOpenDocumentNumber(const int num);

    // Return number of open document in docuemnt list from last session
    static std::optional<int> GetOpenDocumentNumber();

    // Return list of recent documents. String - path of document
    static QStringList GetRecentDocuments();

    // Store list of recent documents
    // recentDocs - documents path
    static void SetRecentDocuments(QStringList recentDocs);
};

#endif // SETTINGS_H
