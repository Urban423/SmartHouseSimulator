

template<typename T>
MatrixGraph<T>::MatrixGraph(bool oriented): IGraph<T, MatrixNeighborIterator<T>>(0) 
{
	this->oriented = oriented;
}

template<typename T>
MatrixGraph<T>::MatrixGraph(unsigned int number_of_vertices, bool oriented): IGraph<T, MatrixNeighborIterator<T>>(number_of_vertices) 
{
	data.resize(number_of_vertices * number_of_vertices);
	this->oriented = oriented;
	for(unsigned int i = 0; i < number_of_vertices; ++i) {
		for(unsigned int j = 0; j < number_of_vertices; ++j) {
			if(i == j) { data[i * number_of_vertices + j] = 0; continue;}
			data[i * number_of_vertices + j] = inf;
		}
	}
};

template<typename T>
IGraph<T, MatrixNeighborIterator<T>>* MatrixGraph<T>::clone() 
{
	MatrixGraph<T>* copy = new MatrixGraph<T>(this->number_of_vertices);
	copy->data = this->data; 
	return copy;
}


template<typename T>
inline T MatrixGraph<T>::getEdge(unsigned int i, unsigned int j) {
	return data[i * this->number_of_vertices + j];
};

template<typename T>
inline void MatrixGraph<T>::addEdge(unsigned int i, unsigned int j, T w) {
	if(w == inf) {
		data[j * this->number_of_vertices + i] = w;
		return;
	}
	data[i * this->number_of_vertices + j] = w;
	if(this->oriented) {
		data[j * this->number_of_vertices + i] = w;
	}
}

template<typename T>
inline void MatrixGraph<T>::setEdge(unsigned int i, unsigned int j, T w) {
	data[i * this->number_of_vertices + j] = w;
	if(this->oriented) {
		data[j * this->number_of_vertices + i] = w;
	}
}

template<typename T>
void MatrixGraph<T>::addVertecies(int new_number_of_vertecies) 
{
	int size = this->number_of_vertices + new_number_of_vertecies;
	std::vector<T> new_data(size * size, inf);

	T* ptr = &data[0];
	T* new_ptr = &new_data[0];
	for(int i = 0; i < this->number_of_vertices; i++) {
		for(int j = 0; j < this->number_of_vertices; j++) {
			*(new_ptr++) = *(ptr++);
		}
		new_ptr += new_number_of_vertecies;
	}

	data = std::move(new_data);
	this->number_of_vertices = size;
}

template<typename T>
inline MatrixNeighborIterator<T> MatrixGraph<T>::neighbors(unsigned int v) {
	return MatrixNeighborIterator(this, v);
}

template<typename T>
void MatrixGraph<T>::clear() {
	for(unsigned int i = 0; i < this->number_of_vertices; ++i) {
		for(unsigned int j = 0; j < this->number_of_vertices; ++j) {
			if(i == j) { data[i * this->number_of_vertices + j] = 0; continue;}
			data[i * this->number_of_vertices + j] = inf;
		}
	}
}





//iterator
template<typename T>
MatrixNeighborIterator<T>::MatrixNeighborIterator(MatrixGraph<T>* graph, unsigned int vertex)
: graph(graph), vertex(vertex), current(0)  {}

template<typename T>
bool MatrixNeighborIterator<T>::hasNext() {
	while (current == vertex || (current < graph->size() && graph->data[vertex * graph->size() + current] == inf)) {
	   ++current;
	}
	return current < graph->size();
}

template<typename T>
std::pair<unsigned int, T> MatrixNeighborIterator<T>::next() {
	unsigned int u = current;
	return {u, graph->data[vertex * graph->size() + current++]};
};