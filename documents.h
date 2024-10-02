#ifndef DOCUMENTS_H
#define DOCUMENTS_H

#include <QList>

#include "document.h"

/**
 * @brief Static class. Hold all open docuemnts in the same order
 * as in Main Window.
 */
class Documents
{
private:
    // List of currently open documents. Its order is the same as in Main Window tabs
    static QList<Document*> openDocuments;

public:
    // Define current document in list.
    static const int CURRENT = -1;

    Documents() = delete;
    Documents(const Documents &) = delete;

    // Initialize documents.
    // docs - Open documents from docs
    static void Init(QStringList docs);
    static void Deinit();

    // Open new document with path, with position after n.
    // If there are no open documents in position 0.
    // Error: range_error - n has invalid postion
    // runtime_error
    static Document & open(const QString path, int n = CURRENT);

    // Close document in postion n, or last document if n = LAST
    // Error: range_error
    static void close(int n = CURRENT);

    // Close current document
    static void closeCurrent() noexcept;

    // Number of currently open documents
    static int getCount() noexcept;

    // Return Document with number b
    // Error: range_error
    static Document & getDocument(int n);

    // Return current document
    static Document & getCurrent() noexcept;

    // Set current document to document with nuber n
    static void setCurrent(const int n);

    // Return number of current open document
    static int getCurrentNumber();

    // Swap 2 documents
    // Error: range_error
    static void swap(int i0, int i1);

    // Return directory of recent open document
    static QString getLastOpenDir();

    // Get list of recent open documents
    static QStringList getRecentDocuments();
};

#endif // DOCUMENTS_H
