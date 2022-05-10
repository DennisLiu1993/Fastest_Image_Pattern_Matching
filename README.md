# Fastest Image Pattern Matching

Using C++/MFC/OpenCV to build a NCC-Based Image Alignment algorithm
The matching algorithm is based on Normalized Cross Correlation, and the formular is as followed:
![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/ManualImage/NCC.jpg)

# steps to build this project
1.	Download Visual Studio 2017 or newer versions
2.	Check on the option of "x86 and x64 version of C++ MFC"
3.	Install
4.	Open ELCVMatchTool.sln
5.	Upgrade if it is required
6.	Open this project's property page
7.	Modified "General-Output Directory" to the .exe directory you want
8.	Choose the SDK version you have in "General-Windows SDK Version"
9.	Choose the right toolset you have in "General-Platform Toolset" (for me, it is Visual Studio 2017 (v141))
10.	Go to "VC++ Directories", and type in "Include Directories" for your own OpenCV
11.	Type in "Library Directories" for your own OpenCV's library path
12.	Go to "Linker-Input", and type in library name (for me, it is opencv_world310d_vs2017.lib)
13.	Make sure that your opencvXX.dll and MatchTool.Lang are in the same directory as .exe of this project

# usage of this project
1.	Select the Language you want
2.	Drag Source Image to the Left Area
3.	Drag Dst Image to the Right Top Area
4.	Push "Execute Button"

# tests

test0 - with user interface

![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/ManualImage/UIwithResult.jpg)

test1

![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result1.jpg)

test2

![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result2.jpg)

test3

![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result3.jpg)

test4

![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result4.jpg)

test5

![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result6.jpg)

# Parameters Setting
1. **Target Number**: possible max objects you want to find in the inspection image
2. **Max OverLap Ratio**: (the overlap area between two findings) / area of golden sample
3. **Score (Similarity)**: accepted similarity of findings (0~1), lower score causes more execution time
4. **Tolerance Angle**: possible rotation of targets in the inspection image (0~180, meaning search range from -180~180), higher angle causes more execution time
5. **Min Reduced Area**: the min area of toppest level in image pyrimid (trainning stage)

# about outputs
1. results are sorted by score (decreasing order)
2. Angles: inspected rotation of findings
3. PosX, PosY: pixel position of findings

# Demonstration Video Link
https://www.youtube.com/watch?v=2h_lN79SpMM&ab_channel=%E5%8A%89%E6%9F%8F%E5%AF%AC

# This project can also be used as OCR detection
https://www.youtube.com/watch?v=lM0NK6xVNfg&ab_channel=%E5%8A%89%E6%9F%8F%E5%AF%AC
