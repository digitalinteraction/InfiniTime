#pragma once
#include "cueband.h"
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <components/heartrate/Ppg.h>

namespace Pinetime {
  namespace Drivers {
    class Hrs3300;
  }
  namespace Controllers {
    class HeartRateController;
  }
  namespace Applications {
    class HeartRateTask {
    public:
      enum class Messages : uint8_t { GoToSleep, WakeUp, StartMeasurement, StopMeasurement };
      enum class States { Idle, Running };

      explicit HeartRateTask(Drivers::Hrs3300& heartRateSensor, Controllers::HeartRateController& controller);
      void Start();
      void Work();
      void PushMessage(Messages msg);

#ifdef CUEBAND_HR_EPOCH
      void SetHrEpoch(bool hrEpoch) { this->hrEpoch = hrEpoch; }
      bool IsHrEpoch() { return this->hrEpoch; }
      // Get heart rate tracker stats and clear
      int HrStats(int *meanBpm, int *minBpm, int *maxBpm, bool clear);
      int sumBpm = 0;
      int countBpm = 0;
      int minBpm = 0;
      int maxBpm = 0;
#endif

#ifdef CUEBAND_BUFFER_RAW_HR
      void SetRawMeasurement(bool rawMeasurement) { this->rawMeasurement = rawMeasurement; }
      bool IsRawMeasurement() { return this->rawMeasurement; }

      // Only public for adding dummy test measurements
      void BufferAdd(uint32_t measurement);

      // If NULL pointer: count of buffer entries available since previous cursor position
      // otherwise: read from buffer from previous cursor position, return count, update cursor position
      size_t BufferRead(uint32_t *data, size_t *cursor, size_t maxCount);
#endif

    private:
      static void Process(void* instance);
      void StartMeasurement();
      void StopMeasurement();

      TaskHandle_t taskHandle;
      QueueHandle_t messageQueue;
      States state = States::Running;
      Drivers::Hrs3300& heartRateSensor;
      Controllers::HeartRateController& controller;
      Controllers::Ppg ppg;
      bool measurementStarted = false;

#ifdef CUEBAND_HR_EPOCH
      bool hrEpoch = false;
#endif
#ifdef CUEBAND_BUFFER_RAW_HR
      bool rawMeasurement = false;
      uint8_t lastMeasurement = 0;
      unsigned int lastMeasurementAge = 0;
      static const size_t hrmCapacity = 32;
      volatile size_t numSamples = 0;
      uint32_t hrmBuffer[hrmCapacity];
#endif
    };

  }
}
