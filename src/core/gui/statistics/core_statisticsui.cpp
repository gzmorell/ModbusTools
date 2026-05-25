#include "core_statisticsui.h"

mbCoreStatisticsUi::mbCoreStatisticsUi(QWidget *parent) :
    QWidget(parent)
{
}

void mbCoreStatisticsUi::setStatWindowTitle(const QString &name)
{
    QString title = QString("%1 [Stat]").arg(name);
    setWindowTitle(title);
    Q_EMIT statWindowTitleChanged(title);
}
