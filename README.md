# RadeonProRenderUE

# Setup project

## Generate Visual Studio files

Right-click on RPR.uproject and select "Generate Visual Studio project files".<br>
It should create a "RPR.sln" file

## Compile

- Open the "RPR.sln" file.
- Check that your build configuration is in "Development Editor" (or "DebugGame Editor")
- Press "Local Windows Debugger" to start the build.

At the end of the build, the project should start.


# Build the RPR plugin for marketplace

## For the first time

You will need to set an environment variable that will tell to the build script where is your UE4 installation.

### Unix (Linux/MacOSX)

- Open a terminal
- Write `setenv UE4_Path **[Path to the root of your UE4 installation]**`. Add **"** around your path if there is spaces in it.<br>
Example :
	- `setenv UE4_Path D:/Softwares/UE_4.18`
	- `setenv UE4_Path "~/softs/my UE4 installation"`


## Start the build

### Windows

- Enter the directory **BuildPluginTools**
- Start **Build_RPRPlugin_XXX.bat** with **XXX** being the version you want (using double-click)
- Wait for everything to be setup. It will take a while since it will compile the whole plugin and copy some binary/image files.<br>
- Once your build is over, go to **PluginStaging\UE4_[YourUE4Version]**
- Your plugin is right here, ready to be zipped in hosted

![Screenshot of Build_RPRPlugin.bat in action](Medias/BuildRPRPlugin_Screenshot01.jpg "Build_RPRPlugin.bat in action")

You can also use **Build_RPRPlugin_All.bat** to build on all versions.


### Unix (Linux/MacOSX)

- Open a terminal
- Navigate through **RadeonProRenderUE/BuildPluginTools** (use `cd` and `ls` to respectively move among the directories and list files in the current directory).
- Start the build script using `./Build_RPRPlugin.sh`
- The **UE4_Path** should be displayed at the beginning. Check that it is correct.
- Wait for the build to be completed. Press **Ctrl+C** if you want to abort the operation.
- Once your build is over, go to **PluginStaging/UE4_[YourUE4Version]**
- Your plugin is right here, ready to be zipped in hosted
