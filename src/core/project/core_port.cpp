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
#include "core_port.h"

#include "core_project.h"

mbCorePort::Strings::Strings() :
    name(QStringLiteral("name")),
    type(QStringLiteral("type"))

{
}

const mbCorePort::Strings &mbCorePort::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCorePort::Defaults::Defaults() :
    name(QStringLiteral("Port")),
    type(Modbus::TCP)
{
}

const mbCorePort::Defaults &mbCorePort::Defaults::instance()
{
    static const Defaults d;
    return d;
}

mbCorePort::CoreStatistics::CoreStatistics() :
    mb::BaseStatistics()
{
    countTx             = 0;
    countRx             = 0;
    countBadTimeout     = 0;
    countBadCRC         = 0;
    cycleCount          = 0;
    cycleSumDuration    = 0;
    cycleLastDuration   = 0;
    cycleMinDuration    = UINT32_MAX;
    cycleMaxDuration    = 0;
    cycleAvgDuration    = 0;
}

mbCorePort::mbCorePort(QObject *parent)
    : QObject{parent}
{
    const Modbus::Defaults &d = Modbus::Defaults::instance();

    m_project = nullptr;

    // common
    m_settings.type         = d.type;
    // tcp
    m_settings.port         = d.port;
    m_settings.timeout      = d.timeout;
    m_settings.maxconn      = d.maxconn;
    // serial
    //m_settings.serialPortName = dSerial.serialPortName;
    m_settings.baudRate     = d.baudRate;
    m_settings.dataBits     = d.dataBits;
    m_settings.parity       = d.parity;
    m_settings.stopBits     = d.stopBits;
    m_settings.flowControl  = d.flowControl;
    m_settings.timeoutFB    = d.timeoutFirstByte;
    m_settings.timeoutIB    = d.timeoutInterByte;
    // common
    m_settings.isBroadcastEnabled = d.isBroadcastEnabled;

}

mbCorePort::~mbCorePort()
{
    delete m_stat;
}

void mbCorePort::setProjectCore(mbCoreProject *project)
{
    m_project = project;
}

void mbCorePort::setName(const QString &name)
{
    QString tn = name;
    if (tn.isEmpty())
        tn = Defaults::instance().name;
    if (m_project && m_project->portCore(tn) != this)
    {
        if (m_project->hasPort(tn))
            tn = m_project->freePortName(tn);
        m_project->portRename(this, tn);
    }
    setObjectName(tn);
    Q_EMIT changed();
    Q_EMIT nameChanged(tn);
}

MBSETTINGS mbCorePort::settings() const
{
    const Strings &sPort = Strings::instance();
    const Modbus::Strings &s = Modbus::Strings::instance();

    MBSETTINGS r;
    // common
    r.insert(sPort.name, name());
    r.insert(sPort.type, Modbus::toString(type()));
    // tcp
    r.insert(s.host   , m_settings.host   );
    r.insert(s.port   , m_settings.port   );
    r.insert(s.timeout, m_settings.timeout);
    r.insert(s.maxconn, m_settings.maxconn);
    // serial
    r.insert(s.serialPortName  , m_settings.serialPortName);
    r.insert(s.baudRate        , m_settings.baudRate);
    r.insert(s.dataBits        , m_settings.dataBits);
    r.insert(s.parity          , Modbus::toString(m_settings.parity     ));
    r.insert(s.stopBits        , Modbus::toString(m_settings.stopBits   ));
    r.insert(s.flowControl     , Modbus::toString(m_settings.flowControl));
    r.insert(s.timeoutFirstByte, m_settings.timeoutFB);
    r.insert(s.timeoutInterByte, m_settings.timeoutIB);
    // common
    r.insert(s.isBroadcastEnabled, m_settings.isBroadcastEnabled);
    return r;
}

bool mbCorePort::setSettings(const MBSETTINGS &settings)
{
    const Strings &sPort = Strings::instance();
    const Modbus::Strings &s = Modbus::Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    bool ok;

    // common
    it = settings.find(sPort.name);
    if (it != end)
    {
        QVariant var = it.value();
        setName(var.toString());
    }

    it = settings.find(sPort.type);
    if (it != end)
    {
        QVariant var = it.value();
        Modbus::ProtocolType v = Modbus::toProtocolType(var, &ok);
        if (ok)
            setType(v);
    }

    // tcp
    it = settings.find(s.host);
    if (it != end)
    {
        QVariant var = it.value();
        setHost(var.toString());
    }

    it = settings.find(s.port);
    if (it != end)
    {
        QVariant var = it.value();
        uint16_t v = static_cast<uint16_t>(var.toUInt(&ok));
        if (ok)
            setPort(v);
    }

    it = settings.find(s.timeout);
    if (it != end)
    {
        QVariant var = it.value();
        uint32_t v = static_cast<uint32_t>(var.toUInt(&ok));
        if (ok)
            setTimeout(v);
    }

    it = settings.find(s.maxconn);
    if (it != end)
    {
        QVariant var = it.value();
        uint32_t v = static_cast<uint32_t>(var.toUInt(&ok));
        if (ok)
            setMaxConnections(v);
    }

    // serial
    it = settings.find(s.serialPortName);
    if (it != end)
    {
        QVariant var = it.value();
        setSerialPortName(var.toString());
    }

    it = settings.find(s.baudRate);
    if (it != end)
    {
        QVariant var = it.value();
        int32_t v = static_cast<int32_t>(var.toInt(&ok));
        if (ok)
            setBaudRate(v);
    }

    it = settings.find(s.dataBits);
    if (it != end)
    {
        QVariant var = it.value();
        int8_t v = Modbus::toDataBits(var, &ok);
        if (ok)
            setDataBits(v);
    }

    it = settings.find(s.parity);
    if (it != end)
    {
        QVariant var = it.value();
        Modbus::Parity v = Modbus::toParity(var, &ok);
        if (ok)
            setParity(v);
    }

    it = settings.find(s.stopBits);
    if (it != end)
    {
        QVariant var = it.value();
        Modbus::StopBits v = Modbus::toStopBits(var, &ok);
        if (ok)
            setStopBits(v);
    }

    it = settings.find(s.flowControl);
    if (it != end)
    {
        QVariant var = it.value();
        Modbus::FlowControl v = Modbus::toFlowControl(var, &ok);
        if (ok)
            setFlowControl(v);
    }

    it = settings.find(s.timeoutFirstByte);
    if (it != end)
    {
        QVariant var = it.value();
        uint32_t v = static_cast<uint32_t>(var.toUInt(&ok));
        if (ok)
            setTimeoutFirstByte(v);
    }

    it = settings.find(s.timeoutInterByte);
    if (it != end)
    {
        QVariant var = it.value();
        uint32_t v = static_cast<uint32_t>(var.toUInt(&ok));
        if (ok)
            setTimeoutInterByte(v);
    }

    // common
    {
        bool v = Modbus::getSettingBroadcastEnabled(settings, &ok);
        if (ok)
            setBroadcastEnabled(v);
    }
    Q_EMIT changed();
    return true;
}

void mbCorePort::resetStatistics()
{
    m_statLock.lockForWrite();
    const auto countTxOld = m_stat->countTx;
    const auto countRxOld = m_stat->countRx;
    resetStatisticsInner();
    const auto countTxNew = m_stat->countTx;
    const auto countRxNew = m_stat->countRx;
    m_statLock.unlock();

    if (countTxOld != countTxNew)
        Q_EMIT statCountTxChanged(countTxNew);
    if (countRxOld != countRxNew)
        Q_EMIT statCountRxChanged(countRxNew);

}

void mbCorePort::incStatCountTx()
{
    m_statLock.lockForWrite();
    auto v = ++m_stat->countTx;
    m_statLock.unlock();
    Q_EMIT statCountTxChanged(v);
}

void mbCorePort::incStatCountRx()
{
    m_statLock.lockForWrite();
    auto v = ++m_stat->countRx;
    m_statLock.unlock();
    Q_EMIT statCountRxChanged(v);
}

void mbCorePort::setStatCycleTime(quint64 time)
{
    QWriteLocker locker(&m_statLock);
    m_stat->cycleCount++;
    m_stat->cycleSumDuration += time;
    m_stat->cycleAvgDuration = static_cast<uint32_t>(m_stat->cycleSumDuration / m_stat->cycleCount);
    m_stat->cycleLastDuration = static_cast<uint32_t>(time);
    if (time < m_stat->cycleMinDuration)
        m_stat->cycleMinDuration = static_cast<uint32_t>(time);
    if (time > m_stat->cycleMaxDuration)
        m_stat->cycleMaxDuration = static_cast<uint32_t>(time);

    setStatCycleTimeInner(time);
}

void mbCorePort::setStatStatus(Modbus::StatusCode status, mb::Timestamp_t timestamp, const QString &err)
{
    QWriteLocker locker(&m_statLock);
    m_stat->lastStatus = status;
    m_stat->lastTimestamp = timestamp;
    if (Modbus::StatusIsGood(status))
    {
        m_stat->countGood++;
        m_stat->lastSuccessTimestamp = timestamp;
    }
    else
    {
        m_stat->countBad++;
        switch (status)
        {
        case Modbus::Status_BadSerialReadTimeout:
        case Modbus::Status_BadTcpReadTimeout:
        case Modbus::Status_BadUdpReadTimeout:
            m_stat->countBadTimeout++;
            break;
        case Modbus::Status_BadCrc:
        case Modbus::Status_BadLrc:
            m_stat->countBadCRC++;
            break;
        default:
            m_stat->countBadStandard += Modbus::StatusIsStandardError(status);
            break;
        }
        m_stat->lastErrorStatus = status;
        m_stat->lastErrorTimestamp = timestamp;
        m_stat->lastErrorText = err;
    }
    setStatStatusInner(status, timestamp, err);
}

void mbCorePort::resetStatisticsInner()
{
    *m_stat = CoreStatistics();
}

void mbCorePort::setStatCycleTimeInner(quint64 /*time*/)
{
    // Note: Base implementation does nothing
}

void mbCorePort::setStatStatusInner(Modbus::StatusCode /*status*/, mb::Timestamp_t /*timestamp*/, const QString &/*err*/)
{
    // Note: Base implementation does nothing
}
