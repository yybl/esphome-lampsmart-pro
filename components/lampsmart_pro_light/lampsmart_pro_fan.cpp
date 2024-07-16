#include "lampsmart_utils.h"
#include "lampsmart_pro_fan.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

#include <esp_gap_ble_api.h>
#include <esp_gatts_api.h>

namespace esphome
{
  namespace lampsmartpro
  {

    void LampSmartProFan::setup()
    {
      register_service(&LampSmartProFan::on_pair, "pair_" + this->get_object_id());
      register_service(&LampSmartProFan::on_unpair, "unpair_" + this->get_object_id());
    }

    fan::FanTraits LampSmartProFan::get_traits()
    {
      auto traits = fan::FanTraits();
      traits.set_direction(true);
      traits.set_speed(true);
      traits.set_supported_speed_count(6);
      return traits;
    }

    char *LampSmartProFan::getHostDeviceIdentifier()
    {
      uint32_t hash = this->get_object_id_hash();
      int crc = CRC16((char *)&hash, 4, 0);

      static char hostId[2];
      hostId[0] = (crc >> 8) & 255;
      hostId[1] = crc & 255;
      return hostId;
    }

    void LampSmartProFan::on_pair()
    {
      ESP_LOGD(TAG, "LampSmartProFan::on_pair called!");

      char *hostId = getHostDeviceIdentifier();
      send_packet(CMD_PAIR, hostId[0], hostId[1]);
    }

    void LampSmartProFan::on_unpair()
    {
      ESP_LOGD(TAG, "LampSmartProFan::on_unpair called!");

      char *hostId = getHostDeviceIdentifier();
      send_packet(CMD_UNPAIR, hostId[0], hostId[1]);
    }

    void LampSmartProFan::send_packet(uint16_t cmd, uint8_t arg1, uint8_t arg2)
    {
      char *hostId = getHostDeviceIdentifier();
      uint8_t *packet = (uint8_t *)buildPacket(cmd, hostId[0], hostId[1], arg1, arg2, group_id_);

      // Skip first byte (BLE packet size indicator)
      ESP_ERROR_CHECK_WITHOUT_ABORT(esp_ble_gap_config_adv_data_raw(&packet[1], 31));
      ESP_ERROR_CHECK_WITHOUT_ABORT(esp_ble_gap_start_advertising(&ADVERTISING_PARAMS));
      delay(tx_duration_);
      ESP_ERROR_CHECK_WITHOUT_ABORT(esp_ble_gap_stop_advertising());
    }

    void LampSmartProFan::control(const fan::FanCall &call) {
      if (call.get_state().has_value()) {
        this->state = *call.get_state();
      }
      if (call.get_oscillating().has_value()) {
        this->oscillating = *call.get_oscillating();
      }
      if (call.get_speed().has_value()) {
        this->speed = *call.get_speed();
        send_packet(CMD_GEAR, this->speed, 0);
      }
      if (call.get_direction().has_value()) {
        this->direction = *call.get_direction();
        send_packet(CMD_GEAR, static_cast<uint8_t>(this->direction), 0);
      }

      this->publish_state();
    }

  } // namespace lampsmartpro
} // namespace esphome

#endif
