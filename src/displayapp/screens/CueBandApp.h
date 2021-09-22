#pragma once

#include "cueband.h"

#include <cstdint>
#include <lvgl/lvgl.h>
#include "systemtask/SystemTask.h"
#include "Screen.h"
#include "components/datetime/DateTimeController.h"
#include "components/motion/MotionController.h"
#ifdef CUEBAND_ACTIVITY_ENABLED
#include "components/activity/ActivityController.h"
#endif

namespace Pinetime {

  namespace Controllers {
    class Settings;
  }

  namespace Applications {
    namespace Screens {

      class CueBandApp : public Screen {
        public:
          CueBandApp(
            DisplayApp* app, 
            System::SystemTask& systemTask, 
            Controllers::DateTime& dateTimeController,
            Controllers::MotionController& motionController, 
            Controllers::Settings &settingsController
#ifdef CUEBAND_ACTIVITY_ENABLED
            , Controllers::ActivityController& activityController
#endif
          );

          ~CueBandApp() override;

          void Refresh() override;
          void Update();

          bool OnTouchEvent(TouchEvents event) override;

          int ScreenCount();

        private:
          Pinetime::System::SystemTask& systemTask;
          Pinetime::Controllers::DateTime& dateTimeController;
          Controllers::MotionController& motionController;
          Controllers::Settings& settingsController;
#ifdef CUEBAND_ACTIVITY_ENABLED
          Controllers::ActivityController& activityController;
#endif

          lv_task_t* taskUpdate;
          lv_obj_t *lInfo;

          int screen = 0;

      };
    }
  }
}
