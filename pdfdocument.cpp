#include <stdexcept>

#include "pdfdocument.h"
#include "pdfpagegraphicsitem.h"

using namespace std;

// Gap beetween pages
constexpr int PageGap = 10;
constexpr qreal ScaleFactor = 1.2;

PdfDocument::PdfDocument(const QString fileName):
    fileName(fileName)
{
    document = Poppler::Document::load(fileName);
    if(document == nullptr || document->isLocked())
    {
        delete document;
        QString msg = QString("Open file %1 error").arg(fileName);
        throw runtime_error(msg.toStdString());
    }

    document->setRenderHint(Poppler::Document::TextAntialiasing);

    // Create QGraphicsScene and fill it with PDF document pages
    scene = new QGraphicsScene();

    fillSceneWithPages();

    // Create contents
    QVector<Poppler::OutlineItem> outline = document->outline();
    if(!outline.empty())
        contentItemModel = new ContentItemModel(outline);
    else
        contentItemModel = nullptr;
}

PdfDocument::~PdfDocument()
{
    delete scene;
    delete document;
    delete contentItemModel;
}

QString PdfDocument::getFileName() const
{
    return fileName;
}

QGraphicsScene * PdfDocument::getScene() const
{
    return scene;
}

int PdfDocument::getPageNumber() const
{
    return document->numPages();
}

int PdfDocument::getCurrentPage() const
{
    return currentPage;
}

void PdfDocument::setCurrentPage(int page)
{
    currentPage = page;
}

QAbstractItemModel * PdfDocument::getContentItemModel() const
{
    return contentItemModel;
}

void PdfDocument::zoomIn()
{
    currentScale *= ScaleFactor;
    fillSceneWithPages();
}

void PdfDocument::zoomOut()
{
    currentScale /= ScaleFactor;
    fillSceneWithPages();
}

void PdfDocument::fillSceneWithPages()
{
    scene->clear();
    const int numPages = document->numPages();

    int y = 0;
    for(int i = 0; i < numPages; i++)
    {
        PdfPageGraphicsItem * item = new PdfPageGraphicsItem(document, i, currentScale);
        item->setPos(0, y);
        scene->addItem(item);

        int h = item->boundingRect().height();
        y += h + PageGap;
    }

    // Update scene height before go to page. Without this showPage not
    // working in this place.
    scene->height();
}
