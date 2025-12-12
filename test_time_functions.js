const { getTimeInPcmFrames, getTimeInMilliseconds, setTimeInPcmFrames, setTimeInMilliseconds } = require('./lib/binding');

console.log('=== Testing Time Functions ===');

// Get initial time
const initialPcm = getTimeInPcmFrames();
const initialMs = getTimeInMilliseconds();
console.log('Initial time in PCM frames:', initialPcm);
console.log('Initial time in milliseconds:', initialMs);

// Set time in PCM frames
const newPcmTime = 10000;
setTimeInPcmFrames(newPcmTime);
const updatedPcm = getTimeInPcmFrames();
const updatedMsFromPcm = getTimeInMilliseconds();
console.log('\nAfter setting PCM frames to', newPcmTime + ':');
console.log('Time in PCM frames:', updatedPcm);
console.log('Time in milliseconds:', updatedMsFromPcm);

// Set time in milliseconds
const newMsTime = 5000;
setTimeInMilliseconds(newMsTime);
const updatedMs = getTimeInMilliseconds();
const updatedPcmFromMs = getTimeInPcmFrames();
console.log('\nAfter setting milliseconds to', newMsTime + ':');
console.log('Time in milliseconds:', updatedMs);
console.log('Time in PCM frames:', updatedPcmFromMs);

console.log('\n=== Testing Complete ===');