#include "dcel.cpp"
#include <algorithm>

#define SPLIT_VERTEX 1
#define MERGE_VERTEX 2
#define END_VERTEX 3
#define START_VERTEX 4
#define REGULAR_VERTEX 5

#define A 1
#define B 2

/** Functor definition.
*/
struct compare_y{

	/** Comparator to order vertices by decreasing y-coord
	*/
	bool operator()(const vertex* a, const vertex* b) const{
		if(a->coordinates.y < b->coordinates.y)
		return true;
	else if(a->coordinates.y == b->coordinates.y and a->coordinates.x > b->coordinates.x)
		return true;
	else
		return false;
	}
};

/** Functor definition.
*/
struct compare_edges_left{

	/** Functor to order edges of the polygon in
	* the status structure, tau, in order of left to right.
	* NOTE: this assumes any edge that will be added to tau will slope
	* downwards i.e its origin is higher than its destination.
	*/
	bool operator()(const half_edge* a, const half_edge* b) const{

		vertex* a_origin = a->origin;
		vertex* b_origin = b->origin;

		point lower_origin;
		point higher_origin;
		int lower_edge;

		/* Picking lower origin pt. */
		if(a_origin->coordinates.y < b_origin->coordinates.y){
			lower_origin = a_origin->coordinates;
			higher_origin = b_origin->coordinates;
			lower_edge = A;
		}
		else{
			lower_origin = b_origin->coordinates;
			higher_origin = a_origin->coordinates;
			lower_edge = B;
		}

		/* Finding the x-coordinate of the pt. where a horizontal
		* line through the lower_origin intersects the other edge.
		*/
		float x;

		point p1 = higher_origin;
		point p2 = (lower_edge == A)? b->twin->origin->coordinates : a->twin->origin->coordinates;

		float m = p1.slope(p2);
		float y0 = p1.y - p1.x * m;

		float intersectionpt_x = m * higher_origin.x + y0;

		/* returns true if edge A is to the left of edge B */
		if(lower_origin.x < intersectionpt_x){
			if(lower_edge == A)
				return true;
			else
				return false;
		}
		else{
			if(lower_edge == A)
				return false;
			else
				return true;
		}

	}
};

/* Function declarations */
int check_vertex_type(dcel&, vertex*);

void handle_start_vertex(dcel&, set<half_edge*, compare_edges_left >&, vertex*);					
void handle_merge_vertex(dcel&, set<half_edge*, compare_edges_left >&, vertex*);					
void handle_regular_vertex(dcel&,set<half_edge*, compare_edges_left >&, vertex*);
void handle_split_vertex(dcel&, set<half_edge*, compare_edges_left >&, vertex*);					
void handle_end_vertex(dcel&, set<half_edge*, compare_edges_left >&, vertex*);					

half_edge* find_immediate_left(set<half_edge*, compare_edges_left >&, vertex*);

/** Function to partition simple polygon G
* into y-monotone polygons.
*/
void make_monotone(dcel& G){

	/* Making priority queue of vertices */
	vector<vertex*> p_queue;
	set<vertex*>::iterator it;
	for(it = G.vertices.begin(); it != G.vertices.end();it++){
		p_queue.push_back((*it));
	}
	make_heap(p_queue.begin(), p_queue.end(), compare_y());

	/* Status structure with custom comparator */
	set<half_edge*, compare_edges_left> tau;

	/* Handling event points one-by-one */
	while(p_queue.size() != 0){

		/* Removing highest priority vertex */
		pop_heap(p_queue.begin(), p_queue.end(), compare_y());
		vertex* curr = p_queue.back();
		p_queue.pop_back();

		/* Checking type of vertex */
		int type = check_vertex_type(G, curr);
		// cout << "v" << curr->id << " : ";

		/* Calling handler functions based on type */
		switch(type){
			case START_VERTEX:
				// cout << " start\n";
				handle_start_vertex(G, tau, curr);
				//cout << "Ready to handle next\n";
				break;
			case END_VERTEX:
				// cout << " end\n";
				handle_end_vertex(G, tau, curr);
				break;
			case MERGE_VERTEX:
				// cout << " merge\n";
				handle_merge_vertex(G, tau, curr);
				break;
			case SPLIT_VERTEX:
				// cout << " split\n";
				handle_split_vertex(G, tau, curr);
				break;
			case REGULAR_VERTEX:
				// cout << "regular\n";
				handle_regular_vertex(G, tau, curr);
				break;
			default:	
				cout << " ???\n";
		}
	}
}

/** Checks whether a vertex is a turn vertex or regular vertex.
* NOTE: only works if the DCEL initially stores only one polygon.
*/
int check_vertex_type(dcel& G, vertex* v){

	// cout << "Entered function: CHECK VERTEX TYPE for vertices: " << v->id << endl;				//PRINT

	/* Selecting e as the half-edge out of v that bounds the polygon interior. */
	half_edge* e = (v->incident_edge->incident_face == NULL)? v->incident_edge->prev->twin: v->incident_edge;

	/* Sets neighboring vertices of v */
	vertex* neighbor1 = e->twin->origin;
	vertex* neighbor2 = e->prev->origin;

	/* Checking whether both neighbors are above/below v*/
	if(neighbor1->coordinates.y >= v->coordinates.y and neighbor2->coordinates.y >= v->coordinates.y){
		/* Both neighbors are above v */

		/* Checking orientation of line passing through these points */
		point p1 = neighbor2->coordinates;
		point p2 = v->coordinates;
		point p3 = neighbor1->coordinates;

		// cout << "Almost Exiting function: CHECK VERTEX TYPE for vertices: " << v->id << endl;					//PRINT

		if(is_counter_clockwise(p1, p2, p3) == 1)
			return END_VERTEX;
		else
			return MERGE_VERTEX;
	}
	else if(neighbor1->coordinates.y < v->coordinates.y and neighbor2->coordinates.y < v->coordinates.y){
		/* Both neighbors are below v */

		/* Checking orientation of line passing through these points */
		point p1 = neighbor2->coordinates;
		point p2 = v->coordinates;
		point p3 = neighbor1->coordinates;

		// cout << " Almost Exiting function: CHECK VERTEX TYPE for vertices: " << v->id << endl;				//PRINT

		if(is_counter_clockwise(p1, p2, p3) == 1)
			return START_VERTEX;
		else
			return SPLIT_VERTEX;
	}
	else{
		// cout << " Almost Exiting function: CHECK VERTEX TYPE for vertices: " << v->id << endl;				//PRINT
		return REGULAR_VERTEX;
	}
}

/** Handles start vertices.
*/
void handle_start_vertex(dcel& G, set<half_edge*, compare_edges_left >& tau, vertex* v){

	/* Selecting the half-edge originating from v that bounds the polygon interior as e */
	half_edge* e = (v->incident_edge->incident_face == NULL)? v->incident_edge->prev->twin: v->incident_edge ;

	/* Inserting it into the status */
	tau.insert(e);

	/* Setting its helper */
	e->helper = v;
}

/** Handles end vertices.
*/
void handle_end_vertex(dcel& G, set<half_edge*, compare_edges_left >& tau, vertex* v){

	// cout << "Entered function: HANDLE END VERTEX for vertices: " << v->id << endl;				//PRINT

	/* Selecting the half-edge ending at v that bounds the polygon interior. */
	half_edge* e = (v->incident_edge->incident_face == NULL)? v->incident_edge->twin : v->incident_edge->prev ;

	/* If e is horizontal, return */
	if(e->origin->coordinates.y == e->twin->origin->coordinates.y)
		return;

	/* Inserting diagonal if merge vertex */
	if(check_vertex_type(G, e->helper) == MERGE_VERTEX){
		G.split_face(e, v, e->helper);
	}

	/* Removing edge from status */
	tau.erase(e);

	// cout << "Exited function: HANDLE END VERTEX for vertices: " << v->id << endl;				//PRINT
}

/** Handles split-vertex
*/
void handle_split_vertex(dcel& G, set<half_edge*, compare_edges_left >& tau, vertex* v){

	half_edge* e = find_immediate_left(tau, v);

	/* Selecting the half-edge ending at v that bounds the polygon interior as e */
	half_edge* h = (v->incident_edge->incident_face == NULL)? v->incident_edge->twin : v->incident_edge->prev ;

	/* Inserting diagonal and updating helper */
	//insert checking condition for unique edge here
	G.split_face(h, v, e->helper);
	e->helper = v;

	/* Selecting the half-edge originating from v that bounds the polygon interior as e */
	h = (v->incident_edge->incident_face == NULL)? v->incident_edge->prev->twin: v->incident_edge ;

	/* Inserting edge into status */
	tau.insert(h);
	h->helper = v;

}

/** Handles merge vertex.
*/
void handle_merge_vertex(dcel& G, set<half_edge*, compare_edges_left >& tau, vertex* v){

	/* Selecting the half-edge ending at v that bounds the polygon interior. */
	half_edge* e = (v->incident_edge->incident_face == NULL)? v->incident_edge->twin : v->incident_edge->prev ;

	/* If merge vertex, insert diagonal */
	if(check_vertex_type(G, e->helper) == MERGE_VERTEX)
		G.split_face(e, v, e->helper);

	/* Deleting edge from status */
	tau.erase(e);

	/* ASSUMPTION: v->incident_edge will never be a half_edge belonging to an interior diagonal
	* This is because the incident_edge attribute never gets updated after initialization of the graph. */
	half_edge* h = v->incident_edge;
	if(h->incident_face == NULL){
		h = h->prev->twin->prev; 				/* Can't use a different way to set h because a diagonal was added in the first half of this function */
	}else{
		h = h->prev;
	}

	/* Finding edge to the immediate left of v */
	e = find_immediate_left(tau, v);
	if(check_vertex_type(G, e->helper) == MERGE_VERTEX)
		G.split_face(h, v, e->helper);

	/* Updating helper of imm. left edge */
	e->helper = v;
}

/** Handles regular vertices.
*/
void handle_regular_vertex(dcel& G, set<half_edge*, compare_edges_left >& tau, vertex* v){

	/* If the interior of the polygon lies to the right of v */
	if(G.interior_is_right_of(v)){							//FUNCTION NEEDS TO BE WRITTEN

		/* Selecting the half-edge ending at v that bounds the polygon interior. */
		half_edge* e = (v->incident_edge->incident_face == NULL)? v->incident_edge->twin : v->incident_edge->prev ;

		/* If merge vertex, insert diagonal */
		if(check_vertex_type(G, e->helper) == MERGE_VERTEX)
			G.split_face(e, v, e->helper);

		/* Deleting edge from status */
		tau.erase(e);

		/* Selecting the half-edge originating from v that bounds the polygon interior as e */
		half_edge* h = (v->incident_edge->incident_face == NULL)? v->incident_edge->prev->twin: v->incident_edge ;

		/* Inserting edge into status */
		tau.insert(h);
		h->helper = v;
	}
	else{

		/* Finding edge to the immediate left of v */
		half_edge* e;
		e = find_immediate_left(tau, v);

		half_edge* h = (v->incident_edge->incident_face == NULL)? v->incident_edge->twin : v->incident_edge->prev ;

		//if(e == NULL) cout << "NULL\n";													//PRINT
		if(check_vertex_type(G, e->helper) == MERGE_VERTEX)
			G.split_face(h, v, e->helper);

		/* Updating helper of imm. left edge */
		e->helper = v;
	}
}

/** Finds the edge immediately to the left of the given vertex
* in the status structure, tau (Uses binary search).
*/
half_edge* find_immediate_left(set<half_edge*, compare_edges_left >& tau, vertex* v){

	//cout << "Entered function: IMMEDIATE LEFT for vertex: " << v->id << endl;				//PRINT

	/* Creating a dummy half-edge with v as origin (for comparison) */
	half_edge e(-1, v);

	/* NOTE: There is no error checking for the case that there might not be any
	* edge to the left of v in tau, because it is assumed the algorithm will
	* never call this function on a vertex that does not have an edge to its
	* left in tau.
	*/
	set<half_edge*, compare_edges_left >::iterator it = tau.lower_bound(&e);
	it--;

	//cout << "Exited function: IMMEDIATE LEFT for vertex: " << v->id << endl;				//PRINT
	return (*it);
}