// This program publishes randomly-generated velocity
// messages for turtlesim.
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>  // For geometry_msgs::Twist
#include <stdlib.h> // For rand() and RAND_MAX
#include <turtlesim/Pose.h>
#include <iomanip> // for std::setprecision and std::fixed

#include <iostream>
//using namespace std;

#include <math.h>

//Declare some global variables even though its bad practice
//in order to grab the message received data
double posX, posY, orientation;


void poseMessageReceived(const turtlesim::Pose& msg) {
	posX = msg.x;			//TODO not working as you thought
	posY = msg.y;			//how do you grab these values?
	orientation = msg.theta;		//global variables I guess? lol
}


//should we have all of our code run within the message received function?
//we need to 

   // 1) Determine the desired heading to the goal (i.e., the heading angle from the robot's current position to the goal).
   // 2) Determine the heading error.
   // 3) Set the angular velocity of the robot to be proportional to the heading error.
   // 4) Set the linear velocity of the robot to be proportional to the distance to the goal. 




int main(int argc, char **argv) {
	//try to see if declaring variables here will receive the data in the callback function
	double xg, yg, angleDiff, distance, angleGoal;		//TODO verify we can get subscriber data back from the callback function like this
	double sinGoal, cosGoal, sinOrientation, cosOrientation;

        double klinear = .5; //.5		//set the linear proportional gain TODO play with these values
	double kangular = 2;		//set the angular proportional gain

	int count = 0;
	int threshold = 500;

	int loopRate = 500;

	// Initialize the ROS system and become a node.
	ros::init(argc, argv, "turtleControl");
	ros::NodeHandle nh;

	// Create a subscriber object.
	ros::Subscriber sub = nh.subscribe("turtle1/pose", 1000,
	&poseMessageReceived);

	// Create a publisher object.
	ros::Publisher pub = nh.advertise<geometry_msgs::Twist>(
	"turtle1/cmd_vel", 1000);

	//Ask the user for the goal position
	std::cout << "Please Enter the Desired Goal Position (xg,yg): "<< std::endl;
	std::cout << "xg = ";
	std::cin >> xg;
	std::cout << "yg = ";
	std::cin >> yg;


	// Loop at 2Hz until the node is shut down.
	ros::Rate rate(loopRate);		//TODO adjust loop rate until good results
	while(ros::ok()) {
		ros::spinOnce();	//let ROS take over to receive the current pose

		geometry_msgs::Twist msg;	//declare the publisher msg
		
		distance = (double) sqrt( pow((xg-posX), 2) + pow((yg - posY), 2) );		//calculate distance
		
/*
		sinGoal = (yg - posY)/distance;
		cosGoal = (xg - posX)/distance;
		angleGoal = atan2(sinGoal, cosGoal);
		sinOrientation = sin(orientation);
		cosOrientation = cos(orientation);
		angleDiff = atan2(sinGoal - sinOrientation, cosGoal - cosOrientation); 
*/
		angleGoal = atan2(yg-posY, xg-posX);
		angleDiff = angleGoal - orientation;		//calculate angle diff in radians (i believe)

		if(angleDiff > 3.14)
			angleDiff = angleDiff - 6.28;
		else if(angleDiff < -3.14)
			angleDiff = angleDiff + 6.28;
		
		//verify the proper direction of error
		
		if(count < threshold)		//set the angle first and then move linearly
		{
			msg.linear.x = 0;
		}
		else
		{
			msg.linear.x = klinear*distance;  //represents velocity in the direction of motion
		}
		msg.angular.z = kangular*angleDiff;

   		// Send a message to rosout with the details.
    		ROS_INFO_STREAM("Sending velocity command:"
      		<< " linear=" << msg.linear.x
      		<< " angular=" << msg.angular.z
		<< " angleGoal=" << angleGoal
		<< " orientation=" << orientation
		<< " angleDiff=" << angleDiff
		<< "count=" << count);
		std::cout << std::endl;


//TODO clean up the info stream, rate sleep, and ros::spinOnce() calls so it works well
		// Publish the message.
		pub.publish(msg);

		count++;

		// Wait until it's time for another iteration.
		rate.sleep();	
	}
}
