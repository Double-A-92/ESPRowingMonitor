#pragma once

#include "../utils/EEPROM/EEPROM.service.interface.h"
#include "../utils/configuration.h"
#include "./flywheel.service.interface.h"
#include "./stroke.controller.interface.h"
#include "./stroke.service.interface.h"

class StrokeController final : public IStrokeController
{
    IStrokeService &strokeService;
    IFlywheelService &flywheelService;
    IEEPROMService &eepromService;

    unsigned int previousRevCount = 0;
    unsigned int previousStrokeCount = 0U;
    unsigned long previousRawImpulseCount = 0U;

    RowingDataModels::RowingMetrics rowerState{
        0.0,
        0ULL,
        0ULL,
        0U,
        0U,
        0U,
        0.0,
        0.0,
        std::vector<float>{},
    };

    RowingDataModels::FlywheelData flywheelData{
        0UL,
        0UL,
        0ULL,
        0UL,
        0UL,
        0UL,
    };

public:
    StrokeController(IStrokeService &_strokeService, IFlywheelService &_flywheelService, IEEPROMService &eepromService);

    void begin() override;
    void update() override;

    [[nodiscard]] const RowingDataModels::RowingMetrics &getAllData() const override;
    [[nodiscard]] unsigned int getPreviousRevCount() const override;
    void setPreviousRevCount() override;
    [[nodiscard]] unsigned int getPreviousStrokeCount() const override;
    void setPreviousStrokeCount() override;

    [[nodiscard]] unsigned long getPreviousRawImpulseCount() const override;
    void setPreviousRawImpulseCount() override;
    [[nodiscard]] unsigned long getRawImpulseCount() const override;
    [[nodiscard]] unsigned long getLastImpulseTime() const override;

    [[nodiscard]] unsigned long getDeltaTime() const override;
    [[nodiscard]] unsigned long long getLastRevTime() const override;
    [[nodiscard]] unsigned int getRevCount() const override;
    [[nodiscard]] unsigned long long getLastStrokeTime() const override;
    [[nodiscard]] unsigned short getStrokeCount() const override;
    [[nodiscard]] Configurations::precision getDistance() const override;
    [[nodiscard]] Configurations::precision getRecoveryDuration() const override;
    [[nodiscard]] Configurations::precision getDriveDuration() const override;
    [[nodiscard]] short getAvgStrokePower() const override;
    [[nodiscard]] unsigned char getDragFactor() const override;
};