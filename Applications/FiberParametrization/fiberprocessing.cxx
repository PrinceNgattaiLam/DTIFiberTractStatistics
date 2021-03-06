#include "fiberprocessing.h"

GroupType::Pointer readFiberFile(std::string filename)
{
    if (filename.rfind(".vt")!= std::string::npos)
    {
        // Build up the principal data structure for fiber tracts
        GroupType::Pointer fibergroup = GroupType::New();

        vtkSmartPointer<vtkPolyData> fibdata=vtkSmartPointer<vtkPolyData>::New();

        if (filename.rfind(".vtk")!=std::string::npos) //if it's a .vtk file
        {
            vtkSmartPointer<vtkPolyDataReader> reader = vtkPolyDataReader::New(); //We use vtkPolyDataReader
            reader->SetFileName(filename.c_str());
            if(reader->OpenVTKFile())
            {
                std::cout<<"---Reading VTK input file at "<<filename.c_str()<<std::endl;
                reader->Update();
                fibdata = reader->GetOutput();
            }
            else
            {
                throw itk::ExceptionObject("File Non Valid");
            }


        }
        else if (filename.rfind(".vtp")!=std::string::npos)//if it's a .vtp file
        {
            vtkSmartPointer<vtkXMLPolyDataReader> reader= vtkXMLPolyDataReader::New();//We use vtkXMLPolyDataReader
            reader->SetFileName(filename.c_str());
            if(reader->CanReadFile(filename.c_str()))
            {
                std::cout<<"---Reading VTP input file at "<<filename.c_str()<<std::endl;
                reader->Update();
                fibdata = reader->GetOutput();
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

        // TYPEDEFS
        typedef  itk::SymmetricSecondRankTensor<double,3> ITKTensorType;
        typedef  ITKTensorType::EigenValuesArrayType LambdaArrayType;

        // Iterate over VTK data
        const int nfib = fibdata->GetNumberOfCells(); // "nfib" = number of fibers in the input fiber bundle
        if(nfib<1)
        {
            throw itk::ExceptionObject("Empty Input Fibers");
        }
        int pindex = -1;
        for(int i = 0; i < nfib; ++i)
        {
            itk::DTITubeSpatialObject<3>::Pointer dtiTube = itk::DTITubeSpatialObject<3>::New();
            vtkSmartPointer<vtkCell> fib = fibdata->GetCell(i); // "fib" = i th fiber in fibdata

            vtkSmartPointer<vtkPoints> points = fib->GetPoints(); // "points" contains all the points of one fiber fib
            std::vector<DTIPointType> pointsToAdd;

            vtkSmartPointer<vtkDataArray> fibtensordata = fibdata->GetPointData()->GetTensors();

            for(int j = 0; j < points->GetNumberOfPoints(); ++j) // For each point in the fiber "fib"
            {
                ++pindex;
                double * coordinates = points->GetPoint(j);

                DTIPointType pt;
                // Convert from RAS to LPS for vtk
                pt.SetPosition(coordinates[0], coordinates[1], coordinates[2]); // We store its coordinates
                pt.SetRadius(0.5); // We set a radius of 0.5 by default
                pt.SetColor(0.0, 0.0, 1.0); // We set blue color by default

                if (fibtensordata != NULL)
                {
                    double * vtktensor = fibtensordata->GetTuple9(pindex);
                    float floattensor[6];
                    ITKTensorType itktensor;

                    floattensor[0] = itktensor[0] = vtktensor[0];
                    floattensor[1] = itktensor[1] = vtktensor[1];
                    floattensor[2] = itktensor[2] = vtktensor[2];
                    floattensor[3] = itktensor[3] = vtktensor[4];
                    floattensor[4] = itktensor[4] = vtktensor[5];
                    floattensor[5] = itktensor[5] = vtktensor[8];

                    pt.SetTensorMatrix(floattensor); // We conpute the Tensor Matrix for each point

                    LambdaArrayType lambdas;


                    itktensor.ComputeEigenValues(lambdas); // Need to do eigenanalysis of the tensor

                    // We compute the differents diffusivity values according to their mathematics expressions
                    float md = (lambdas[0] + lambdas[1] + lambdas[2])/3;
                    float fa = sqrt(1.5) * sqrt((lambdas[0] - md)*(lambdas[0] - md) +
                            (lambdas[1] - md)*(lambdas[1] - md) +
                            (lambdas[2] - md)*(lambdas[2] - md))
                            / sqrt(lambdas[0]*lambdas[0] + lambdas[1]*lambdas[1] + lambdas[2]*lambdas[2]);

                    float logavg = (log(lambdas[0])+log(lambdas[1])+log(lambdas[2]))/3;

                    //                    float ga =  sqrt( SQ2(log(lambdas[0])-logavg)
                    //                            + SQ2(log(lambdas[1])-logavg)
                    //                            + SQ2(log(lambdas[2])-logavg) );
                    //                  float fro = sqrt(lambdas[0]*lambdas[0] + lambdas[1]*lambdas[1] + lambdas[2]*lambdas[2]);
                    float ad = lambdas[2];
                    float rd = (lambdas[0] + lambdas[1])/2;

                    // We create the corresponding fields and add theirs values
                    pt.AddField("FA",fa);
                    pt.AddField("MD",md);
                    //pt.AddField("FRO",fro);
                    pt.AddField("l2",lambdas[1]);
                    pt.AddField("l3",lambdas[0]);
                    pt.AddField("l1",ad);
                    pt.AddField("RD",rd);
                    //pt.AddField("GA",ga);
                }
                pointsToAdd.push_back(pt); // We add the point to our vector
            }

            dtiTube->SetPoints(pointsToAdd);// We set all the points corresponding to one fiber in a Saptial Object
            fibergroup->AddSpatialObject(dtiTube);// And we add this Spatial Object in our Group of Spatial Object
        }

        return fibergroup;
    } // end process .vtk .vtp
    else
    {
        throw itk::ExceptionObject("Unknown fiber file");
    }
}

void writeFiberFile(const std::string & filename, GroupType::Pointer fibergroup){

    // Make sure origins are updated
    fibergroup->ComputeObjectToWorldTransform();

    // VTK Poly Data
    if (filename.rfind(".vt") != std::string::npos)
    {
        // Build VTK data structure
        vtkSmartPointer<vtkPolyData> polydata(vtkPolyData::New());
        vtkSmartPointer<vtkFloatArray> tensorsdata(vtkFloatArray::New());
        vtkSmartPointer<vtkFloatArray> faData(vtkFloatArray::New());
        // vtkSmartPointer<vtkFloatArray> mdData(vtkFloatArray::New());
        // vtkSmartPointer<vtkFloatArray> rdData(vtkFloatArray::New());
        // vtkSmartPointer<vtkFloatArray> l1Data(vtkFloatArray::New());
        // vtkSmartPointer<vtkFloatArray> l2Data(vtkFloatArray::New());
        // vtkSmartPointer<vtkFloatArray> l3Data(vtkFloatArray::New());
        vtkSmartPointer<vtkFloatArray> fiberIndex(vtkFloatArray::New());
        vtkSmartPointer<vtkIdList> ids(vtkIdList::New());
        vtkSmartPointer<vtkPoints> pts(vtkPoints::New());
        faData->SetName("FA");
        // mdData->SetName("MD");
        // rdData->SetName("RD");
        // l1Data->SetName("L1");
        // l2Data->SetName("L2");
        // l3Data->SetName("L3");
        fiberIndex->SetName("FiberLocationIndex");
        tensorsdata->SetNumberOfComponents(9);
        polydata->SetPoints (pts);

        ids->SetNumberOfIds(0);
        pts->SetNumberOfPoints(0);
        polydata->Allocate();

        std::auto_ptr<ChildrenListType> children(fibergroup->GetChildren(0));
        typedef ChildrenListType::const_iterator IteratorType;

        for (IteratorType it = children->begin(); it != children->end(); it++)
        {
            itk::SpatialObject<3>* tmp = (*it).GetPointer();
            itk::DTITubeSpatialObject<3>* tube = dynamic_cast<itk::DTITubeSpatialObject<3>* >(tmp);
            unsigned int nPointsOnFiber = tube->GetNumberOfPoints();
            vtkIdType currentId = ids->GetNumberOfIds();

            for (unsigned int k = 0; k < nPointsOnFiber; k++)
            {
                itk::Point<double, 3> v(tube->GetPoint(k)->GetPosition());
                itk::Vector<double, 3> spacing(tube->GetSpacing());
                itk::Vector<double, 3> origin(tube->GetObjectToWorldTransform()->GetOffset());

                // convert origin from LPS -> RAS
                /*       origin[0] = -origin[0];
           origin[1] = -origin[1];*/

                vtkIdType id;
                // Need to multiply v by spacing and origin
                // Also negate the first to convert from LPS -> RAS
                // for slicer 3
                /*        id = pts->InsertNextPoint(-v[0] * spacing[0] + origin[0],
                                     -v[1] * spacing[1] + origin[1],
                                     v[2] * spacing[2] + origin[2]);*/

                id = pts->InsertNextPoint(v[0] * spacing[0] + origin[0],
                        v[1] * spacing[1] + origin[1],
                        v[2] * spacing[2] + origin[2]);

                ids->InsertNextId(id);

                itk::DTITubeSpatialObjectPoint<3>* sopt = dynamic_cast<itk::DTITubeSpatialObjectPoint<3>* >(tube->GetPoint(k));
                float vtktensor[9];
                vtktensor[0] = sopt->GetTensorMatrix()[0];
                vtktensor[1] = sopt->GetTensorMatrix()[1];
                vtktensor[2] = sopt->GetTensorMatrix()[2];
                vtktensor[3] = sopt->GetTensorMatrix()[1];
                vtktensor[4] = sopt->GetTensorMatrix()[3];
                vtktensor[5] = sopt->GetTensorMatrix()[4];
                vtktensor[6] = sopt->GetTensorMatrix()[2];
                vtktensor[7] = sopt->GetTensorMatrix()[4];
                vtktensor[8] = sopt->GetTensorMatrix()[5];

                tensorsdata->InsertNextTupleValue(vtktensor);
                faData->InsertNextTuple1(sopt->GetField("FA"));
                // mdData->InsertNextTuple1(sopt->GetField("MD"));
                // rdData->InsertNextTuple1(sopt->GetField("RD"));
                // l1Data->InsertNextTuple1(sopt->GetField("L1"));
                // l2Data->InsertNextTuple1(sopt->GetField("L2"));
                // l3Data->InsertNextTuple1(sopt->GetField("L3"));
                fiberIndex->InsertNextTuple1(k);

            }
            
            polydata->InsertNextCell(VTK_POLY_LINE, nPointsOnFiber, ids->GetPointer(currentId));
        }

        polydata->GetPointData()->SetTensors(tensorsdata);
        polydata->GetPointData()->SetActiveScalars("FiberLocationIndex");
        polydata->GetPointData()->SetScalars(fiberIndex);
        polydata->GetPointData()->SetActiveScalars("FA");
        polydata->GetPointData()->SetScalars(faData);
        // polydata->GetPointData()->SetActiveScalars("MD");
        // polydata->GetPointData()->SetScalars(mdData);
        // polydata->GetPointData()->SetActiveScalars("RD");
        // polydata->GetPointData()->SetScalars(rdData);
        // polydata->GetPointData()->SetActiveScalars("L1");
        // polydata->GetPointData()->SetScalars(l1Data);
        // polydata->GetPointData()->SetActiveScalars("L2");
        // polydata->GetPointData()->SetScalars(l2Data);
        // polydata->GetPointData()->SetActiveScalars("L3");
        // polydata->GetPointData()->SetScalars(l3Data);

        // Legacy
        if (filename.rfind(".vtk") != std::string::npos)
        {
        std::cout<<"---Writting VTK output File to "<<filename.c_str()<<std::endl;
            vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
            //       fiberwriter->SetFileTypeToBinary();
            fiberwriter->SetFileName(filename.c_str());
#if (VTK_MAJOR_VERSION < 6)
            fiberwriter->SetInput(polydata);
#else
            fiberwriter->SetInputData(polydata);
#endif
            fiberwriter->Update();
        }
        // XML
        else if (filename.rfind(".vtp") != std::string::npos)
        {
        std::cout<<"---Writting VTP output File to "<<filename.c_str()<<std::endl;
            vtkSmartPointer<vtkXMLPolyDataWriter> fiberwriter = vtkXMLPolyDataWriter::New();
            fiberwriter->SetFileName(filename.c_str());
#if (VTK_MAJOR_VERSION < 6)
            fiberwriter->SetInput(polydata);
#else
            fiberwriter->SetInputData(polydata);
#endif
            fiberwriter->Update();
        }
        else
        {
            throw itk::ExceptionObject("Unknown file format for fibers");
        }
    }
    else
    {
        throw itk::ExceptionObject("Unknown file format for fibers");
    }
}

