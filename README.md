# ManoMachineAssembler
Assembler for the Mano Machine designed in Cedarville's Computer Architecture class.

## Hardware Requirements
Your system must support at minumum version 4.3 of OpenGL. To see if your computer is supported, check your hardware and driver version using the following links:
 - [Intel](https://www.intel.com/content/www/us/en/support/articles/000005524/graphics.html)
 - [NVIDIA](https://developer.nvidia.com/opengl-driver)

## Environment Setup
### Install vcpkg
Install [vcpkg](https://github.com/microsoft/vcpkg), and follow the instructions to integrate it with Visual Studio. After it is installed, run the following commands:
```
vcpkg install glfw3
vcpkg install glew
vcpkg install glm
```

If you integrated vcpkg with Visual Studio, the libraries will be automatically added to your projects. 

### Compile NFD
Next, download [Native File Dialogue](https://github.com/mlabbe/nativefiledialog) and compile it using Visual Studio. The .sln file can be found in `build/vs2010`. 
Make sure to use the same compiler versionas you plan on using for this project!

Once compilation of NFD is finished, you will need to tell this project where to find the files for NFD. Under project properties, add `(NFD)/src/include` to
Additional Include Directories and `(NFD)/build/lib/Debug/x86/`to Additional Library Dependencies under the Debug configuration, and `(NFD)/build/lib/Release/x86/` under
the Release configuration (optional).

## How to Use
You can either start typing Mano Assembly or click Open File and select a .asm file to edit. You can save your progress any time with the Save button. When you are ready to 
compile the assembly, click compile. The assembler will make two passes, and save the output of each stage to a file. A dialog box will ask you to name the file and set the 
filepath. The symbol file will be `.sym`, and the assembly will be `.cdm`. If any errors are encountered, an error message will be displayed on the stage that created the error, 
and no file for that stage will be saved. To see the output of each stage within the application, press either the Symbol button or the Assembly button. **Note: These text boxes 
are not editable.** To return to editing your code, press edit.
