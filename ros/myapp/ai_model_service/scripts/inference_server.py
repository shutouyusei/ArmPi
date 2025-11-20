#!/usr/bin/env python3
import rospy
import torch
import cv2
import torchvision.transforms as T
from cv_bridge import CvBridge
from ai_model_service.srv import PredictAction, PredictActionResponse
from armpi_operation_msgs.msg import RobotCommand

# model
from ai_model_service.mlp_model import MlpModel
from ai_model_service.act_model import ActModel

class InferenceServer:
    def __init__(self, model_path):
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        rospy.loginfo(f"Using device: {self.device}")

        loaded_data = torch.load(f"{model_path}.pt"")
        # --- モデルの切り替えロジック ---
        if loaded_data["model_type"] == "mlp":
            self.model_strategy = MlpModel(loaded_data["model"], self.device)
        elif loaded_data["model_type"]== "act":
            self.model_strategy = ActModel(loaded_data["model"], self.device)
        else:
            raise ValueError(f"Unknown model type: {model_type}")
        # -----------------------------

        self.bridge = CvBridge()
        self.init_transforms()
        
        # ROSパラメータや定義
        self.setup_ros_interface()

    def init_transforms(self):
        """画像の前処理定義"""
        self.image_transform = T.Compose([
            T.ToPILImage(),
            T.Resize((224, 224)),
            T.ToTensor(),
            T.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]),
        ])
        self.state_columns = [
            "joint1_pos", "joint2_pos", "joint3_pos", 
            "joint4_pos", "joint5_pos", "r_joint_pos"
        ]

    def setup_ros_interface(self):
        self.service = rospy.Service(
            "predict_action", PredictAction, self.handle_predict_request
        )
        rospy.loginfo("推論サービス準備完了")

    def preprocess_image(self, image_msg):
        cv_image = self.bridge.imgmsg_to_cv2(image_msg, desired_encoding="bgr8")
        cv_image_rgb = cv2.cvtColor(cv_image, cv2.COLOR_BGR2RGB)
        # Actモデル用に次元調整が必要ならここで行うか、Strategy内で行う
        image_tensor = self.image_transform(cv_image_rgb)
        
        # グレースケール画像のバグ対応（前回の議論より）
        if image_tensor.shape[0] == 1:
             image_tensor = image_tensor.repeat(3, 1, 1)
             
        return image_tensor

    def preprocess_state(self, joint_state_msg):
        state_dict = dict(zip(joint_state_msg.name, joint_state_msg.position))
        state_vector = []
        for col_name in self.state_columns:
            joint_name = col_name.replace("_pos", "")
            if joint_name not in state_dict:
                # 安全のため0埋めやエラー処理
                rospy.logwarn(f"{joint_name} not found in joint states")
                state_vector.append(0.0) 
            else:
                state_vector.append(state_dict[joint_name])
        return torch.tensor(state_vector, dtype=torch.float32)

    def actions_to_ros_msg(self, action_values):
        """数値リストをROSメッセージに変換"""
        cmd_msg = RobotCommand()
        # アクションの次元数や順序がモデルによって変わらない前提
        # もし変わるなら、このマッピングもStrategyクラスに移動すべきです
        keys = [
            "chassis_move_forward", "chassis_move_right", "angular_right",
            "arm_x", "arm_y", "arm_z", "arm_alpha", "rotation", "gripper_close"
        ]
        for i, key in enumerate(keys):
            if i < len(action_values):
                setattr(cmd_msg, key, float(action_values[i]))
        return cmd_msg

    def handle_predict_request(self, req):
        try:
            image_tensor = self.preprocess_image(req.current_image)
            state_tensor = self.preprocess_state(req.current_joint_state)

            predicted_actions = self.model_strategy.predict(image_tensor, state_tensor)

            res = PredictActionResponse()
            res.predicted_command = self.actions_to_ros_msg(predicted_actions)
            return res

        except Exception as e:
            rospy.logerr(f"推論エラー: {e}")
            return None

if __name__ == "__main__":
    import argparse
    import os
    import sys

    argv = rospy.myargv(argv=sys.argv)
    
    parser = argparse.ArgumentParser()
    parser.add_argument('--model_name', type=str, required=True, help='ファイル名 (例: model.pt)')
    args = parser.parse_args(argv[1:])

    rospy.init_node("imitation_service_server")
    
    home_dir = os.path.expanduser("~") 
    model_path = os.path.join(home_dir, "ros_ws/models", args.model_name)
    rospy.loginfo(f"Using model: {model_path}")
    
    server = InferenceServer(model_path)
    rospy.spin()
