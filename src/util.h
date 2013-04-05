#pragma once
#include <ostream>

#define CUR_LOCATION "@" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ">>"

//see also UTIL::cout for special outputs

#define PRINT(msg) std::cout << CUR_LOCATION << " >> " << msg << std::endl
#define ABORT(msg) PRINT(msg); throw msg;
#define HALT(msg) PRINT(msg); exit(1);
#define COUT(msg) PRINT(msg);
#define CHECK(cond, str) if(!(cond)){ PRINT(str); throw(str); }

//##########################################################################
// stable utility functions (misc)
//##########################################################################

FILE *fopen_s(const char *path, const char *mode);
void std_seed();
double rand(double lowerLimit, double upperLimit);
double randn(double m, double stddev);
double randn_boxmuller(double m, double stddev);
double normpdf(double x, double mu=0.0, double s=1.0);
double dist(double x1, double x2, double y1, double y2);
int hashit(const char *str);

template<typename T>
int hashit(std::vector<T> in);

std::string get_data_path();
std::string get_tris_str(std::string relative_path);
std::string get_robot_str();
std::string get_chair_str();
std::string get_logging_str(char* prefix, std::string s);

//##########################################################################
// logger / stable
//##########################################################################
struct Logger{
private:
	FILE *fp;
	std::string name;
public:
	Logger(std::string name = "log_output.tmp");
	void operator()(std::string fmt, ...);
};
//##########################################################################
// COUT/STREAM FUNCTIONS
//##########################################################################

template<typename T> 
std::ostream &operator<<(std::ostream &s,std::vector< std::vector<T> > tt) { 
	for(uint i=0;i<tt.size();i++){
		s << tt;
	}
	return s;

}
template<typename T> 
std::ostream &operator<<(std::ostream &s,std::vector<T> t) { 
	s << "["; 
	for(uint i=0;i<t.size();i++){
		s << t[i] << (i==t.size()-1?"":",");
	}
	return s << "]";
}


struct CSVReader{
private:
	FILE *fp;
	std::string name;
public:
	CSVReader(std::string name, char delimiter=' ');
	bool line(std::vector<double> &in);
	std::vector< std::vector<double> > getVV(uint numbers_per_line);
	~CSVReader();
};

//##########################################################################
//Special functions / Experimental stuff / not yet stable
//##########################################################################
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	//generic array build by using recursive variadic templates
	template<class T, typename ...A>
	std::vector<T> ARRAY( A ...args);
#endif


//##########################################################################

#include "util_template.cpp"
