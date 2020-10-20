#include "segmatch/descriptors/fpfh.hpp"
#include "segmatch/parameters.hpp"
#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <sensor_msgs/PointCloud2.h>
#include <iostream>

#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_types.h>
#include <pcl/PCLPointCloud2.h>
#include <pcl/conversions.h>
#include <pcl_ros/transforms.h>

#include <pcl/io/pcd_io.h>

#include <algorithm>

int main(int argc, char **argv) {
	// Create Segmatched FPFH object.
	segmatch::DescriptorsParameters dummy_params;
	segmatch::FpfhDescriptor fpfh_tester(dummy_params);

	// Load point clouds from bag file.
	rosbag::Bag bag;
	bag.open("/home/nikhilesh/Documents/segments/segments.bag", rosbag::bagmode::Read);
	std::vector<std::string> topics;
	topics.push_back(std::string("segmatch/source_representation"));
	rosbag::View view(bag, rosbag::TopicQuery(topics));
	sensor_msgs::PointCloud2::ConstPtr input;
	int l=0;
	for(rosbag::MessageInstance const m: rosbag::View(bag))
	{	
		l++;
		std::cout<<"HalloI"<<std::endl;
		input = m.instantiate<sensor_msgs::PointCloud2>();
		if (input!=NULL) 
			{
				std::cout<<"Break"<<std::endl;
				break;
			}
	}

	if(input==NULL)
	{
		std::cout<<"OOOOO"<<std::endl;
	}
	
	pcl::PCLPointCloud2 pcl_pc2;
    pcl_conversions::toPCL(*input,pcl_pc2);
    pcl::PointCloud<pcl::PointXYZI>::Ptr temp_cloud(new pcl::PointCloud<pcl::PointXYZI>);
    pcl::fromPCLPointCloud2(pcl_pc2,*temp_cloud);

	std::cout<<"Read one pc scan "<<temp_cloud->size()<<std::endl;

	bag.close();

	pcl::io::savePCDFileASCII ("/home/nikhilesh/Documents/segments/test_pcd.pcd", *temp_cloud);

	// Count number of distinct segments (different intensity values).
	std::vector<int> segment_ids;
	for(auto it=temp_cloud->begin(); it!=temp_cloud->end();it++)
	{
		int intensity = int(it->intensity);
		bool already_assigned = (std::find(segment_ids.begin(), segment_ids.end(), intensity) != segment_ids.end());
		if(!already_assigned)
		{
			segment_ids.push_back(intensity);
		}
	}

	std::cout<<"There are "<<segment_ids.size()<<" segments in the point cloud!"<<std::endl;
	
	// Create one point cloud for each segment.
	std::vector<pcl::PointCloud<pcl::PointXYZI>, Eigen::aligned_allocator<pcl::PointXYZI>> cloud_segments(segment_ids.size());
	std::cout<<"Created "<<cloud_segments.size()<<" segments."<<std::endl;

	for(auto pt_it = temp_cloud->begin();pt_it!=temp_cloud->end();pt_it++)
	{
		int segment_id = int(pt_it->intensity);
		int segment_idx = 3;// Find entry in segment_ids.
		cloud_segments[segment_idx].push_back(*pt_it);
	}

	for(int i = 0; i<cloud_segments.size();i++)
	{
		if(cloud_segments[i].size()>0)
		{
			std::cout<<"Cloud No. "<<i<<" has "<<cloud_segments[i].size()<<" points."<<std::endl;
			pcl::io::savePCDFileASCII ("/home/nikhilesh/Documents/segments/segment" + std::to_string(i)+".pcd", cloud_segments[i]);
		}
	
	}

	// Pass each point cloud to FPFH object.

	// Retrieve feature.

	// Histogram plot from feature.

	// Save pc to pcd file.

	// Save histogram to file.

	// Close bag file.

	std::cout<<"Hello WELT"<<std::endl;

}