# ベースイメージとして ROS Noetic の最小構成を使用
FROM ros:noetic-ros-core

# 環境変数を設定（後のユーザ設定に利用）
ENV HOME="/home/rosuser"
ENV ROS_WS="/home/rosuser/ros_ws"

# ユーザーの作成と権限設定
RUN useradd -m rosuser && \
    echo "rosuser ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/rosuser && \
    chmod 0440 /etc/sudoers.d/rosuser

# ROSの環境を読み込む
RUN echo "source /opt/ros/noetic/setup.bash" >> /home/rosuser/.bashrc && \
  echo "sudo chown -R rosuser:rosuser $ROS_WS" >> /home/rosuser/.bashrc && \
  echo "pip install ./src/myapp/ai_model_service/src/ai_modules/third_party/diffusion" >> /home/rosuser/.bashrc

# 依存パッケージと catkin_tools のインストール
USER root
# catkin_tools (python3-catkin-tools) と、ビルドに必要な依存関係をインストール
RUN apt-get update && apt-get install -y \
    python3-pip \
    python3-catkin-tools \
    git \
    build-essential \
    ros-noetic-image-transport\
    ros-noetic-cv-bridge\
    libsdl2-dev \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install --upgrade pip


RUN pip3 install \
    torch==2.1.0 \
    torchvision==0.16.0 \
    opencv-python \
    ipython\
    packaging\
    robomimic==0.2.0\
    diffusers \
    einops \
    zarr \
    omegaconf

# ユーザーを切り替えて作業
USER rosuser
WORKDIR $ROS_WS
