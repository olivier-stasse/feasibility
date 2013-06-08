#pragma once
#include <vector>
#ifdef PQP_COLLISION_CHECKING
#include <pqp/PQP.h>
#endif
#include "rviz/rviz_visualmarker.h"

struct MResults{
	uint iterations;
	double time;
	uint steps;
	unsigned long long feasibilityChecks; //guaranteed bit sizes (at least): long (32 bit), long long (64 bit)
	bool success;
	void print(){
		std::cout << "success: " << success << ", steps: " << steps << ", iterations: " << iterations << ", time: " << time << ", feasibilityChecks: " << feasibilityChecks << std::endl;
	}
};
struct MotionPlanner{
protected:
	static Environment *environment;
	ros::Geometry goal;
	MResults results;
	

public:
	MotionPlanner(Environment *env){
		if(environment!=NULL){
			environment=NULL; //only delete pointer not object, so that we can handle multiple environments in the main loop
		}
		environment = env;
	}
	virtual ~MotionPlanner(){}
	

	void plan(){
		if(environment->isChanged()){
			ros::Geometry goalG = environment->getGoal();
			setGoal( goalG );
			ros::Geometry start = environment->getStart();
			setStart( start );

			std::vector<ros::RVIZVisualMarker*> objects = environment->getObjects();
			std::vector<ros::RVIZVisualMarker*>::iterator it;
			for(it=objects.begin(); it!=objects.end(); it++){
				addObjectToPlanner(*it);
			}
		}
		ROS_INFO("start planner");
		start_planner();
	}

	virtual void publish() = 0;
	virtual bool success() = 0;
	virtual MResults getResults(){
		return results;
	}
protected:
	virtual void start_planner() = 0;
	virtual void setGoal( ros::Geometry &goal ) = 0;
	virtual void setStart( ros::Geometry &start ) = 0;
	virtual void addObjectToPlanner(ros::RVIZVisualMarker *m) = 0;

};
Environment *MotionPlanner::environment;