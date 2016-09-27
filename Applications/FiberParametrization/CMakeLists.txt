
cmake_minimum_required(VERSION 2.8)

PROJECT(FiberParametrization)

enable_testing()
#Find VTK Package
find_package(VTK REQUIRED)

#Find ITK Package
find_package(ITK REQUIRED)

#Find Slicer Execution Model Package
find_package(SlicerExecutionModel REQUIRED)

include(${VTK_USE_FILE})
include(${ITK_USE_FILE})
include(${SlicerExecutionModel_USE_FILE})

file(
GLOB
source_files
*.[ch]pp
*.[ch]xx
*.[ch]
)


SEMMacroBuildCLI(
  NAME fiberparametrization
  ADDITIONAL_SRCS ${source_files}
  EXECUTABLE_ONLY
  TARGET_LIBRARIES ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  )


##########TESTING##########
SET(FiberParametrization ${source_files})
SET(Output "/work/dprince/Projects/DTIFiberTractStatistics/Applications/FiberParametrization/Outputs")
SET(OKInput1 "/work/dprince/Projects/DTIFiberTractStatistics/Applications/FiberParametrization/Inputs/Arc_L_FT_bundle_clean_processed.vtp")
SET(OKInput2 "/work/dprince/Projects/DTIFiberTractStatistics/Applications/FiberParametrization/Inputs/Arc_R_FT_bundle_clean_processed.vtp")
SET(OKInput3 "/work/dprince/Projects/DTIFiberTractStatistics/Applications/FiberParametrization/Inputs/CorpusCallosum_Genu_bundle_clean_processed.vtp")
SET(KOInput1 "/work/dprince/Projects/DTIFiberTractStatistics/Applications/FiberParametrization/Inputs/PediatricAtlas_071714FinalAtlasDTI.nrrd") #NRRD File instead of .vtk
SET(KOInput2 "/work/dprince/Projects/DTIFiberTractStatistics/Applications/FiberParametrization/CorpusCallosum_Genu_bundle_clean_processed.vtp") #Wrong path for file

# OK ARGS

SET(OKARGS1 "--inputfilename" ${OKInput1} "-N" "30" "--outputfilename" ${Output}/Output_testok1.vtp)
SET(OKARGS2 "-N" "30" "--inputfilename" ${OKInput1} "--outputfilename" ${Output}/Output_testok2.vtp) # TEST with random order in the arguments

# KO ARGS
SET(KOARGS1 ${Input1} "30") # TEST wrong number of arguments 2 instead of 6
SET(KOARGS2 "--inputfilename" ${OKInput1} "-N" "-1" "--outputfilename" ${Output}/Output_testko1.vtp) # TEST wrong value of number of samples, must be >=0
SET(KOARGS3 "--inputfilename" ${KOInput1} "-N" "30" "--outputfilename" ${Output}/Output_testko2.vtp) # TEST wrong format for the input file, must be .vtp or .vtk
SET(KOARGS4 "--inputfilename" ${KOInput2} "-N" "30" "--outputfilename" ${Output}/Output_testko3.vtp) # TEST wrong filepath for the input file, input file must exists
SET(KOARGS5 "--inputfilename" ${KOInput2} "-N" "abc" "--outputfilename" ${Output}/Output_testko4.vtp) # TEST wrong type for the number of samples, must be an integer


# TESTS OK
add_test(NAME TestOK_GoodNbArgs COMMAND fiberparametrization ${OKARGS1})
add_test(NAME TestOK_RandomOrderArgs COMMAND fiberparametrization ${OKARGS2})


# TESTS KO
add_test(NAME TestKO_WrongNbArgs COMMAND fiberparametrization ${KOARGS1})
add_test(NAME TestK0_WrongValueNbSamples COMMAND fiberparametrization ${KOARGS2})
add_test(NAME TestK0_WrongInputFileFormat COMMAND fiberparametrization ${KOARGS3})
add_test(NAME TestK0_WrongInputPath COMMAND fiberparametrization ${KOARGS4})
add_test(NAME TestK0_WrongTypeNbSamples COMMAND fiberparametrization ${KOARGS5})


