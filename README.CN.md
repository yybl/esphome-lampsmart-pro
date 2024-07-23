# esphome-lampsmart-pro
Custom ESPHome component to interface with Chinese "LampSmart Pro" Bluetooth lights.

Basically it's https://github.com/aronsky/esphome-components and https://github.com/MasterDevX/lampify combined to work with more (or different?) lights.

## How to try it

1. Create an ESPHome configuration that references the repo (using `external_components`)
2. Add a light entity to the configuration with the `lampsmart_pro_light` platform(将灯和风扇的group设置为同一个值，同一个组配对一次即可)
3. Build the configuration and flash it to an ESP32 device (since BLE is used, ESP8266-based devices are a no-go)
4. Add the new ESPHome node to your Home Assistant instance
5. Use the newly exposed service (`esphome.<esphome-node-name>_pair`) to pair with your light (call the service withing 5 seconds of powering it with a switch).
6. Enjoy controlling your BLE light with Home Assistant!

## Example configuration (LampSmart Pro)

```yaml
external_components:
  # shorthand
  source: github://zt8989/esphome-lampsmart-pro

light:
  - platform: lampsmart_pro_light
    name: Kitchen Light
    duration: 1000
    group: 0
    default_transition_length: 0s
fan:
  - platform: lampsmart_pro_light
    name: Kitchen Fan
    duration: 1000
    group: 0
    default_transition_length: 0s
```
