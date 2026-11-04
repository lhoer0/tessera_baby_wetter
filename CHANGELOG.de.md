# Changelog

## 0.1.6 - 2026-11-04

- Situations-Dropdown direkt im Kachel-Layout (`OutfitTile`) bedienbar gemacht: Ein Tippen auf den Situations-Button oben rechts klappt das Auswahlmenü direkt auf der Kachel auf, ohne zuerst die Detailansicht zu öffnen.

## 0.1.5 - 2026-11-04

- Dynamisches Kleidungs-Raster auf Kachel und Detailkarte: bleibt bis 8 sichtbare Teile im großen `4×2`-Raster, wechselt bei 9–10 Teilen automatisch auf `5×2` und bei 11–12 Teilen auf `4×3`.
- Windel wird auch in der Detailansicht automatisch ausgeblendet, sobald mehr als 8 Kleidungsstücke empfohlen werden, damit keine unnötige zweite Seite entsteht.

## 0.1.4 - 2026-11-04

- Vereinfachte Einrichtung: Doppelte Plugin-Eingaben entfernt – Sensoren und Geburtsdatum werden jetzt ausschließlich direkt an der Kachel auf der Seite hinterlegt.
- Direkte Unterstützung für Wetter-Entitäten (`weather.*`, z. B. `weather.forecast_home`) bei der Außentemperatur: liest das `temperature`-Attribut sowie Regen automatisch aus, sodass kein separater Außentemperatursensor nötig ist.
- Zweizeilige Textdarstellung (`ui::text`) für alle Kleidungsstücke auf Kachel und Detailkarte, damit längere Begriffe (`dicke Strumpfhose`, `dünne Handschuhe`, `Langarmbody`, `dünner Pullover`) nicht mehr mit `...` abgeschnitten werden.
- Kleidungs-Icons (`Handschuhe`, `Langarmbody`, `Pullover`, `Füßlinge`, `Wollwalk`) optisch überarbeitet.

## 0.1.3 - 2026-11-04

- `tessera::Tone::WARN` durch `tessera::Tone::BUSY` in `GarmentIcon::draw()` ersetzt.

## 0.1.2 - 2026-11-04

- C++-Typen (`theme::Role` und `Font::BODY`) in `GarmentIcon` und `OutfitTile` korrigiert.

## 0.1.1 - 2026-11-04

- Vollbild-Kachelgröße (`full: true`, 2x3 auf dem 4-Zoll Guition Display) freigeschaltet und das Kachel-Raster (4 Spalten x 2 Zeilen) für 2x2- und 2x3-Kacheln optimiert, sodass bis zu 8 Kleidungsstücke direkt auf der Kachel Platz finden.
- Eingebaute LVGL-Vektor-Kleidungsgrafiken (`GarmentIcon`) für alle 42 Kleidungsstücke ergänzt, damit jedes Kleidungsstück sofort (und auch ohne erreichbaren Bild-Server) als farbiges Kleidungs-Icon dargestellt wird.

## 0.1.0 - 2026-11-04

- Erste Version des `baby_wetter`-Plugins für Tessera (optimiert für das 4-Zoll Guition ESP32-S3 Display).
- Vollständige Kleidungsempfehlungen und Original-Grafiken von `baby-wetter.de`.
- Konfigurierbarer Außentemperatursensor sowie zweiter Raumtemperatursensor (für *Schlafen* und *Zuhause*).
- Automatische Altersberechnung (`0–3 Monate` vs. `4+ Monate`) über das eingegebene Geburtsdatum.
- Interaktives Dropdown-Menü für die Situation (`Allgemein` als Standard, sowie `Kinderwagen`, `Babytrage`, `Auto`, `Schlafen`, `Zuhause`) direkt auf dem Display, im Tessera-Editor und in Home Assistant.
