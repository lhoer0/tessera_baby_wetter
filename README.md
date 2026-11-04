# Baby-Wetter (`baby_wetter`)

> Clothing recommendations and original graphics from [baby-wetter.de](https://baby-wetter.de/) for your Tessera ESP32 display (such as the 4-inch Guition ESP32-S3-4848S040).

## Set up

1. Add the plugin via its GitHub repository link in the Tessera app under **Plugins -> Add with a link** (or copy the folder into `/config/tessera-plugins/baby_wetter/`).
2. Place the **Baby-Wetter Kleidung** tile (`1x1` up to `4x4` / full page) on a page of your display and configure its options right on the page:
   - **Wetter-Entität oder Außentemperatursensor (`temp_sensor`)**: Choose your Home Assistant weather entity (e.g., `weather.forecast_home`) or an outdoor temperature sensor. When a weather entity is selected, both the outdoor temperature (`temperature` attribute) and rain condition are read automatically without needing a separate outdoor sensor!
   - **Raumtemperatursensor (`indoor_temp_sensor`)**: Optional room temperature sensor or thermostat (`sensor.*` / `climate.*`) for *Schlafen* and *Zuhause* (automatically falls back to room temperature estimation from the outdoor temperature if left empty).
   - **Geburtsdatum (`birthdate`)**: Your baby's birthdate in `TT.MM.JJJJ` or `JJJJ-MM-TT` format (e.g., `15.08.2026`) to automatically calculate the age in months (`0–3 Monate` vs. `4+ Monate`).
3. Tap the tile (or the **Situation** dropdown pill) on the display to open the full detail card and switch the **Situation** (`Allgemein` by default, `Kinderwagen`, `Babytrage`, `Auto`, `Schlafen`, `Zuhause`) via the dropdown menu.

## Privacy

When your screen has picture support (such as the 4-inch Guition display), the Tessera add-on fetches the clothing item PNG icons from `https://baby-wetter.de/wp-content/uploads/...`. No personal data, sensor values, or birthdates are sent to `baby-wetter.de`; all calculations happen locally on the ESP32 display. See [baby-wetter.de Privacy Policy](https://baby-wetter.de/datenschutzerklaerung/).
