#include "fiberfeaturescreator.h"
#include "vtkCurvatures.h"
#include "vtkObjectFactory.h"
#include <sstream>


vtkStandardNewMacro(FiberFeaturesCreator);

FiberFeaturesCreator::FiberFeaturesCreator(){
	this->inputFibers = vtkSmartPointer<vtkPolyData>::New();
	this->outputFibers = vtkSmartPointer<vtkPolyData>::New();
    this->nbLandmarks = 0;
    this->arclengthOn = false;
    this->torsionsOn = false;
    this->curvaturesOn = false;
}

FiberFeaturesCreator::~FiberFeaturesCreator(){}

void FiberFeaturesCreator::SetInput(vtkSmartPointer<vtkPolyData> input)
{
	this->inputFibers = input;
}
void FiberFeaturesCreator::SetNbLandmarks(int nbLandmarks)
{
	this->nbLandmarks = nbLandmarks;
}

void FiberFeaturesCreator::init_output()
{
	this->outputFibers=this->inputFibers;
}

void FiberFeaturesCreator::Update()
{
	this->init_output();
	this->update_methods_state();

	if(this->arclengthOn)
	{

		this->compute_landmarks();
		this->compute_arclength_feature();

	}
	if(this->torsionsOn)
	{
		this->compute_torsions_feature();

	}
	if(this->curvaturesOn)
	{
		this->compute_curvatures_feature();

	}

}

// vtkSmartPointer<vtkPolyData> FiberFeaturesCreator::GetOutput()
// {
// 	return this->outputFibers;
// }


	
	
void FiberFeaturesCreator::WriteOutputFile(std::string filename)
{
	if (filename.rfind(".vtk") != std::string::npos)
        {
            vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
            //       fiberwriter->SetFileTypeToBinary();
            fiberwriter->SetFileName(filename.c_str());
#if (VTK_MAJOR_VERSION < 6)
            fiberwriter->SetInput(this->outputFibers);
#else
            fiberwriter->SetInputData(this->outputFibers);
#endif
            fiberwriter->Update();
        }
        // XML
        else if (filename.rfind(".vtp") != std::string::npos)
        {
            vtkSmartPointer<vtkXMLPolyDataWriter> fiberwriter = vtkXMLPolyDataWriter::New();
            fiberwriter->SetFileName(filename.c_str());
#if (VTK_MAJOR_VERSION < 6)
            fiberwriter->SetInput(this->outputFibers);
#else
            fiberwriter->SetInputData(this->outputFibers);
#endif
            fiberwriter->Update();
        }

}	

void FiberFeaturesCreator::compute_landmarks()//std::vector<std::vector<vtkPoints*> > compute_landmarks()
{
	//TEST SUR INPUTSFIBERS ETC...
	std::vector<vtkFloatArray*> arclengthMatrix;
	//std::vector<std::vector<double*> > landmarks;
	
	vtkSmartPointer<vtkDataArray> arclengthArray = this->inputFibers->GetPointData()->GetScalars("SamplingDistance2Origin");
	int k = 0;
	int NbPtCurrentFiber;
	int NbFibers = this->inputFibers->GetNumberOfCells();
	for(int i=0; i<NbFibers; i++)
	{
		vtkFloatArray* arclengthArrayCurrent = vtkFloatArray::New();
		NbPtCurrentFiber = this->inputFibers->GetCell(i)->GetPoints()->GetNumberOfPoints();
		for(int j=0; j<NbPtCurrentFiber; j++)
		{
			arclengthArrayCurrent->InsertNextTuple1(arclengthArray->GetTuple1(k));
			k++;
		}
		arclengthMatrix.push_back(arclengthArrayCurrent);
	}
	for(int i=0; i<NbFibers; i++)
	{
		std::vector<int> landmarksIndexArray = find_landmarks_index(arclengthMatrix[i]->GetNumberOfTuples() , this->nbLandmarks);
		std::vector<double*> landmarksPtToAdd;
		vtkPoints* pts = this->inputFibers->GetCell(i)->GetPoints();
		double* pt;
		for (int j = 0; j < this->nbLandmarks; ++j)
		{
			double* pt = new double[3];
			pt[0] = (pts->GetPoint(landmarksIndexArray[j]))[0];
			pt[1] = (pts->GetPoint(landmarksIndexArray[j]))[1];
			pt[2] = (pts->GetPoint(landmarksIndexArray[j]))[2];
			//std::cout<<"Pt  x = "<<pt[0]<<" y = "<<pt[1]<<" z = "<<pt[2]<<std::endl;
			landmarksPtToAdd.push_back(pt);
		}
		// for (int j = 0; j < this->nbLandmarks; ++j)
		// {
		// 	std::cout<<"LandmarksToAdd  x = "<<landmarksPtToAdd[j][0]<<" y = "<<landmarksPtToAdd[j][1]<<" z = "<<landmarksPtToAdd[j][2]<<std::endl;
		// }

		this->landmarks.push_back(landmarksPtToAdd);
	}



	// for(int i=0; i<this->landmarks.size(); i++)
	// {
	// 	for(int j=0; j<5; j++)
	// 	{
	// 		std::cout<<this->landmarks[i][j][0]<<" ";
	// 	}
	// 	std::cout<<std::endl<<std::endl<<std::endl;
	// }
		/* TEST CODE*/
	// puts("AFFICHAGE INFOS LANDMARKS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	// std::cout<<"Nb Fibres = "<<this->landmarks.size()<<std::endl;
	// std::cout<<"Nb LandMarks = "<<this->landmarks[0].size()<<std::endl;
	// std::cout<<"Nb LandMarks = "<<this->landmarks[1].size()<<std::endl;
	// std::cout<<"Nb LandMarks = "<<this->landmarks[2].size()<<std::endl;
	// std::cout<<"Nb LandMarks = "<<this->landmarks[3].size()<<std::endl;


}
// Method Arclength;
void FiberFeaturesCreator::compute_arclength_feature()
{
	//TEST ON THE METHOD AND IF LANDMARKS NON VOID AND INPUTS TOO

	int NbFibers = this->inputFibers->GetNumberOfCells();
	int NbPtOnFiber;
	std::vector<std::string> arclengthLabel;
	arclengthLabel.push_back("EuclidianDistance2Min");
	for(int k=1; k<this->nbLandmarks-1; k++)
	{

		std::string k_char = static_cast<std::ostringstream*>( &( std::ostringstream() << k) )->str();
		std::string nb_char = static_cast<std::ostringstream*>( &( std::ostringstream() << this->nbLandmarks-1) )->str();
		arclengthLabel.push_back("EuclidianDistance2_"+k_char+"/"+nb_char+"Max");	
		
	}
	arclengthLabel.push_back("EuclidianDistance2Max");

	double * p1;
	double * p0;
	double * p2;
	for(int k=0; k<this->nbLandmarks; k++)
	{
		vtkSmartPointer<vtkFloatArray> arclength = vtkFloatArray::New() ;
		arclength->SetName(arclengthLabel[k].c_str());
		for(int i=0; i<NbFibers; i++)
		{
			double* p1 = new double[3];
			p1[0] = this->landmarks[i][k][0];
			p1[1] = this->landmarks[i][k][1];
			p1[2] = this->landmarks[i][k][2];
			//p1 = this->landmarks[i][k];
			vtkPoints* pts = this->inputFibers->GetCell(i)->GetPoints();
			NbPtOnFiber = pts->GetNumberOfPoints();
			for(int j=0; j<NbPtOnFiber; j++)
			{
				double* p0 = new double[3];
				p0[0] = pts->GetPoint(j)[0];
				p0[1] = pts->GetPoint(j)[1];
				p0[2] = pts->GetPoint(j)[2];
				//p0 = pts->GetPoint(j);
				double tuple = sqrt((p0[0]-p1[0])*(p0[0]-p1[0])+(p0[1]-p1[1])*(p0[1]-p1[1])+(p0[2]-p1[2])*(p0[2]-p1[2]));
				arclength->InsertNextTuple1(tuple);
				// std::cout<<"P0 x="<<p0[0]<<" y="<<p0[1]<<" z="<<p0[2]<<std::endl;
				// std::cout<<"P1 x="<<p1[0]<<" y="<<p1[1]<<" z="<<p1[2]<<std::endl;
			}

		}
		this->outputFibers->GetPointData()->SetActiveScalars(arclengthLabel[k].c_str());
		this->outputFibers->GetPointData()->SetScalars(arclength);

	}
	
}

std::vector<int>  find_landmarks_index(int nbSamples , int nbLandmarks)
{
	std::vector<int> index;

	for(int k=0; k < nbLandmarks; k++)
	{
		index.push_back(k*(nbSamples/(nbLandmarks-1)));
	}
	return index;
}

// Method Torsions;	
void FiberFeaturesCreator::compute_torsions_feature()
{
	
}

// Method Curvature;	
void FiberFeaturesCreator::compute_curvatures_feature()
{

}

void FiberFeaturesCreator::update_methods_state()
{
	arclengthOn = true;
    torsionsOn = false;
    curvaturesOn = false;
}


