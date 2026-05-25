#ifndef MBCLIENTSCANNERUNITMODEL_H
#define MBCLIENTSCANNERUNITMODEL_H

#include <QAbstractTableModel>

class mbClientScanner;

class mbClientScannerUnitModel : public QAbstractTableModel
{
public:
    explicit mbClientScannerUnitModel(mbClientScanner *scanner, QObject *parent = nullptr);

public: // table model interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private Q_SLOTS:
    void deviceAdded(int index);
    void cleared();

private:
    mbClientScanner *m_scanner;
    QStringList m_devices;
};

#endif // MBCLIENTSCANNERUNITMODEL_H
