// Activity Sync Service
// Dan Jackson, 2021

#include "cueband.h"

#ifdef CUEBAND_ACTIVITY_ENABLED

#if defined(CUEBAND_CUE_ENABLED)
#include "components/cue/CueController.h"
#endif

#include "ActivityService.h"

#include "systemtask/SystemTask.h"

#define MAX_PACKET 20

int ActivityCallback(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt, void* arg) {
  auto ActivityService = static_cast<Pinetime::Controllers::ActivityService*>(arg);
  return ActivityService->OnCommand(conn_handle, attr_handle, ctxt);
}

Pinetime::Controllers::ActivityService::ActivityService(Pinetime::System::SystemTask& system,
        Controllers::Ble& bleController,
        Controllers::Settings& settingsController,
        Pinetime::Controllers::ActivityController& activityController
    ) : m_system(system),
    bleController {bleController},
    settingsController {settingsController},
    activityController {activityController}
    {

    characteristicDefinition[0] = {
        .uuid = (ble_uuid_t*) (&activityStatusCharUuid), 
        .access_cb = ActivityCallback, 
        .arg = this, 
        .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        .val_handle = &statusHandle
    };
    characteristicDefinition[1] = {
        .uuid = (ble_uuid_t*) (&activityBlockIdCharUuid), 
        .access_cb = ActivityCallback, 
        .arg = this, 
        .flags = BLE_GATT_CHR_F_WRITE, // | BLE_GATT_CHR_F_READ
    };
    characteristicDefinition[2] = {
        .uuid = (ble_uuid_t*) (&activityBlockDataCharUuid),
        .access_cb = ActivityCallback,
        .arg = this,
        .flags = BLE_GATT_CHR_F_NOTIFY, // | BLE_GATT_CHR_F_READ
        .val_handle = &transmitHandle
    };
    characteristicDefinition[3] = {
        .uuid = (ble_uuid_t*) (&activityEncStatusCharUuid), 
        .access_cb = ActivityCallback, 
        .arg = this, 
        .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC,
        .val_handle = &encStatusHandle
    };
    characteristicDefinition[4] = {
        .uuid = (ble_uuid_t*) (&activityConfigCharUuid), 
        .access_cb = ActivityCallback, 
        .arg = this, 
        .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        .val_handle = &configHandle
    };
    characteristicDefinition[5] = {0};

    serviceDefinition[0] = {
        .type = BLE_GATT_SVC_TYPE_PRIMARY, 
        .uuid = (ble_uuid_t*) &activityUuid, 
        .characteristics = characteristicDefinition
    };
    serviceDefinition[1] = {0};
}

void Pinetime::Controllers::ActivityService::Init() {
  int res = 0;
  res = ble_gatts_count_cfg(serviceDefinition);
  ASSERT(res == 0);

  res = ble_gatts_add_svcs(serviceDefinition);
  ASSERT(res == 0);
}

void Pinetime::Controllers::ActivityService::Disconnect() {
  // Free resources
  readLogicalBlockIndex = ACTIVITY_BLOCK_INVALID;
  blockLength = 0;
  blockOffset = 0;
  free(blockBuffer);
  blockBuffer = nullptr;
  packetTransmitting = false;
}

bool Pinetime::Controllers::ActivityService::IsSending() {
    return blockBuffer != nullptr && blockLength != 0 && blockOffset < blockLength;
}

void Pinetime::Controllers::ActivityService::Idle() {
    SendNextPacket();
    if (readPending) {
        StartRead();
    }
}

void Pinetime::Controllers::ActivityService::SendNextPacket() {
    // TODO: Remove this flag as not used properly (TxNotification called when queued rather than sent)
    packetTransmitting = false;

    if (IsSending() && !packetTransmitting) {
        size_t maxPacket = MAX_PACKET;
#ifdef CUEBAND_USE_FULL_MTU
        size_t mtu = bleController.GetMtu();
        if (mtu - 3 > maxPacket) maxPacket =  mtu - 3;  // minus 1-byte opcode and 2-byte handle
#endif
        for (int i = 0; i < CUEBAND_TX_COUNT; i++) {
            if (blockBuffer == nullptr || blockLength <= 0 || blockOffset >= blockLength) break;
            size_t len = blockLength - blockOffset;
            if (len > maxPacket) len = maxPacket;
            auto* om = ble_hs_mbuf_from_flat(blockBuffer + blockOffset, len);
            packetTransmitting = true;  // BLE_GAP_EVENT_NOTIFY_TX event is called before transmission
            if (ble_gattc_notify_custom(tx_conn_handle, transmitHandle, om) == 0) { // BLE_HS_ENOMEM / os_msys_num_free()
                blockOffset += len;
            } else {
                packetTransmitting = false;
                break;
            }
        }
    }
}

void Pinetime::Controllers::ActivityService::TxNotification(ble_gap_event* event) {
  // Transmission
  // event->notify_tx.attr_handle; // attribute handle
  // event->notify_tx.conn_handle; // connection handle
  // event->notify_tx.indication;  // 0=notification, 1=indication
  // event->notify_tx.status;      // 0=successful, BLE_HS_EDONE=indication ACK, BLE_HS_ETIMEOUT=indication ACK not received, other=error
activityController.temp_transmit_count_all++;   // TODO: remove
  if (event->notify_tx.attr_handle == transmitHandle) {
activityController.temp_transmit_count++;   // TODO: remove
      packetTransmitting = false;
      //tx_conn_handle = event->notify_tx.conn_handle;
      //SendNextPacket();
  }
}

void Pinetime::Controllers::ActivityService::StartRead() {
    if (!readPending) { return; }
    readPending = false;

    uint16_t len = ACTIVITY_BLOCK_SIZE;
    uint16_t prefix = sizeof(len);  // 2

    if (readLogicalBlockIndex == ACTIVITY_BLOCK_INVALID) { return; }

    if (blockBuffer == nullptr) {
        blockBuffer = (uint8_t *)malloc(prefix + len);
    }

    if (blockBuffer == nullptr || IsSending()) {
        len = 0;        // Error: memory failure or busy
    } else {
        if (!activityController.ReadLogicalBlock(readLogicalBlockIndex, blockBuffer + prefix)) {
            len = 0;    // read failure
// HACK: Temporary dummy data for out-of-range blocks
#if defined(CUEBAND_DEBUG_DUMMY_MISSING_BLOCKS)
len = ACTIVITY_BLOCK_SIZE;
for (int i = 0; i < len; i++) {
blockBuffer[i + prefix] = (uint8_t)i;
}
#endif
        }
    }

    if (len == 0) {
        // Send empty length
        auto* omLen = ble_hs_mbuf_from_flat(&len, sizeof(len));
        ble_gattc_notify_custom(tx_conn_handle, transmitHandle, omLen);
    } else {
        memcpy(blockBuffer, &len, sizeof(len));
        blockOffset = 0;
        blockLength = prefix + len;
//                SendNextPacket();

        readLogicalBlockIndex++;
    }

}

int Pinetime::Controllers::ActivityService::OnCommand(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt) {

    bool trusted = true;
#ifdef CUEBAND_TRUSTED_ACTIVITY
    trusted = bleController.IsTrusted();
#endif

#ifdef CUEBAND_ACTIVITY_ENABLED
    m_system.CommsActivity();
#endif

    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) { // Reading

        if (attr_handle == statusHandle || attr_handle == encStatusHandle) {
            uint8_t status[20];

            // @0 Earliest available logical block ID
            uint32_t earliestBlockId = activityController.EarliestLogicalBlock();
            status[0] = (uint8_t)(earliestBlockId >> 0);
            status[1] = (uint8_t)(earliestBlockId >> 8);
            status[2] = (uint8_t)(earliestBlockId >> 16);
            status[3] = (uint8_t)(earliestBlockId >> 24);

            // @4 Last available logical block ID (the active block -- partially written)
            uint32_t activeBlockId = activityController.ActiveLogicalBlock();
            status[4] = (uint8_t)(activeBlockId >> 0);
            status[5] = (uint8_t)(activeBlockId >> 8);
            status[6] = (uint8_t)(activeBlockId >> 16);
            status[7] = (uint8_t)(activeBlockId >> 24);

            // @8 Size (bytes) of each block
            uint16_t blockSize = activityController.BlockSize(); 
            status[8] = (uint8_t)(blockSize >> 0);
            status[9] = (uint8_t)(blockSize >> 8);

            // @10  Size of each epoch (seconds)
            uint16_t epochInterval = activityController.EpochInterval(); 
            status[10] = (uint8_t)(epochInterval >> 0);
            status[11] = (uint8_t)(epochInterval >> 8);

            // @12 Maximum number of epoch samples in each block
            uint16_t maxSamplesPerBlock = activityController.MaxSamplesPerBlock(); 
            status[12] = (uint8_t)(maxSamplesPerBlock >> 0);
            status[13] = (uint8_t)(maxSamplesPerBlock >> 8);

            // @14 Status flags
            uint8_t status_flags = 0x00;
            if (firmwareValidator.IsValidated()) status_flags |= 0x01;      // b0 = firmware validated
            if (activityController.IsInitialized()) status_flags |= 0x02;   // b1 = service initialized
            if (bleController.IsTrusted()) status_flags |= 0x04;            // b2 = connection trusted
            if (m_system.GetBatteryController().IsPowerPresent()) status_flags |= 0x08;   // b3 = externally connected: power present
            status[14] = status_flags;

            // @15 Reserved
            status[15] = 0x00;

            // @16 Challenge bytes
            uint32_t challenge = 0;
#ifdef CUEBAND_TRUSTED_CONNECTION
            challenge = bleController.GetChallenge();
#endif
            status[16] = (uint8_t)(challenge >> 0);
            status[17] = (uint8_t)(challenge >> 8);
            status[18] = (uint8_t)(challenge >> 16);
            status[19] = (uint8_t)(challenge >> 24);


            int res = os_mbuf_append(ctxt->om, &status, sizeof(status));
            return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

        } else if (attr_handle == configHandle) {
            // Read activity configuration
            uint8_t cfg[10];

            // @0 Version
            cfg[0] = 1; 

            // @1 Reserved
            cfg[1] = 0; 

            // @2 Format
            uint16_t format = activityController.getFormat();
            cfg[2] = (uint8_t)(format >> 0);
            cfg[3] = (uint8_t)(format >> 8);

            // @4 Epoch interval
            uint16_t epochInterval = activityController.getEpochInterval();
            cfg[4] = (uint8_t)(epochInterval >> 0);
            cfg[5] = (uint8_t)(epochInterval >> 8);

            // @6 HRM interval
            uint16_t hrmInterval = activityController.getHrmInterval();
            cfg[6] = (uint8_t)(hrmInterval >> 0);
            cfg[7] = (uint8_t)(hrmInterval >> 8);

            // @8 HRM duration
            uint16_t hrmDuration = activityController.getHrmDuration();
            cfg[8] = (uint8_t)(hrmDuration >> 0);
            cfg[9] = (uint8_t)(hrmDuration >> 8);

            int res = os_mbuf_append(ctxt->om, &cfg, sizeof(cfg));
            return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

        } else if (ble_uuid_cmp(ctxt->chr->uuid, (ble_uuid_t*) &activityBlockIdCharUuid) == 0 && trusted) {
            int res = os_mbuf_append(ctxt->om, &readLogicalBlockIndex, sizeof(readLogicalBlockIndex));
            return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }

    } if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) { // Writing
        size_t notifSize = OS_MBUF_PKTLEN(ctxt->om);
        uint8_t data[notifSize + 1];
        data[notifSize] = '\0';     // NULL-terminate
        os_mbuf_copydata(ctxt->om, 0, notifSize, data);

        // Writing to the block id
        if (ble_uuid_cmp(ctxt->chr->uuid, (ble_uuid_t*) &activityBlockIdCharUuid) == 0 && trusted) {
            readLogicalBlockIndex = activityController.ActiveLogicalBlock();
            if (notifSize >= 4) { 
                readLogicalBlockIndex = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
            }
            // Trigger the response
            tx_conn_handle = conn_handle;
            readPending = true;
            // StartRead() is called in idle

        } else if (attr_handle == statusHandle || attr_handle == encStatusHandle) {

            // Wake
            {
                const char *cmdWake = "Wake!";
                if (trusted && notifSize == strlen(cmdWake) && memcmp(data, cmdWake, strlen(cmdWake)) == 0) {
                    m_system.PushMessage(Pinetime::System::Messages::GoToRunning);
                }
            }

            // Sleep
            {
                const char *cmdSleep = "Sleep!";
                if (trusted && notifSize == strlen(cmdSleep) && memcmp(data, cmdSleep, strlen(cmdSleep)) == 0) {
                    m_system.PushMessage(Pinetime::System::Messages::GoToSleep);
                }
            }

            // Erase
            {
                const char *cmdErase = "Erase!";
                if (trusted && notifSize == strlen(cmdErase) && memcmp(data, cmdErase, strlen(cmdErase)) == 0) {
                    activityController.DestroyData();
#ifdef CUEBAND_CUE_ENABLED
                    m_system.GetCueController().Reset(true);
#endif
                }
            }

            // Validate
            {
                const char *cmdValidate = "Validate!";
                if (trusted && notifSize == strlen(cmdValidate) && memcmp(data, cmdValidate, strlen(cmdValidate)) == 0) {
#ifdef CUEBAND_ALLOW_REMOTE_FIRMWARE_VALIDATE
                    firmwareValidator.Validate();
#endif
                }
            }

            // Reset
            {
                const char *cmdReset = "Reset!";
                if (trusted && notifSize == strlen(cmdReset) && memcmp(data, cmdReset, strlen(cmdReset)) == 0) {
                    firmwareValidator.Reset();
                }
            }

            // Trust next connection (within 120 seconds), e.g. a separate DFU connection
            {
                const char *cmdReconnect = "Reconnect!";
                if (trusted && notifSize == strlen(cmdReconnect) && memcmp(data, cmdReconnect, strlen(cmdReconnect)) == 0) {
#if defined(CUEBAND_TRUSTED_CONNECTION)
                    bleController.SetTrusted(true);
#endif
                }
            }

            // Vibration
            {
                uint32_t value = 0xffffffff;
                if (data[0] == 'V' && data[1] == '@' && notifSize >= 1 + 1 + 4) {
                    value = data[2] | (data[3] << 8) | (data[4] << 16) | (data[5] << 24);
                } else if (data[0] == 'V' && data[1] == '#' && notifSize >= 1 + 1) {
                    value = (uint32_t)strtol((const char *)data + 2, NULL, 0);
                }
                if (value != 0xffffffff) {
#ifdef CUEBAND_MOTOR_PATTERNS
                    m_system.GetMotorController().RunIndex(value);
#else
                    m_system.GetMotorController().RunForDuration(value);
#endif
                }
            }

            // Challenge Response
            {
                if (data[0] == '!' && notifSize >= 1 + 4) {
#if defined(CUEBAND_TRUSTED_CONNECTION)
                    uint32_t response = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
                    bleController.ProvideChallengeResponse(response);
#endif
                }
            }

            // Provide key directly (testing only)
            {
                if (data[0] == '@' && notifSize >= 1) {
#if defined(CUEBAND_TRUSTED_CONNECTION)
                    const char *key = (const char *)data + 1;
                    size_t length = notifSize - 1;
                    bleController.ProvideKey(key, length);
#endif
                }
            }

            // Remote options: none
            {
                const char *cmd = "OptNone";
                if (trusted && notifSize == strlen(cmd) && memcmp(data, cmd, strlen(cmd)) == 0) {
#if defined(CUEBAND_CUE_ENABLED)
                    m_system.GetCueController().SetOptionsMaskValue(0, 0);
#endif
                }
            }

            // Remote options: cueing
            {
                const char *cmd = "OptCue";
                if (trusted && notifSize == strlen(cmd) && memcmp(data, cmd, strlen(cmd)) == 0) {
#if defined(CUEBAND_CUE_ENABLED)
                    m_system.GetCueController().SetOptionsMaskValue(
                        CueController::OPTIONS_CUE_ENABLED | CueController::OPTIONS_CUE_STATUS | CueController::OPTIONS_CUE_DETAILS | CueController::OPTIONS_CUE_MANUAL,
                        CueController::OPTIONS_CUE_ENABLED | CueController::OPTIONS_CUE_STATUS | CueController::OPTIONS_CUE_DETAILS | CueController::OPTIONS_CUE_MANUAL
                    );
#endif
                }
            }

            // Remote options: disallowed cueing
            {
                const char *cmd = "OptDis";
                if (trusted && notifSize == strlen(cmd) && memcmp(data, cmd, strlen(cmd)) == 0) {
#if defined(CUEBAND_CUE_ENABLED)
                    m_system.GetCueController().SetOptionsMaskValue(
                        CueController::OPTIONS_CUE_ENABLED | CueController::OPTIONS_CUE_STATUS | CueController::OPTIONS_CUE_DETAILS | CueController::OPTIONS_CUE_MANUAL | CueController::OPTIONS_CUE_DISALLOW,
                        CueController::OPTIONS_CUE_ENABLED | CueController::OPTIONS_CUE_STATUS | CueController::OPTIONS_CUE_DETAILS | CueController::OPTIONS_CUE_MANUAL | CueController::OPTIONS_CUE_DISALLOW
                    );
#endif
                }
            }

        } else if (attr_handle == configHandle && notifSize >= 10) {
            uint8_t version = data[0];
            if (version == 0 || version == 1) {
                //uint8_t reserved = data[1];
                uint16_t format = (uint16_t)(data[2] | (data[3] << 8));
                uint16_t epochInterval = (uint16_t)(data[4] | (data[5] << 8));
                uint16_t hrmInterval = (uint16_t)(data[6] | (data[7] << 8));
                uint16_t hrmDuration = (uint16_t)(data[8] | (data[9] << 8));

                if (activityController.ChangeConfig(false, format, epochInterval, hrmInterval, hrmDuration)) {
                    activityController.FlushBlock();
                    activityController.ChangeConfig(true, format, epochInterval, hrmInterval, hrmDuration);
                    activityController.DeferWriteConfig();
                    activityController.StartNewBlock();
                }
            }

        }

    }
    return 0;
}

#endif
