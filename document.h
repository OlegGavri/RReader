#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QString>
#include <QGraphicsScene>
#include <QAbstractItemModel>

class Document
{
public:
    virtual ~Document() = 0;

    // Graphic scene with pages
    virtual QGraphicsScene * getScene() const = 0;

    // Full file path of this document file
    virtual QString getFileName() const = 0;

    // Get page number of document
    virtual int getPageNumber() const = 0;

    // Get current page
    virtual int getCurrentPage() const = 0;

    virtual void setCurrentPage(int page) = 0;

    // Return content item model for this document
    // It cat set this model for QTreeView
    // Return nullptr, if model not present
    virtual QAbstractItemModel * getContentItemModel() const = 0;

    // Return/set current scale factor for the document
    virtual void zoomIn() = 0;
    virtual void zoomOut() = 0;

    // Create new Document class for fileName file
    static Document * createDocument(const QString fileName);
};

#endif // DOCUMENT_H
