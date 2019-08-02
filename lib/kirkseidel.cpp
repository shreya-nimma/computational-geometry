#include <iostream>
#include <algorithm>
#include <vector>

/** Removes duplicates in a vector of points 
*/
void remove_dups(vector<point>& S){
	sort(S.begin(), S.end());
	S.erase(unique(S.begin(), S.end()),S.end());
}

//------------------------------------------
// PREVIOUS FUNCTIONS

/* Comparator function to sort 
	line segments by slope.
*/
// bool compare_slopes(line_segment m, line_segment n){
// 	return m.slope < n.slope;
// }

// /** Returns the median slope
// 	of a set of line segments 
// */
// float med_slope(vector<line_segment>& pairs){
// 	std::sort(pairs.begin(), pairs.end(), compare_slopes);
// 	int index = pairs.size()/2;
// 	if(pairs.size() % 2 == 0){
// 		return (pairs[index - 1].slope + pairs[index].slope)/2;
// 	}else
// 		return pairs[index].slope;
// }

/* Comparator function to sort points by x-coord 
*/
// bool compare_points_x(point p1, point p2){
// 	return p1.x < p2.x;
// }

// /* Finds the median x-coordinate 
// 	value in a set of points 
// */
// float find_median_x_coord(vector<point>& T){
// 	std::sort(T.begin(), T.end(), compare_points_x);
// 	int index = T.size()/2;
// 	if(T.size() % 2 == 0){
// 		return (T[index - 1].x + T[index].x)/2;
// 	}else
// 		return T[index].x;
// }
//---------------------------------------------

/**	Finds the median of a list of upto five
	floating point numbers 
*/
float quick_median(vector<float>& values){
	std::sort(values.begin(), values.end());
	int index = values.size()/2;
	if(values.size() % 2 == 0){
		return values[index - 1];
	}else
		return values[index];
}

/**	Returns a pivot element to the calling
	function using the median of medians algorithm.
*/
float median_of_medians(vector<float>& values){
	vector<float> medians;
	size_t i;
	size_t j = 0;
	for(i = 0; i + 5 <= values.size(); i += 5){
		j += 5;
		vector<float> bucket;
		bucket.push_back(values[i]);
		bucket.push_back(values[i + 1]);
		bucket.push_back(values[i + 2]);
		bucket.push_back(values[i + 3]);
		bucket.push_back(values[i + 4]);

		float m = quick_median(bucket);
		medians.push_back(m);
	}

	if(values.size()%5 != 0){
		vector<float> bucket;
		while(j < values.size()){
			bucket.push_back(values[j++]);
		}
		float m = quick_median(bucket);
		medians.push_back(m);
	}

	if(medians.size() == 1)
		return medians[0];
	else{
		if(medians.size() == 0)
			exit(-1);
		return median_of_medians(medians);
	}
}

/**	Returns the value of the element at the kth
	position of a vector, when sorted in ascending
	order.
*/
float kth_position(vector<float>& values, int k){

	if(k > values.size()){
		cout << "Something went wrong! Value of k greater than array size." << endl;
		exit(-1);
	}

	float pivot = median_of_medians(values);
	vector<float> less_or_equal;
	vector<float> greater;

	int position = 1;
	bool first_equal = true;
	std::vector<float>::iterator it_f;
	for(it_f = values.begin(); it_f != values.end(); it_f++){
		if(pivot > *it_f){
			less_or_equal.push_back(*it_f);
			position++;
		}
		else if(pivot == *it_f){
			if(first_equal){
				first_equal = false;
				continue;
			}
			else{
				less_or_equal.push_back(*it_f);
				position++;
			}
		}
		else{
			greater.push_back(*it_f);
		}
	}

	if(position == k)
		return pivot;
	else if(k < position){
		return kth_position(less_or_equal, k);
	}else
		return kth_position(greater, k - position);
}

/** Returns the value of the median slope of a set
	of line segments.
*/
float med_slope(vector<line_segment>& pairs){
	int size = pairs.size();
	if(size == 1)
		return pairs[0].slope;
	vector<float> slopes;

	std::vector<line_segment>::iterator it_l;
	for(it_l = pairs.begin(); it_l != pairs.end(); it_l++)
		slopes.push_back(it_l->slope);

	int middle = (pairs.size()/2) + 1;
	if(pairs.size() % 2 == 1){
		return kth_position(slopes, middle);
	}
	else{
		float a = kth_position(slopes, middle);
		float b = kth_position(slopes, middle - 1);
		return (a + b)/2;
	}
}

/** Returns the value of a median x-coordinate
	in a set of points.
*/
float find_median_x_coord(vector<point>& T){
	int size = T.size();
	if(size == 1)
		return T[0].x;
	vector<float> x_coords;

	std::vector<point>::iterator it_p;
	for(it_p = T.begin(); it_p != T.end(); it_p++)
		x_coords.push_back(it_p->x);

	int middle = (T.size()/2) + 1;
	if(T.size() % 2 == 1){
		return kth_position(x_coords, middle);
	}
	else{
		float a = kth_position(x_coords, middle);
		float b = kth_position(x_coords, middle - 1);
		return (a + b)/2;
	}
}

/** Returns the upper bridge line segment.
*/
line_segment upper_bridge(vector<point>& T, float median_x){

	/* Contains potential endpoints of upper bridge, initially empty */
	vector<point> candidates;

	/* If only two points, return them as upper bridge */
	if(T.size() == 2){
		line_segment result;
		if(T[0].x < T[1].x || (T[0].x == T[1].x && T[0].y > T[1].y)){  //Double check this condition
			result.start = T[0];
			result.end = T[1];
		}else{
			result.start = T[1];
			result.end = T[0];
		}
		return result;
	}

	/* Arbitrarily pair up points */
	vector<line_segment> pairs;
	if(T.size() != 0){

		/* Iterating points in jumps of two */
		for(size_t i = 0; i < T.size() - 1; i += 2){

			/* If slope does not exist, push one point into candidates */
			if(T[i].x == T[i+1].x){
				if(T[i].y > T[i+1].y)
					candidates.push_back(T[i]);
				else
					candidates.push_back(T[i+1]);
				continue;
			}

			line_segment temp;

			/* Forming line segment, with point with smaller x-coord as start point */
			if(T[i].x <= T[i+1].x){
				temp.start = T[i];
				temp.end = T[i+1];
			}else{
				temp.start = T[i+1];
				temp.end = T[i];
			}

			/* Calculating slope */
			temp.slope = (temp.start.y - temp.end.y)/(temp.start.x - temp.end.x);

			/* Only pushing points with real slopes into 'pairs' */
			pairs.push_back(temp);
		}
	}else
		cout << "Something went wrong. T is empty" << endl;

	/* If odd number of points, putting the left out one in candidates */
	if(T.size() % 2 == 1)
		candidates.push_back(T.back());

	/* Calculating median slope, to be used as slope of support line */
	float median_slope = med_slope(pairs);

	vector<line_segment> small, equal, large;

	/* Binning point pairs based on their slope wrt the median slope */
	std::vector<line_segment>::iterator it_l;
	for(it_l = pairs.begin(); it_l != pairs.end(); it_l++){
		if(it_l->slope < median_slope){
			small.push_back(*it_l);
		}
		else if(it_l->slope == median_slope){
			equal.push_back(*it_l);
		}
		else{
			large.push_back(*it_l);
		}
	}


	vector<point> max;
	max.push_back(T.front());
	float max_y_intercept = max.front().y - median_slope * max.front().x;

	/* max contains all points which make maximum y-intercept given median slope */
	std::vector<point>::iterator it_p = T.begin();
	it_p++;
	for(; it_p != T.end(); it_p++){
		float temp_y_intercept = it_p->y - median_slope * it_p->x;

		/* If greater y-intercept found */
		if(max_y_intercept < temp_y_intercept){
			max.clear();
			max.push_back(*it_p);
			max_y_intercept = temp_y_intercept;
		}
		/* If new point of equal max y-intercept found */
		else if(max_y_intercept == temp_y_intercept){
			max.push_back(*it_p);
		}
	}

	/* Points in max that have the least and largest x-coord value respectively */
	point p_k = max.front();
	point p_m = max.front();
	for(it_p = max.begin(); it_p != max.end(); it_p++){
		if(it_p->x < p_k.x)												//CHECK WHAT HAPPENS IF EQUAL
			p_k = *it_p;
		if(it_p->x > p_m.x)
			p_m = *it_p;
	}

	/* Prune and conquer step */
	if(p_k.x <= median_x && p_m.x > median_x){
		line_segment temp;
		temp.start = p_k;
		temp.end = p_m;
		return temp;
	}else if(p_m.x <= median_x){
		for(it_l = large.begin(); it_l != large.end(); it_l++){
			candidates.push_back(it_l->end);
		}
		for(it_l = equal.begin(); it_l != equal.end(); it_l++){
			candidates.push_back(it_l->end);
		}
		for(it_l = small.begin(); it_l != small.end(); it_l++){
			candidates.push_back(it_l->start);
			candidates.push_back(it_l->end);
		}
	}else if(p_k.x > median_x){
		for(it_l = small.begin(); it_l != small.end(); it_l++){
			candidates.push_back(it_l->start);
		}
		for(it_l = equal.begin(); it_l != equal.end(); it_l++){
			candidates.push_back(it_l->start);
		}
		for(it_l = large.begin(); it_l != large.end(); it_l++){
			candidates.push_back(it_l->start);
			candidates.push_back(it_l->end);
		}
	}

	return upper_bridge(candidates, median_x);
	
}

/** Function to return lower bridge line segment 
*/
line_segment lower_bridge(vector<point>& T, float median_x){

	/* Contains potential endpoints of lower bridge, initially empty */
	vector<point> candidates;

	/* If only two points, return them as lower bridge */
	if(T.size() == 2){
		line_segment result;
		if(T[0].x < T[1].x || (T[0].x == T[1].x && T[0].y > T[1].y)){  //Double check this condition
			result.start = T[0];
			result.end = T[1];
		}else{
			result.start = T[1];
			result.end = T[0];
		}
		return result;
	}

	/* Arbitrarily pair up points */
	vector<line_segment> pairs;
	if(T.size() != 0){

		/* Iterating points in jumps of two */
		for(size_t i = 0; i < T.size() - 1; i += 2){

			/* If slope does not exist, push one point into candidates */
			if(T[i].x == T[i+1].x){
				if(T[i].y < T[i+1].y)
					candidates.push_back(T[i]);
				else
					candidates.push_back(T[i+1]);
				continue;
			}

			line_segment temp;

			/* Forming line segment, with point with smaller x-coord as start point */
			if(T[i].x <= T[i+1].x){
				temp.start = T[i];
				temp.end = T[i+1];
			}else{
				temp.start = T[i+1];
				temp.end = T[i];
			}

			/* Calculating slope */
			temp.slope = (temp.start.y - temp.end.y)/(temp.start.x - temp.end.x);

			/* Only pushing points with real slopes into 'pairs' */
			pairs.push_back(temp);
		}
	}else
		cout << "Something went wrong. T is empty" << endl;

	/* If odd number of points, putting the left out one in candidates */
	if(T.size() % 2 == 1)
		candidates.push_back(T.back());

	/* Calculating median slope, to be used as slope of support line */
	float median_slope = med_slope(pairs);

	vector<line_segment> small, equal, large;

	/* Binning point pairs based on their slope wrt the median slope */
	std::vector<line_segment>::iterator it_l;
	for(it_l = pairs.begin(); it_l != pairs.end(); it_l++){
		if(it_l->slope < median_slope){
			small.push_back(*it_l);
		}
		else if(it_l->slope == median_slope){
			equal.push_back(*it_l);
		}
		else{
			large.push_back(*it_l);
		}
	}

	vector<point> min;
	min.push_back(T.front());
	float min_y_intercept = min.front().y - median_slope * min.front().x;

	/* min contains all points which make minimum y-intercept given median slope */
	std::vector<point>::iterator it_p = T.begin();
	it_p++;
	for(; it_p != T.end(); it_p++){
		float temp_y_intercept = it_p->y - median_slope * it_p->x;

		/* If smaller y-intercept found */
		if(min_y_intercept > temp_y_intercept){
			min.clear();
			min.push_back(*it_p);
			min_y_intercept = temp_y_intercept;
		}
		/* If new point of equal min y-intercept found */
		else if(min_y_intercept == temp_y_intercept){
			min.push_back(*it_p);
		}
	}

	/* Points in min that have the least and largest x-coord value respectively */
	point p_k = min.front();
	point p_m = min.front();
	for(it_p = min.begin(); it_p != min.end(); it_p++){
		if(it_p->x < p_k.x)
			p_k = *it_p;
		if(it_p->x > p_m.x)
			p_m = *it_p;
	}

	/* Prune and conquer step */
	if(p_k.x <= median_x && p_m.x > median_x){
		line_segment temp;
		temp.start = p_k;
		temp.end = p_m;
		return temp;
	}else if(p_m.x <= median_x){
		for(it_l = small.begin(); it_l != small.end(); it_l++){
			candidates.push_back(it_l->end);
		}
		for(it_l = equal.begin(); it_l != equal.end(); it_l++){
			candidates.push_back(it_l->end);
		}
		for(it_l = large.begin(); it_l != large.end(); it_l++){
			candidates.push_back(it_l->start);
			candidates.push_back(it_l->end);
		}
	}else if(p_k.x > median_x){
		for(it_l = large.begin(); it_l != large.end(); it_l++){
			candidates.push_back(it_l->start);
		}
		for(it_l = equal.begin(); it_l != equal.end(); it_l++){
			candidates.push_back(it_l->start);
		}
		for(it_l = small.begin(); it_l != small.end(); it_l++){
			candidates.push_back(it_l->start);
			candidates.push_back(it_l->end);
		}
	}

	return lower_bridge(candidates, median_x);
}

/** Returns the points in the upper hull 
	in left-right order 
*/
vector<point> upper_hull(point& p_umin, point& p_umax, vector<point>& T){
	vector<point> u_hull;

	/* Base case */
	if(p_umax == p_umin){
		u_hull.push_back(p_umin);
		return u_hull;
	}

	/* Finds median x-coord value, this value used as line splitting points */
	float median_x = find_median_x_coord(T);

	/* Obtain coordinates of upper bridge */
	line_segment u_bridge = upper_bridge(T, median_x);

	/* Defining the boundaries within which to discard points */
	line_segment left_boundary;
	line_segment right_boundary;

	left_boundary.start = p_umin;
	left_boundary.end = u_bridge.start;
	right_boundary.start = u_bridge.end;
	right_boundary.end = p_umax;

	/* Split points into left and right */
	vector<point> T_left, T_right;
	std::vector<point>::iterator it;

	for(it = T.begin(); it != T.end(); it++){
		if(left_boundary.is_right_of(*it) and it->x < median_x)
			T_left.push_back(*it);
		if(!(right_boundary.is_right_of(*it)) and it->x > median_x)
			T_right.push_back(*it);
	}
	T_left.push_back(u_bridge.start);
	T_right.push_back(u_bridge.end);
	T_left.push_back(p_umin);
	T_right.push_back(p_umax);

	remove_dups(T_left);
	remove_dups(T_right);

	vector<point> left_uhull = upper_hull(p_umin, u_bridge.start, T_left);
	vector<point> right_uhull = upper_hull(u_bridge.end, p_umax, T_right);

	/* Concatenating the left upper hull to the upper bridge and the right upper hull */
	for(it = left_uhull.begin(); it != left_uhull.end(); it++)
		u_hull.push_back(*it);
	u_hull.push_back(u_bridge.start);
	u_hull.push_back(u_bridge.end);
	for(it = right_uhull.begin(); it != right_uhull.end(); it++)
		u_hull.push_back(*it);

	return u_hull;
}

/** Returns the points in the lower hull 
	in left-right order 
*/vector<point> lower_hull(point& p_lmin, point& p_lmax, vector<point>& T){
	vector<point> l_hull;

	/* Base case */
	if(p_lmax == p_lmin){
		l_hull.push_back(p_lmin);
		return l_hull;
	}

	/* Finds median x-coord value, this value used as line splitting points */
	float median_x = find_median_x_coord(T);

	/* Obtain coordinates of lower bridge */
	line_segment l_bridge = lower_bridge(T, median_x);

	/* Defining the boundaries within which to discard points */
	line_segment left_boundary;
	line_segment right_boundary;

	left_boundary.start = p_lmin;
	left_boundary.end = l_bridge.start;
	right_boundary.start = l_bridge.end;
	right_boundary.end = p_lmax;

	/* Split points into left and right */
	vector<point> T_left, T_right;
	std::vector<point>::iterator it;

	for(it = T.begin(); it != T.end(); it++){
		if(left_boundary.is_right_of(*it) and it-> x < median_x)
			T_left.push_back(*it);
		if(!(right_boundary.is_right_of(*it)) and it->x > median_x)
			T_right.push_back(*it);
	}
	T_left.push_back(l_bridge.start);
	T_right.push_back(l_bridge.end);
	T_left.push_back(p_lmin);
	T_right.push_back(p_lmax);

	/* Recursive part */
	remove_dups(T_left);
	remove_dups(T_right);

	vector<point> left_lhull = lower_hull(p_lmin, l_bridge.start, T_left);
	vector<point> right_lhull = lower_hull(l_bridge.end, p_lmax, T_right);

	/* Concatenating the left lower hull to the lower bridge and the right lower hull */
	for(it = left_lhull.begin(); it != left_lhull.end(); it++)
		l_hull.push_back(*it);
	l_hull.push_back(l_bridge.start);
	l_hull.push_back(l_bridge.end);
	for(it = right_lhull.begin(); it != right_lhull.end(); it++)
		l_hull.push_back(*it);

	return l_hull;
}

/** Implements kirkpatrick-seidel prune and 
	search algorithm. Returns points in the
	convex hull, in clockwise order.
*/
vector<point> kirkseidel(vector<point> point_set){

	std::vector<point>::iterator it;

	point p_umax = point_set.front();
	point p_umin = point_set.back();

	point p_lmax = point_set.front();
	point p_lmin = point_set.back();

	/* Setting p_umin, p_lmin to point with least x-coord, p_umax, p_lmax to point with largest x-coord */
	for(it = point_set.begin(); it != point_set.end(); it++){
		if(p_umin.x > it->x || (p_umin.x == it->x && p_umin.y < it->y)){
			p_umin = *it;
		}
		if(p_umax.x < it->x || (p_umax.x == it->x && p_umax.y < it->y)){
			p_umax = *it;
		}
		if(p_lmin.x > it->x || (p_lmin.x == it->x && p_lmin.y > it->y)){
			p_lmin = *it;
		}
		if(p_lmax.x < it->x || (p_lmax.x == it->x && p_lmax.y > it->y)){
			p_lmax = *it;
		}
	}

	/* Initializing T to points with x values between those of p_umin, p_umax */
	vector<point> T;
	for(it = point_set.begin(); it != point_set.end(); it++){
		if(it->x > p_umin.x && it->x < p_umax.x)
			T.push_back(*it);
	}
	T.push_back(p_umin);
	T.push_back(p_umax);

	/* Upper Hull */
	vector<point> u_hull = upper_hull(p_umin, p_umax, T);
	u_hull.erase(unique(u_hull.begin(), u_hull.end()),u_hull.end());
	// cout << "-----------------------\n";
	// cout << "Upper hull is: \n";
	// print_pts(u_hull);

	/* Clearing points from T, and repeating process to create lower hull */
	T.clear();
	for(it = point_set.begin(); it != point_set.end(); it++){
		if(it->x > p_lmin.x && it->x < p_lmax.x)
			T.push_back(*it);
	}
	T.push_back(p_lmin);
	T.push_back(p_lmax);

	/* Lower Hull */
	vector<point> l_hull = lower_hull(p_lmin, p_lmax, T);
	l_hull.erase(unique(l_hull.begin(), l_hull.end()),l_hull.end());
	// cout << "-----------------------\n";
	// cout << "Lower hull is: \n";
	// print_pts(l_hull);
	// cout << "-----------------------\n";

	// /* Concatenating lower and upper hulls */
	vector<point> convex_hull;	
	for(it = u_hull.begin(); it != u_hull.end(); it++){
		convex_hull.push_back(*it);
	}
	point front = convex_hull.front();
	point back = convex_hull.back();
	std::vector<point>::reverse_iterator rit;
	for(rit = l_hull.rbegin(); rit != l_hull.rend(); rit++){
		if(!(*rit == front or *rit == back)){
			convex_hull.push_back(*rit);
		}
	}
	return convex_hull;
}