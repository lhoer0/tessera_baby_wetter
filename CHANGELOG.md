# Changelog

## 0.1.1 - 2026-11-04

- Enabled full-screen tile size (`full: true`, 2x3 on Guition 4-inch display) and improved the 2x2 / 2x3 tile grid layout to display up to 8 clothing items across 4 columns x 2 rows.
- Added built-in LVGL vector clothing illustrations (`GarmentIcon`) for all 42 clothing items so visual icons appear immediately and remain visible when remote pictures are loading or unavailable.

## 0.1.0 - 2026-11-04

- Initial release of the `baby_wetter` plugin for Tessera (Guition 4-inch ESP32-S3 and compatible displays).
- Full clothing recommendation engine and original clothing item images from `baby-wetter.de`.
- Configurable outdoor and indoor temperature sensors (`temperature_sensor` and `indoor_sensor`).
- Automatic baby age calculation (`0-3 months` vs. `4+ months`) from configurable `birthdate`.
- Interactive Situation dropdown (`Allgemein` as default, plus `Kinderwagen`, `Babytrage`, `Auto`, `Schlafen`, `Zuhause`) on the display card, in the Tessera editor, and as a Home Assistant `select` entity.
