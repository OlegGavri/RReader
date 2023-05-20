#include <cassert>

#include <QImage>
#include <stdexcept>

#include <QDebug>
#include <QPainter>

#include "djvupagegraphicsitem.h"
#include "djvudocument.h"

using namespace std;

const int PageBorderWidth = 2;

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

DjvuPageGraphicsItem::DjvuPageGraphicsItem(
    ddjvu_document_t * const document,
    const int pageNum,
    const qreal scale):
    document(document), pageNum(pageNum), scale(scale)
{

}

QRectF DjvuPageGraphicsItem::boundingRect() const
{
    ddjvu_page_t * page = ddjvu_page_create_by_pageno(
        document,
        pageNum);

    assert(page);

    ddjvu_context_t * context = DjvuDocument::getContext();

    while(!ddjvu_page_decoding_done(page))
        handle_ddjvu_messages(context, true);

    int w = ddjvu_page_get_width(page);
    int h = ddjvu_page_get_height(page);

    return QRectF(- w/2 - PageBorderWidth/2, -h/2 - PageBorderWidth/2, w, h);
}

void DjvuPageGraphicsItem::paint(
    QPainter *painter,
    [[maybe_unused]]  const QStyleOptionGraphicsItem * option,
    [[maybe_unused]] QWidget * widget)
{
    ddjvu_page_t * page = ddjvu_page_create_by_pageno(
        document,
        pageNum);
    assert(page);

    ddjvu_context_t * context = DjvuDocument::getContext();

    while(!ddjvu_page_decoding_done(page))
        handle_ddjvu_messages(context, true);

    int w = ddjvu_page_get_width(page);
    int h = ddjvu_page_get_height(page);

    ddjvu_render_mode_t mode = DDJVU_RENDER_COLOR;
    ddjvu_rect_t pagerect = {0,0, static_cast<unsigned int>(w), static_cast<unsigned int>(h)};
    ddjvu_rect_t renderrect = {0,0, static_cast<unsigned int>(w), static_cast<unsigned int>(h)};
    unsigned int masks[] = { 0xFF0000, 0x00FF00, 0x0000FF };

    ddjvu_format_t * pixelformat = ddjvu_format_create(
        DDJVU_FORMAT_RGBMASK32,
        3,
        masks);
    ddjvu_format_set_row_order(pixelformat, true);
    ddjvu_format_set_y_direction(pixelformat, true);

    char * imagebuffer = new char[w * 4 * h];

    int r = ddjvu_page_render(
        page,
        mode,
        &pagerect,
        &renderrect,
        pixelformat,
        w * 4,
        imagebuffer);
    assert(r);

    // Cleanup function, delete image buffer, when it will become no needed.
    auto cleanupFunction = [] (void * info) {
        char * imagebuffer = reinterpret_cast<char*>(info);
        delete [] imagebuffer;
    };

    QImage image = QImage(
        reinterpret_cast<unsigned char *>(imagebuffer),
        w,
        h,
        w * 4,
        QImage::Format_RGB32,
        cleanupFunction,
        imagebuffer);

    QPixmap pixmap = QPixmap::fromImage(image);

    painter->drawPixmap(-w/2, -h/2, pixmap);

    // Draw page border
    QPen pen;
    pen.setWidth(PageBorderWidth);
    painter->setPen(pen);
    painter->drawRect(-w/2, -h/2, w, h);
}

qreal DjvuPageGraphicsItem::sceneYTop() const
{
    qreal y0 = scenePos().y();
    QRectF rect = boundingRect();

    return y0 - rect.height()/2;
}

int DjvuPageGraphicsItem::getPageNum() const
{
    return pageNum;
}
