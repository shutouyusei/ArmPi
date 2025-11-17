#include <armpi_controller/keyboard/SDLHandler.h>

SDLHandler::SDLHandler() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    ROS_FATAL("Failed initializing SDL: %s", SDL_GetError());
    ros::shutdown();
    return;
  }

  window_ = SDL_CreateWindow("ArmPi Key Controller (Active this window)",
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             300, 50, // ウィンドウサイズ (任意)
                             SDL_WINDOW_SHOWN);

  if (window_ == nullptr) {
    ROS_FATAL("Failed to create SDL window: %s", SDL_GetError());
    ros::shutdown();
  }
}

SDLHandler::~SDLHandler() {
  if (window_) {
    SDL_DestroyWindow(window_);
    window_ = nullptr;
  }
  SDL_Quit();
}


bool SDLHandler::is_pressed(SDL_Scancode code) {
  return key_states_.count(code) && key_states_.at(code);
}

bool SDLHandler::is_pressed_1time(SDL_Scancode code){
  return key_states_.count(code) && key_states_.at(code) && !(prev_key_states_.count(code) && prev_key_states_.at(code));
}

void SDLHandler::pollSDLEvents() {
  SDL_Event event;
  prev_key_states_ = key_states_;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_KEYDOWN:
        key_states_[event.key.keysym.scancode] = true;
        break;
      case SDL_KEYUP:
        key_states_[event.key.keysym.scancode] = false;
        break;
      case SDL_QUIT:
        ros::shutdown();
        break;
    }
  }
}
