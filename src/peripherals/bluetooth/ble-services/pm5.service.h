#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>

#include "NimBLEDevice.h"

#include "../../../utils/enums.h"
#include "../../../rower/stroke.model.h"
#include "./pm5.service.interface.h"

class Pm5Service : public IPm5Service
{
    NimBLEService *discoveryService = nullptr;
    NimBLEService *controlService = nullptr;
    NimBLEService *rowingService = nullptr;

    NimBLECharacteristic *generalStatusCharacteristic = nullptr;
    NimBLECharacteristic *strokeDataCharacteristic = nullptr;
    NimBLECharacteristic *multiplexedInfoCharacteristic = nullptr;

    // Discovery Characteristics
    NimBLECharacteristic *modelNumberCharacteristic = nullptr;
    NimBLECharacteristic *serialNumberCharacteristic = nullptr;
    NimBLECharacteristic *hwRevisionCharacteristic = nullptr;
    NimBLECharacteristic *fwRevisionCharacteristic = nullptr;
    NimBLECharacteristic *mfgNameCharacteristic = nullptr;
    NimBLECharacteristic *machineTypeCharacteristic = nullptr;

    void setupDiscoveryService(NimBLEServer *pServer);
    void setupControlService(NimBLEServer *pServer);
    void setupRowingService(NimBLEServer *pServer);

#pragma pack(push, 1)
    struct Pm5StrokeData
    {
        unsigned char elapsedTime[3];
        unsigned char distance[3];
        unsigned char driveLength;
        unsigned char driveTime;
        unsigned short strokeRecoveryTime;
        unsigned short strokeDistance;
        unsigned short peakDriveForce;
        unsigned short avgDriveForce;
        unsigned short workPerStroke;
        unsigned short strokeCount;
    };

    struct Pm5GeneralStatus
    {
        unsigned char elapsedTime[3];
        unsigned char distance[3];
        unsigned char workoutType;
        unsigned char intervalType;
        unsigned char workoutState;
        unsigned char rowingState;
        unsigned char strokeState;
        unsigned char totalWorkDistance[3];
        unsigned char workoutDuration[3];
        unsigned char workoutDurationType;
        unsigned char dragFactor;
    };
#pragma pack(pop)



    unsigned short lastStrokeCount = 0;
    Configurations::precision lastDistance = 0.0;
    Pm5StrokeData lastStrokeData = {};

public:
    NimBLEService *setup(NimBLEServer *pServer, BleServiceFlag bleServiceFlag) override;
    void broadcastBaseMetrics(const RowingDataModels::RowingMetrics &data);
    void broadcastBaseMetrics(const BleMetricsModel::BleMetricsData &data) override;
    const std::vector<unsigned char> &getClientIds() const override;

    // Helper functions for packing data
    void updateStrokeData(const RowingDataModels::RowingMetrics &data);
    void updateGeneralStatus(const RowingDataModels::RowingMetrics &data);
};
