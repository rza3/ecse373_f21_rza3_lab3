#include "ros/ros.h"
#include "std_msgs/String.h"

#include <sstream>
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/LaserScan.h"

geometry_msgs::Twist des;
sensor_msgs::LaserScan laserInput;
/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
int awake = 0;
void des_vel_Callback(const geometry_msgs::Twist::ConstPtr& msg)
{
  des = *msg;
  ROS_INFO("I heard the linear x is: [%2.2f]", msg->linear.x);
}
void laser_Callback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
  laserInput = *msg;
  awake = 1;
  ROS_INFO("I heard: [%s]", msg->header.frame_id.c_str());
  ROS_WARN("At index 134, x range is: [%2.2f]", msg->ranges[134]);
}

int main(int argc, char **argv)
{
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "talker");

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;

  /**
   * The advertise() function is how you tell ROS that you want to
   * publish on a given topic name. This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing. After this advertise() call is made, the master
   * node will notify anyone who is trying to subscribe to this topic name,
   * and they will in turn negotiate a peer-to-peer connection with this
   * node.  advertise() returns a Publisher object which allows you to
   * publish messages on that topic through a call to publish().  Once
   * all copies of the returned Publisher object are destroyed, the topic
   * will be automatically unadvertised.
   *
   * The second parameter to advertise() is the size of the message queue
   * used for publishing messages.  If messages are published more quickly
   * than we can send them, the number here specifies how many messages to
   * buffer up before throwing some away.
   */
  ros::Publisher chatter_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000);
  ros::Subscriber sub1 = n.subscribe("des_vel",1000,des_vel_Callback);
  ros::Subscriber sub2 = n.subscribe("laser_1",1000,laser_Callback);
  ros::Rate loop_rate(10);

  /**
   * A count of how many messages we have sent. This is used to create
   * a unique string for each message.
   */
  geometry_msgs::Twist cmd; 
  while(!awake && ros::ok()){
  	ROS_WARN("No robot yet!!");
	ros::spinOnce();
	loop_rate.sleep();
  }
  int count = 0;
  bool nearedWall = 0;
  while (ros::ok())
  {
    cmd = des;
    if(nearedWall){
    	if(laserInput.ranges[134]<1.5){
		cmd.linear.x = -1;
		cmd.angular.z = 0.5;
		ROS_WARN("Getting out");
        }
        else
        	nearedWall = 0;
    }
    if(laserInput.ranges[134]<1){
        ROS_WARN("About to run into a wall, backing up!");
	cmd.linear.x = -0.5;
        cmd.angular.z = 0.5;
	nearedWall = 1;
    }
   
    for(int i = 0; i<270; i++){
	if(i!=134 && laserInput.ranges[i]<0.5){
		cmd.linear.x = 0;
		cmd.angular.z = 0.5;
		ROS_WARN("Something's near the wall!");
	}
    }
    /**
     * This is a message object. You stuff it with data, and then publish it.
     */
    /*std_msgs::String msg;
    std::stringstream ss;
    ss << "hello world " << count;
    msg.data = ss.str();
    ROS_INFO("%s", msg.data.c_str());*/

    /**
     * The publish() function is how you send messages. The parameter
     * is the message object. The type of this object must agree with the type
     * given as a template parameter to the advertise<>() call, as was done
     * in the constructor above.
     */
    
    chatter_pub.publish(cmd);

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
  }


  return 0;
}
