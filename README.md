# MapArt-Maker
An Image to litematic converter for making Minecraft Maparts.

 ## Folder Layout
All code is in `src`.

`Processing` is Hunsinger's color code

`GUI` is polarbub's GUI

`libraries` all the libraries used. PSL is `polar standard library`.

## Build System
I'm using cmake because it's somewhat cross-platform.

In Visual Studio you can open CMakeLists.txt as a project.

In CLion you can open the folder containing CMakeLists.txt.

### Manual build
```shell
clang++ -fPIE -fPIC -I/mnt/saves/code/c++/Graphics/wx/MapArt-Maker/Object/src/libraries/PSL/include/psl -I/mnt/saves/code/c++/Graphics/wx/MapArt-Maker/Object/src/libraries/PSL/internal_include/  -std=gnu++20 -o psl.cpp.o -c /mnt/saves/code/c++/Graphics/wx/MapArt-Maker/Object/src/libraries/PSL/src/psl.cpp -O3
clang++ -fPIE -fPIC -I/mnt/saves/code/c++/Graphics/wx/MapArt-Maker/Object/src/libraries/PSL/include/psl -I/mnt/saves/code/c++/Graphics/wx/MapArt-Maker/Object/src/libraries/PSL/internal_include/  -std=gnu++20 -o image.cpp.o -c /mnt/saves/code/c++/Graphics/wx/MapArt-Maker/Object/src/libraries/PSL/src/image.cpp -O3
clang++ -fPIE -fPIC -g -shared -Wl,-soname,libPSL.so.1 -o libPSL.so.0.0.1 psl.cpp.o image.cpp.o

g++ -fPIE -fPIC -I/mnt/saves/code/c++/Graphics/wx/MapArt-Maker/Object/src/libraries/PSL/include -I/mnt/saves/code/c++/Graphics/wx/MapArt-Maker/Object/src/include -I/mnt/saves/code/c++/Graphics/wx/MapArt-Maker/Object/src/Processing -masm=intel -std=gnu++20 -o Main.cpp.o -c /mnt/saves/code/c++/Graphics/wx/MapArt-Maker/Object/src/Processing/Main.cpp -O3

clang++ Main.cpp.o psl.cpp.o image.cpp.o -o CLI -O3 
```

## TODO Comments
CLion has a todo feature that lets me find comments starting with a configurable value. I have it set up like so;

1) `ADD:` A feature needs to be added.
2) `TEST:` A new feature needs to be tested.
3) `FIX:` A bug needs to be fixed.
4) `DEBUG:` A mystery bug needs to have the cause found on.
