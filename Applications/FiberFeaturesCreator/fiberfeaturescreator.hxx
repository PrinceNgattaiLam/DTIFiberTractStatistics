#include "fiberfeaturescreator.h"
#include "vtkObjectFactory.h"
#include <vtkSimplePointsWriter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkTetra.h>
#include <string> 
#include <fstream>

#define NB_LINES 250
#define NB_WORDS 250

// #define BLUE "\033[0;34m"
// #define RED "\033[0;31m"
// #define GREEN_BOLD "\033[0;32m"
// #define YELLOW "\033[0;33m"
// #define NC "\033[0m"





vtkStandardNewMacro(FiberFeaturesCreator);

FiberFeaturesCreator::FiberFeaturesCreator(){
	this->inputFibers = vtkSmartPointer<vtkPolyData>::New();
	this->outputFibers = vtkSmartPointer<vtkPolyData>::New();
	this->modelFibers = vtkSmartPointer<vtkPolyData>::New();
	this->avgLandmarks = vtkSmartPointer<vtkPoints>::New();
    this->nbLandmarks = 0;
    this->landmarkOn = false;
    this->torsionsOn = false;
    this->curvaturesOn = false;
    this->fcsvPointsOn = false;
    this->vtPointsOn = false;
    this->landmarksFilename = "";
}

FiberFeaturesCreator::~FiberFeaturesCreator(){}

void FiberFeaturesCreator::SetInput(std::string input, std::string model, std::string landmarkfile, std::string output)
{
	this->inputFibers = readVTKFile(input);

	if (!model.empty())
	{
		this->modelFibers = readVTKFile(model);
		this->landmarksFilename = output.substr(0,output.rfind("."))+"_landmarks";
	}
	
	else if (landmarkfile.empty())
	{
		this->fcsvPointsOn = false;
		this->vtPointsOn = false;
	}
	else if (landmarkfile.rfind(".vtk")!=std::string::npos||landmarkfile.rfind(".vtp")!=std::string::npos)
	{
		this->vtPointsOn = true;
		this->landmarksFilename = landmarkfile;
	}
	else if (landmarkfile.rfind(".fcsv")!=std::string::npos)
	{
		this->fcsvPointsOn = true;
		this->landmarksFilename = landmarkfile;
	}
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
	if(fcsvPointsOn)
	{
		std::cout<<std::endl<<GREEN_BOLD<<"---FCSV File found, Computing landmarks from the FCSV File"<<NC<<std::endl;
		this->compute_landmarks_from_fcsv();
		this->compute_landmarks_average();
	}
	else if(vtPointsOn)
	{
		std::cout<<std::endl<<GREEN_BOLD<<"---VTK File found, Computing landmarks from the VTK File"<<NC<<std::endl;
		this->compute_landmarks_from_vtk_vtp();
		this->compute_landmarks_average();
	}
	else
	{

		std::cout<<std::endl<<GREEN_BOLD<<"---No VTK or FCSV File for the landmarks, Computing landmarks from the model"<<NC<<std::endl;
		this->compute_landmarks_from_model();
		this->compute_landmarks_average();
	}

	if(this->landmarkOn)
	{
		std::cout<<std::endl;
		puts("Computing Distance To Landmarks Features of the Fibers");
		puts("******************************************************");
		this->compute_landmark_feature();
		if(!fcsvPointsOn && !vtPointsOn)
			this->write_landmarks_file();
	

	}
	if(this->torsionsOn)
	{
		std::cout<<std::endl;
		puts("Computing Torsions Features of the Fibers");
		puts("*****************************************");
		this->compute_torsions_feature();

	}
	if(this->curvaturesOn)
	{
		std::cout<<std::endl;
		puts("Computing Curvatures Features of the Fibers");
		puts("*****************************************");
		this->compute_curvatures_feature();

	}

}

vtkSmartPointer<vtkPolyData> FiberFeaturesCreator::GetOutput()
{
	return this->outputFibers;
}

void FiberFeaturesCreator::compute_landmarks_from_model()//std::vector<std::vector<vtkPoints*> > compute_landmarks()
{
	//TEST SUR INPUTSFIBERS ETC...
	std::vector<vtkFloatArray*> arclengthMatrix;
	//std::vector<std::vector<double*> > landmarks;

	vtkSmartPointer<vtkDataArray> arclengthArray = this->modelFibers->GetPointData()->GetScalars("SamplingDistance2Origin");
	int NbPtCurrentFiber;
	int NbFibers = this->modelFibers->GetNumberOfCells();
	int k = 0;
	for(int i=0; i<NbFibers; i++)
	{
		vtkFloatArray* arclengthArrayCurrent = vtkFloatArray::New();
		NbPtCurrentFiber = this->modelFibers->GetCell(i)->GetPoints()->GetNumberOfPoints();
		for(int j=0; j<NbPtCurrentFiber; j++)
		{
			if(!arclengthArray) 
			{
				throw itk::ExceptionObject("Error during the extraction of the scalars 'SamplingDistance2Origin' in the model. Please check that the model has the model fiber is valid (Must have 'SamplingDistance2Origin' as a scalar parameter)");
			}

			arclengthArrayCurrent->InsertNextTuple1(arclengthArray->GetTuple1(k));
			arclengthArray->GetTuple1(k);
			k++;
		}
		arclengthMatrix.push_back(arclengthArrayCurrent);
	}
	for(int i=0; i<NbFibers; i++)
	{
		std::vector<int> landmarksIndexArray = find_landmarks_index(arclengthMatrix[i]->GetNumberOfTuples() , this->nbLandmarks);
		//std::cout<<"Size = "<<landmarksIndexArray.size()<<"   ";
		vtkSmartPointer<vtkPoints> landmarksPtToAdd = vtkSmartPointer<vtkPoints>::New();
		//landmarksPtToAdd->SetNumberOfPoints(this->nbLandmarks);
		vtkPoints* pts = this->modelFibers->GetCell(i)->GetPoints();
		double* pt;
		for (int j = 0; j < this->nbLandmarks; ++j)
		{
			double* pt = new double[3];
			vtkIdList* pid;
			pt[0] = (pts->GetPoint(landmarksIndexArray[j]))[0];
			pt[1] = (pts->GetPoint(landmarksIndexArray[j]))[1];
			pt[2] = (pts->GetPoint(landmarksIndexArray[j]))[2];
			//std::cout<<"Pt  x = "<<pt[0]<<" y = "<<pt[1]<<" z = "<<pt[2]<<std::endl;
			landmarksPtToAdd->InsertNextPoint(pt[0],pt[1],pt[2]);
		}

		this->landmarks.push_back(landmarksPtToAdd);
	}

}
void FiberFeaturesCreator::compute_landmarks_from_vtk_vtp()
{
	vtkSmartPointer<vtkPolyData> vtkLandmarks = readVTKFile(this->landmarksFilename);
	int NbPoints = vtkLandmarks->GetNumberOfPoints();
	vtkSmartPointer<vtkPoints> point = vtkSmartPointer<vtkPoints>::New();
	for(int i=0; i<NbPoints; ++i)
	{
		point->InsertNextPoint(vtkLandmarks->GetPoint(i)[0],vtkLandmarks->GetPoint(i)[1],vtkLandmarks->GetPoint(i)[2]);
	}
	this->landmarks.push_back(point);

}
void FiberFeaturesCreator::compute_landmarks_from_fcsv()
{
	std::fstream fcsvfile(this->landmarksFilename.c_str());
	std::string line;
	std::string mot;
	std::string words[NB_LINES][NB_WORDS]; // !!!! WARNING DEFINE AND TO PROTECT IF SUPERIOR TO 20
	int i,j;
	vtkSmartPointer<vtkPoints> landmarksPtToAdd = vtkSmartPointer<vtkPoints>::New();
	
	if(fcsvfile)
	{
		getline(fcsvfile, line);
		fcsvfile>>mot;
		while(mot=="#")
		{
			if(getline(fcsvfile, line))
				fcsvfile>>mot;
			else
				mot="#";
		}

		i=0;
		do
		{
			
			std::size_t pos_end;// = mot.find(",,");
			std::size_t pos1;
			j=0;
			do
			{
				std::size_t pos0 = 0;
				pos1 = mot.find(',');
				pos_end = mot.find(",,");
				words[i][j] = mot.substr(pos0, pos1-pos0);
				mot = mot.substr(pos1+1);
				j++;
			}
			while(pos1+1<pos_end);
			i++;
		}
		while(fcsvfile>>mot);
		int NbPoints = i;
		int NbFibers = this->inputFibers->GetNumberOfCells();
		this->SetNbLandmarks(NbPoints);
		for (int i = 0; i < NbPoints; ++i)
		{
			double x = atof(words[i][1].c_str());
			double y = atof(words[i][2].c_str());
			double z = atof(words[i][3].c_str());
			landmarksPtToAdd->InsertNextPoint(x,y,z);
		}
		for (int i = 0; i < NbFibers; ++i)
		{
			this->landmarks.push_back(landmarksPtToAdd);
		}
	}
	else
	{
		std::cout<<"Error !";
	}
}

// Method landmark;
void FiberFeaturesCreator::compute_landmark_feature()
{
	int NbFibers=this->inputFibers->GetNumberOfCells();
	if(NbFibers < 0)
	{
		throw itk::ExceptionObject("Empty input fiber");
	}

	int NbPtOnFiber;
	std::vector<std::string> landmarkLabel;
	for(int k=1; k<this->nbLandmarks+1; k++)
	{

		std::string k_char = static_cast<std::ostringstream*>( &( std::ostringstream() << k) )->str();
		landmarkLabel.push_back("Distance2Landmark"+k_char);
		
	}

	double * p1;
	double * p0;
	double * p2;
	for(int k=0; k<this->nbLandmarks; k++)
	{
		vtkSmartPointer<vtkFloatArray> dist2landmark = vtkFloatArray::New() ;
		
		std::cout<<"---Computing Scalar "<<landmarkLabel[k]<<std::endl;
		dist2landmark->SetName(landmarkLabel[k].c_str());
		for(int i=0; i<NbFibers; i++)
		{
			double* p1 = new double[3];

				p1[0] = (this->avgLandmarks->GetPoint(k))[0];
				p1[1] = (this->avgLandmarks->GetPoint(k))[1];
				p1[2] = (this->avgLandmarks->GetPoint(k))[2];

			vtkPoints* pts = this->inputFibers->GetCell(i)->GetPoints();
			NbPtOnFiber = pts->GetNumberOfPoints();
			for(int j=0; j<NbPtOnFiber; j++)
			{
				double* p0 = new double[3];
				p0[0] = pts->GetPoint(j)[0];
				p0[1] = pts->GetPoint(j)[1];
				p0[2] = pts->GetPoint(j)[2];
				double tuple = sqrt((p0[0]-p1[0])*(p0[0]-p1[0])+(p0[1]-p1[1])*(p0[1]-p1[1])+(p0[2]-p1[2])*(p0[2]-p1[2]));
				dist2landmark->InsertNextTuple1(tuple);
			}

		}
		this->outputFibers->GetPointData()->SetActiveScalars(landmarkLabel[k].c_str());
		this->outputFibers->GetPointData()->SetScalars(dist2landmark);
	}
	
}

// Method Torsions;	
void FiberFeaturesCreator::compute_landmarks_average()
{
	double avg_x=0, avg_y=0, avg_z=0;
	for(int i=0; i<this->nbLandmarks; i++)
	{
		for(int j=0; j<this->landmarks.size(); j++)
		{
			avg_x += this->landmarks[j]->GetPoint(i)[0];
			avg_y += this->landmarks[j]->GetPoint(i)[1];
			avg_z += this->landmarks[j]->GetPoint(i)[2];
			// if(i==4) 
			// {
			// 	std::cout<<"avgZ ="<<this->landmarks[1]->GetPoint(i)[2]<<"   ";
			// }
		}
		avg_x /= this->landmarks.size();
		avg_y /= this->landmarks.size();
		avg_z /= this->landmarks.size();

		this->avgLandmarks->InsertNextPoint(avg_x,avg_y,avg_z);
		avg_x=0; avg_y=0; avg_z=0;
	}
}

// Method Torsions;	
void FiberFeaturesCreator::compute_torsions_feature()
{
	
}

// Method Curvature;	
void FiberFeaturesCreator::compute_curvatures_feature()
{
	
	
}

void FiberFeaturesCreator::SetLandmarksOn()
{
	this->landmarkOn = true;
}

void FiberFeaturesCreator::SetCurvatureOn()
{
	this->curvaturesOn = true;
}

void FiberFeaturesCreator::SetTorsionOn()
{
	this->torsionsOn = true;
}

void FiberFeaturesCreator::write_landmarks_file()
{
	std::string fcsv = this->landmarksFilename + ".fcsv";
	std::ofstream fcsvfile;
	fcsvfile.open(fcsv.c_str());
	std::cout<<BLUE_BOLD<<"---Writting FCSV Landmarks File to "<<CYAN_BOLD<<fcsv.c_str()<<NC<<std::endl;
	fcsvfile << "# Markups fiducial file version = 4.5\n";
	fcsvfile << "# CoordinateSystem = 0\n";
	fcsvfile << "# columns = id,x,y,z,ow,ox,oy,oz,vis,sel,lock,label,desc,associatedNodeID\n";
	for(int i=0; i<this->nbLandmarks; i++)
	{
		fcsvfile <<"Landmark_"<<i<<","<<this->avgLandmarks->GetPoint(i)[0]<<","<<this->avgLandmarks->GetPoint(i)[1]<<","<<this->avgLandmarks->GetPoint(i)[2];
		fcsvfile <<",0,0,0,1,1,1,0,F-"<<i+1<<",,\n";
	}
	fcsvfile.close();

	std::string vtkfile = this->landmarksFilename + ".vtk";
	vtkSmartPointer<vtkPolyData> landmarksPolydata = vtkSmartPointer<vtkPolyData>::New();
	vtkPoints* points = vtkPoints::New();
	for(int i=0; i<this->nbLandmarks; i++)
	{
		points->InsertPoint(i,this->avgLandmarks->GetPoint(i)[0],this->avgLandmarks->GetPoint(i)[1],this->avgLandmarks->GetPoint(i)[2]);
	}
	landmarksPolydata->SetPoints(points);
	writeVTKFile(vtkfile.c_str(), landmarksPolydata);
}



std::vector<int> find_landmarks_index(int nbSamples , int nbLandmarks)
{
	std::vector<int> index;

	for(int k=0; k < nbLandmarks; k++)
	{
		index.push_back(k*((nbSamples-1)/(nbLandmarks-1)));
	}
	return index;
}