#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include <poppler/qt5/poppler-qt5.h>

#include "document.h"
#include "contentitemmodel.h"

//
// Information about open document and QGraphicsScene, filled with pages QGraphicsItem
//
class PdfDocument : public Document
{
public:
    // Open document
    PdfDocument(const QString fileName);
    ~PdfDocument();

    QString getFileName() const override;

    // Graphic scene with pages
    QGraphicsScene * getScene() const override;

    // Get page number of document
    int getPageNumber() const override;

    // Get current page
    int getCurrentPage() const override;

    void setCurrentPage(int page) override;

    // Return content item model for this document
    // It cat set this model for QTreeView
    // Return nullptr, if model not present
    QAbstractItemModel * getContentItemModel() const override;

    // Return/set current scale factor for the document
    void zoomIn() override;
    void zoomOut() override;

private:
    const QString fileName;

    Poppler::Document * document = nullptr;

    QGraphicsScene * scene = nullptr;

    // QAbstractItemModel for QTreeView. Contain contents
    ContentItemModel * contentItemModel = nullptr;

    int currentPage = 0;

    qreal currentScale = 1.0;

    // Clear scene and fill it with new pages
    void fillSceneWithPages();
};

#endif // PDFDOCUMENT_H
