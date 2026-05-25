#include "core_widgetsettingslog.h"
#include "ui_core_widgetsettingslog.h"

#include "core_modelsettingslogcolors.h"
#include "core_delegatesettingslogcolors.h"

#include <core.h>
#include <gui/core_ui.h>
#include <gui/dialogs/core_dialogs.h>
#include <gui/logview/core_logview.h>

mbCoreWidgetSettingsLog::mbCoreWidgetSettingsLog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mbCoreWidgetSettingsLog),
    m_modelColors(new mbCoreModelSettingsLogColors(this))
{
    ui->setupUi(this);

    ui->spMaxSize->setMinimum(MBLOGVIEW_MAXSIZE_MIN);
    ui->spMaxSize->setMaximum(INT_MAX);
    setLogViewFont(mbCoreLogView::Defaults::instance().font);
    connect(ui->btnDefaultColors, &QToolButton::clicked,
            m_modelColors, &mbCoreModelSettingsLogColors::setDefaultColors);
    ui->viewColors->setModel(m_modelColors);
    ui->viewColors->setItemDelegate(new mbCoreDelegateSettingsLogColors(this));

    connect(ui->btnFont, &QPushButton::clicked, this, &mbCoreWidgetSettingsLog::slotFont);

}

mbCoreWidgetSettingsLog::~mbCoreWidgetSettingsLog()
{
    delete ui;
}

mb::LogFlags mbCoreWidgetSettingsLog::logFlags() const
{
    mb::LogFlags flags = mb::LogFlags();
    flags = static_cast<mb::LogFlags>(flags | (ui->chbLogError  ->isChecked() * mb::Log_Error   ));
    flags = static_cast<mb::LogFlags>(flags | (ui->chbLogWarning->isChecked() * mb::Log_Warning ));
    flags = static_cast<mb::LogFlags>(flags | (ui->chbLogInfo   ->isChecked() * mb::Log_Info    ));
    flags = static_cast<mb::LogFlags>(flags | (ui->chbLogTx     ->isChecked() * mb::Log_Tx      ));
    flags = static_cast<mb::LogFlags>(flags | (ui->chbLogRx     ->isChecked() * mb::Log_Rx      ));
    flags = static_cast<mb::LogFlags>(flags | (ui->chbLogDebug  ->isChecked() * mb::Log_Debug   ));
    return flags;
}

void mbCoreWidgetSettingsLog::setLogFlags(mb::LogFlags flags)
{
    ui->chbLogError  ->setChecked(flags & mb::Log_Error  );
    ui->chbLogWarning->setChecked(flags & mb::Log_Warning);
    ui->chbLogInfo   ->setChecked(flags & mb::Log_Info   );
    ui->chbLogTx     ->setChecked(flags & mb::Log_Tx     );
    ui->chbLogRx     ->setChecked(flags & mb::Log_Rx     );
    ui->chbLogDebug  ->setChecked(flags & mb::Log_Debug  );
}

bool mbCoreWidgetSettingsLog::useTimestamp() const
{
    return ui->chbUseTimestamp->isChecked();
}

void mbCoreWidgetSettingsLog::setUseTimestamp(bool use)
{
    ui->chbUseTimestamp->setChecked(use);
}

QString mbCoreWidgetSettingsLog::formatDateTime() const
{
    return ui->lnFormat->text();
}

void mbCoreWidgetSettingsLog::setFormatDateTime(const QString &format)
{
    ui->lnFormat->setText(format);
}

int mbCoreWidgetSettingsLog::logViewMaxSize() const
{
    return ui->spMaxSize->value();
}

void mbCoreWidgetSettingsLog::setLogViewMaxSize(int sz)
{
    ui->spMaxSize->setValue(sz);
}

QString mbCoreWidgetSettingsLog::logViewFont() const
{
    QFont f = getLogViewFont();
    return f.toString();
}

void mbCoreWidgetSettingsLog::setLogViewFont(const QString &font)
{
    QFont f;
    f.fromString(font);
    setLogViewFont(f);
}

QVariant mbCoreWidgetSettingsLog::logViewColorMap() const
{
    return mb::toVariant(m_modelColors->colorMap());
}

void mbCoreWidgetSettingsLog::setLogViewColorMap(const QVariant &v)
{
    m_modelColors->setColorMap(mb::toColorMap(v));
}

QFont mbCoreWidgetSettingsLog::getLogViewFont() const
{
    QFont f = ui->cmbFontFamily->currentFont();
    f.setPointSize(ui->spFontSize->value());
    return f;
}

void mbCoreWidgetSettingsLog::setLogViewFont(const QFont &f)
{
    ui->cmbFontFamily->setCurrentFont(f);
    ui->spFontSize->setValue(f.pointSize());
}

void mbCoreWidgetSettingsLog::slotFont()
{
    mbCoreUi *ui = mbCore::globalCore()->coreUi();
    QFont f = getLogViewFont();
    if (ui->dialogsCore()->getFont(f, ui, "Font"))
    {
        setLogViewFont(f);
    }
}
