const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const ota = require('zigbee-herdsman-converters/lib/ota');
const e = exposes.presets;

const definition = {
    zigbeeModel: ['LYWSD03MMC'],
    model: 'LYWSD03MMC',
    vendor: 'Xiaomi',
    description: 'Temperature & humidity sensor',
    fromZigbee: [fz.temperature, fz.humidity, fz.battery],
    toZigbee: [],
    configure: async (device, coordinatorEndpoint, logger) => {
        const endpoint = device.getEndpoint(1);
        const bindClusters = ['msTemperatureMeasurement', 'msRelativeHumidity', 'genPowerCfg'];
        await reporting.bind(endpoint, coordinatorEndpoint, bindClusters);
        await reporting.temperature(endpoint);
        await reporting.humidity(endpoint);
        await reporting.batteryVoltage(endpoint);
        await reporting.batteryPercentageRemaining(endpoint);
    },
    exposes: [e.temperature(), e.humidity(), e.battery()],
    ota: ota.zigbeeOTA,
};

module.exports = definition;
