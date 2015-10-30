#include "matrix.h"
#include "Timer.h"
#include <conio.h>

bool pick_accelerator();

Timer timer;

int main()
{
	int n_start = 25;
	int end_n = 1600;
	int n_step = 2;

	//Timer First Time is Wrong 
	timer.Start();
	Sleep(1000);
	timer.Stop();
	double t = timer.Elapsed();

	if (pick_accelerator())
	{
		cout << "This Program compare GPU and CPU performance for 'N'*'N' matrix multiplication." << endl;
		cout << endl << "______________________________________________________" << endl << "N:\tCPU Time:\tGPU Time:\t\t" << endl << "______________________________________________________" << endl;
		for (int n = n_start; n <= end_n; n *= n_step)
		{
			matrix mat1(n, n), mat2(n, n);
			mat1.random(1, 100);
			mat2.random(1, 100);

			//GPU Time Calc
			timer.Start();
			matrix mc_g = mat1.gpu_multiply<32>(&mat2);
			timer.Stop();
			double gpuTime = timer.Elapsed();

			//CPU Time Calc
			timer.Start();
			matrix mc_c = mat1.cpu_multiply(&mat2);
			timer.Stop();
			double cpuTime = timer.Elapsed();

			std::cout << n<< "\t" << cpuTime << " ms\t" << gpuTime << " ms\t" << endl << endl;
		}
	}
	getch();
	return 0;
}

bool pick_accelerator()
{
	vector<accelerator> accs = accelerator::get_all();
	accelerator chosen_one;

	auto result =
		std::find_if(accs.begin(), accs.end(), [](const accelerator& acc)
	{
		return !acc.is_emulated &&
			acc.supports_double_precision &&
			!acc.has_display;
	});

	if (result != accs.end())
		chosen_one = *(result);

	wcout << "Selected Device Is: "<<chosen_one.description << endl << "-------------------------------------------------" << endl;

	bool success = accelerator::set_default(chosen_one.device_path);
	return success;
}