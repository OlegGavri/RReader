#ifndef DJVUDOCUMENT_H
#define DJVUDOCUMENT_H

#include <libdjvu/ddjvuapi.h>

#include "document.h"

//
// DJVU document
//
class DjvuDocument : public Document
{
public:
    DjvuDocument(const QString fileName);
    ~DjvuDocument();

    // Overrided methods from document class
    QGraphicsScene * getScene() const override;
    QString getFileName() const override;
    int getPageNumber() const override;
    int getCurrentPage() const override;
    void setCurrentPage(int page) override;
    QAbstractItemModel * getContentItemModel() const override;
    void zoomIn() override;
    void zoomOut() override;

private:
    friend class DjvuPageGraphicsItem;

    // DJVU libre context object. It's common for the application
    static ddjvu_context_t * context;

    // Return ddjvu context for this application.
    static ddjvu_context_t * getContext();

    const QString fileName;
    ddjvu_document_t * document;

    QGraphicsScene * scene;
    qreal currentScale = 1.0;

    int currentPage = 0;

    // Number of pages in document
    int totalPageNumber = 0;

    // Clear scene and fill it with new pages
    void fillSceneWithPages();
};

#endif // DJVUDOCUMENT_H
