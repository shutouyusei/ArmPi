# ArmPi — Imitation Learning for Robot Arm Control

## Project Overview
Imitation learning system for HiWonder ArmPi robot arm. Collects human demonstrations via keyboard, trains CNN+MLP models, and deploys for autonomous control.

## Tech Stack
- **Robotics**: ROS 1 (Noetic), Inverse Kinematics
- **Languages**: C++17 (control), Python 3.10 (ML/inference)
- **ML**: PyTorch, TorchVision, Diffusion Policy
- **Data**: HDF5, Pandas, OpenCV
- **Infra**: Docker, Conda, SDL2

## Repository Structure
- `ros/armpi/` — Low-level robot control (servo, chassis, IK)
- `ros/myapp/armpi_controller/` — Controller abstraction (keyboard / AI)
- `ros/myapp/collect_data/` — Data collection from human demonstrations
- `ros/myapp/ai_model_service/` — ML inference ROS service
- `ros/share/armpi_operation_msgs/` — Custom ROS messages
- `scripts/convert/` — ROS bag to HDF5 conversion
- `scripts/` — Utilities (video creation, docker, rsync)

## GitHub
- Repository: shutouyusei/ArmPi
- Branches: `main`, `collect_play_data` (periodic collection feature)

## Rules for Claude
- All commit messages in English, format: `<type>: <description>`
- Types: feat, fix, refactor, docs, test, chore
- One logical change per commit
- Do not modify C++ servo control code without explicit instruction
- Do not modify submodule references without explicit instruction
- Run `catkin_make` inside Docker if build verification is needed
