#include "point.cpp"
#include <vector>
#include <fstream>
using namespace std;

class vertex;
class face;
class half_edge;

/** Stores a graph.
*/
class dcel{
public:
	set<half_edge*> half_edges;					/**< Set of half edges in the graph. */
	set<face*> faces;							/**< Set of faces in the graph. */
	set<vertex*> vertices;						/**< Set of vertices in the graph. */

	dcel(vector<point>);

	void print();

	/** Given a half-edge h that ends at u, and two vertices u, and v,
	 this inserts an edge between u and v and splits the face into two.
	*/
	void split_face(half_edge*, vertex*, vertex*);

	/** Checks if an edge exists between two pairs of vertices. */
	bool edge_exists(vertex*, vertex*);

	/** Returns true if polygon stored in DCEL is to the right of vertex v
	 NOTE: assumes that there is only one polygon stored in the DCEL. 
	*/
	bool interior_is_right_of(vertex*);

	void printGraph(string s);

};

/** Stores a face of a graph 
*/
class face{
public:
	half_edge* incident_edge;				/**< Stores a pointer to any incident edge on this face. */
	int id;									/**< Stores the face ID. */

	/** Prints the attributes of this face. */
	void print();

	/** Method to print the attributes of a given face object.
	*/
	face(){
		id = -1;
		incident_edge = NULL;
	}

	face(int i, half_edge* he){
		id = i;
		incident_edge = he;
	}
};

/** Stores an edge of a graph.
*/
class half_edge{
public:
	vertex* origin;					/**< Stores a pointer to the origin pt of this half_edge. */
	half_edge* twin;				/**< Stores a pointer to this half edge's twin half edge. */
	half_edge* next;				/**< Stores a pointer to next half_edge along incident face. */
	half_edge* prev;				/**< Stores a pointer to previous half_edge along incident face. */
	face* incident_face;			/**< Stores a pointer to the face upon which this half edge is incidient upon. */
	int id;							/**< Stores the face ID. */

	vertex* helper;					/**< Stores a pointer to the helper vertex of this half_edge. (Used during polyogon monotonization.)*/

	/** Method to print the attributes of a half-edge parameter.
	NOTE: this does not print the helper attribute of the half-edge.
	*/
	void print();

	half_edge(){
		origin = NULL;
		twin = NULL;
		next = NULL;
		prev = NULL;
		incident_face = NULL;
		id = -1;
		helper = NULL;
	}

	half_edge(int i, vertex* o){
		origin = o;
		id = i;
		twin = NULL;
		next = NULL;
		prev = NULL;
		incident_face = NULL;
		helper = NULL;
	}
};

/** Stores a vertex of a polygon.
*/
class vertex{
public:
	point coordinates;					/**< Stores a point representing this vertex. */
	half_edge* incident_edge;			/**< Stores a pointer to any half_edge that is incident from this vertex. */
	int id;								/**< Stores this vertex ID. */

	vertex(float x, float y){
		point p(x, y);
		coordinates = p;
		incident_edge = NULL;
		id = -1;
	}

	vertex(float x, float y, int i){
		point p(x, y);
		coordinates = p;
		incident_edge = NULL;
		id = i;
	}

	vertex(point p, int i){
		coordinates = p;
		incident_edge = NULL;
		id = i;
	}

	/** Method to print out the attributes of a given vertex.
	*/
	void print(){
		cout << "| Vertex v" << id << " | \n\t";
		cout << "Coords: (" << coordinates.x << ", " << coordinates.y << ")\n\t";
		if(incident_edge == NULL)
			cout << "Incid. edge: null\n";
		else
			cout << "Incid. edge: e" << incident_edge->id << endl;
	}

	/** Checks if this verte is above a given vertex passed as a paramter.
	*/
	bool is_above(vertex* v){
		if(this->coordinates.is_above(v->coordinates))
			return true;
		else return false;
	}
};

/** Method to print the attributes of a half-edge parameter.
	NOTE: this does not print the helper attribute of the half-edge.
*/
void half_edge :: print(){
		cout << "| Half-edge e" << id << " | \n";

		if(origin == NULL) cout << "\t Origin: null\n";
		else cout << "\t Origin: v" << origin->id << "\n";

		if(twin == NULL) cout << "\t Twin: null\n";
		else cout << "\t Twin: e" << twin->id << "\n";

		if(next == NULL) cout << "\t Next: null\n";
		else cout << "\t Next: e" << next->id << "\n";

		if(prev == NULL) cout << "\t Prev: null\n";
		else cout << "\t Prev: e" << prev->id << "\n";

		if(incident_face == NULL) cout << "\t Incid. face: null\n";
		else cout << "\t Incid. face: f" << incident_face->id << "\n";
	}

/** Method to print the attributes of a given face object.
*/
void face :: print(){
	cout << "| Face f" << id << " | \n";

	if(incident_edge == NULL) cout << "\t Incid. edge: null\n";
	else cout << "\t Incid. edge: e" << incident_edge->id << "\n";
}

/** Method to print to command line the atttributes of a given dcel instance.
*/
void dcel :: print(){
	cout << "- - - - - - - - - - - - - -\n";
	cout << "- - - - - FACES - - - - - -\n";
	set<face*>::iterator it = faces.begin();
	while(it != faces.end()){
		(*it)->print();
		it++;
	}
	cout << "- - - -  VERTICES - - - - -\n";
	set<vertex*>::iterator it2 = vertices.begin();
	while(it2 != vertices.end()){
		(*it2)->print();
		it2++;

	}
	cout << "- - - - - EDGES - - - - - -\n";
	set<half_edge*>::iterator it3 = half_edges.begin();
	while(it3 != half_edges.end()){
		(*it3)->print();
		it3++;
	}
	cout << "- - - - - - - - - - - - - -\n";
}

/** Checks if an edge exists between two pairs of vertices.
*/
bool dcel :: edge_exists(vertex* u, vertex* v){

	half_edge* h = u->incident_edge;

	if(h->twin->origin == v){
		return true;
	}

	half_edge* curr = h->twin->next;
	while(curr != h){
		if(h->twin->origin == v)
			return true;
		curr = curr->twin->next;
	}

	return false;
}

/** Given a half-edge h that ends at u, and two vertices u, and v,
 this inserts an edge between u and v and splits the face into two.
*/
void dcel :: split_face(half_edge* h, vertex* u, vertex* v){

	/* NOTE: The pseudocode for this algorithm was implemented with the help of the guide
	*  found here: https://www.ti.inf.ethz.ch/ew/lehre/CG12/lecture/Chapter%205.pdf.
	*  Refer to the 'split-face' algorithm.
	*/

	/* First check that there is no edge already existing between u and v */
	if(edge_exists(u, v))
		return;

	// cout << "Entered function: SPLIT FACE for hedge: " << h->id << " vertices: " << u->id << " and " << v->id << endl;				//PRINT

	/* A new face is added to the faces list*/
	face* old_face = h->next->incident_face;
	face* new_face = new face();
	new_face->id = faces.size() + 1;
	faces.insert(new_face);

	/* Two new half edges are added to the edge list */
	half_edge* h1 = new half_edge();
	half_edge* h2 = new half_edge();

	h1->incident_face = old_face;

	h1->id = half_edges.size() + 1;
	h2->id = half_edges.size() + 2;
	half_edges.insert(h1);
	half_edges.insert(h2);

	/* Updating incident edges of the faces */
	old_face->incident_edge = h1;
	new_face->incident_edge = h2;

	/* Setting the two new half edges as twins of each other */
	h1->twin = h2;
	h2->twin = h1;

	/* Setting origins of the new half-edges */
	h1->origin = u;
	h2->origin = v;

	/* Updating attributes on upper left part of edge */
	h2->next = h->next;
	h2->next->prev = h2;

	/* Updating attributes on lower left part of edge */
	h1->prev = h;
	h->next = h1;

	/* Updating incident face of all edges in the cycle including h2 */
	half_edge* i = h2;
	while(i->twin->origin != v){
		// cout << "i id: " << i->id << endl;
		i->incident_face = new_face;
		i = i->next;
	}
	i->incident_face = new_face;

	/* Updating attributes on upper right part of edge */
	h1->next = i->next;
	h1->next->prev = h1;

	/* Updating attributes on lower right part of edge */
	i->next = h2;
	h2->prev = i;

	//testing
	// i = h1;
	// while(h1->twin->origin != u){
	// 	i->incident_face = new_face;
	// 	i = i-> next;
	// }
	// i->incident_face = new_face;

	// cout << "Exited function: SPLIT FACE for vertices: " << u->id << "and" << v->id << endl;				//PRINT
}


/** Returns true if polygon stored in DCEL is to the right of vertex v
 NOTE: assumes that there is only one polygon stored in the DCEL. 
*/
bool dcel :: interior_is_right_of(vertex* v){

	//cout << "Entered function: interior is right of for vertex: " << v->id << endl;				//PRINT

	/* Find the edge originating from v that  bounds the polygon interior */
	half_edge* e = v->incident_edge;
	if(e->incident_face == NULL)
		e = e->twin->next;

	/* If origin pt is higher than its endpt, the interior lies on the right */
	if(e->origin->coordinates.y > e->twin->origin->coordinates.y){
		//cout << "Exited function: interior is right of for vertex: " << v->id << endl;			//PRINT
		return true;
	}
	else{
		//cout << "Exited function: interior is right of for vertex: " << v->id << endl;			//PRINT
		return false;	
	}

}

/** Prints graph to .dat file for gnuplotting
*/
void dcel :: printGraph(string s){
		ofstream myfile;
		myfile.open(s);

		/* Iterating through all the faces */
		set<face*>::iterator it = faces.begin();
		while(it != faces.end()){

			/* Iterating through all the edges in a face */
			int iter = 1;
			half_edge* e = (*it)->incident_edge;
			half_edge* curr = (*it)->incident_edge;
			while(1){
				// curr->print();

				// cout << iter++ << " \n";
				myfile << curr->origin->coordinates.x << " ";
				myfile << curr->origin->coordinates.y << "\n";
 
				myfile << curr->twin->origin->coordinates.x << " ";
				myfile << curr->twin->origin->coordinates.y << "\n\n";

				curr = curr->next;

				if(curr == e)
					break;
			}
			it++;
		}
		myfile.close();
	}

dcel::dcel(vector<point> p){

	vector<vertex*> V;
	vector<face*> F;
	vector<half_edge*> H;

	int pointid = 0;

	/* Inserting vertices */
	for(int i = 0; i < p.size(); i++){
		V.push_back(new vertex(p[i], pointid++));
	}

	/* Inserting half_edges */
	int edgeid = 0;
	for(int i = 0; i < V.size(); i++){
		half_edge* curr = new half_edge(edgeid++, V[i]);
		H.push_back(curr);
		V[i]->incident_edge = curr;

		if(i > 0)
			V[i]->incident_edge->prev = V[i-1]->incident_edge;
	}
	V[0]->incident_edge->prev = V[V.size()-1]->incident_edge;

	/* Setting the next variable of half_edges */
	for(int i = 0; i < V.size(); i++){
		if(i == V.size() - 1){
			V[i]->incident_edge->next = V[0]->incident_edge;
		}
		else{
			V[i]->incident_edge->next = V[i+1]->incident_edge;
		}
	}

	/* Creating twin half_edges */
	for(int i = 0; i < V.size(); i++){
		half_edge* temp = new half_edge(edgeid++, V[i]);
		H.push_back(temp);

		int index = (i == 0)? V.size() - 1 : i - 1;

		temp->twin = V[index]->incident_edge;
		V[index]->incident_edge->twin = temp;
	}

	/* Setting next of twin half_edges */
	half_edge* next = NULL;
	for(int i = 0; i < V.size(); i++){

		int nindex = (i == 0)? V.size() - 1 : i - 1;
		V[i]->incident_edge->twin->next = V[nindex]->incident_edge->twin;

		int pindex = (i == V.size()-1)? 0: i + 1;
		V[i]->incident_edge->twin->prev = V[pindex]->incident_edge->twin; 
	}

	/* Pushing back a new face pointer */
	F.push_back(new face(1, V[0]->incident_edge));
	for(int i = 0; i < V.size(); i++){
		V[i]->incident_edge->incident_face = F[0];
	}

	/* Copying all vertices */
	for(int i = 0; i < V.size(); i++){
		vertices.insert(V[i]);
	}

	/* Copying all half_edges */
	for(int i = 0; i < H.size(); i++){
		half_edges.insert(H[i]);
	}

	/* Copying all face records */
	for(int i = 0; i < F.size(); i++){
		faces.insert(F[i]);
	}
}