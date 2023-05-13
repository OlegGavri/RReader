#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QString>
#include <QGraphicsScene>
#include <QAbstractItemModel>

#include <poppler/qt5/poppler-qt5.h>

#include "contentitemmodel.h"

//
// Information about open document and QGraphicsScene, filled with pages QGraphicsItem
//
class Document
{
public:
    // Open document
    Document(const QString fileName);
    ~Document();

    // Graphic scene with pages
    QGraphicsScene * getScene() const;

    // Get page number of document
    int getPageNumber() const;

    // Get current page
    int getCurrentPage() const;

    void setCurrentPage(int page);

    // Return content item model for this document
    // It cat set this model for QTreeView
    // Return nullptr, if model not present
    QAbstractItemModel * getContentItemModel() const;

    // Return/set current scale factor for the document
    void zoomIn();
    void zoomOut();

private:

    Poppler::Document * document = nullptr;

    QGraphicsScene * scene = nullptr;

    // QAbstractItemModel for QTreeView. Contain contents
    ContentItemModel * contentItemModel = nullptr;

    int currentPage = 0;

    qreal currentScale = 1.0;

    // Clear scene and fill it with new pages
    void fillSceneWithPages();
};

#endif // DOCUMENT_H
