#include <iostream>
#include <set>
#include <vector>
#include <chrono>
#include "../lib/polygon_monotonization.cpp"
#include "../lib/polygon_triangulation.cpp"

using namespace std::chrono;

int main(){

	/* Empty graph */
	vector<point> points;
	points.push_back(point(0, 2));
	points.push_back(point(0, 0));
	points.push_back(point(2, 0));
	points.push_back(point(2, 2));
	points.push_back(point(1, 1));
	dcel G(points);
	
	auto start = high_resolution_clock::now();
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);

	/* Printing input graph before modifications */
	cout << "Time taken to create initial DCEL: ";
	cout << duration.count() << " microseconds\n";
	G.printGraph("before.dat");

	/* Printing graph after splitting into y-monotone polygons */
	start = high_resolution_clock::now();
	make_monotone(G);
	stop = high_resolution_clock::now();
	cout << "Time taken to monotonize polygon: ";
	duration = duration_cast<microseconds>(stop - start);
	cout << duration.count() << " microseconds\n";
	G.printGraph("after_m.dat");

	// Printing graph after triangulating each y-monotone piece of it 
	start = high_resolution_clock::now();
	triangulate_dcel(G);
	stop = high_resolution_clock::now();
	cout << "Time taken to triangulate polygon: ";
	duration = duration_cast<microseconds>(stop - start);
	cout << duration.count() << " microseconds\n";
	G.printGraph("after_t.dat");

	cout << "PROGRAM FINISHED!\n";

}