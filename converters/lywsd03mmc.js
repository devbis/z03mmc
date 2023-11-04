const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const tz = require('zigbee-herdsman-converters/converters/toZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const ota = require('zigbee-herdsman-converters/lib/ota');
const e = exposes.presets;

const definition = {
    zigbeeModel: ['LYWSD03MMC'],
    model: 'LYWSD03MMC',
    vendor: 'Xiaomi',
    description: 'Temperature & humidity sensor',
    fromZigbee: [fz.temperature, fz.humidity, fz.battery, fz.hvac_user_interface],
    toZigbee: [tz.thermostat_temperature_display_mode],
    configure: async (device, coordinatorEndpoint, logger) => {
        const endpoint = device.getEndpoint(1);
        const bindClusters = ['msTemperatureMeasurement', 'msRelativeHumidity', 'genPowerCfg'];
        await reporting.bind(endpoint, coordinatorEndpoint, bindClusters);
        await reporting.temperature(endpoint, {min: 10, max: 300, change: 10});
        await reporting.humidity(endpoint, {min: 10, max: 300, change: 50});
        await reporting.batteryVoltage(endpoint);
        await reporting.batteryPercentageRemaining(endpoint);
    },
    exposes: [
        e.temperature(), e.humidity(), e.battery(),
        exposes.enum('temperature_display_mode', exposes.access.ALL, ['celsius', 'fahrenheit'])
        .withDescription('The temperature format displayed on the screen'),
    ],
    ota: ota.zigbeeOTA,
};

module.exports = definition;
