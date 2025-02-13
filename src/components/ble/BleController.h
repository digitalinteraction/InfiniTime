#pragma once

#include "cueband.h"

#include <array>
#include <cstdint>

#ifdef CUEBAND_BUILD_FIXES
#include <cstddef>  // size_t
#endif

namespace Pinetime {
  namespace Controllers {
    class Ble {
    public:
      using BleAddress = std::array<uint8_t, 6>;
      enum class FirmwareUpdateStates { Idle, Running, Validated, Error };
      enum class AddressTypes { Public, Random, RPA_Public, RPA_Random };

      Ble() = default;
      bool IsConnected() const;
      void Connect();
      void Disconnect();

      bool IsRadioEnabled() const;
      void EnableRadio();
      void DisableRadio();

      void StartFirmwareUpdate();
      void StopFirmwareUpdate();
      void FirmwareUpdateTotalBytes(uint32_t totalBytes);
      void FirmwareUpdateCurrentBytes(uint32_t currentBytes);
      void State(FirmwareUpdateStates state) {
        firmwareUpdateState = state;
      }

      bool IsFirmwareUpdating() const {
        return isFirmwareUpdating;
      }
      uint32_t FirmwareUpdateTotalBytes() const {
        return firmwareUpdateTotalBytes;
      }
      uint32_t FirmwareUpdateCurrentBytes() const {
        return firmwareUpdateCurrentBytes;
      }
      FirmwareUpdateStates State() const {
        return firmwareUpdateState;
      }

      void Address(BleAddress&& addr) {
        address = addr;
      }
      const BleAddress& Address() const {
        return address;
      }
      void AddressType(AddressTypes t) {
        addressType = t;
      }
      void SetPairingKey(uint32_t k) {
        pairingKey = k;
      }
      uint32_t GetPairingKey() const {
        return pairingKey;
      }

#if defined(CUEBAND_TRUSTED_CONNECTION)
      bool IsTrusted() const {
        if (!isConnected) return false;
        // Bonded connections are trusted
        if (bonded) return true;
        // Connections established shortly after restarting are trusted
        if (connectedElapsed < 5 * 60) return true;
        // Connections that have been authenticated are trusted
        if (trusted) return true;
        return false;
      }
      void SetTrusted(bool nearFuture = false) {
        // Trust the current connection, if established
        if (isConnected) trusted = true;
        // Trust a re-connection in the near future
        if (nearFuture) {
          trustSoonElapsed = elapsed;
        }
      }
      uint32_t GetChallenge();
      bool ProvideChallengeResponse(uint32_t response);
      bool ProvideKey(const char *key, size_t length);  // testing only
      void SetBonded() {
        if (isConnected) bonded = true;
      }
      void TimeChanged(uint32_t now) {
        this->now = now;
        elapsed++;
      }
      uint32_t GetElapsed() {   // Elapsed up-time (incremented rather than blindly trust system uptime), e.g. to only allow DFU within a certain time after reboot
        return elapsed;
      }
      uint32_t now = 0;
      uint32_t elapsed = 0;
      bool trusted = false;
      bool bonded = false;
      uint32_t trustSoonElapsed = 0xffffffff;
      uint32_t connectedElapsed = 0xffffffff;
      uint32_t connectedTime = 0xffffffff;
#else
      bool IsTrusted() const { return true; }
#endif

#if defined(CUEBAND_SERVICE_UART_ENABLED) || defined(CUEBAND_ACTIVITY_ENABLED)
      void SetMtu(size_t newMtu) {
        mtu = newMtu;
      }

      size_t GetMtu() {
        return mtu;
      }
#endif

    private:
#if defined(CUEBAND_SERVICE_UART_ENABLED) || defined(CUEBAND_ACTIVITY_ENABLED)
      size_t mtu = 23;
#endif

      bool isConnected = false;
      bool isRadioEnabled = true;
      bool isFirmwareUpdating = false;
      uint32_t firmwareUpdateTotalBytes = 0;
      uint32_t firmwareUpdateCurrentBytes = 0;
      FirmwareUpdateStates firmwareUpdateState = FirmwareUpdateStates::Idle;
      BleAddress address;
      AddressTypes addressType;
      uint32_t pairingKey = 0;
    };
  }
}
