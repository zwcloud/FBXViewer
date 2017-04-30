# FBXViewer
A FBX file viewer including a 3D model information extracter

# Build

## Install Dependencies:

* DirectX SDK
  1. Install Microsoft DirectX SDK (June 2010)
  2. Copy all contents inside `XXXX\Microsoft DirectX SDK (June 2010)\Include` into `$(SolutionDir)\include\dxsdk`.
  3. Copy all contents inside `XXXX\Microsoft DirectX SDK (June 2010)\Lib` into `$(SolutionDir)\lib\dxsdk`.

* FBX SDK
  1. Install FBX SDK 2014.2.1 (not the latest one)
  2. Copy all contents inside `XXXX\Autodesk\FBX\FBX SDK\2014.2.1\include` into `$(SolutionDir)\include\fbxsdk`.
  3. Copy all contents inside `XXXX\Autodesk\FBX\FBX SDK\2014.2.1\lib\vs2010` into `$(SolutionDir)\lib\fbxsdk`.

Finall layout of directory include and lib:

> ©À©¤include
> ©¦  ©À©¤dxsdk
> ©¦  ©¸©¤fbxsdk
> ©¸©¤lib
>     ©À©¤dxsdk
>     ©¦  ©À©¤x64
>     ©¦  ©¸©¤x86
>     ©¸©¤fbxsdk
>         ©À©¤x64
>         ©¦  ©À©¤debug
>         ©¦  ©¸©¤release
>         ©¸©¤x86
>             ©À©¤debug
>             ©¸©¤release

## Build in VS2017
1. Open `FBXViewer.sln` with Visual Studio 2017.
2. Build solution.

## Run
In VS2017, Press F5 to debug run FBXViewer project.

# License
MIT, Zou Wei (zwcloud, zwcloud@hotmail.com)

# Credit
scorpid.FBX: a free animated 3d model downloaded [from 3DRT](https://3drt.com/store/characters/scorpid-monster.html). [License](http://3drt.com/store/terms-of-use-license.html)