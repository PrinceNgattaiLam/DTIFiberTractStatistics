#include "deviationfunction.h"

DeviationFunction::DeviationFunction(int vectorsize, int num_res, bool with_grad):vnl_least_squares_function(vectorsize, num_res, with_grad ? use_gradient : no_gradient){}
DeviationFunction::~DeviationFunction(){}

void DeviationFunction::f (vnl_vector< double > const &x, vnl_vector< double > &fx)
  {
  	vnl_vector< double > mw_X(this->vectorsize); 	//With Weight
  	vnl_vector< double > mw_Y(this->vectorsize);	//With Weight
  	vnl_vector< double > mw_Z(this->vectorsize);	//With Weight

  	double meanX, meanY, meanZ;
  	double VarX=0, VarY=0, VarZ=0, covXY=0, covXZ=0, covYZ=0;

  	for (int i = 0; i < vectorsize; ++i)
  	{
  		mw_X(i) = this->m_X(i)* x(i);
  		mw_Y(i) = this->m_Y(i)* x(i);
  		mw_Z(i) = this->m_Z(i)* x(i);
  	}
  	meanX = mw_X.mean();
  	meanY = mw_Y.mean();
  	meanZ = mw_Z.mean();

  	for (int i = 0; i < vectorsize; ++i)
  	{
  		VarX += pow((mw_X(i) - meanX),2);
  		VarY += pow((mw_Y(i) - meanY),2);
  		VarZ += pow((mw_Z(i) - meanZ),2);
  		covXY += (mw_X(i) - meanX)*((mw_Y(i) - meanY));
  		covXZ += (mw_X(i) - meanX)*((mw_Z(i) - meanZ));
  		covYZ += (mw_Y(i) - meanY)*((mw_Z(i) - meanZ));

  	}
  	VarX/=this->vectorsize;
  	VarY/=this->vectorsize;
  	VarZ/=this->vectorsize;
  	covXY/=this->vectorsize;
  	covXZ/=this->vectorsize;
  	covYZ/=this->vectorsize;

  	vnl_matrix<double> F(3,3);
  	F(0,0)=VarX;	F(0,1)=covXY;	F(0,2)=covXZ;
  	F(1,0)=covXY;	F(1,1)=VarY;	F(1,2)=covYZ;
  	F(2,0)=covXZ;	F(1,2)=covYZ;	F(2,2)=VarZ;


  	fx(0)=vnl_determinant(F);
  	//fx(0)=1/fx(0);
    //fx(0) = pow(x(0) - 2, 2) + pow(x(1) - 4, 2);
  }

  void DeviationFunction::trace(int iteration, vnl_vector< double > const &x, vnl_vector< double > const & fx)
  {
	    cout << "Iteration " << iteration << endl
			 << "-------------" << endl;

  }