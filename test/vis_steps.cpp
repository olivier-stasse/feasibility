#include <ros/ros.h>
#include <ros/time.h>
#include <std_msgs/String.h>
#include <visualization_msgs/Marker.h>
#include <vector>
#include <iostream>

#include <fcl/shape/geometric_shapes.h>
#include <fcl/math/vec_3f.h>
#include <fcl/BVH/BVH_model.h>

#include <fast-replanning/fast-replanning-interface.hh>

#include "util/util.h"
#include "planner/trajectory_visualizer.h"
#include "rviz/visualmarker.h"

int main( int argc, char** argv )
{
	using namespace ros;
	ros::init(argc, argv, "footstep_visualizer2");
	ros::NodeHandle n;
	ros::Rate r(1);

	std::string prefix = get_data_path();
	std::string chair_file = get_chair_str();
	std::string robot_file = get_robot_str();

	//######################################################
	// set robot start
	//######################################################
	std::vector<double> start;
	start.push_back(0.0);
	start.push_back(0.0);

	double cx=0.49,cy=0.05,cz=0.0;
	double rx=1.0,ry=0.00,rz=0.0;

	//######################################################
	// fastPlanner
	//######################################################
	//planner->addObstacleFromDatabase(CHAIR, cx, cy, cz, 1,1,1); 
	//planner->addObstacleFromDatabase(BOX, 0.49, 0.21, 0, 1,1,1); 
	//planner->updateLocalizationProtected(start);

	//######################################################
	// set goal 
	//######################################################
	std::vector<double> goal;
	goal.push_back(2.5); goal.push_back(0.0); goal.push_back(0.0);

	TrajectoryVisualizer *tv = new TrajectoryVisualizer();
	CSVReader data_q("playback_q.dat");
	std::vector<double> q = data_q.getV();
	ROS_INFO("q size: %d", q.size());

	r.sleep();

	while (ros::ok())
	{
		SphereMarker ss(start.at(0), start.at(1));
		ss.publish();

		SphereMarker sm(goal.at(0), goal.at(1));
		sm.publish();

		ros::RVIZVisualMarker *c;
		c = new ros::ColladaObject("package://feasibility/data/wall_laas8.obj");
		c->setXYZ(1.5,-2.5,-0.01);
		c->setRPYRadian(0,0,M_PI);
		c->publish();
		
		CSVReader data_steps("playback_steps.dat");
		
		std::vector<std::vector<double> > fsi;
		fsi = data_steps.getVV(4);


		double last_xL = 0;
		double last_yL = 0;
		double last_tL = 0;
		double last_xR = 0;
		double last_yR = 0;
		double last_tR = 0;

		double xold = 0;
		double yold = 0;
		double told = 0;
		//xold = fsi.at(0).at(0);
		//yold = fsi.at(0).at(1);
		//told = fsi.at(0).at(2);


		for(uint i=0;i<fsi.size();i++){
			//half-foot-step-format v.3.0:
			// 1: x
			// 2: y
			// 3: theta
			// 4: ascii code for L or R
			// 5-11: do not know
			printf("[%d] ", i);
			for(uint j=0;j<fsi.at(i).size();j++){
				printf("%f ",fsi.at(i).at(j));
			}
			printf("\n");
			double scale=1;
			double x,y,t;

//double radV = -state_prev->x[2]*PI/180.0;
//s.x = cos(radV)*(state_curr->x[0] - state_prev->x[0]) - 
//  sin(radV)*(state_curr->x[1] - state_prev->x[1]);
//s.y = sin(radV)*(state_curr->x[0] - state_prev->x[0]) + 
//  cos(radV)*(state_curr->x[1] - state_prev->x[1]);
//s.theta = (state_curr->x[2] - state_prev->x[2])*PI/180.0;
//s.f = state_prev->x[3];
//tempVector.at(i).abs_x = abs_x + cos(abs_theta)*x - sin(abs_theta)*y;        
//tempVector.at(i).abs_y = abs_y + sin(abs_theta)*x + cos(abs_theta)*y;
//tempVector.at(i).abs_theta = abs_theta + theta;

			x = fsi.at(i).at(0);
			y = fsi.at(i).at(1);
			t = fsi.at(i).at(2);
			double abs_x = xold;
			double abs_y = yold;
			double abs_t = told;
			char foot = fsi.at(i).at(3);
//			double radV = -told * M_PI  / 180.0;
//			double newX = cos(radV)*(x-xold) - sin(radV)*(y-yold);
//			double newY = sin(radV)*(x-xold) + cos(radV)*(y-yold);
//			double newT = (t-told)*M_PI/180.0;

			double newX = abs_x + cos(abs_t)*x-sin(abs_t)*y;
			double newY = abs_y + sin(abs_t)*x+cos(abs_t)*y;
			double newT = abs_t + t;
			FootMarker f(newX,newY,newT);
			xold=newX;
			yold=newY;
			told=newT;

			if(foot == 'R'){
				f.set_color(1,0,0);
				f.drawLine(last_xR,last_yR);
				last_xR = x;
				last_yR = y;
			}else{
				f.set_color(0,1,0);
				f.drawLine(last_xL,last_yL);
				last_xL = x;
				last_yL = y;
			}

			if(i==0 || i==1){
				f.set_color(1,1,0);
			}
			if(i==fsi.size()-1 || i==fsi.size()-2){
				f.set_color(1,0.6,0);
			}

			f.publish();
			ROS_INFO("published footstep [%d] at x=%f, y=%f, theta=%f", i,newX,newY,newT);
		}
		//######################################################

		if(q.size()>0){
			//Replay trajectory
			tv->init(q);
			ros::Rate rq(500);
			while(tv->next()){
				ros::spinOnce();
				rq.sleep();
			}
		}
		r.sleep();

	}
}
