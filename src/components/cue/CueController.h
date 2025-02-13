#pragma once

#include "cueband.h"

#ifdef CUEBAND_CUE_ENABLED

#include "components/settings/Settings.h"
#include "components/fs/FS.h"
#include "components/activity/ActivityController.h"
#include "components/motor/MotorController.h"
#include "components/cue/ControlPointStore.h"

#include <cstdint>

#define PROMPT_MAX_CONTROLS 64

namespace Pinetime::Controllers { class Battery; }  // #include "components/battery/BatteryController.h"

namespace Pinetime {
  namespace Controllers {

    typedef uint16_t options_t;

    class CueController {
    public:
      CueController(Controllers::Settings& settingsController, Controllers::FS& fs, Controllers::ActivityController& activityController, Controllers::MotorController& motorController, Controllers::Battery& batteryController);

      void Init();
      void TimeChanged(uint32_t timestamp, uint32_t uptime);

      bool IsInitialized() { return initialized; }
      void Vibrate(unsigned int style);

      const static unsigned int INTERVAL_OFF = 0;
      const static unsigned int MAXIMUM_RUNTIME_OFF = 0;
      const static unsigned int DEFAULT_PROMPT_STYLE = 3;
      const static unsigned int DEFAULT_INTERVAL = 60;
      const static unsigned int DEFAULT_DURATION = (10 * 60);

      // Options
      const static options_t OPTIONS_CUE_SETTING    = (1 << 0); // Feature: Allow user to toggle any non-overridden cueing functionality in the settings menu.
      const static options_t OPTIONS_CUE_ENABLED    = (1 << 1); // Feature: Globally enable cueing
      const static options_t OPTIONS_CUE_STATUS     = (1 << 2); // Feature: Show cueing status on watch face (when cueing enabled)
      const static options_t OPTIONS_CUE_DETAILS    = (1 << 3); // Feature: Enable the cue app for cue details (when cueing enabled)
      const static options_t OPTIONS_CUE_MANUAL     = (1 << 4); // Feature: Enable mute and manual cueing from the cue details (when cueing enabled and the cue app is enabled)
      const static options_t OPTIONS_CUE_DISALLOW   = (1 << 5); // Feature: Temporarily and visibly disallow cueing (when cueing enabled)
      const static options_t OPTIONS_APPS_DISABLE   = (1 << 6); // Feature: Disable app launcher
      const static options_t OPTIONS_CUE_RESERVED_7 = (1 << 6); // (reserved)

      const static options_t OPTIONS_DEFAULT = OPTIONS_CUE_SETTING | OPTIONS_CUE_STATUS | OPTIONS_CUE_DETAILS | OPTIONS_CUE_MANUAL; // OPTIONS_CUE_ENABLED
      const static options_t OPTIONS_STARTING = 0;  // Nothing allowed until started

      options_t GetOptionsMaskValue(options_t *base = nullptr, options_t *mask = nullptr, options_t *value = nullptr);
      bool SetOptionsMaskValue(options_t mask, options_t value);  // Remotely overridden values
      bool SetOptionsBaseValue(options_t new_base_value);         // Local user-configured values

      bool SetInterval(unsigned int interval, unsigned int maximumRuntime);
      void SetPromptStyle(unsigned int promptStyle = DEFAULT_PROMPT_STYLE);

      void GetStatus(uint32_t *active_schedule_id, uint16_t *max_control_points, uint16_t *current_control_point, uint32_t *override_remaining, uint32_t *intensity, uint32_t *interval, uint32_t *duration);
      void GetLastImpromptu(unsigned int *lastInterval, unsigned int *promptStyle);

      void Reset(bool everything);
      ControlPoint GetStoredControlPoint(int index);
      void ClearScratch();
      void SetScratchControlPoint(int index, ControlPoint controlPoint);
      void CommitScratch(uint32_t version);

      bool IsSetting() { return (GetOptionsMaskValue() & OPTIONS_CUE_SETTING) != 0; }
      bool IsGloballyEnabled() { return (GetOptionsMaskValue() & OPTIONS_CUE_ENABLED) != 0; }
      bool IsAllowed() { return IsGloballyEnabled() && ((GetOptionsMaskValue() & OPTIONS_CUE_DISALLOW) == 0); }   // Globally enabled and not disallowed
      bool IsShowStatus() { 
        options_t options = GetOptionsMaskValue();
        return IsGloballyEnabled() && ((options & OPTIONS_CUE_STATUS) != 0);
      }
      bool IsOpenDetails() { 
        options_t options = GetOptionsMaskValue();
        return IsGloballyEnabled() && ((options & OPTIONS_CUE_DETAILS) != 0);
      }
      bool IsManualAllowed() {
        options_t options = GetOptionsMaskValue();
        return IsAllowed() && ((options & OPTIONS_CUE_DETAILS) != 0) && ((options & OPTIONS_CUE_MANUAL) != 0);
      }
      bool IsAppsDisabled() {
        options_t options = GetOptionsMaskValue();
        return (options & OPTIONS_APPS_DISABLE) != 0;
      }
      bool IsTemporary() { return currentUptime < overrideEndTime && interval > 0; }
      bool IsSnoozed() { return currentUptime < overrideEndTime && interval == 0; }
      bool IsScheduled() { return currentUptime >= overrideEndTime; }
      bool IsWithinScheduledPrompt() { return effectiveScheduledInterval > 0; }

      bool SilencedAsUnworn();

      const char *Description(bool detailed = false, const char **symbol = nullptr);
      void DebugText(char *debugText);

    private:

      int ReadCues(uint32_t *version);
      int WriteCues();
      void DeferWriteCues();

      // State initialized (delay initialized)
      bool initialized = false;

      // Cache description
      bool descriptionValid = false;
      char description[80];
      const char *icon = "";
      bool descriptionDetailed = false;

      // Options
      options_t options_base_value = OPTIONS_STARTING;
      options_t options_overridden_mask = 0;
      options_t options_overridden_value = 0;

      Pinetime::Controllers::ControlPointStore store;
      unsigned short version;
      Pinetime::Controllers::control_point_packed_t controlPoints[PROMPT_MAX_CONTROLS];
      Pinetime::Controllers::control_point_packed_t scratch[PROMPT_MAX_CONTROLS];

      Controllers::Settings& settingsController;
      Controllers::FS& fs;
      Controllers::ActivityController& activityController;
      Controllers::MotorController& motorController;
      Controllers::Battery& batteryController;

      const static uint32_t UPTIME_NONE = (uint32_t)-1;

      uint32_t currentTime = 0;
      uint32_t currentUptime = 0;

      // Current scheduled cue control point
      int currentCueIndex = ControlPoint::INDEX_NONE;
      Pinetime::Controllers::ControlPoint currentControlPoint;
      unsigned int cueRemaining = 0;

      uint32_t lastPrompt = UPTIME_NONE;    // Uptime at the last prompt

      uint32_t overrideEndTime = 0;         // End of override time
      unsigned int interval = INTERVAL_OFF; // Current prompt interval (seconds), 0=snooze sheduled prompts

      unsigned int lastInterval = DEFAULT_INTERVAL;         // Last configured prompt interval
      unsigned int promptStyle = DEFAULT_PROMPT_STYLE;      // Last configured prompt style
      unsigned int settingsChanged = 0;                     // Settings change -> save debounce
      int lastCueIndex = ControlPoint::INDEX_NONE;

      // Track the current effective sheduled interval
      unsigned int effectiveScheduledInterval = 0;

      int readError = -1;                 // (Debug) File read status
      int writeError = -1;                // (Debug) File write status
    };
  }
}

#endif
