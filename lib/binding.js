const path = require('path');
const os = require('os');
let addon = {}, binary_path;
switch (os.platform()) {
    case 'win32':
        if (os.arch() === 'ia32') {
            binary_path = path.join(__dirname, 'node-miniaudio.node');
        } else if (os.arch() === 'x64') {
            binary_path = path.join(__dirname, 'node-miniaudio.node');
        }
        addon = require(binary_path);
        break;
    case 'darwin':
        binary_path = path.join(__dirname, 'node-miniaudio.node'); 
        addon = require(binary_path);
        break;
    case 'linux':
        if (os.arch() === 'ia32') {
            addon = path.join(__dirname, 'node-miniaudio.node');
        } else if (os.arch() === 'x64') {
            addon = path.join(__dirname, 'node-miniaudio.node');
        }
        break;
    default:
        binary_path = path.join(__dirname, 'node-miniaudio.node'); 
        addon = require(binary_path);
}

module.exports.playAudio = addon.playAudio;
module.exports.getDevices = addon.getDevices;
module.exports.getTimeInPcmFrames = addon.getTimeInPcmFrames;
module.exports.getTimeInMilliseconds = addon.getTimeInMilliseconds;
module.exports.setTimeInPcmFrames = addon.setTimeInPcmFrames;
module.exports.setTimeInMilliseconds = addon.setTimeInMilliseconds;
module.exports.getChannels = addon.getChannels;
module.exports.getSampleRate = addon.getSampleRate;
module.exports.setVolume = addon.setVolume;
module.exports.getVolume = addon.getVolume;