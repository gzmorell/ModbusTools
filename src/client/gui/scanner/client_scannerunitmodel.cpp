#include "client_scannerunitmodel.h"

#include "client_scanner.h"

mbClientScannerUnitModel::mbClientScannerUnitModel(mbClientScanner *scanner, QObject *parent)
    : QAbstractTableModel{parent}
{
    m_scanner = scanner;
    connect(m_scanner, &mbClientScanner::deviceAdded, this, &mbClientScannerUnitModel::deviceAdded);
    connect(m_scanner, &mbClientScanner::cleared, this, &mbClientScannerUnitModel::cleared);
}

int mbClientScannerUnitModel::rowCount(const QModelIndex &) const
{
    return m_devices.count();
}

int mbClientScannerUnitModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant mbClientScannerUnitModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return m_devices.value(index.row());
    return QVariant();
}

void mbClientScannerUnitModel::deviceAdded(int index)
{
    beginInsertRows(QModelIndex(), index, index);
    m_devices.append(m_scanner->deviceInfoStr(index));
    endInsertRows();
}

void mbClientScannerUnitModel::cleared()
{
    beginResetModel();
    m_devices.clear();
    endResetModel();
}
