# Fastest Image Pattern Matching
**The fasted template matching implementation on the Internet.**

Using C++/MFC/OpenCV to build a NCC-Based Image Alignment algorithm
The matching algorithm is based on Normalized Cross Correlation, and the formular is as followed:
![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Manual%20Image/NCC.jpg)

# Improvements
1. rotation invariant
2. using image pyrimid as an searching strategy
3. minimizing the inspection area on the top level of image pyrimid
4. optimized rotation time from opencv by setting needed "size" and modifying rotation matrix
5. rotation precision is as high as possible

# Comparison with commercial libraries
[Inspection Image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Test%20Images/Src7.bmp) : 4024 X 3036 

[Template Image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Test%20Images/Dst7.bmp): 762 X 521
Library    |Index | Score | Angle | PosX | PosY | Execution Time
---------  |----- |-------|-------| -----  | -----  |----------------------
My Tool    |0     |   1   | 0.046 |1725.857|1045.433| 112ms
My Tool    |1     |   0.998   | -119.979 |2662.869|1537.446| 
My Tool    |2     |  0.991   | 120.150 |1768.936|2098.494| 
Cognex     |0     |   1   | 0.030 |1725.960|1045.470| 125ms
Cognex     |1     |   0.989   | -119.960 |2663.750|1538.040| 
Cognex     |2     |  0.983   | 120.090 |1769.250|2099.410| 
Aisys    |0     |   1   | 0 |1726.000|1045.500| 202ms
Aisys    |1     |   0.990   | -119.935 |2663.630|1539.060| 
Aisys    |2     |  0.979   | 120.000 |1769.63|2099.780| 

note: if you want to get a best performance, please make sure you are using release verson (both this project and OpenCV dll). That's because O2-related significantly affect efficiency, and the difference of Debug and Release can up to 7 times for some cases.

# Steps to build this project
1.	Download Visual Studio 2017 or newer versions
2.	Check on the option of "x86 and x64 version of C++ MFC"
3.	Install
4.	Open ELCVMatchTool.sln
5.	Upgrade if it is required
6.	Open this project's property page
7.	Modified "General-Output Directory" to the .exe directory you want (usually the directory where your opencv_worldXX.dll locates)
8.	Choose the SDK version you have in "General-Windows SDK Version"
9.	Choose the right toolset you have in "General-Platform Toolset" (for me, it is Visual Studio 2017 (v141))
10.	Go to "VC++ Directories", and type in "Include Directories" for your own OpenCV (e.g. C:\OpenCV3.1\opencv\build\include or C:\OpenCV4.0\opencv\build\include)
11.	Type in "Library Directories" for your own OpenCV's library path (the directory where your opencv_worldXX.lib locates)
12.	Go to "Linker-Input", and type in library name (e.g. opencv_world310d_vs2017.lib or opencv_world401d.lib)
13.	Make sure that your opencv_worldXX.dll and MatchTool.Lang are in the same directory as .exe of this project

# Adaptation for OpenCV4.X
Select Debug_4.X or Release_4.X in "Solution Configuration"
![image](https://user-images.githubusercontent.com/104763587/169198235-f023ba0f-2039-4f00-8816-d270f7c03575.png)


# Usage of this project
1.	Select the Language you want
2.	Drag Source Image to the Left Area
3.	Drag Dst Image to the Right Top Area
4.	Push "Execute Button"

# Tests

test0 - with user interface

![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Manual%20Image/UIwithResult.jpg)

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
4. **Tolerance Angle**: possible rotation of targets in the inspection image (180 means search range is from -180~180), higher angle causes more execution time
      or you can push "↓" button to select 2 angle range
5. **Min Reduced Area**: the min area of toppest level in image pyrimid (trainning stage)

# About outputs
1. results are sorted by score (decreasing order)
2. Angles: inspected rotation of findings
3. PosX, PosY: pixel position of findings

# Demonstration Video
[youtube link](https://www.youtube.com/watch?v=2h_lN79SpMM)

![Image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Manual%20Gif/Fastest%20implement%20of%20Image%20Pattern%20Matching%20with%20arbitrary%20rotation%20using%20OpenCV.%E5%9F%BA%E6%96%BCOpenCV%E7%9A%84%E8%B6%85%E5%BF%AB%E9%80%9F%E5%9C%96%E5%83%8F%E5%AE%9A%E4%BD%8D%E6%BC%94%E7%AE%97%E6%B3%95.gif)

# This project can also be used as OCR
[youtube link](https://www.youtube.com/watch?v=lM0NK6xVNfg)

![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Manual%20Gif/NCCBasedOCR.gif)

