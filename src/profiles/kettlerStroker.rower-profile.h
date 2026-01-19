#pragma once

#include "../utils/enums.h"

// NOLINTBEGIN(cppcoreguidelines-macro-usage,cppcoreguidelines-macro-to-enum)

#define DEVICE_NAME Kettler Stroker
#define MODEL_NUMBER "07982-500"
#define ADD_BLE_SERVICE_TO_DEVICE_NAME false

// Hardware settings
#define IMPULSES_PER_REVOLUTION 4
#define FLYWHEEL_INERTIA 0.0293
#define CONCEPT_2_MAGIC_NUMBER 2.8

// Multiply the radii of pulleys that are *driven* by ropes/belts (6.5cm and 1.6875cm), and divide by the radius of the pulley that *drives* the belt (8cm).
// This single radius gives the same torque multiplication at the flywheel axle as the whole (real) pulley train.
#define SPROCKET_RADIUS 6.5 / 8.0 * 1.6875

// Sensor signal filter settings
#define ENABLE_DEBOUNCE_FILTER false
#define ROTATION_DEBOUNCE_TIME_MIN 8
#define ROWING_STOPPED_THRESHOLD_PERIOD 7'000

// Drag factor filter settings
#define GOODNESS_OF_FIT_THRESHOLD 0.8
#define MAX_DRAG_FACTOR_RECOVERY_PERIOD 6'000 // Max recovery time in ms
#define LOWER_DRAG_FACTOR_THRESHOLD 25
#define UPPER_DRAG_FACTOR_THRESHOLD 2'500  // Magnetic rowers can have very high drag factors
#define DRAG_COEFFICIENTS_ARRAY_LENGTH 10 // Magnetic rowers DF jumps around a lot (as they are speed dependent), need longer average to smooth it out

// Stroke phase detection filter settings
#define MINIMUM_POWERED_TORQUE 0
#define MINIMUM_DRAG_TORQUE 0.4
#define STROKE_DETECTION_TYPE STROKE_DETECTION_TORQUE
#define MINIMUM_RECOVERY_SLOPE 0    // Only relevant if STROKE_DETECTION_TYPE is either BOTH or SLOPE
#define MINIMUM_RECOVERY_TIME 500    // Quarter of the typical recovery time in ms
#define MINIMUM_DRIVE_TIME 200    // Quarter of the typical drive time in ms
#define IMPULSE_DATA_ARRAY_LENGTH 8    // 2 * IMPULSES_PER_REVOLUTION
#define DRIVE_HANDLE_FORCES_MAX_CAPACITY 100    // Typical number of pulses per stroke

// NOLINTEND(cppcoreguidelines-macro-usage,cppcoreguidelines-macro-to-enum)