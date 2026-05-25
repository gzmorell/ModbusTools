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
#ifndef SERVER_RUNDEVICE_H
#define SERVER_RUNDEVICE_H

#include <QSet>
#include <mbcore.h>

class mbServerPort;
class mbServerDevice;

class mbServerRunDevice : public ModbusInterface
{
public:
    explicit mbServerRunDevice(mbServerPort *port);
    virtual ~mbServerRunDevice();

public: // Modbus::Interface
    Modbus::StatusCode readCoils(uint8_t unit, uint16_t offset, uint16_t count, void *values) override;
    Modbus::StatusCode readDiscreteInputs(uint8_t unit, uint16_t offset, uint16_t count, void *values) override;
    Modbus::StatusCode readHoldingRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values) override;
    Modbus::StatusCode readInputRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values) override;
    Modbus::StatusCode writeSingleCoil(uint8_t unit, uint16_t offset, bool value) override;
    Modbus::StatusCode writeSingleRegister(uint8_t unit, uint16_t offset, uint16_t value) override;
    Modbus::StatusCode readExceptionStatus(uint8_t unit, uint8_t *status) override;
    Modbus::StatusCode diagnosticsReturnQueryData(uint8_t unit, const void *indata, uint8_t insize, void *outdata, uint8_t *outsize) override;
    Modbus::StatusCode diagnosticsRestartCommunicationsOption(uint8_t unit, bool clearEventLog) override;
    Modbus::StatusCode diagnosticsReturnDiagnosticRegister(uint8_t unit, uint16_t *value) override;
    Modbus::StatusCode diagnosticsChangeAsciiInputDelimiter(uint8_t unit, char delimiter) override;
    Modbus::StatusCode diagnosticsForceListenOnlyMode(uint8_t unit) override;
    Modbus::StatusCode diagnosticsClearCountersAndDiagnosticRegister(uint8_t unit) override;
    Modbus::StatusCode diagnosticsReturnBusMessageCount(uint8_t unit, uint16_t *count) override;
    Modbus::StatusCode diagnosticsReturnBusCommunicationErrorCount(uint8_t unit, uint16_t *count) override;
    Modbus::StatusCode diagnosticsReturnBusExceptionErrorCount(uint8_t unit, uint16_t *count) override;
    Modbus::StatusCode diagnosticsReturnServerMessageCount(uint8_t unit, uint16_t *count) override;
    Modbus::StatusCode diagnosticsReturnServerNoResponseCount(uint8_t unit, uint16_t *count) override;
    Modbus::StatusCode diagnosticsReturnServerNAKCount(uint8_t unit, uint16_t *count) override;
    Modbus::StatusCode diagnosticsReturnServerBusyCount(uint8_t unit, uint16_t *count) override;
    Modbus::StatusCode diagnosticsReturnBusCharacterOverrunCount(uint8_t unit, uint16_t *count) override;
    Modbus::StatusCode diagnosticsClearOverrunCounterAndFlag(uint8_t unit) override;
    Modbus::StatusCode getCommEventCounter(uint8_t unit, uint16_t *status, uint16_t *eventCount) override;
    Modbus::StatusCode getCommEventLog(uint8_t unit, uint16_t *status, uint16_t *eventCount, uint16_t *messageCount, void *eventBuff, uint8_t *eventBuffSize) override;
    Modbus::StatusCode writeMultipleCoils(uint16_t offset, uint16_t count, const void *values);
    Modbus::StatusCode writeMultipleCoils(uint8_t unit, uint16_t offset, uint16_t count, const void *values) override;
    Modbus::StatusCode writeMultipleRegisters(uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values) override;
    Modbus::StatusCode reportServerID(uint8_t unit, void *data, uint8_t *count) override;
    Modbus::StatusCode readFileRecord(uint8_t unit, const Modbus::FileRecord *records, uint8_t recordsCount, void *outData, uint8_t *outSize = nullptr) override;
    Modbus::StatusCode writeFileRecord(uint8_t unit, const Modbus::FileRecord *records, uint8_t recordsCount, const void *inData, uint8_t *inSize = nullptr) override;
    Modbus::StatusCode maskWriteRegister(uint8_t unit, uint16_t offset, uint16_t andMask, uint16_t orMask) override;
    Modbus::StatusCode readWriteMultipleRegisters(uint8_t unit, uint16_t readOffset, uint16_t readCount, uint16_t *readValues, uint16_t writeOffset, uint16_t writeCount, const uint16_t *writeValues) override;
    Modbus::StatusCode readFIFOQueue(uint8_t unit, uint16_t fifoadr, uint16_t *values, uint16_t *count) override;
    Modbus::StatusCode readDeviceIdentification(uint8_t unit, uint8_t readDeviceId, uint8_t objectId, void *data, uint8_t *dataSize, uint8_t *numberOfObjects = nullptr, uint8_t *conformityLevel = nullptr, bool *moreFollows = nullptr, uint8_t *nextObjectId = nullptr) override;

public:
    void pushEvent(uint8_t event);

public: // settings
    inline bool isBroadcastEnabled() const { return m_settings.isBroadcastEnabled; }
    inline void setBroadcastEnabled(bool enable) { m_settings.isBroadcastEnabled = enable; }

public:
    inline mbServerPort* port() const { return m_port; }
    inline bool isBroadcast(uint8_t unit) const { return (unit == 0) && isBroadcastEnabled(); }
    inline QSet<mbServerDevice*> devices() const { return m_devices; }
    inline QSet<uint8_t> unitNumbers() const { return m_unitNumbers; }
    inline mbServerDevice *device(uint8_t unit) const { return m_units[unit]; }
    void setDevice(uint8_t unit, mbServerDevice *device);

private:
    struct
    {
        bool isBroadcastEnabled;
    } m_settings;

private:
    mbServerPort* m_port;

private: // devices
    static const int UnitsSize = 256;
    mbServerDevice *m_units[UnitsSize];
    QSet<mbServerDevice*> m_devices;
    QSet<uint8_t> m_unitNumbers;
    mb::Timestamp_t m_timestamp;
};

#endif // SERVER_RUNDEVICE_H
