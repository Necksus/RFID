const zigbeeHerdsmanConverters = require('zigbee-herdsman-converters');
const zigbeeHerdsmanUtils = require('zigbee-herdsman-converters/lib/utils');


const exposes = zigbeeHerdsmanConverters['exposes'] || require("zigbee-herdsman-converters/lib/exposes");
const ea = exposes.access;
const e = exposes.presets;
const fz = zigbeeHerdsmanConverters.fromZigbeeConverters || zigbeeHerdsmanConverters.fromZigbee;
const tz = zigbeeHerdsmanConverters.toZigbeeConverters || zigbeeHerdsmanConverters.toZigbee;

const ptvo_switch = (zigbeeHerdsmanConverters.findByModel)?zigbeeHerdsmanConverters.findByModel('ptvo.switch'):zigbeeHerdsmanConverters.findByDevice({modelID: 'ptvo.switch'});
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
        e.switch().withEndpoint('l2'),
        e.switch().withEndpoint('l3'),
        e.switch().withEndpoint('l4')
    ],
    meta: {
        multiEndpoint: true        
    },
    endpoint: (device) => {
        return {
            l1: 1, 
			action: 1, 
			'l2': 2, 
			'l3': 3, 
			'l4': 4
        };
    },
    icon: 'data:image/png;base64,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',
    configure: async (device, coordinatorEndpoint, logger) => {
            const endpoint = device.getEndpoint(1);
      await endpoint.read('genBasic', ['modelId', 'swBuildId', 'powerSource']);
    },

};

module.exports = device;
