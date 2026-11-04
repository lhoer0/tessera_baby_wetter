# Changelog

## 0.1.6 - 2026-11-04

- Enabled direct interactive Situation dropdown on the grid tile (`OutfitTile`): tapping the situation pill on the top right opens the dropdown menu directly inside the tile without switching to the detail card.

## 0.1.5 - 2026-11-04

- Dynamic clothing grid layout on tile and detail card: uses large `4×2` for up to 8 visible items, automatically switches to `5×2` for 9–10 items, and `4×3` for 11–12 items.
- Automatically hides `Windel` (diaper) on the detail card when more than 8 items are recommended so a second page is not needed for a single item.

## 0.1.4 - 2026-11-04

- Simplified configuration: removed duplicate plugin-level inputs so sensors and birthdate are configured exclusively on the tile in the page editor.
- Added direct `weather` entity support for `temp_sensor` (reads `temperature` attribute and rain status automatically without needing a separate outdoor temperature sensor).
- Switched clothing item captions on the tile and detail card to 2-line wrapped text (`ui::text`) to prevent `...` truncation on longer names (`dicke Strumpfhose`, `dünne Handschuhe`, `Langarmbody`, `dünner Pullover`).
- Refined garment vector icons (`Handschuhe`, `Langarmbody`, `Pullover`, `Füßlinge`, `Wollwalk`) for clearer silhouettes.

## 0.1.3 - 2026-11-04

- Replaced `tessera::Tone::WARN` with `tessera::Tone::BUSY` in `GarmentIcon::draw()`.

## 0.1.2 - 2026-11-04

- Fixed C++ types (`theme::Role` and `Font::BODY`) in `GarmentIcon` and `OutfitTile`.

## 0.1.1 - 2026-11-04

- Enabled full-screen tile size (`full: true`, 2x3 on Guition 4-inch display) and improved the 2x2 / 2x3 tile grid layout to display up to 8 clothing items across 4 columns x 2 rows.
- Added built-in LVGL vector clothing illustrations (`GarmentIcon`) for all 42 clothing items so visual icons appear immediately and remain visible when remote pictures are loading or unavailable.

## 0.1.0 - 2026-11-04

- Initial release of the `baby_wetter` plugin for Tessera (Guition 4-inch ESP32-S3 and compatible displays).
- Full clothing recommendation engine and original clothing item images from `baby-wetter.de`.
- Configurable outdoor and indoor temperature sensors (`temperature_sensor` and `indoor_sensor`).
- Automatic baby age calculation (`0-3 months` vs. `4+ months`) from configurable `birthdate`.
- Interactive Situation dropdown (`Allgemein` as default, plus `Kinderwagen`, `Babytrage`, `Auto`, `Schlafen`, `Zuhause`) on the display card, in the Tessera editor, and as a Home Assistant `select` entity.
