#!/usr/bin/env python3
import rospy
import torch
from ai_model_service.srv import PredictAction
from ai_model_service.networks.mlp_network import MlpNetwork
from cv_bridge import CvBridge
import cv2
import torchvision.transforms as T
from armpi_operation_msgs.msg import RobotCommand
from ai_model_service.srv import PredictActionResponse


class InferenceServer:
    def __init__(self, model_path):
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        self.action_columns = [
            "chassis_move_forward",
            "chassis_move_right",
            "angular_right",
            "arm_x",
            "arm_y",
            "arm_z",
            "arm_alpha",
            "rotation",
            "gripper_close",
        ]
        self.state_columns = [
            "joint1_pos",
            "joint2_pos",
            "joint3_pos",
            "joint4_pos",
            "joint5_pos",
            "r_joint_pos",
        ]

        try:
            self.model = torch.load(model_path, map_location=self.device)
            self.model.to(self.device)
            self.model.eval()
        except FileNotFoundError:
            print(f"ERROR:FileNotFoundError {model_path}")
        except Exception as e:
            print(f"ERROR: {e}")

        self.bridge = CvBridge()

        self.image_transform = T.Compose(
            [
                T.ToPILImage(),
                T.Resize((224, 224)),
                T.ToTensor(),
                T.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]),
            ]
        )
        self.service = rospy.Service(
            "predict_action", PredictAction, self.handle_predict_request
        )
        rospy.loginfo("推論サービス (predict_action) 準備完了。")

    def preprocess_image(self, image_msg):
        cv_image = self.bridge.imgmsg_to_cv2(image_msg, desired_encoding="bgr8")
        cv_image_rgb = cv2.cvtColor(cv_image, cv2.COLOR_BGR2RGB)
        image_tensor = self.image_transform(cv_image_rgb)
        return image_tensor

    def preprocess_state(self, joint_state_msg):
        state_dict = dict(zip(joint_state_msg.name, joint_state_msg.position))
        state_vector = []
        for col_name in self.state_columns:
            joint_name = col_name.replace("_pos", "")  # 'joint1_pos' -> 'joint1'
            if joint_name not in state_dict:
                raise ValueError(f"JointStateに {joint_name} が見つかりません")
            state_vector.append(state_dict[joint_name])

        return torch.tensor(state_vector, dtype=torch.float32)

    def actions_to_ros_msg(self, predicted_actions):
        cmd_msg = RobotCommand()
        cmd_msg.chassis_move_forward = predicted_actions[0]
        cmd_msg.chassis_move_right = predicted_actions[1]
        cmd_msg.angular_right = predicted_actions[2]
        cmd_msg.arm_x = predicted_actions[3]
        cmd_msg.arm_y = predicted_actions[4]
        cmd_msg.arm_z = predicted_actions[5]
        cmd_msg.arm_alpha = predicted_actions[6]
        cmd_msg.rotation = predicted_actions[7]
        cmd_msg.gripper_close = predicted_actions[8]

        return cmd_msg

    def handle_predict_request(self, req):
        try:
            image_tensor = self.preprocess_image(req.current_image)
            state_tensor = self.preprocess_state(req.current_joint_state)

            with torch.no_grad():
                image_batch = image_tensor.unsqueeze(0).to(self.device)
                state_batch = state_tensor.unsqueeze(0).to(self.device)

                predicted_actions_logits = self.model(image_batch, state_batch)
                predicted_indicies = torch.argmax(predicted_actions_logits, dim=1)
                predicted_actions_tensor = predicted_indicies - 1

                predicted_actions = predicted_actions_tensor.squeeze(0).cpu().numpy()

            res = PredictActionResponse()
            cmd_msg = self.actions_to_ros_msg(predicted_actions)
            res.predicted_command = cmd_msg

            return res

        except Exception as e:
            rospy.logerr(f"推論リクエストの処理中にエラー: {e}")
            return None


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--model_name', type=str,required=True, help='name of AI model')
    args = parser.parse_args()
    rospy.init_node("imitation_service_server")
    server = InferenceServer(f"home/rosuser/ros_ws/models/{args.model_path}")
    rospy.spin()
