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
#ifndef CORE_GLOBAL_H
#define CORE_GLOBAL_H

class QComboBox;
class QSpinBox;

class mbCoreDevice;

#include <mbcore.h>

namespace mb {

MBTOOLS_EXPORT MBSETTINGS parseExtendedAttributesStr(const QString &str);

/// \details Return list of baud rates
MBTOOLS_EXPORT QVariantList availableBaudRate();

/// \details Return list of data bits
MBTOOLS_EXPORT QVariantList availableDataBits();

/// \details Return list of `Parity` values
MBTOOLS_EXPORT QVariantList availableParity();

/// \details Return list of `StopBits` values
MBTOOLS_EXPORT QVariantList availableStopBits();

/// \details Return list of `FlowControl` values
MBTOOLS_EXPORT QVariantList availableFlowControl();

/// \details Return processed byte order counting device setting `SwapBytes`
MBTOOLS_EXPORT mb::SwapData getSwapBytes(mbCoreDevice *device, mb::SwapData swapBytes);

/// \details Return processed register order counting device setting `RegisterOrder`
MBTOOLS_EXPORT mb::RegisterOrder getRegisterOrder(mbCoreDevice *device, mb::RegisterOrder registerOrder);

/// \details Fill Modbus ProtocolType combo box
MBTOOLS_EXPORT void fillProtocolTypeComboBox(QComboBox *cmb);

/// \details Fill digital format combo box
MBTOOLS_EXPORT void fillDigitalFormatComboBox(QComboBox *cmb);


} // namespace mb

#endif // CORE_GLOBAL_H
