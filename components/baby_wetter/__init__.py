"""Baby Wetter: clothing recommendations from baby-wetter.de on a tile and a detail card.

Reads the outdoor and indoor temperature sensors from Home Assistant, the situation select entity, and the configured
birthdate, and registers the tile ("outfit") and detail card ("details") with Tessera's smart_display plugin host.
"""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select, sensor, smart_display
from esphome.const import CONF_ID

DEPENDENCIES = ["smart_display"]

CONF_OUTDOOR_TEMP = "outdoor_temp"
CONF_INDOOR_TEMP = "indoor_temp"
CONF_SITUATION = "situation"
CONF_BIRTHDATE = "birthdate"

baby_wetter_ns = cg.esphome_ns.namespace("baby_wetter")
BabyWetter = baby_wetter_ns.class_("BabyWetter", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(BabyWetter),
        cv.Required(CONF_OUTDOOR_TEMP): cv.use_id(sensor.Sensor),
        cv.Required(CONF_INDOOR_TEMP): cv.use_id(sensor.Sensor),
        cv.Required(CONF_SITUATION): cv.use_id(select.Select),
        cv.Required(CONF_BIRTHDATE): cv.string,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await smart_display.register_plugin(var, __file__)
    cg.add(var.set_outdoor_temp(await cg.get_variable(config[CONF_OUTDOOR_TEMP])))
    cg.add(var.set_indoor_temp(await cg.get_variable(config[CONF_INDOOR_TEMP])))
    cg.add(var.set_situation(await cg.get_variable(config[CONF_SITUATION])))
    cg.add(var.set_birthdate(config[CONF_BIRTHDATE]))
