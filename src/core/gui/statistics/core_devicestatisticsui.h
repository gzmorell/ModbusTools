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
#ifndef CORE_DEVICESTATISTICSUI_H
#define CORE_DEVICESTATISTICSUI_H

#include "core_statisticsui.h"

class QLineEdit;
class QPlainTextEdit;
class mbCoreDevice;

class MBTOOLS_EXPORT mbCoreDeviceStatisticsUi : public mbCoreStatisticsUi
{
    Q_OBJECT
public:
    explicit mbCoreDeviceStatisticsUi(mbCoreDevice *device, QWidget *parent = nullptr);
    ~mbCoreDeviceStatisticsUi();

public:
    inline mbCoreDevice *deviceCore() const { return m_device; }
    QString name() const;

public:
    virtual void syncStatistics();

public Q_SLOTS:
    virtual void resetStatistics();

protected:
    bool event(QEvent *event) override;

protected:
    void syncStatisticsCoreInner();
    inline bool isScanning() const { return m_timerId > 0; }
    void startScanning();
    void stopScanning();

protected:
    mbCoreDevice *m_device;

    struct 
    {
        QLineEdit*      lnSinceTimestamp      ;
        QLineEdit*      lnLastStatus          ;
        QLineEdit*      lnLastTimestamp       ;
        QLineEdit*      lnLastSuccessTimestamp;
        QLineEdit*      lnLastErrorStatus     ;
        QLineEdit*      lnLastErrorTimestamp  ;
        QPlainTextEdit* txtLastErrorText      ;
        QLineEdit*      lnCountTx             ;
        QLineEdit*      lnCountRx             ;
        QLineEdit*      lnCountGood           ;
        QLineEdit*      lnCountBad            ;
        QLineEdit*      lnCountBadStandard    ;
    } m_ui;

protected:
    int m_timerId;
};

#endif // CORE_DEVICESTATISTICSUI_H
