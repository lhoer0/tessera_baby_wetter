#pragma once
#include <cstdint>
#include "esphome/core/component.h"
#include "esphome/components/select/select.h"
#include "esphome/components/smart_display/plugin_api.h"

namespace esphome::baby_wetter {

enum class Situation : uint8_t {
  ALLGEMEIN = 0,
  KINDERWAGEN = 1,
  BABYTRAGE = 2,
  AUTO = 3,
  SCHLAFEN = 4,
  ZUHAUSE = 5,
};

struct AgeInfo {
  bool valid = false;
  bool is_older_baby = false;  // false: 0-3 months ('neugeborenes'), true: 4+ months ('baby')
  int months = 0;
};

struct OutfitResult {
  uint8_t items[14]{};
  uint8_t count = 0;
  bool has_temp = false;
  bool used_estimated_indoor = false;
  float display_temp = 0.0f;
  AgeInfo age{};
  Situation situation = Situation::ALLGEMEIN;
};

class BabyWetter : public Component, public tessera::Plugin {
 public:
  void setup() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  bool settings(tessera::SettingsPage &page) override;

  void set_situation(select::Select *s) { situation_ = s; }

  Situation active_situation() const;
  int situation_index() const { return static_cast<int>(active_situation()); }
  void set_situation_index(int index);
  void set_situation_mode(Situation sit) { set_situation_index(static_cast<int>(sit)); }

  const char *situation_label(Situation sit) const;
  static Situation parse_situation_code(const char *code, Situation fallback);

  // If the user taps the Situation dropdown pill on the tile, the card opens with the dropdown expanded.
  void request_open_dropdown(bool open) { open_with_dropdown_ = open; }
  bool consume_open_dropdown() {
    bool v = open_with_dropdown_;
    open_with_dropdown_ = false;
    return v;
  }

  // Store the tile's configured sensors/birthdate so the detail card uses them seamlessly.
  void set_card_overrides(float tile_out_temp, float tile_in_temp, bool tile_rain, const char *tile_birthdate,
                          Situation tile_sit, bool fixed_sit) {
    card_out_temp_ = tile_out_temp;
    card_in_temp_ = tile_in_temp;
    card_rain_ = tile_rain;
    card_birthdate_ = tile_birthdate ? tile_birthdate : "";
    card_sit_ = tile_sit;
    card_fixed_sit_ = fixed_sit;
  }

  OutfitResult evaluate_for_card(uint32_t epoch) const;
  OutfitResult evaluate(float out_temp, float in_temp, bool rain, const char *birthdate_str, Situation sit,
                        uint32_t epoch) const;

 protected:
  select::Select *situation_{};

  bool open_with_dropdown_ = false;
  float card_out_temp_ = NAN;
  float card_in_temp_ = NAN;
  bool card_rain_ = false;
  tessera::Text card_birthdate_{};
  Situation card_sit_ = Situation::ALLGEMEIN;
  bool card_fixed_sit_ = false;
};

}  // namespace esphome::baby_wetter
