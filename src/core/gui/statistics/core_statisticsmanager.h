/*
    Modbus Tools

    Created: 2026
    Author: Serhii Marchuk, https://github.com/serhmarch

    Copyright (C) 2026  Serhii Marchuk

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
#ifndef CORE_STATISTICSMANAGER_H
#define CORE_STATISTICSMANAGER_H

#include <QHash>
#include <QObject>

#include <mbcore.h>

class mbCoreProject;
class mbCorePort;
class mbCoreDevice;

class mbCoreStatisticsUi;
class mbCorePortStatisticsUi;
class mbCoreDeviceStatisticsUi;

class MBTOOLS_EXPORT mbCoreStatisticsManager : public QObject
{
    Q_OBJECT

public:
    explicit mbCoreStatisticsManager(QObject *parent = nullptr);

public: // project
    inline mbCoreProject *projectCore() const { return m_project; }
    QList<mbCoreStatisticsUi*> statisticsUisCore() const;
    inline mbCoreStatisticsUi *activeStatisticsUiCore() const { return m_activeStatisticsUi; }
    virtual void removeStatisticsUi(mbCoreStatisticsUi *ui);

public: // port statistics
    inline QList<mbCorePortStatisticsUi*> portStatisticsUisCore() const { return m_portStatisticsUis; }
    inline bool hasPortStatisticsUi(const mbCorePort *port) const { return m_hashPortStatisticsUis.contains(port); }
    inline int portStatisticsCount() const { return m_hashPortStatisticsUis.count(); }
    inline mbCorePortStatisticsUi *portStatisticsUiCore(mbCorePort *port) const { return m_hashPortStatisticsUis.value(port, nullptr); }
    mbCorePortStatisticsUi *portStatisticsUiCore(const QString &name) const;
    void addPortStatisticsUi(mbCorePort *port);
    mbCorePortStatisticsUi *getOrCreatePortStatisticsUiCore(mbCorePort *port);
    mbCorePortStatisticsUi *getOrCreatePortStatisticsUiCore(const QString &name);

public: // device statistics
    inline QList<mbCoreDeviceStatisticsUi*> deviceStatisticsUisCore() const { return m_deviceStatisticsUis; }
    inline bool hasDeviceStatisticsUi(const mbCoreDevice *device) const { return m_hashDeviceStatisticsUis.contains(device); }
    inline int deviceStatisticsCount() const { return m_hashDeviceStatisticsUis.count(); }
    inline mbCoreDeviceStatisticsUi *deviceStatisticsUiCore(mbCoreDevice *device) const { return m_hashDeviceStatisticsUis.value(device, nullptr); }
    mbCoreDeviceStatisticsUi *deviceStatisticsUiCore(const QString &name) const;
    void addDeviceStatisticsUi(mbCoreDevice *device);
    mbCoreDeviceStatisticsUi *getOrCreateDeviceStatisticsUiCore(mbCoreDevice *device);
    mbCoreDeviceStatisticsUi *getOrCreateDeviceStatisticsUiCore(const QString &name);

Q_SIGNALS:
    void statisticsUiAdd(mbCoreStatisticsUi *ui);
    void statisticsUiRemove(mbCoreStatisticsUi *ui);
    void statisticsUiActivated(mbCoreStatisticsUi*);

public Q_SLOTS:
    void setActiveStatisticsUi(mbCoreStatisticsUi *ui);

protected Q_SLOTS:
    virtual void setProject(mbCoreProject *project);
    virtual void portStatisticsAdd(mbCorePort *port);
    virtual void portStatisticsRemove(mbCorePort *port);
    virtual void deviceStatisticsAdd(mbCoreDevice *device);
    virtual void deviceStatisticsRemove(mbCoreDevice *device);

protected:
    virtual mbCorePortStatisticsUi *createPortStatisticsUi(mbCorePort *port) = 0;
    virtual mbCoreDeviceStatisticsUi *createDeviceStatisticsUi(mbCoreDevice *device) = 0;

protected:
    mbCoreProject *m_project;
    mbCoreStatisticsUi *m_activeStatisticsUi;

    typedef QList<mbCorePortStatisticsUi*> PortStatisticsUis_t;
    typedef QHash<const mbCorePort*, mbCorePortStatisticsUi*> HashPortStatisticsUis_t;
    PortStatisticsUis_t m_portStatisticsUis;
    HashPortStatisticsUis_t m_hashPortStatisticsUis;

    typedef QList<mbCoreDeviceStatisticsUi*> DeviceStatisticsUis_t;
    typedef QHash<const mbCoreDevice*, mbCoreDeviceStatisticsUi*> HashDeviceStatisticsUis_t;
    DeviceStatisticsUis_t m_deviceStatisticsUis;
    HashDeviceStatisticsUis_t m_hashDeviceStatisticsUis;
};

#endif // CORE_STATISTICSMANAGER_H
