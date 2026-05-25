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
#include "server_statisticsmanager.h"

#include <project/server_project.h>
#include <project/server_port.h>
#include <project/server_device.h>

#include "server_portstatisticsui.h"
#include "server_devicestatisticsui.h"

mbServerStatisticsManager::mbServerStatisticsManager(QObject *parent) :
    mbCoreStatisticsManager(parent)
{
}

void mbServerStatisticsManager::setProject(mbCoreProject *project)
{
    mbCoreStatisticsManager::setProject(project);
    //if (this->project() && (this->project()->portCount() == 1))
    //    Q_EMIT maximizeStatisticsUi();
}

mbCorePortStatisticsUi *mbServerStatisticsManager::createPortStatisticsUi(mbCorePort *port)
{
    return new mbServerPortStatisticsUi(static_cast<mbServerPort*>(port), nullptr);
}

mbCoreDeviceStatisticsUi *mbServerStatisticsManager::createDeviceStatisticsUi(mbCoreDevice *device)
{
    return new mbServerDeviceStatisticsUi(static_cast<mbServerDevice*>(device), nullptr);
}
