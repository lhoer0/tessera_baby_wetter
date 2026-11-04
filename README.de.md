# Baby-Wetter (`baby_wetter`)

> Kleidungsempfehlungen und Original-Grafiken von [baby-wetter.de](https://baby-wetter.de/) für dein Tessera ESP32 Display (z. B. 4-Zoll Guition ESP32-S3-4848S040).

## Einrichtung

1. Kopiere den Ordner `baby_wetter` nach `/config/tessera-plugins/baby_wetter/` in Home Assistant (oder binde ihn später über einen GitHub-Repository-Link in Tessera unter **Plugins -> Mit Link hinzufügen** ein).
2. Füge das Plugin deinem Display hinzu und konfiguriere die Eingaben (`inputs`):
   - **Außentemperatursensor (`temperature_sensor`)**: Dein Home-Assistant-Außentemperatursensor (genutzt für *Allgemein*, *Kinderwagen*, *Babytrage*, *Auto*).
   - **Raumtemperatursensor (`indoor_sensor`)**: Zweiter Temperatursensor für das Kinder-/Schlafzimmer (genutzt für *Schlafen* und *Zuhause*; fällt automatisch auf die Raumtemperatur-Schätzung zurück, falls kein Wert vorliegt).
   - **Geburtsdatum (`birthdate`)**: Geburtsdatum des Babys im Format `TT.MM.JJJJ` oder `JJJJ-MM-TT` (z. B. `15.08.2026`) zur automatischen Berechnung des Alters (`0–3 Monate` vs. `4+ Monate`).
3. Platziere die Kachel **Baby-Wetter Kleidung** (`1x1` bis `4x4`) auf einer Seite deines Displays.
4. Über das **Situations-Dropdown** (auf dem Display in der Kachel/Detail-Ansicht, in den Display-Einstellungen oder über die Home-Assistant-Entität `select.<display>_baby_wetter_situation`) kannst du jederzeit zwischen **Allgemein** *(Standard)*, **Kinderwagen**, **Babytrage**, **Auto**, **Schlafen** und **Zuhause** wechseln.

## Datenschutz

Das Tessera-Add-on lädt ausschließlich die statischen Kleidungs-PNG-Grafiken von `https://baby-wetter.de/wp-content/uploads/...`. Es werden keinerlei Sensordaten oder das Geburtsdatum an `baby-wetter.de` übertragen – die gesamte Berechnung erfolgt lokal auf dem ESP32.
