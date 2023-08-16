﻿
// MatchToolDlg.h: 標頭檔
//
#include <opencv2/opencv.hpp>
#ifdef OPENCV_4X
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/types_c.h>
#endif
using namespace cv;
using namespace std;
#pragma once

struct s_TemplData
{
	vector<Mat> vecPyramid;
	vector<Scalar> vecTemplMean;
	vector<double> vecTemplNorm;
	vector<double> vecInvArea;
	vector<BOOL> vecResultEqual1;
	BOOL bIsPatternLearned;
	int iBorderColor;
	void clear ()
	{
		vector<Mat> ().swap (vecPyramid);
		vector<double> ().swap (vecTemplNorm);
		vector<double> ().swap (vecInvArea);
		vector<Scalar> ().swap (vecTemplMean);
		vector<BOOL> ().swap (vecResultEqual1);
	}
	void resize (int iSize)
	{
		vecTemplMean.resize (iSize);
		vecTemplNorm.resize (iSize, 0);
		vecInvArea.resize (iSize, 1);
		vecResultEqual1.resize (iSize, FALSE);
	}
	s_TemplData ()
	{
		bIsPatternLearned = FALSE;
	}
};
struct s_MatchParameter
{
	Point2d pt;
	double dMatchScore;
	double dMatchAngle;
	//Mat matRotatedSrc;
	Rect rectRoi;
	double dAngleStart;
	double dAngleEnd;
	RotatedRect rectR;
	Rect rectBounding;
	BOOL bDelete;

	double vecResult[3][3];//for subpixel
	int iMaxScoreIndex;//for subpixel
	BOOL bPosOnBorder;
	Point2d ptSubPixel;
	double dNewAngle;

	s_MatchParameter (Point2f ptMinMax, double dScore, double dAngle)//, Mat matRotatedSrc = Mat ())
	{
		pt = ptMinMax;
		dMatchScore = dScore;
		dMatchAngle = dAngle;

		bDelete = FALSE;
		dNewAngle = 0.0;

		bPosOnBorder = FALSE;
	}
	s_MatchParameter ()
	{
		double dMatchScore = 0;
		double dMatchAngle = 0;
	}
	~s_MatchParameter ()
	{

	}
};
struct s_SingleTargetMatch
{
	Point2d ptLT, ptRT, ptRB, ptLB, ptCenter;
	double dMatchedAngle;
	double dMatchScore;
	//hungdang
	vector<Point> vecContour1;//biggest
	vector<Point> vecContour2;//second biggest
};
struct s_BlockMax
{
	struct Block 
	{
		Rect rect;
		double dMax;
		Point ptMaxLoc;
		Block ()
		{}
		Block (Rect rect_, double dMax_, Point ptMaxLoc_)
		{
			rect = rect_;
			dMax = dMax_;
			ptMaxLoc = ptMaxLoc_;
		}
	};
	s_BlockMax ()
	{}
	vector<Block> vecBlock;
	Mat matSrc;
	s_BlockMax (Mat matSrc_, Size sizeTemplate)
	{
		matSrc = matSrc_;
		//將matSrc 拆成數個block，分別計算最大值
		int iBlockW = sizeTemplate.width * 2;
		int iBlockH = sizeTemplate.height * 2;

		int iCol = matSrc.cols / iBlockW;
		BOOL bHResidue = matSrc.cols % iBlockW != 0;

		int iRow = matSrc.rows / iBlockH;
		BOOL bVResidue = matSrc.rows % iBlockH != 0;

		if (iCol == 0 || iRow == 0)
		{
			vecBlock.clear ();
			return;
		}

		vecBlock.resize (iCol * iRow);
		int iCount = 0;
		for (int y = 0; y < iRow ; y++)
		{
			for (int x = 0; x < iCol; x++)
			{
				Rect rectBlock (x * iBlockW, y * iBlockH, iBlockW, iBlockH);
				vecBlock[iCount].rect = rectBlock;
				minMaxLoc (matSrc (rectBlock), 0, &vecBlock[iCount].dMax, 0, &vecBlock[iCount].ptMaxLoc);
				vecBlock[iCount].ptMaxLoc += rectBlock.tl ();
				iCount++;
			}
		}
		if (bHResidue && bVResidue)
		{
			Rect rectRight (iCol * iBlockW, 0, matSrc.cols - iCol * iBlockW, matSrc.rows);
			Block blockRight;
			blockRight.rect = rectRight;
			minMaxLoc (matSrc (rectRight), 0, &blockRight.dMax, 0, &blockRight.ptMaxLoc);
			blockRight.ptMaxLoc += rectRight.tl ();
			vecBlock.push_back (blockRight);

			Rect rectBottom (0, iRow * iBlockH, iCol * iBlockW, matSrc.rows - iRow * iBlockH);
			Block blockBottom;
			blockBottom.rect = rectBottom;
			minMaxLoc (matSrc (rectBottom), 0, &blockBottom.dMax, 0, &blockBottom.ptMaxLoc);
			blockBottom.ptMaxLoc += rectBottom.tl ();
			vecBlock.push_back (blockBottom);
		}
		else if (bHResidue)
		{
			Rect rectRight (iCol * iBlockW, 0, matSrc.cols - iCol * iBlockW, matSrc.rows);
			Block blockRight;
			blockRight.rect = rectRight;
			minMaxLoc (matSrc (rectRight), 0, &blockRight.dMax, 0, &blockRight.ptMaxLoc);
			blockRight.ptMaxLoc += rectRight.tl ();
			vecBlock.push_back (blockRight);
		}
		else
		{
			Rect rectBottom (0, iRow * iBlockH, matSrc.cols, matSrc.rows - iRow * iBlockH);
			Block blockBottom;
			blockBottom.rect = rectBottom;
			minMaxLoc (matSrc (rectBottom), 0, &blockBottom.dMax, 0, &blockBottom.ptMaxLoc);
			blockBottom.ptMaxLoc += rectBottom.tl ();
			vecBlock.push_back (blockBottom);
		}
	}
	void UpdateMax (Rect rectIgnore)
	{
		if (vecBlock.size () == 0)
			return;
		//找出所有跟rectIgnore交集的block
		int iSize = vecBlock.size ();
		for (int i = 0; i < iSize ; i++)
		{
			Rect rectIntersec = rectIgnore & vecBlock[i].rect;
			//無交集
			if (rectIntersec.width == 0 && rectIntersec.height == 0)
				continue;
			//有交集，更新極值和極值位置
			minMaxLoc (matSrc (vecBlock[i].rect), 0, &vecBlock[i].dMax, 0, &vecBlock[i].ptMaxLoc);
			vecBlock[i].ptMaxLoc += vecBlock[i].rect.tl ();
		}
	}
	void GetMaxValueLoc (double& dMax, Point& ptMaxLoc)
	{
		int iSize = vecBlock.size ();
		if (iSize == 0)
		{
			minMaxLoc (matSrc, 0, &dMax, 0, &ptMaxLoc);
			return;
		}
		//從block中找最大值
		int iIndex = 0;
		dMax = vecBlock[0].dMax;
		for (int i = 1 ; i < iSize; i++)
		{
			if (vecBlock[i].dMax >= dMax)
			{
				iIndex = i;
				dMax = vecBlock[i].dMax;
			}
		}
		ptMaxLoc = vecBlock[iIndex].ptMaxLoc;
	}
};
// CMatchToolDlg 對話方塊
class CMatchToolDlg : public CDialogEx
{
// 建構
public:
	CMatchToolDlg(CWnd* pParent = nullptr);	// 標準建構函式

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MATCHTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int m_iMaxPos;
	double m_dMaxOverlap;
	double m_dScore;
	double m_dToleranceAngle;
	int m_iMinReduceArea;
	int m_iMessageCount;
	afx_msg void OnLoadSrc ();
private:
	CMenu m_Menu;
private:
	cv::Mat m_matSrc;
	cv::Mat m_matDst;
	void RefreshSrcView ();
	void RefreshDstView ();
	double m_dSrcScale;
	double m_dDstScale;
	CPoint m_ptMouseMove;
	s_TemplData m_TemplData;
	void LearnPattern ();
	BOOL Match ();
	int GetTopLayer (Mat* matTempl, int iMinDstLength);
	void MatchTemplate (cv::Mat& matSrc, s_TemplData* pTemplData, cv::Mat& matResult, int iLayer, BOOL bUseSIMD);
	void GetRotatedROI (Mat& matSrc, Size size, Point2f ptLT, double dAngle, Mat& matROI);
	void CCOEFF_Denominator (cv::Mat& matSrc, s_TemplData* pTemplData, cv::Mat& matResult, int iLayer);
	Size  GetBestRotationSize (Size sizeSrc, Size sizeDst, double dRAngle);
	Point2f ptRotatePt2f (Point2f ptInput, Point2f ptOrg, double dAngle);
	void FilterWithScore (vector<s_MatchParameter>* vec, double dScore);
	void FilterWithRotatedRect (vector<s_MatchParameter>* vec, int iMethod = CV_TM_CCOEFF_NORMED, double dMaxOverLap = 0);
	Point GetNextMaxLoc (Mat & matResult, Point ptMaxLoc, Size sizeTemplate, double& dMaxValue, double dMaxOverlap);
	Point GetNextMaxLoc (Mat & matResult, Point ptMaxLoc, Size sizeTemplate, double& dMaxValue, double dMaxOverlap, s_BlockMax& blockMax);
	void SortPtWithCenter (vector<Point2f>& vecSort);
	vector<s_SingleTargetMatch> m_vecSingleTargetData;
	void DrawDashLine (Mat& matDraw, Point ptStart, Point ptEnd, Scalar color1 = Scalar (0, 0, 255), Scalar color2 = Scalar::all (255));
	void DrawMarkCross (Mat& matDraw, int iX, int iY, int iLength, Scalar color, int iThickness);
	LRESULT OnShowMSG (WPARAM wMSGPointer, LPARAM lIsShowTime);
	CString m_strExecureTime;
	void LoadSrc ();
	void LoadDst ();
	BOOL m_bShowResult;
	vector<vector<Point>> m_vecChipContour;//hungdang
	void GetFirstSecondLargestContour(Mat matBinary, Rect rectBounding, vector<Point>& vecContour1, vector<Point>& vecContour2);
public:
	afx_msg void OnLoadDst ();
	afx_msg void OnDropFiles (HDROP hDropInfo);
	afx_msg void OnMouseMove (UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonExecute ();
	CListCtrl m_listMsg;
	BOOL m_bDebugMode;
	CScrollBar m_vScrollBar;
	CScrollBar m_hScrollBar;
	afx_msg BOOL OnMouseWheel (UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//縮放
	int m_iScaleTimes;
	double m_dNewScale;
	CStatusBar m_statusBar;
	//
	BOOL bInitial;
	int m_iDlgOrgWidth;
	int m_iDlgOrgHeight;
	int m_iScreenWidth;
	int m_iScreenHeight;
	float m_fMaxScaleX;
	float m_fMaxScaleY;
	CList<CRect, CRect> m_listRect;
	afx_msg void OnSize (UINT nType, int cx, int cy);
	CStatic m_staticMaxPos;
	CFont m_font;
	afx_msg HBRUSH OnCtlColor (CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CButton m_ckOutputROI;
	BOOL SubPixEsimation (vector<s_MatchParameter>* vec, double* dX, double* dY, double* dAngle, double dAngleStep, int iMaxScoreIndex);

	//輸出ROI
	void OutputRoi (s_SingleTargetMatch ss);

	CButton m_ckBitwiseNot;
	afx_msg void OnLvnKeydownListMsg (NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_bSubPixel;


	afx_msg void OnBnClickedButtonChangeToleranceMode ();
	BOOL m_bToleranceRange;
	double m_dTolerance1;
	double m_dTolerance2;
	double m_dTolerance3;
	double m_dTolerance4;

	void PumpMessages ();
	CComboBox m_cbLanSelect;


	void ChangeLanguage (CString strLan);
	CString m_strLanIndex;
	CString m_strLanScore;
	CString m_strLanAngle;
	CString m_strLanPosX;
	CString m_strLanPosY;
	CString m_strLanExecutionTime;
	CString m_strLanSourceImageSize;
	CString m_strLanDstImageSize;
	CString m_strLanPixelPos;
	afx_msg void OnCbnSelchangeComboLan ();
	CButton m_ckSIMD;
	CString m_strTotalNum;
};
