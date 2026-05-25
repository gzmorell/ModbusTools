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
#include "client_rundevice.h"

#include <project/client_device.h>
#include "client_runitem.h"
#include "client_runmessage.h"

mbClientRunDevice::mbClientRunDevice(mbClientDevice *device)
{
    m_device = device;
    m_settings.name                      = m_device->name                     ();
    m_settings.unit                      = m_device->unit                     ();
    m_settings.maxReadCoils              = m_device->maxReadCoils             ();
    m_settings.maxReadDiscreteInputs     = m_device->maxReadDiscreteInputs    ();
    m_settings.maxReadInputRegisters     = m_device->maxReadInputRegisters    ();
    m_settings.maxReadHoldingRegisters   = m_device->maxReadHoldingRegisters  ();
    m_settings.maxWriteMultipleCoils     = m_device->maxWriteMultipleCoils    ();
    m_settings.maxWriteMultipleRegisters = m_device->maxWriteMultipleRegisters();
    m_settings.funcWriteSingleCoil       = m_device->funcWriteSingleCoil      ();
    m_settings.funcWriteSingleRegister   = m_device->funcWriteSingleRegister  ();
}

mbClientRunDevice::~mbClientRunDevice()
{
    qDeleteAll(m_itemsToWrite);
}

void mbClientRunDevice::pushItemsToRead(const QList<mbClientRunItem *> &itemsToRead)
{
    QWriteLocker _(&m_lock);
    m_itemsToRead.append(itemsToRead);
}

bool mbClientRunDevice::popItemsToRead(QList<mbClientRunItem*> &items)
{
    QWriteLocker _(&m_lock);
    if (m_itemsToRead.count())
    {
        items = m_itemsToRead;
        m_itemsToRead.clear();
        return true;
    }
    return false;
}

void mbClientRunDevice::pushItemsToWrite(const QList<mbClientRunItem *> &items)
{
    QWriteLocker _(&m_lock);
    m_itemsToWrite.append(items);
}

void mbClientRunDevice::pushItemToWrite(mbClientRunItem *item)
{
    QWriteLocker _(&m_lock);
    m_itemsToWrite.append(item);
}

bool mbClientRunDevice::popItemsToWrite(QList<mbClientRunItem *> &items)
{
    QWriteLocker _(&m_lock);
    if (m_itemsToWrite.count())
    {
        items = m_itemsToWrite;
        m_itemsToWrite.clear();
        return true;
    }
    return false;
}

bool mbClientRunDevice::hasExternalMessage() const
{
    QReadLocker _(&m_lock);
    return m_externalMessages.count();
}

void mbClientRunDevice::pushExternalMessage(const mbClientRunMessagePtr &message)
{
    QWriteLocker _(&m_lock);
    m_externalMessages.enqueue(message);
}

bool mbClientRunDevice::popExternalMessage(mbClientRunMessagePtr *message)
{
    QWriteLocker _(&m_lock);
    if (m_externalMessages.count())
    {
        *message = m_externalMessages.dequeue();
        return true;
    }
    return false;
}
