

template<typename T>
MatrixGraph<T>::MatrixGraph(bool oriented): IGraph<T>(0) 
{
	this->oriented = oriented;
}

template<typename T>
MatrixGraph<T>::MatrixGraph(unsigned int number_of_vertices, bool oriented): IGraph<T>(number_of_vertices) 
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
IGraph<T>* MatrixGraph<T>::clone() 
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
	T* new_data = (T*)malloc(size * size* sizeof(T));
	T* ptr = &data[0];
	T* new_ptr = new_data;
	for(int i = 0; i < this->number_of_vertices; i++) {
		int j = 0;
		for(; j < this->number_of_vertices; j++) {
			*(new_ptr++) = *(ptr++);
		}
		for(; j < size; j++) {
			*(new_ptr++) = inf;
		}
	}
	new_data[size * size - 1] = 0;
	
	data.setArray(new_data, size * size);
	this->number_of_vertices += new_number_of_vertecies;
}

template<typename T>
inline std::unique_ptr<typename IGraph<T>::NeighborIterator> MatrixGraph<T>::neighbors(unsigned int v) {
	return std::make_unique<MatrixNeighborIterator>(this, v);
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
MatrixGraph<T>::MatrixNeighborIterator::MatrixNeighborIterator(MatrixGraph* graph, unsigned int vertex)
: graph(graph), vertex(vertex), current(0)  {}

template<typename T>
bool MatrixGraph<T>::MatrixNeighborIterator::hasNext() {
	while (current == vertex || (current < graph->number_of_vertices && graph->data[vertex * graph->number_of_vertices + current] == inf)) {
	   ++current;
	}
	return current < graph->number_of_vertices;
}

template<typename T>
std::pair<unsigned int, T> MatrixGraph<T>::MatrixNeighborIterator::next() {
	unsigned int u = current;
	return {u, graph->data[vertex * graph->number_of_vertices + current++]};
};