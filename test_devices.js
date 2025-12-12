const { getDevices } = require('./lib');

try {
    console.log('Getting audio devices...');
    const devices = getDevices();
    console.log('Devices:', devices);
    
    console.log('\nPlayback devices:');
    devices.playback.forEach((device, index) => {
        console.log(`${index + 1}. ${device.name}`);
    });
    
    console.log('\nCapture devices:');
    devices.capture.forEach((device, index) => {
        console.log(`${index + 1}. ${device.name}`);
    });
} catch (error) {
    console.error('Error getting devices:', error);
}