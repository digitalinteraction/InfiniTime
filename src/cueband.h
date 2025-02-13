// cueband.h - Central place for cue.band configuration
// Dan Jackson, 2021

#pragma once

#if defined(PINETIME_IS_RECOVERY)
    #if defined(CUEBAND_CONFIGURATION_WARNINGS) // Only warn during compilation of main.cpp
        //#warning "Note: Recovery build -- no cue.band specific options"
    #endif
#else


//#define CUEBAND_LOG

// Preprocessor fun
#define CUEBAND_STRINGIZE(S) #S
#define CUEBAND_STRINGIZE_STRINGIZE(S) CUEBAND_STRINGIZE(S)

#define CUEBAND_MINOR_FIXES             // Fix minor issues in upstream InfiniTime code
#define CUEBAND_BUILD_FIXES             // Fix build issues (possibly?) in upstream InfiniTime code
#define CUEBAND_FIX_WARNINGS            // Ignore warnings in original InfiniTime code (without modifying that code)
/*
#ifdef CUEBAND_FIX_WARNINGS
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif
  // ...
#ifdef CUEBAND_FIX_WARNINGS
#pragma GCC diagnostic pop
#endif
*/

// This is the cueband-specific version/revision -- the InfiniTime version is in CUEBAND_PROJECT_VERSION_{MAJOR,MINOR,PATCH}
#define CUEBAND_VERSION_NUMBER 22  // 1-byte public firmware release number (stored in block format); see also CUEBAND_FORMAT_VERSION
#define CUEBAND_REVISION_NUMBER 0  // Revision number (only appears in user-visible version string, but not in block format)
#define CUEBAND_VERSION "" CUEBAND_STRINGIZE_STRINGIZE(CUEBAND_VERSION_NUMBER) "." CUEBAND_STRINGIZE_STRINGIZE(CUEBAND_REVISION_NUMBER) "." CUEBAND_PROJECT_COMMIT_HASH  // User-visible revision string
#define CUEBAND_APPLICATION_TYPE 0x0002 // Only returned in UART device query

#define CUEBAND_DEVICE_NAME "CueBand-######"  // "InfiniTime" // "InfiniTime-######"
#define CUEBAND_SERIAL_ADDRESS

#define CUEBAND_TRUSTED_CONNECTION      // Determine if a connection is trusted (required)
#define CUEBAND_USE_TRUSTED_CONNECTION  // Default switch for specific services to require trusted connections

// Global scratch buffer (only safe if use is not overlapped) to save some RAM (an not require dynamic allocation)
#define CUEBAND_GLOBAL_SCRATCH_BUFFER 256
#ifdef CUEBAND_GLOBAL_SCRATCH_BUFFER
extern unsigned char cuebandGlobalScratchBuffer[CUEBAND_GLOBAL_SCRATCH_BUFFER] __attribute__((aligned(8)));
#endif

// Simple UART service
// See: src/components/ble/UartService.cpp
// See: src/components/ble/UartService.h
// See: src/components/ble/NimbleController.h
#define CUEBAND_SERVICE_UART_ENABLED

// Streaming accelerometer samples over UART
// See: src/components/ble/UartService.cpp
// See: src/components/ble/UartService.h
// See: src/systemtask/SystemTask.cpp
#define CUEBAND_STREAM_ENABLED

// Streaming includes raw HR signal
#define CUEBAND_BUFFER_RAW_HR

// Stream the resampled data
//#define CUEBAND_STREAM_RESAMPLED

// App screens
// See: src/displayapp/Apps.h
// See: src/displayapp/screens/ApplicationList.cpp
// See: src/displayapp/DisplayApp.cpp

// See: displayapp/screens/InfoApp.h
// See: displayapp/screens/InfoApp.cpp
#define CUEBAND_INFO_APP_ENABLED
#define CUEBAND_INFO_APP_SYMBOL "\xEF\x84\xA9" // info // "?" // "I"
#define CUEBAND_INFO_APP_ID     // FW Version, QR Code and MAC address
#ifdef CUEBAND_INFO_APP_ID
    #define CUEBAND_INFO_APP_BARCODE
    #define CUEBAND_INFO_APP_QR
    #define QR_IMAGE_INVERT     // More recent specs of QR Codes allow light-on dark -- also saves memory as the quiet zone is skipped as the image is displayed on a dark background
    #define QR_TRUE_COLOR       // Can only transform (zoom) a true color image rather than 1bpp image, but a scale of x4 is (4*4=)x16 the number of pixels, and at LV_COLOR_DEPTH==16 takes the same amount of memory
#endif

#define CUEBAND_METRONOME_ENABLED

#define CUEBAND_AXES 3          // Must be 3

// Activity monitoring
#define CUEBAND_ACTIVITY_ENABLED
#define CUEBAND_HR_EPOCH
#define CUEBAND_HR_SAMPLING_SHORT_DELAY     // Always use a short delay while sampling
#define CUEBAND_DEBUG_PREVIOUS_BPM 5        // Store recent entries for debugging

// Cue prompts
#define CUEBAND_CUE_ENABLED

#define CUEBAND_DEFAULT_SCREEN_TIMEOUT 30000    // 15000 // Ideally matching one of the options in SettingDisplay.cpp

#define CUEBAND_ALLOW_REMOTE_FIRMWARE_VALIDATE  // risky
#define CUEBAND_ALLOW_REMOTE_RESET

//#define CUEBAND_USE_FULL_MTU    // (not tested) use full MTU size, reduces packet overhead and could be up to ~15% faster for bulk transmission.
//#define CUEBAND_UART_CHARACTERISTIC_READ        // (temporarily set the "READ" bits on the UART characteristics -- even though they are not handled)

#define CUEBAND_DEBUG_ACTIVITY   // Collect additional debug info for movement

#define CUEBAND_DEBUG_ADV       // Collect debug info for advertising state

#define CUEBAND_DETECT_UNSET_TIME 1577836800  // Before 2020-01-01 00:00:00

//#define CUEBAND_DETECT_FACE_DOWN 15         // (when activity is enabled) detect when the watch is face-down without being interacted with for 15 seconds
//#define CUEBAND_DETECT_WEAR_TIME (10 * 60)  // (when activity is enabled) detect the watch is unlikely to be worn after 10 minutes of no movement on at least two of the axes
#define CUEBAND_NO_SCHEDULED_PROMPTS_WHEN_UNSET_TIME    // When time is unset, do not use scheduled prompts (but do use impromptu ones)
#define CUEBAND_SILENT_WHEN_UNWORN          // Do not prompt when unworn

#define CUEBAND_UPTIME_1024                 // Track system uptime in units of 1024
#define CUEBAND_TRACK_MOTOR_TIMES (35*1024/10)  // Determine when the motor was active until (e.g. so that any affected accelerometer samples can be ignored), this is the settle time (+ jitter, etc) to add in milliseconds
//#define CUEBAND_DEBUG_TRACK_MOTOR_TIMES 1   // Stream output to UART: 1=remove, 2=leave and zero-out, 3=leave and zero-out in a pattern

#define CUEBAND_SYMBOLS

#define CUEBAND_APP_RELOAD_SCREENS            // Cueband app reloads for each screen (so that the transitions work correctly)
#define CUEBAND_MANUAL_PROMPT_MUTE_STOP       // When manually prompting, mute button stops on first press, rather than enter mute screen

#define CUEBAND_MOTOR_PATTERNS  // Allow vibration motor patterns

// Various customizations for the UI and existing PineTime services
#define CUEBAND_CUSTOMIZATION
//#define CUEBAND_SAVE_MEMORY                           // Actions to reduce program memory
//#define CUEBAND_DEBUG_INIT_TIME                       // While debugging, use the build time to initialize the clock if the time is invalid
//#define CUEBAND_PREVENT_ACCIDENTAL_RECOVERY_MODE      // Make it trickier to accidentally wipe the firmware by holding the button while worn (risky)
#define CUEBAND_LONGER_PRESS_INFO

#define CUEBAND_FIX_DFU_LARGE_PACKETS                   // Could do with more testing (but uses original code for smaller MTU anyway)

// Local build configuration overrides above switches
#if defined(__has_include)
  #if __has_include("cueband.local.h")
    #if defined(CUEBAND_CONFIGURATION_WARNINGS)
      #warning "Using local build configuration: cueband.local.h"
    #endif
    #include "cueband.local.h"    // #define CUEBAND_LOCAL_KEY "secret"
  #else
    #if defined(CUEBAND_CONFIGURATION_WARNINGS)
      #warning "cueband.local.h not found, using default cueband configuration"
    #endif
  #endif
#else
  #if defined(CUEBAND_CONFIGURATION_WARNINGS)
    #warning "__has_include not supported, using default cueband configuration"
  #endif
#endif

// Local build configuration: custom device name
#ifdef CUEBAND_LOCAL_DEVICE_NAME
  #if defined(CUEBAND_CONFIGURATION_WARNINGS)
    #warning "This build is using a non-default device name for local testing: CUEBAND_LOCAL_DEVICE_NAME"
  #endif
  #ifdef CUEBAND_DEVICE_NAME
    #undef CUEBAND_DEVICE_NAME
  #endif
  #define CUEBAND_DEVICE_NAME CUEBAND_LOCAL_DEVICE_NAME
#endif

// Local build configuration: custom info
#ifndef CUEBAND_GITHUB_ORG
    #define CUEBAND_GITHUB_ORG "cueband" // "InfiniTimeOrg"
#endif

// Debug DFU large packets
#ifdef CUEBAND_FIX_DFU_LARGE_PACKETS
    #define CUEBAND_DEBUG_DFU
#endif

#if defined(CUEBAND_TRUSTED_CONNECTION) && defined(CUEBAND_USE_TRUSTED_CONNECTION)
  #define CUEBAND_TRUSTED_DFU           // Only allow DFU over trusted connection (risky?)
  #define CUEBAND_TRUSTED_UART          // Only allow most UART commands over a trusted connection
  #define CUEBAND_TRUSTED_ACTIVITY      // Only allow Activity Service commands over a trusted connection
  #define CUEBAND_TRUSTED_CUE           // Only allow Cue Service commands over a trusted connection

  #define CUEBAND_TRUSTED_IMMEDIATE_ALERT     // Only allow immediate alert over a trusted connection
  #define CUEBAND_TRUSTED_ALERT_NOTIFICATION  // Only allow alert notification over a trusted connection
#endif

#ifdef CUEBAND_CUE_ENABLED
  // See: displayapp/screens/CueBandApp.h
  // See: displayapp/screens/CueBandApp.cpp
  #define CUEBAND_APP_ENABLED
  #define CUEBAND_DISABLE_APPS_STOPS_ALARMS
#endif

#if defined(CUEBAND_APP_ENABLED)
    #define CUEBAND_APP_SYMBOL "\xEF\xA0\xBE" // cuebandCue / 0xf83e, wave-square  // "!" // "C"
    //#define CUEBAND_APP_SYMBOL_ALTERNATIVE "C"    // For application list as icon not in font for application menu
    #define CUEBAND_WATCHFACE_LIMIT_OPTIONS         // Show limited watch face options (digital & analog)
    #define CUEBAND_TAP_WATCHFACE_LAUNCH_APP
    #define CUEBAND_SWIPE_WATCHFACE_LAUNCH_APP
    #define CUEBAND_WATCHFACE_CUE_STATUS
    #define CUEBAND_ANALOG_WATCHFACE_REMOVE_LABEL       // Remove "InfiniTime" label from original analog watch face (to make room for cue status)
    #define CUEBAND_APP_QUICK_SETTINGS  // Launch from quick settings
    #define CUEBAND_OPTIONS_APP_ENABLED // CueBandOptionsApp
#endif

#ifdef CUEBAND_CUSTOMIZATION

    // See: src/displayapp/screens/WatchFaceDigital.cpp
    // See: src/displayapp/screens/WatchFaceAnalog.cpp
    // See: src/displayapp/screens/PineTimeStyle.cpp
    #define CUEBAND_CUSTOMIZATION_NO_INVALID_TIME       // Do not show invalid date/time

    // See: src/displayapp/screens/WatchFaceDigital.cpp
    #define CUEBAND_CUSTOMIZATION_NO_HR                 // Removes from digital watch face
    #define CUEBAND_CUSTOMIZATION_NO_STEPS              // Removes from digital watch face and settings menu

    // See: src/displayapp/screens/ApplicationList.cpp
    #ifndef CUEBAND_DISABLE_CUSTOMIZATION_ONLY_ESSENTIAL_APPS
        #define CUEBAND_CUSTOMIZATION_ONLY_ESSENTIAL_APPS   // Only keep essential watch apps in the launcher (stopwatch, timer, alarm)
    #endif

    // See: src/components/ble/NimbleController.cpp
    #define CUEBAND_SERVICE_MUSIC_DISABLED
    #define CUEBAND_SERVICE_WEATHER_DISABLED
    #define CUEBAND_SERVICE_NAV_DISABLED
    #define CUEBAND_SERVICE_HR_DISABLED
    #define CUEBAND_SERVICE_MOTION_DISABLED
    #define CUEBAND_SERVICE_FS_DISABLED

    // Disable discovery for service clients (alert notification client and time client)
    #define CUEBAND_SERVICE_CLIENTS_DISABLED
#endif

#ifdef CUEBAND_SAVE_MEMORY
    // Remove analog clock background (do not use!), saves 14 kB, untested
    #define CUEBAND_ANALOG_WATCHFACE_NO_IMAGE
    // Use 1-bit-per-pixel version of the analog clock background (do not use!), saves ~7 kB
    //#define CUEBAND_ANALOG_WATCHFACE_1BPP_IMAGE
#endif

#if defined(CUEBAND_STREAM_ENABLED) || defined(CUEBAND_ACTIVITY_ENABLED)

    // Extend accelerometer driver (Bma421.cpp) and Motion Controller (MotionController.cpp) to buffer multiple samples
    #define CUEBAND_BUFFER_ENABLED

    #define CUEBAND_STREAM_DECIMATE 1       // Only stream 1 in N (1=50 Hz, 2=25 Hz)

    #define CUEBAND_FAST_LOOP_IF_REQUIRED

    #define CUEBAND_BUFFER_SAMPLE_RATE 50     // 50 Hz
    #define CUEBAND_BUFFER_SAMPLE_RANGE 8     // +/- 8g
    #define CUEBAND_BUFFER_12BIT_SCALE (2048/CUEBAND_BUFFER_SAMPLE_RANGE)    // at +/- 8g, 256 (12-bit) raw units = 1 'g'
    #define CUEBAND_BUFFER_16BIT_SCALE (CUEBAND_BUFFER_12BIT_SCALE << 4)     // at +/- 8g, 4096

    #define CUEBAND_SCALE_MILLI_G(_v) ((int)((long)(_v) * CUEBAND_BUFFER_16BIT_SCALE / 1000))

    // Original firmware expects the MotionController to return values scaled at +/- 2g -- see Bma421::Init()
    #define CUEBAND_ORIGINAL_RANGE 2                                // +/- 2g
    #define CUEBAND_ORIGINAL_SCALE (2048/CUEBAND_ORIGINAL_RANGE)    // at +/- 2g, 1024 (12-bit) raw units = 1 'g'

    #define CUEBAND_MOTION_INCLUDE_TEMPERATURE

    // Use the FIFO
    #define CUEBAND_FIFO_ENABLED

    #ifdef CUEBAND_FIFO_ENABLED
        // FIFO settings
        #define CUEBAND_FIFO_POLL_RATE 10        // 10 Hz -- Read FIFO approximately this rate (plus jitter) -- TODO: Consider using watermark interrupt instead

        #define CUEBAND_SAMPLE_MAX 32  // (120 * CUEBAND_BUFFER_SAMPLE_RATE / CUEBAND_FIFO_POLL_RATE / 100) // 60 samples (allow 20% margin on rate)

        #define CUEBAND_FIFO_BUFFER_LENGTH (CUEBAND_SAMPLE_MAX * 6)   // FIFO: 360 bytes (up to 1024)

        #define CUEBAND_BUFFER_EFFECTIVE_RATE CUEBAND_BUFFER_SAMPLE_RATE
    #else
        // If not using FIFO, fall back to (rubbish) polled sampling
        #define CUEBAND_POLLED_ENABLED
        #define CUEBAND_POLLED_INPUT_RATE 8     // 8 Hz
        #define CUEBAND_POLLED_OUTPUT_RATE 32   // 30/32 Hz (-> 32 Hz as x4 integer scaling for fake-synthesized nearest-neighbour sampling from polled input rate)

        #define CUEBAND_BUFFER_EFFECTIVE_RATE CUEBAND_POLLED_OUTPUT_RATE

        #define CUEBAND_SAMPLE_MAX 1
    #endif

#endif

// Don't bother reading activity and temperature if they're not used
#ifdef CUEBAND_BUFFER_ENABLED
    #define CUEBAND_DONT_READ_UNUSED_ACCELEROMETER_VALUES
#endif

// Don't allow the SPI-NOR flash or SPI bus to sleep (TODO: Coordinate this with ActivityController to write to files)
#ifdef CUEBAND_ACTIVITY_ENABLED
    #define CUEBAND_DONT_SLEEP_SPI
    #define CUEBAND_DONT_SLEEP_NOR_FLASH
#endif

#if defined(CUEBAND_DETECT_WEAR_TIME) || defined(CUEBAND_DETECT_FACE_DOWN)
    #define CUEBAND_ACTIVITY_STATS       // compute per-second stats (min/max) on the activity
#endif

// If none of these are defined, the build should be identical to stock InfiniTime firmware
#if defined(CUEBAND_SERVICE_UART_ENABLED) || defined(CUEBAND_APP_ENABLED) || defined(CUEBAND_ACTIVITY_ENABLED) || defined(CUEBAND_CUE_ENABLED)

    // Modifications to version
    // See: src/components/ble/DeviceInformationService.h
    #define CUEBAND_VERSION_MANUFACTURER_EXTENSION "/cb" CUEBAND_VERSION
    #define CUEBAND_VERSION_MODEL_EXTENSION "/cb" CUEBAND_VERSION
    #define CUEBAND_VERSION_SOFTWARE_EXTENSION "/cb" CUEBAND_VERSION

    // Modifications to Firmware App screen
    // See: src/displayapp/screens/FirmwareValidation.cpp
    #define CUEBAND_INFO_FIRMWARE "\ncb/" CUEBAND_VERSION

    // Modifications to System Info App screen
    // See: src/displayapp/screens/SystemInfo.cpp
    #define CUEBAND_INFO_SYSTEM "\ncb/" CUEBAND_VERSION

#endif

#if defined(CUEBAND_ACTIVITY_ENABLED)
    #define CUEBAND_FS_FILESIZE_ENABLED
    #define CUEBAND_FS_FILETELL_ENABLED
    #define CUEBAND_BLUETOOTH_DISABLE_WARNING
#endif

#define CUEBAND_FIX_CURRENT_TIME_CLIENT // Validate response length

//#define CUEBAND_POSSIBLE_FIX_BLE_CONNECT_SERVICE_DISCOVERY_TIMEOUT      // Possible race condition on service discovery?

//#define CUEBAND_NO_ADV_RSP              // Optional test: do not split name into advertising response (name truncated, no UUID advertised)

#define CUEBAND_DELAY_START 50          // Delay cue.band service initialization (main loop iterations) -- 50 from start = approx. 3-5 seconds.

// Logging (30-40 days)
#define CUEBAND_ACTIVITY_MAXIMUM_BLOCKS 512 // 512 = 128 kB, ~10 days/file;  
#define CUEBAND_ACTIVITY_FILES 4            // 3-4 files gives 30-40 days

#ifdef CUEBAND_ACTIVITY_EPOCH_INTERVAL
    #ifdef CUEBAND_CONFIGURATION_WARNINGS
        #warning "This build has a non-default CUEBAND_ACTIVITY_EPOCH_INTERVAL and must not be used for a release"
    #endif
#else
    #define CUEBAND_ACTIVITY_EPOCH_INTERVAL 60  // 60
#endif

#ifdef CUEBAND_ACTIVITY_HRM_INTERVAL_DEFAULT
    #ifdef CUEBAND_CONFIGURATION_WARNINGS
        #warning "This build has a non-default CUEBAND_ACTIVITY_HRM_INTERVAL_DEFAULT and must not be used for a release"
    #endif
#else
    #define CUEBAND_ACTIVITY_HRM_INTERVAL_DEFAULT 0
#endif

#ifdef CUEBAND_ACTIVITY_HRM_DURATION_DEFAULT
    #ifdef CUEBAND_CONFIGURATION_WARNINGS
        #warning "This build has a non-default CUEBAND_ACTIVITY_HRM_DURATION_DEFAULT and must not be used for a release"
    #endif
#else
    #define CUEBAND_ACTIVITY_HRM_DURATION_DEFAULT 0
#endif

#ifdef CUEBAND_FORMAT_VERSION
    #ifdef CUEBAND_CONFIGURATION_WARNINGS
        #warning "This build has a non-default CUEBAND_FORMAT_VERSION and must not be used for a release"
    #endif
#else
    #define CUEBAND_FORMAT_VERSION CUEBAND_FORMAT_VERSION_ORIGINAL_ACTIVITY_0002
#endif

#define ACTIVITY_BLOCK_SIZE 256

#define ACTIVITY_RATE 40    // Common activity monitor rate: 32 Hz (Philips Actiwatch Spectrum+/Pro/2, CamNtech Actiwave Motion, Minisun IDEEA, Fit.life Fitmeter, BodyMedia SenseWear); or 30 Hz (ActiGraph GT3X/GT1M)
// Always store MEAN(SVMMO), additionally:
#define CUEBAND_ACTIVITY_HIGH_PASS  // ...store MEAN(FILTER(SVMMO)), otherwise: store plain MEAN(SVM)

#define CUEBAND_FORMAT_VERSION_MIN 0x0002
#define CUEBAND_FORMAT_VERSION_ORIGINAL_ACTIVITY_0002 0x0002
#define CUEBAND_FORMAT_VERSION_HR_RANGE_0003 0x0003
#define CUEBAND_FORMAT_VERSION_MICRO_EPOCHS_0080 0x0080
#define CUEBAND_FORMAT_VERSION_MAX 0xfffe   // unused

// 0x0000=30 Hz data, no high-pass filter, no SVMMO
// 0x0001=30 Hz data, no high-pass filter, SVMMO present
// 0x0002=40 Hz data, SVMMO, high-pass SVMMO
// 0x0003=40 Hz, SVMMO, HR range

#define CUEBAND_TX_COUNT 26    // Queue multiple notifications at once (hopefully to send more than one per connection interval)
//#define CUEBAND_DEBUG_DUMMY_MISSING_BLOCKS



// Methods
#ifdef __cplusplus
extern "C" {
#endif

#ifdef CUEBAND_LOG
void cblog(const char *str);
#endif

#ifdef __cplusplus
}
#endif





// Config compatibility checks
#if defined(CUEBAND_TRACK_MOTOR_TIMES) && !defined(CUEBAND_UPTIME_1024)
    #error "CUEBAND_TRACK_MOTOR_TIMES requires CUEBAND_UPTIME_1024"
#endif
#if defined(CUEBAND_FIFO_ENABLED) && !defined(CUEBAND_BUFFER_ENABLED)
    #error "CUEBAND_FIFO_ENABLED requires CUEBAND_BUFFER_ENABLED"
#endif
#if defined(CUEBAND_STREAM_ENABLED) && !defined(CUEBAND_POLLED_ENABLED) && !defined(CUEBAND_FIFO_ENABLED)
    #error "CUEBAND_STREAM_ENABLED requires CUEBAND_FIFO_ENABLED or CUEBAND_POLLED_ENABLE"
#endif
#if defined(CUEBAND_ACTIVITY_ENABLED) && !defined(CUEBAND_POLLED_ENABLED) && !defined(CUEBAND_FIFO_ENABLED)
    #error "CUEBAND_ACTIVITY_ENABLED requires CUEBAND_FIFO_ENABLED or CUEBAND_POLLED_ENABLE"
#endif
#if defined(CUEBAND_CUE_ENABLED) && !defined(CUEBAND_ACTIVITY_ENABLED)
    #error "CUEBAND_CUE_ENABLED requires CUEBAND_ACTIVITY_ENABLED (for wear time and to store log)"
#endif
#if defined(CUEBAND_STREAM_ENABLED) && !defined(CUEBAND_SERVICE_UART_ENABLED)
    #error "CUEBAND_STREAM_ENABLED requires CUEBAND_SERVICE_UART_ENABLED"
#endif
#if defined(CUEBAND_POLLED_ENABLED) && defined(CUEBAND_FIFO_ENABLED)
    #error "At most one of CUEBAND_POLLED_ENABLED / CUEBAND_FIFO_ENABLED may be defined"
#endif
#if defined(CUEBAND_CUSTOMIZATION_NO_INVALID_TIME) && !defined(CUEBAND_DETECT_UNSET_TIME)
    #error "CUEBAND_CUSTOMIZATION_NO_INVALID_TIME requires CUEBAND_DETECT_UNSET_TIME"
#endif
#if defined(CUEBAND_NO_SCHEDULED_PROMPTS_WHEN_UNSET_TIME) && !defined(CUEBAND_DETECT_UNSET_TIME)
    #error "CUEBAND_NO_SCHEDULED_PROMPTS_WHEN_UNSET_TIME requires CUEBAND_DETECT_UNSET_TIME"
#endif

// Debug CUEBAND_TRACK_MOTOR_TIMES
#ifdef CUEBAND_DEBUG_TRACK_MOTOR_TIMES
    #ifdef CUEBAND_CONFIGURATION_WARNINGS
        #warning "This build has CUEBAND_DEBUG_TRACK_MOTOR_TIMES and must not be used for a release"
    #endif
    #ifndef CUEBAND_STREAM_ENABLED
        #error "CUEBAND_DEBUG_TRACK_MOTOR_TIMES requires CUEBAND_STREAM_ENABLED"
    #endif
    #ifndef CUEBAND_STREAM_RESAMPLED
        #define CUEBAND_STREAM_RESAMPLED
        #ifdef CUEBAND_CONFIGURATION_WARNINGS
            #warning "Setting CUEBAND_STREAM_RESAMPLED as required for CUEBAND_DEBUG_TRACK_MOTOR_TIMES"
        #endif
    #endif
    #if (ACTIVITY_RATE != CUEBAND_BUFFER_EFFECTIVE_RATE)
        #ifdef CUEBAND_CONFIGURATION_WARNINGS
            #warning "Altering ACTIVITY_RATE to match CUEBAND_BUFFER_EFFECTIVE_RATE as required for CUEBAND_DEBUG_TRACK_MOTOR_TIMES"
        #endif
        #undef ACTIVITY_RATE
        #define ACTIVITY_RATE CUEBAND_BUFFER_EFFECTIVE_RATE
    #endif
#endif

// Warnings for non-standard build
#if defined(CUEBAND_CONFIGURATION_WARNINGS) // Only warn during compilation of main.cpp
#ifdef CUEBAND_SAVE_MEMORY
    #warning "This is a CUEBAND_SAVE_MEMORY build and should not be released"
#endif
#if ((CUEBAND_BUFFER_EFFECTIVE_RATE == ACTIVITY_RATE) && defined(CUEBAND_STREAM_RESAMPLED) && !defined(CUEBAND_DEBUG_TRACK_MOTOR_TIMES))
    #warning "CUEBAND_STREAM_RESAMPLED defined, but CUEBAND_BUFFER_EFFECTIVE_RATE == ACTIVITY_RATE, this will be inefficient"
#endif
#if defined(CUEBAND_PREVENT_ACCIDENTAL_RECOVERY_MODE)
    #warning "CUEBAND_PREVENT_ACCIDENTAL_RECOVERY_MODE defined - makes it trickier to accidentally wipe the firmware by holding the button while worn (risky)"
#endif
#if defined(CUEBAND_DEBUG_INIT_TIME)
    #warning "CUEBAND_DEBUG_INIT_TIME defined - invalid times will be set to build time (DO NOT RELEASE THIS BUILD)"
#endif
#if !defined(CUEBAND_TRUSTED_DFU)
    #warning "CUEBAND_TRUSTED_DFU not defined -- this build does not require trusted connections for DFU"
#endif
#if defined(CUEBAND_SERVICE_UART_ENABLED) && !defined(CUEBAND_TRUSTED_UART)
    #warning "CUEBAND_TRUSTED_UART not defined -- this build does not require trusted connections for UART"
#endif
#if defined(CUEBAND_ACTIVITY_ENABLED) && !defined(CUEBAND_TRUSTED_ACTIVITY)
    #warning "CUEBAND_TRUSTED_ACTIVITY not defined -- this build does not require trusted connections for Activity Service"
#endif
#if defined(CUEBAND_CUE_ENABLED) && !defined(CUEBAND_TRUSTED_CUE)
    #warning "CUEBAND_TRUSTED_CUE not defined -- this build does not require trusted connections for Cue Service"
#endif
#if !defined(CUEBAND_FIFO_ENABLED)
    #warning "CUEBAND_FIFO_ENABLED not defined - won't use sensor FIFO"
#endif
#if (ACTIVITY_BLOCK_SIZE != 256)
    #warning "ACTIVITY_BLOCK_SIZE non-standard value (usually 256)"
#endif
#if !defined(CUEBAND_SERVICE_UART_ENABLED)
    #warning "CUEBAND_SERVICE_UART_ENABLED not defined - no UART service."
#endif
#if !defined(CUEBAND_STREAM_ENABLED)
    #warning "CUEBAND_STREAM_ENABLED not defined - no streaming supported."
#endif
#if !defined(CUEBAND_APP_ENABLED)
    #warning "CUEBAND_APP_ENABLED not defined - no app."
#endif
#if !defined(CUEBAND_ACTIVITY_ENABLED)
    #warning "CUEBAND_ACTIVITY_ENABLED not defined - no activity monitoring."
#endif
#if !defined(CUEBAND_CUE_ENABLED)
    #warning "CUEBAND_CUE_ENABLED not defined - no cueing."
#endif
#if defined(CUEBAND_ACTIVITY_ENABLED) && (CUEBAND_ACTIVITY_EPOCH_INTERVAL != 60)
    #warning "CUEBAND_ACTIVITY_EPOCH_INTERVAL != 60 - non-standard epoch size."
#endif
#if (ACTIVITY_BLOCK_SIZE != 256)
    #warning "ACTIVITY_BLOCK_SIZE non-standard value (usually 256)"
#endif
#if (CUEBAND_ACTIVITY_MAXIMUM_BLOCKS != 512)
    #warning "CUEBAND_ACTIVITY_MAXIMUM_BLOCKS non-standard value (usually 512)"     // approx. 10 days per 128kB file when 28 minutes per block (256-byte blocks, 8-bytes per sample, 60 second epoch)
#endif
#if defined(CUEBAND_DEBUG_DUMMY_MISSING_BLOCKS)
    #warning "CUEBAND_DEBUG_DUMMY_MISSING_BLOCKS should not normally be defined"
#endif
#if defined(CUEBAND_MAXIMUM_SAMPLES_PER_BLOCK)
    #warning "CUEBAND_MAXIMUM_SAMPLES_PER_BLOCK should not normally be defined (usually all available space is used)"
#endif
#if defined(CUEBAND_DEBUG_ADV_LOG)
    #warning "This is a build for debugging advertising - CUEBAND_DEBUG_ADV_LOG"
#endif
#if defined(CUEBAND_TRUSTED_CONNECTION) && !defined(CUEBAND_LOCAL_KEY)
  #warning "CUEBAND_LOCAL_KEY not specified in cueband.local.h -- using empty key"
#endif
#endif

// SystemTask.cpp:
//   uint32_t systick_counter = nrf_rtc_counter_get(portNRF_RTC_REG);

#endif // PINETIME_IS_RECOVERY
