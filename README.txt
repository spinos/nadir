Beginner's Unreal Engine 4 Editor Mode Plugin

Features:
*Click Send button to put selected actor name in a text field
*Print level sequence name currently in editor
*Find transform track of selected actor in level sequence
*Print frame range, display rate, tick resolution, track info
*Open save file dialog
*Use boost via third party library
*Use hdf5 via third party library
*Create a static mesh
*Custom actor with dynamically created components can be stored and reconstructed

How to use Boost:
*Have a lot errors when mixing boost and unreal headers. A workaround is to compile a dynamic library
as the wrapper for boost, using CMake in the normal way. Only include boost headers in .cpp files 
so they won't be exposed to unreal. 
*Copy .h .lib .dll files to Source/ThirdParty/whatever, and use whatever.Build.cs to create a module for it. 
*In the main plugin module, call PublicDependencyModuleNames.AddRange("whatever") to find the library headers. 
*During module startup, call FPlatformProcess::GetDllHandle("path_to_whatever_dll") to load the dynamic library,
before invoking functions inside it.

How to use HDF5:
*Shared hdf5 library won't work. Edit option (BUILD_SHARED_LIBS "Build Shared Libraries" OFF) to build static library only.
In CMakeFilters.cmake, set to find zlib and szip static librarties only. Build zlib and szip static library only as well.
Link static hdf5, zlib, szip, so everything will reside inside the third party library. Resulting dll will be a lot bigger.
*Load third party dll only. No need to load hdf5, zlib, szip dlls any more.

A Valid Mesh
*Must have at least one UV set
*Face connection is clockwise
*Make its package dirty so it can be saved

Dynamically Created Components
*Cannot be saved, save the hierarchy of components in json as a property in PreSave()
*Reconstruct the components in PostLoad()
