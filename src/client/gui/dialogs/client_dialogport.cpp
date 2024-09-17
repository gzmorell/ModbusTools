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
#include "client_dialogport.h"
#include "ui_client_dialogport.h"

#include <QIntValidator>

#include <client.h>
#include <project/client_port.h>


mbClientDialogPort::Strings::Strings() :
    cachePrefix(QStringLiteral("Client.Ui.Dialogs.Port."))
{
}

const mbClientDialogPort::Strings &mbClientDialogPort::Strings::instance()
{
    static const Strings s;
    return s;
}

mbClientDialogPort::mbClientDialogPort(QWidget *parent) :
    mbCoreDialogPort(parent),
    ui(new Ui::mbClientDialogPort)
{
    ui->setupUi(this);

    const Modbus::Defaults &d = Modbus::Defaults::instance();

    QLineEdit* ln;

    // Host
    ln = ui->lnHost;
    ln->setText(d.host);

    m_ui.lnName            = ui->lnName           ;
    m_ui.cmbType           = ui->cmbType          ;
    m_ui.cmbSerialPortName = ui->cmbSerialPortName;
    m_ui.cmbBaudRate       = ui->cmbBaudRate      ;
    m_ui.cmbDataBits       = ui->cmbDataBits      ;
    m_ui.cmbParity         = ui->cmbParity        ;
    m_ui.cmbStopBits       = ui->cmbStopBits      ;
    m_ui.cmbFlowControl    = ui->cmbFlowControl   ;
    m_ui.spTimeoutFB       = ui->spTimeoutFB      ;
    m_ui.spTimeoutIB       = ui->spTimeoutIB      ;
    m_ui.spPort            = ui->spPort           ;
    m_ui.spTimeout         = ui->spTimeout        ;
    m_ui.stackedWidget     = ui->stackedWidget    ;
    m_ui.pgTCP             = ui->pgTCP            ;
    m_ui.pgSerial          = ui->pgSerial         ;
    m_ui.buttonBox         = ui->buttonBox        ;

    initializeBaseUi();
}

mbClientDialogPort::~mbClientDialogPort()
{
    delete ui;
}

MBSETTINGS mbClientDialogPort::cachedSettings() const
{
    Modbus::Strings ss = Modbus::Strings::instance();
    const QString &prefix = Strings().cachePrefix;
    MBSETTINGS m = mbCoreDialogPort::cachedSettings();
    m[prefix+ss.host] = ui->lnHost->text();
    return m;
}

void mbClientDialogPort::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogPort::setCachedSettings(m);

    Modbus::Strings ss = Modbus::Strings::instance();
    const QString &prefix = Strings().cachePrefix;
    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();
    //bool ok;

    it = m.find(prefix+ss.host); if (it != end) ui->lnHost->setText(it.value().toString());

}

void mbClientDialogPort::fillFormInner(const MBSETTINGS &settings)
{
    Modbus::Strings ss = Modbus::Strings::instance();

    ui->lnHost->setText(settings.value(ss.host).toString());
}

void mbClientDialogPort::fillDataInner(MBSETTINGS &settings) const
{
    Modbus::Strings ss = Modbus::Strings::instance();

    settings[ss.host] = ui->lnHost->text();
}
