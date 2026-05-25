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
#include "core_statisticsmanager.h"

#include <QMdiArea>
#include <QMdiSubWindow>

#include <core.h>

#include <project/core_project.h>
#include <project/core_port.h>
#include <project/core_dataview.h>

#include "core_portstatisticsui.h"
#include "core_devicestatisticsui.h"

mbCoreStatisticsManager::mbCoreStatisticsManager(QObject *parent) : QObject(parent)
{
    m_project = nullptr;
    m_activeStatisticsUi = nullptr;

    mbCore *core = mbCore::globalCore();
    connect(core, &mbCore::projectChanged, this, &mbCoreStatisticsManager::setProject);
}

QList<mbCoreStatisticsUi *> mbCoreStatisticsManager::statisticsUisCore() const
{
    QList<mbCoreStatisticsUi*> list;
    Q_FOREACH (mbCorePortStatisticsUi* ui, m_portStatisticsUis)
        list.append(ui);
    Q_FOREACH (mbCoreDeviceStatisticsUi* ui, m_deviceStatisticsUis)
        list.append(ui);
    return list;
}

void mbCoreStatisticsManager::removeStatisticsUi(mbCoreStatisticsUi *ui)
{
    if (mbCorePortStatisticsUi *portUi = qobject_cast<mbCorePortStatisticsUi*>(ui))
    {
        portStatisticsRemove(portUi->portCore());
    }
    else if (mbCoreDeviceStatisticsUi *deviceUi = qobject_cast<mbCoreDeviceStatisticsUi*>(ui))
    {
        deviceStatisticsRemove(deviceUi->deviceCore());
    }
}

mbCorePortStatisticsUi *mbCoreStatisticsManager::portStatisticsUiCore(const QString &name) const
{
    if (m_project)
    {
        mbCorePort *p = m_project->portCore(name);
        return m_hashPortStatisticsUis.value(p, nullptr);
    }
    return nullptr;
}

void mbCoreStatisticsManager::addPortStatisticsUi(mbCorePort *port)
{
    portStatisticsAdd(port);
}

mbCorePortStatisticsUi *mbCoreStatisticsManager::getOrCreatePortStatisticsUiCore(mbCorePort *port)
{
    mbCorePortStatisticsUi* ui = portStatisticsUiCore(port);
    if (!ui)
    {
        portStatisticsAdd(port);
        ui = portStatisticsUiCore(port);
    }
    return ui;
}
    
mbCorePortStatisticsUi *mbCoreStatisticsManager::getOrCreatePortStatisticsUiCore(const QString &name)
{
    if (m_project)
    {
        mbCorePort *p = m_project->portCore(name);
        if (p)
            return getOrCreatePortStatisticsUiCore(p);
    }
    return nullptr;
}

void mbCoreStatisticsManager::addDeviceStatisticsUi(mbCoreDevice *device)
{
    deviceStatisticsAdd(device);
}

mbCoreDeviceStatisticsUi *mbCoreStatisticsManager::getOrCreateDeviceStatisticsUiCore(mbCoreDevice *device)
{
    mbCoreDeviceStatisticsUi* ui = deviceStatisticsUiCore(device);
    if (!ui)
    {
        deviceStatisticsAdd(device);
        ui = deviceStatisticsUiCore(device);
    }
    return ui;
}
    
mbCoreDeviceStatisticsUi *mbCoreStatisticsManager::getOrCreateDeviceStatisticsUiCore(const QString &name)
{
    if (m_project)
    {
        mbCoreDevice *d = m_project->deviceCore(name);
        if (d)
            return getOrCreateDeviceStatisticsUiCore(d);
    }
    return nullptr;
}

mbCoreDeviceStatisticsUi *mbCoreStatisticsManager::deviceStatisticsUiCore(const QString &name) const
{
    if (m_project)
    {
        mbCoreDevice *d = m_project->deviceCore(name);
        return m_hashDeviceStatisticsUis.value(d, nullptr);
    }
    return nullptr;
}

void mbCoreStatisticsManager::setActiveStatisticsUi(mbCoreStatisticsUi *ui)
{
    // TODO: ASSERT m_hashStatisticsUis.contains(ui->statistics())
    if (m_activeStatisticsUi != ui)
    {
        m_activeStatisticsUi = ui;
        Q_EMIT statisticsUiActivated(ui);
    }
}

void mbCoreStatisticsManager::setProject(mbCoreProject *p)
{
    mbCoreProject *project = static_cast<mbCoreProject*>(p);
    if (m_project != project)
    {
        if (m_project)
        {
            Q_FOREACH (mbCorePort* p, m_project->portsCore())
                portStatisticsRemove(p);
            Q_FOREACH (mbCoreDevice* d, m_project->devicesCore())
                deviceStatisticsRemove(d);
            m_project->disconnect(this);
            m_activeStatisticsUi = nullptr;
        }
        m_project = project;
        if (m_project)
        {
            connect(m_project, &mbCoreProject::portRemoving, this, &mbCoreStatisticsManager::portStatisticsRemove);
        }
    }
}

void mbCoreStatisticsManager::portStatisticsAdd(mbCorePort *port)
{
    mbCorePortStatisticsUi *ui = createPortStatisticsUi(port);
    m_portStatisticsUis.append(ui);
    m_hashPortStatisticsUis.insert(port, ui);
    Q_EMIT statisticsUiAdd(ui);
}

void mbCoreStatisticsManager::portStatisticsRemove(mbCorePort *port)
{
    mbCorePortStatisticsUi *ui = portStatisticsUiCore(port);
    if (!ui)
        return;
    if (m_activeStatisticsUi == ui)
        m_activeStatisticsUi = nullptr;
    ui->disconnect(this);
    Q_EMIT statisticsUiRemove(ui);
    m_portStatisticsUis.removeOne(ui);
    m_hashPortStatisticsUis.remove(port);
    ui->deleteLater(); // Note: need because 'ui' can have 'QMenu'-children located in stack which is trying to delete
}

void mbCoreStatisticsManager::deviceStatisticsAdd(mbCoreDevice *device)
{
    mbCoreDeviceStatisticsUi *ui = createDeviceStatisticsUi(device);
    m_deviceStatisticsUis.append(ui);
    m_hashDeviceStatisticsUis.insert(device, ui);
    Q_EMIT statisticsUiAdd(ui);
}

void mbCoreStatisticsManager::deviceStatisticsRemove(mbCoreDevice *device)
{
    mbCoreDeviceStatisticsUi *ui = deviceStatisticsUiCore(device);
    if (!ui)
        return;
    if (m_activeStatisticsUi == ui)
        m_activeStatisticsUi = nullptr;
    ui->disconnect(this);
    Q_EMIT statisticsUiRemove(ui);
    m_deviceStatisticsUis.removeOne(ui);
    m_hashDeviceStatisticsUis.remove(device);
    ui->deleteLater(); // Note: need because 'ui' can have 'QMenu'-children located in stack which is trying to delete
}
