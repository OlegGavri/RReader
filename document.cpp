#include <QFileInfo>

#include "document.h"
#include "pdfdocument.h"
#include "djvudocument.h"

using namespace std;

static QString getFileExtension(const QString fileName)
{
    QFileInfo fileInfo(fileName);
    return fileInfo.completeSuffix();
}

// Create new Document class for fileName file
Document * Document::createDocument(const QString fileName)
{
    const QString extension = getFileExtension(fileName);
    if(extension == "pdf")
        return new PdfDocument(fileName);
    else if(extension == "djvu")
        return new DjvuDocument(fileName);
    else
        throw std::runtime_error("Invalid file format");
}

Document::~Document()
{
}
