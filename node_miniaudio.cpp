#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <napi.h>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <mutex>
#include <string>

class AudioEngine {
public:
    AudioEngine() {
        //std::cout << "[DEBUG] Initializing MiniAudio engine..." << std::endl;
        if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
            std::cerr << "[ERROR] Failed to initialize MiniAudio engine!" << std::endl;
            throw std::runtime_error("Failed to initialize MiniAudio engine.");
        }
        //std::cout << "[DEBUG] MiniAudio engine initialized successfully!" << std::endl;
    }

    ~AudioEngine() {
        //std::cout << "[DEBUG] Cleaning up MiniAudio engine..." << std::endl;
        ma_engine_uninit(&engine);
    }

    Napi::Value PlayAudio(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        //std::cout << "[DEBUG] PlayAudio function called..." << std::endl;

        if (info.Length() < 2 || !info[0].IsString() || !info[1].IsFunction()) {
            std::cerr << "[ERROR] Invalid arguments! Expected a file path and a callback function." << std::endl;
            Napi::Error::New(env, "Error: Expected a file path (string) and a callback function.").ThrowAsJavaScriptException();
            return env.Null();
        }

        std::string filePath = info[0].As<Napi::String>().Utf8Value();
        Napi::Function callback = info[1].As<Napi::Function>();

        //std::cout << "[DEBUG] Received file path: " << filePath << std::endl;

        if (!std::ifstream(filePath)) {
            std::cerr << "[ERROR] Audio file not found at path: " << filePath << std::endl;
            Napi::Error::New(env, "Error: Audio file not found at path: " + filePath).ThrowAsJavaScriptException();
            return env.Null();
        }

        ma_sound sound;
        //std::cout << "[DEBUG] Initializing sound..." << std::endl;
        if (ma_sound_init_from_file(&engine, filePath.c_str(), 0, NULL, NULL, &sound) != MA_SUCCESS) {
            std::cerr << "[ERROR] Sound initialization failed!" << std::endl;
            Napi::Error::New(env, "Sound initialization failed.").ThrowAsJavaScriptException();
            return env.Null();
        }

        //std::cout << "[DEBUG] Starting sound playback..." << std::endl;
        if (ma_sound_start(&sound) != MA_SUCCESS) {
            std::cerr << "[ERROR] Failed to start sound playback!" << std::endl;
            ma_sound_uninit(&sound);
            Napi::Error::New(env, "Error: Failed to start sound playback!").ThrowAsJavaScriptException();
            return env.Null();
        }

        //std::cout << "[DEBUG] Audio is now playing: " << filePath << std::endl;

        // Sleep for a reasonable duration to allow audio playback before cleanup
        //std::cout << "[DEBUG] Sleeping to allow sound playback..." << std::endl;
        Sleep(500);  // Adjust the sleep duration based on your audio length

        //std::cout << "[DEBUG] Uninitializing sound..." << std::endl;
        ma_sound_uninit(&sound);

        //std::cout << "[DEBUG] Executing callback..." << std::endl;
        callback.Call({Napi::String::New(env, "Audio finished playing!")});

        return Napi::String::New(env, "Playing: " + filePath);
    }

    Napi::Value GetDevices(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        
        // Initialize a miniaudio context
        ma_context context;
        ma_context_config contextConfig = ma_context_config_init();
        
        if (ma_context_init(NULL, 0, &contextConfig, &context) != MA_SUCCESS) {
            Napi::Error::New(env, "Failed to initialize MiniAudio context.").ThrowAsJavaScriptException();
            return env.Null();
        }
        
        // Get devices information
        ma_device_info* pPlaybackDeviceInfos;
        ma_uint32 playbackDeviceCount;
        ma_device_info* pCaptureDeviceInfos;
        ma_uint32 captureDeviceCount;
        
        ma_result result = ma_context_get_devices(&context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount);
        
        Napi::Object devices = Napi::Object::New(env);
        Napi::Array playbackDevices = Napi::Array::New(env, playbackDeviceCount);
        Napi::Array captureDevices = Napi::Array::New(env, captureDeviceCount);
        
        if (result == MA_SUCCESS) {
            // Process playback devices
            for (ma_uint32 i = 0; i < playbackDeviceCount; ++i) {
                Napi::Object device = Napi::Object::New(env);
                device.Set("name", Napi::String::New(env, pPlaybackDeviceInfos[i].name));
                playbackDevices.Set(i, device);
            }
            
            // Process capture devices
            for (ma_uint32 i = 0; i < captureDeviceCount; ++i) {
                Napi::Object device = Napi::Object::New(env);
                device.Set("name", Napi::String::New(env, pCaptureDeviceInfos[i].name));
                captureDevices.Set(i, device);
            }
        }
        
        // Clean up the context
        ma_context_uninit(&context);
        
        // Return devices information
        devices.Set("playback", playbackDevices);
        devices.Set("capture", captureDevices);
        return devices;
    }

    Napi::Value GetTimeInPcmFrames(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        ma_uint64 time = ma_engine_get_time_in_pcm_frames(&engine);
        return Napi::Number::New(env, time);
    }

    Napi::Value GetTimeInMilliseconds(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        ma_uint64 time = ma_engine_get_time_in_milliseconds(&engine);
        return Napi::Number::New(env, time);
    }

    Napi::Value SetTimeInPcmFrames(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        
        if (info.Length() < 1 || !info[0].IsNumber()) {
            Napi::Error::New(env, "Error: Expected a time value (number).").ThrowAsJavaScriptException();
            return env.Null();
        }
        
        ma_uint64 time = static_cast<ma_uint64>(info[0].As<Napi::Number>().Uint32Value());
        ma_result result = ma_engine_set_time_in_pcm_frames(&engine, time);
        
        if (result != MA_SUCCESS) {
            Napi::Error::New(env, "Error: Failed to set time in PCM frames.").ThrowAsJavaScriptException();
            return env.Null();
        }
        
        return env.Undefined();
    }

    Napi::Value SetTimeInMilliseconds(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        
        if (info.Length() < 1 || !info[0].IsNumber()) {
            Napi::Error::New(env, "Error: Expected a time value (number).").ThrowAsJavaScriptException();
            return env.Null();
        }
        
        ma_uint64 time = static_cast<ma_uint64>(info[0].As<Napi::Number>().Uint32Value());
        ma_result result = ma_engine_set_time_in_milliseconds(&engine, time);
        
        if (result != MA_SUCCESS) {
            Napi::Error::New(env, "Error: Failed to set time in milliseconds.").ThrowAsJavaScriptException();
            return env.Null();
        }
        
        return env.Undefined();
    }

    Napi::Value GetChannels(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        ma_uint32 channels = ma_engine_get_channels(&engine);
        return Napi::Number::New(env, channels);
    }

    Napi::Value GetSampleRate(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        ma_uint32 sampleRate = ma_engine_get_sample_rate(&engine);
        return Napi::Number::New(env, sampleRate);
    }

    Napi::Value SetVolume(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        
        if (info.Length() < 1 || !info[0].IsNumber()) {
            Napi::Error::New(env, "Error: Expected a volume value (number).").ThrowAsJavaScriptException();
            return env.Null();
        }
        
        float volume = static_cast<float>(info[0].As<Napi::Number>().FloatValue());
        ma_result result = ma_engine_set_volume(&engine, volume);
        
        if (result != MA_SUCCESS) {
            Napi::Error::New(env, "Error: Failed to set volume.").ThrowAsJavaScriptException();
            return env.Null();
        }
        
        return env.Undefined();
    }

    Napi::Value GetVolume(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        float volume = ma_engine_get_volume(&engine);
        return Napi::Number::New(env, volume);
    }

private:
    ma_engine engine;
};

// Singleton engine instance with mutex for safety
std::mutex engineMutex;
AudioEngine* globalAudioEngine = nullptr;

Napi::Value PlayAudioWrapper(const Napi::CallbackInfo& info) {
    std::lock_guard<std::mutex> lock(engineMutex);
    //std::cout << "[DEBUG] PlayAudioWrapper function called..." << std::endl;

    if (!globalAudioEngine) {
        //std::cout << "[DEBUG] Creating new AudioEngine instance..." << std::endl;
        globalAudioEngine = new AudioEngine();
    }

    return globalAudioEngine->PlayAudio(info);
}

Napi::Value GetDevicesWrapper(const Napi::CallbackInfo& info) {
    std::lock_guard<std::mutex> lock(engineMutex);
    //std::cout << "[DEBUG] GetDevicesWrapper function called..." << std::endl;

    if (!globalAudioEngine) {
        //std::cout << "[DEBUG] Creating new AudioEngine instance..." << std::endl;
        globalAudioEngine = new AudioEngine();
    }

    return globalAudioEngine->GetDevices(info);
}

Napi::Value GetTimeInPcmFramesWrapper(const Napi::CallbackInfo& info) {
    std::lock_guard<std::mutex> lock(engineMutex);

    if (!globalAudioEngine) {
        globalAudioEngine = new AudioEngine();
    }

    return globalAudioEngine->GetTimeInPcmFrames(info);
}

Napi::Value GetTimeInMillisecondsWrapper(const Napi::CallbackInfo& info) {
    std::lock_guard<std::mutex> lock(engineMutex);

    if (!globalAudioEngine) {
        globalAudioEngine = new AudioEngine();
    }

    return globalAudioEngine->GetTimeInMilliseconds(info);
}

Napi::Value SetTimeInPcmFramesWrapper(const Napi::CallbackInfo& info) {
    std::lock_guard<std::mutex> lock(engineMutex);

    if (!globalAudioEngine) {
        globalAudioEngine = new AudioEngine();
    }

    return globalAudioEngine->SetTimeInPcmFrames(info);
}

Napi::Value SetTimeInMillisecondsWrapper(const Napi::CallbackInfo& info) {
    std::lock_guard<std::mutex> lock(engineMutex);

    if (!globalAudioEngine) {
        globalAudioEngine = new AudioEngine();
    }

    return globalAudioEngine->SetTimeInMilliseconds(info);
}

Napi::Value GetChannelsWrapper(const Napi::CallbackInfo& info) {
    std::lock_guard<std::mutex> lock(engineMutex);

    if (!globalAudioEngine) {
        globalAudioEngine = new AudioEngine();
    }

    return globalAudioEngine->GetChannels(info);
}

Napi::Value GetSampleRateWrapper(const Napi::CallbackInfo& info) {
    std::lock_guard<std::mutex> lock(engineMutex);

    if (!globalAudioEngine) {
        globalAudioEngine = new AudioEngine();
    }

    return globalAudioEngine->GetSampleRate(info);
}

Napi::Value SetVolumeWrapper(const Napi::CallbackInfo& info) {
    std::lock_guard<std::mutex> lock(engineMutex);

    if (!globalAudioEngine) {
        globalAudioEngine = new AudioEngine();
    }

    return globalAudioEngine->SetVolume(info);
}

Napi::Value GetVolumeWrapper(const Napi::CallbackInfo& info) {
    std::lock_guard<std::mutex> lock(engineMutex);

    if (!globalAudioEngine) {
        globalAudioEngine = new AudioEngine();
    }

    return globalAudioEngine->GetVolume(info);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    //std::cout << "[DEBUG] Initializing module exports..." << std::endl;
    exports.Set("playAudio", Napi::Function::New(env, PlayAudioWrapper));
    exports.Set("getDevices", Napi::Function::New(env, GetDevicesWrapper));
    exports.Set("getTimeInPcmFrames", Napi::Function::New(env, GetTimeInPcmFramesWrapper));
    exports.Set("getTimeInMilliseconds", Napi::Function::New(env, GetTimeInMillisecondsWrapper));
    exports.Set("setTimeInPcmFrames", Napi::Function::New(env, SetTimeInPcmFramesWrapper));
    exports.Set("setTimeInMilliseconds", Napi::Function::New(env, SetTimeInMillisecondsWrapper));
    exports.Set("getChannels", Napi::Function::New(env, GetChannelsWrapper));
    exports.Set("getSampleRate", Napi::Function::New(env, GetSampleRateWrapper));
    exports.Set("setVolume", Napi::Function::New(env, SetVolumeWrapper));
    exports.Set("getVolume", Napi::Function::New(env, GetVolumeWrapper));
    return exports;
}

NODE_API_MODULE(node_miniaudio, Init)