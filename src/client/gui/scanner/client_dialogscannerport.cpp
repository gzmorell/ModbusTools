#include "client_dialogscannerport.h"
#include "ui_client_dialogscannerport.h"

mbClientDialogScannerPort::Strings::Strings() :
    title(QStringLiteral("Edit ports")),
    cachePrefix(QStringLiteral("Ui.Dialogs.Scanner.Ports")),
    rangeStart(QStringLiteral("Ui.Dialogs.Scanner.PortStart")),
    rangeEnd  (QStringLiteral("Ui.Dialogs.Scanner.PortEnd")),
    single    (QStringLiteral("Ui.Dialogs.Scanner.SinglePort"))
{
}

const mbClientDialogScannerPort::Strings &mbClientDialogScannerPort::Strings::instance()
{
    static const Strings s;
    return s;
}

mbClientDialogScannerPort::mbClientDialogScannerPort(QWidget *parent) :
    mbCoreDialogBase(Strings::instance().cachePrefix, parent),
    ui(new Ui::mbClientDialogScannerPort)
{
    ui->setupUi(this);

    const Modbus::Defaults &md = Modbus::Defaults::instance();

    ui->spPortStart->setMinimum(1);
    ui->spPortStart->setMaximum(USHRT_MAX);
    ui->spPortEnd  ->setMinimum(1);
    ui->spPortEnd  ->setMaximum(USHRT_MAX);
    ui->spSinglePort ->setMinimum(1);
    ui->spSinglePort ->setMaximum(USHRT_MAX);

    ui->spPortStart->setValue(md.port);
    ui->spPortEnd  ->setValue(md.port + 1);
    ui->spSinglePort ->setValue(md.port);

    connect(ui->btnAddRange , &QPushButton::clicked, this, &mbClientDialogScannerPort::slotAddRange );
    connect(ui->btnAddSingle, &QPushButton::clicked, this, &mbClientDialogScannerPort::slotAddSingle);
    connect(ui->btnRemove   , &QPushButton::clicked, this, &mbClientDialogScannerPort::slotRemove   );
    connect(ui->btnClear    , &QPushButton::clicked, this, &mbClientDialogScannerPort::slotClear    );
    connect(ui->btnUp       , &QPushButton::clicked, this, &mbClientDialogScannerPort::slotMoveUp   );
    connect(ui->btnDown     , &QPushButton::clicked, this, &mbClientDialogScannerPort::slotMoveDown );

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &mbClientDialogScannerPort::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &mbClientDialogScannerPort::reject);
}

mbClientDialogScannerPort::~mbClientDialogScannerPort()
{
    delete ui;
}

MBSETTINGS mbClientDialogScannerPort::cachedSettings() const
{
    const mbClientDialogScannerPort::Strings &vs = mbClientDialogScannerPort::Strings::instance();
    const QString &prefix = Strings().cachePrefix;

    MBSETTINGS m = mbCoreDialogBase::cachedSettings();
    m[prefix+vs.rangeStart] = ui->spPortStart->value();
    m[prefix+vs.rangeEnd  ] = ui->spPortEnd  ->value();
    m[prefix+vs.single    ] = ui->spSinglePort->value();

    return m;
}

void mbClientDialogScannerPort::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogBase::setCachedSettings(m);

    const mbClientDialogScannerPort::Strings &vs = mbClientDialogScannerPort::Strings::instance();
    const QString &prefix = Strings().cachePrefix;

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    it = m.find(prefix+vs.rangeStart); if (it != end) ui->spPortStart ->setValue(it.value().toInt());
    it = m.find(prefix+vs.rangeEnd  ); if (it != end) ui->spPortEnd   ->setValue(it.value().toInt());
    it = m.find(prefix+vs.single    ); if (it != end) ui->spSinglePort->setValue(it.value().toInt());
}

bool mbClientDialogScannerPort::getValues(QVariantList &values, const QString &title)
{
    QList<uint16_t> valuePorts;
    Q_FOREACH (auto v, values)
    {
        valuePorts.append(v.toUInt());
    }
    const bool result = getValues(valuePorts, title);
    if (result)
    {
        values.clear();
        Q_FOREACH (auto v, valuePorts)
        {
            values.append(QVariant(v));
        }
    }
    return result;
}

bool mbClientDialogScannerPort::getValues(QList<uint16_t> &values, const QString &title)
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

void mbClientDialogScannerPort::fillForm(const QList<uint16_t> &values)
{
    ui->lsCurrent->clear();
    Q_FOREACH (auto v, values)
    {
        ui->lsCurrent->addItem(QString::number(v));
    }
}

void mbClientDialogScannerPort::fillData(QList<uint16_t> &values)
{
    values.clear();
    for (int i = 0; i < ui->lsCurrent->count(); i++)
    {
        QListWidgetItem *item = ui->lsCurrent->item(i);
        values.append(item->data(Qt::DisplayRole).toString().toUShort());
    }
}

void mbClientDialogScannerPort::slotAddRange()
{
    uint16_t start = static_cast<uint16_t>(ui->spPortStart->value());
    uint16_t end = static_cast<uint16_t>(ui->spPortEnd->value());

    quint32 inc = (start > end) ? static_cast<quint32>(-1) : 1;
    for (uint16_t p = start;; p += inc)
    {
        ui->lsCurrent->addItem(QString::number(p));
        if (p == end)
            break;
    }
}

void mbClientDialogScannerPort::slotAddSingle()
{
    uint16_t p = static_cast<uint16_t>(ui->spSinglePort->value());
    ui->lsCurrent->addItem(QString::number(p));
}

void mbClientDialogScannerPort::slotRemove()
{
    QList<QListWidgetItem*> ls = ui->lsCurrent->selectedItems();
    Q_FOREACH (QListWidgetItem *w, ls)
    {
        delete w;
    }
}

void mbClientDialogScannerPort::slotClear()
{
    ui->lsCurrent->clear();
}

void mbClientDialogScannerPort::slotMoveUp()
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

void mbClientDialogScannerPort::slotMoveDown()
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
