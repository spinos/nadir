#if defined _WIN32 || defined _WIN64
    #include <Windows.h>

    #define DLLEXPORT __declspec(dllexport)
#else
    #include <stdio.h>
#endif

#ifndef DLLEXPORT
    #define DLLEXPORT
#endif

#include "ExampleLibraryData.h"

#include <iostream>
#include <string>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <hdf5.h>
#ifdef __cplusplus
extern "C"
{
#endif

DLLEXPORT void ExampleLibraryFunction(const ExampleLibraryData &msg)
{
    std::string fullmsg = boost::str(boost::format("hellow world %1% %2% of how to use dll") % msg.name() % msg.age() );
#if defined _WIN32 || defined _WIN64
	MessageBox(NULL, TEXT(fullmsg.c_str()), NULL, MB_OK);
#else
    printf("Hello World");
#endif
}

DLLEXPORT bool ExampleLibraryTestDirectory(const std::string &name)
{
    boost::filesystem::path apath(name);
    if (boost::filesystem::is_directory(apath))
        return true;
    return false;
}

DLLEXPORT bool ExampleLibraryTestHdf(const std::string &filename)
{

    hid_t file_id = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if(file_id < 0) return false;
    
    hid_t group_id = H5Gcreate2(file_id, "/MyGroup", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    
    herr_t status = H5Gclose(group_id);
    
    hsize_t     dims[2];
    dims[0] = 4; 
    dims[1] = 6; 
    hid_t dataspace_id = H5Screate_simple(2, dims, NULL);
    
    hid_t dataset_id = H5Dcreate2(file_id, "/dset", H5T_STD_I32BE, dataspace_id, 
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
                          
    status = H5Dclose(dataset_id);
    
    status = H5Sclose(dataspace_id);
   
    status = H5Fclose(file_id);
    return true;

}

#ifdef __cplusplus
}
#endif

