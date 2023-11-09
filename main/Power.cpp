#include <cstring>
#include "freertos/FreeRTOS.h"

#include "driver/gpio.h"
#include "driver/i2c.h"

#include "Power.hpp"

#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000

static int readI2C(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len);
static int writeI2C(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len);

static const char *TAG = "Power";
Logger *Power::log = Logger::getInstance();

Power::Power() {}

void Power::pmuInit()
{
    pmu = new XPowersPMU();

    if (!pmu->begin(AXP2101_SLAVE_ADDRESS, readI2C, writeI2C))
        log->error(TAG, "PMU not available");

    log->info(TAG, "Chip ID: 0x%x", pmu->getChipID());

    log->info(TAG, "Setting voltage and current limit to 4.36V 1000mA");
    pmu->setVbusVoltageLimit(XPOWERS_AXP2101_VBUS_VOL_LIM_4V36);
    pmu->setVbusCurrentLimit(XPOWERS_AXP2101_VBUS_CUR_LIM_1000MA);

    log->info(TAG, "Setting VSYS power down voltage to 3.0V");
    pmu->setSysPowerDownVoltage(3000);

    pmu->disableTSPinMeasure();
    pmu->enableBattDetection();
    pmu->enableVbusVoltageMeasure();
    pmu->enableBattVoltageMeasure();
    pmu->enableSystemVoltageMeasure();
    pmu->setChargingLedMode(XPOWERS_CHG_LED_CTRL_CHG);

    // gpioInit();
    pmu->disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
    pmu->clearIrqStatus();

    log->info(TAG, "Setting Precharge current: 200mA");
    pmu->setPrechargeCurr(XPOWERS_AXP2101_PRECHARGE_200MA);
    log->info(TAG, "Setting Charge Termination Current: 25mA");
    pmu->setChargerTerminationCurr(XPOWERS_AXP2101_CHG_ITERM_25MA);

    log->info(TAG, "Setting Charge Constant Current: 1.0A");
    pmu->setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_1000MA);

    log->info(TAG, "Setting Charge Target Voltage : 4.2V");
    pmu->setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V2);

    pmu->setLowBatWarnThreshold(10);
    pmu->setLowBatShutdownThreshold(5);
    pmu->fuelGaugeControl(true, true);
}

Power *Power::init()
{
    // TODO: Move this out if other device will be on I2C bus
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO,
        .scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = I2C_MASTER_FREQ_HZ
        },
        .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
    };

    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);

    Power *power = new Power();
    power->pmuInit();
    return power;
}

uint16_t Power::getBatteryVoltage()
{
    return pmu->getBattVoltage();
}

static int readI2C(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, devAddr, &regAddr, 1, data, len, pdMS_TO_TICKS(1000));
}

static int writeI2C(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len)
{
    uint8_t write_buf[len + 1];
    write_buf[0] = regAddr;
    std::memcpy(write_buf + 1, data, len);

    return i2c_master_write_to_device(I2C_MASTER_NUM, devAddr, write_buf, len + 1, pdMS_TO_TICKS(1000));
}