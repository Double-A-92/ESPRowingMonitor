#include "pm5.service.h"

#include <algorithm>

#include "../../../utils/enums.h"
#include "../../../globals.h"

NimBLEService *Pm5Service::setup(NimBLEServer *pServer, BleServiceFlag bleServiceFlag)
{
    if (bleServiceFlag != BleServiceFlag::Pm5Service)
    {
        return nullptr;
    }

    Log.verboseln("Setting up PM5 Service");

    setupDiscoveryService(pServer);
    setupControlService(pServer);
    setupRowingService(pServer);

    return rowingService;
}

void Pm5Service::setupDiscoveryService(NimBLEServer *pServer)
{
    discoveryService = pServer->createService(Pm5BleFlags::discoverySvcUuid);

    modelNumberCharacteristic = discoveryService->createCharacteristic(
        Pm5BleFlags::modelNumberUuid,
        NIMBLE_PROPERTY::READ);
    modelNumberCharacteristic->setValue("PM5");

    serialNumberCharacteristic = discoveryService->createCharacteristic(
        Pm5BleFlags::serialNumberUuid,
        NIMBLE_PROPERTY::READ);
    serialNumberCharacteristic->setValue(Configurations::serialNumber);

    hwRevisionCharacteristic = discoveryService->createCharacteristic(
        Pm5BleFlags::hwRevisionUuid,
        NIMBLE_PROPERTY::READ);
    hwRevisionCharacteristic->setValue("100");

    fwRevisionCharacteristic = discoveryService->createCharacteristic(
        Pm5BleFlags::fwRevisionUuid,
        NIMBLE_PROPERTY::READ);
    fwRevisionCharacteristic->setValue("207"); // Emulate a valid FW version

    mfgNameCharacteristic = discoveryService->createCharacteristic(
        Pm5BleFlags::mfgNameUuid,
        NIMBLE_PROPERTY::READ);
    mfgNameCharacteristic->setValue("Concept2");

    machineTypeCharacteristic = discoveryService->createCharacteristic(
        Pm5BleFlags::machineTypeUuid,
        NIMBLE_PROPERTY::READ);
    // Machine Type 0x01 = Static Rower per some docs, or 0x05 per OpenRowingMonitor?
    // OpenRowingMonitor uses 0x05.
    unsigned char machineType[] = {0x05};
    machineTypeCharacteristic->setValue(machineType, 1);

    discoveryService->start();
}

void Pm5Service::setupControlService(NimBLEServer *pServer)
{
    controlService = pServer->createService(Pm5BleFlags::controlSvcUuid);

    // PM5 Control Characteristics (Basic implementation stub)
    controlService->createCharacteristic(
        Pm5BleFlags::controlTransmitUuid,
        NIMBLE_PROPERTY::WRITE);
    
    controlService->createCharacteristic(
        Pm5BleFlags::controlReceiveUuid,
        NIMBLE_PROPERTY::NOTIFY);

    controlService->start();
}

void Pm5Service::setupRowingService(NimBLEServer *pServer)
{
    rowingService = pServer->createService(Pm5BleFlags::rowingSvcUuid);

    generalStatusCharacteristic = rowingService->createCharacteristic(
        Pm5BleFlags::generalStatusUuid,
        NIMBLE_PROPERTY::NOTIFY);

    strokeDataCharacteristic = rowingService->createCharacteristic(
        Pm5BleFlags::strokeDataUuid,
        NIMBLE_PROPERTY::NOTIFY);

    multiplexedInfoCharacteristic = rowingService->createCharacteristic(
        Pm5BleFlags::multiplexedInfoUuid,
        NIMBLE_PROPERTY::NOTIFY);

    rowingService->start();
}

void Pm5Service::broadcastBaseMetrics(const RowingDataModels::RowingMetrics &data)
{
    updateStrokeData(data);

    updateGeneralStatus(data);
}

void Pm5Service::broadcastBaseMetrics(const BleMetricsModel::BleMetricsData &data)
{
    // Implementation not needed for PM5 as it uses RowingMetrics, but required by interface
}

const std::vector<unsigned char> &Pm5Service::getClientIds() const
{
    // Simplified: Return non-empty if any notify enabled.
    // Since we removed getSubscribedCount, we can just return a dummy if we needed to,
    // but effectively this is unused by BluetoothController for PM5 logic.
    static std::vector<unsigned char> clients;
    clients.clear();
    
    // We cannot easily check subscription count without callbacks.
    // For now returning empty. The controller calls broadcastBaseMetrics blindly for PM5 anyway.

    return clients;
}

void Pm5Service::updateStrokeData(const RowingDataModels::RowingMetrics &data)
{
    // Only update if stroke count has increased
    if (data.strokeCount <= lastStrokeCount && lastStrokeCount != 0)
    {
        // Use cached data if available (re-notify? usually only notify on change)
        // If we want to strictly emulate PM5, we might notify periodically?
        // But PM5 usually notifies on stroke event.
        // Let's just return if no new stroke, unless it's the very first one?
        // Actually, broadcastBaseMetrics might be called for keep-alive.
        // If no new stroke, we just keep the old valid data in `lastStrokeData` but do we Notify it again?
        // If connection is new, maybe?
        return; 
    }

    Pm5StrokeData payload{};
    
    unsigned long elapsedTimeCs = data.lastStrokeTime / 10000;
    
    // Elapsed Time (UInt24)
    payload.elapsedTime[0] = static_cast<unsigned char>(elapsedTimeCs & 0xFF);
    payload.elapsedTime[1] = static_cast<unsigned char>((elapsedTimeCs >> 8) & 0xFF);
    payload.elapsedTime[2] = static_cast<unsigned char>((elapsedTimeCs >> 16) & 0xFF);

    // Distance (UInt24, 0.1m)
    auto distDeciMeters = static_cast<unsigned long>(data.distance * 10);
    payload.distance[0] = static_cast<unsigned char>(distDeciMeters & 0xFF);
    payload.distance[1] = static_cast<unsigned char>((distDeciMeters >> 8) & 0xFF);
    payload.distance[2] = static_cast<unsigned char>((distDeciMeters >> 16) & 0xFF);

    // Drive Time (UInt8, 0.01s)
    payload.driveTime = static_cast<unsigned char>(data.driveDuration / 10000);

    // Stroke Recovery Time (UInt16, 0.01s)
    payload.strokeRecoveryTime = static_cast<unsigned short>(data.recoveryDuration / 10000);

    // Stroke Distance (0.01m resolution means x * 100)
    // Calculate delta from last saved distance
    double strokeDist = data.distance - lastDistance;
    strokeDist = std::max<double>(strokeDist, 0); // Reset handling?

    payload.strokeDistance = static_cast<unsigned short>(strokeDist * 100);

    // Update state
    lastDistance = data.distance;
    lastStrokeCount = data.strokeCount;

    // Peak Drive Force (UInt16, 0.1 N)
    if (!data.driveHandleForces.empty())
    {
        float maxForce = *std::max_element(data.driveHandleForces.begin(), data.driveHandleForces.end());
        payload.peakDriveForce = static_cast<unsigned short>(maxForce * 10);

        float sumForce = std::accumulate(data.driveHandleForces.begin(), data.driveHandleForces.end(), 0.0f);
        float avgForce = sumForce / data.driveHandleForces.size();
        payload.avgDriveForce = static_cast<unsigned short>(avgForce * 10);
    }
    else
    {
        payload.peakDriveForce = 0;
        payload.avgDriveForce = 0;
    }
    
    // Work Per Stroke (UInt16, Joules)
    double cycleTimeSec = (data.driveDuration + data.recoveryDuration) / 1000000.0;
    double workJ = data.avgStrokePower * cycleTimeSec;
    payload.workPerStroke = static_cast<unsigned short>(workJ);

    payload.strokeCount = static_cast<unsigned short>(data.strokeCount);
    
    // Drive Length - unknown, keep 0
    payload.driveLength = 0;

    // Cache it
    lastStrokeData = payload;

    strokeDataCharacteristic->setValue(reinterpret_cast<uint8_t*>(&payload), sizeof(payload));
    strokeDataCharacteristic->notify();
}

void Pm5Service::updateGeneralStatus(const RowingDataModels::RowingMetrics &data)
{
    Pm5GeneralStatus payload{};
    
    unsigned long elapsedTimeCs = data.lastRevTime / 10000; // Using lastRevTime for continuous update?
    
    payload.elapsedTime[0] = static_cast<unsigned char>(elapsedTimeCs & 0xFF);
    payload.elapsedTime[1] = static_cast<unsigned char>((elapsedTimeCs >> 8) & 0xFF);
    payload.elapsedTime[2] = static_cast<unsigned char>((elapsedTimeCs >> 16) & 0xFF);
    
    auto distDeciMeters = static_cast<unsigned long>(data.distance * 10);
    payload.distance[0] = static_cast<unsigned char>(distDeciMeters & 0xFF);
    payload.distance[1] = static_cast<unsigned char>((distDeciMeters >> 8) & 0xFF);
    payload.distance[2] = static_cast<unsigned char>((distDeciMeters >> 16) & 0xFF);

    payload.workoutType = 0; // Just Row
    payload.intervalType = 0;
    payload.workoutState = 1; // Work
    payload.rowingState = 1; // Active
    payload.strokeState = 1; // Drive?
    
    // Total Work Distance (UInt24, 1m)
    auto totalWorkDist = static_cast<unsigned long>(data.distance);
    payload.totalWorkDistance[0] = static_cast<unsigned char>(totalWorkDist & 0xFF);
    payload.totalWorkDistance[1] = static_cast<unsigned char>((totalWorkDist >> 8) & 0xFF);
    payload.totalWorkDistance[2] = static_cast<unsigned char>((totalWorkDist >> 16) & 0xFF);

    // Workout Duration (UInt24, 0.01s) - 0 for Just Row (no target)
    payload.workoutDuration[0] = 0; 
    payload.workoutDuration[1] = 0; 
    payload.workoutDuration[2] = 0;

    payload.workoutDurationType = 0; // Time (default)

    // Drag Factor (UInt8)
    // Coeff * 1e6. e.g. 0.000115 * 1000000 = 115.
    payload.dragFactor = static_cast<unsigned char>(std::lround(data.dragCoefficient * 1e6)); 

    generalStatusCharacteristic->setValue(reinterpret_cast<uint8_t*>(&payload), sizeof(payload));
    generalStatusCharacteristic->notify();
}
