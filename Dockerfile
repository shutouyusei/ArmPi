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

# 依存パッケージと catkin_tools のインストール
USER root
# catkin_tools (python3-catkin-tools) と、ビルドに必要な依存関係をインストール
RUN apt-get update && apt-get install -y \
    python3-pip \
    python3-catkin-tools \
    git \
    build-essential \
    && rm -rf /var/lib/apt/lists/*


# ユーザーを切り替えて作業
USER rosuser
WORKDIR $ROS_WS

