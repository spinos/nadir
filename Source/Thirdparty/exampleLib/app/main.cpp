#include <iostream>
#include <ExampleLibrary.h>
#include <ExampleLibraryData.h>

int main(int argc, char *argv[])
{
    std::cout << "\n app test D:/tmp ";
    
    if( ExampleLibraryTestDirectory("D:/tmp") )
        std::cout << " is a folder ";
    else
        std::cout << " not a folder ";
    
    std::cout << "\n test hdf5 " << ExampleLibraryTestHdf("D:/foo.hes");
    
    ExampleLibraryData d;
    d.setNameAge("foxtrap", 43);
	ExampleLibraryFunction(d);
    return 1;
}
