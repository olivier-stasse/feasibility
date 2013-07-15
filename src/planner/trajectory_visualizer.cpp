#include <vector>
#include <ros/ros.h>
#include <ros/time.h>
#include <ros/package.h>
#include <robot_state_publisher/robot_state_publisher.h>
#include <sensor_msgs/JointState.h>
#include <kdl_parser/kdl_parser.hpp>
#include <string>
#include "planner/trajectory_visualizer.h"

void TrajectoryVisualizer::init(std::vector<double> &q){
	this->_q = &q;
	this->_offset=4;
	this->_Nframes = (this->_q->size()-this->_offset)/17.0; //12 joint valzues + 3 CoM (x,y,t) + 2 ZMP (x,y)
	this->_ctrFrames = 0;
}
TrajectoryVisualizer::TrajectoryVisualizer(double x, double y){
	ros::Rate r(10);

	KDL::Tree tree("/base_link");
	urdf::Model my_model;
	ROS_INFO("searching for path of package 'feasibility' ...");
	std::string URDFFilename = ros::package::getPath("feasibility")+"/data/hrp2a.urdf";

	if (!kdl_parser::treeFromFile(URDFFilename, tree))
	{
		ROS_ERROR("Failed to construct kdl tree");
	}
	this->setPlanarWorldBaseTransform(x,y,M_PI/8);
	this->_rsp = new robot_state_publisher::RobotStatePublisher(tree);

	this->reset();
}
void TrajectoryVisualizer::setUpperBodyJointsDefault( std::map<std::string, double> &q ){
	q["RARM_JOINT0"] = 0.0;
	q["LARM_JOINT0"] = 0.0;
	q["RARM_JOINT1"] = -0.1;
	q["LARM_JOINT1"] = -0.1;
	q["RARM_JOINT2"] = 0.0;
	q["LARM_JOINT2"] = 0.0;
	q["RARM_JOINT3"] = -1.0;
	q["LARM_JOINT3"] = -1.0;
	q["RARM_JOINT4"] = 0.0;
	q["LARM_JOINT4"] = 0.0;
	q["RARM_JOINT5"] = -0.3;
	q["LARM_JOINT5"] = -0.3;
	q["RARM_JOINT6"] = 0.0;
	q["LARM_JOINT6"] = 0.0;

	q["RHAND_JOINT0"] = 0.0;
	q["RHAND_JOINT1"] = 0.0;
	q["RHAND_JOINT2"] = 0.0;
	q["RHAND_JOINT3"] = 0.0;
	q["RHAND_JOINT4"] = 0.0;

	q["LHAND_JOINT0"] = 0.0;
	q["LHAND_JOINT1"] = 0.0;
	q["LHAND_JOINT2"] = 0.0;
	q["LHAND_JOINT3"] = 0.0;
	q["LHAND_JOINT4"] = 0.0;


	q["HEAD_JOINT0"] = 0.0;
	q["HEAD_JOINT1"] = 0.3;

	q["CHEST_JOINT0"] = 0.0;
	q["CHEST_JOINT1"] = 0.0;

	//setConstTransform("CHEST_LINK0", "torso");
	//setTranslationTransform("torso", "RARM_LINK0", 0.008, -0.250, 0.181);
}
void TrajectoryVisualizer::reset(){
	std::map<std::string, double> q;
	q["RLEG_JOINT0"] = 0.0;
	q["RLEG_JOINT1"] = 0.0;
	q["RLEG_JOINT2"] = 0.0;
	q["RLEG_JOINT3"] = 0.0;
	q["RLEG_JOINT4"] = 0.0;
	q["RLEG_JOINT5"] = 0.0;

	q["LLEG_JOINT0"] = 0.0;
	q["LLEG_JOINT1"] = 0.0;
	q["LLEG_JOINT2"] = 0.0;
	q["LLEG_JOINT3"] = 0.0;
	q["LLEG_JOINT4"] = 0.0;
	q["LLEG_JOINT5"] = 0.0;

	setUpperBodyJointsDefault(q);

	_rsp->publishFixedTransforms();
	_rsp->publishTransforms(q, ros::Time::now());
}
void TrajectoryVisualizer::rewind(){
	_ctrFrames = 0;
}
bool TrajectoryVisualizer::next(){
	if(_ctrFrames >= _Nframes){
		return false;
	}

	std::map<std::string, double> q;

	q["RLEG_JOINT0"] = _q->at(_offset + _ctrFrames*17 + 0);
	q["RLEG_JOINT1"] = _q->at(_offset + _ctrFrames*17 + 1);
	q["RLEG_JOINT2"] = _q->at(_offset + _ctrFrames*17 + 2);
	q["RLEG_JOINT3"] = _q->at(_offset + _ctrFrames*17 + 3);
	q["RLEG_JOINT4"] = _q->at(_offset + _ctrFrames*17 + 4);
	q["RLEG_JOINT5"] = _q->at(_offset + _ctrFrames*17 + 5);

	q["LLEG_JOINT0"] = _q->at(_offset + _ctrFrames*17 + 6);
	q["LLEG_JOINT1"] = _q->at(_offset + _ctrFrames*17 + 7);
	q["LLEG_JOINT2"] = _q->at(_offset + _ctrFrames*17 + 8);
	q["LLEG_JOINT3"] = _q->at(_offset + _ctrFrames*17 + 9);
	q["LLEG_JOINT4"] = _q->at(_offset + _ctrFrames*17 + 10);
	q["LLEG_JOINT5"] = _q->at(_offset + _ctrFrames*17 + 11);

	setUpperBodyJointsDefault(q);

	_rsp->publishFixedTransforms();
	_rsp->publishTransforms(q, ros::Time::now());

	double CoM[3];
	CoM[0]=_q->at(_offset + _ctrFrames*17 + 12);
	CoM[1]=_q->at(_offset + _ctrFrames*17 + 13);
	CoM[2]=_q->at(_offset + _ctrFrames*17 + 14);

	this->setPlanarWorldBaseTransform(CoM[0], CoM[1], CoM[2]);
	_ctrFrames++;
	return true;
}
std::vector<double> TrajectoryVisualizer::getFinalCoM(){
	std::vector<double> CoM(3);
	CoM.at(0)=_q->at(_offset + (_Nframes-1)*17 + 12);
	CoM.at(1)=_q->at(_offset + (_Nframes-1)*17 + 13);
	CoM.at(2)=_q->at(_offset + (_Nframes-1)*17 + 14);
	return CoM;
}

void TrajectoryVisualizer::setPlanarWorldBaseTransform(double x, double y, double yaw){
	tf::Transform transform;
	transform.setOrigin( tf::Vector3(x,y,-0.05) ); //in frame base_link
	tf::Quaternion com_rot;
	com_rot.setRPY(0,0,yaw);
	transform.setRotation(com_rot);

	_br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "/world_frame", "/base_link"));

}

void TrajectoryVisualizer::setTranslationTransform(const char* from, const char* to, double x, double y, double z){
	tf::Transform transform;
	transform.setOrigin( tf::Vector3(x,y,z) ); //in frame base_link
	tf::Quaternion com_rot;
	com_rot.setRPY(0,0,0);
	transform.setRotation(com_rot);

	_br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), from, to));
}
void TrajectoryVisualizer::setConstTransform(const char* from, const char* to){
	tf::Transform transform;
	transform.setOrigin( tf::Vector3(0,0,0) ); //in frame base_link
	tf::Quaternion com_rot;
	com_rot.setRPY(0,0,0);
	transform.setRotation(com_rot);

	_br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), from, to));
}