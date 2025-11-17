#include <armpi_servo/ArmpiServo.h>

// --- コンストラクタとデストラクタ ---

ArmpiServo::ArmpiServo(ros::NodeHandle &nh) : nh_(nh), ik_service_name_("compute_arm_ik_and_move") {
  reset();
  ik_client_ = nh_.serviceClient<armpi_servo::ComputeArmIK>(ik_service_name_);
  // サービスが起動するまで待機
  waitForService();
}

ArmpiServo::~ArmpiServo() { ROS_INFO("ArmpiServo shutting down."); }


void ArmpiServo::reset(){
  current_armpos = {0.0, 0.12, 0.15, -90.0, -180.0, 0.0, 200.0};
};
// --- プライベートメソッド ---

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
  srv.request.alpha1 = armpos.alpha1;
  srv.request.alpha2 = armpos.alpha2;
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
  armpos.x = move(command.arm_x);
  armpos.y = move(command.arm_y);
  armpos.z = move(command.arm_z);

  if (-0.3 <= current_armpos.x + armpos.x && current_armpos.x + armpos.x <= 0.3) {
    armpos.x += current_armpos.x;
  }else{
    armpos.x = current_armpos.x;
  }

  if (-0.1 <= current_armpos.y + armpos.y && current_armpos.y + armpos.y <= 0.3) {
    armpos.y += current_armpos.y;
  }else{
    armpos.y = current_armpos.y;
  }

  if (-0.1 <= current_armpos.z + armpos.z && current_armpos.z + armpos.z <= 0.3) {
    armpos.z += current_armpos.z;
  }else{
    armpos.z = current_armpos.z;
  }

  armpos.gripper = grab(command.gripper_close);
  if (0 <= current_armpos.gripper + armpos.gripper && current_armpos.gripper + armpos.gripper <= 600) {
    armpos.gripper += current_armpos.gripper;
  }else{
    armpos.gripper = current_armpos.gripper;
  }
  return armpos;
}

float ArmpiServo::move(const int arm) {
  return arm * IK_STEP;
}

float ArmpiServo::grab(const int gripper_close) {
  return gripper_close * GRIPPER_STEP;
}
