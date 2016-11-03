#include "fiberreorientation.h"
#include "vtkObjectFactory.h"
vtkStandardNewMacro(FiberReorientation);

FiberReorientation::FiberReorientation(){
	this->inputFibers = vtkSmartPointer<vtkPolyData>::New();
	this->SetHasReversed(false);
};
FiberReorientation::~FiberReorientation(){};


void FiberReorientation::SetInput(std::string input)
{
	this->inputFibers = readVTKFile(input);
}

void FiberReorientation::SetInputData(vtkSmartPointer<vtkPolyData> input)
{
	this->inputFibers = input;
}

vtkSmartPointer<vtkPolyData> FiberReorientation::GetOutput()
{
	std::cout<<"Nb of Fibers: "<<this->outputFibers->GetNumberOfCells()<<std::endl;
	std::cout<<"Nb of Points: "<<this->outputFibers->GetNumberOfPoints()<<std::endl;
	return this->outputFibers;
}

void FiberReorientation::Update()
{
	this->init_output();
	this->init_startPoints();
	this->init_endPoints();
	int NbFibers = this->inputFibers->GetNumberOfCells();

	vtkSmartPointer<vtkIdList> ids = vtkSmartPointer<vtkIdList>::New();
	ids->SetNumberOfIds(0);
	vtkIdType currentId = ids->GetNumberOfIds();
	vtkCell* cell = this->inputFibers->GetCell(0);
	vtkPoints* pts = cell->GetPoints();
	int count_reverse_fiber=0;
	int NbPts = cell->GetNumberOfPoints();
	// this->outputFibers->InsertNextCell(VTK_POLY_LINE, NbPts, ids->GetPointer(currentId));


	std::ofstream backlog;
	backlog.open("backlog.txt");
	std::cout<<"---Processing Fibers"<<std::endl;
	double currentPt[3] = {this->startPoints->GetPoint(0)[0],this->startPoints->GetPoint(0)[1],this->startPoints->GetPoint(0)[2]};
	for(int i = 0; i < NbFibers-1; i++)
	{
		double distStart = this->compute_dist_next_startpoint(currentPt, i);
		double distEnd = this->compute_dist_next_endpoint(currentPt, i);
		currentId = ids->GetNumberOfIds();
		vtkCell* cell = this->inputFibers->GetCell(i+1);
		pts = cell->GetPoints();
		NbPts = cell->GetNumberOfPoints();
		if(distStart>distEnd)
		{
			this->SetHasReversed(true);
			count_reverse_fiber++;
			backlog<<"---Reversing Fiber #"<<i<<std::endl;
			// std::cout<<"---Reversing Fiber #"<<i<<std::endl;
			this->outputFibers->ReverseCell(i+1);
			currentPt[0] = pts->GetPoint(NbPts-1)[0];
			currentPt[1] = pts->GetPoint(NbPts-1)[1];
			currentPt[2] = pts->GetPoint(NbPts-1)[2];
		}
	}
	backlog<<"Nb Fibers reversed: "<<count_reverse_fiber;
	std::cout<<"---Fibers Processed"<<std::endl;
	std::cout<<endl<<"Nb Fibers reversed: "<<count_reverse_fiber<<endl<<endl;
	backlog<<"Nb Total Fibers: "<<NbFibers<<endl;
	backlog.close();

	
}
double FiberReorientation::compute_dist_next_startpoint(double* pt, int index)
{
	double x0 = pt[0];
	double x1 = this->startPoints->GetPoint(index+1)[0];
	double y0 = pt[1];
	double y1 = this->startPoints->GetPoint(index+1)[1];
	double z0 = pt[2];
	double z1 = this->startPoints->GetPoint(index+1)[2];
	return sqrt((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1)+(z0-z1)*(z0-z1));

}
double FiberReorientation::compute_dist_next_endpoint(double* pt, int index)
{
	double x0 = pt[0];
	double x1 = this->endPoints->GetPoint(index+1)[0];
	double y0 = pt[1];
	double y1 = this->endPoints->GetPoint(index+1)[1];
	double z0 = pt[2];
	double z1 = this->endPoints->GetPoint(index+1)[2];
	return sqrt((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1)+(z0-z1)*(z0-z1));
}
void  FiberReorientation::init_startPoints()
{
	this->startPoints = vtkSmartPointer<vtkPoints>::New();
	this->startPoints->SetNumberOfPoints(0);
	int NbFibers = this->inputFibers->GetNumberOfCells();
	for (int i = 0; i < NbFibers; ++i)
	{
		double x = this->inputFibers->GetCell(i)->GetPoints()->GetPoint(0)[0];
		double y = this->inputFibers->GetCell(i)->GetPoints()->GetPoint(0)[1];
		double z = this->inputFibers->GetCell(i)->GetPoints()->GetPoint(0)[2];
		this->startPoints->InsertNextPoint(x,y,z);
	}

}
void  FiberReorientation::init_endPoints()
{

	this->endPoints = vtkSmartPointer<vtkPoints>::New();
	this->endPoints->SetNumberOfPoints(0);
	int NbFibers = this->inputFibers->GetNumberOfCells();
	for (int i = 0; i < NbFibers; ++i)
	{
		int NbPtOnFiber = this->inputFibers->GetCell(i)->GetPoints()->GetNumberOfPoints();
		double x = this->inputFibers->GetCell(i)->GetPoints()->GetPoint(NbPtOnFiber-1)[0];
		double y = this->inputFibers->GetCell(i)->GetPoints()->GetPoint(NbPtOnFiber-1)[1];
		double z = this->inputFibers->GetCell(i)->GetPoints()->GetPoint(NbPtOnFiber-1)[2];
		this->endPoints->InsertNextPoint(x,y,z);
	}

}



void FiberReorientation::init_output()
{
	this->outputFibers = vtkSmartPointer<vtkPolyData>::New();
	//this->outputPts = vtkSmartPointer<vtkPoints>::New();
	//this->outputPts->SetNumberOfPoints(0);
	//this->outputFibers->SetPoints(this->outputPts);
	this->outputFibers= this->inputFibers;
}

void FiberReorientation::SetHasReversed(bool state)
{
	this->hasReversed = state;
}

bool FiberReorientation::GetHasReversed()
{
	return this->hasReversed;
}

	
	