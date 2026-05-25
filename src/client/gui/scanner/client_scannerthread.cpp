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
#include "client_scannerthread.h"

#include <ModbusClientPort.h>
#include <ModbusNetPort.h>
#include <ModbusRtuPort.h>
#include <ModbusAscPort.h>

#include <client.h>

#include "client_scanner.h"

mbClientScannerThread::mbClientScannerThread(mbClientScanner *scanner, QObject *parent)
    : QThread(parent)
{
    m_scanner = scanner;
    m_ctrlRun = true;
    m_unitStart = Modbus::VALID_MODBUS_ADDRESS_BEGIN;
    m_unitEnd = Modbus::VALID_MODBUS_ADDRESS_END;
    moveToThread(this);
}

mbClientScannerThread::~mbClientScannerThread()
{
}

void mbClientScannerThread::setSettings(const Modbus::Settings &settings)
{
    const Modbus::Strings &s = Modbus::Strings::instance();

    m_settings = settings;
    m_period    = mbClientScanner::getSettingPeriod   (settings);
    m_unitStart = mbClientScanner::getSettingUnitStart(settings);
    m_unitEnd   = mbClientScanner::getSettingUnitEnd  (settings);
    m_request   = mbClientScanner::getSettingRequest  (settings);
    m_combinationCount = 1;

    m_unitInc = m_unitStart > m_unitEnd ? -1 : +1;
    m_divMods   .clear();
    m_names     .clear();
    m_valuesList.clear();

#define DEFINE_COMBINATION_ELEMENT(elem)            \
        if (elem.count())                           \
        {                                           \
                DivMod dm;                          \
                dm.div = m_combinationCount;        \
                dm.mod = elem.count();              \
                m_divMods.append(dm);               \
                m_names.append(s.elem);             \
                m_valuesList.append(elem);          \
                m_combinationCount *= elem.count(); \
        }

    Modbus::ProtocolType type = Modbus::getSettingType(settings);
    switch (type)
    {
    case Modbus::ASC:
    case Modbus::RTU:
    {
        QVariantList baudRate = mbClientScanner::getSettingBaudRate(settings);
        QVariantList dataBits = mbClientScanner::getSettingDataBits(settings);
        QVariantList parity   = mbClientScanner::getSettingParity  (settings);
        QVariantList stopBits = mbClientScanner::getSettingStopBits(settings);

        DEFINE_COMBINATION_ELEMENT(stopBits)
        DEFINE_COMBINATION_ELEMENT(parity  )
        DEFINE_COMBINATION_ELEMENT(dataBits)
        DEFINE_COMBINATION_ELEMENT(baudRate)
    }
        break;
    default:
        QVariantList host = mbClientScanner::getSettingHost(settings);
        QVariantList port = mbClientScanner::getSettingPort(settings);
        
        DEFINE_COMBINATION_ELEMENT(host)
        DEFINE_COMBINATION_ELEMENT(port)

        break;
    }
    m_combinationCountAll = m_combinationCount * (qAbs(m_unitEnd - m_unitStart) + 1) * m_request.count();
    m_statTx = 0;
    m_statRx = 0;
}

void mbClientScannerThread::run()
{
    const mbClientScanner::Strings &s = mbClientScanner::Strings::instance();
    m_ctrlRun = true;
    mbClient::LogInfo(s.name, QStringLiteral("Start scanning"));


    Modbus::Settings settings = m_settings;
    uint8_t dummy[MB_MAX_BYTES+1];
    uint16_t *regdummy = reinterpret_cast<uint16_t*>(dummy);
    memset(dummy, 0, sizeof(dummy));

    quint32 deviceFound = 0;
    quint32 funcCount = 0;
    for (uint c = 0; m_ctrlRun && (c < m_combinationCount); c++)
    {
        // Get comibation number for each setting
        for (quint16 si = 0; si < m_names.count(); si++)
        {
            const QString &name = m_names.at(si);
            const DivMod &dm = m_divMods.at(si);
            // Calc value of each setting corresponding to current combination number
            int sc = (c / dm.div) % dm.mod;
            const QVariant &v = m_valuesList.at(si).at(sc);
            settings[name] = v;
        }
        ModbusClientPort *clientPort = Modbus::createClientPort(settings, false);
        QString sPort;
        switch (clientPort->type())
        {
        case Modbus::ASC:
        {
            ModbusAscPort *port = static_cast<ModbusAscPort*>(clientPort->port());
            clientPort->connect(&ModbusClientPort::signalTx, this, &mbClientScannerThread::slotAsciiTx);
            clientPort->connect(&ModbusClientPort::signalRx, this, &mbClientScannerThread::slotAsciiRx);
            sPort = QString("ASC:%1:%2:%3%4%5")
                        .arg(port->portName(),
                             QString::number(port->baudRate()),
                             QString::number(port->dataBits()),
                             mbClientScanner::toShortParityStr(port->parity()),
                             mbClientScanner::toShortStopBitsStr(port->stopBits()));
        }
            break;
        case Modbus::RTU:
        {
            ModbusRtuPort *port = static_cast<ModbusRtuPort*>(clientPort->port());
            clientPort->connect(&ModbusClientPort::signalTx, this, &mbClientScannerThread::slotBytesTx);
            clientPort->connect(&ModbusClientPort::signalRx, this, &mbClientScannerThread::slotBytesRx);
            sPort = QString("RTU:%1:%2:%3%4%5")
                        .arg(port->portName(),
                             QString::number(port->baudRate()),
                             QString::number(port->dataBits()),
                             mbClientScanner::toShortParityStr(port->parity()),
                             mbClientScanner::toShortStopBitsStr(port->stopBits()));
        }
            break;
        default:
        {
            ModbusNetPort *port = static_cast<ModbusNetPort*>(clientPort->port());
            clientPort->connect(&ModbusClientPort::signalTx, this, &mbClientScannerThread::slotBytesTx);
            clientPort->connect(&ModbusClientPort::signalRx, this, &mbClientScannerThread::slotBytesRx);
            sPort = QString("%1:%2:%3")
                        .arg(Modbus::sprotocolType(clientPort->type()),
                             port->host(),
                             QString::number(port->port()));
        }
            break;
        }
        mbClient::LogInfo(s.name, QString("Begin scanning '%1'").arg(sPort));
        for (int unit = m_unitStart;; unit+=m_unitInc)
        {
            if (!m_ctrlRun)
                break;
            QString sPortUnit = QString("%1,Unit=%2").arg(sPort, QString::number(unit));
            clientPort->setObjectName(sPortUnit.toLatin1().constData());
            m_scanner->setStatDevice(sPortUnit);
            Modbus::StatusCode status = Modbus::Status_Bad;
            bool deviceIsFound = false;
            Q_FOREACH (auto &f, m_request)
            {
                memset(dummy, 0, sizeof(dummy));
                m_scanner->setFunctionBegin(sPort, static_cast<uint8_t>(unit), f);
                auto tmend = mb::currentTimestamp() + m_period;
                while (m_ctrlRun)
                {
                    switch (f.function())
                    {
                    case MBF_READ_COILS:
                        status = clientPort->readCoils(static_cast<uint8_t>(unit), f.offset(), f.count(), dummy);
                        break;
                    case MBF_READ_DISCRETE_INPUTS:
                        status = clientPort->readDiscreteInputs(static_cast<uint8_t>(unit), f.offset(), f.count(), dummy);
                        break;
                    case MBF_READ_HOLDING_REGISTERS:
                        status = clientPort->readHoldingRegisters(static_cast<uint8_t>(unit), f.offset(), f.count(), regdummy);
                        break;
                    case MBF_READ_INPUT_REGISTERS:
                        status = clientPort->readInputRegisters(static_cast<uint8_t>(unit), f.offset(), f.count(), regdummy);
                        break;
                    case MBF_WRITE_SINGLE_COIL:
                        status = clientPort->writeSingleCoil(static_cast<uint8_t>(unit), f.offset(), 0);
                        break;
                    case MBF_WRITE_SINGLE_REGISTER:
                        status = clientPort->writeSingleRegister(static_cast<uint8_t>(unit), f.offset(), 0);
                        break;
                    case MBF_READ_EXCEPTION_STATUS:
                        status = clientPort->readExceptionStatus(static_cast<uint8_t>(unit), dummy);
                        break;
                    case MBF_DIAGNOSTICS:
                        switch (f.subfunction())
                        {
                        case MBF_DIAGNOSTICS_RETURN_QUERY_DATA:
                            status = clientPort->diagnosticsReturnQueryData(static_cast<uint8_t>(unit), dummy, 2, &dummy[1], dummy);
                            break;
                        case MBF_DIAGNOSTICS_RESTART_COMMUNICATIONS_OPTION:
                            status = clientPort->diagnosticsRestartCommunicationsOption(static_cast<uint8_t>(unit), dummy[0]);
                            break;
                        case MBF_DIAGNOSTICS_RETURN_DIAGNOSTIC_REGISTER:
                            status = clientPort->diagnosticsReturnDiagnosticRegister(static_cast<uint8_t>(unit), reinterpret_cast<uint16_t*>(dummy));
                            break;
                        case MBF_DIAGNOSTICS_CHANGE_ASCII_INPUT_DELIMITER:
                            status = clientPort->diagnosticsChangeAsciiInputDelimiter(static_cast<uint8_t>(unit), dummy[0]);
                            break;
                        case MBF_DIAGNOSTICS_FORCE_LISTEN_ONLY_MODE:
                            status = clientPort->diagnosticsForceListenOnlyMode(static_cast<uint8_t>(unit));
                            break;
                        case MBF_DIAGNOSTICS_CLEAR_COUNTERS_AND_DIAGNOSTIC_REGISTER:
                            status = clientPort->diagnosticsClearCountersAndDiagnosticRegister(static_cast<uint8_t>(unit));
                            break;
                        case MBF_DIAGNOSTICS_RETURN_BUS_MESSAGE_COUNT:
                            status = clientPort->diagnosticsReturnBusMessageCount(static_cast<uint8_t>(unit), reinterpret_cast<uint16_t*>(dummy));
                            break;
                        case MBF_DIAGNOSTICS_RETURN_BUS_COMMUNICATION_ERROR_COUNT:
                            status = clientPort->diagnosticsReturnBusCommunicationErrorCount(static_cast<uint8_t>(unit), reinterpret_cast<uint16_t*>(dummy));
                            break;
                        case MBF_DIAGNOSTICS_RETURN_BUS_EXCEPTION_ERROR_COUNT:
                            status = clientPort->diagnosticsReturnBusExceptionErrorCount(static_cast<uint8_t>(unit), reinterpret_cast<uint16_t*>(dummy));
                            break;
                        case MBF_DIAGNOSTICS_RETURN_SERVER_MESSAGE_COUNT:
                            status = clientPort->diagnosticsReturnServerMessageCount(static_cast<uint8_t>(unit), reinterpret_cast<uint16_t*>(dummy));
                            break;
                        case MBF_DIAGNOSTICS_RETURN_SERVER_NO_RESPONSE_COUNT:
                            status = clientPort->diagnosticsReturnServerNoResponseCount(static_cast<uint8_t>(unit), reinterpret_cast<uint16_t*>(dummy));
                            break;
                        case MBF_DIAGNOSTICS_RETURN_SERVER_NAK_COUNT:
                            status = clientPort->diagnosticsReturnServerNAKCount(static_cast<uint8_t>(unit), reinterpret_cast<uint16_t*>(dummy));
                            break;
                        case MBF_DIAGNOSTICS_RETURN_SERVER_BUSY_COUNT:
                            status = clientPort->diagnosticsReturnServerBusyCount(static_cast<uint8_t>(unit), reinterpret_cast<uint16_t*>(dummy));
                            break;
                        case MBF_DIAGNOSTICS_RETURN_BUS_CHARACTER_OVERRUN_COUNT:
                            status = clientPort->diagnosticsReturnBusCharacterOverrunCount(static_cast<uint8_t>(unit), reinterpret_cast<uint16_t*>(dummy));
                            break;
                        case MBF_DIAGNOSTICS_CLEAR_OVERRUN_COUNTER_AND_FLAG:
                            status = clientPort->diagnosticsClearOverrunCounterAndFlag(static_cast<uint8_t>(unit));
                            break;
                        default:
                            status = Modbus::Status_BadIllegalFunction;
                            break;
                        }
                        break;
                    case MBF_GET_COMM_EVENT_COUNTER:
                        status = clientPort->getCommEventCounter(static_cast<uint8_t>(unit), &regdummy[0], &regdummy[1]);
                        break;
                    case MBF_GET_COMM_EVENT_LOG:
                        status = clientPort->getCommEventLog(static_cast<uint8_t>(unit), &regdummy[0], &regdummy[1], &regdummy[2], &dummy[7], &dummy[6]);
                        break;
                    case MBF_WRITE_MULTIPLE_COILS:
                        status = clientPort->writeMultipleCoils(static_cast<uint8_t>(unit), f.offset(), f.count(), dummy);
                        break;
                    case MBF_WRITE_MULTIPLE_REGISTERS:
                        status = clientPort->writeMultipleRegisters(static_cast<uint8_t>(unit), f.offset(), f.count(), regdummy);
                        break;
                    case MBF_REPORT_SERVER_ID:
                        status = clientPort->reportServerID(static_cast<uint8_t>(unit), &dummy[1], &dummy[0]);
                        break;
                    case MBF_READ_FILE_RECORD:
                        status = clientPort->readFileRecord(static_cast<uint8_t>(unit), f.fileRecords().data(), f.fileRecords().count(), &dummy[1], &dummy[0]);
                        break;
                    case MBF_MASK_WRITE_REGISTER:
                        status = clientPort->maskWriteRegister(static_cast<uint8_t>(unit), f.offset(), 0, 0);
                        break;
                    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
                        status = clientPort->readWriteMultipleRegisters(static_cast<uint8_t>(unit), f.offset(), f.count(), regdummy, f.offset(), f.count(), regdummy);
                        break;
                    case MBF_READ_FIFO_QUEUE:
                        status = clientPort->readFIFOQueue(static_cast<uint8_t>(unit), f.offset(), &regdummy[1], &regdummy[0]);
                        break;
                    case MBF_ENCAPSULATED_INTERFACE_TRANSPORT:
                        status = clientPort->readDeviceIdentification(static_cast<uint8_t>(unit), f.deviceId(), f.objectId(), &dummy[1], &dummy[0]);
                        break;
                    }

                    if (Modbus::StatusIsProcessing(status))
                        Modbus::msleep(1);
                    else
                        break;
                }
                if (Modbus::StatusIsGood(status))
                {
                    if (!deviceIsFound)
                    {
                        deviceIsFound = true;
                        Modbus::setSettingUnit(settings, unit);
                        m_scanner->deviceAdd(settings);
                        m_scanner->setStatFound(++deviceFound);
                    }
                    m_scanner->setFunctionCompleted(sPort, unit, f, status);
                }
                else if (Modbus::StatusIsBad(status))
                {
                    mbClient::LogError(s.name, QString("%1 Error (%2): %3").arg(sPortUnit, QString::number(status, 16), clientPort->lastErrorText()));
                    m_scanner->setFunctionCompleted(sPort, unit, f, status);
                }
                auto percent = ++funcCount*100/m_combinationCountAll;
                m_scanner->setStatPercent(percent);
                if (percent >= 100)
                    break;
                while (m_ctrlRun)
                {
                    auto tm = mb::currentTimestamp();
                    if (tm >= tmend)
                        break;
                    Modbus::msleep(1);
                }
            }
            if (unit == m_unitEnd)
                break;
        }
        clientPort->close();
        mbClient::LogInfo(s.name, QString("End scanning '%1'").arg(sPort));
        delete clientPort;
    }
    mbClient::LogInfo(s.name, QStringLiteral("Finish scanning"));
}

void mbClientScannerThread::slotBytesTx(const Modbus::Char *source, const uint8_t *buff, uint16_t size)
{
    mbClient::LogTx(mbClientScanner::Strings::instance().name, source + QStringLiteral(": ") + Modbus::bytesToString(buff, size).data());
    incStatTx();
}

void mbClientScannerThread::slotBytesRx(const Modbus::Char *source, const uint8_t *buff, uint16_t size)
{
    mbClient::LogRx(mbClientScanner::Strings::instance().name, source + QStringLiteral(": ") + Modbus::bytesToString(buff, size).data());
    incStatRx();
}

void mbClientScannerThread::slotAsciiTx(const Modbus::Char *source, const uint8_t *buff, uint16_t size)
{
    mbClient::LogTx(mbClientScanner::Strings::instance().name, source + QStringLiteral(": ") + Modbus::asciiToString(buff, size).data());
    incStatTx();
}

void mbClientScannerThread::slotAsciiRx(const Modbus::Char *source, const uint8_t *buff, uint16_t size)
{
    mbClient::LogRx(mbClientScanner::Strings::instance().name, source + QStringLiteral(": ") + Modbus::asciiToString(buff, size).data());
    incStatRx();
}

void mbClientScannerThread::incStatTx()
{
    m_statTx++;
    m_scanner->setStatCountTx(m_statTx);
}

void mbClientScannerThread::incStatRx()
{
    m_statRx++;
    m_scanner->setStatCountRx(m_statRx);
}
