#ifndef CLIENT_DIALOGSCANNERPORT_H
#define CLIENT_DIALOGSCANNERPORT_H

#include <core/gui/dialogs/core_dialogbase.h>


namespace Ui {
class mbClientDialogScannerPort;
}

class mbClientDialogScannerPort : public mbCoreDialogBase
{
    Q_OBJECT

public:
    struct Strings : public mbCoreDialogBase::Strings
    {
        const QString title;
        const QString cachePrefix;
        const QString rangeStart;
        const QString rangeEnd;
        const QString single;
        Strings();
        static const Strings &instance();
    };

public:
    explicit mbClientDialogScannerPort(QWidget *parent = nullptr);
    ~mbClientDialogScannerPort();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

public:
    bool getValues(QVariantList &values, const QString &title = QString());
    bool getValues(QList<uint16_t> &values, const QString &title = QString());

private:
    void fillForm(const QList<uint16_t> &values);
    void fillData(QList<uint16_t> &values);

private Q_SLOTS:
    void slotAddRange();
    void slotAddSingle();
    void slotRemove();
    void slotClear();
    void slotMoveUp();
    void slotMoveDown();

private:
    Ui::mbClientDialogScannerPort *ui;
};

#endif // CLIENT_DIALOGSCANNERPORT_H
