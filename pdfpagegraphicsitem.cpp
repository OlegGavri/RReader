#include <cassert>

#include <QPainter>

#include "pdfpagegraphicsitem.h"

// Pages border width
const int PageBorderWidth = 2;

PdfPageGraphicsItem::PdfPageGraphicsItem( const Poppler::Document * popplerDocument, const int pageNum, const qreal scale):
    popplerDocument(popplerDocument),
    pageNum(pageNum),
    scale(scale)
{

}

QRectF PdfPageGraphicsItem::boundingRect() const
{
    Poppler::Page * page = popplerDocument->page(pageNum);
    QSizeF size = page->pageSizeF() * scale;

    qreal w = size.width();
    qreal h = size.height();

    return QRectF(- w/2 - PageBorderWidth/2, -h/2 - PageBorderWidth/2, w, h);
}

void PdfPageGraphicsItem::paint(
    QPainter *painter,
    [[maybe_unused]] const QStyleOptionGraphicsItem * option,
    [[maybe_unused]] QWidget * widget)
{
    Poppler::Page * page = popplerDocument->page(pageNum);
    QSizeF size = page->pageSize() * scale;

    qreal w = size.width();
    qreal h = size.height();

    // Draw image
    QImage image = page->renderToImage(72.0 * scale, 72.0 * scale);
    assert(!image.isNull());

    QPixmap pixmap = QPixmap::fromImage(image);

    painter->drawPixmap(-w/2, -h/2, pixmap);

    // Draw page border
    QPen pen;
    pen.setWidth(PageBorderWidth);
    painter->setPen(pen);
    painter->drawRect(-w/2, -h/2, w, h);
}

qreal PdfPageGraphicsItem::sceneYTop() const
{
    qreal y0 = scenePos().y();
    QRectF rect = boundingRect();

    return y0 - rect.height()/2;
}

int PdfPageGraphicsItem::getPageNum() const
{
    return pageNum;
}
