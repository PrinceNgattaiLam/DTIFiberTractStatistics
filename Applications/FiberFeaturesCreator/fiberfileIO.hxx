#include "fiberfileIO.h"


vtkSmartPointer<vtkPolyData> readVTKFile (std::string filename)
{           // VTK
	if (filename.rfind(".vtk") != std::string::npos)
	{
        std::cout<<std::endl<<BLUE_BOLD<<"---Reading VTK input file at "<<CYAN_BOLD<<filename.c_str()<<NC<<std::endl;
        vtkSmartPointer<vtkPolyDataReader> fiberReader = vtkPolyDataReader::New();
        fiberReader->SetFileName(filename.c_str());
        if(fiberReader->OpenVTKFile())
        {
            fiberReader->Update();
            return fiberReader->GetOutput();
        }
        else
        {
            throw itk::ExceptionObject("File Non Valid");
        }

    }
	        // XML
    else if (filename.rfind(".vtp") != std::string::npos)
    {
        std::cout<<std::endl<<BLUE_BOLD<<"---Reading VTP input file at "<<CYAN_BOLD<<filename.c_str()<<std::endl;
        vtkSmartPointer<vtkXMLPolyDataReader> fiberReader = vtkXMLPolyDataReader::New();
        fiberReader->SetFileName(filename.c_str());
        if(fiberReader->CanReadFile(filename.c_str()))
        {
            fiberReader->Update();
            return fiberReader->GetOutput();
        }
        else
        {
            throw itk::ExceptionObject("File Non Valid");
        }
    }
    else
    {
        throw itk::ExceptionObject("Unknown file format for fibers");
    }

}

void writeVTKFile (std::string filename, vtkSmartPointer<vtkPolyData> output)
{	
	if (filename.rfind(".vtk") != std::string::npos)
	{
        std::cout<<std::endl<<BLUE_BOLD<<"---Writting VTK output File to "<<CYAN_BOLD<<filename.c_str()<<NC<<std::endl;
        vtkSmartPointer<vtkPolyDataWriter> fiberWriter = vtkPolyDataWriter::New();
        fiberWriter->SetFileName(filename.c_str());

		#if (VTK_MAJOR_VERSION < 6)
            fiberWriter->SetInput(output);
		#else
            fiberWriter->SetInputData(output);
		#endif
            fiberWriter->Update();
    }
	        // XML
    else if (filename.rfind(".vtp") != std::string::npos)
    {
        std::cout<<BLUE_BOLD<<"---Writting VTP output File to "<<CYAN_BOLD<<filename.c_str()<<NC<<std::endl;
    	vtkSmartPointer<vtkXMLPolyDataWriter> fiberWriter = vtkXMLPolyDataWriter::New();
    	fiberWriter->SetFileName(filename.c_str());
		#if (VTK_MAJOR_VERSION < 6)
        	fiberWriter->SetInput(output);
		#else
        	fiberWriter->SetInputData(output);
		#endif
        	fiberWriter->Update();
    }
    else
    {
        throw itk::ExceptionObject("Unknown file format for fibers");
    }

	
}