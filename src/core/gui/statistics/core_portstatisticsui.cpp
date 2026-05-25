#include "core_portstatisticsui.h"

#include <QLineEdit>
#include <QPlainTextEdit>

#include <project/core_port.h>

mbCorePortStatisticsUi::mbCorePortStatisticsUi(mbCorePort *port, QWidget *parent) :
    mbCoreStatisticsUi(parent),
    m_port(port)
{
    m_timerId = 0;
    connect(m_port, &mbCorePort::nameChanged, this, &mbCorePortStatisticsUi::setStatWindowTitle);
    setStatWindowTitle(m_port->name());
}

mbCorePortStatisticsUi::~mbCorePortStatisticsUi()
{
}

QString mbCorePortStatisticsUi::name() const
{
    return m_port ? m_port->name() : QString();
}

void mbCorePortStatisticsUi::syncStatistics()
{
    syncStatisticsCoreInner();
}

void mbCorePortStatisticsUi::resetStatistics()
{
    m_port->resetStatistics();
    syncStatistics();
}

bool mbCorePortStatisticsUi::event(QEvent *event)
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

void mbCorePortStatisticsUi::syncStatisticsCoreInner()
{
    auto s = m_port->statisticsCore();
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
    m_ui.lnCountBadTimeout     ->setText(QString::number(s.countBadTimeout  ));
    m_ui.lnCountBadStandard    ->setText(QString::number(s.countBadStandard ));
    m_ui.lnCountBadCRC         ->setText(QString::number(s.countBadCRC      ));
    m_ui.lnCycleCount          ->setText(QString::number(s.cycleCount       ));
    m_ui.lnCycleSumDuration    ->setText(QString::number(s.cycleSumDuration ));
    m_ui.lnCycleLastDuration   ->setText(QString::number(s.cycleLastDuration));
    m_ui.lnCycleMinDuration    ->setText(QString::number(s.cycleMinDuration ));
    m_ui.lnCycleMaxDuration    ->setText(QString::number(s.cycleMaxDuration ));
    m_ui.lnCycleAvgDuration    ->setText(QString::number(s.cycleAvgDuration ));
}

void mbCorePortStatisticsUi::startScanning()
{
    if (!isScanning())
    {
        m_timerId = startTimer(500);
    }
}

void mbCorePortStatisticsUi::stopScanning()
{
    if (isScanning())
    {
        killTimer(m_timerId);
        m_timerId = 0;
    }
}
