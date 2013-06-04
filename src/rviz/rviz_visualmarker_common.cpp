#include "rviz_visualmarker.h"

namespace ros{
	Text::Text(double x, double y, double z, char *c): RVIZVisualMarker(){
		this->g.x = x;
		this->g.y = y;
		this->g.z = z;
		this->g.setYawRadian(0);
		this->g.sz=0.1;
		init_marker();
		marker.text = std::string(c);
		text = marker.text;
	}
	std::string Text::name(){
		return text;
	}
	uint32_t Text::get_shape(){
		return visualization_msgs::Marker::TEXT_VIEW_FACING;
	}
	Color Text::get_color(){
		return ros::TEXT_COLOR;
	}
	SphereMarker::SphereMarker(double x, double y, double r, double z): RVIZVisualMarker() {
		this->g.x = x;
		this->g.y = y;
		this->g.z = z;
		this->g.setYawRadian(0);
		this->g.sx=r;
		this->g.sy=r;
		this->g.sz=0.05;
		this->g.z = this->g.z + this->g.sz/2;
		init_marker();
	}
	std::string SphereMarker::name(){
		return std::string("sphere");
	}
	uint32_t SphereMarker::get_shape(){
		return visualization_msgs::Marker::CYLINDER;
	}
	Color SphereMarker::get_color(){
		return ros::MAGENTA;
	}
	CubeMarker::CubeMarker(double x, double y, double r, double yaw): RVIZVisualMarker() {
		this->g.x = x;
		this->g.y = y;
		this->g.setYawRadian(yaw);
		this->g.sx=r;
		this->g.sy=r;
		this->g.sz=r;
		this->g.z = this->g.z + this->g.sz/2; //bottom of cube should be at desired z, not center
		init_marker();
	}
	std::string CubeMarker::name(){
		return std::string("cuberizer");
	}
	uint32_t CubeMarker::get_shape(){
		return visualization_msgs::Marker::CUBE;
	}
	Color CubeMarker::get_color(){
		return ros::MAGENTA;
	}

	CylinderMarkerTriangles::CylinderMarkerTriangles(double x, double y, double r, double h): RVIZVisualMarker() {
		this->g.x = x;
		this->g.y = y;
		this->g.z = 0.0;
		this->g.setYawRadian(0);
		this->g.sx=1;
		this->g.sy=1;
		this->g.sz=1;
		this->g.z = 0.0;
		this->g.radius = r;
		this->g.height = h;
		uint N=20;

		this->bvh = new fcl::BVHModel< BoundingVolume >();
		cylinder2BVH(this->bvh, N, r, h);
		cylinder2marker(this->marker, N, r, h);
		init_marker();
	}
	std::string CylinderMarkerTriangles::name(){
		return std::string("cylinder_triangles");
	}
	uint32_t CylinderMarkerTriangles::get_shape(){
		return visualization_msgs::Marker::TRIANGLE_LIST;
	}
	Color CylinderMarkerTriangles::get_color(){
		return ros::Color(1,0,0,0.3);
	}
#ifdef FCL_COLLISION_CHECKING
	std::pair< std::vector<fcl::Vec3f>, std::vector<fcl::Triangle> > CylinderMarkerTriangles::getCylinderVerticesAndTriangles(uint N, double radius, double height){

		std::vector<fcl::Vec3f> vertices;
		std::vector<fcl::Triangle> triangles;

		//ROS_INFO("created object in FCL with %d triangles and %d vertices.\n", bvh->num_tris, bvh->num_vertices);
		uint Nvertices = N+N+2*N; //top,bottom and connect the points by double triangles

		uint i=0;//counter of vertices
		for(double h=0;h<=height;h+=height){
			double oldx = radius;
			double oldy = 0.0;
			for(double t=0;t<2*M_PI;t+=2*M_PI/N){
				
				//first vertex at middle point
				fcl::Vec3f a(0, 0, h);
				//second vertex at old pos
				fcl::Vec3f b(oldx, oldy, h);
				//third vertex at next pos
				//t= 2*M_PI/(N-i-1);
				double newx = cos(t)*radius;
				double newy = sin(t)*radius;
				fcl::Vec3f c(newx, newy, h);

				vertices.push_back(a);
				vertices.push_back(b);
				vertices.push_back(c);

				fcl::Triangle t(i,i+1,i+2);
				triangles.push_back(t);
				i=i+3;

				oldx=newx;
				oldy=newy;
			}
		}


		double oldx = radius;
		double oldy = 0.0;
		for(double t=0;t<2*M_PI;t+=2*M_PI/N){
			double newx = cos(t)*radius;
			double newy = sin(t)*radius;

			fcl::Vec3f a(oldx, oldy, 0);
			fcl::Vec3f b(oldx, oldy, height);
			fcl::Vec3f c(newx, newy, 0);

			vertices.push_back(a);
			vertices.push_back(b);
			vertices.push_back(c);

			fcl::Triangle t(i,i+1,i+2);
			triangles.push_back(t);
			i=i+3;

			fcl::Vec3f d(newx, newy, 0);
			fcl::Vec3f e(oldx, oldy, height);
			fcl::Vec3f f(newx, newy, height);

			vertices.push_back(d);
			vertices.push_back(e);
			vertices.push_back(f);

			fcl::Triangle t2(i,i+1,i+2);
			triangles.push_back(t2);
			i=i+3;

			oldx = newx;
			oldy = newy;
		}
		return std::make_pair( vertices, triangles );

	}
	void CylinderMarkerTriangles::cylinder2BVH(fcl::BVHModel< BoundingVolume > *m, uint N, double radius, double height){
		std::pair< std::vector<fcl::Vec3f>, std::vector<fcl::Triangle> > vt;
		vt = getCylinderVerticesAndTriangles(N, radius, height);

		std::vector<fcl::Vec3f> vertices = vt.first;
		std::vector<fcl::Triangle> triangles = vt.second;

		bvh->bv_splitter.reset (new fcl::BVSplitter<BoundingVolume>(fcl::SPLIT_METHOD_MEAN));
		bvh->beginModel();
		bvh->addSubModel(vertices, triangles);
		bvh->endModel();
	}
	void CylinderMarkerTriangles::cylinder2marker(visualization_msgs::Marker &marker, uint N, double radius, double height){
		
		std::pair< std::vector<fcl::Vec3f>, std::vector<fcl::Triangle> > vt;
		vt = getCylinderVerticesAndTriangles(N, radius, height);

		std::vector<fcl::Vec3f> vertices = vt.first;
		std::vector<fcl::Triangle> triangles = vt.second;

		Color cc = get_color();
		std_msgs::ColorRGBA c;
		c.r = cc.r;
		c.g = cc.g;
		c.b = cc.b;
		c.a = cc.a;

		for (uint i = 0; i < vertices.size(); i++){
			geometry_msgs::Point p;
			p.x = vertices.at(i)[0];
			p.y = vertices.at(i)[1];
			p.z = vertices.at(i)[2];
			//ROS_INFO("vertices %f %f %f\n",p.x,p.y,p.z);
			marker.points.push_back(p);
			marker.colors.push_back(c);
		}
	}
#endif

}
