#include "components/motion/MotionController.h"

using namespace Pinetime::Controllers;

void MotionController::Update(int16_t x, int16_t y, int16_t z, uint32_t nbSteps) {
  if (this->nbSteps != nbSteps && service != nullptr) {
    service->OnNewStepCountValue(nbSteps);
  }

  if(service != nullptr && (this->x != x || this->y != y || this->z != z)) {
    service->OnNewMotionValues(x, y, z);
  }

  this->x = x;
  this->y = y;
  this->z = z;
  this->nbSteps = nbSteps;
}

bool MotionController::ShouldWakeUp(bool isSleeping) {
  if ((x + 335) <= 670 && z < 0) {
    if (not isSleeping) {
      if (y <= 0) {
        return false;
      } else {
        lastYForWakeUp = 0;
        return false;
      }
    }

    if (y >= 0) {
      lastYForWakeUp = 0;
      return false;
    }
    if (y + 230 < lastYForWakeUp) {
      lastYForWakeUp = y;
      return true;
    }
  }
  return false;
}
void MotionController::IsSensorOk(bool isOk) {
  isSensorOk = isOk;
}
void MotionController::Init(Pinetime::Drivers::Bma421::DeviceTypes types) {
  switch(types){
    case Drivers::Bma421::DeviceTypes::BMA421: this->deviceType = DeviceTypes::BMA421; break;
    case Drivers::Bma421::DeviceTypes::BMA425: this->deviceType = DeviceTypes::BMA425; break;
    default: this->deviceType = DeviceTypes::Unknown; break;
  }
#ifdef CUEBAND_BUFFER_ENABLED
  accelValues = NULL;
  totalSamples = 0;
  lastCount = 0;
#endif
}

void MotionController::SetService(Pinetime::Controllers::MotionService* service) {
  this->service = service;
}

#ifdef CUEBAND_BUFFER_ENABLED
void MotionController::GetBufferData(int16_t **accelValues, unsigned int *lastCount, unsigned int *totalSamples) {
  *accelValues = this->accelValues;
  *lastCount = this->lastCount;
  *totalSamples = this->totalSamples;
  return;
}

void MotionController::SetBufferData(int16_t *accelValues, unsigned int lastCount, unsigned int totalSamples) {
  this->accelValues = accelValues;
  this->lastCount = lastCount;
  this->totalSamples = totalSamples;
}
#endif
