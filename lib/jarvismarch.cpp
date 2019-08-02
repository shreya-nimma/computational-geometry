#include <iostream>
#include <algorithm>
#include <vector>


/** Returns point with the least polar angle
	with reference to a given origin point.
*/
point findpt_least_polar_angle(vector<point>& point_set, point origin){

	/* Initializes result point */
	point result = point_set.front();
	std::vector<point>::iterator it = point_set.begin();
	it++;
	for(; it != point_set.end(); it++){
		if(is_counter_clockwise(origin, *it, result) == 1){
			result = *it;
		}else if(is_counter_clockwise(origin, *it, result) == 0){
			float d1 = origin.distance(*it);
			float d2 = origin.distance(result);
			result = (d1 > d2)? *it : result;
		}
	}

	return result;
}

/** Returns the points in the convex hull, 
	in anti-clockwise order 
*/
vector<point> jarvis_march(vector<point> point_set){

	/* Initial empty convex hull */
	vector<point> convex_hull;

	/* Base case */
	if(point_set.size() < 3){
		cerr << "Not enough points to make a convex hull!" << endl;
		exit (EXIT_FAILURE);
	}
	
	/* Finding the start point of the algorithm */
	/* i.e least y-coordinate, x-coordinate used as tiebreaker */
	point start = point_set.front();
	std::vector<point>::iterator it;
	for(it = point_set.begin(); it!= point_set.end(); it++){
		if(start.y > it->y || (start.y == it->y && start.x > it->x)){
			start = *it;
		}
	}

	point current = start;
	bool first_iter = true;
	do{
		if(first_iter){
			/* remove start from set */
			point_set.erase(std::remove(point_set.begin(), point_set.end(), start), point_set.end());
		}
		current = findpt_least_polar_angle(point_set, current);
		if(first_iter){
			/* add it back */
			point_set.push_back(start);
			first_iter = false;
		}
		convex_hull.push_back(current);
		point_set.erase(std::remove(point_set.begin(), point_set.end(), current), point_set.end());

	}while(!(current == start));	

	return convex_hull;
}