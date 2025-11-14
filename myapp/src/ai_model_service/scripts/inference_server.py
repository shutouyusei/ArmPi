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
        self.model = MlpNetwork(state_input_dim=6, action_output_dim=14)
        try:
            self.model.load_state_dict(torch.load(model_path, map_location=self.device,weights_only=True))
            self.model.to(self.device)
            self.model.eval()
        except FileNotFoundError:
            print(f"ERROR:FileNotFoundError {model_path}")
        except Exception as e:
            print(f"ERROR: {e}")

        self.bridge = CvBridge()
        self.state_columns = ['joint1_pos', 'joint2_pos', 'joint3_pos', 'joint4_pos', 'joint5_pos', 'r_joint_pos']
        self.image_transform = T.Compose([
            T.ToPILImage(),
            T.Resize((224, 224)),
            T.ToTensor(),
            T.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])
        ])
        self.service = rospy.Service("predict_action", PredictAction, self.handle_predict_request)
        rospy.loginfo("推論サービス (predict_action) 準備完了。")

    def preprocess_image(self,image_msg):
        cv_image = self.bridge.imgmsg_to_cv2(image_msg, desired_encoding="bgr8")
        cv_image_rgb = cv2.cvtColor(cv_image, cv2.COLOR_BGR2RGB)
        image_tensor = self.image_transform(cv_image_rgb)
        return image_tensor

    def preprocess_state(self, joint_state_msg):
            state_dict = dict(zip(joint_state_msg.name, joint_state_msg.position))
            state_vector = []
            for col_name in self.state_columns:
                joint_name = col_name.replace('_pos', '') # 'joint1_pos' -> 'joint1'
                if joint_name not in state_dict:
                    raise ValueError(f"JointStateに {joint_name} が見つかりません")
                state_vector.append(state_dict[joint_name])
                
            return torch.tensor(state_vector, dtype=torch.float32)

    def actions_to_ros_msg(self, predicted_actions):
        cmd_msg = RobotCommand()
        cmd_msg.base_velocity.linear.x = predicted_actions[0]
        cmd_msg.base_velocity.linear.y = predicted_actions[1]
        cmd_msg.base_velocity.linear.z = predicted_actions[2]
        cmd_msg.base_velocity.angular.x = predicted_actions[3]
        cmd_msg.base_velocity.angular.y = predicted_actions[4]
        cmd_msg.base_velocity.angular.z = predicted_actions[5]
        cmd_msg.arm_x = predicted_actions[6]
        cmd_msg.arm_y = predicted_actions[7]
        cmd_msg.arm_z = predicted_actions[8]
        cmd_msg.gripper = predicted_actions[9]
        cmd_msg.arm_alpha = predicted_actions[10]
        cmd_msg.arm_alpha1 = predicted_actions[11]
        cmd_msg.arm_alpha2 = predicted_actions[12]
        cmd_msg.gripper_position = predicted_actions[13]
        
        return cmd_msg

    def handle_predict_request(self, req):
        try:
            image_tensor = self.preprocess_image(req.current_image)
            state_tensor = self.preprocess_state(req.current_joint_state)
            
            with torch.no_grad():
                image_batch = image_tensor.unsqueeze(0).to(self.device)
                state_batch = state_tensor.unsqueeze(0).to(self.device)
                
                predicted_actions_tensor = self.model(image_batch, state_batch)
                predicted_actions = predicted_actions_tensor.squeeze(0).cpu().numpy()

            res = PredictActionResponse()
            cmd_msg = self.actions_to_ros_msg(predicted_actions)
            res.predicted_command = cmd_msg
            
            return res

        except Exception as e:
            rospy.logerr(f"推論リクエストの処理中にエラー: {e}")
            return None

if __name__ == '__main__':
    rospy.init_node("imitation_service_server")
    MODEL_FILE = "./src/ai_model_service/model/model.pt"
    server = InferenceServer(MODEL_FILE)
    rospy.spin()
