#include "core_devicestatisticsui.h"

#include <QLineEdit>
#include <QPlainTextEdit>

#include <project/core_device.h>

mbCoreDeviceStatisticsUi::mbCoreDeviceStatisticsUi(mbCoreDevice *device, QWidget *parent) :
    mbCoreStatisticsUi(parent),
    m_device(device)
{
    m_timerId = 0;
    connect(m_device, &mbCoreDevice::nameChanged, this, &mbCoreDeviceStatisticsUi::setStatWindowTitle);
    setStatWindowTitle(m_device->name());
}

mbCoreDeviceStatisticsUi::~mbCoreDeviceStatisticsUi()
{
}

QString mbCoreDeviceStatisticsUi::name() const
{
    return m_device ? m_device->name() : QString();
}

void mbCoreDeviceStatisticsUi::syncStatistics()
{
    syncStatisticsCoreInner();
}

void mbCoreDeviceStatisticsUi::resetStatistics()
{
    m_device->resetStatistics();
    syncStatistics();
}

bool mbCoreDeviceStatisticsUi::event(QEvent *event)
{
    if (event->type() == QEvent::Show)
    {
        syncStatistics();
        startScanning();
    }
    else if (event->type() == QEvent::Hide)
    {
        stopScanning();
    }
    else if (event->type() == QEvent::Timer)
    {
        syncStatistics();
    }
    return QWidget::event(event);
}

void mbCoreDeviceStatisticsUi::syncStatisticsCoreInner()
{
    auto s = m_device->statisticsCore();
    m_ui.lnSinceTimestamp      ->setText(mb::toString(s.sinceTimestamp      ));
    m_ui.lnLastStatus          ->setText(mb::toString(s.lastStatus          ));
    m_ui.lnLastTimestamp       ->setText(mb::toString(s.lastTimestamp       ));
    m_ui.lnLastSuccessTimestamp->setText(mb::toString(s.lastSuccessTimestamp));
    m_ui.lnLastErrorStatus     ->setText(mb::toString(s.lastErrorStatus     ));
    m_ui.lnLastErrorTimestamp  ->setText(mb::toString(s.lastErrorTimestamp  ));
    m_ui.txtLastErrorText      ->setPlainText(s.lastErrorText                );
    m_ui.lnCountTx             ->setText(QString::number(s.countTx          ));
    m_ui.lnCountRx             ->setText(QString::number(s.countRx          ));
    m_ui.lnCountGood           ->setText(QString::number(s.countGood        ));
    m_ui.lnCountBad            ->setText(QString::number(s.countBad         ));
    m_ui.lnCountBadStandard    ->setText(QString::number(s.countBadStandard ));
}

void mbCoreDeviceStatisticsUi::startScanning()
{
    if (!isScanning())
    {
        m_timerId = startTimer(500);
    }
}

void mbCoreDeviceStatisticsUi::stopScanning()
{
    if (isScanning())
    {
        killTimer(m_timerId);
        m_timerId = 0;
    }
}
