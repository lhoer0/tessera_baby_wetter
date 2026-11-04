#include "baby_wetter.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace esphome::baby_wetter {

using tessera::Font;
namespace ui = tessera::ui;

struct ItemDef {
  const char *key;
  const char *fallback_de;
};

// All 42 clothing item definitions
static constexpr ItemDef kCatalog[] = {
  {"item_0", "Windel"},
  {"item_1", "Langarmbody"},
  {"item_2", "Socken"},
  {"item_3", "Schlafanzug"},
  {"item_4", "Schlafsack 3-3,5 TOG"},
  {"item_5", "Schlafsack 2,5 TOG"},
  {"item_6", "Schlafsack 1,0 TOG"},
  {"item_7", "Kurzarmbody"},
  {"item_8", "Schlafsack 0,5 TOG"},
  {"item_9", "Hose"},
  {"item_10", "Strickjacke"},
  {"item_11", "dicke Socken"},
  {"item_12", "dünne Decke"},
  {"item_13", "Pullover"},
  {"item_14", "dünner Pullover"},
  {"item_15", "dünne Socken"},
  {"item_16", "Langarmshirt"},
  {"item_17", "Shirt"},
  {"item_18", "Barfuß"},
  {"item_19", "dünne Hose"},
  {"item_20", "Strumpfhose"},
  {"item_21", "Wollwalk"},
  {"item_22", "Handschuhe"},
  {"item_23", "Wintermütze"},
  {"item_24", "Füßlinge"},
  {"item_25", "Jacke"},
  {"item_26", "Lammfell"},
  {"item_27", "Leggings"},
  {"item_28", "Tragejacke/-cover"},
  {"item_29", "Halstuch"},
  {"item_30", "dicke Strumpfhose"},
  {"item_31", "dünne Handschuhe"},
  {"item_32", "dicke Hose"},
  {"item_33", "Hose"},
  {"item_34", "Pullover"},
  {"item_35", "Mütze"},
  {"item_36", "dünne Mütze"},
  {"item_37", "Sonnenhut"},
  {"item_38", "Regenschutz"},
  {"item_39", "Fußsack Auto"},
  {"item_40", "Einschlagdecke"},
  {"item_41", "dünnes Häubchen"},
};
static constexpr size_t kCatalogCount = sizeof(kCatalog) / sizeof(kCatalog[0]);

static const char *item_label(const tessera::Plugin *plugin, uint8_t id) {
  if (id >= kCatalogCount) return "";
  const char *translated = plugin ? plugin->text(kCatalog[id].key) : "";
  return (translated && translated[0]) ? translated : kCatalog[id].fallback_de;
}

static void set_items(OutfitResult &out, const uint8_t *items, size_t count) {
  out.count = static_cast<uint8_t>(std::min(count, sizeof(out.items)));
  for (uint8_t i = 0; i < out.count; ++i) out.items[i] = items[i];
}

// ---- GarmentIcon: Local LVGL vector clothing illustrations ----

struct GarmentIcon {
  lv_obj_t *box = nullptr;
  lv_obj_t *parts[6]{};
  lv_obj_t *badge = nullptr;
  int size = 0;

  void build(lv_obj_t *parent, int x, int y, int side) {
    size = side;
    box = ui::block(parent, theme::CARD);
    lv_obj_set_size(box, side, side);
    lv_obj_set_pos(box, x, y);
    lv_obj_set_style_bg_opa(box, LV_OPA_TRANSP, 0);
    for (int i = 0; i < 6; ++i) {
      parts[i] = ui::block(box, theme::ACCENT);
      lv_obj_add_flag(parts[i], LV_OBJ_FLAG_HIDDEN);
    }
    badge = ui::label(box, Font::BODY, theme::ON_ACCENT);
    lv_obj_set_style_text_align(badge, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_add_flag(badge, LV_OBJ_FLAG_HIDDEN);
  }

  void set_bounds(int x, int y, int side) {
    size = side;
    if (box) {
      lv_obj_set_size(box, side, side);
      lv_obj_set_pos(box, x, y);
    }
  }

  void hide() {
    if (box) lv_obj_add_flag(box, LV_OBJ_FLAG_HIDDEN);
  }

  void set_part(int idx, int px, int py, int pw, int ph, int radius, tessera::Tone tone, theme::Role fallback_role) {
    if (idx < 0 || idx >= 6 || !parts[idx]) return;
    const int x = (px * size) / 100;
    const int y = (py * size) / 100;
    const int w = std::max(2, (pw * size) / 100);
    const int h = std::max(2, (ph * size) / 100);
    const int r = (radius * size) / 100;
    lv_obj_set_pos(parts[idx], x, y);
    lv_obj_set_size(parts[idx], w, h);
    lv_obj_set_style_radius(parts[idx], r, 0);
    ui::set_fill(parts[idx], tone, fallback_role);
    lv_obj_clear_flag(parts[idx], LV_OBJ_FLAG_HIDDEN);
  }

  void set_badge(const char *txt, int py) {
    if (!badge) return;
    if (!txt || !txt[0]) {
      lv_obj_add_flag(badge, LV_OBJ_FLAG_HIDDEN);
      return;
    }
    const int lh = ui::line_height(Font::BODY);
    lv_obj_set_size(badge, size, lh);
    lv_obj_set_pos(badge, 0, (py * size) / 100);
    ui::set_color(badge, theme::ON_ACCENT);
    ui::set_text(badge, txt);
    lv_obj_clear_flag(badge, LV_OBJ_FLAG_HIDDEN);
  }

  void draw(uint8_t id) {
    if (!box) return;
    lv_obj_clear_flag(box, LV_OBJ_FLAG_HIDDEN);
    for (int i = 0; i < 6; ++i) {
      if (parts[i]) lv_obj_add_flag(parts[i], LV_OBJ_FLAG_HIDDEN);
    }
    if (badge) lv_obj_add_flag(badge, LV_OBJ_FLAG_HIDDEN);

    const auto acc = tessera::Tone::ACCENT;
    const auto good = tessera::Tone::GOOD;
    const auto warn = tessera::Tone::BUSY;
    const auto norm = tessera::Tone::NORMAL;

    switch (id) {
      case 0:  // Windel
        set_part(0, 20, 26, 60, 16, 6, norm, theme::MUTED);
        set_part(1, 24, 36, 52, 30, 12, norm, theme::MUTED);
        set_part(2, 34, 52, 32, 22, 10, norm, theme::MUTED);
        set_part(3, 22, 30, 14, 7, 3, acc, theme::ACCENT);
        set_part(4, 64, 30, 14, 7, 3, acc, theme::ACCENT);
        break;

      case 1:  // Langarmbody
        set_part(0, 6, 20, 18, 44, 6, acc, theme::ACCENT);
        set_part(1, 76, 20, 18, 44, 6, acc, theme::ACCENT);
        set_part(2, 28, 16, 44, 50, 8, acc, theme::ACCENT);
        set_part(3, 35, 62, 30, 18, 9, acc, theme::ACCENT);
        set_part(4, 38, 12, 24, 10, 5, norm, theme::TRACK);
        set_part(5, 43, 72, 14, 5, 2, norm, theme::TRACK);
        break;

      case 7:  // Kurzarmbody
        set_part(0, 8, 20, 16, 22, 5, acc, theme::ACCENT);
        set_part(1, 76, 20, 16, 22, 5, acc, theme::ACCENT);
        set_part(2, 28, 16, 44, 50, 8, acc, theme::ACCENT);
        set_part(3, 35, 62, 30, 18, 9, acc, theme::ACCENT);
        set_part(4, 38, 12, 24, 10, 5, norm, theme::TRACK);
        set_part(5, 43, 72, 14, 5, 2, norm, theme::TRACK);
        break;

      case 3:   // Schlafanzug
      case 21:  // Wollwalk
      case 28:  // Tragejacke/-cover
      {
        const auto t = (id == 21) ? warn : (id == 28 ? good : acc);
        set_part(0, 6, 20, 18, 38, 6, t, theme::ACCENT);
        set_part(1, 76, 20, 18, 38, 6, t, theme::ACCENT);
        set_part(2, 28, 16, 44, 42, 8, t, theme::ACCENT);
        set_part(3, 28, 52, 19, 34, 6, t, theme::ACCENT);
        set_part(4, 53, 52, 19, 34, 6, t, theme::ACCENT);
        if (id == 21 || id == 28) {
          set_part(5, 48, 24, 4, 32, 2, norm, theme::TRACK);
        }
        break;
      }

      case 4:  // Schlafsack 3-3,5 TOG
      case 5:  // Schlafsack 2,5 TOG
      case 6:  // Schlafsack 1,0 TOG
      case 8:  // Schlafsack 0,5 TOG
      case 39: // Fußsack Auto
      {
        const auto t = (id == 4 || id == 5) ? warn : (id == 39 ? good : acc);
        set_part(0, 28, 14, 14, 18, 5, t, theme::ACCENT);
        set_part(1, 58, 14, 14, 18, 5, t, theme::ACCENT);
        set_part(2, 24, 26, 52, 34, 10, t, theme::ACCENT);
        set_part(3, 18, 44, 64, 42, 20, t, theme::ACCENT);
        if (id == 4) set_badge("3.5", 52);
        else if (id == 5) set_badge("2.5", 52);
        else if (id == 6) set_badge("1.0", 52);
        else if (id == 8) set_badge("0.5", 52);
        break;
      }

      case 9:   // Hose
      case 19:  // dünne Hose
      case 27:  // Leggings
      case 32:  // dicke Hose
      case 33:  // Hose
      {
        const auto t = (id == 32 || id == 9) ? acc : good;
        set_part(0, 24, 16, 52, 22, 6, t, theme::ACCENT);
        set_part(1, 24, 32, 22, 52, 6, t, theme::ACCENT);
        set_part(2, 54, 32, 22, 52, 6, t, theme::ACCENT);
        set_part(3, 24, 16, 52, 8, 3, norm, theme::MUTED);
        break;
      }

      case 20:  // Strumpfhose
      case 30:  // dicke Strumpfhose
      {
        const auto t = (id == 30) ? warn : acc;
        set_part(0, 24, 14, 52, 20, 6, t, theme::ACCENT);
        set_part(1, 24, 30, 20, 48, 6, t, theme::ACCENT);
        set_part(2, 56, 30, 20, 48, 6, t, theme::ACCENT);
        set_part(3, 14, 70, 28, 14, 6, t, theme::ACCENT);
        set_part(4, 58, 70, 28, 14, 6, t, theme::ACCENT);
        break;
      }

      case 10:  // Strickjacke
      case 25:  // Jacke
      {
        const auto t = (id == 25) ? warn : good;
        set_part(0, 6, 22, 18, 46, 6, t, theme::ACCENT);
        set_part(1, 76, 22, 18, 46, 6, t, theme::ACCENT);
        set_part(2, 28, 18, 44, 56, 8, t, theme::ACCENT);
        set_part(3, 47, 18, 6, 56, 2, norm, theme::TRACK);
        break;
      }

      case 13:  // Pullover
      case 14:  // dünner Pullover
      case 16:  // Langarmshirt
      case 34:  // Pullover
      {
        const auto t = (id == 13 || id == 34) ? warn : acc;
        set_part(0, 6, 22, 18, 44, 6, t, theme::ACCENT);
        set_part(1, 76, 22, 18, 44, 6, t, theme::ACCENT);
        set_part(2, 28, 18, 44, 52, 8, t, theme::ACCENT);
        set_part(3, 38, 14, 24, 10, 5, norm, theme::TRACK);
        set_part(4, 28, 64, 44, 8, 3, norm, theme::MUTED);
        break;
      }

      case 17:  // Shirt (Kurzarm)
        set_part(0, 8, 22, 16, 22, 5, good, theme::ACCENT);
        set_part(1, 76, 22, 16, 22, 5, good, theme::ACCENT);
        set_part(2, 28, 18, 44, 54, 8, good, theme::ACCENT);
        set_part(3, 38, 14, 24, 10, 5, norm, theme::TRACK);
        break;

      case 2:   // Socken
      case 11:  // dicke Socken
      case 15:  // dünne Socken
      {
        const auto t = (id == 11) ? warn : acc;
        set_part(0, 22, 20, 20, 46, 6, t, theme::ACCENT);
        set_part(1, 12, 54, 30, 18, 8, t, theme::ACCENT);
        set_part(2, 58, 20, 20, 46, 6, t, theme::ACCENT);
        set_part(3, 58, 54, 30, 18, 8, t, theme::ACCENT);
        break;
      }

      case 24:  // Füßlinge (Booties)
        set_part(0, 22, 24, 20, 32, 6, warn, theme::ACCENT);
        set_part(1, 10, 46, 34, 26, 12, warn, theme::ACCENT);
        set_part(2, 20, 20, 24, 10, 4, norm, theme::MUTED);
        set_part(3, 58, 24, 20, 32, 6, warn, theme::ACCENT);
        set_part(4, 56, 46, 34, 26, 12, warn, theme::ACCENT);
        set_part(5, 56, 20, 24, 10, 4, norm, theme::MUTED);
        break;

      case 18:  // Barfuß
        set_part(0, 24, 30, 20, 44, 10, good, theme::ACCENT);
        set_part(1, 56, 30, 20, 44, 10, good, theme::ACCENT);
        set_part(2, 24, 20, 8, 8, 4, good, theme::ACCENT);
        set_part(3, 35, 20, 8, 8, 4, good, theme::ACCENT);
        set_part(4, 57, 20, 8, 8, 4, good, theme::ACCENT);
        set_part(5, 68, 20, 8, 8, 4, good, theme::ACCENT);
        break;

      case 22:  // Handschuhe
      case 31:  // dünne Handschuhe
      {
        const auto t = (id == 22) ? warn : acc;
        set_part(0, 10, 20, 26, 42, 13, t, theme::ACCENT);
        set_part(1, 32, 34, 10, 16, 5, t, theme::ACCENT);
        set_part(2, 12, 62, 22, 10, 3, norm, theme::MUTED);
        set_part(3, 64, 20, 26, 42, 13, t, theme::ACCENT);
        set_part(4, 58, 34, 10, 16, 5, t, theme::ACCENT);
        set_part(5, 66, 62, 22, 10, 3, norm, theme::MUTED);
        break;
      }

      case 23:  // Wintermütze
      case 35:  // Mütze
      case 36:  // dünne Mütze
      case 41:  // dünnes Häubchen
      {
        const auto t = (id == 23 || id == 35) ? warn : acc;
        set_part(0, 20, 28, 60, 44, 24, t, theme::ACCENT);
        set_part(1, 16, 62, 68, 14, 6, norm, theme::MUTED);
        if (id == 23 || id == 35) {
          set_part(2, 41, 12, 18, 18, 9, t, theme::ACCENT);
        }
        break;
      }

      case 37:  // Sonnenhut
        set_part(0, 24, 26, 52, 34, 18, warn, theme::ACCENT);
        set_part(1, 10, 54, 80, 14, 7, warn, theme::ACCENT);
        break;

      case 29:  // Halstuch
        set_part(0, 16, 24, 68, 18, 6, warn, theme::ACCENT);
        set_part(1, 26, 38, 48, 22, 8, warn, theme::ACCENT);
        set_part(2, 38, 54, 24, 22, 8, warn, theme::ACCENT);
        break;

      case 12:  // dünne Decke
      case 26:  // Lammfell
      case 40:  // Einschlagdecke
      {
        const auto t = (id == 26) ? warn : good;
        set_part(0, 18, 18, 64, 64, 12, t, theme::ACCENT);
        set_part(1, 26, 26, 48, 48, 8, norm, theme::CARD);
        set_part(2, 32, 32, 36, 36, 6, t, theme::ACCENT);
        break;
      }

      case 38:  // Regenschutz
      default:
        set_part(0, 14, 26, 72, 34, 30, acc, theme::ACCENT);
        set_part(1, 47, 18, 6, 56, 3, norm, theme::MUTED);
        set_part(2, 47, 68, 16, 10, 4, norm, theme::MUTED);
        break;
    }
  }
};

// Estimate indoor room temperature from outdoor temperature (baby-wetter.de CalculateInsideTemperature default Neubau/Mittel)
static int estimate_inside_temperature(float temp_outside) {
  const float factor = temp_outside >= 20.0f ? (0.15f * 1.5f) : 0.15f;
  const float factor_sun = 0.15f;
  float room = 20.0f + (temp_outside - 20.0f) * factor;
  float sun_effect = std::max(0.0f, (temp_outside - 10.0f) * 0.075f) * (1.0f + factor_sun);
  return static_cast<int>(std::lround(room + sun_effect));
}

// Parse birthdate in "DD.MM.YYYY" or "YYYY-MM-DD" format and calculate baby's age in months.
// baby-wetter.de categories: 0-3 months ('neugeborenes' -> is_older_baby = false), 4+ months ('baby' -> is_older_baby = true).
static AgeInfo calculate_age(const char *birthdate_str, uint32_t epoch) {
  AgeInfo info{};
  if (!birthdate_str || !birthdate_str[0]) return info;
  unsigned a = 0, b = 0, c = 0;
  char sep1 = 0, sep2 = 0;
  if (std::sscanf(birthdate_str, "%u%c%u%c%u", &a, &sep1, &b, &sep2, &c) != 5) return info;

  int by = 0, bm = 0, bd = 0;
  if (a >= 1900 && sep1 == '-' && sep2 == '-') {
    by = static_cast<int>(a);
    bm = static_cast<int>(b);
    bd = static_cast<int>(c);
  } else if (c >= 1900 && (sep1 == '.' || sep1 == '/' || sep1 == '-')) {
    bd = static_cast<int>(a);
    bm = static_cast<int>(b);
    by = static_cast<int>(c);
  } else {
    return info;
  }
  if (bm < 1 || bm > 12 || bd < 1 || bd > 31) return info;
  info.valid = true;

  if (!epoch) epoch = tessera::epoch();
  if (!epoch) {
    info.months = 0;
    info.is_older_baby = false;
    return info;
  }

  const tessera::LocalTime now = tessera::local_time(epoch);
  int months = (now.year - by) * 12 + (now.month - bm);
  if (now.day < bd) --months;
  if (months < 0) months = 0;
  info.months = months;
  info.is_older_baby = (months >= 4);
  return info;
}

static void evaluate_sleep(int t, OutfitResult &out) {
  if (t <= 15) {
    static constexpr uint8_t kItems[] = {0, 1, 2, 3, 4};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<=17 && t>=16) {
    static constexpr uint8_t kItems[] = {0, 1, 2, 3, 5};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<=19 && t>=18) {
    static constexpr uint8_t kItems[] = {0, 1, 3, 6};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<=21 && t>=20) {
    static constexpr uint8_t kItems[] = {0, 7, 3, 6};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<=23 && t>=22) {
    static constexpr uint8_t kItems[] = {0, 3, 8};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<=25 && t>=24) {
    static constexpr uint8_t kItems[] = {0, 7, 8};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<=28 && t>=26) {
    static constexpr uint8_t kItems[] = {0, 7};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>28) {
    static constexpr uint8_t kItems[] = {0};
    set_items(out, kItems, sizeof(kItems));
  }
}

static void evaluate_inside(int t, bool older, OutfitResult &out) {
  if (t <= 15 && !older) {
    static constexpr uint8_t kItems[] = {0, 1, 9, 10, 11, 12};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t <= 15 && older) {
    static constexpr uint8_t kItems[] = {0, 1, 9, 10, 11};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<=17 && t>=16 && !older) {
    static constexpr uint8_t kItems[] = {0, 1, 9, 13, 11, 12};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<=17 && t>=16 && older) {
    static constexpr uint8_t kItems[] = {0, 1, 9, 13, 11};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<=19 && t>=18 && !older) {
    static constexpr uint8_t kItems[] = {0, 1, 9, 14, 15};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<=19 && t>=18 && older) {
    static constexpr uint8_t kItems[] = {0, 1, 9, 16, 15};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<=21 && t>=20 && !older) {
    static constexpr uint8_t kItems[] = {0, 1, 9, 16, 15};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<=21 && t>=20 && older) {
    static constexpr uint8_t kItems[] = {0, 1, 9, 17, 18};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<=23 && t>=22) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 18};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<=25 && t>=24) {
    static constexpr uint8_t kItems[] = {0, 7, 19};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<=28 && t>=26) {
    static constexpr uint8_t kItems[] = {0, 7};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>28) {
    static constexpr uint8_t kItems[] = {0};
    set_items(out, kItems, sizeof(kItems));
  }
}

static void evaluate_outdoor(int t, bool older, Situation sit, bool rain, OutfitResult &out) {
  if (t<-3 && !older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 16, 10, 2, 21, 22, 23, 24};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<-3 && !older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 16, 10, 2, 25, 22, 23, 26, 24};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<-3 && !older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 27, 9, 16, 2, 21, 22, 23, 28, 24};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<-3 && older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 16, 13, 2, 21, 22, 23, 24, 29};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<-3 && older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 16, 13, 2, 25, 22, 23, 26, 29, 24};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<-3 && older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 27, 9, 16, 2, 21, 22, 23, 29, 28, 24};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<3 && t>=-3 && !older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 10, 2, 21, 22, 23, 24};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<3 && t>=-3 && !older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 10, 11, 25, 22, 23, 26};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<3 && t>=-3 && !older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 27, 9, 10, 2, 22, 23, 28, 24};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<3 && t>=-3 && older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 13, 2, 21, 22, 23, 24, 29};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<3 && t>=-3 && older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 13, 11, 25, 22, 23, 26, 29};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<3 && t>=-3 && older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 27, 9, 13, 2, 22, 23, 29, 28, 24};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<10 && t>=3 && !older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 30, 13, 2, 21, 31, 23, 24};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<10 && t>=3 && !older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 30, 13, 11, 25, 31, 23, 26};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<10 && t>=3 && !older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 32, 13, 11, 23, 28};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<10 && t>=3 && older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 34, 2, 21, 31, 23, 24, 29};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<10 && t>=3 && older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 34, 11, 25, 31, 23, 26, 29};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<10 && t>=3 && older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 34, 11, 23, 28};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<15 && t>=10 && !older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 14, 11, 21, 35};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<15 && t>=10 && !older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 14, 11, 10, 35, 26};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<15 && t>=10 && !older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 9, 14, 11, 35, 28};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<15 && t>=10 && older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 16, 11, 21, 35};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<15 && t>=10 && older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 16, 2, 10, 35, 26};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<15 && t>=10 && older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 16, 2, 35, 28};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<18 && t>=15 && !older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 14, 11, 10, 36};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<18 && t>=15 && !older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 14, 11, 10, 36, 12};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<18 && t>=15 && !older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 14, 11, 36};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<18 && t>=15 && older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 16, 11, 14, 36};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<18 && t>=15 && older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 16, 11, 14, 36, 12};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<18 && t>=15 && older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 16, 11, 36};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<22 && t>=18 && !older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 14, 15, 36};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<22 && t>=18 && !older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 14, 15, 36, 12};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<22 && t>=18 && !older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 15, 36};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<22 && t>=18 && older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 16, 15, 36};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<22 && t>=18 && older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 16, 15, 36, 12};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<22 && t>=18 && older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 15, 36};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<25 && t>=22 && !older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 14, 15, 37};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<25 && t>=22 && !older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 14, 15, 37, 12};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<25 && t>=22 && !older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15, 37};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<25 && t>=22 && older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 16, 15, 37};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<25 && t>=22 && older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 16, 15};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<25 && t>=22 && older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15, 37};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>=25 && !older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15, 37};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>=25 && !older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15, 37};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>=25 && !older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15, 37};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>=25 && older && sit == Situation::ALLGEMEIN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15, 37};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>=25 && older && sit == Situation::KINDERWAGEN && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>=25 && older && sit == Situation::BABYTRAGE && !rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15, 37};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<-3 && !older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 16, 10, 2, 21, 22, 23, 24, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<-3 && !older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 16, 10, 2, 25, 22, 23, 26, 24, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<-3 && !older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 27, 9, 16, 2, 21, 22, 23, 28, 24, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<-3 && older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 16, 13, 2, 21, 22, 23, 24, 29, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<-3 && older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 16, 13, 2, 25, 22, 23, 26, 29, 24, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<-3 && older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 27, 9, 16, 2, 21, 22, 23, 29, 28, 24, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<3 && t>=-3 && !older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 10, 2, 21, 22, 23, 24, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<3 && t>=-3 && !older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 10, 11, 25, 22, 23, 26, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<3 && t>=-3 && !older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 27, 9, 10, 2, 22, 23, 28, 24, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<3 && t>=-3 && older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 13, 2, 21, 22, 23, 24, 29, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<3 && t>=-3 && older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 13, 11, 25, 22, 23, 26, 29, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<3 && t>=-3 && older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 27, 9, 13, 2, 22, 23, 29, 28, 24, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<10 && t>=3 && !older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 30, 13, 2, 21, 31, 23, 24, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<10 && t>=3 && !older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 30, 13, 11, 25, 31, 23, 26, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<10 && t>=3 && !older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 32, 13, 11, 23, 28, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<10 && t>=3 && older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 34, 2, 21, 31, 23, 24, 29, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<10 && t>=3 && older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 34, 11, 25, 31, 23, 26, 29, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<10 && t>=3 && older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 34, 11, 23, 28, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<15 && t>=10 && !older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 14, 11, 21, 35, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<15 && t>=10 && !older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 14, 11, 10, 35, 26, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<15 && t>=10 && !older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 9, 14, 11, 35, 28, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<15 && t>=10 && older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 16, 11, 21, 35, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<15 && t>=10 && older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 16, 2, 10, 35, 26, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<15 && t>=10 && older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 16, 2, 35, 28, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<18 && t>=15 && !older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 14, 11, 10, 36, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<18 && t>=15 && !older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 14, 11, 10, 36, 12, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<18 && t>=15 && !older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 14, 11, 36, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<18 && t>=15 && older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 16, 11, 14, 36, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<18 && t>=15 && older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 16, 11, 14, 36, 12, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<18 && t>=15 && older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 16, 11, 36, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<22 && t>=18 && !older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 14, 15, 36, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<22 && t>=18 && !older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 14, 15, 36, 12, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<22 && t>=18 && !older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 15, 36, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<22 && t>=18 && older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 16, 15, 36, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<22 && t>=18 && older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 16, 15, 36, 12, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<22 && t>=18 && older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 15, 36, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<25 && t>=22 && !older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 14, 15, 37, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<25 && t>=22 && !older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 14, 15, 37, 12, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<25 && t>=22 && !older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15, 37, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<25 && t>=22 && older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 16, 15, 37, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<25 && t>=22 && older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 16, 15, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<25 && t>=22 && older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15, 37, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>=25 && !older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15, 37, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>=25 && !older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15, 37, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>=25 && !older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15, 37, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>=25 && older && sit == Situation::ALLGEMEIN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15, 37, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>=25 && older && sit == Situation::KINDERWAGEN && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>=25 && older && sit == Situation::BABYTRAGE && rain) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15, 37, 38};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<-3 && !older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 16, 10, 2, 23, 39, 24};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<-3 && older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 16, 13, 2, 23, 29, 39, 24};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<3 && t>=-3 && !older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 10, 2, 23, 39};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<3 && t>=-3 && older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 9, 13, 2, 23, 39};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<10 && t>=3 && !older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 1, 30, 13, 2, 35, 40};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<10 && t>=3 && older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 34, 2, 35, 40};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<15 && t>=10 && !older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 1, 20, 14, 11, 41, 40};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<15 && t>=10 && older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 16, 11, 40};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<18 && t>=15 && !older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 14, 11, 12};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<18 && t>=15 && older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 16, 11, 12};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<22 && t>=18 && !older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 14, 15};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<22 && t>=18 && older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 1, 33, 16, 15};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<25 && t>=22 && !older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<25 && t>=22 && older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 1, 19, 15};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<28 && t>=25 && !older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 7, 19};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t<28 && t>=25 && older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 7, 19};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>=28 && !older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 7};
    set_items(out, kItems, sizeof(kItems));
  }
  else if (t>=28 && older && sit == Situation::AUTO) {
    static constexpr uint8_t kItems[] = {0, 7};
    set_items(out, kItems, sizeof(kItems));
  }
}

static float parse_sensor_state(JsonObjectConst entity_obj) {
  if (entity_obj.isNull()) return NAN;
  JsonObjectConst attrs = entity_obj["attributes"];
  if (!attrs.isNull()) {
    JsonVariantConst curr = attrs["current_temperature"];
    if (!curr.isNull()) {
      float v = curr.as<float>();
      if (!std::isnan(v)) return v;
    }
    JsonVariantConst temp = attrs["temperature"];
    if (!temp.isNull()) {
      float v = temp.as<float>();
      if (!std::isnan(v)) return v;
    }
  }
  const char *state = entity_obj["state"] | "";
  if (!state[0] || std::strcmp(state, "unavailable") == 0 || std::strcmp(state, "unknown") == 0) return NAN;
  char *end = nullptr;
  float v = std::strtof(state, &end);
  return (end && end != state) ? v : NAN;
}

static bool parse_rain_entity(JsonObjectConst entity_obj) {
  if (entity_obj.isNull()) return false;
  const char *state = entity_obj["state"] | "";
  if (std::strcmp(state, "rainy") == 0 || std::strcmp(state, "pouring") == 0 ||
      std::strcmp(state, "lightning-rainy") == 0 || std::strcmp(state, "hail") == 0) {
    return true;
  }
  JsonVariantConst prob = entity_obj["attributes"]["precipitation_probability"];
  if (!prob.isNull()) {
    return (prob.as<float>() >= 10.0f);
  }
  float val = parse_sensor_state(entity_obj);
  return !std::isnan(val) && val >= 10.0f;
}

// ---- OutfitCard: Full-width detail card with interactive Situation Dropdown ----

class OutfitCard : public tessera::Card {
 public:
  static constexpr int kMaxCardSlots = 12;

  explicit OutfitCard(BabyWetter *plugin) : plugin_(plugin) {}

  void open(const tessera::CardContext &c) override {
    parent_ = c.parent;
    width_ = c.width;
    height_ = c.height;
    dropdown_open_ = plugin_->consume_open_dropdown();

    const bool large = ui::large();
    pad_ = ui::px(large ? 10 : 6);
    header_h_ = ui::mm(8);
    drop_w_ = std::min(width_ / 2, ui::px(large ? 200 : 140));
    drop_x_ = width_ - drop_w_;
    drop_y_ = 0;

    // Header info (Temperature + Age) on the left
    info_label_ = ui::label(parent_, large ? Font::TITLE : Font::BODY_LARGE, theme::INK);
    lv_obj_set_size(info_label_, std::max(1, drop_x_ - pad_), ui::line_height(large ? Font::TITLE : Font::BODY_LARGE));
    lv_obj_set_pos(info_label_, 0, (header_h_ - ui::line_height(large ? Font::TITLE : Font::BODY_LARGE)) / 2);

    // Interactive Situation Dropdown Button on the top right
    drop_btn_ = ui::block(parent_, theme::BUTTON);
    lv_obj_set_size(drop_btn_, drop_w_, header_h_);
    lv_obj_set_pos(drop_btn_, drop_x_, drop_y_);

    drop_btn_label_ = ui::label(drop_btn_, Font::BODY_LARGE, theme::INK);
    const int chev_w = ui::px(large ? 24 : 18);
    lv_obj_set_size(drop_btn_label_, std::max(1, drop_w_ - chev_w - pad_ * 2), ui::line_height(Font::BODY_LARGE));
    lv_obj_set_pos(drop_btn_label_, pad_, (header_h_ - ui::line_height(Font::BODY_LARGE)) / 2);

    drop_btn_icon_ = ui::label(drop_btn_, Font::ICON_SMALL, theme::MUTED);
    lv_obj_set_size(drop_btn_icon_, chev_w, ui::line_height(Font::ICON_SMALL));
    lv_obj_set_pos(drop_btn_icon_, drop_w_ - chev_w - pad_ / 2, (header_h_ - ui::line_height(Font::ICON_SMALL)) / 2);

    // Up to 12 Clothing item slots (dynamically arranged as 4x2, 5x2, or 4x3) below header
    grid_top_ = header_h_ + pad_;
    pager_h_ = ui::mm(7);

    for (int i = 0; i < kMaxCardSlots; ++i) {
      slot_bg_[i] = ui::block(parent_, theme::CARD);
      slot_icon_[i].build(parent_, 0, 0, ui::px(32));
      slot_cap_[i] = ui::text(parent_, Font::BODY, theme::INK);
      lv_obj_set_style_text_align(slot_cap_[i], LV_TEXT_ALIGN_CENTER, 0);
    }

    // Pager bar at the bottom when more than 12 items exist
    pager_y_ = height_ - pager_h_;
    prev_btn_ = ui::block(parent_, theme::BUTTON);
    lv_obj_set_size(prev_btn_, ui::mm(10), pager_h_);
    lv_obj_set_pos(prev_btn_, 0, pager_y_);
    prev_lbl_ = ui::label(prev_btn_, Font::BODY_LARGE, theme::INK);
    lv_obj_set_size(prev_lbl_, ui::mm(10), ui::line_height(Font::BODY_LARGE));
    lv_obj_set_style_text_align(prev_lbl_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(prev_lbl_, 0, (pager_h_ - ui::line_height(Font::BODY_LARGE)) / 2);
    ui::set_text(prev_lbl_, "<");

    next_btn_ = ui::block(parent_, theme::BUTTON);
    lv_obj_set_size(next_btn_, ui::mm(10), pager_h_);
    lv_obj_set_pos(next_btn_, width_ - ui::mm(10), pager_y_);
    next_lbl_ = ui::label(next_btn_, Font::BODY_LARGE, theme::INK);
    lv_obj_set_size(next_lbl_, ui::mm(10), ui::line_height(Font::BODY_LARGE));
    lv_obj_set_style_text_align(next_lbl_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(next_lbl_, 0, (pager_h_ - ui::line_height(Font::BODY_LARGE)) / 2);
    ui::set_text(next_lbl_, ">");

    page_lbl_ = ui::label(parent_, Font::BODY, theme::MUTED);
    lv_obj_set_size(page_lbl_, width_ - ui::mm(22), ui::line_height(Font::BODY));
    lv_obj_set_style_text_align(page_lbl_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(page_lbl_, ui::mm(11), pager_y_ + (pager_h_ - ui::line_height(Font::BODY)) / 2);

    // Dropdown overlay menu (6 situations), created last so it sits on top of the grid
    drop_row_h_ = std::min(ui::mm(7), (height_ - header_h_ - ui::px(4)) / 6);
    const int menu_h = drop_row_h_ * 6 + ui::px(4);
    drop_menu_ = ui::block(parent_, theme::RAISED);
    lv_obj_set_size(drop_menu_, drop_w_, menu_h);
    lv_obj_set_pos(drop_menu_, drop_x_, header_h_ + ui::px(2));

    for (int i = 0; i < 6; ++i) {
      drop_item_bg_[i] = ui::block(drop_menu_, theme::TRACK);
      lv_obj_set_size(drop_item_bg_[i], drop_w_ - ui::px(4), drop_row_h_ - ui::px(2));
      lv_obj_set_pos(drop_item_bg_[i], ui::px(2), ui::px(2) + i * drop_row_h_);

      drop_item_lbl_[i] = ui::label(drop_item_bg_[i], Font::BODY_LARGE, theme::INK);
      lv_obj_set_size(drop_item_lbl_[i], drop_w_ - ui::px(16), ui::line_height(Font::BODY_LARGE));
      lv_obj_set_pos(drop_item_lbl_[i], ui::px(8), (drop_row_h_ - ui::px(2) - ui::line_height(Font::BODY_LARGE)) / 2);
      ui::set_text(drop_item_lbl_[i], plugin_->situation_label(static_cast<Situation>(i)));
    }

    render(tessera::epoch());
  }

  void on_tick(uint32_t epoch) override { render(epoch); }

  void on_theme() override { render(tessera::epoch()); }

  bool on_back() override {
    if (dropdown_open_) {
      dropdown_open_ = false;
      render(tessera::epoch());
      return true;
    }
    return false;
  }

  void on_tap() override {
    const tessera::Point pt = tessera::tap_point();
    if (pt.x < 0 || pt.y < 0) return;

    // Check tap on dropdown button
    if (pt.x >= drop_x_ && pt.x <= drop_x_ + drop_w_ && pt.y >= drop_y_ && pt.y <= drop_y_ + header_h_) {
      dropdown_open_ = !dropdown_open_;
      render(tessera::epoch());
      return;
    }

    // When dropdown menu is open, check selection or close it
    if (dropdown_open_) {
      const int menu_top = header_h_ + ui::px(2);
      const int menu_bottom = menu_top + drop_row_h_ * 6 + ui::px(4);
      if (pt.x >= drop_x_ && pt.x <= drop_x_ + drop_w_ && pt.y >= menu_top && pt.y <= menu_bottom) {
        int idx = (pt.y - menu_top - ui::px(2)) / std::max(1, drop_row_h_);
        if (idx >= 0 && idx < 6) {
          plugin_->set_situation_index(idx);
          page_ = 0;
        }
      }
      dropdown_open_ = false;
      render(tessera::epoch());
      return;
    }

    // Check pager buttons if visible
    if (visible_items_ > page_size_ && pt.y >= pager_y_) {
      if (pt.x <= ui::mm(12) && page_ > 0) {
        --page_;
        render(tessera::epoch());
      } else if (pt.x >= width_ - ui::mm(12) && (page_ + 1) * page_size_ < visible_items_) {
        ++page_;
        render(tessera::epoch());
      }
    }
  }

 private:
  void update_grid_layout(int cols, int rows, bool show_pager) {
    if (cols == cur_cols_ && rows == cur_rows_ && show_pager == cur_pager_) return;
    cur_cols_ = cols;
    cur_rows_ = rows;
    cur_pager_ = show_pager;
    page_size_ = std::min(kMaxCardSlots, cols * rows);

    const int bottom_reserve = show_pager ? pager_h_ : 0;
    const int grid_h = std::max(1, height_ - grid_top_ - bottom_reserve);
    const int cell_w = (width_ - pad_ * (cols - 1)) / cols;
    const int cell_h = (grid_h - pad_ * (rows - 1)) / rows;
    const int lh = ui::line_height(Font::BODY);
    const int cap_h = (cell_h >= lh * 3 + ui::px(12)) ? (lh * 2) : lh;
    const int pic_side = std::max(1, std::min(cell_w - ui::px(4), cell_h - cap_h - ui::px(4)));

    for (int i = 0; i < page_size_; ++i) {
      const int col = i % cols;
      const int row = i / cols;
      const int cx = col * (cell_w + pad_);
      const int cy = grid_top_ + row * (cell_h + pad_);

      lv_obj_set_size(slot_bg_[i], cell_w, cell_h);
      lv_obj_set_pos(slot_bg_[i], cx, cy);

      slot_icon_[i].set_bounds(cx + (cell_w - pic_side) / 2, cy + ui::px(2), pic_side);

      lv_obj_set_size(slot_cap_[i], cell_w - ui::px(4), cap_h);
      lv_obj_set_pos(slot_cap_[i], cx + ui::px(2), cy + cell_h - cap_h - ui::px(2));
    }
    for (int i = page_size_; i < kMaxCardSlots; ++i) {
      lv_obj_add_flag(slot_bg_[i], LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(slot_cap_[i], LV_OBJ_FLAG_HIDDEN);
      slot_icon_[i].hide();
    }
  }

  void render(uint32_t epoch) {
    const OutfitResult res = plugin_->evaluate_for_card(epoch);
    // Omit Windel (item 0) when more than 8 items exist so the 8 outer garments fit in the large 4x2 grid
    const int offset = (res.count > 8 && res.items[0] == 0) ? 1 : 0;
    visible_items_ = std::max(0, static_cast<int>(res.count) - offset);

    int cols = 4;
    int rows = 2;
    if (visible_items_ > 10) {
      cols = 4;
      rows = 3;
    } else if (visible_items_ > 8) {
      cols = 5;
      rows = 2;
    }
    const bool show_pager = (visible_items_ > cols * rows);
    update_grid_layout(cols, rows, show_pager);

    if (page_ * page_size_ >= visible_items_) page_ = 0;

    // Header text: Temperature + Age
    char hdr[96];
    const char *age_str = res.age.is_older_baby ? plugin_->text("age_baby") : plugin_->text("age_newborn");
    std::string months_txt = res.age.valid ? tessera::format(plugin_->text("months_short"), res.age.months) : std::string(age_str);
    if (res.has_temp) {
      const bool indoor = (res.situation == Situation::SCHLAFEN || res.situation == Situation::ZUHAUSE);
      const char *t_lbl = indoor ? (res.used_estimated_indoor ? plugin_->text("estimated_room") : plugin_->text("indoor_label"))
                                 : plugin_->text("outdoor_label");
      std::snprintf(hdr, sizeof(hdr), "%s: %.1f °C  ·  %s", t_lbl, res.display_temp, months_txt.c_str());
    } else {
      std::snprintf(hdr, sizeof(hdr), "%s  ·  %s", plugin_->text("no_temp"), months_txt.c_str());
    }
    ui::set_text(info_label_, hdr);

    // Dropdown button state
    ui::set_text(drop_btn_label_, plugin_->situation_label(res.situation));
    ui::set_text(drop_btn_icon_, dropdown_open_ ? "\U000F0143" : "\U000F0140");

    if (dropdown_open_) {
      lv_obj_clear_flag(drop_menu_, LV_OBJ_FLAG_HIDDEN);
      for (int i = 0; i < 6; ++i) {
        const bool active = (static_cast<int>(res.situation) == i);
        ui::set_fill(drop_item_bg_[i], active ? tessera::Tone::ACCENT : tessera::Tone::NORMAL, theme::TRACK);
        ui::set_color(drop_item_lbl_[i], active ? theme::ON_ACCENT : theme::INK);
      }
    } else {
      lv_obj_add_flag(drop_menu_, LV_OBJ_FLAG_HIDDEN);
    }

    // Populate the grid slots for the current page
    const int start = offset + page_ * page_size_;
    for (int i = 0; i < page_size_; ++i) {
      const int item_idx = start + i;
      if (item_idx < res.count) {
        const uint8_t id = res.items[item_idx];
        lv_obj_clear_flag(slot_bg_[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(slot_cap_[i], LV_OBJ_FLAG_HIDDEN);
        ui::set_text(slot_cap_[i], item_label(plugin_, id));
        if (dropdown_open_) {
          slot_icon_[i].hide();
        } else {
          slot_icon_[i].draw(id);
        }
      } else {
        lv_obj_add_flag(slot_bg_[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(slot_cap_[i], LV_OBJ_FLAG_HIDDEN);
        slot_icon_[i].hide();
      }
    }

    // Pager visibility
    if (show_pager && !dropdown_open_) {
      lv_obj_clear_flag(prev_btn_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(next_btn_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(page_lbl_, LV_OBJ_FLAG_HIDDEN);
      char pbuf[32];
      const int pages = (visible_items_ + page_size_ - 1) / page_size_;
      std::snprintf(pbuf, sizeof(pbuf), "%d / %d (%d)", page_ + 1, pages, visible_items_);
      ui::set_text(page_lbl_, pbuf);
    } else {
      lv_obj_add_flag(prev_btn_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(next_btn_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(page_lbl_, LV_OBJ_FLAG_HIDDEN);
    }
  }

  BabyWetter *plugin_;
  lv_obj_t *parent_{};
  lv_obj_t *info_label_{};
  lv_obj_t *drop_btn_{}, *drop_btn_label_{}, *drop_btn_icon_{};
  lv_obj_t *drop_menu_{};
  lv_obj_t *drop_item_bg_[6]{};
  lv_obj_t *drop_item_lbl_[6]{};
  lv_obj_t *slot_bg_[kMaxCardSlots]{};
  lv_obj_t *slot_cap_[kMaxCardSlots]{};
  GarmentIcon slot_icon_[kMaxCardSlots]{};
  lv_obj_t *prev_btn_{}, *prev_lbl_{}, *next_btn_{}, *next_lbl_{}, *page_lbl_{};

  int width_ = 0, height_ = 0, pad_ = 0, grid_top_ = 0;
  int header_h_ = 0, drop_x_ = 0, drop_y_ = 0, drop_w_ = 0, drop_row_h_ = 0;
  int pager_y_ = 0, pager_h_ = 0;
  int cur_cols_ = 0, cur_rows_ = 0, page_size_ = 8;
  bool cur_pager_ = false;
  int page_ = 0, visible_items_ = 0;
  bool dropdown_open_ = false;
};

// ---- OutfitTile: Responsive tile (1x1 up to full-page 2x3 / 4x4) with Situation dropdown pill & clothing items ----

class OutfitTile : public tessera::Tile {
 public:
  static constexpr int kMaxTileSlots = 12;

  explicit OutfitTile(BabyWetter *plugin) : plugin_(plugin) {}

  void create(const tessera::TileContext &c) override {
    width_ = c.width;
    height_ = c.height;
    cols_ = c.columns;
    rows_ = c.rows;
    tile_index_ = c.tile;
    name_ = c.name ? c.name : "";

    const char *sit_opt = c.options["situation"] | "follow";
    fixed_sit_ = (std::strcmp(sit_opt, "follow") != 0 && sit_opt[0] != '\0');
    tile_sit_ = BabyWetter::parse_situation_code(sit_opt, Situation::ALLGEMEIN);
    tile_birthdate_ = c.options["birthdate"] | "";
    plugin_->set_card_overrides(opt_out_temp_, opt_in_temp_, opt_rain_, tile_birthdate_.c_str(), tile_sit_, fixed_sit_);

    const bool large = ui::large();
    pad_ = ui::px(large ? 6 : 4);
    header_h_ = ui::line_height(Font::BODY_LARGE) + ui::px(large ? 8 : 4);

    // Situation Dropdown Pill on top right of tile
    pill_w_ = (cols_ == 1) ? width_ : std::min(width_ * 3 / 5, ui::px(large ? 150 : 105));
    pill_x_ = width_ - pill_w_;
    pill_bg_ = ui::block(c.parent, theme::TRACK);
    lv_obj_set_size(pill_bg_, pill_w_, header_h_);
    lv_obj_set_pos(pill_bg_, pill_x_, 0);

    pill_lbl_ = ui::label(pill_bg_, Font::BODY, theme::INK);
    const int chev_w = ui::px(large ? 18 : 14);
    lv_obj_set_size(pill_lbl_, std::max(1, pill_w_ - chev_w - pad_ * 2), ui::line_height(Font::BODY));
    lv_obj_set_pos(pill_lbl_, pad_, (header_h_ - ui::line_height(Font::BODY)) / 2);

    pill_icon_ = ui::label(pill_bg_, Font::ICON_SMALL, theme::MUTED);
    lv_obj_set_size(pill_icon_, chev_w, ui::line_height(Font::ICON_SMALL));
    lv_obj_set_pos(pill_icon_, pill_w_ - chev_w - ui::px(2), (header_h_ - ui::line_height(Font::ICON_SMALL)) / 2);
    ui::set_text(pill_icon_, "\U000F0140");

    // Temperature + Age label on top left (or second row on 1x1)
    meta_lbl_ = ui::label(c.parent, Font::BODY, theme::MUTED);
    if (cols_ == 1) {
      lv_obj_set_size(meta_lbl_, width_, ui::line_height(Font::BODY));
      lv_obj_set_pos(meta_lbl_, 0, header_h_ + ui::px(2));
    } else {
      lv_obj_set_size(meta_lbl_, std::max(1, pill_x_ - pad_), ui::line_height(Font::BODY));
      lv_obj_set_pos(meta_lbl_, 0, (header_h_ - ui::line_height(Font::BODY)) / 2);
    }

    content_top_ = (cols_ == 1) ? (header_h_ + ui::line_height(Font::BODY) + ui::px(4)) : (header_h_ + pad_);
    content_h_ = std::max(1, height_ - content_top_);

    show_grid_ = (rows_ >= 2 || cols_ >= 2);
    if (show_grid_) {
      if (width_ >= ui::px(large ? 300 : 210)) {
        base_cols_ = 4;
      } else if (width_ >= ui::px(large ? 220 : 150)) {
        base_cols_ = 3;
      } else {
        base_cols_ = 2;
      }
      base_rows_ = (rows_ >= 2 && content_h_ >= ui::px(large ? 110 : 76)) ? 2 : 1;
      max_slots_ = (base_cols_ == 4 && base_rows_ == 2) ? kMaxTileSlots : std::min(8, base_cols_ * base_rows_);

      for (int i = 0; i < max_slots_; ++i) {
        slot_bg_[i] = ui::block(c.parent, theme::TRACK);
        slot_icon_[i].build(c.parent, 0, 0, ui::px(28));
        slot_cap_[i] = ui::text(c.parent, Font::BODY, theme::INK);
        lv_obj_set_style_text_align(slot_cap_[i], LV_TEXT_ALIGN_CENTER, 0);
      }

      // Dropdown overlay menu directly on the tile (6 situations), created after grid so it sits on top
      drop_row_h_ = std::max(ui::px(22), std::min(ui::mm(6), (height_ - header_h_ - ui::px(4)) / 6));
      drop_menu_w_ = std::max(pill_w_, std::min(width_, ui::px(large ? 180 : 135)));
      drop_menu_x_ = width_ - drop_menu_w_;
      const int menu_h = drop_row_h_ * 6 + ui::px(4);
      drop_menu_ = ui::block(c.parent, theme::RAISED);
      lv_obj_set_size(drop_menu_, drop_menu_w_, menu_h);
      lv_obj_set_pos(drop_menu_, drop_menu_x_, header_h_ + ui::px(2));
      lv_obj_add_flag(drop_menu_, LV_OBJ_FLAG_HIDDEN);

      for (int i = 0; i < 6; ++i) {
        drop_item_bg_[i] = ui::block(drop_menu_, theme::TRACK);
        lv_obj_set_size(drop_item_bg_[i], drop_menu_w_ - ui::px(4), drop_row_h_ - ui::px(2));
        lv_obj_set_pos(drop_item_bg_[i], ui::px(2), ui::px(2) + i * drop_row_h_);

        drop_item_lbl_[i] = ui::label(drop_item_bg_[i], Font::BODY, theme::INK);
        lv_obj_set_size(drop_item_lbl_[i], drop_menu_w_ - ui::px(12), ui::line_height(Font::BODY));
        lv_obj_set_pos(drop_item_lbl_[i], ui::px(6), (drop_row_h_ - ui::px(2) - ui::line_height(Font::BODY)) / 2);
        ui::set_text(drop_item_lbl_[i], plugin_->situation_label(static_cast<Situation>(i)));
      }
    } else {
      summary_lbl_ = ui::text(c.parent, Font::BODY, theme::INK);
      lv_obj_set_size(summary_lbl_, width_, content_h_);
      lv_obj_set_pos(summary_lbl_, 0, content_top_);
    }

    render(tessera::epoch());
  }

  void on_state(JsonObjectConst data) override {
    JsonObjectConst entities = data["entities"];
    if (!entities.isNull()) {
      opt_out_temp_ = parse_sensor_state(entities["temp_sensor"]);
      opt_in_temp_ = parse_sensor_state(entities["indoor_temp_sensor"]);
      if (!entities["rain_sensor"].isNull()) {
        opt_rain_ = parse_rain_entity(entities["rain_sensor"]);
      } else {
        opt_rain_ = parse_rain_entity(entities["temp_sensor"]);
      }
    }
    plugin_->set_card_overrides(opt_out_temp_, opt_in_temp_, opt_rain_, tile_birthdate_.c_str(), tile_sit_, fixed_sit_);
  }

  void on_tick(uint32_t epoch) override { render(epoch); }

  void on_theme() override { render(tessera::epoch()); }

  void on_tap() override {
    const tessera::Point pt = tessera::tap_point();

    // On grid tiles, tapping the Situation pill toggles the in-tile dropdown directly without opening the detail card
    if (show_grid_ && drop_menu_) {
      const bool tapped_pill = (pt.x >= pill_x_ - ui::px(8) && pt.y >= 0 && pt.y <= header_h_ + ui::px(8));
      if (tapped_pill) {
        tile_dropdown_open_ = !tile_dropdown_open_;
        render(tessera::epoch());
        return;
      }

      if (tile_dropdown_open_) {
        const int menu_top = header_h_ + ui::px(2);
        const int menu_bottom = menu_top + drop_row_h_ * 6 + ui::px(4);
        if (pt.x >= drop_menu_x_ - ui::px(4) && pt.x <= drop_menu_x_ + drop_menu_w_ + ui::px(4) &&
            pt.y >= menu_top && pt.y <= menu_bottom) {
          int idx = (pt.y - menu_top - ui::px(2)) / std::max(1, drop_row_h_);
          if (idx >= 0 && idx < 6) {
            fixed_sit_ = false;
            tile_sit_ = static_cast<Situation>(idx);
            plugin_->set_situation_index(idx);
          }
        }
        tile_dropdown_open_ = false;
        render(tessera::epoch());
        return;
      }
    }

    plugin_->set_card_overrides(opt_out_temp_, opt_in_temp_, opt_rain_, tile_birthdate_.c_str(), tile_sit_, fixed_sit_);
    plugin_->request_open_dropdown(false);
    tessera::open_card(plugin_->plugin_id(), "details", "", tile_index_,
                       name_.empty() ? plugin_->text("title") : roomy::plain(name_));
  }

 private:
  void update_tile_grid(int cols, int rows) {
    if (cols == grid_cols_ && rows == grid_rows_) return;
    grid_cols_ = cols;
    grid_rows_ = rows;
    slot_count_ = std::min(max_slots_, grid_cols_ * grid_rows_);

    const int cell_w = (width_ - pad_ * (grid_cols_ - 1)) / grid_cols_;
    const int cell_h = (content_h_ - pad_ * (grid_rows_ - 1)) / grid_rows_;
    const int lh = ui::line_height(Font::BODY);
    const bool two_line_cap = (cell_h >= lh * 3 + ui::px(14));
    const int cap_h = two_line_cap ? (lh * 2) : lh;
    const int pic_side = std::max(1, std::min(cell_w - ui::px(4), cell_h - cap_h - ui::px(4)));

    for (int i = 0; i < slot_count_; ++i) {
      const int col = i % grid_cols_;
      const int row = i / grid_cols_;
      const int cx = col * (cell_w + pad_);
      const int cy = content_top_ + row * (cell_h + pad_);

      lv_obj_set_size(slot_bg_[i], cell_w, cell_h);
      lv_obj_set_pos(slot_bg_[i], cx, cy);

      slot_icon_[i].set_bounds(cx + (cell_w - pic_side) / 2, cy + ui::px(2), pic_side);

      lv_obj_set_size(slot_cap_[i], cell_w - ui::px(2), cap_h);
      lv_obj_set_pos(slot_cap_[i], cx + ui::px(1), cy + cell_h - cap_h - ui::px(2));
    }
    for (int i = slot_count_; i < max_slots_; ++i) {
      lv_obj_add_flag(slot_bg_[i], LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(slot_cap_[i], LV_OBJ_FLAG_HIDDEN);
      slot_icon_[i].hide();
    }
  }

  void render(uint32_t epoch) {
    const Situation sit = fixed_sit_ ? tile_sit_ : plugin_->active_situation();
    const float out_t = opt_out_temp_;
    const float in_t = opt_in_temp_;
    const char *bdate = tile_birthdate_.c_str();

    const OutfitResult res = plugin_->evaluate(out_t, in_t, opt_rain_, bdate, sit, epoch);

    ui::set_text(pill_lbl_, plugin_->situation_label(res.situation));
    if (pill_icon_) {
      ui::set_text(pill_icon_, tile_dropdown_open_ ? "\U000F0143" : "\U000F0140");
    }
    if (drop_menu_) {
      if (tile_dropdown_open_) {
        lv_obj_clear_flag(drop_menu_, LV_OBJ_FLAG_HIDDEN);
        for (int i = 0; i < 6; ++i) {
          const bool active = (static_cast<int>(res.situation) == i);
          ui::set_fill(drop_item_bg_[i], active ? tessera::Tone::ACCENT : tessera::Tone::NORMAL, theme::TRACK);
          ui::set_color(drop_item_lbl_[i], active ? theme::ON_ACCENT : theme::INK);
        }
      } else {
        lv_obj_add_flag(drop_menu_, LV_OBJ_FLAG_HIDDEN);
      }
    }

    char meta[64];
    const char *age_short = res.age.is_older_baby ? plugin_->text("age_baby") : plugin_->text("age_newborn");
    std::string m_str = res.age.valid ? tessera::format(plugin_->text("months_short"), res.age.months) : std::string(age_short);
    if (res.has_temp) {
      std::snprintf(meta, sizeof(meta), "%.1f °C · %s", res.display_temp, m_str.c_str());
    } else {
      std::snprintf(meta, sizeof(meta), "%s", m_str.c_str());
    }
    ui::set_text(meta_lbl_, meta);

    if (show_grid_) {
      const int base_cap = base_cols_ * base_rows_;
      // Skip Windel (index 0) when more items exist than fit in the standard grid, so outer clothes stay big
      const int offset = (res.count > base_cap && res.count > 1 && res.items[0] == 0) ? 1 : 0;
      const int visible_items = std::max(0, static_cast<int>(res.count) - offset);

      int cols = base_cols_;
      int rows = base_rows_;
      if (base_cols_ == 4 && base_rows_ == 2) {
        if (visible_items > 10 && content_h_ >= ui::px(ui::large() ? 180 : 126)) {
          cols = 4;
          rows = 3;
        } else if (visible_items > 8) {
          cols = 5;
          rows = 2;
        }
      }
      update_tile_grid(cols, rows);

      for (int i = 0; i < slot_count_; ++i) {
        const int idx = offset + i;
        if (idx < res.count) {
          const uint8_t id = res.items[idx];
          lv_obj_clear_flag(slot_bg_[i], LV_OBJ_FLAG_HIDDEN);
          lv_obj_clear_flag(slot_cap_[i], LV_OBJ_FLAG_HIDDEN);
          if (tile_dropdown_open_) {
            slot_icon_[i].hide();
          } else {
            slot_icon_[i].draw(id);
          }
          if (i == slot_count_ - 1 && idx + 1 < res.count) {
            char more[48];
            std::snprintf(more, sizeof(more), "%s +%d", item_label(plugin_, id), (res.count - 1) - idx);
            ui::set_text(slot_cap_[i], more);
          } else {
            ui::set_text(slot_cap_[i], item_label(plugin_, id));
          }
        } else {
          lv_obj_add_flag(slot_bg_[i], LV_OBJ_FLAG_HIDDEN);
          lv_obj_add_flag(slot_cap_[i], LV_OBJ_FLAG_HIDDEN);
          slot_icon_[i].hide();
        }
      }
    } else if (summary_lbl_) {
      if (!res.has_temp) {
        ui::set_text(summary_lbl_, plugin_->text("no_temp"));
      } else {
        std::string list_txt;
        for (int i = 0; i < res.count; ++i) {
          if (i > 0) list_txt += ", ";
          list_txt += item_label(plugin_, res.items[i]);
        }
        ui::set_text(summary_lbl_, list_txt);
      }
    }
  }

  BabyWetter *plugin_;
  lv_obj_t *pill_bg_{}, *pill_lbl_{}, *pill_icon_{}, *meta_lbl_{}, *summary_lbl_{};
  lv_obj_t *drop_menu_{};
  lv_obj_t *drop_item_bg_[6]{};
  lv_obj_t *drop_item_lbl_[6]{};
  lv_obj_t *slot_bg_[kMaxTileSlots]{};
  lv_obj_t *slot_cap_[kMaxTileSlots]{};
  GarmentIcon slot_icon_[kMaxTileSlots]{};

  int width_ = 0, height_ = 0, cols_ = 1, rows_ = 1, tile_index_ = -1;
  int pad_ = 0, header_h_ = 0, pill_x_ = 0, pill_w_ = 0, content_top_ = 0, content_h_ = 0;
  int drop_row_h_ = 0, drop_menu_x_ = 0, drop_menu_w_ = 0;
  int base_cols_ = 2, base_rows_ = 1, max_slots_ = 0;
  int grid_cols_ = 0, grid_rows_ = 0, slot_count_ = 0;
  bool show_grid_ = false;
  bool fixed_sit_ = false;
  bool tile_dropdown_open_ = false;
  Situation tile_sit_ = Situation::ALLGEMEIN;
  tessera::Text name_{};
  tessera::Text tile_birthdate_{};
  float opt_out_temp_ = NAN;
  float opt_in_temp_ = NAN;
  bool opt_rain_ = false;
};

// ---- BabyWetter plugin implementation ----

void BabyWetter::setup() {
  add_tile("outfit", [this]() { return new OutfitTile(this); });
  add_card("details", [this]() { return new OutfitCard(this); }, true);
}

Situation BabyWetter::active_situation() const {
  if (!situation_ || !situation_->has_state()) return Situation::ALLGEMEIN;
  const auto &s = situation_->current_option();
  if (s == "Kinderwagen") return Situation::KINDERWAGEN;
  if (s == "Babytrage") return Situation::BABYTRAGE;
  if (s == "Auto") return Situation::AUTO;
  if (s == "Schlafen") return Situation::SCHLAFEN;
  if (s == "Zuhause") return Situation::ZUHAUSE;
  return Situation::ALLGEMEIN;
}

void BabyWetter::set_situation_index(int index) {
  static constexpr const char *kOptions[6] = {
      "Allgemein", "Kinderwagen", "Babytrage", "Auto", "Schlafen", "Zuhause",
  };
  if (index < 0 || index >= 6) index = 0;
  if (situation_) {
    auto call = situation_->make_call();
    call.set_option(kOptions[index]);
    call.perform();
  }
  card_fixed_sit_ = false;
  tessera::refresh();
}

const char *BabyWetter::situation_label(Situation sit) const {
  switch (sit) {
    case Situation::KINDERWAGEN:
      return text("sit_kiwa")[0] ? text("sit_kiwa") : "Kinderwagen";
    case Situation::BABYTRAGE:
      return text("sit_trage")[0] ? text("sit_trage") : "Babytrage";
    case Situation::AUTO:
      return text("sit_auto")[0] ? text("sit_auto") : "Auto";
    case Situation::SCHLAFEN:
      return text("sit_schlafen")[0] ? text("sit_schlafen") : "Schlafen";
    case Situation::ZUHAUSE:
      return text("sit_zuhause")[0] ? text("sit_zuhause") : "Zuhause";
    case Situation::ALLGEMEIN:
    default:
      return text("sit_none")[0] ? text("sit_none") : "Allgemein";
  }
}

Situation BabyWetter::parse_situation_code(const char *code, Situation fallback) {
  if (!code || !code[0]) return fallback;
  if (std::strcmp(code, "none") == 0 || std::strcmp(code, "Allgemein") == 0) return Situation::ALLGEMEIN;
  if (std::strcmp(code, "kiwa") == 0 || std::strcmp(code, "Kinderwagen") == 0) return Situation::KINDERWAGEN;
  if (std::strcmp(code, "trage") == 0 || std::strcmp(code, "Babytrage") == 0) return Situation::BABYTRAGE;
  if (std::strcmp(code, "auto") == 0 || std::strcmp(code, "Auto") == 0) return Situation::AUTO;
  if (std::strcmp(code, "schlafen") == 0 || std::strcmp(code, "Schlafen") == 0) return Situation::SCHLAFEN;
  if (std::strcmp(code, "zuhause") == 0 || std::strcmp(code, "Zuhause") == 0) return Situation::ZUHAUSE;
  return fallback;
}

OutfitResult BabyWetter::evaluate_for_card(uint32_t epoch) const {
  const Situation sit = card_fixed_sit_ ? card_sit_ : active_situation();
  return evaluate(card_out_temp_, card_in_temp_, card_rain_, card_birthdate_.c_str(), sit, epoch);
}

OutfitResult BabyWetter::evaluate(float out_temp, float in_temp, bool rain, const char *birthdate_str, Situation sit,
                                  uint32_t epoch) const {
  OutfitResult res{};
  res.situation = sit;
  res.age = calculate_age(birthdate_str, epoch);

  const bool is_indoor = (sit == Situation::SCHLAFEN || sit == Situation::ZUHAUSE);
  if (is_indoor) {
    int t_in = 20;
    if (!std::isnan(in_temp)) {
      res.has_temp = true;
      res.display_temp = in_temp;
      res.used_estimated_indoor = false;
      t_in = static_cast<int>(std::lround(in_temp));
    } else if (!std::isnan(out_temp)) {
      res.has_temp = true;
      t_in = estimate_inside_temperature(out_temp);
      res.display_temp = static_cast<float>(t_in);
      res.used_estimated_indoor = true;
    } else {
      res.has_temp = false;
      res.display_temp = 20.0f;
    }
    if (sit == Situation::SCHLAFEN) {
      evaluate_sleep(t_in, res);
    } else {
      evaluate_inside(t_in, res.age.is_older_baby, res);
    }
  } else {
    float t_use = !std::isnan(out_temp) ? out_temp : in_temp;
    int t_out = 15;
    if (!std::isnan(t_use)) {
      res.has_temp = true;
      res.display_temp = t_use;
      t_out = static_cast<int>(std::lround(t_use));
    } else {
      res.has_temp = false;
      res.display_temp = 15.0f;
    }
    evaluate_outdoor(t_out, res.age.is_older_baby, sit, rain, res);
  }
  return res;
}

bool BabyWetter::settings(tessera::SettingsPage &page) {
  page.icon = "\U000F068F";
  page.choice(
      text("situation"),
      {
          situation_label(Situation::ALLGEMEIN),
          situation_label(Situation::KINDERWAGEN),
          situation_label(Situation::BABYTRAGE),
          situation_label(Situation::AUTO),
          situation_label(Situation::SCHLAFEN),
          situation_label(Situation::ZUHAUSE),
      },
      [this]() { return situation_index(); },
      [this](int idx) { set_situation_index(idx); });
  page.card(text("open_card"), "\U000F068F", "details");
  return true;
}

}  // namespace esphome::baby_wetter
