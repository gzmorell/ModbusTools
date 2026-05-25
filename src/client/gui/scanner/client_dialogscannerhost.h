#ifndef CLIENT_DIALOGSCANNERHOST_H
#define CLIENT_DIALOGSCANNERHOST_H

#include <core/gui/dialogs/core_dialogbase.h>


namespace Ui {
class mbClientDialogScannerHost;
}

class mbClientDialogScannerHost : public mbCoreDialogBase
{
    Q_OBJECT

public:
    struct Strings : public mbCoreDialogBase::Strings
    {
        const QString title      ;
        const QString cachePrefix;
        const QString rangeStart ;
        const QString rangeEnd   ;
        const QString single     ;
        Strings();
        static const Strings &instance();
    };

public:
    explicit mbClientDialogScannerHost(QWidget *parent = nullptr);
    ~mbClientDialogScannerHost();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

public:
    bool getValues(QVariantList &values, const QString &title = QString());
    bool getValues(QStringList &values, const QString &title = QString());

private:
    void fillForm(const QStringList &hosts);
    void fillData(QStringList &hosts);

private Q_SLOTS:
    void slotAddRange();
    void slotAddSingle();
    void slotRemove();
    void slotClear();
    void slotMoveUp();
    void slotMoveDown();

private:
    Ui::mbClientDialogScannerHost *ui;
};

#endif // CLIENT_DIALOGSCANNERHOST_H
