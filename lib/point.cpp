#include <iostream>
#include <string>
#include <math.h>
using namespace std;

/** Represents cartesian points. 
*/
class point{
	public:
		float x;
		float y;
		int id;

	point(){
		x = 0;
		y = 0;
	}

	/** Constructor without id. 
	*/
	point(float x1, float y1){
		x = x1;
		y = y1;
	}

	/** Constructor with id member.
	*/
	point(float x1, float y1, int pid){
		x = x1;
		y = y1;
		id = pid;
	}

	/** To recenter points around a new origin 
	*/
	point operator-(point origin){
		point temp(0, 0, 0);
		temp.x = x - origin.x;
		temp.y = y - origin.y;
		return temp;
	}

	/** Adds the x and y components of two
		to those of another point.
	*/
	point operator+(point origin){
		point temp(0, 0, 0);
		temp.x = x + origin.x;
		temp.y = y + origin.y;
		return temp;
	}

	/** Checks for equality of x and y 
		coordinates.
	*/
	bool operator==(point p2){
		if(this->x == p2.x and this->y == p2.y)
			return true;
		return false;
	}

	/** Utility function to print a point 
	*/
	void print(){
		cout << "P(" << this->x << ", " << this->y << ")" << endl;
	}

	float cross(point p2){
		return this->x * p2.y - this->y * p2.x;
	}

	float dot(point p2){
		return this->x * p2.x + this->y * p2.y;
	}

	/** Checks if two points have the same angle.
	*/
	bool eq_angle(point p2){
		if((*this).cross(p2) == 0 and (*this).dot(p2) != 0)
			return true;
		return false;
	}

	/** Returns quadrant of a point:
		Gives a fractional number if
		it lies on an axis.
	 */
	float findquadrant(){
		if(this->x == 0 && this->y == 0) return 0;
		if(this->x > 0 && this->y == 0) return 0.5;
		if(this->x > 0 && this->y > 0) return 1;

		if(this->x == 0 && this->y > 0) return 1.5;
		if(this->x < 0 && this->y > 0) return 2;
		if(this->x < 0 && this->y == 0) return 2.5;

		if(this->x < 0 && this->y < 0) return 3;
		if(this->x == 0 && this->y < 0) return 3.5;
		if(this->x > 0 && this->y < 0) return 4;
	}

	/** For radial sorting by polar angle 
	*/
	bool operator<(point p2){

		/* If the points aren't in the same quadrant */
		float quad1 = (*this).findquadrant();
		float quad2 = p2.findquadrant();
		if(quad1 != quad2){
			return quad1 < quad2;
		}

		/* If angle is equal, sorts based on magnitude/distance from origin */
		/* Remove points of same angle later, if needed */
		if((*this).cross(p2) == 0 and (*this).dot(p2) != 0){
			return (*this).dot(*this) < p2.dot(p2);
		}
		
		else if(this->y == 0 && this->x > 0)
			return true;
		else if(p2.y == 0 && this->y > 0)
			return false;
		else if(this->y > 0 and p2.y < 0)
			return true;
		else if(this->y < 0 and p2.y > 0)
			return false;
		return (*this).cross(p2) > 0; //returns true if p1 is clockwise from p2
	}

	float distance(point p){
		return sqrt((p.x - this->x)*(p.x - this->x) + (p.y-this->y)*(p.y-this->y));
	}

	/* Assumes this point is (x1, y1) and point passed as parameter is (x2, y2) */
	float slope(point p){
		return (p.y - this->y)/(p.x - this->x);
	}

	/** Returns true if point passed as parameter is below this point.
	*/
	bool is_above(point p){
		if((this->y > p.y) or ((this->y) == p.y and this->x < p.x))
			return true;
		else return false;
	}
};

class radial{
	public:
		float r;
		float theta;
		int id;

	radial(float r1, float theta1, int pid){
		r = r1;
		theta = theta1;
		id = pid;
	}
};

/** To represent line segments
	in polar coordinates.
 */
class segment{
	public:
		radial start;
		radial end;
		int id;
	segment(float r1, float theta1, float r2, float theta2, int pid) : start(r1, theta1, 0), end(r2, theta2, pid)
	{
		id = pid;
	}
};

/** Utility function to print a vector of points 
*/
void print_pts(vector<point> point_set){
	for(std::vector<point>::iterator it = point_set.begin(); it != point_set.end(); it++){
		(*it).print();
	}
}


/** Checks if the line bc makes a left turn 
	with respect to line ab, given the
	parameters points a, b, c in that order.
*/
int is_counter_clockwise(point a, point b, point c){
	float res = ((b.y - a.y)*(c.x - b.x) - (c.y - b.y)*(b.x - a.x));
	if(res < 0)
		return 1;
	else if(res == 0)
		return 0;
	else
		return -1;
}

/** Representation of line
	segments in cartesian 
	coordinates.
*/
class line_segment{
	public:
		point start = point(0, 0);
		point end = point(0, 0);
		float slope;

	/** Utility function to print the
		start and end points of the
		line segment.
	*/
	void print(){
		cout << "(" << start.x << ", " << start.y << ") to ";
		cout << "(" << end.x << ", " << end.y << ") slope: " << slope << endl;
	}

	/** Decides whether the line segment
		lies to the right of a given 
		segment.
	*/
	bool is_right_of(point p){

		/* If line segment is vertical */
		if(start.x == end.x){
			if(p.x < start.x)
				return true;
			else 
				return false;
		}

		/* Find the upper point */
		point upper(0, 0);
		point lower(0, 0);
		if(start.y > end.y){
			upper = start;
			lower = end;
		}
		else{
			upper = end;
			lower = start;
		}

		if(is_counter_clockwise(lower, upper, p) == 1)
			return true;
		else
			return false;

	}

};
