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
#ifndef CLIENT_GLOBAL_H
#define CLIENT_GLOBAL_H

#include <mbcore.h>
#include <core_global.h>

#ifndef MBTOOLS_CLIENT_APP_NAME
#define MBTOOLS_CLIENT_APP_NAME "mbclient"
#endif

class mbClientPort;
class mbClientDevice;
class mbClientDataViewItem;
class mbClientRunMessage;
class mbClientRunMessageRaw;

typedef mb::SharedPointer<mbClientRunMessage> mbClientRunMessagePtr;
typedef mb::SharedPointer<mbClientRunMessageRaw> mbClientRunMessageRawPtr;


// Note: Memento class for Client messages
class mbClientMessageParams
{
public:
    mbClientMessageParams()
    {
        m_func = MBF_READ_HOLDING_REGISTERS;
        m_offset = 0;
        m_count = 0;
        m_format = mb::Dec16;
        m_writeOffset = 0;
        m_writeCount = 0;
        m_status = 0;
        m_eventCount = 0;
        m_messageCount = 0;
        m_deviceId = 0;
        m_objectId = 0;
        m_numberOfObjects = 0;
        m_conformityLevel = 0;
        m_numberOfObjects = 0;
        m_nextObjectId = 0;
        m_usedFields = 0;
    }

public:
    inline int function() const { return m_func; }
    inline void setFunction(int func) { m_func = func; m_usedFields |= UsedFunc; }
    inline bool hasFunction() const { return (m_usedFields & UsedFunc) != 0; }
    inline void clearFunction() { m_usedFields &= ~UsedFunc; }

    inline uint16_t offset() const { return m_offset; } 
    inline void setOffset(uint16_t offset) { m_offset = offset; m_usedFields |= UsedOffset; }
    inline bool hasOffset() const { return (m_usedFields & UsedOffset) != 0; }
    inline void clearOffset() { m_usedFields &= ~UsedOffset; }

    inline uint16_t count() const { return m_count; }
    inline void setCount(uint16_t count) { m_count = count; m_usedFields |= UsedCount; }
    inline bool hasCount() const { return (m_usedFields & UsedCount) != 0; }
    inline void clearCount() { m_usedFields &= ~UsedCount; }

    inline mb::Format format() const { return m_format; }
    inline void setFormat(mb::Format format) { m_format = format; m_usedFields |= UsedFormat; }
    inline bool hasFormat() const { return (m_usedFields & UsedFormat) != 0; }
    inline void clearFormat() { m_usedFields &= ~UsedFormat; }

    inline uint16_t subfunction() const { return m_subfunc; }
    inline void setSubfunction(uint16_t subfunc) { m_subfunc = subfunc; m_usedFields |= UsedSubfunction; }
    inline bool hasSubfunction() const { return (m_usedFields & UsedSubfunction) != 0; }
    inline void clearSubfunction() { m_usedFields &= ~UsedSubfunction; }

    inline uint16_t writeOffset() const { return m_writeOffset; }
    inline void setWriteOffset(uint16_t writeOffset) { m_writeOffset = writeOffset; m_usedFields |= UsedWriteOffset; }
    inline bool hasWriteOffset() const { return (m_usedFields & UsedWriteOffset) != 0; }
    inline void clearWriteOffset() { m_usedFields &= ~UsedWriteOffset; }
    
    inline uint16_t writeCount() const { return m_writeCount; }
    inline void setWriteCount(uint16_t writeCount) { m_writeCount = writeCount; m_usedFields |= UsedWriteCount; }
    inline bool hasWriteCount() const { return (m_usedFields & UsedWriteCount) != 0; }
    inline void clearWriteCount() { m_usedFields &= ~UsedWriteCount; }

    inline uint16_t maskAnd() const { return m_maskAnd; }
    inline void setMaskAnd(uint16_t maskAnd) { m_maskAnd = maskAnd; m_usedFields |= UsedMaskAnd; }
    inline bool hasMaskAnd() const { return (m_usedFields & UsedMaskAnd) != 0; }
    inline void clearMaskAnd() { m_usedFields &= ~UsedMaskAnd; }

    inline uint16_t maskOr() const { return m_maskOr; }
    inline void setMaskOr(uint16_t maskOr) { m_maskOr = maskOr; m_usedFields |= UsedMaskOr; }
    inline bool hasMaskOr() const { return (m_usedFields & UsedMaskOr) != 0; }
    inline void clearMaskOr() { m_usedFields &= ~UsedMaskOr; }

    inline uint16_t status() const { return m_status; }
    inline void setStatus(uint16_t status) { m_status = status; m_usedFields |= UsedStatus; }
    inline bool hasStatus() const { return (m_usedFields & UsedStatus) != 0; }
    inline void clearStatus() { m_usedFields &= ~UsedStatus; }

    inline uint16_t eventCount() const { return m_eventCount; }
    inline void setEventCount(uint16_t eventCount) { m_eventCount = eventCount; m_usedFields |= UsedEventCount; }
    inline bool hasEventCount() const { return (m_usedFields & UsedEventCount) != 0; }
    inline void clearEventCount() { m_usedFields &= ~UsedEventCount; } 

    inline uint16_t messageCount() const { return m_messageCount; }
    inline void setMessageCount(uint16_t messageCount) { m_messageCount = messageCount; m_usedFields |= UsedMessageCount; }
    inline bool hasMessageCount() const { return (m_usedFields & UsedMessageCount) != 0; }
    inline void clearMessageCount() { m_usedFields &= ~UsedMessageCount; }

    inline uint8_t deviceId() const { return m_deviceId; }
    inline void setDeviceId(uint8_t deviceId) { m_deviceId = deviceId; m_usedFields |= UsedDeviceId; }
    inline bool hasDeviceId() const { return (m_usedFields & UsedDeviceId) != 0; }
    inline void clearDeviceId() { m_usedFields &= ~UsedDeviceId; }  

    inline uint8_t objectId() const { return m_objectId; }
    inline void setObjectId(uint8_t objectId) { m_objectId = objectId; m_usedFields |= UsedObjectId; }
    inline bool hasObjectId() const { return (m_usedFields & UsedObjectId) != 0; }
    inline void clearObjectId() { m_usedFields &= ~UsedObjectId; }  

    inline uint8_t numberOfObjects() const { return m_numberOfObjects; }
    inline void setNumberOfObjects(uint8_t numberOfObjects) { m_numberOfObjects = numberOfObjects; m_usedFields |= UsedNumberOfObjects; }
    inline bool hasNumberOfObjects() const { return (m_usedFields & UsedNumberOfObjects) != 0; }
    inline void clearNumberOfObjects() { m_usedFields &= ~UsedNumberOfObjects; }    

    inline uint8_t conformityLevel() const { return m_conformityLevel; }
    inline void setConformityLevel(uint8_t conformityLevel) { m_conformityLevel = conformityLevel; m_usedFields |= UsedConformityLevel; }
    inline bool hasConformityLevel() const { return (m_usedFields & UsedConformityLevel) != 0; }
    inline void clearConformityLevel() { m_usedFields &= ~UsedConformityLevel; }    

    inline uint8_t nextObjectId() const { return m_nextObjectId; }
    inline void setNextObjectId(uint8_t nextObjectId) { m_nextObjectId = nextObjectId; m_usedFields |= UsedNextObjectId; }
    inline bool hasNextObjectId() const { return (m_usedFields & UsedNextObjectId) != 0; }
    inline void clearNextObjectId() { m_usedFields &= ~UsedNextObjectId; }

    inline bool moreFollows() const { return m_moreFollows; }
    inline void setMoreFollows(bool moreFollows) { m_moreFollows = moreFollows; m_usedFields |= UsedMoreFollows; }
    inline bool hasMoreFollows() const { return (m_usedFields & UsedMoreFollows) != 0; }
    inline void clearMoreFollows() { m_usedFields &= ~UsedMoreFollows; }

    inline const QVector<Modbus::FileRecord>& fileRecords() const { return m_fileRecords; }
    inline QVector<Modbus::FileRecord>& fileRecords() { return m_fileRecords; }
    inline QByteArray fileRecordsAsByteArray() const { return QByteArray((const char*)m_fileRecords.constData(), sizeof(Modbus::FileRecord)*m_fileRecords.count()); }
    inline void setFileRecords(const QVector<Modbus::FileRecord> &fileRecords) { m_fileRecords = fileRecords; m_usedFields |= UsedFileRecords; }
    inline void setFileRecords(const QByteArray &b) { m_fileRecords.resize(b.length()/sizeof(Modbus::FileRecord)); memcpy(m_fileRecords.data(), b.constData(), b.length()); m_usedFields |= UsedFileRecords; }
    inline void setFileRecords(const Modbus::FileRecord* fileRecords, int count) { m_fileRecords.resize(count); memcpy(m_fileRecords.data(), fileRecords, count*sizeof(Modbus::FileRecord)); m_usedFields |= UsedFileRecords; }
    inline bool hasFileRecords() const { return (m_usedFields & UsedFileRecords) != 0; }
    inline void clearFileRecords() { m_usedFields &= ~UsedFileRecords; }

    inline const QVariant& data() const { return m_data; }
    inline void setData(const QVariant &data) { m_data = data; m_usedFields |= UsedData; }
    inline bool hasData() const { return (m_usedFields & UsedData) != 0; }
    inline void clearData() { m_usedFields &= ~UsedData; }

private:
    enum UsedFields
    {
        UsedFunc            = 0x00000001,
        UsedOffset          = 0x00000002,
        UsedCount           = 0x00000004,
        UsedFormat          = 0x00000008,
        UsedWriteOffset     = 0x00000010,
        UsedWriteCount      = 0x00000020,
        UsedMaskAnd         = 0x00000040,
        UsedMaskOr          = 0x00000080,
        UsedSubfunction     = 0x00000100,
        UsedStatus          = 0x00000200,
        UsedEventCount      = 0x00000400,
        UsedMessageCount    = 0x00000800,
        UsedDeviceId        = 0x00001000,
        UsedObjectId        = 0x00002000,
        UsedNumberOfObjects = 0x00004000,
        UsedConformityLevel = 0x00008000,
        UsedNextObjectId    = 0x00010000,
        UsedMoreFollows     = 0x00020000,
        UsedFileRecords     = 0x40000000,
        UsedData            = 0x80000000
    };
    int m_usedFields;
    int m_func;
    union
    {
        uint16_t m_offset;
        uint16_t m_subfunc;
    };
    uint16_t m_count;
    mb::Format m_format;

    union
    {
        uint16_t m_writeOffset;
        uint16_t m_maskAnd;
    };

    union
    {
        uint16_t m_writeCount;
        uint16_t m_maskOr;
    };

    union 
    {
        struct // Event Log
        {
            uint16_t m_status;
            uint16_t m_eventCount;
            uint16_t m_messageCount;
        };

        struct // Read Device and Object IDs
        {
            uint8_t m_deviceId;
            uint8_t m_objectId;
            uint8_t m_numberOfObjects;
            uint8_t m_conformityLevel;
            uint8_t m_nextObjectId;
            bool m_moreFollows;
        };
    };
    
    QVector<Modbus::FileRecord> m_fileRecords;
    QVariant m_data;

};
Q_DECLARE_METATYPE(mbClientMessageParams)

class mbClientMessageConverter
{
public:
    struct DataParams
    {
        mb::SwapData         swapBytes         ;
        mb::RegisterOrder    registerOrder     ;
        mb::DigitalFormat    byteArrayFormat   ;
        mb::StringEncoding   stringEncoding    ;
        mb::StringLengthType stringLengthType  ;
        QString              byteArraySeparator;
    };

public:
    mbClientMessageConverter();

public:
    inline mb::SwapData swapBytes() const  { return m_dataParams.swapBytes; }
    inline void setSwapBytes(mb::SwapData swapBytes) { m_dataParams.swapBytes = swapBytes; }

    inline mb::RegisterOrder registerOrder() const  { return m_dataParams.registerOrder; }
    inline void setRegisterOrder(mb::RegisterOrder registerOrder) { m_dataParams.registerOrder = registerOrder; }

    inline mb::DigitalFormat byteArrayFormat() const  { return m_dataParams.byteArrayFormat; }
    inline void setByteArrayFormat(mb::DigitalFormat byteArrayFormat) { m_dataParams.byteArrayFormat = byteArrayFormat; }

    inline mb::StringEncoding stringEncoding() const  { return m_dataParams.stringEncoding; }
    inline void setStringEncoding(mb::StringEncoding stringEncoding) { m_dataParams.stringEncoding = stringEncoding; }

    inline mb::StringLengthType stringLengthType() const  { return m_dataParams.stringLengthType; }
    inline void setStringLengthType(mb::StringLengthType stringLengthType) { m_dataParams.stringLengthType = stringLengthType; }

    inline QString byteArraySeparator() const  { return m_dataParams.byteArraySeparator; }
    inline void setByteArraySeparator(const QString &byteArraySeparator) { m_dataParams.byteArraySeparator = byteArraySeparator; }

public:
    QByteArray toByteArray(const mbClientMessageParams &params) const;
    QVariant toVariant(const mbClientMessageParams &params) const;

public:
    QStringList toStringListBits(const QByteArray &data, uint16_t count) const;
    QStringList toStringListNumbers(const QByteArray &data, mb::Format format) const;
    QByteArray fromStringListBits(const QStringList &ls) const;
    QByteArray fromStringListNumbers(const QStringList &ls, mb::Format format) const;
    bool fromStringNumber(mb::Format format, const QString &v, void *buff) const;
    QStringList dataToStringList(const QString &s) const;
    static QString getEventLogDescription(uint8_t eventId);
    static Modbus::MemoryType getMemoryType(const mbClientMessageParams &params);
    inline static bool isBitMemory(Modbus::MemoryType memType) { return memType == Modbus::Memory_0x || memType == Modbus::Memory_1x; }
    inline static bool isBitMemory(const mbClientMessageParams &params) { return isBitMemory(getMemoryType(params)); }
    inline static bool isRegMemory(Modbus::MemoryType memType) { return memType == Modbus::Memory_4x || memType == Modbus::Memory_3x; }
    inline static bool isRegMemory(const mbClientMessageParams &params) { return isRegMemory(getMemoryType(params)); }
    QStringList saveClientMessages(const QList<mbClientMessageParams> messages) const;
    QList<mbClientMessageParams> restoreClientMessages(const QStringList &messages) const;
    QString saveClientMessageParams(const mbClientMessageParams &params, bool useFunc = true, bool useData = true) const;
    mbClientMessageParams restoreClientMessageParams(const QString &params, bool *ok = nullptr, uint8_t func = 0) const;
    static QHash<QString, QString> getClientParamMap(const QString &params);
    static QString serializeStringList(const QStringList &ls, QChar sep = ',');
    static QStringList deserializeStringList(const QString &s, QChar sep = ',');

private:
    DataParams m_dataParams;
};

namespace mb {

namespace Client {

typedef mbClientPort* PortHandle_t;
typedef mbClientDevice* DeviceHandle_t;
typedef mbClientDataViewItem* ItemHandle_t;

} // namespace Client

} // namespace mb

#endif // CLIENT_GLOBAL_H
