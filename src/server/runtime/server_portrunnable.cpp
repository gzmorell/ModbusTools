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
#include "server_portrunnable.h"

#include <ModbusServerPort.h>
#include <ModbusTcpServer.h>

#include <server.h>

#include <project/server_port.h>

#include "server_rundevice.h"

mbServerPortRunnable::mbServerPortRunnable(mbServerPort *serverPort, const Modbus::Settings &settings, mbServerRunDevice *device, QObject *parent)
    : QObject(parent)
{
    m_port = serverPort;
    m_device = device;
    m_modbusPort = Modbus::createServerPort(device, settings);
    m_modbusPort->setBroadcastEnabled(serverPort->isBroadcastEnabled());

    // units map
    uint8_t unitmap[MB_UNITMAP_SIZE];
    memset(unitmap, 0, MB_UNITMAP_SIZE);
    Q_FOREACH(uint8_t unit , m_device->unitNumbers())
        MB_UNITMAP_SET_BIT(unitmap, unit, true)
    m_modbusPort->setUnitMap(unitmap);

    // Note: m_modbusPort can NOT be nullptr
    switch (m_modbusPort->type())
    {
    case Modbus::RTU:
    case Modbus::UDP:
    case Modbus::RTUvUDP:
        m_modbusPort->connect(&ModbusServerPort::signalTx, this, &mbServerPortRunnable::slotBytesTx);
        m_modbusPort->connect(&ModbusServerPort::signalRx, this, &mbServerPortRunnable::slotBytesRx);
        break;
    case Modbus::ASC:
    case Modbus::ASCvUDP:
        m_modbusPort->connect(&ModbusServerPort::signalTx, this, &mbServerPortRunnable::slotAsciiTx);
        m_modbusPort->connect(&ModbusServerPort::signalRx, this, &mbServerPortRunnable::slotAsciiRx);
        break;
    case Modbus::ASCvTCP:
        m_modbusPort->connect(&ModbusServerPort::signalTx, this, &mbServerPortRunnable::slotAsciiTx);
        m_modbusPort->connect(&ModbusServerPort::signalRx, this, &mbServerPortRunnable::slotAsciiRx);
        m_modbusPort->connect(&ModbusTcpServer::signalNewConnection, this, &mbServerPortRunnable::slotNewConnection);
        m_modbusPort->connect(&ModbusTcpServer::signalCloseConnection, this, &mbServerPortRunnable::slotCloseConnection);
        break;
    default:
        m_modbusPort->connect(&ModbusServerPort::signalTx, this, &mbServerPortRunnable::slotBytesTx);
        m_modbusPort->connect(&ModbusServerPort::signalRx, this, &mbServerPortRunnable::slotBytesRx);
        m_modbusPort->connect(&ModbusTcpServer::signalNewConnection, this, &mbServerPortRunnable::slotNewConnection);
        m_modbusPort->connect(&ModbusTcpServer::signalCloseConnection, this, &mbServerPortRunnable::slotCloseConnection);
        break;
    }

    m_modbusPort->connect(&ModbusServerPort::signalError    , this, &mbServerPortRunnable::slotError    );
    m_modbusPort->connect(&ModbusServerPort::signalCompleted, this, &mbServerPortRunnable::slotCompleted);

    QString name = settings.value(mbServerPort::Strings::instance().name).toString();
    m_modbusPort->setObjectName(name.toUtf8().constData());
    setName(name);
}

mbServerPortRunnable::~mbServerPortRunnable()
{
    delete m_modbusPort;
}

void mbServerPortRunnable::setName(const QString &name)
{
    //m_modbusPort->setObjectName(name.toLatin1().constData());
    setObjectName(name);
}

void mbServerPortRunnable::run()
{
    QElapsedTimer timer;
    timer.start();
    // Main server cycle
    m_modbusPort->process();
    //------------------------------------------
    // Statistics
    qint64 microsElapsed = timer.nsecsElapsed() / 1000;
    m_port->setStatCycleTime(microsElapsed);
}

void mbServerPortRunnable::close()
{
    m_modbusPort->close();
    while (!m_modbusPort->isStateClosed())
    {
        m_modbusPort->process();
        QThread::yieldCurrentThread();
    }
}

void mbServerPortRunnable::slotBytesTx(const Modbus::Char *source, const uint8_t* buff, uint16_t size)
{
    mbServer::LogTx(source, Modbus::bytesToString(buff, size).data());
    m_port->incStatCountTx();
}

void mbServerPortRunnable::slotBytesRx(const Modbus::Char *source, const uint8_t* buff, uint16_t size)
{
    mbServer::LogRx(source, Modbus::bytesToString(buff, size).data());
    m_port->incStatCountRx();
}

void mbServerPortRunnable::slotAsciiTx(const Modbus::Char *source, const uint8_t* buff, uint16_t size)
{
    mbServer::LogTx(source, Modbus::asciiToString(buff, size).data());
    m_port->incStatCountTx();
}

void mbServerPortRunnable::slotAsciiRx(const Modbus::Char *source, const uint8_t* buff, uint16_t size)
{
    mbServer::LogRx(source, Modbus::asciiToString(buff, size).data());
    m_port->incStatCountRx();
}

void mbServerPortRunnable::slotError(const Modbus::Char *source, Modbus::StatusCode status, const Modbus::Char *text)
{
    switch (status)
    {
    case Modbus::Status_BadSerialReadTimeout:
    case Modbus::Status_BadUdpReadTimeout:
        return; // do not count timeout errors, because they are normal for server
    case Modbus::Status_BadCrc:
    case Modbus::Status_BadLrc:
        m_device->pushEvent(MB_RECEIVE_EVENT_COMMUNICATION_ERROR);
        break;
    default:
        break;
    }
    Modbus::Timestamp tm = Modbus::currentTimestamp();
    m_port->setStatStatus(status, tm, QString(text));
    mbServer::LogError(source, QString("Error(0x%1): %2").arg(QString::number(status, 16), text));
}

void mbServerPortRunnable::slotCompleted(const Modbus::Char *, Modbus::StatusCode status)
{
    if (Modbus::StatusIsGood(status))
    {
        Modbus::Timestamp tm = Modbus::currentTimestamp();
        m_port->setStatStatus(status, tm);
    }
}

void mbServerPortRunnable::slotNewConnection(const Modbus::Char *source)
{
    mbServer::LogInfo(name(), QStringLiteral("New Connection: ") + source);
}

void mbServerPortRunnable::slotCloseConnection(const Modbus::Char *source)
{
    mbServer::LogInfo(name(), QStringLiteral("Close Connection: ") + source);
}

