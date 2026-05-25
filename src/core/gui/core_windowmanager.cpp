/*
    Modbus Tools

    Created: 2023
    Author: Serhii Marchuk, https://github.com/serhmarch

    Copyright (C) 2023  Serhii Marchuk

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "core_windowmanager.h"

#include <QEvent>
#include <QMdiArea>
#include <QMdiSubWindow>

#include <mbcore_binaryreader.h>
#include <mbcore_binarywriter.h>

#include <project/core_project.h>
#include <project/core_dataview.h>

#include "core_ui.h"
#include "dataview/core_dataviewmanager.h"
#include "dataview/core_dataviewui.h"

#include "statistics/core_statisticsmanager.h"
#include "statistics/core_portstatisticsui.h"
#include "statistics/core_devicestatisticsui.h"

mbCoreWindowManager::Strings::Strings() :
    prefixViewMode(QStringLiteral("view:")),
    prefixDataView(QStringLiteral("dat:")),
    prefixPortStatistics(QStringLiteral("pstat:")),
    prefixDeviceStatistics(QStringLiteral("dstat:"))
{
}

const mbCoreWindowManager::Strings &mbCoreWindowManager::Strings::instance()
{
    static Strings s;
    return s;
}

mbCoreWindowManager::mbCoreWindowManager(mbCoreUi *ui, mbCoreDataViewManager *dataViewManager, mbCoreStatisticsManager *statisticsManager) :
    QObject(ui)
{
    m_ui = ui;
    m_dataViewManager = dataViewManager;
    m_statisticsManager = statisticsManager;

    m_area = new QMdiArea(ui);
    m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_area->setTabPosition(QTabWidget::South);
    m_area->setTabsClosable(false);
    m_area->setTabsMovable(true);
    connect(m_area, &QMdiArea::subWindowActivated, this, &mbCoreWindowManager::subWindowActivated);

    connect(mbCore::globalCore(), &mbCore::projectChanged, this, &mbCoreWindowManager::setProject);

    connect(m_dataViewManager, &mbCoreDataViewManager::dataViewUiAdd, this, &mbCoreWindowManager::dataViewUiAdd);
    connect(m_dataViewManager, &mbCoreDataViewManager::dataViewUiRemove, this, &mbCoreWindowManager::dataViewUiRemove);
    Q_FOREACH (mbCoreDataViewUi *ui, m_dataViewManager->dataViewUisCore())
        dataViewUiAdd(ui);

    connect(m_statisticsManager, &mbCoreStatisticsManager::statisticsUiAdd   , this, &mbCoreWindowManager::statisticsUiAdd   );
    connect(m_statisticsManager, &mbCoreStatisticsManager::statisticsUiRemove, this, &mbCoreWindowManager::statisticsUiRemove);
}

mbCoreDataView *mbCoreWindowManager::activeDataViewCore() const
{
    return m_dataViewManager->activeDataViewCore();
}

void mbCoreWindowManager::setActiveDataViewCore(mbCoreDataView *dataView)
{
    mbCoreDataViewUi *ui = m_dataViewManager->dataViewUiCore(dataView);
    if (ui)
    {
        QMdiSubWindow *sw = m_hashWindows.value(ui);
        if (sw)
            m_area->setActiveSubWindow(sw);
    }
}

mbCoreStatisticsUi *mbCoreWindowManager::activeStatisticsUiCore() const
{
    return m_statisticsManager->activeStatisticsUiCore();
}

void mbCoreWindowManager::setActiveStatisticsUi(mbCoreStatisticsUi *ui)
{
    QMdiSubWindow *sw = m_hashWindows.value(ui);
    if (sw)
        m_area->setActiveSubWindow(sw);
}

QMdiSubWindow *mbCoreWindowManager::getMdiSubWindowForNameWithPrefix(const QString &nameWithPrefix)
{
    const Strings &s = Strings::instance();
    if (nameWithPrefix.startsWith(s.prefixDataView))
    {
        mbCoreDataViewUi *ui = m_dataViewManager->dataViewUiCore(nameWithPrefix.mid(s.prefixDataView.size()));
        return m_hashWindows.value(ui);
    }
    else if (nameWithPrefix.startsWith(Strings::instance().prefixPortStatistics))
    {
        mbCorePortStatisticsUi *ui = m_statisticsManager->getOrCreatePortStatisticsUiCore(nameWithPrefix.mid(s.prefixPortStatistics.size()));
        return m_hashWindows.value(ui);
    }
    else if (nameWithPrefix.startsWith(Strings::instance().prefixDeviceStatistics))
    {
        mbCoreDeviceStatisticsUi *ui = m_statisticsManager->getOrCreateDeviceStatisticsUiCore(nameWithPrefix.mid(s.prefixDeviceStatistics.size()));
        return m_hashWindows.value(ui);
    }
    return nullptr;
}

QString mbCoreWindowManager::getMdiSubWindowNameWithPrefix(const QMdiSubWindow *sw) const
{
    if (mbCoreDataViewUi *ui = qobject_cast<mbCoreDataViewUi *>(sw->widget()))
    {
        const Strings &s = Strings::instance();
        return s.prefixDataView+ui->name();
    }
    else if (mbCorePortStatisticsUi *ui = qobject_cast<mbCorePortStatisticsUi *>(sw->widget()))
    {
        const Strings &s = Strings::instance();
        return s.prefixPortStatistics+ui->name();
    }
    else if (mbCoreDeviceStatisticsUi *ui = qobject_cast<mbCoreDeviceStatisticsUi *>(sw->widget()))
    {
        const Strings &s = Strings::instance();
        return s.prefixDeviceStatistics+ui->name();
    }
    return QString();
}

void mbCoreWindowManager::showDataViewUi(mbCoreDataViewUi *ui)
{
    QMdiSubWindow *sw = m_hashWindows.value(ui);
    if (sw)
        showSubWindow(sw);
    else
        dataViewUiAdd(ui);
}

void mbCoreWindowManager::showPortStatistics(mbCorePort *port)
{
    mbCoreStatisticsUi *ui = m_statisticsManager->portStatisticsUiCore(port);
    if (ui)
        setActiveStatisticsUi(ui);
    else
        m_statisticsManager->addPortStatisticsUi(port);
}

void mbCoreWindowManager::showDeviceStatistics(mbCoreDevice *device)
{
    mbCoreStatisticsUi *ui = m_statisticsManager->deviceStatisticsUiCore(device);
    if (ui)
        setActiveStatisticsUi(ui);
    else
        m_statisticsManager->addDeviceStatisticsUi(device);
}

void mbCoreWindowManager::actionWindowViewSubWindow()
{
    setViewMode(QMdiArea::SubWindowView);
}

void mbCoreWindowManager::actionWindowViewTabbed()
{
    setViewMode(QMdiArea::TabbedView);
}

void mbCoreWindowManager::actionWindowDataViewCloseAll()
{
    Q_FOREACH(mbCoreDataViewUi *ui, m_dataViewManager->dataViewUisCore())
    {
        QMdiSubWindow *sw = m_hashWindows.value(ui);
        if (sw)
            closeSubWindow(sw);
    }
}

void mbCoreWindowManager::actionWindowStatisticsCloseAll()
{
    Q_FOREACH(mbCoreStatisticsUi *ui, m_statisticsManager->statisticsUisCore())
    {
        QMdiSubWindow *sw = m_hashWindows.value(ui);
        if (sw)
            closeSubWindow(sw);
    }
}

void mbCoreWindowManager::actionWindowCloseAll()
{
    actionWindowDataViewCloseAll();
    actionWindowStatisticsCloseAll();
}

void mbCoreWindowManager::actionWindowCascade()
{
    if (m_area->viewMode() == QMdiArea::TabbedView)
        actionWindowViewSubWindow();
    m_area->cascadeSubWindows();
}

void mbCoreWindowManager::actionWindowTile()
{
    if (m_area->viewMode() == QMdiArea::TabbedView)
        actionWindowViewSubWindow();
    Q_FOREACH(QMdiSubWindow *sw, m_area->subWindowList())
    {
        sw->show();
        sw->widget()->show();
    }
    m_area->tileSubWindows();
}

QByteArray mbCoreWindowManager::saveWindowsState()
{
    /*
    record := nameWithPrefix + windowState + geometry

    nameWithPrefix := int32 byteCount + byte[byteCount]  // UTF-8
    windowState    := int32                               // Qt::WindowState flags
    geometry       := int32 x + int32 y + int32 w + int32 h    
    */
    const Strings &s = Strings::instance();

    mbCoreBinaryWriter writer;
    QString name = s.prefixViewMode;
    int windowViewMode = this->viewMode();
    QRect dummyGeometry;

    writer.write(name);
    writer.write(windowViewMode);
    writer.write(dummyGeometry);

    Q_FOREACH (QMdiSubWindow *sw, m_area->subWindowList(QMdiArea::StackingOrder))
    {
        saveWindowStateInner(writer, getMdiSubWindowNameWithPrefix(sw), sw);
    }
    return writer.data();
}

void mbCoreWindowManager::saveWindowStateInner(mbCoreBinaryWriter &writer, const QString &nameWithPrefix, const QWidget *w)
{
    writer.write(nameWithPrefix);
    writer.write(static_cast<int>(w->windowState()));
    writer.write(w->geometry());
}

bool mbCoreWindowManager::restoreWindowsState(const QByteArray &v)
{
    /*
    record := nameWithPrefix + windowState + geometry

    nameWithPrefix := int32 byteCount + byte[byteCount]  // UTF-8
    windowState    := int32                               // Qt::WindowState flags
    geometry       := int32 x + int32 y + int32 w + int32 h    
    */
    mbCoreProject *p = mbCore::globalCore()->projectCore();
    if (p)
    {
        mbCoreBinaryReader reader(v);
        QString name;
        int windowViewMode;
        QRect dummyGeometry;
        // Try to retrieve view mode
        if (reader.read(name) && reader.read(windowViewMode) && reader.read(dummyGeometry))
        {
            const Strings &s = Strings::instance();

            if (name == s.prefixViewMode)
            {
                switch (windowViewMode)
                {
                case QMdiArea::TabbedView:
                    this->setViewMode(QMdiArea::TabbedView);
                    break;
                default:
                    this->setViewMode(QMdiArea::SubWindowView);
                    break;
                }
            }
            else // For backward compatibility with v0.4 and older
                reader.reset();
        }
        while (reader.isProcessing())
        {
            if (!restoreWindowStateInner(reader))
                return false;
        }
    }
    return false;
}

bool mbCoreWindowManager::restoreWindowStateInner(mbCoreBinaryReader &reader)
{
    QString nameWithPrefix;
    int windowState;
    QRect geometry;
    if (reader.read(nameWithPrefix) && reader.read(windowState) && reader.read(geometry))
    {
        QMdiSubWindow* sw = getMdiSubWindowForNameWithPrefix(nameWithPrefix);
        if (sw)
        {
            windowState &= ~Qt::WindowActive;
            sw->setWindowState(static_cast<Qt::WindowState>(windowState));
            sw->raise();
            sw->setGeometry(geometry);
        }
    }
    else
        return false;
    return true;
}

void mbCoreWindowManager::setViewMode(QMdiArea::ViewMode viewMode)
{
    if (m_area->viewMode() != viewMode)
    {
        m_area->setViewMode(viewMode);
        QIcon icon = (viewMode == QMdiArea::TabbedView) ? QIcon() : m_area->windowIcon();
        Q_FOREACH(QMdiSubWindow *sw, m_hashWindows)
            sw->setWindowIcon(icon);
        Q_EMIT viewModeChanged(viewMode);
    }
}

QMdiSubWindow *mbCoreWindowManager::subWindowAdd(QWidget *ui)
{
    QMdiSubWindow* sw = new QMdiSubWindow(m_area);
    //sw->setWindowIcon(QIcon());
    //sw->setAttribute(Qt::WA_DeleteOnClose, false);
    sw->setWidget(ui);
    sw->setWindowFlag(Qt::CustomizeWindowHint, false);
    QIcon icon = (viewMode() == QMdiArea::TabbedView) ? QIcon() : m_area->windowIcon();
    sw->setWindowIcon(icon);
    m_hashWindows.insert(ui, sw);
    m_area->addSubWindow(sw);
    sw->installEventFilter(this);
    sw->show();
    return sw;
}

QMdiSubWindow *mbCoreWindowManager::subWindowRemove(QWidget *ui)
{
    QMdiSubWindow* sw = m_hashWindows.value(ui, nullptr);
    if (sw)
    {
        m_hashWindows.remove(ui);
        if (m_area->activeSubWindow() == sw)
            m_area->activateNextSubWindow();
        m_area->removeSubWindow(sw);
        sw->setWidget(nullptr);
        ui->setParent(nullptr);
    }
    return sw;
}

void mbCoreWindowManager::setProject(mbCoreProject *p)
{
    if (p)
        restoreWindowsState(p->windowsData());
}

void mbCoreWindowManager::dataViewUiAdd(mbCoreDataViewUi *ui)
{
    QMdiSubWindow* sw = subWindowAdd(ui);
    connect(ui, &mbCoreDataViewUi::nameChanged, sw, &QWidget::setWindowTitle);
    sw->setWindowTitle(ui->name());
}

void mbCoreWindowManager::dataViewUiRemove(mbCoreDataViewUi *ui)
{
    ui->disconnect(this);
    QMdiSubWindow* sw = subWindowRemove(ui);
    if (sw)
    {
        sw->deleteLater();
    }
}

void mbCoreWindowManager::statisticsUiAdd(mbCoreStatisticsUi *ui)
{
    QMdiSubWindow* sw = subWindowAdd(ui);
    QString title = ui->windowTitle();
    sw->setWindowTitle(title);
    //connect(ui, &mbCoreDataViewUi::nameChanged, sw, &QWidget::setWindowTitle);
    //sw->setWindowTitle(ui->name());
}

void mbCoreWindowManager::statisticsUiRemove(mbCoreStatisticsUi *ui)
{
    ui->disconnect(this);
    QMdiSubWindow* sw = subWindowRemove(ui);
    if (sw)
    {
        sw->deleteLater();
    }
}

void mbCoreWindowManager::subWindowActivated(QMdiSubWindow *sw)
{
    if (sw)
    {
        QWidget *w = sw->widget();
        if (mbCoreDataViewUi *ui = qobject_cast<mbCoreDataViewUi*>(w))
        {
            ui->blockSignals(true);
            m_dataViewManager->setActiveDataViewUi(ui);
            ui->blockSignals(false);
            return;
        }
    }
}

void mbCoreWindowManager::showSubWindow(QMdiSubWindow *sw)
{
    sw->show();
    sw->widget()->show();
    m_area->setActiveSubWindow(sw);
}

void mbCoreWindowManager::closeSubWindow(QMdiSubWindow *sw)
{
    if (mbCoreStatisticsUi *ui = qobject_cast<mbCoreStatisticsUi*>(sw->widget()))
        m_statisticsManager->removeStatisticsUi(ui);
    else
    {
        subWindowRemove(sw->widget());
        sw->deleteLater();
    }
}

bool mbCoreWindowManager::eventFilter(QObject *obj, QEvent *e)
{
    switch (e->type())
    {
    case QEvent::Close:
    {
        QMdiSubWindow *sw = qobject_cast<QMdiSubWindow*>(obj);
        //Q_ASSERT (sw != nullptr);
        closeSubWindow(sw);
        return true;
    }
    default:
        break;
    }
    return QObject::eventFilter(obj, e);
}

