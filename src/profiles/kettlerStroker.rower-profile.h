#pragma once

#include "../utils/enums.h"

// NOLINTBEGIN(cppcoreguidelines-macro-usage,cppcoreguidelines-macro-to-enum)

#define DEVICE_NAME Kettler Stroker
#define MODEL_NUMBER 07982-500
#define ADD_BLE_SERVICE_TO_DEVICE_NAME false
#define DEFAULT_BLE_SERVICE BleServiceFlag::FtmsService 

// Hardware settings
#define IMPULSES_PER_REVOLUTION 4
#define FLYWHEEL_INERTIA 0.0265
#define SPROCKET_RADIUS 1.6875
#define CONCEPT_2_MAGIC_NUMBER 2.8

// Sensor signal filter settings
#define ENABLE_DEBOUNCE_FILTER true
#define ROTATION_DEBOUNCE_TIME_MIN 7
#define ROWING_STOPPED_THRESHOLD_PERIOD 7'000

// Drag factor filter settings
#define GOODNESS_OF_FIT_THRESHOLD 0.6
#define MAX_DRAG_FACTOR_RECOVERY_PERIOD 6'000
#define LOWER_DRAG_FACTOR_THRESHOLD 75
#define UPPER_DRAG_FACTOR_THRESHOLD 10000
#define DRAG_COEFFICIENTS_ARRAY_LENGTH 1

// Stroke phase detection filter settings
#define MINIMUM_POWERED_TORQUE 0
#define MINIMUM_DRAG_TORQUE 0.2
#define STROKE_DETECTION_TYPE STROKE_DETECTION_TORQUE
#define MINIMUM_RECOVERY_SLOPE_MARGIN 0 // Only relevant if STROKE_DETECTION_TYPE is either BOTH or TORQUE
#define MINIMUM_RECOVERY_SLOPE 0        // Only relevant if STROKE_DETECTION_TYPE is either BOTH or SLOPE
#define MINIMUM_RECOVERY_TIME 800
#define MINIMUM_DRIVE_TIME 400
#define IMPULSE_DATA_ARRAY_LENGTH 7
#define DRIVE_HANDLE_FORCES_MAX_CAPACITY 60

// NOLINTEND(cppcoreguidelines-macro-usage,cppcoreguidelines-macro-to-enum)