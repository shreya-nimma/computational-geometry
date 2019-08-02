#include <iostream>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include "../lib/point.cpp"
#include "../lib/jarvismarch.cpp"
#include "../lib/kirkseidel.cpp"
using namespace std;

/** Main function to call Jarvis March and
	Kirk-Patrick algorithms.
*/
int main(int argc, char* argv[]){

	vector<point> point_set;
	vector<point> convex_hull;

  /* Taking in input points from input file */
	int n = 1;
	ifstream input(argv[1]);
	if (input.is_open())
    {
    	string line;
    	
    	while ( getline (input,line) )
    	{
    		istringstream iss(line);
    		vector<string> result;
    		for(std::string s; iss >> s; )
    			result.push_back(s);
      		float x = std::stof(result[0].c_str());
      		float y = std::stof(result[1].c_str());
      		point p(x,y,n);
      		++n;
      		point_set.push_back(p);
    	}
    	input.close();
    }
  	else 
  		cout << "Unable to open file";

	/******* Jarvis March - O(m * n) ***********/

	convex_hull = jarvis_march(point_set);
	cout << "JARVIS MARCH: The required convex hull (in anticlockwise order) is: " << endl;
	print_pts(convex_hull);

	/* store the output points in a file */
	ofstream output1 ("output_jarvis.txt");
	int i;
	for(i=0; i<convex_hull.size(); ++i)
	{
		if (output1.is_open())
  		{
    		output1 << convex_hull[i].x<<" "<<convex_hull[i].y<<"\n";   		
  		}
	}
	output1 << convex_hull[0].x<<" "<<convex_hull[0].y<<"\n";
  	output1.close();

  	string command = "gnuplot -p -e \"plot '";
  	command.append(argv[1]);
  	command.append("' , 'output_jarvis.txt' using 1:2 with lines\"");
  	const char* final_cmd1 = command.c_str();
  	
  	system(final_cmd1);

	/******* Kirk-Seidel - O(n * logh) ************/

	convex_hull.clear();
	convex_hull = kirkseidel(point_set);
	cout << "KIRK-SEIDEL: The required convex hull (in clockwise order) is: " << endl;
	print_pts(convex_hull);

  /* store the output points in a file */
	ofstream output2 ("output_kirk.txt");
	for(i=0; i<convex_hull.size(); ++i)
	{
		if (output2.is_open())
  		{
    		output2 << convex_hull[i].x<<" "<<convex_hull[i].y<<"\n";   		
  		}
	}
	output2 << convex_hull[0].x<<" "<<convex_hull[0].y<<"\n";
  	output2.close();

  	command = "gnuplot -p -e \"plot '";
  	command.append(argv[1]);
  	command.append("' , 'output_kirk.txt' using 1:2 with lines\"");
  	const char* final_cmd2 = command.c_str();
  	
  	system(final_cmd2);
}