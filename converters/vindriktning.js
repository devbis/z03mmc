const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const ota = require('zigbee-herdsman-converters/lib/ota');
const e = exposes.presets;
const ea = exposes.access;


const definition = {
    zigbeeModel: ['Vindriktning z03mmc'],
    model: 'Vindriktning (03mmc)',
    vendor: 'IKEA',
    description: 'Air Quality Sensor',
    fromZigbee: [fz.temperature, fz.humidity, fz.pm25, fz.pm1, fz.pm10],
    toZigbee: [],
    ota: ota.zigbeeOTA,
    exposes: [
      e.pm25(),
      e.pm1(),
      e.pm10(),
      e.temperature(),
      e.humidity(),
    ],
    configure: async (device, coordinatorEndpoint, logger) => {
        const endpoint = device.getEndpoint(1);
        const bindClusters = ['msTemperatureMeasurement', 'msRelativeHumidity', 'pm25Measurement', 'pm1Measurement', 'pm10Measurement'];
        await reporting.bind(endpoint, coordinatorEndpoint, bindClusters);
        await reporting.temperature(endpoint, {min: 10, max: 300, change: 10});
        await reporting.humidity(endpoint, {min: 10, max: 300, change: 50});
        const payload = [{
            attribute: 'measuredValue',
            minimumReportInterval: 10,
            maximumReportInterval: 120,
            reportableChange: 2,
        }]
        await endpoint.configureReporting('pm25Measurement', payload);
        await endpoint.configureReporting('pm1Measurement', payload);
        await endpoint.configureReporting('pm10Measurement', payload);
    },
    icon: 'data:image/jpeg;base64,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',
};

module.exports = definition;

