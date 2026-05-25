#ifndef CLIENT_DEVICESTATISTICSUI_H
#define CLIENT_DEVICESTATISTICSUI_H

#include <gui/statistics/core_devicestatisticsui.h>

class mbClientDevice;

namespace Ui {
class mbClientDeviceStatisticsUi;
}

class mbClientDeviceStatisticsUi : public mbCoreDeviceStatisticsUi
{
    Q_OBJECT

public:
    explicit mbClientDeviceStatisticsUi(mbClientDevice *device, QWidget *parent = nullptr);
    ~mbClientDeviceStatisticsUi();

public:
    inline mbClientDevice *device() const { return reinterpret_cast<mbClientDevice*>(m_device); }

public:
    void syncStatistics() override;

private:
    Ui::mbClientDeviceStatisticsUi *ui;
};

#endif // CLIENT_DEVICESTATISTICSUI_H
