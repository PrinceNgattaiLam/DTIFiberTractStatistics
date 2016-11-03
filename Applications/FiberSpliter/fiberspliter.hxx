#include "fiberspliter.h"
#include "vtkObjectFactory.h"
vtkStandardNewMacro(FiberSpliter);

FiberSpliter::FiberSpliter(){
	this->inputFibers = vtkSmartPointer<vtkPolyData>::New();
};
FiberSpliter::~FiberSpliter(){};


void FiberSpliter::SetInput(std::string input)
{
	this->inputFibers = readVTKFile(input);
	int locExt = input.find_last_of('.');
	int locName = input.find_last_of('/');

	this->ext = input.substr(locExt+1);
	this->filename = input.substr(locName+1, locExt-locName-1);
}

void FiberSpliter::SetInputData(vtkSmartPointer<vtkPolyData> input)
{
	this->inputFibers = input;
}

void FiberSpliter::SetOutput(std::string output)
{
	this->outputDir = output;
}

void FiberSpliter::Update()
{
	int NbFibers = this->inputFibers->GetNumberOfCells();

	for(int i =0; i<NbFibers; i++)
	{
		this->extract_fiber(i);
	}



	
}

void FiberSpliter::extract_fiber(int i)
{
	vtkSmartPointer<vtkPolyData> outputFiber = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> pts = vtkPoints::New();
	vtkSmartPointer<vtkPoints> inputpts = vtkPoints::New();
	outputFiber->SetPoints (pts);
	outputFiber->Allocate();
	vtkSmartPointer<vtkIdList> ids = vtkSmartPointer<vtkIdList>::New();

	inputpts = this->inputFibers->GetCell(i)->GetPoints();
	int NbPoints = this->inputFibers->GetCell(i)->GetNumberOfPoints();
    vtkIdType currentId = ids->GetNumberOfIds();
    for( size_t j = 0 ; j < NbPoints; j++)
    {
        vtkIdType id;
        id = pts->InsertNextPoint(inputpts->GetPoint(j)[0],
                                  inputpts->GetPoint(j)[1],
                                  inputpts->GetPoint(j)[2]);
        ids->InsertNextId(id);
    }
    outputFiber->InsertNextCell(VTK_POLY_LINE,NbPoints,ids->GetPointer(0));
	std::string i_char = static_cast<std::ostringstream*>( &( std::ostringstream() << i) )->str();
	// std::cout<<"Nb Cells: "<<outputFiber->GetNumberOfCells()<<" - Nb Points: "<<outputFiber->GetCell(0)->GetNumberOfPoints()<<" - Nb Pts Inputs "<<NbPoints<<std::endl;
	// std::cout<<this->outputDir+"/"+this->filename+"_"+ i_char+this->ext<<std::endl;
	writeVTKFile(this->outputDir+"/"+this->filename+"_"+ i_char+"."+this->ext, outputFiber);

}


	
	