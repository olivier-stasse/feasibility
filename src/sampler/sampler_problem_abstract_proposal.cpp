#include "sampler_problem_abstract.h"
#include "util/util.h"

Eigen::VectorXd Proposal::operator()( Eigen::VectorXd &x ){
	Eigen::VectorXd q(x.size());
	for(int i=0;i<x.size();i++){
		q(i) = randn(x(i), q_stddev(i));

		//lets assume that limits are rarely visited, such that
		//the stationary distribution will not be affected by
		//reseting samples under the limits
		if(q(i)<q_constraints_low(i)) q(i)=q_constraints_low(i);
		if(q(i)>q_constraints_high(i)) q(i)=q_constraints_high(i);
	}
	return q;
}
Eigen::VectorXd Proposal::init(){
	return init_middle();
}
Eigen::VectorXd Proposal::init_rand(){
	Eigen::VectorXd x(q_stddev.size());
	for(int i=0;i<q_stddev.size();i++){
		x(i) = rand( q_constraints_low(i), q_constraints_high(i) );
	}
	return x;
}
Eigen::VectorXd Proposal::init_middle(){
	Eigen::VectorXd x(q_stddev.size());
	for(int i=0;i<q_stddev.size();i++){
		x(i) = q_constraints_low(i) + (q_constraints_high(i) -  q_constraints_low(i) )/2.0;
	}
	return x;
}
