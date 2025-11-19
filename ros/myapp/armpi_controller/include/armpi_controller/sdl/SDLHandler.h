#pragma once
#include <ros/ros.h>
#include <SDL2/SDL.h>
#include <map>

class SDLHandler {
public:
  SDLHandler();
  ~SDLHandler();
  bool is_pressed(SDL_Scancode code);
  bool is_pressed_1time(SDL_Scancode code);
  void pollSDLEvents();
private:
  SDL_Window* window_ = nullptr;
  std::map<SDL_Scancode, bool> key_states_;
  std::map<SDL_Scancode, bool> prev_key_states_;
};
