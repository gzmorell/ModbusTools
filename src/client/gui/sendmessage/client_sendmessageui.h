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
#ifndef CLIENT_DIALOGSENDMESSAGE_H
#define CLIENT_DIALOGSENDMESSAGE_H

#include <gui/dialogs/core_dialogbase.h>
#include <client_global.h>

class mbCoreProject;
class mbCorePort;
class mbCoreDevice;
class mbCoreAddressWidget;

class mbClientProject;
class mbClientPort;
class mbClientDevice;
class mbClientSendMessageWidget;
class mbClientSendMessageDefaultWidget;
class mbClientSendMessageDiagnosticsWidget;
class mbClientSendMessageReadFIFOQueueWidget;
class mbClientSendMessageFileRecordsWidget;
class mbClientSendMessageGetCommEventCounterWidget;
class mbClientSendMessageGetCommEventLogWidget;
class mbClientSendMessageReportServerIdWidget;
class mbClientSendMessageReadDeviceIdWidget;
class mbClientSendMessageReadWriteMultipleRegistersWidget;
class mbClientSendMessageMaskWriteRegisterWidget;
class mbClientSendMessageListModel;

namespace Ui {
class mbClientSendMessageUi;
}

class mbClientSendMessageUi : public mbCoreDialogBase
{
    Q_OBJECT

public:
    enum SendTo
    {
        SendToDevice   = 0,
        SendToPortUnit = 1
    };
    Q_ENUM(SendTo)

public:
    struct Strings : public mbCoreDialogBase::Strings
    {
        const QString prefix                ;
        const QString sendTo                ;
        const QString unit                  ;
        const QString function              ;
        const QString list                  ;
        const QString period                ;
        Strings();
        static const Strings &instance();
    };

public:
    static QString getEventLogDescription(uint8_t eventId);

public:
    explicit mbClientSendMessageUi(QWidget *parent = nullptr);
    ~mbClientSendMessageUi();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

public:
    inline mbClientMessageConverter *converter() const { return const_cast<mbClientMessageConverter *>(&m_converter); }
    inline bool isTimerRunning() const { return (m_timer > 0); }
    inline bool isTimerStopped() const { return (m_timer <= 0); }

private Q_SLOTS:
    void setProject(mbCoreProject *p);
    void addPort(mbCorePort *port);
    void removePort(mbCorePort *port);
    void renamePort(mbCorePort *port, const QString newName);
    void addDevice(mbCoreDevice *device);
    void removeDevice(mbCoreDevice *device);
    void renameDevice(mbCoreDevice *device, const QString newName);
    void setCurrentFuncIndex(int funcIndex);
    void setRunStatus(int status);

private:
    void addFunctionWidget(mbClientSendMessageWidget *w);
    mbClientSendMessageWidget *currentFunctionWidget() const;

private Q_SLOTS: // list
    void slotListShowHide();
    void slotListInsert  ();
    void slotListEdit    ();
    void slotListRemove  ();
    void slotListClear   ();
    void slotListMoveUp  ();
    void slotListMoveDown();
    void slotListImport  ();
    void slotListExport  ();

private Q_SLOTS: // send
    void slotSendOne ();
    void slotSendList();
    void slotStop    ();

private Q_SLOTS:
    void slotBytesTx(const QByteArray &bytes);
    void slotBytesRx(const QByteArray &bytes);
    void slotAsciiTx(const QByteArray &bytes);
    void slotAsciiRx(const QByteArray &bytes);
    void messageCompleted();
    void getListItem(const QModelIndex &index);

private:
    void timerEvent(QTimerEvent *event) override;

private:
    QStringList getListItems() const;
    void setListItems(const QStringList& list);
    int currentListIndex() const;
    void setCurrentListIndex(int i);
    mbClientPort *currentPort() const;
    mbClientDevice *currentDevice() const;
    void setEnableParams(bool v);
    void setSendTo(int type);
    void createMessage();
    void createMessageList();
    mbClientRunMessage* createMessage(const mbClientMessageParams &params);
    void sendMessage();
    void prepareToSend(mbClientRunMessage *msg);
    void clearAfterSend(mbClientRunMessage *msg);
    void startSendMessages();
    void stopSendMessages();

private:
    bool prepareSendParams();
    void fillParams(mbClientMessageParams &params);
    void fillForm(mbClientMessageParams &params);
    void fillForm(const mbClientRunMessagePtr &message);
    uint8_t getCurrentFuncNum() const;
    void setCurrentFuncNum(uint8_t func);

private:
    Ui::mbClientSendMessageUi *ui;

private:
    mbClientProject *m_project;
    int m_sendTo;
    QList<uint8_t> m_funcNums;
    QMap<uint8_t, mbClientSendMessageWidget*> m_funcWidget;
    int m_timer;

private:
    struct DataParams
    {
        uint16_t maxReadCoils             ;
        uint16_t maxReadDiscreteInputs    ;
        uint16_t maxReadHoldingRegisters  ;
        uint16_t maxReadInputRegisters    ;
        uint16_t maxWriteMultipleCoils    ;
        uint16_t maxWriteMultipleRegisters;
    };

    DataParams m_dataParams;
    mbClientPort *m_port;
    uint8_t m_unit;
    mbClientDevice *m_device;
    mbClientSendMessageListModel *m_list;
    QList<mbClientRunMessagePtr> m_messageList;
    int m_messageIndex;
    mbClientMessageConverter m_converter;
};

#endif // CLIENT_DIALOGSENDMESSAGE_H
