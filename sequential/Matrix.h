#ifndef MATRIX_H_
#define MATRIX_H_

class Matrix  {
public:
	int dim;
	bool** data = NULL;

	Matrix(int d)  {
		dim = d;
		data = new bool*[dim];

		for(unsigned i = 0; i < dim; i++)
			data[i] = new bool[dim];

		for(unsigned i = 0; i < dim; i++)
			for(unsigned j = 0; j < dim; j++)
				data[i][j] = false;
	}

	~Matrix()  {
		for(unsigned i = 0; i < dim; i++)
			delete data[i];

		delete [] data;
	}
};

#endif