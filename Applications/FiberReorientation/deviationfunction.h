#ifndef DEVIATIONFUNCTION_H
#define DEVIATIONFUNCTION_H
#include <vtkPoints.h>
#include <fstream>
#include <vtkSmartPointer.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_determinant.h>

class DeviationFunction: public vnl_least_squares_function
{
public: 
	DeviationFunction(int vectorsize=10, int num_res=10, bool with_grad=false);
	~DeviationFunction();

	virtual void f(vnl_vector<double> const &x, vnl_vector<double> &fx);
	virtual void trace(int iteration, vnl_vector< double > const &x, vnl_vector< double > const & fx);

	void SetX(vnl_vector< double> X){
		m_X = X;
	}
	void SetY(vnl_vector< double> Y){
		m_Y = Y;
	}
	void SetZ(vnl_vector< double> Z){
		m_Z = Z;
	}
	void SetSize(int size){
		vectorsize = size;
	}

private:
	vnl_vector< double> m_X;
	vnl_vector< double> m_Y;
	vnl_vector< double> m_Z;

	int vectorsize;
};

#endif // DEVIATIONFUNCTION_H