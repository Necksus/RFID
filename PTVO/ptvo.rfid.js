const zigbeeHerdsmanConverters = require('zigbee-herdsman-converters');
const zigbeeHerdsmanUtils = require('zigbee-herdsman-converters/lib/utils');


const exposes = zigbeeHerdsmanConverters.exposes;
const ea = exposes.access;
const e = exposes.presets;
const fz = zigbeeHerdsmanConverters.fromZigbeeConverters;
const tz = zigbeeHerdsmanConverters.toZigbeeConverters;

const ptvo_switch = zigbeeHerdsmanConverters.findByDevice({modelID: 'ptvo.switch'});
fz.legacy = ptvo_switch.meta.tuyaThermostatPreset;
fz.ptvo_on_off = {
  cluster: 'genOnOff',
  type: ['attributeReport', 'readResponse'],
  convert: (model, msg, publish, options, meta) => {
      if (msg.data.hasOwnProperty('onOff')) {
          const channel = msg.endpoint.ID;
          const endpointName = `l${channel}`;
          const binaryEndpoint = model.meta && model.meta.binaryEndpoints && model.meta.binaryEndpoints[endpointName];
          const prefix = (binaryEndpoint) ? model.meta.binaryEndpoints[endpointName] : 'state';
          const property = `${prefix}_${endpointName}`;
      if (binaryEndpoint) {
            return {[property]: msg.data['onOff'] === 1};
          }
          return {[property]: msg.data['onOff'] === 1 ? 'ON' : 'OFF'};
      }
  },
};



const device = {
    zigbeeModel: ['ptvo.rfid'],
    model: 'ptvo.rfid',
    vendor: 'Fox-Nest Inc',
    description: 'DIY RFID tag reader',
    fromZigbee: [
        fz.on_off,
        fz.ignore_basic_report,
        fz.ptvo_switch_uart,
        fz.ptvo_multistate_action
    ],
    toZigbee: [
        tz.ptvo_switch_trigger, 
        tz.ptvo_switch_uart,
        tz.on_off
    ],
    exposes: [
        exposes.text('action', ea.STATE_SET).withDescription('RFID tag scanned'),
        e.switch().withEndpoint('l2')
    ],
    meta: {
        multiEndpoint: true        
    },
    endpoint: (device) => {
        return {
            l1: 1, action: 1, 'l2': 2,
        };
    },
    configure: async (device, coordinatorEndpoint, logger) => {
            const endpoint = device.getEndpoint(1);
      await endpoint.read('genBasic', ['modelId', 'swBuildId', 'powerSource']);
    },

};

module.exports = device;
