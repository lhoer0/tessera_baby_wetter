# Baby-Wetter (`baby_wetter`)

> Lokale Kleidungsempfehlungen für Babys (angelehnt an [baby-wetter.de](https://baby-wetter.de/)) mit integrierten LVGL-Kleidungs-Icons für dein Tessera ESP32 Display (z. B. 4-Zoll Guition ESP32-S3-4848S040).

## Einrichtung

1. Binde das Plugin über den GitHub-Repository-Link in Tessera unter **Plugins -> Mit Link hinzufügen** ein (oder kopiere den Ordner nach `/config/tessera-plugins/baby_wetter/`).
2. Platziere die Kachel **Baby-Wetter Kleidung** (`1x1` bis `4x4` / Ganze Seite) auf einer Seite deines Displays und stelle die Optionen direkt an der Kachel ein:
   - **Wetter-Entität oder Außentemperatursensor (`temp_sensor`)**: Wähle einfach deine Wetter-Entität (z. B. `weather.forecast_home`) oder einen Außentemperatursensor. Bei einer Wetter-Entität werden sowohl die Außentemperatur als auch Regen automatisch ausgelesen – ein eigener Außensensor ist nicht nötig!
   - **Raumtemperatursensor (`indoor_temp_sensor`)**: Optionaler Temperatursensor oder Thermostat (`sensor.*` / `climate.*`) für das Kinder-/Schlafzimmer (genutzt für *Schlafen* und *Zuhause*; fällt automatisch auf die Raumtemperatur-Schätzung zurück, falls leer).
   - **Geburtsdatum (`birthdate`)**: Geburtsdatum des Babys im Format `TT.MM.JJJJ` oder `JJJJ-MM-TT` (z. B. `15.08.2026`) zur automatischen Berechnung des Alters (`0–3 Monate` vs. `4+ Monate`).
3. Über das **Situations-Dropdown** (direkt oben rechts auf der Kachel, in der Detail-Ansicht, in den Display-Einstellungen oder über die Home-Assistant-Entität `select.<display>_baby_wetter_situation`) kannst du jederzeit zwischen **Allgemein** *(Standard)*, **Kinderwagen**, **Babytrage**, **Auto**, **Schlafen** und **Zuhause** wechseln.

## Datenschutz

Dieses Plugin arbeitet **zu 100 % lokal** auf deinem ESP32-Display und deinem Home-Assistant-Server. Alle Kleidungs-Grafiken werden lokal als LVGL-Vektor-Icons auf dem Gerät gezeichnet. Es werden keinerlei externe Server kontaktiert und keine Sensordaten, Wetterdaten oder Geburtsdaten ins Internet übertragen.
