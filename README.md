# Fastest Image Pattern Matching

<p dir="auto">Using C++/MFC/OpenCV to build a NCC-Based Image Matching algorithm</p>
<p dir="auto">The Pattern Matching algorithm is based on Normalized Cross Correlation, and the formular is as followed:</p>
<p dir="auto"><a target="_blank" rel="noopener noreferrer" href="https://user-images.githubusercontent.com/104763587/167124708-e73ac583-dead-4628-8f4a-4fd6396c64f9.png"><img src="https://user-images.githubusercontent.com/104763587/167124708-e73ac583-dead-4628-8f4a-4fd6396c64f9.png" alt="image" style="max-width: 100%;"></a></p>
<h1 dir="auto"><a id="user-content-steps-to-build-this-project" class="anchor" aria-hidden="true" href="#steps-to-build-this-project"><svg class="octicon octicon-link" viewBox="0 0 16 16" version="1.1" width="16" height="16" aria-hidden="true"><path fill-rule="evenodd" d="M7.775 3.275a.75.75 0 001.06 1.06l1.25-1.25a2 2 0 112.83 2.83l-2.5 2.5a2 2 0 01-2.83 0 .75.75 0 00-1.06 1.06 3.5 3.5 0 004.95 0l2.5-2.5a3.5 3.5 0 00-4.95-4.95l-1.25 1.25zm-4.69 9.64a2 2 0 010-2.83l2.5-2.5a2 2 0 012.83 0 .75.75 0 001.06-1.06 3.5 3.5 0 00-4.95 0l-2.5 2.5a3.5 3.5 0 004.95 4.95l1.25-1.25a.75.75 0 00-1.06-1.06l-1.25 1.25a2 2 0 01-2.83 0z"></path></svg></a>steps to build this project</h1>
<ol dir="auto">
<li>Download Visual Studio 2017 or newer versions</li>
<li>Check on the option of "x86 and x64 version of C++ MFC"</li>
<li>Install</li>
<li>Open ELCVMatchTool.sln</li>
<li>Upgrade if it is required</li>
<li>Open this project's property page</li>
<li>Modified "General-Output Directory" to the .exe directory you want</li>
<li>Choose the SDK version you have in "General-Windows SDK Version"</li>
<li>Choose the right toolset you have in "General-Platform Toolset" (for me, it is Visual Studio 2017 (v141))</li>
<li>Go to "VC++ Directories", and type in "Include Directories" for your own OpenCV</li>
<li>Type in "Library Directories" for your own OpenCV's library path</li>
<li>Go to "Linker-Input", and type in library name (for me, it is opencv_world310d_vs2017.lib)</li>
<li>Make sure that your <strong>opencvXX.dll</strong> and <strong>MatchTool.Lang</strong> are in the same directory as .exe of this project</li>
</ol>
<h1 dir="auto"><a id="user-content-usage-of-this-project" class="anchor" aria-hidden="true" href="#usage-of-this-project"><svg class="octicon octicon-link" viewBox="0 0 16 16" version="1.1" width="16" height="16" aria-hidden="true"><path fill-rule="evenodd" d="M7.775 3.275a.75.75 0 001.06 1.06l1.25-1.25a2 2 0 112.83 2.83l-2.5 2.5a2 2 0 01-2.83 0 .75.75 0 00-1.06 1.06 3.5 3.5 0 004.95 0l2.5-2.5a3.5 3.5 0 00-4.95-4.95l-1.25 1.25zm-4.69 9.64a2 2 0 010-2.83l2.5-2.5a2 2 0 012.83 0 .75.75 0 001.06-1.06 3.5 3.5 0 00-4.95 0l-2.5 2.5a3.5 3.5 0 004.95 4.95l1.25-1.25a.75.75 0 00-1.06-1.06l-1.25 1.25a2 2 0 01-2.83 0z"></path></svg></a>usage of this project</h1>
<ol dir="auto">
<li>Select the Language you want</li>
<li>Drag Source Image to the Left Area</li>
<li>Drag Dst Image to the Right Top Area</li>
<li>Push "Execute Button"
<a target="_blank" rel="noopener noreferrer" href="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/ManualImage/First%20Step.jpg"><img src="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/raw/main/ManualImage/First%20Step.jpg" alt="image" style="max-width: 100%;"></a></li>
</ol>
<h1 dir="auto"><a id="user-content-tests" class="anchor" aria-hidden="true" href="#tests"><svg class="octicon octicon-link" viewBox="0 0 16 16" version="1.1" width="16" height="16" aria-hidden="true"><path fill-rule="evenodd" d="M7.775 3.275a.75.75 0 001.06 1.06l1.25-1.25a2 2 0 112.83 2.83l-2.5 2.5a2 2 0 01-2.83 0 .75.75 0 00-1.06 1.06 3.5 3.5 0 004.95 0l2.5-2.5a3.5 3.5 0 00-4.95-4.95l-1.25 1.25zm-4.69 9.64a2 2 0 010-2.83l2.5-2.5a2 2 0 012.83 0 .75.75 0 001.06-1.06 3.5 3.5 0 00-4.95 0l-2.5 2.5a3.5 3.5 0 004.95 4.95l1.25-1.25a.75.75 0 00-1.06-1.06l-1.25 1.25a2 2 0 01-2.83 0z"></path></svg></a>tests</h1>
<p dir="auto">Test 1</p>
<p dir="auto"><a target="_blank" rel="noopener noreferrer" href="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result1.jpg"><img src="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/raw/main/Result%20Images/Result1.jpg" alt="image" style="max-width: 100%;"></a></p>
<p dir="auto">Test 2</p>
<p dir="auto"><a target="_blank" rel="noopener noreferrer" href="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result2.jpg"><img src="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/raw/main/Result%20Images/Result2.jpg" alt="image" style="max-width: 100%;"></a></p>
<p dir="auto">Test 3</p>
<p dir="auto"><a target="_blank" rel="noopener noreferrer" href="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result3.jpg"><img src="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/raw/main/Result%20Images/Result3.jpg" alt="image" style="max-width: 100%;"></a></p>
<p dir="auto">Test 4</p>
<p dir="auto"><a target="_blank" rel="noopener noreferrer" href="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result4.jpg"><img src="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/raw/main/Result%20Images/Result4.jpg" alt="image" style="max-width: 100%;"></a></p>
<p dir="auto">Test 6</p>
<p dir="auto"><a target="_blank" rel="noopener noreferrer" href="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/blob/main/Result%20Images/Result6.jpg"><img src="https://github.com/DennisLiu1993/Fastest_Image_Pattern_Matching/raw/main/Result%20Images/Result6.jpg" alt="image" style="max-width: 100%;"></a></p>
<h1 dir="auto"><a id="user-content-parameters-setting" class="anchor" aria-hidden="true" href="#parameters-setting"><svg class="octicon octicon-link" viewBox="0 0 16 16" version="1.1" width="16" height="16" aria-hidden="true"><path fill-rule="evenodd" d="M7.775 3.275a.75.75 0 001.06 1.06l1.25-1.25a2 2 0 112.83 2.83l-2.5 2.5a2 2 0 01-2.83 0 .75.75 0 00-1.06 1.06 3.5 3.5 0 004.95 0l2.5-2.5a3.5 3.5 0 00-4.95-4.95l-1.25 1.25zm-4.69 9.64a2 2 0 010-2.83l2.5-2.5a2 2 0 012.83 0 .75.75 0 001.06-1.06 3.5 3.5 0 00-4.95 0l-2.5 2.5a3.5 3.5 0 004.95 4.95l1.25-1.25a.75.75 0 00-1.06-1.06l-1.25 1.25a2 2 0 01-2.83 0z"></path></svg></a>Parameters Setting</h1>
<h1 dir="auto"><a id="user-content-demonstration-video-link" class="anchor" aria-hidden="true" href="#demonstration-video-link"><svg class="octicon octicon-link" viewBox="0 0 16 16" version="1.1" width="16" height="16" aria-hidden="true"><path fill-rule="evenodd" d="M7.775 3.275a.75.75 0 001.06 1.06l1.25-1.25a2 2 0 112.83 2.83l-2.5 2.5a2 2 0 01-2.83 0 .75.75 0 00-1.06 1.06 3.5 3.5 0 004.95 0l2.5-2.5a3.5 3.5 0 00-4.95-4.95l-1.25 1.25zm-4.69 9.64a2 2 0 010-2.83l2.5-2.5a2 2 0 012.83 0 .75.75 0 001.06-1.06 3.5 3.5 0 00-4.95 0l-2.5 2.5a3.5 3.5 0 004.95 4.95l1.25-1.25a.75.75 0 00-1.06-1.06l-1.25 1.25a2 2 0 01-2.83 0z"></path></svg></a>Demonstration Video Link</h1>
<p dir="auto"><a href="https://www.youtube.com/watch?v=2h_lN79SpMM&amp;ab_channel=%E5%8A%89%E6%9F%8F%E5%AF%AC" rel="nofollow">https://www.youtube.com/watch?v=2h_lN79SpMM&amp;ab_channel=%E5%8A%89%E6%9F%8F%E5%AF%AC</a></p>
<h1 dir="auto"><a id="user-content-this-project-can-also-be-used-as-ocr-detection" class="anchor" aria-hidden="true" href="#this-project-can-also-be-used-as-ocr-detection"><svg class="octicon octicon-link" viewBox="0 0 16 16" version="1.1" width="16" height="16" aria-hidden="true"><path fill-rule="evenodd" d="M7.775 3.275a.75.75 0 001.06 1.06l1.25-1.25a2 2 0 112.83 2.83l-2.5 2.5a2 2 0 01-2.83 0 .75.75 0 00-1.06 1.06 3.5 3.5 0 004.95 0l2.5-2.5a3.5 3.5 0 00-4.95-4.95l-1.25 1.25zm-4.69 9.64a2 2 0 010-2.83l2.5-2.5a2 2 0 012.83 0 .75.75 0 001.06-1.06 3.5 3.5 0 00-4.95 0l-2.5 2.5a3.5 3.5 0 004.95 4.95l1.25-1.25a.75.75 0 00-1.06-1.06l-1.25 1.25a2 2 0 01-2.83 0z"></path></svg></a>This project can also be used as OCR detection</h1>
<p dir="auto"><a href="https://www.youtube.com/watch?v=lM0NK6xVNfg&amp;ab_channel=%E5%8A%89%E6%9F%8F%E5%AF%AC" rel="nofollow">https://www.youtube.com/watch?v=lM0NK6xVNfg&amp;ab_channel=%E5%8A%89%E6%9F%8F%E5%AF%AC</a></p>
