#include "client_scannerfuncmodel.h"

#include <QColor>
#include <QBrush>

#include "client_scanner.h"

mbClientScannerFuncModel::mbClientScannerFuncModel(mbClientScanner *scanner, QObject *parent)
    : QAbstractTableModel{parent}
{
    m_scanner = scanner;
    connect(m_scanner, &mbClientScanner::statFunctionCompleted, this, &mbClientScannerFuncModel::funcAdd);
    connect(m_scanner, &mbClientScanner::cleared, this, &mbClientScannerFuncModel::clear);
}

QVariant mbClientScannerFuncModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        switch (orientation)
        {
        case Qt::Horizontal:
            switch (section)
            {
            case Column_Port  : return QStringLiteral("Port");
            case Column_Unit  : return QStringLiteral("Unit");
            case Column_Func  : return QStringLiteral("Func");
            case Column_Params: return QStringLiteral("Params");
            case Column_Status: return QStringLiteral("Status");
            }
            break;
        case Qt::Vertical:
            return section+1;
        }
        break;
    }
    return QVariant();
}

int mbClientScannerFuncModel::rowCount(const QModelIndex &) const
{
    return m_func.count();
}

int mbClientScannerFuncModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QVariant mbClientScannerFuncModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
        case Column_Port  : return m_func.at(index.row()).port;
        case Column_Unit  : return m_func.at(index.row()).unit;
        case Column_Func  : return QString("%1").arg(m_func.at(index.row()).func, 2, 10, QLatin1Char('0'));
        case Column_Params: return m_func.at(index.row()).params;
        case Column_Status: return mb::toString(m_func.at(index.row()).status);
        }
        break;
    case Qt::BackgroundRole:
    {
        auto s = m_func.at(index.row()).status;
        if (Modbus::StatusIsGood(s))
            return QBrush(QColor(0xCC, 0xFF, 0xCC));
        if (Modbus::StatusIsBad(s))
            return QBrush(QColor(0xFF, 0xCC, 0xCC));
    }
        break;
    }
    return QVariant();
}

void mbClientScannerFuncModel::funcAdd(const QString &port, quint8 unit, const mbClientMessageParams &params, int status)
{
    mbClientMessageConverter conv;
    int c = m_func.count();
    Item item;
    item.port = port;
    item.unit = unit;
    item.func = params.function();
    item.params = conv.saveClientMessageParams(params, false, false);
    item.status = static_cast<Modbus::StatusCode>(status);
    beginInsertRows(QModelIndex(), c, c);
    m_func.append(item);
    endInsertRows();
}

void mbClientScannerFuncModel::clear()
{
    beginResetModel();
    m_func.clear();
    endResetModel();
}
