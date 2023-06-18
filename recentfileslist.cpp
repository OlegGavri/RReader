#include "recentfileslist.h"

RecentFilesList::RecentFilesList()
{

}

bool RecentFilesList::exist(QString text)
{
    for(int i = 0; i < size(); i++)
    {
        QAction * action = at(i);
        if(action->text() == text)
            return true;
    }

    return false;
}
