#include <armpi_servo/ArmpiServo.h>

// --- コンストラクタとデストラクタ ---

ArmpiServo::ArmpiServo(ros::NodeHandle& nh)
    : nh_(nh), ik_service_name_("compute_arm_ik_and_move")
{
    // サービス接続クライアントの初期化
    ik_client_ = nh_.serviceClient<armpi_servo::ComputeArmIK>(ik_service_name_);
    // サービスが起動するまで待機
    waitForService();
}

ArmpiServo::~ArmpiServo() {
    ROS_INFO("ArmpiServo shutting down.");
}

// --- プライベートメソッド ---

void ArmpiServo::waitForService() {
    ROS_INFO("Waiting for IK Action Service: %s...", ik_service_name_.c_str());
    // サービスの存在が確認されるまで待機
    ik_client_.waitForExistence();
    ROS_INFO("IK Service ready.");
}


bool ArmpiServo::requestArmMove(double x, double y, double z, double alpha, double alpha1, double alpha2) 
{
    // サービスが利用可能か再確認 (なくても通常はwaitForExistence()で十分だが念のため)
    if (!ik_client_.exists()) {
        ROS_ERROR("IK Service is not available.");
        waitForService(); // 再度待機を試みる
        return false;
    }

    armpi_servo::ComputeArmIK srv;
    
    // サービスの入力 (Request) を設定
    srv.request.x = x;
    srv.request.y = y;
    srv.request.z = z;
    srv.request.alpha = alpha;
    srv.request.alpha1 = alpha1;
    srv.request.alpha2 = alpha2;
    
    ROS_INFO("Calling IK service for (%.2f, %.2f, %.2f)...", x, y, z);
    
    // サービスを呼び出し
    if (ik_client_.call(srv)) {
        if (srv.response.success) {
            ROS_INFO("Move successful! S3=%d, S4=%d", srv.response.servo3, srv.response.servo4);
            return true;
        } else {
            ROS_WARN("Move failed: IK found no solution.");
            return false;
        }
    } else {
        ROS_ERROR("Failed to call IK service. Check network/service server.");
        return false;
    }
}
