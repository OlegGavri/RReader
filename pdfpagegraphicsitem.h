#ifndef PDFPAGEGRAPHICSITEM_H
#define PDFPAGEGRAPHICSITEM_H

#include <poppler/qt5/poppler-qt5.h>

#include "pagegraphicsitem.h"

//
// This class draw one page of PDF document in scene
//
class PdfPageGraphicsItem : public PageGraphicsItem
{
public:
    PdfPageGraphicsItem(const Poppler::Document * popplerDocument, const int pageNum, const qreal scale);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget) override;

    // Return y coordinate of top boundedRect side in scene coordinate system
    qreal sceneYTop() const override;

    int getPageNum() const override;

private:
    const Poppler::Document * popplerDocument;
    const int pageNum;
    const qreal scale;
};

#endif // PDFPAGEGRAPHICSITEM_H
