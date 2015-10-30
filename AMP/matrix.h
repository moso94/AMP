#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <amp.h>
#include <math.h>
#include <iostream>

using namespace std;

using namespace concurrency;

class matrix
{

	double *data;
	int rows, cols;

public:
	matrix();
	matrix(int row, int col);
	~matrix();
	template<int tile>
	matrix& gpu_multiply(matrix* mat);
	matrix& cpu_multiply(matrix* mat);
	void random(double minv, double maxv);


	void print();
	double get_val(int i, int j);
	void set_val(int i, int j, double v);
};


template<int tile>
matrix& matrix::gpu_multiply(matrix* mat)
{
	if (cols != mat->rows)
	{
		cout << "Matrices are not compatible";
	}
	matrix *res_mat = new matrix(rows, mat->cols);

	concurrency::extent<2> e_a(rows, cols), e_b(mat->rows, mat->cols), e_c(rows, mat->cols);

	array_view<const double, 2> d_a(e_a, this->data);
	array_view<const double, 2> d_b(e_b, mat->data);
	array_view<double, 2> d_c(e_c, res_mat->data);
	d_c.discard_data();

	concurrency::extent<2> res_domain(e_c);

	parallel_for_each(res_domain.tile<tile, tile>().pad(), [=](tiled_index<tile, tile> tile_idx) restrict(amp)
	{
		index<2> l_idx = tile_idx.local;
		index<2> g_idx = tile_idx.global;
		double val = 0;


		for (int i = 0; i < (d_a.extent[1]); i += tile)
		{
			tile_static double loc_A[tile][tile];
			tile_static double loc_B[tile][tile];

			if (g_idx[0] < d_a.extent[0] && i + l_idx[1] < d_a.extent[1])
				loc_A[l_idx[0]][l_idx[1]] = d_a(g_idx[0], i + l_idx[1]);
			else
				loc_A[l_idx[0]][l_idx[1]] = 0;

			if (g_idx[1] < d_b.extent[1] && i + l_idx[0] < d_b.extent[0])
				loc_B[l_idx[0]][l_idx[1]] = d_b(i + l_idx[0], g_idx[1]);
			else
				loc_B[l_idx[0]][l_idx[1]] = 0;

			tile_idx.barrier.wait_with_tile_static_memory_fence();

			for (int k = 0; k < tile; k++)
			{
				val += loc_A[l_idx[0]][k] * loc_B[k][l_idx[1]];
			}

			tile_idx.barrier.wait_with_tile_static_memory_fence();
		}
		if (g_idx[0] < d_c.extent[0] && g_idx[1] < d_c.extent[1])
		{
			d_c[tile_idx.global] = val;
		}
	});
	d_c.synchronize();

	return (*res_mat);
}

#endif