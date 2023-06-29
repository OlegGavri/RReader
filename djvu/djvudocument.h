#ifndef DJVUDOCUMENT_H
#define DJVUDOCUMENT_H

#include <libdjvu/ddjvuapi.h>

#include "document.h"
#include "djvucontentitemmodel.h"

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
    ContentsItemModel * getContentItemModel() const override;
    void zoomIn() override;
    void zoomOut() override;
    qreal getScale() const override;
    void setScale(qreal) override;

    void saveSettings() override;

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

    // Number of pages in document
    int totalPageNumber = 0;

    DjvuContentItemModel * contentsItemModel = nullptr;

    // Clear scene and fill it with new pages
    void fillSceneWithPages();
};

#endif // DJVUDOCUMENT_H
