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
#ifndef CORE_WINDOWMANAGER_H
#define CORE_WINDOWMANAGER_H

#include <QHash>
#include <QMdiArea>

#include <mbcore.h>

class QMdiSubWindow;

class mbCoreBinaryReader;
class mbCoreBinaryWriter;

class mbCoreProject;
class mbCorePort;
class mbCoreDevice;
class mbCoreDataView;

class mbCoreUi;
class mbCoreDataViewManager;
class mbCoreDataViewUi;

class mbCoreStatisticsManager;
class mbCoreStatisticsUi;

class MBTOOLS_EXPORT mbCoreWindowManager : public QObject
{
    Q_OBJECT

public:
    struct MBTOOLS_EXPORT Strings
    {
        const QString prefixViewMode;
        const QString prefixDataView;
        const QString prefixPortStatistics;
        const QString prefixDeviceStatistics;

        Strings();
        static const Strings &instance();
    };

public:
    explicit mbCoreWindowManager(mbCoreUi *ui, mbCoreDataViewManager *dataViewManager, mbCoreStatisticsManager *statisticsManager);

public:
    inline QWidget *centralWidget() const { return m_area; }
    inline QMdiArea::ViewMode viewMode() const { return m_area->viewMode(); }

public:
    inline mbCoreUi *uiCore() const { return m_ui; }

public:
    inline mbCoreDataViewManager *dataViewManagerCore() const { return m_dataViewManager; }
    mbCoreDataView *activeDataViewCore() const;
    void setActiveDataViewCore(mbCoreDataView *dataView);
    inline void setActiveDataView(mbCoreDataView *dataView) { setActiveDataViewCore(dataView); }

public:
    inline mbCoreStatisticsManager *statisticsManagerCore() const { return m_statisticsManager; }
    mbCoreStatisticsUi *activeStatisticsUiCore() const;
    void setActiveStatisticsUi(mbCoreStatisticsUi *ui);

public:
    virtual QMdiSubWindow *getMdiSubWindowForNameWithPrefix(const QString &name);
    virtual QString getMdiSubWindowNameWithPrefix(const QMdiSubWindow *sw) const;

public Q_SLOTS:
    void showDataViewUi(mbCoreDataViewUi *ui);
    void showPortStatistics(mbCorePort *port);
    void showDeviceStatistics(mbCoreDevice *device);
    void actionWindowViewSubWindow();
    void actionWindowViewTabbed();
    void actionWindowDataViewCloseAll();
    void actionWindowStatisticsCloseAll();
    virtual void actionWindowCloseAll();
    void actionWindowCascade();
    void actionWindowTile();

public:
    QByteArray saveWindowsState();
    bool restoreWindowsState(const QByteArray &v);

protected:
    void setViewMode(QMdiArea::ViewMode viewMode);
    QMdiSubWindow *subWindowAdd(QWidget *ui);
    QMdiSubWindow *subWindowRemove(QWidget *ui);
    void saveWindowStateInner(mbCoreBinaryWriter &writer, const QString &nameWithPrefix, const QWidget *w);
    bool restoreWindowStateInner(mbCoreBinaryReader &reader);

Q_SIGNALS:
    void viewModeChanged(int viewMode);

protected Q_SLOTS:
    void setProject(mbCoreProject *p);
    void dataViewUiAdd(mbCoreDataViewUi *ui);
    void dataViewUiRemove(mbCoreDataViewUi *ui);
    void statisticsUiAdd(mbCoreStatisticsUi *ui);
    void statisticsUiRemove(mbCoreStatisticsUi *ui);

protected Q_SLOTS:
    virtual void subWindowActivated(QMdiSubWindow *sw);

protected:
    void showSubWindow(QMdiSubWindow *sw);
    virtual void closeSubWindow(QMdiSubWindow *sw);
    bool eventFilter(QObject *obj, QEvent *e) override;

protected:
    QMdiArea *m_area;
    mbCoreUi *m_ui;
    mbCoreDataViewManager *m_dataViewManager;

    mbCoreStatisticsManager *m_statisticsManager;

    // Windows
    typedef QHash<const QWidget*, QMdiSubWindow*> HashWindows_t;
    HashWindows_t m_hashWindows;
};

#endif // CORE_WINDOWMANAGER_H
