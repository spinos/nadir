Beginner's Unreal Engine 4 Editor Mode Plugin

Features:
*Click Send button to put selected actor name in a text field
*Print level sequence name currently in editor
*Find transform track of selected actor in level sequence
*Print frame range, display rate, tick resolution, track info
*Open save file dialog
*Use boost via third party library

How to use Boost:
*Have a lot errors when mixing boost and unreal headers. A workaround is to compile a dynamic library
as the wrapper for boost, using CMake in the normal way. Only include boost headers in .cpp files 
so they won't be exposed to unreal. 
*Copy .h .lib .dll files to Source/ThirdParty/whatever, and use whatever.Build.cs to create a module for it. 
*In the main plugin module, call PublicDependencyModuleNames.AddRange("whatever") to find the library headers. 
*During module startup, call FPlatformProcess::GetDllHandle("path_to_whatever_dll") to load the dynamic library,
before invoking functions inside it.
