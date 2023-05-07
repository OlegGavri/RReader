#include <cassert>

#include <QPainter>

#include "pagegraphicsitem.h"

// Pages border width
const int PageBorderWidth = 2;

PageGraphicsItem::PageGraphicsItem(const Poppler::Document * document, const int pageNum):
    document(document),
    pageNum(pageNum)
{

}

QRectF PageGraphicsItem::boundingRect() const
{
    Poppler::Page * page = document->page(pageNum);
    QSizeF size = page->pageSizeF();

    qreal w = size.width();
    qreal h = size.height();

    return QRectF(- w/2 - PageBorderWidth/2, -h/2 - PageBorderWidth/2, w, h);
}

void PageGraphicsItem::paint(
    QPainter *painter,
    [[maybe_unused]] const QStyleOptionGraphicsItem * option,
    [[maybe_unused]] QWidget * widget)
{
    Poppler::Page * page = document->page(pageNum);
    QSizeF size = page->pageSize();

    qreal w = size.width();
    qreal h = size.height();

    // Draw image
    QImage image = page->renderToImage();
    assert(!image.isNull());

    QPixmap pixmap = QPixmap::fromImage(image);

    painter->drawPixmap(-w/2, -h/2, pixmap);

    // Draw page border
    QPen pen;
    pen.setWidth(PageBorderWidth);
    painter->setPen(pen);
    painter->drawRect(-w/2, -h/2, w, h);
}

qreal PageGraphicsItem::sceneYTop() const
{
    qreal y0 = scenePos().y();
    QRectF rect = boundingRect();

    return y0 - rect.height()/2;
}
