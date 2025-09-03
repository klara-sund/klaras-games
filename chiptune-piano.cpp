#include <iostream>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>

// Define our audio parameters
const int SAMPLE_RATE = 44100;
const int SAMPLES_PER_BUFFER = 512;
const int CHANNELS = 1; // Mono
const double VOLUME = 0.5;

// Global state for audio generation
double g_frequency = 0.0;
double g_phase = 0.0;

// This callback function is called by SDL whenever it needs more audio data
void audio_callback(void* userdata, Uint8* stream, int len) {
    // Cast the stream to a signed 16-bit integer array
    Sint16* audio_stream = reinterpret_cast<Sint16*>(stream);
    int num_samples = len / sizeof(Sint16);

    for (int i = 0; i < num_samples; ++i) {
        if (g_frequency > 0.0) {
            // Calculate the current sample value for a square wave
            Sint16 sample = (std::sin(g_phase * 2.0 * M_PI) >= 0.0) ? 
                             static_cast<Sint16>(32767.0 * VOLUME) : 
                             static_cast<Sint16>(-32767.0 * VOLUME);
            
            // Increment the phase for the next sample
            g_phase += g_frequency / SAMPLE_RATE;
        
            // Wrap the phase to prevent it from growing too large
            if (g_phase >= 1.0) {
                g_phase -= 1.0;
            }
            
            audio_stream[i] = sample;
        } else {
            // If frequency is 0, play silence
            audio_stream[i] = 0;
        }
    }
}

// Function to get the frequency of a musical note
double noteToFrequency(int note) {
    // A4 is 440 Hz (MIDI note 69)
    double a4_freq = 440.0;
    double a4_midi_note = 69.0;
    return a4_freq * std::pow(2.0, (note - a4_midi_note) / 12.0);
}

int main(int argc, char* argv[]) {
    // Initialize SDL's audio and video subsystems
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Set up desired audio specification
    SDL_AudioSpec desired;
    SDL_zero(desired);
    desired.freq = SAMPLE_RATE;
    desired.format = AUDIO_S16SYS; // Signed 16-bit audio
    desired.channels = CHANNELS;
    desired.samples = SAMPLES_PER_BUFFER;
    desired.callback = audio_callback;

    // Open the audio device
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &desired, NULL, 0);
    if (deviceId == 0) {
        std::cerr << "Failed to open audio device! SDL Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Start playing audio
    SDL_PauseAudioDevice(deviceId, 0);
    
    // Create a simple window for visual feedback
    SDL_Window* window = SDL_CreateWindow("Chiptune Piano", 
                                          SDL_WINDOWPOS_UNDEFINED, 
                                          SDL_WINDOWPOS_UNDEFINED, 
                                          800, 600, 
                                          SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        SDL_CloseAudioDevice(deviceId);
        SDL_Quit();
        return 1;
    }
    
    std::cout << "Chiptune Piano is running. Press keys for notes." << std::endl;
    std::cout << "Keys: A=C, S=D, D=E, F=F, G=G, H=A, J=B, K=C (Octave 5)" << std::endl;
    std::cout << "Press 'Q' to quit." << std::endl;

    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_a: g_frequency = noteToFrequency(60); break; // C4
                    case SDLK_s: g_frequency = noteToFrequency(62); break; // D4
                    case SDLK_d: g_frequency = noteToFrequency(64); break; // E4
                    case SDLK_f: g_frequency = noteToFrequency(65); break; // F4
                    case SDLK_g: g_frequency = noteToFrequency(67); break; // G4
                    case SDLK_h: g_frequency = noteToFrequency(69); break; // A4
                    case SDLK_j: g_frequency = noteToFrequency(71); break; // B4
                    case SDLK_k: g_frequency = noteToFrequency(72); break; // C5
                    case SDLK_q: quit = true; break;
                }
            } else if (event.type == SDL_KEYUP) {
                // Stop the sound when the key is released
                switch (event.key.keysym.sym) {
                    case SDLK_a:
                    case SDLK_s:
                    case SDLK_d:
                    case SDLK_f:
                    case SDLK_g:
                    case SDLK_h:
                    case SDLK_j:
                    case SDLK_k:
                        g_frequency = 0.0;
                        break;
                }
            }
        }
        SDL_Delay(10); // Don't hog the CPU
    }

    // Clean up
    SDL_DestroyWindow(window);
    SDL_CloseAudioDevice(deviceId);
    SDL_Quit();

    return 0;
}
