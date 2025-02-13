#pragma once

#include "cueband.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      namespace Symbols {
        static constexpr const char* none = "";
        static constexpr const char* batteryHalf = "\xEF\x89\x82";
        static constexpr const char* heartBeat = "\xEF\x88\x9E";
        static constexpr const char* bluetooth = "\xEF\x8A\x94";
        static constexpr const char* plug = "\xEF\x87\xA6";
        static constexpr const char* shoe = "\xEF\x95\x8B";
        static constexpr const char* clock = "\xEF\x80\x97";
        static constexpr const char* info = "\xEF\x84\xA9";
        static constexpr const char* list = "\xEF\x80\xBA";
        static constexpr const char* sun = "\xEF\x86\x85";
        static constexpr const char* check = "\xEF\x95\xA0";
        static constexpr const char* music = "\xEF\x80\x81";
        static constexpr const char* tachometer = "\xEF\x8F\xBD";
        static constexpr const char* paintbrush = "\xEF\x87\xBC";
        static constexpr const char* paddle = "\xEF\x91\x9D";
        static constexpr const char* map = "\xEF\x96\xa0";
        static constexpr const char* phone = "\xEF\x82\x95";
        static constexpr const char* phoneSlash = "\xEF\x8F\x9D";
        static constexpr const char* volumMute = "\xEF\x9A\xA9";
        static constexpr const char* volumUp = "\xEF\x80\xA8";
        static constexpr const char* volumDown = "\xEF\x80\xA7";
        static constexpr const char* stepForward = "\xEF\x81\x91";
        static constexpr const char* stepBackward = "\xEF\x81\x88";
        static constexpr const char* play = "\xEF\x81\x8B";
        static constexpr const char* pause = "\xEF\x81\x8C";
        static constexpr const char* stop = "\xEF\x81\x8D";
        static constexpr const char* stopWatch = "\xEF\x8B\xB2";
        static constexpr const char* hourGlass = "\xEF\x89\x92";
        static constexpr const char* lapsFlag = "\xEF\x80\xA4";
        static constexpr const char* drum = "\xEF\x95\xA9";
        static constexpr const char* chartLine = "\xEF\x88\x81";
        static constexpr const char* eye = "\xEF\x81\xAE";
        static constexpr const char* home = "\xEF\x80\x95";

        // lv_font_sys_48.c
        static constexpr const char* settings = "\xEE\xA4\x82"; // e902

        static constexpr const char* brightnessHigh = "\xEE\xA4\x84";   // e904
        static constexpr const char* brightnessLow = "\xEE\xA4\x85";    // e905
        static constexpr const char* brightnessMedium = "\xEE\xA4\x86"; // e906

        static constexpr const char* notificationsOff = "\xEE\xA4\x8B"; // e90b
        static constexpr const char* notificationsOn = "\xEE\xA4\x8C";  // e90c

        static constexpr const char* highlight = "\xEE\xA4\x87"; // e907

#ifdef CUEBAND_SYMBOLS
        // cueband_20.c & cueband_48.c
        static constexpr const char* cuebandCue       = "\xEF\xA0\xBE";                  // 0xf83e, wave-square
        static constexpr const char* cuebandIsCueing  = "\xEF\x89\xB4";                  // 0xf274, calendar-check
        static constexpr const char* cuebandNotCueing = "\xEF\x89\xB2";                  // 0xf272, calendar-minus
        static constexpr const char* cuebandScheduled = "\xEF\x81\xB3";                  // 0xf073, calendar-alt
        static constexpr const char* cuebandSilence   = "\xEF\x9A\xA9";                  // 0xf6a9, volume-mute
        static constexpr const char* cuebandImpromptu = "\xEF\x81\x8B";                  // 0xf04b, play
        static constexpr const char* cuebandPreferences ="\xEF\x80\x93";                 // 0xf013, cog
        static constexpr const char* cuebandCancel    = "\xEF\x90\x90";                  // 0xf410, window-close
        static constexpr const char* cuebandInterval  = "\xEF\x8B\xB2";                  // 0xf2f2, stopwatch
        static constexpr const char* cuebandIntensity = "\xEF\xA0\xBE";                  // 0xf83e, wave-square (duplicate)
        static constexpr const char* cuebandMinus     = "\xEF\x81\xA8";                  // 0xf068, minus
        static constexpr const char* cuebandPlus      = "\xEF\x81\xA7";                  // 0xf067, plus
        static constexpr const char* cuebandPrevious  = "\xEF\x81\xA0";                  // 0xf060, arrow-left
        static constexpr const char* cuebandNext      = "\xEF\x81\xA1";                  // 0xf061, arrow-right

        static constexpr const char* cuebandCycle     = "\xEF\x81\xB9";                  // 0xf079, retweet (unused)
        static constexpr const char* cuebandCycleUp   = "\xEF\x85\xA1";                  // 0xf161, sort-amount-up (unused)
        static constexpr const char* cuebandPause     = "\xEF\x81\x8C";                  // 0xf04c, pause (unused)
#endif

      }
    }
  }
}
