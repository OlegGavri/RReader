#include <stdexcept>

#include <QDir>

#include "settings.h"
#include "documents.h"
#include "document.h"


using namespace std;

const int RecentDocumentsLen = 5;


QList<Document*> Documents::openDocuments;

// Current document number
static int currentNumber = 0;

// Dir where last documents was opened.
static QString lastOpenDir;

// List of recent documents
static QStringList recentDocuments;

void Documents::Init()
{
    //TODO: open last documents
    if(Settings::GetLastOpenDir().has_value())
    {
        lastOpenDir = Settings::GetLastOpenDir().value();
    }
    else
    {
        lastOpenDir = QDir::homePath();
    }

    recentDocuments = Settings::GetRecentDocuments();
}

void Documents::Deinit()
{
    Settings::SetLastOpenDir(lastOpenDir);
    Settings::SetRecentDocuments(recentDocuments);
}

Document & Documents::open(const QString path, int n)
{
    Document * newDocument;

    if(openDocuments.empty())
    {
        newDocument = Document::createDocument(path);
        openDocuments.append(newDocument);
        currentNumber = 0;
    }
    else
    {
        if(n == CURRENT)
        {
            n = currentNumber;
        }

        if(n >= openDocuments.size())
        {
            throw range_error(QString("Try to open document in invalid postion %1, size: %2")
                                  .arg(n)
                                  .arg(openDocuments.size()).toStdString());
        }

        newDocument = Document::createDocument(path);
        openDocuments.insert(n, newDocument);
        currentNumber = n + 1;
    }

    if(recentDocuments.size() < RecentDocumentsLen)
    {
        if(!recentDocuments.contains(path))
            recentDocuments.append(path);
    }
    else
    {
        if(!recentDocuments.contains(path))
        {
            recentDocuments.pop_front();
            recentDocuments.append(path);
        }
    }

    QFileInfo info(path);
    lastOpenDir = info.absolutePath();

    return *newDocument;
}

void Documents::close(int n)
{
    if(n == CURRENT)
    {
        n = currentNumber;
    }

    if(n >= openDocuments.size())
    {
        throw range_error(QString("Try to close document in invalid postion %1(size: %2)")
                            .arg(n)
                              .arg(openDocuments.size())
                              .toStdString());
    }

    Document * document = openDocuments.at(n);
    openDocuments.removeAt(n);

    if(n == currentNumber)
    {
        if(n >= 0)
        {
            currentNumber--;
        }
    }

    delete document;
}

void Documents::closeCurrent() noexcept
{
    close(currentNumber);
}

int Documents::getCount() noexcept
{
    return openDocuments.size();
}

Document & Documents::getDocument(int n)
{
    return *openDocuments[n];
}

Document & Documents::getCurrent() noexcept
{
    return *openDocuments[currentNumber];
}

void Documents::setCurrent(const int n)
{
    currentNumber = n;
}

int Documents::getCurrentNumber()
{
    return currentNumber;
}

void Documents::swap(int i0, int i1)
{
    if(i0 == CURRENT)
        i0 = currentNumber;
    if(i1 == CURRENT)
        i1 = currentNumber;

    if(i0 >= openDocuments.size())
        throw range_error(QString("Try to swap (%1, %2), size: %3")
            .arg(i0)
            .arg(i1)
            .arg(openDocuments.size()).toStdString());

    openDocuments.swapItemsAt(i0, i1);
}

QString Documents::getLastOpenDir()
{
    return lastOpenDir;
}

QStringList Documents::getRecentDocuments()
{
    return recentDocuments;
}
