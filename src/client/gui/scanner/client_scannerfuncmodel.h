#ifndef MBCLIENTSCANNERFUNCMODEL_H
#define MBCLIENTSCANNERFUNCMODEL_H

#include <QAbstractTableModel>

#include <client_global.h>

class mbClientScanner;

class mbClientScannerFuncModel : public QAbstractTableModel
{
public:
    enum Columns
    {
        Column_Port  ,
        Column_Unit  ,
        Column_Func  ,
        Column_Params,
        Column_Status,
        ColumnCount
    };

public:
    explicit mbClientScannerFuncModel(mbClientScanner *scanner, QObject *parent = nullptr);

public: // table model interface
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private Q_SLOTS:
    void funcAdd(const QString &port, quint8 unit, const mbClientMessageParams &params, int status);
    void clear();

private:
    mbClientScanner *m_scanner;

private:
    struct Item
    {
        QString port;
        uint8_t unit;
        uint8_t func;
        QString params;
        Modbus::StatusCode status;
    };
    QList<Item> m_func;
};

#endif // MBCLIENTSCANNERFUNCMODEL_H
