#ifndef CLIENT_PORTSTATISTICSUI_H
#define CLIENT_PORTSTATISTICSUI_H

#include <gui/statistics/core_portstatisticsui.h>

class mbServerPort;

namespace Ui {
class mbServerPortStatisticsUi;
}

class mbServerPortStatisticsUi : public mbCorePortStatisticsUi
{
    Q_OBJECT

public:
    explicit mbServerPortStatisticsUi(mbServerPort *port, QWidget *parent = nullptr);
    ~mbServerPortStatisticsUi();

public:
    inline mbServerPort *port() const { return reinterpret_cast<mbServerPort*>(m_port); }

public:
    void syncStatistics() override;

private:
    Ui::mbServerPortStatisticsUi *ui;
};

#endif // CLIENT_PORTSTATISTICSUI_H
