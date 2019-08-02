// #include "dcel.cpp"
#include <algorithm>
#include <stack>
#include <unordered_map>

using namespace std;

#define LEFT 1
#define RIGHT 2

#define CCW 1
#define CW -1

void triangulate_m_polygon(dcel&, face*, half_edge*);

/** Triangulates all y-monotone polygons present in G.
*/
void triangulate_dcel(dcel& G){
	
	set<face*>::iterator it = G.faces.begin();
	set<face*> polygons;
	while(it != G.faces.end()){
		polygons.insert((*it));
		it++;
	}

	it = polygons.begin();
	while(it != polygons.end()){
		triangulate_m_polygon(G, (*it), (*it)->incident_edge);
		it++;
	}
	
}

/** Triangulates a y-monotone polygon given an interior half-edge.
*/
void triangulate_m_polygon(dcel& G, face* f, half_edge* e){

	// cout << "TRIANGULATING face " << f->id << " incd edge: " << e->id << endl;	//PRINT

	int ID = f->id;

	/* Finds the lowest vertex of the polygon */
	half_edge* curr = e;

	while(not (curr->prev->origin->is_above(curr->origin) and curr->next->origin->is_above(curr->origin)))
		curr = curr->next;
	vertex* lowest = curr->origin;

	/* Finds the highest vertex of the polygon */
	curr = e;
	while(curr->prev->origin->is_above(curr->origin) or curr->next->origin->is_above(curr->origin))
		curr = curr->next;
	vertex* highest = curr->origin;

	/* A hash table on vertex ID to store which chain the vertex belongs to */
	unordered_map<int, int> chain;

	/* Setting curr to the interior half_edge from the highest vertex */
	half_edge* x = e;
	while(x->origin != highest)
		x = x->next;

	/* Inserting vertices on left chain into hash map */
	curr = x->next;
	while(curr->origin != lowest){
		chain.insert(make_pair(curr->origin->id, LEFT));
		curr = curr->next;
	}

	/* Inserting vertices on right chain into hash map */
	curr = x->prev;
	while(curr->origin != lowest){
		chain.insert(make_pair(curr->origin->id, RIGHT));
		curr = curr->prev;
	}

	/* Merging vertices into a list, sorted by decreasing y co-ordinate */
	half_edge* y = x->prev;
	vector<vertex*> ordered_list;
	ordered_list.push_back(highest);
	x = x->next;

	while(x->origin != lowest or y->origin != lowest){

		/* If the left vertices are over */
		if(x->origin == lowest){
			while(y->origin != lowest){
				ordered_list.push_back(y->origin);
				y = y->prev;
			}
			continue;
		}
		/* If the right vertices are over */
		if(y->origin == lowest){
			while(x->origin != lowest){
				ordered_list.push_back(x->origin);
				x = x->next;
			}
			continue;
		}

		/* Otherwise insert the highest vertex */
		if(x->origin->coordinates.y >= y->origin->coordinates.y){
			ordered_list.push_back(x->origin);
			x = x->next;
		}
		else{
			ordered_list.push_back(y->origin);
			y = y->prev;
		}

	}
	ordered_list.push_back(lowest);

	/* Stack algorithm */
	stack<vertex*> s;
	vector<vertex*>::iterator it = ordered_list.begin();

	/* Pushing v1 and v2 onto the stack */
	s.push((*it));
	it++;
	s.push((*it));
	it++;

	while((*it) != ordered_list.back()){

		/* If elements are not on the same chain */
		if(chain.at((*it)->id) != chain.at((s.top())->id)){

			// cout << "Entered top \n";							//PRINT

			/* Storing top element to push later */
			vertex* u = s.top();
			while(s.size() != 1){
				vertex* v = s.top();
				s.pop();

				half_edge* h = v->incident_edge;
				while(h->incident_face->id != ID)
					h = h->twin->next;
				h = h->prev;

				G.split_face(h, v, (*it));
				ID = G.faces.size();
			}
			s.pop();
			s.push(u);
			s.push((*it));
		}
		/* If elements are on the same chain */
		else{

				// cout << "Entered bottom \n";						//PRINT

				/* Popping the first element */
				vertex* u = s.top();
				s.pop();

				/* Popping all the elements off */
				while(!s.empty()){
					vertex* v = s.top();

					/* Checking if diagonal lies inside the polygon */
					int direction = is_counter_clockwise(u->coordinates, v->coordinates, (*it)->coordinates);
					int interior_to_right = G.interior_is_right_of(v);

					if((interior_to_right and direction == CW) or (!interior_to_right and direction == CCW))
					{
						half_edge* h = (*it)->incident_edge;
						while(h->incident_face->id!= ID)
							h = h->twin->next;
						h = h->prev;

						/* Inserting diagonal between current vertex and top of stack */
						G.split_face(h, (*it), v);
						ID = G.faces.size();

						/* Popping top of stack */
						u = s.top();
						s.pop();
					}
					else
						break;
				}

				/* Pushing current element and last vertex to which diagonal was made */
				s.push(u);
				s.push((*it));
		}
		it++;
	}

	/* Popping the first element on the stack */
	s.pop();

	/* Inserting diagonals between current vertex and all on stack except last one */
	vertex* v = *it;
	while(s.size() != 1){

		vertex* u = s.top();
		s.pop();

		half_edge* h = v->incident_edge;
		while(h->incident_face->id != ID)
			h = h->twin->next;
		h = h->prev;

		/* Inserting diagonal */
		G.split_face(h, v, u);
		ID = G.faces.size();
	}

	/* Pop last element on stack */
	s.pop();
}