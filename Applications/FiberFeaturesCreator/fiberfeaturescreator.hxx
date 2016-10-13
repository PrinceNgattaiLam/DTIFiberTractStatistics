#include "fiberfeaturescreator.h"
#include "vtkObjectFactory.h"
#include <vtkSimplePointsWriter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkTetra.h>
#include <string> 
#include <fstream>

#define NB_LINES_MAX 20 // We cannot choose more than 20 fcsv points
#define NB_WORDS_MAX 15 // We cannot have more than 15 words per line


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

void FiberFeaturesCreator::SetInput(std::string input, std::string model, std::string landmarkfile)
{
	this->inputFibers = readVTKFile(input);

	if(!model.empty())
	{
		this->modelFibers = readVTKFile(model);
	}
	
	if(landmarkfile.empty())
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
		std::cout<<std::endl<<std::endl;
		puts("---FCSV File found, Computing landmarks from the FCSV File");
		this->compute_landmarks_from_fcsv();
		this->compute_landmarks_average();
	}
	else if(vtPointsOn)
	{
		std::cout<<std::endl<<std::endl;
		puts("---VTK File found, Computing landmarks from the VTK File");
		this->compute_landmarks_from_vtk_vtp();
		this->compute_landmarks_average();
	}

		//this->SetNbLandmarks((int)this->landmarks[0]->GetNumberOfPoints);
			/* TEST CODE*/
	 // for(int i=0; i<this->landmarks.size(); i++)
	 // {
	 // 	for(int j=0; j<this->nbLandmarks; j++)
	 // 	{
	 // 		// puts("JE SAIS PAS 3!");
	 // 		// vtkPoints * pointetoile = this->landmarks[i];
	 // 		// std::cout<<"Nb Point: "<<pointetoile->GetNumberOfPoints()<<" ";
	 // 		// puts("JE SAIS PAS 4!");
	 // 		// double * pp = GetPoint(pp);
	 // 		// puts("JE SAIS PAS 5!");
	 // 		// std::cout<<*pp<<" ";
	 // 		// std::cout<<pp[0]<<" ";


	 // 		std::cout<<this->landmarks[i]->GetPoint(j)[0]<<" "<<this->landmarks[i]->GetPoint(j)[1]<<" "<<this->landmarks[i]->GetPoint(j)[2]<<" ";
	 // 	}
	 // 	std::cout<<std::endl<<std::endl<<std::endl;
	 // }
	else
	{

		std::cout<<std::endl<<std::endl;
		puts("---No VTK or FCSV File for the landmarks, Computing landmarks from the model");
		this->compute_landmarks_from_model();
		this->compute_landmarks_average();
	}

	if(this->landmarkOn)
	{
		std::cout<<std::endl;
		puts("Computing Distance To Landmarks Features of the Fibers");
		puts("******************************************************");
		puts("Start");
		if(!fcsvPointsOn && !vtPointsOn)
			this->write_landmarks_file();
		this->compute_landmark_feature();
		puts("Finished");
		std::cout<<std::endl;

	}
	if(this->torsionsOn)
	{
		std::cout<<std::endl<<std::endl;
		puts("Computing Torsions Features of the Fibers");
		puts("*****************************************");
		puts("Start");
		this->compute_torsions_feature();
		puts("Finished");
		std::cout<<std::endl;

	}
	if(this->curvaturesOn)
	{
		std::cout<<std::endl;
		puts("Computing Curvatures Features of the Fibers");
		puts("*****************************************");
		puts("Start");
		this->compute_curvatures_feature();
		puts("Finished");
		std::cout<<std::endl;

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
	vtkSmartPointer<vtkDataArray> fiberindexArray = this->modelFibers->GetPointData()->GetScalars("FiberLocationIndex");
	int NbPtCurrentFiber;
	int NbFibers = this->modelFibers->GetNumberOfCells();
	int k = 0;

	for(int i=0; i<NbFibers; i++)
	{
		vtkFloatArray* arclengthArrayCurrent = vtkFloatArray::New();
		NbPtCurrentFiber = this->modelFibers->GetCell(i)->GetPoints()->GetNumberOfPoints();
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



	 // std::cout<<"Nb LandMarks = "<<this->landmarks[0].size()<<std::endl;
	 // std::cout<<"Nb LandMarks = "<<this->landmarks[1].size()<<std::endl;
	 // std::cout<<"Nb LandMarks = "<<this->landmarks[2].size()<<std::endl;
	 // std::cout<<"Nb LandMarks = "<<this->landmarks[3].size()<<std::endl;


}
void FiberFeaturesCreator::compute_landmarks_from_vtk_vtp()
{
	vtkSmartPointer<vtkPolyData> vtkLandmarks = readVTKFile(this->landmarksFilename);
	int NbCells = vtkLandmarks->GetNumberOfCells();
	for(int i=0; i<NbCells; ++i)
	{
		vtkSmartPointer<vtkPoints> PtToAdd = vtkSmartPointer<vtkPoints>::New();
		vtkSmartPointer<vtkPoints> points = vtkLandmarks->GetCell(i)->GetPoints();
		this->landmarks.push_back(points);
	}

}
void FiberFeaturesCreator::compute_landmarks_from_fcsv()
{
	std::fstream fcsvfile(this->landmarksFilename.c_str());
	std::string line;
	std::string mot;
	std::string words[NB_LINES_MAX][NB_WORDS_MAX]; // !!!! WARNING DEFINE AND TO PROTECT IF SUPERIOR TO 20
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
		
		// for (int u = 0; u < i; ++u)
		// {
		// 	for (int v = 0; v < j; ++v)
		// 	{
		// 		std::cout<<words[u][v]<<" ";
		// 	}
		// 	std::cout<<std::endl;
		// }
	}
	else
	{
		std::cout<<"Error !";
	}
}

// Method landmark;
void FiberFeaturesCreator::compute_landmark_feature()
{
	//TEST ON THE METHOD AND IF LANDMARKS NON VOID AND INPUTS TOO
	int NbFibers=this->inputFibers->GetNumberOfCells();

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
				//p0 = pts->GetPoint(j);
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
		}
		avg_x /= this->landmarks.size();
		avg_y /= this->landmarks.size();
		avg_z /= this->landmarks.size();
		this->avgLandmarks->InsertNextPoint(avg_x,avg_y,avg_z);
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

	std::string fcsv = "/work/dprince/Projects/DTIFiberTractStatistics/Applications/FiberFeaturesCreator/Outputs/landmarks.fcsv";
	std::string vtp = "/work/dprince/Projects/DTIFiberTractStatistics/Applications/FiberFeaturesCreator/Outputs/landmarks.vtp";
	std::ofstream fcsvfile;
	fcsvfile.open(fcsv.c_str());
	std::cout<<"---Writting FCSV Landmarks File to "<<fcsv.c_str()<<std::endl;
	fcsvfile << "# Markups fiducial file version = 4.5\n";
	fcsvfile << "# CoordinateSystem = 0\n";
	fcsvfile << "# columns = id,x,y,z,ow,ox,oy,oz,vis,sel,lock,label,desc,associatedNodeID\n";
	for(int i=0; i<this->nbLandmarks; i++)
	{
		fcsvfile <<"Landmark_"<<i<<","<<this->avgLandmarks->GetPoint(i)[0]<<","<<this->avgLandmarks->GetPoint(i)[1]<<","<<this->avgLandmarks->GetPoint(i)[2];
		fcsvfile <<",0,0,0,1,1,1,0,F-"<<i+1<<",,\n";
	}
	fcsvfile.close();

	// std::cout<<"---Writting VTP Landmarks File to "<<vtp.c_str()<<std::endl;
	// vtkSmartPointer<vtkPoints> pointsToWrite = vtkSmartPointer<vtkPoints>::New();
	// vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
 //  	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
 //  	polydata->SetPoints(pointsToWrite);
	// polydata->Allocate();
	
	// vtkSmartPointer<vtkIdList> pid = vtkSmartPointer<vtkIdList>::New();
	// for (int i = 0; i < this->landmarks.size(); ++i)
	// {
	// 	for (int j = 0; j < this->nbLandmarks; ++j)
	// 	{
	// 		pid->InsertNextId(pointsToWrite->InsertNextPoint(this->landmarks[i]->GetPoint(j)[0],this->landmarks[i]->GetPoint(j)[1],this->landmarks[i]->GetPoint(j)[2]));
	// 	}

	// 	polydata->InsertNextCell(VTK_POLY_VERTEX, pid);
	// }
	
	// //polydata->SetVerts(vertices);
	// vtkSmartPointer<vtkXMLPolyDataWriter> writer =  vtkSmartPointer<vtkXMLPolyDataWriter>::New();
	// //vtkSmartPointer<vtkSimplePointsWriter> writer = vtkSmartPointer<vtkSimplePointsWriter>::New();
	// writer->SetFileName(vtp.c_str());
	// writer->SetInputData(polydata);
 //  	writer->Update();

}



std::vector<int> find_landmarks_index(int nbSamples , int nbLandmarks)
{
	std::vector<int> index;

	for(int k=0; k < nbLandmarks; k++)
	{
		index.push_back(k*(nbSamples/(nbLandmarks-1)));
	}
	return index;
}