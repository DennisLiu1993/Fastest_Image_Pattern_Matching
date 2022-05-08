# Fastest_Image_Pattern_Matching
Using C++/MFC/OpenCV to build a NCC-Based Image Matching algorithm

The Pattern Matching algorithm is based on Normalized Cross Correlation, and the formular is as followed:

![image](https://user-images.githubusercontent.com/104763587/167124708-e73ac583-dead-4628-8f4a-4fd6396c64f9.png)

# steps to build this project
1. Download Visual Studio 2017 or newer versions
2. Check on the option of "x86 and x64 version of C++ MFC"
3. Install
4. Open ELCVMatchTool.sln
5. Upgrade if it is required
6. Open this project's property page
7. Modified "General-Output Directory" to the .exe directory you want
8. Choose the SDK version you have in "General-Windows SDK Version"
9. Choose the right toolset you have in "General-Platform Toolset" (for me, it is Visual Studio 2017 (v141))
10.  Go to "VC++ Directories", and type in "Include Directories" for your own OpenCV 
11.  Type in "Library Directories" for your own OpenCV's library path
12.  Go to "Linker-Input", and type in library name (for me, it is opencv_world310d_vs2017.lib)
13.  Make sure that your opencv.dll is in the same directory as .exe of this project

# tests
Test 1

![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result1.jpg)

Test 2

![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result2.jpg)

Test 3

![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result3.jpg)

Test 4

![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result4.jpg)

Test 6

![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result6.jpg)

# Parameters Setting

# Demonstration Video Link
https://www.youtube.com/watch?v=2h_lN79SpMM&ab_channel=%E5%8A%89%E6%9F%8F%E5%AF%AC
