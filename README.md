# Fastest Image Pattern Matching
## **The best template matching implementation in the world.**

Using C++/MFC/OpenCV to build a Normalized Cross Corelation-based image alignment algorithm

The result means the similarity of two images, and the formular is as followed:
![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Manual%20Image/NCC.jpg)
![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Manual%20Image/HighLevel.png)

# New feature: 
1. C++ shared object (.so) with Neon SIMD for **Python** is runnable on Unix (Ventura 13.3) and Linux (Ubuntu Linux 22.04.02) System. 
   Super fast using -O3
2. C++ .so with Pybind11 for **Python**
![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Manual%20Image/CrossPlatform.png)

# Improvements
1. rotation invariant, and rotation precision is as high as possible
2. using image pyrimid as a searching strategy to speed up 4~128 times the original NCC method (depending on template size), minimizing the inspection area on the top level of image pyrimid
3. optimizing rotation time comsuming from OpenCV by setting needed "size" and modifying rotation matrix
4. **SIMD version of image convolution** (especially useful for large templates)
   
   **4.1 update Neon SIMD on MacOS version .so, super fast**
5. optimizing the function GetNextMaxLoc () with struct s_BlockMax, for special cases whose template sizes are extremely smaller than source sizes, and for large TargetNumber. 
   
   It gets quite far.
   
      Test case: [Src10](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Test%20Images/Src10.bmp) (3648 X 3648) and [Dst10](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Test%20Images/Dst10.jpg) (54 X 54)
      
      **Effect: time consuming reduces from 534 ms to 100 ms. speed up 434%**

# In Comparison with commercial libraries
<table>
  <tr>
    <td align="center">
      <img src="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Test%20Images/Src7.bmp" alt="Inspection Image" width="400"><br>
      Inspection Image: 4024 X 3036
    </td>
    <td align="center">
      <img src="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Test%20Images/Dst7.bmp" alt="Template Image" width="100"><br>
      Template Image: 762 X 521
    </td>
  </tr>
</table>

Library    |Index | Score | Angle | PosX | PosY | Execution Time
---------  |----- |-------|-------| -----  | -----  |----------------------
My Tool    |0     |   1   | 0.046 |1725.857|1045.433| **76ms**  🎖️
My Tool    |1     |   0.998   | -119.979 |2662.869|1537.446| 
My Tool    |2     |  0.991   | 120.150 |1768.936|2098.494| 
Cognex     |0     |   1   | 0.030 |1725.960|1045.470| **125ms**
Cognex     |1     |   0.989   | -119.960 |2663.750|1538.040| 
Cognex     |2     |  0.983   | 120.090 |1769.250|2099.410| 
Aisys    |0     |   1   | 0 |1726.000|1045.500| **202ms**
Aisys    |1     |   0.990   | -119.935 |2663.630|1539.060| 
Aisys    |2     |  0.979   | 120.000 |1769.63|2099.780| 

**note**: if you want to get a best performance, please make sure you are using release verson (both this project and OpenCV dll). That's because O2-related settings significantly affects efficiency, and the difference of Debug and Release can up to 7 times for some cases.

# Performance Tests (I7-10700)
**test0** - with user interface
![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Manual%20Image/UIwithResult.jpg)

| Test | Metrics | Image |
|------|---------|-------|
| **Test1** | **Execution Time**:<br>164ms<br>(80ms with SIMD)<br><br>**Parameters**:<br> TargetNum=5<br>Overlap=0.8<br>Score=0.8<br>Tolerance Angle=180 | <img src="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result8.jpg" alt="Test 1 Result" width="800"> |
| **Test2** | **Execution Time**:<br>237ms<br>(175ms with SIMD) | <img src="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result1.jpg" alt="Test 2 Result" width="800"> |
| **Test3** | **Execution Time**:<br>152ms<br>(100ms with SIMD) | <img src="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result2.jpg" alt="Test 3 Result" width="800"> |
| **Test4** | **Execution Time**:<br>21ms<br><br>**Parameters**:<br>TargetNum=38<br>Score=0.8<br>Tolerance Angle=0<br>Min Reduced Area=256 | <img src="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result3.jpg" alt="Test 4 Result" width="800"> |
| **Test5** | **Execution Time**:<br>27ms | <img src="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result4.jpg" alt="Test 5 Result" width="800"> |
| **Test6** | **Execution Time**:<br>1157ms<br>(657ms with SIMD)<br><br>**Parameters**:<br>TargetNum=15<br>Score=0.8<br>Tolerance Angle=180<br>Min Reduced Area=256 | <img src="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result6.jpg" alt="Test 6 Result" width="800"> |
| **Test7** | **Execution Time**:<br>18ms<br><br>**Parameters**:<br>TargetNum=100<br>Score=0.5<br>Tolerance Angle=0<br>MaxOverlap=0.5<br>Min Reduced Area=1024 | <img src="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result9.jpg" alt="Test 7 Result" width="800"> |

# Steps to build this project
1.	Download Visual Studio 2017 or newer versions
2.	Check on the option of "x86 and x64 version of C++ MFC"
3.	Install
4.	Open MatchTool.vcxproj
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
1.Select Debug_4.X or Release_4.X in "Solution Configuration"
![image](https://user-images.githubusercontent.com/104763587/169198235-f023ba0f-2039-4f00-8816-d270f7c03575.png)

2.Do step 10~12 in previous section

# Usage of this project
1.	Select the Language you want
2.	Drag Source Image to the Left Area
3.	Drag Dst Image to the Right Top Area
4.	Push "Execute Button"

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

# This project can also be used as Optical Character Recognition (OCR)
[youtube link](https://www.youtube.com/watch?v=lM0NK6xVNfg)


![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Manual%20Gif/NCCBasedOCR.gif)

# Special Items 
contact information: dennisliu1993@gmail.com
1. C++ shared library (.so) for python (Unix-ARM64, Ubuntu 22.04.02-ARM64)
2. C++/MFC dll for .Net framework (Windows)
3. pure C++ dll for Python (Windows)
4. pybind11 .so

![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Manual%20Gif/macFPM.gif)
![image](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Manual%20Gif/FPMLib_for_Python.gif)

# Reference Papers
1. [Template Matching using Fast Normalized Cross Correlation](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Template%20Matching%20using%20Fast%20Normalized%20Cross%20Correlation.pdf)
2. [computers_and_electrical_engineering_an_accelerating_cpu_based_correlation-based_image_alignment](https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/computers_and_electrical_engineering_an_accelerating_cpu_based_correlation-based_image_alignment.pdf)

# Special Note:

If you encounter an error(exception) on the constructor of opencv class "RotatedRect", modify the content in `types.cpp`:
this might due to Windows updates

```cpp
RotatedRect::RotatedRect(const Point2f& _point1, const Point2f& _point2, const Point2f& _point3)
{
    Point2f _center = 0.5f * (_point1 + _point3);
    Vec2f vecs[2];
    vecs[0] = Vec2f(_point1 - _point2);
    vecs[1] = Vec2f(_point2 - _point3);
    double x = std::max(norm(_point1), std::max(norm(_point2), norm(_point3)));
    double a = std::min(norm(vecs[0]), norm(vecs[1]));
    // check that given sides are perpendicular
    // this is the line you need to modify
    CV_Assert( std::fabs(vecs[0].ddot(vecs[1])) * a <= FLT_EPSILON * 9 * x * (norm(vecs[0]) * norm(vecs[1])) );

    // wd_i stores which vector (0,1) or (1,2) will make the width
    // One of them will definitely have slope within -1 to 1
    int wd_i = 0;
    if( std::fabs(vecs[1][1]) < std::fabs(vecs[1][0]) ) wd_i = 1;
    int ht_i = (wd_i + 1) % 2;

    float _angle = std::atan(vecs[wd_i][1] / vecs[wd_i][0]) * 180.0f / (float) CV_PI;
    float _width = (float) norm(vecs[wd_i]);
    float _height = (float) norm(vecs[ht_i]);

    center = _center;
    size = Size2f(_width, _height);
    angle = _angle;
}
```

modify threshold value of **CV_Assert line** to a bigger one

then recompile the source code
