/**
 * Callback function type for audio playback completion.
 * @param message - Message indicating playback completion.
 */

export type AudioPlaybackCallback = (message: string) => void;

export type CommonCallback = (message: string) => void;

/**
 * Audio device information.
 */
export interface AudioDeviceInfo {
    /**
     * Name of the audio device.
     */
    name: string;
}

/**
 * Device information object containing playback and capture devices.
 */
export interface DeviceInfo {
    /**
     * Array of playback devices.
     */
    playback: AudioDeviceInfo[];
    /**
     * Array of capture devices.
     */
    capture: AudioDeviceInfo[];
}

/**
 * Plays an audio file asynchronously.
 * @param filePath - Absolute or relative path to the audio file (must be supported format like WAV, MP3, etc.).
 * @param callback - Function that gets called when playback is complete.
 * @returns A string indicating playback start.
 */
export function playAudio(filePath: string, callback: AudioPlaybackCallback): string;

/**
 * Gets information about available audio devices.
 * @returns An object containing arrays of playback and capture devices.
 */
export function getDevices(callback: CommonCallback): DeviceInfo;

/**
 * Gets the engine's current time in PCM frames.
 * @returns The current time in PCM frames.
 */
export function getTimeInPcmFrames(callback: CommonCallback): number;

/**
 * Gets the engine's current time in milliseconds.
 * @returns The current time in milliseconds.
 */
export function getTimeInMilliseconds(callback: CommonCallback): number;

/**
 * Sets the engine's current time in PCM frames.
 * @param time - The time to set in PCM frames.
 */
export function setTimeInPcmFrames(time: number, callback: CommonCallback): void;

/**
 * Sets the engine's current time in milliseconds.
 * @param time - The time to set in milliseconds.
 */
export function setTimeInMilliseconds(time: number, callback: CommonCallback): void;

/**
 * Gets the number of channels used by the engine.
 * @returns The number of channels.
 */
export function getChannels(callback: CommonCallback): number;

/**
 * Gets the sample rate used by the engine.
 * @returns The sample rate in Hz.
 */
export function getSampleRate(callback: CommonCallback): number;

/**
 * Sets the master volume of the engine.
 * @param volume - The volume level (0.0 to 1.0, or higher for amplification).
 */
export function setVolume(volume: number, callback: CommonCallback): void;

/**
 * Gets the current master volume of the engine.
 * @returns The current volume level.
 */
export function getVolume(callback: CommonCallback): number;
