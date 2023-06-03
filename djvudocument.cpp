#include <qdebug.h>
#include <QCoreApplication>

#include "djvudocument.h"
#include "djvupagegraphicsitem.h"

using namespace std;

constexpr int PageGap = 10;

ddjvu_context_t * DjvuDocument::context = nullptr;

// Handle dvju messages while DJVU operation. The documentation say that
// this handler is not optional. But I don't understand why.
static void handle_ddjvu_messages(ddjvu_context_t *ctx, int wait)
{
    const ddjvu_message_t *msg;
    if (wait)
        ddjvu_message_wait(ctx);

    while ((msg = ddjvu_message_peek(ctx)))
    {
        switch(msg->m_any.tag)
        {
        case DDJVU_ERROR:
            throw runtime_error("DjVu error");
            break;

        case DDJVU_INFO:
            qInfo() << "DjVu info: " << msg->m_error.message;
            break;

        case DDJVU_NEWSTREAM:
            qInfo() << "DjVu new stream";
            break;

        default: break;
        }
        ddjvu_message_pop(ctx);
    }
}

ddjvu_context_t * DjvuDocument::getContext()
{
    return context;
}

DjvuDocument::DjvuDocument(const QString fileName):
    fileName(fileName)
{
    //
    // Open and decode document
    //
    const QString appName = QCoreApplication::applicationName();

    // Create new context, if it was not created early
    if(!context)
        context = ddjvu_context_create(appName.toStdString().c_str());

    if(context == nullptr)
    {
        throw runtime_error("DjVu context creation error");
    }

    document = ddjvu_document_create_by_filename_utf8(
        context,
        fileName.toStdString().c_str(),
        false);
    if(document == nullptr)
    {
        throw runtime_error("DjVu document open error");
    }

    // Decode document
    while(!ddjvu_document_decoding_done(document))
        handle_ddjvu_messages(context, true);

    totalPageNumber = ddjvu_document_get_pagenum(document);

    // Create QGraphicsScene and fill it with document pages
    scene = new QGraphicsScene();

    fillSceneWithPages();

    // Get contents item model
    miniexp_t expOutline;
    while ((expOutline = ddjvu_document_get_outline(document)) == miniexp_dummy)
        handle_ddjvu_messages(context, TRUE);

    if(expOutline != miniexp_nil)
    {
        contentsItemModel = new DjvuContentItemModel(expOutline);
    }
}

DjvuDocument::~DjvuDocument()
{
    ddjvu_document_release(document);
    ddjvu_context_release(context);
    delete scene;
    delete contentsItemModel;
}

QGraphicsScene * DjvuDocument::getScene() const
{
    return scene;
}

QString DjvuDocument::getFileName() const
{
    return fileName;
}

int DjvuDocument::getPageNumber() const
{
    return totalPageNumber;
}

int DjvuDocument::getCurrentPage() const
{
    return currentPage;
}

void DjvuDocument::setCurrentPage(int page)
{
    currentPage = page;
}

ContentsItemModel * DjvuDocument::getContentItemModel() const
{
    return contentsItemModel;
}

void DjvuDocument::zoomIn()
{
    //TODO:
}

void DjvuDocument::zoomOut()
{
    //TODO:
}

void DjvuDocument::fillSceneWithPages()
{
    //
    // Fill scene with DjvuPageGraphicsItem. Each DjvuPageGraphicsItem display page of document.
    //
    scene->clear();
    const int pageNum = getPageNumber();

    // y-position center of document
    int y = 0;

    // Height of previous page. E.g. if add page 10, hPrev is height of page 9, etc
    int hPrev = 0;

    for(int i = 0; i < pageNum; i++)
    {
        DjvuPageGraphicsItem * item = new DjvuPageGraphicsItem(document, i, currentScale);
        int h = item->boundingRect().height();
        y += (h + hPrev)/2 + PageGap;

        item->setPos(0, y);
        scene->addItem(item);

        hPrev = h;
    }
}
