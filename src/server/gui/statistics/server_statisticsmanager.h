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
#ifndef CLIENT_STATISTICSMANAGER_H
#define CLIENT_STATISTICSMANAGER_H

#include <core/gui/statistics/core_statisticsmanager.h>

class mbServerProject;
class mbServerPort;
class mbServerPortStatisticsUi;

class mbServerStatisticsManager : public mbCoreStatisticsManager
{
    Q_OBJECT

public:
    explicit mbServerStatisticsManager(QObject *parent = nullptr);

public: // project
    inline mbServerProject *project() const { return reinterpret_cast<mbServerProject*>(projectCore()); }

public: // statistics ui
    inline mbServerPortStatisticsUi *portStatisticsUi(mbServerPort *port) const { return reinterpret_cast<mbServerPortStatisticsUi*>(portStatisticsUiCore(reinterpret_cast<mbCorePort*>(port))); }

protected:
    void setProject(mbCoreProject *project) override;
    mbCorePortStatisticsUi *createPortStatisticsUi(mbCorePort *port) override;
    mbCoreDeviceStatisticsUi *createDeviceStatisticsUi(mbCoreDevice *device) override;

//Q_SIGNALS:
//    void maximizeDataViewUi();
};

#endif // CLIENT_STATISTICSMANAGER_H
