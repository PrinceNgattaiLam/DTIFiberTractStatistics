#include "transformpointsfilter.h"
#include "vtkObjectFactory.h"


#define NB_LINES_MAX 500 // We cannot choose more than 500 fcsv points
#define NB_WORDS_MAX 100 // We cannot have more than 100 words per line
vtkStandardNewMacro(TransformPointsFilter);

TransformPointsFilter::TransformPointsFilter(){

    this->m_X = vnl_matrix<float>(); // Input Points
    this->m_Y = vnl_matrix<float>(); // Output Points
    this->M = vnl_matrix<float>(3,3); // Transform Matrix
    this->t = vnl_matrix<float>(); // Translation Matrix
};
TransformPointsFilter::~TransformPointsFilter(){};


void TransformPointsFilter::SetInputFilename(std::string input)
{
	// Check If File Exists
		this->inputfilename = input;
		if(input.rfind(".fcsv")!=std::string::npos)
		{	
			std::cout<<BLUE_BOLD<<"---FCSV File, Extraction of points from the file "<<CYAN_BOLD<<input<<NC<<std::endl;
			this->read_fcsv_points();
		}
		else if(input.rfind(".vtk")!=std::string::npos || input.rfind(".vtp")!=std::string::npos)
		{
			std::cout<<"---VTK File, Extraction of points from the file "<<input<<std::endl;
			this->read_vtk_points();
		}
		else
		{
			throw itk::ExceptionObject("Unknown File Format");
		}
}

void TransformPointsFilter::SetOutputFilename(std::string output)
{
	this->outputfilename = output;
}

void TransformPointsFilter::SetTransform(std::string t_filename)
{
	int nbRow = this->m_X.rows();
	int nbCol = this->m_X.columns();

	typedef itk::TransformBaseTemplate<float> TransformType;
	// typedef itk::Transform<float> TransformType;
	// itk::SmartPointer< TransformType > transform = itk::SmartPointer< TransformType >::New();

#if (ITK_VERSION_MAJOR == 4 && ITK_VERSION_MINOR >= 5) || ITK_VERSION_MAJOR > 4
	itk::TransformFileReaderTemplate<float>::Pointer reader =
	itk::TransformFileReaderTemplate<float>::New();
#else
	itk::TransformFileReader::Pointer reader = itk::TransformFileReader::New();
#endif
	reader->SetFileName(t_filename.c_str());
	reader->Update();


	// Display the transform
	itk::SmartPointer< TransformType > transform = *(reader->GetTransformList()->begin());
	itk::OptimizerParameters<float> Parameter = transform->GetParameters();
	this->t.set_size(nbRow, nbCol);

	for(unsigned int i = 0; i < nbCol; ++i) 
	{
		for(unsigned int j = 0; j < nbCol; ++j) 
		{
			this->M(i,j) = Parameter.GetElement(i*nbCol + j);
		}
	}

	for(unsigned int i = 0; i < nbRow; ++i) 
	{
		for(unsigned int j = 0; j < nbCol; ++j) 
		{
			this->t(i,j) = Parameter.GetElement(9+j);
		}
	}
}

void TransformPointsFilter::Update()
{
	this->m_Y.set_size(this->m_X.rows(),this->m_X.columns());
	this->m_Y = this->m_X*this->M + this->t;
	std::cout<<std::endl<<"Y:"<<this->m_Y<<std::endl;
	std::cout<<"X:"<<this->m_X<<std::endl;
	std::cout<<"M:"<<this->M<<std::endl;
	std::cout<<"t:"<<this->t<<std::endl;
	if(this->outputfilename.rfind(".fcsv")!=std::string::npos)
		this->write_fcsv_points();
	else if(this->outputfilename.rfind(".vtk")!=std::string::npos || this->outputfilename.rfind(".vtp")!=std::string::npos)
		this->write_vtk_points();
	else
		throw itk::ExceptionObject("Wrong Format for Output file");
}

void TransformPointsFilter::read_vtk_points()
{
	vtkSmartPointer<vtkPolyData> vtkLandmarks = readVTKFile(this->inputfilename);
	int NbCells = vtkLandmarks->GetNumberOfCells();
	if(NbCells<1)
		throw itk::ExceptionObject("Wrong Number of cells in the vtk file, must be >=1");
	int NbPoints = vtkLandmarks->GetNumberOfPoints();
	this->m_X.set_size(NbPoints,3);
	for(int i = 0; i<NbCells; i++)
	{
		vtkSmartPointer<vtkPoints> points = vtkLandmarks->GetCell(i)->GetPoints();
		NbPoints = vtkLandmarks->GetCell(i)->GetNumberOfPoints();
		for (int j = 0; j<NbPoints; j++)
		{
			this->m_X(i*NbPoints + j, 0) = points->GetPoint(j)[0];
			this->m_X(i*NbPoints + j, 1) = points->GetPoint(j)[1];
			this->m_X(i*NbPoints + j, 2) = points->GetPoint(j)[2];
		}
	}
}

void TransformPointsFilter::write_vtk_points()
{


}

void TransformPointsFilter::read_fcsv_points()
{
	std::fstream fcsvfile(this->inputfilename.c_str());
	std::string line;
	std::string mot;
	std::string words[NB_LINES_MAX][NB_WORDS_MAX]; // !!!! WARNING DEFINE AND TO PROTECT IF SUPERIOR TO 20
	int i,j;
	// vtkSmartPointer<vtkPoints> landmarksPtToAdd = vtkSmartPointer<vtkPoints>::New();
	
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
				if(j>NB_WORDS_MAX)
					throw itk::ExceptionObject("Too many words per line in the FCSV File");
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

		this->m_X.set_size(NbPoints,3);
		for (int i = 0; i < NbPoints; ++i)
		{
			this->m_X(i, 0) = atof(words[i][1].c_str());
			this->m_X(i, 1) = atof(words[i][2].c_str());
			this->m_X(i, 2) = atof(words[i][3].c_str());
		}
	}
	else
	{
		std::cout<<"Error !";
	}


}
void TransformPointsFilter::write_fcsv_points()
{
	std::string fcsv = this->outputfilename;
	std::ofstream fcsvfile;

	fcsvfile.open(fcsv.c_str());
	std::cout<<BLUE_BOLD<<"---Writting FCSV Landmarks File to "<<CYAN_BOLD<<fcsv.c_str()<<NC<<std::endl;
	fcsvfile << "# Markups fiducial file version = 4.5\n";
	fcsvfile << "# CoordinateSystem = 0\n";
	fcsvfile << "# columns = id,x,y,z,ow,ox,oy,oz,vis,sel,lock,label,desc,associatedNodeID\n";
	for(int i=0; i<this->m_X.rows(); i++)
	{
		fcsvfile <<"Landmark_"<<i<<","<<this->m_Y(i,0)<<","<<this->m_Y(i,1)<<","<<this->m_Y(i,2);
		fcsvfile <<",0,0,0,1,1,1,0,F-"<<i+1<<",,\n";
	}
	fcsvfile.close();
}


	
	