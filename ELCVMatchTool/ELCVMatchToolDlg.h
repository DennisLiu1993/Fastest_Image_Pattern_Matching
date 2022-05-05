
// ELCVMatchToolDlg.h: 標頭檔
//
#include <opencv2/opencv.hpp>
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
};
// CELCVMatchToolDlg 對話方塊
class CELCVMatchToolDlg : public CDialogEx
{
// 建構
public:
	CELCVMatchToolDlg(CWnd* pParent = nullptr);	// 標準建構函式

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ELCVMATCHTOOL_DIALOG };
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
	void MatchTemplate (cv::Mat& matSrc, s_TemplData* pTemplData, cv::Mat& matResult, int iLayer);
	void GetRotatedROI (Mat& matSrc, Size size, Point2f ptLT, double dAngle, Mat& matROI);
	void CCOEFF_Denominator (cv::Mat& matSrc, s_TemplData* pTemplData, cv::Mat& matResult, int iLayer);
	Size  GetBestRotationSize (Size sizeSrc, Size sizeDst, double dRAngle);
	Point2f ptRotatePt2f (Point2f ptInput, Point2f ptOrg, double dAngle);
	void FilterWithScore (vector<s_MatchParameter>* vec, double dScore);
	void FilterWithRotatedRect (vector<s_MatchParameter>* vec, int iMethod = CV_TM_CCOEFF_NORMED, double dMaxOverLap = 0);
	Point GetNextMaxLoc (Mat & matResult, Point ptMaxLoc, double dMinValue, int iTemplateW, int iTemplateH, double& dMaxValue, double dMaxOverlap);
	void SortPtWithCenter (vector<Point2f>& vecSort);
	vector<s_SingleTargetMatch> m_vecSingleTargetData;
	void DrawDashLine (Mat& matDraw, Point ptStart, Point ptEnd, Scalar color1 = Scalar (0, 0, 255), Scalar color2 = Scalar::all (255));
	void DrawMarkCross (Mat& matDraw, int iX, int iY, int iLength, Scalar color, int iThickness);
	LRESULT OnShowMSG (WPARAM wMSGPointer, LPARAM lIsShowTime);
	CString m_strExecureTime;
	void LoadSrc ();
	void LoadDst ();
	BOOL m_bShowResult;
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
};
