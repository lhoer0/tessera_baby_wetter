"""Baby Wetter: clothing recommendations from baby-wetter.de on a tile and a detail card.

Registers the tile ("outfit") and detail card ("details") with Tessera's smart_display plugin host and connects the
Situation select entity.
"""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select, smart_display
from esphome.const import CONF_ID

DEPENDENCIES = ["smart_display"]

CONF_SITUATION = "situation"

baby_wetter_ns = cg.esphome_ns.namespace("baby_wetter")
BabyWetter = baby_wetter_ns.class_("BabyWetter", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(BabyWetter),
        cv.Required(CONF_SITUATION): cv.use_id(select.Select),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await smart_display.register_plugin(var, __file__)
    cg.add(var.set_situation(await cg.get_variable(config[CONF_SITUATION])))
