#pragma once
#include <vector>
#include <robot_state_publisher/robot_state_publisher.h>

// We are using frames only in the form of the Perrin Fast Footstep
// Planner
//
// q vector is filled like this:
//
//q[0]=ID
//q[1]=end of trajectory
//q[2]=time start
//q[3]=time end
//--- 17 values per frame:
//q[4:15]=q values (12 values)
//q[16]=comX
//q[17]=comY
//q[18]=waistOrient in radian
//q[19]=zmpX
//q[20]=zmpY
//
//the next starting points for frames i=1:T
//-> q[4 + i*17 + k], k=0:16;

class TrajectoryVisualizer{
private:
	uint _Nframes;
	uint _ctrFrames;
	uint _offset;
	double _com_offset_x;
	double _com_offset_y;
	double _com_offset_t;

	double _cur_com_offset_x;
	double _cur_com_offset_y;
	double _cur_com_offset_t;

	std::vector<double> *_q;
	robot_state_publisher::RobotStatePublisher *_rsp;
	tf::TransformBroadcaster _br;
public:
	void init(std::vector<double> &q);
	TrajectoryVisualizer(double x=0, double y=0, double t=0);
	std::vector<double> getFinalCoM();
	void setUpperBodyJointsDefault( std::map<std::string, double> &q );
	void setCoMOffset(double cur_com_x, double cur_com_y, double cur_com_t);
	void setCoMOffset(std::vector<double> com);

	void setConstTransform(const char* from, const char* to);
	void setPlanarWorldBaseTransform(double x, double y, double yaw);
	void setTranslationTransform(const char* from, const char* to, double x, double y, double z);
	void reset();
	void rewind();
	bool next();
};
