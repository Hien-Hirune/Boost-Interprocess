# Boost-Interprocess

## How to install Boost?

https://gist.github.com/zrsmithson/0b72e0cb58d0cb946fc48b5c88511da8

## Using Boost
- Include folder: D:\boost\include\boost-1_76
- Linker folder: D:\boost\lib
- Compile file test.cpp in terminal: D:\MinGW\mingw64\bin\g++.exe -g D:\Coding\test.cpp -o D:\Coding\test.exe -I D:\boost\include\boost-1_76 -L D:\boost\lib
- Run file test.exe: .\test

If you use VSCode, you must add include folder `"D:\\boost\\include\\boost-1_76"` on includePath in **c_cpp_properties.json** file.

Adding  `"-I", "D:\\boost\\include\\boost-1_76"` on args in **task.json** if you want to debug with F5.

Adding `"code-runner.executorMap": 
        {
        "cpp": "cd $dir && g++ $fileName -o $fileNameWithoutExt -ID:\\boost\\include\\boost-1_76 && $dir$fileNameWithoutExt"
        },` in **settings.json** if you want to run code by runner-code (Alt-Ctrl-N)



