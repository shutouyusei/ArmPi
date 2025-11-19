#include <armpi_servo/ArmpiServo.h>

// --- コンストラクタとデストラクタ ---

ArmpiServo::ArmpiServo(ros::NodeHandle &nh) : nh_(nh), ik_service_name_("compute_arm_ik_and_move") {
  reset();
  ik_client_ = nh_.serviceClient<armpi_servo::ComputeArmIK>(ik_service_name_);
  sub_ = nh_.subscribe("reset_servo", 1, &ArmpiServo::requestReset, this);

  waitForService();
}

ArmpiServo::~ArmpiServo() { 
  sub_.shutdown();
  ROS_INFO("ArmpiServo shutting down."); 
}

void ArmpiServo::reset(){
  current_armpos = {0.0, 0.2, 0.1, -120.0, 500.0,200.0};
}

void ArmpiServo::requestReset(const std_msgs::Empty::ConstPtr &msg) {
  ROS_INFO("Reset Servo");
  reset();
};

void ArmpiServo::waitForService() {
  ROS_INFO("Waiting for IK Action Service: %s...", ik_service_name_.c_str());
  // サービスの存在が確認されるまで待機
  ik_client_.waitForExistence();
  ROS_INFO("IK Service ready.");
}

bool ArmpiServo::requestArmMove(const ArmCommand &command) {

  ArmPos armpos = calArmPos(command);
  armpi_servo::ComputeArmIK srv;

  // サービスの入力 (Request) を設定
  srv.request.x = armpos.x;
  srv.request.y = armpos.y;
  srv.request.z = armpos.z;
  srv.request.alpha = armpos.alpha;
  srv.request.alpha1 = armpos.alpha - 40;
  srv.request.alpha2 = armpos.alpha + 40;
  srv.request.rotation = armpos.rotation;
  srv.request.gripper = armpos.gripper;

  // サービスを呼び出し
  if (ik_client_.call(srv)) {
    if (srv.response.success) {
      current_armpos = armpos;
      return true;
    } else {
      ROS_WARN("Move failed: IK found no solution.");
      return false;
    }
  } else {
    ROS_ERROR("Failed to call IK service. Check network/service server.");
    return false;
  }
  return false;
}

ArmPos ArmpiServo::calArmPos(const ArmCommand &command) {
  ArmPos armpos;
  armpos.x = move(command.arm_x) + current_armpos.x;
  armpos.y = move(command.arm_y) + current_armpos.y;
  armpos.z = move(command.arm_z) + current_armpos.z;

  armpos.gripper = grab(command.gripper_close) + current_armpos.gripper;

  armpos.alpha = alpha(command.arm_alpha) + current_armpos.alpha;

  armpos.rotation= rotation(command.rotation) + current_armpos.rotation;
  return armpos;
}

float ArmpiServo::move(const int arm) {
  return arm * IK_STEP;
}

float ArmpiServo::grab(const int gripper_close) {
  return gripper_close * GRIPPER_STEP;
}

float  ArmpiServo::alpha(const int alpha){
  return alpha * ALPHA_STEP;
}
float ArmpiServo::rotation(const int rotation){
  return rotation * ROTATION_STEP;
}
