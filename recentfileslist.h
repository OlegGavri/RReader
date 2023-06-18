#ifndef RECENTFILESLIST_H
#define RECENTFILESLIST_H

#include <QList>
#include <QAction>

//
// This class hold list of recent files QAction. This is QAction from File menu.
// Implement some methods for work with list
//
class RecentFilesList : public QList<QAction*>
{
public:
    RecentFilesList();

    // Check that QAction with given text already exist in the list
    bool exist(QString);
};

#endif // RECENTFILESLIST_H
