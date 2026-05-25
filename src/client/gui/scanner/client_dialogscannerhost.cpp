#include "client_dialogscannerhost.h"
#include "ui_client_dialogscannerhost.h"

mbClientDialogScannerHost::Strings::Strings() :
    title(QStringLiteral("Edit hosts")),
    cachePrefix(QStringLiteral("Ui.Dialogs.Scanner.Hosts")),
    rangeStart (QStringLiteral("Ui.Dialogs.Scanner.IPAddrStart")),
    rangeEnd   (QStringLiteral("Ui.Dialogs.Scanner.IPAddrEnd")),
    single     (QStringLiteral("Ui.Dialogs.Scanner.SingleHost"))
{
}

const mbClientDialogScannerHost::Strings &mbClientDialogScannerHost::Strings::instance()
{
    static const Strings s;
    return s;
}

mbClientDialogScannerHost::mbClientDialogScannerHost(QWidget *parent) :
    mbCoreDialogBase(Strings::instance().cachePrefix, parent),
    ui(new Ui::mbClientDialogScannerHost)
{
    ui->setupUi(this);

    const Modbus::Defaults &md = Modbus::Defaults::instance();

    ui->lnIPAddrStart->setText("192.168.1.1");
    ui->lnIPAddrEnd  ->setText("192.168.1.2");
    ui->lnSingleHost ->setText(md.host);

    connect(ui->btnAddRange , &QPushButton::clicked, this, &mbClientDialogScannerHost::slotAddRange );
    connect(ui->btnAddSingle, &QPushButton::clicked, this, &mbClientDialogScannerHost::slotAddSingle);
    connect(ui->btnRemove   , &QPushButton::clicked, this, &mbClientDialogScannerHost::slotRemove   );
    connect(ui->btnClear    , &QPushButton::clicked, this, &mbClientDialogScannerHost::slotClear    );
    connect(ui->btnUp       , &QPushButton::clicked, this, &mbClientDialogScannerHost::slotMoveUp   );
    connect(ui->btnDown     , &QPushButton::clicked, this, &mbClientDialogScannerHost::slotMoveDown );

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &mbClientDialogScannerHost::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &mbClientDialogScannerHost::reject);
}

mbClientDialogScannerHost::~mbClientDialogScannerHost()
{
    delete ui;
}

MBSETTINGS mbClientDialogScannerHost::cachedSettings() const
{
    const mbClientDialogScannerHost::Strings &vs = mbClientDialogScannerHost::Strings::instance();
    const QString &prefix = Strings().cachePrefix;

    MBSETTINGS m = mbCoreDialogBase::cachedSettings();
    m[prefix+vs.rangeStart] = ui->lnIPAddrStart->text();
    m[prefix+vs.rangeEnd  ] = ui->lnIPAddrEnd  ->text();
    m[prefix+vs.single    ] = ui->lnSingleHost ->text();

    return m;
}

void mbClientDialogScannerHost::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogBase::setCachedSettings(m);

    const mbClientDialogScannerHost::Strings &vs = mbClientDialogScannerHost::Strings::instance();
    const QString &prefix = Strings().cachePrefix;

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    it = m.find(prefix+vs.rangeStart); if (it != end) ui->lnIPAddrStart->setText(it.value().toString());
    it = m.find(prefix+vs.rangeEnd  ); if (it != end) ui->lnIPAddrEnd  ->setText(it.value().toString());
    it = m.find(prefix+vs.single    ); if (it != end) ui->lnSingleHost ->setText(it.value().toString());
}

bool mbClientDialogScannerHost::getValues(QVariantList &values, const QString &title)
{
    QStringList valueStrings;
    Q_FOREACH (auto v, values)
    {
        valueStrings.append(v.toString());
    }
    const bool result = getValues(valueStrings, title);
    if (result)
    {
        values.clear();
        Q_FOREACH (auto v, valueStrings)
        {
            values.append(QVariant(v));
        }
    }
    return result;
}

bool mbClientDialogScannerHost::getValues(QStringList &values, const QString &title)
{
    if (title.isEmpty())
        setWindowTitle(Strings::instance().title);
    else
        setWindowTitle(title);
    fillForm(values);
    // ----------------------
    switch (QDialog::exec())
    {
    case QDialog::Accepted:
        fillData(values);
        return true;
    }
    return false;
}

void mbClientDialogScannerHost::fillForm(const QStringList &hosts)
{
    ui->lsCurrent->clear();
    Q_FOREACH (const QString &s, hosts)
    {
        ui->lsCurrent->addItem(s);
    }
}

void mbClientDialogScannerHost::fillData(QStringList &hosts)
{
    hosts.clear();
    for (int i = 0; i < ui->lsCurrent->count(); i++)
    {
        QListWidgetItem *item = ui->lsCurrent->item(i);
        hosts.append(item->data(Qt::DisplayRole).toString());
    }
}

void mbClientDialogScannerHost::slotAddRange()
{
    auto parseIPv4 = [](const QString &s, quint32 &out) -> bool
    {
        const QString trimmed = s.trimmed();
        const QStringList parts = trimmed.split('.');
        if (parts.size() != 4)
            return false;
        quint32 ip = 0;
        for (int i = 0; i < 4; ++i) 
        {
            bool ok = false;
            const int val = parts[i].toInt(&ok);
            if (!ok || val < 0 || val > 255)
                return false;
            ip = (ip << 8) | static_cast<quint32>(val);
        }
        out = ip;
        return true;
    };

    auto ipv4ToString = [](quint32 ip) -> QString
    {
        return QString("%1.%2.%3.%4")
               .arg((ip >> 24) & 0xFF)
               .arg((ip >> 16) & 0xFF)
               .arg((ip >> 8) & 0xFF)
               .arg(ip & 0xFF);
    };

    QString ipstart = ui->lnIPAddrStart->text();
    QString ipend = ui->lnIPAddrEnd->text();

    quint32 start = 0, end = 0;
    if (!parseIPv4(ipstart, start))
        return;
    if (!parseIPv4(ipend, end))
        return;
    quint32 inc = (start > end) ? static_cast<quint32>(-1) : 1;
    for (quint32 ip = start;; ip += inc)
    {
        ui->lsCurrent->addItem(ipv4ToString(ip));
        if (ip == end)
            break;
    }
}

void mbClientDialogScannerHost::slotAddSingle()
{
    QString s = ui->lnSingleHost->text();
    ui->lsCurrent->addItem(s);
}

void mbClientDialogScannerHost::slotRemove()
{
    QList<QListWidgetItem*> ls = ui->lsCurrent->selectedItems();
    Q_FOREACH (QListWidgetItem *w, ls)
    {
        delete w;
    }
}

void mbClientDialogScannerHost::slotClear()
{
    ui->lsCurrent->clear();
}

void mbClientDialogScannerHost::slotMoveUp()
{
    QList<QListWidgetItem*> ls = ui->lsCurrent->selectedItems();
    if (ls.count())
    {
        QListWidgetItem *w = ls.first();
        int i = ui->lsCurrent->row(w);
        if (i > 0)
        {
            ui->lsCurrent->takeItem(i);
            i--;
            ui->lsCurrent->insertItem(i, w);
            ui->lsCurrent->setCurrentItem(w);
        }
    }
}

void mbClientDialogScannerHost::slotMoveDown()
{
    QList<QListWidgetItem*> ls = ui->lsCurrent->selectedItems();
    if (ls.count())
    {
        QListWidgetItem *w = ls.first();
        int i = ui->lsCurrent->row(w);
        if (i < (ui->lsCurrent->count()-1))
        {
            ui->lsCurrent->takeItem(i);
            i++;
            ui->lsCurrent->insertItem(i, w);
            ui->lsCurrent->setCurrentItem(w);
        }
    }
}
