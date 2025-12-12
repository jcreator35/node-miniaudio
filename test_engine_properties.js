const { getChannels, getSampleRate, setVolume, getVolume } = require('./lib/binding');

console.log('=== Testing Engine Properties Functions ===');

// Get initial properties
const channels = getChannels();
const sampleRate = getSampleRate();
const initialVolume = getVolume();

console.log('Number of channels:', channels);
console.log('Sample rate:', sampleRate, 'Hz');
console.log('Initial volume:', initialVolume);

// Test volume control
console.log('\n=== Testing Volume Control ===');

// Set a new volume
const newVolume = 0.5;
console.log('Setting volume to:', newVolume);
setVolume(newVolume);

// Verify the volume change
const updatedVolume = getVolume();
console.log('Updated volume:', updatedVolume);

// Test another volume value
const anotherVolume = 1.2;
console.log('\nSetting volume to:', anotherVolume);
setVolume(anotherVolume);

// Verify the second volume change
const finalVolume = getVolume();
console.log('Final volume:', finalVolume);

console.log('\n=== Testing Complete ===');