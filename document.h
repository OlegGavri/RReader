#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QString>
#include <QGraphicsScene>
#include <QAbstractItemModel>

#include "contentsitemmodel.h"

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
    int getCurrentPage() const
    {
        return currentPage;
    }

    void setCurrentPage(int page)
    {
        currentPage = page;
    }

    // Return content item model for this document
    // It cat set this model for QTreeView
    // Return nullptr, if model not present
    virtual ContentsItemModel * getContentItemModel() const = 0;

    // Return/set current scale factor for the document
    virtual void zoomIn() = 0;
    virtual void zoomOut() = 0;
    virtual void setScale(qreal) = 0;
    virtual qreal getScale() const = 0;

    // Save document settins(page, scale, bookmars etc) in QSettings
    virtual void saveSettings() = 0;

    // Create new Document class for fileName file
    static Document * createDocument(const QString fileName);

private:
    int currentPage = 0;
};

#endif // DOCUMENT_H
