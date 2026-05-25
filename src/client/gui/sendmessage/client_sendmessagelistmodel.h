#ifndef CLIENT_SENDMESSAGELISTMODEL_H
#define CLIENT_SENDMESSAGELISTMODEL_H

#include <QAbstractTableModel>

#include <client_global.h>

class mbClientSendMessageListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns
    {
        Column_Func,
        Column_Param,
        ColumnCount
    };

public:
    explicit mbClientSendMessageListModel(mbClientMessageConverter *conv, QObject *parent = nullptr);
    ~mbClientSendMessageListModel();

public: // QAbstractItemModel interface
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role) const override;

public:
    QList<mbClientMessageParams> messages() const;
    mbClientMessageParams message(int i) const;
    QString messageRepr(int i);
    void setMessages(const QList<mbClientMessageParams> messages);
    void insertMessage(int i, const mbClientMessageParams &params);
    inline void addMessage(const mbClientMessageParams &params) { insertMessage(-1, params); }
    void editMessage(int i, const mbClientMessageParams &params);
    void removeMessage(int i);
    bool moveUp(int i);
    bool moveDown(int i);
    void clear();

private:
    struct Item;
    bool moveTo(int oldPos, int newPos);
    QString paramsRepr(const mbClientMessageParams &params) const;

private:
    mbClientMessageConverter *m_conv;
    QList<Item*> m_items;
};

#endif // CLIENT_SENDMESSAGELISTMODEL_H
