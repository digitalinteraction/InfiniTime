#pragma once

#include "cueband.h"

#include <cstdint>
#define min // workaround: nimble's min/max macros conflict with libstdc++
#define max
#include <host/ble_gap.h>
#undef max
#undef min
#include "Version.h"
#include "cueband.h"

namespace Pinetime {
  namespace Controllers {
    class DeviceInformationService {
    public:
      DeviceInformationService();
      void Init();
#ifdef CUEBAND_SERIAL_ADDRESS
      void SetAddress(const char *address);
#endif

      int OnDeviceInfoRequested(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt);

    private:
      static constexpr uint16_t deviceInfoId {0x180a};
      static constexpr uint16_t manufacturerNameId {0x2a29};
      static constexpr uint16_t modelNumberId {0x2a24};
      static constexpr uint16_t serialNumberId {0x2a25};
      static constexpr uint16_t fwRevisionId {0x2a26};
      static constexpr uint16_t hwRevisionId {0x2a27};
      static constexpr uint16_t swRevisionId {0x2a28};

      static constexpr const char* manufacturerName = "PINE64"
#ifdef CUEBAND_VERSION_MANUFACTURER_EXTENSION
      CUEBAND_VERSION_MANUFACTURER_EXTENSION
#endif
      ;
      static constexpr const char* modelNumber = "PineTime"
#ifdef CUEBAND_VERSION_MODEL_EXTENSION
      CUEBAND_VERSION_MODEL_EXTENSION
#endif
      ;
      static constexpr const char* hwRevision = "1.0.0";
#ifdef CUEBAND_SERIAL_ADDRESS
      char serialNumber[32] = "0";
#else
      static constexpr const char* serialNumber = "0";
#endif
      static constexpr const char* fwRevision = Version::VersionString();
      static constexpr const char* swRevision = "InfiniTime"
#ifdef CUEBAND_VERSION_SOFTWARE_EXTENSION
      CUEBAND_VERSION_SOFTWARE_EXTENSION
#endif
      ;

      static constexpr ble_uuid16_t deviceInfoUuid {.u {.type = BLE_UUID_TYPE_16}, .value = deviceInfoId};

      static constexpr ble_uuid16_t manufacturerNameUuid {.u {.type = BLE_UUID_TYPE_16}, .value = manufacturerNameId};

      static constexpr ble_uuid16_t modelNumberUuid {.u {.type = BLE_UUID_TYPE_16}, .value = modelNumberId};

      static constexpr ble_uuid16_t serialNumberUuid {.u {.type = BLE_UUID_TYPE_16}, .value = serialNumberId};

      static constexpr ble_uuid16_t fwRevisionUuid {.u {.type = BLE_UUID_TYPE_16}, .value = fwRevisionId};

      static constexpr ble_uuid16_t hwRevisionUuid {.u {.type = BLE_UUID_TYPE_16}, .value = hwRevisionId};

      static constexpr ble_uuid16_t swRevisionUuid {.u {.type = BLE_UUID_TYPE_16}, .value = swRevisionId};

      struct ble_gatt_chr_def characteristicDefinition[7];
      struct ble_gatt_svc_def serviceDefinition[2];
    };
  }
}