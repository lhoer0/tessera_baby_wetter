# Baby-Wetter (`baby_wetter`)

> Clothing recommendations and original graphics from [baby-wetter.de](https://baby-wetter.de/) for your Tessera ESP32 display (such as the 4-inch Guition ESP32-S3-4848S040).

## Set up

1. Copy the `baby_wetter` folder into Home Assistant under `/config/tessera-plugins/baby_wetter/` (or add it via a GitHub repository link in the Tessera app under **Plugins -> Add with a link**).
2. Add the plugin to your screen in Tessera and configure the inputs:
   - **Außentemperatursensor (`temperature_sensor`)**: Your Home Assistant outdoor temperature sensor (used for *Allgemein*, *Kinderwagen*, *Babytrage*, *Auto*).
   - **Raumtemperatursensor (`indoor_sensor`)**: Your Home Assistant room temperature sensor (used for *Schlafen* and *Zuhause*; automatically falls back to room temperature estimation if unavailable).
   - **Geburtsdatum (`birthdate`)**: Your baby's birthdate in `TT.MM.JJJJ` or `JJJJ-MM-TT` format (e.g., `15.08.2026`) to automatically calculate the age group (`0–3 Monate` vs. `4+ Monate`).
3. Place the **Baby-Wetter Kleidung** tile (`1x1` up to `4x4`) on a page of your display.
4. Tap the tile (or the **Situation** dropdown pill) on the display to open the full detail card and switch the **Situation** (`Allgemein` by default, `Kinderwagen`, `Babytrage`, `Auto`, `Schlafen`, `Zuhause`) via the dropdown menu.

## Privacy

When your screen has picture support (such as the 4-inch Guition display), the Tessera add-on fetches the clothing item PNG icons from `https://baby-wetter.de/wp-content/uploads/...`. No personal data, sensor values, or birthdates are sent to `baby-wetter.de`; all calculations happen locally on the ESP32 display. See [baby-wetter.de Privacy Policy](https://baby-wetter.de/datenschutzerklaerung/).
