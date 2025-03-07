#include "SDL_rwops.h"
#include <SDL.h>
#include <SDL_audio.h>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

class AudioPlayer {
private:
  SDL_AudioSpec wavSpec;
  Uint32 wavLength;
  Uint8 *wavBuffer;
  SDL_AudioDeviceID deviceId;

public:
  AudioPlayer(const char *filename) {
    SDL_Init(SDL_INIT_AUDIO);

    auto exec_path = std::filesystem::current_path();
    auto audio_path = exec_path.parent_path() / filename;

    SDL_RWops *rwops = SDL_RWFromFile(audio_path.string().c_str(), "rb");

    if (!rwops) {
      throw std::runtime_error("Failed to open audio file");
    }

    if (!SDL_LoadWAV_RW(rwops, 1, &wavSpec, &wavBuffer, &wavLength)) {
      throw std::runtime_error("Failed to load audio file");
    }

    deviceId = SDL_OpenAudioDevice(nullptr, 0, &wavSpec, nullptr, 0);
  }

  void play_audio() {
    SDL_QueueAudio(deviceId, wavBuffer, wavLength);
    SDL_PauseAudioDevice(deviceId, 0);
  }

  ~AudioPlayer() {
    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wavBuffer);
    SDL_Quit();
  }
};

class BadApple {
private:
  static constexpr int FRAME_DELAY_MS = 33; // ~30 FPS
  const std::string frames_path;
  std::unique_ptr<AudioPlayer> audio;

  char get_ascii_char(unsigned char pixel_value) {
    const char ascii_chars[] = " .'`^\",:;Il!i><~+_-?][}{1)(|/"
                               "tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
    int index = pixel_value * (sizeof(ascii_chars) - 2) / 255;

    return ascii_chars[index];
  }

  void floydSteingbergDithering(unsigned char *image, int width, int height) {
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        unsigned char oldPixel = image[y * width + x];
        // Quantization from 0 to 255
        unsigned char newPixel = (oldPixel < 128) ? 0 : 255;

        image[y * width + x] = newPixel;
        int error = static_cast<int>(oldPixel) -
                    static_cast<int>(newPixel); // error is me :D

        // Adjusting error on neighborgh pixels
        if (x + 1 < width) {
          image[y * width + x + 1] = std::min(
              255, std::max(0, static_cast<int>(image[y * width + x + 1]) +
                                   error * 7 / 16));
        }

        if (x - 1 >= 0 && y + 1 < height) {
          image[(y + 1) * width + x - 1] = std::min(
              255,
              std::max(0, static_cast<int>(image[(y + 1) * width + x - 1]) +
                              error * 3 / 16));
        }

        if (y + 1 < height) {
          image[(y + 1) * width + x] = std::min(
              255, std::max(0, static_cast<int>(image[(y + 1) * width + x]) +
                                   error * 5 / 16));
        }

        if (x + 1 < width && y + 1 < height) {
          image[(y + 1) * width + x + 1] = std::min(
              255,
              std::max(0, static_cast<int>(image[(y + 1) * width + x + 1]) +
                              error * 1 / 16));
        }
      }
    }
  }

  // Buffer for storing the frame
  std::string render_frame(unsigned char *data, int width, int height) {
    floydSteingbergDithering(data, width, height);

    std::string buffer;
    // Pre-allocate the buffer
    buffer.reserve(height * (width + 1) / 2);

    for (int y = 0; y < height; y += 2) {
      for (int x = 0; x < width; x++) {
        buffer += get_ascii_char(data[y * width + x]);
      }

      buffer += "\n";
    }

    return buffer;
  }

public:
  explicit BadApple(std::string path) : frames_path(std::move(path)) {
    audio = std::make_unique<AudioPlayer>("bad_apple.wav");

    std::cout << "\033[2J";
  }

  void play() {
    auto exec_path = std::filesystem::current_path();
    auto full_frames_path = exec_path.parent_path() / frames_path;
    std::vector<std::filesystem::path> frame_paths;

    for (const auto &entry :
         std::filesystem::directory_iterator(full_frames_path)) {
      if (entry.path().extension() == ".png") {
        frame_paths.push_back(entry.path());
      }
    }

    std::sort(frame_paths.begin(), frame_paths.end());

    audio->play_audio();

    auto start_time = std::chrono::steady_clock::now();

    for (size_t i = 0; i < frame_paths.size(); i++) {
      auto frame_time =
          start_time + std::chrono::milliseconds(i * FRAME_DELAY_MS);

      int width, height, chan;
      unsigned char *data =
          stbi_load(frame_paths[i].string().c_str(), &width, &height, &chan, 1);

      if (!data)
        continue;

      std::cout << "\033[H" << render_frame(data, width, height) << std::flush;
      stbi_image_free(data);

      // Prices frame timing
      std::this_thread::sleep_until(frame_time);
    }
  }
};

int main() {
  BadApple player("frames");
  player.play();
}