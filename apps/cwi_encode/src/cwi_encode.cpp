// cwi_encode.cpp : Defines the exported functions for the DLL application.
//
#include <cstdint>
#include <sstream>
#include <evaluate_comp.h>
#include <evaluate_comp_impl.hpp>
#include <fstream>
#include <stdio.h>
#include <objbase.h>
//#include "cwi_encode.h"

using namespace std;

struct PLYExport
{
	boost::shared_ptr<pcl::PointCloud<PointXYZRGB>> pc;
};
struct MyPoint
{
	float x;
	float y;
	float z;
	INT8 r;
	INT8 g;
	INT8 b;
};
static MyPoint pointArray[500000];
struct MyPointCloud
{
	MyPoint * pointcloud;
	int size;
	//uint64_t timeStamp;
};
extern "C" __declspec(dllexport) MyPointCloud Cwi_test2(char* filename, void *p)
{
	std::string path(filename);
	std::ofstream log1;
	log1.open("log.txt");
	log1 << "\n Entered the function";
	log1.close();
	auto pe = new PLYExport;
	pe->pc = boost::shared_ptr<pcl::PointCloud<PointXYZRGB>>(new PointCloud<PointXYZRGB>());
	std::ofstream log2;
	log2.open("log.txt", std::ofstream::app);
	log2 << "\n Created empty point cloud object :" << path;
	log2.close();
	int res = load_ply_file<PointXYZRGB>(path, pe->pc);
	std::ofstream log3;
	log3.open("log.txt", std::ofstream::app);
	log3 << "\n Load PLY complete size is :" << sizeof(pe->pc);
	log3.close();
	MyPointCloud ptcld;
	//ptcld.pointcloud = pointArray;
	pcl::PointCloud<PointXYZRGB> cld = *(pe->pc);
	int size = cld.height * cld.width;
	ptcld.size = size;
	std::ofstream log4;
	log4.open("log.txt", std::ofstream::app);
	log4 << "\n Point count :" << size;
	log4.close();
	//ptcld.pointcloud = (MyPoint*)::CoTaskMemAlloc(sizeof(MyPoint) * size);
	ptcld.pointcloud = (MyPoint*)GlobalAlloc(GPTR, sizeof(MyPoint) * size);
	
	for (int i = 0; i < size; i++)
	{
		(ptcld.pointcloud[i]).x = cld.points[i].x;
		(ptcld.pointcloud[i]).y = cld.points[i].y;
		(ptcld.pointcloud[i]).z = cld.points[i].z;
		(ptcld.pointcloud[i]).r = cld.points[i].r;
		(ptcld.pointcloud[i]).g = cld.points[i].g;
		(ptcld.pointcloud[i]).b = cld.points[i].b;
	}
	std::ofstream log5;
	log4.open("log.txt", std::ofstream::app);
	log4 << "\n Points Assigned with GlobalAlloc";
	log4.close();
	return ptcld;
}
extern "C" __declspec(dllexport) MyPointCloud Cwi_decoder(unsigned char * compFrame)
{
	encoder_params par;
	std::ofstream log1;
	log1.open("log.txt");
	log1 << "\n Decoder Initialised";
	log1.close();
	//Default values in signals
	par.num_threads = 1;
	par.do_inter_frame = false;
	par.gop_size = 1;
	par.exp_factor = 0;
	par.octree_bits = 7;
	par.color_bits = 8;
	par.jpeg_quality = 85;
	par.macroblock_size = 16;
	std::ofstream log2;
	log2.open("log.txt");
	log2 << "\n Codec params set";
	log2.close();
	std::stringstream compfr;
	//Convert C# bytestream to stringstream for decoding
	compfr << (char *) compFrame;
	evaluate_comp_impl<PointXYZRGB> evaluate;
	boost::shared_ptr<pcl::PointCloud<PointXYZRGB> > decpc(new PointCloud<PointXYZRGB>());
	decpc->makeShared();
	void * dpc;
	dpc = reinterpret_cast<void *> (&decpc);
	uint64_t tmStmp=0;
	evaluate.evaluate_dc(par, dpc, compfr, tmStmp);
	std::ofstream log3;
	log3.open("log.txt");
	log3 << "\n Point cloud extracted size is :" << (*decpc).points.size();
	log3.close();
	//Format coversion
	MyPointCloud ptcld;
	pcl::PointCloud<PointXYZRGB> cld = *decpc;
	int size = cld.height * cld.width;
	ptcld.size = size;
	ptcld.pointcloud = (MyPoint*)GlobalAlloc(GPTR, sizeof(MyPoint) * size);
	//ptcld.timeStamp = tmStmp;
	for (int i = 0; i < size; i++)
	{
		(ptcld.pointcloud[i]).x = cld.points[i].x;
		(ptcld.pointcloud[i]).y = cld.points[i].y;
		(ptcld.pointcloud[i]).z = cld.points[i].z;
		(ptcld.pointcloud[i]).r = cld.points[i].r;
		(ptcld.pointcloud[i]).g = cld.points[i].g;
		(ptcld.pointcloud[i]).b = cld.points[i].b;
	}
	std::ofstream log4;
	log4.open("log.txt");
	log4 << "\n Created MyPointCloud object";
	log4.close();
	return ptcld;
}