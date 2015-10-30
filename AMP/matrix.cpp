#include "matrix.h"
matrix::matrix()
{
	rows = 0;
	cols = 0;
	data = new double[0];
}

matrix::matrix(int row, int col)
{
	rows = row;
	cols = col;
	data = new double[row * col];
}

matrix& matrix::cpu_multiply(matrix* mat)
{
	matrix *res_mat = new matrix(rows, mat->cols);
	if (cols != mat->rows)
	{
		cout << "Matrices are not compatible";
		return (*res_mat);
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < mat->cols; j++)
		{
			double sum = 0;
			for (int k = 0; k < cols; k++)
			{
				sum += data[i * cols + k] * mat->data[k * mat->cols + j];
			}
			res_mat->set_val(i, j, sum);
		}
	}
	return (*res_mat);
}

void matrix::print()
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
			cout << get_val(i, j) << "\t";
		cout << endl;
	}
}

void matrix::random(double minv, double maxv)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			double v = minv + (double)rand() / ((double)RAND_MAX / (double)(maxv - minv));
			set_val(i, j, v);
		}
	}
}

void matrix::set_val(int i, int j, double v)
{
	if (i >= rows || j >= cols || i < 0 || j < 0)
	{
		cout << "index out of bounds";
	}
	data[i * cols + j] = v;
}

double matrix::get_val(int i, int j)
{
	if (i >= rows || j >= cols || i < 0 || j < 0)
	{
		cout << "index out of bounds";
	}
	return data[i * cols + j];
}

matrix::~matrix()
{
	if (data != 0){ delete[] data;		data = NULL; }
}