#include <stdexcept>

#include "document.h"
#include "pagegraphicsitem.h"

using namespace std;

// Gap beetween pages
constexpr int PageGap = 10;
constexpr qreal ScaleFactor = 1.2;

Document::Document(const QString fileName)
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

Document::~Document()
{
    delete scene;
    delete document;
    delete contentItemModel;
}

QGraphicsScene * Document::getScene() const
{
    return scene;
}

int Document::getPageNumber() const
{
    return document->numPages();
}

int Document::getCurrentPage() const
{
    return currentPage;
}

void Document::setCurrentPage(int page)
{
    currentPage = page;
}

QAbstractItemModel * Document::getContentItemModel() const
{
    return contentItemModel;
}

void Document::zoomIn()
{
    currentScale *= ScaleFactor;
    fillSceneWithPages();
}

void Document::zoomOut()
{
    currentScale /= ScaleFactor;
    fillSceneWithPages();
}

void Document::fillSceneWithPages()
{
    scene->clear();
    const int numPages = document->numPages();

    int y = 0;
    for(int i = 0; i < numPages; i++)
    {
        PageGraphicsItem * item = new PageGraphicsItem(document, i, currentScale);
        item->setPos(0, y);
        scene->addItem(item);

        int h = item->boundingRect().height();
        y += h + PageGap;
    }

    // Update scene height before go to page. Without this showPage not
    // working in this place.
    scene->height();
}
