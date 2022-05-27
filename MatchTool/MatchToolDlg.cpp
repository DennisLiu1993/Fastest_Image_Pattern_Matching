
// ELCVMatchToolDlg.cpp: 實作檔案
//

#include "pch.h"
#include "framework.h"
#include "MatchTool.h"
#include "MatchToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#endif

#define VISION_TOLERANCE 0.0000001
#define D2R (CV_PI / 180.0)
#define R2D (180.0 / CV_PI)
#define MATCH_CANDIDATE_NUM 1

#define SUBITEM_INDEX 0
#define SUBITEM_SCORE 1
#define SUBITEM_ANGLE 2
#define SUBITEM_POS_X 3
#define SUBITEM_POS_Y 4

#define MAX_SCALE_TIMES 10
#define MIN_SCALE_TIMES 0
#define SCALE_RATIO 1.25

#define FONT_SIZE 115
// CELCVMatchToolDlg 對話方塊

bool compareScoreBig2Small (const s_MatchParameter& lhs, const s_MatchParameter& rhs) { return  lhs.dMatchScore > rhs.dMatchScore; }
bool comparePtWithAngle (const pair<Point2f, double> lhs, const pair<Point2f, double> rhs) { return lhs.second < rhs.second; }
bool compareMatchResultByPos (const s_SingleTargetMatch& lhs, const s_SingleTargetMatch& rhs)
{
	double dTol = 2;
	if (fabs (lhs.ptCenter.y - rhs.ptCenter.y) <= dTol)
		return lhs.ptCenter.x < rhs.ptCenter.x;
	else
		return lhs.ptCenter.y < rhs.ptCenter.y;

};
bool compareMatchResultByScore (const s_SingleTargetMatch& lhs, const s_SingleTargetMatch& rhs) { return lhs.dMatchScore > rhs.dMatchScore; }
bool compareMatchResultByPosX (const s_SingleTargetMatch& lhs, const s_SingleTargetMatch& rhs) { return lhs.ptCenter.x < rhs.ptCenter.x; }

void MouseCall (int event, int x, int y, int flag, void* pUserData);
const Scalar colorWaterBlue (230, 255, 102);
const Scalar colorBlue (255, 0, 0);
const Scalar colorYellow (0, 255, 255);
const Scalar colorRed (0, 0, 255);
const Scalar colorBlack (0, 0, 0);
const Scalar colorGray (200, 200, 200);
const Scalar colorSystem (240, 240, 240);
const Scalar colorGreen (0, 255, 0);
const Scalar colorWhite (255, 255, 255);
const Scalar colorPurple (214, 112, 218);
const Scalar colorGoldenrod (15, 185, 255);

CELCVMatchToolDlg::CELCVMatchToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ELCVMATCHTOOL_DIALOG, pParent)
	, m_iMaxPos (5)
	, m_dMaxOverlap (0)
	, m_dScore (0.8)
	, m_dToleranceAngle (0)
	, m_iMinReduceArea (64)
	, m_bDebugMode (FALSE)
	, m_dTolerance1 (40)
	, m_dTolerance2 (60)
	, m_dTolerance3 (-110)
	, m_dTolerance4 (-100)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_dDstScale = 1;
	m_dSrcScale = 1;
	m_iScaleTimes = 0;
	m_dNewScale = 1;
	m_bShowResult = FALSE;
	bInitial = FALSE;
	m_bToleranceRange = FALSE;
}

void CELCVMatchToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange (pDX);
	DDX_Text (pDX, IDC_EDIT_MAX_POS, m_iMaxPos);
	DDV_MinMaxInt (pDX, m_iMaxPos, 1, 200);
	DDX_Text (pDX, IDC_EDIT_MAX_OVERLAP, m_dMaxOverlap);
	DDV_MinMaxDouble (pDX, m_dMaxOverlap, 0, 0.8);
	DDX_Text (pDX, IDC_EDIT_SCORE, m_dScore);
	DDV_MinMaxDouble (pDX, m_dScore, 0, 1);
	DDX_Text (pDX, IDC_EDIT_TOLERANCE_ANGLE, m_dToleranceAngle);
	DDV_MinMaxDouble (pDX, m_dToleranceAngle, 0, 180);
	DDX_Text (pDX, IDC_EDIT_MIN_REDUCE_AREA, m_iMinReduceArea);
	DDV_MinMaxInt (pDX, m_iMinReduceArea, 64, 2048);
	DDX_Control (pDX, IDC_LIST_MSG, m_listMsg);
	DDX_Check (pDX, IDC_CHECK_DEBUG, m_bDebugMode);
	DDX_Control (pDX, IDC_SCROLLBAR_V, m_vScrollBar);
	DDX_Control (pDX, IDC_SCROLLBAR_H, m_hScrollBar);
	DDX_Control (pDX, IDC_STATIC_MAX_POS, m_staticMaxPos);
	DDX_Control (pDX, IDC_CHECK_OUTPUT_ROI, m_ckOutputROI);
	DDX_Control (pDX, IDC_CHECK_BITWISE_NOT, m_ckBitwiseNot);
	DDX_Control (pDX, IDC_CHECK_SUBPIXEL, m_bSubPixel);
	DDX_Text (pDX, IDC_EDIT_TOLERANCE1, m_dTolerance1);
	DDX_Text (pDX, IDC_EDIT_TOLERANCE2, m_dTolerance2);
	DDX_Text (pDX, IDC_EDIT_TOLERANCE3, m_dTolerance3);
	DDX_Text (pDX, IDC_EDIT_TOLERANCE4, m_dTolerance4);
	DDX_Control (pDX, IDC_COMBO_LAN, m_cbLanSelect);
}

BEGIN_MESSAGE_MAP(CELCVMatchToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND (ID_LOAD_SRC, &CELCVMatchToolDlg::OnLoadSrc)
	ON_COMMAND (ID_LOAD_DST, &CELCVMatchToolDlg::OnLoadDst)
	ON_WM_DROPFILES ()
	ON_WM_MOUSEMOVE ()
	ON_BN_CLICKED (IDC_BUTTON_EXECUTE, &CELCVMatchToolDlg::OnBnClickedButtonExecute)
	ON_WM_MOUSEWHEEL ()
	ON_WM_HSCROLL ()
	ON_WM_VSCROLL ()
	ON_WM_SIZE ()
	ON_WM_CTLCOLOR ()
	ON_NOTIFY (LVN_KEYDOWN, IDC_LIST_MSG, &CELCVMatchToolDlg::OnLvnKeydownListMsg)
	ON_WM_SETCURSOR ()
	ON_WM_LBUTTONDOWN ()
	ON_BN_CLICKED (IDC_BUTTON_CHANGE_TOLERANCE_MODE, &CELCVMatchToolDlg::OnBnClickedButtonChangeToleranceMode)
	ON_CBN_SELCHANGE (IDC_COMBO_LAN, &CELCVMatchToolDlg::OnCbnSelchangeComboLan)
END_MESSAGE_MAP()


// CELCVMatchToolDlg 訊息處理常式

BOOL CELCVMatchToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示
	// TODO: 在此加入額外的初始設定
	

	cvNamedWindow ("SrcView");
	HWND hWnd = (HWND)cvGetWindowHandle ("SrcView");
	HWND hParent = ::GetParent (hWnd);
	::ShowWindow (hParent, SW_HIDE);
	::SetParent (hWnd, GetDlgItem (IDC_STATIC_SRC_VIEW)->m_hWnd);
	::ShowWindow (hWnd, SW_SHOW);



	cvNamedWindow ("DstView");
	hWnd = (HWND)cvGetWindowHandle ("DstView");
	hParent = ::GetParent (hWnd);
	::ShowWindow (hParent, SW_HIDE);
	::SetParent (hWnd, GetDlgItem (IDC_STATIC_DST_VIEW)->m_hWnd);
	::ShowWindow (hWnd, SW_HIDE);
	//菜單
	m_Menu.LoadMenuW (IDR_MENU_FILE);
	SetMenu (NULL);
	SetMenu (&m_Menu);
	

	//拖曳檔案
	DragAcceptFiles ();
	//訊息
	m_listMsg.InsertColumn (0, m_strLanIndex, LVCFMT_CENTER, 50);
	m_listMsg.InsertColumn (1, m_strLanScore, LVCFMT_CENTER, 70);
	m_listMsg.InsertColumn (2, m_strLanAngle, LVCFMT_CENTER, 100);
	m_listMsg.InsertColumn (3, m_strLanPosX, LVCFMT_CENTER, 110);
	m_listMsg.InsertColumn (4, m_strLanPosY, LVCFMT_CENTER, 110);

	m_listMsg.SetExtendedStyle (LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_GRIDLINES);
	m_iMessageCount = 0;
	//狀態欄
	CRect rect, rectState;
	GetClientRect (rect);
	static UINT indicators[] = {
		IDS_STRING_EXCUTE_TIME,
		IDS_STRING_SRC_SIZE,
		IDS_STRING_DST_SIZE,
		IDS_STRING_POS
	};

	if (!m_statusBar.Create (this) || !m_statusBar.SetIndicators (indicators, sizeof (indicators) / sizeof (UINT)))
	{
		TRACE0 ("Can't create status bar\n");
		return false;
	}
	m_statusBar.SetPaneInfo (0, indicators[0], SBPS_NOBORDERS, 150);//设置状态栏的宽度
	m_statusBar.SetPaneInfo (1, indicators[1], SBPS_STRETCH, 200);
	m_statusBar.SetPaneInfo (2, indicators[2], SBPS_STRETCH, 200);
	m_statusBar.SetPaneInfo (3, indicators[3], SBPS_STRETCH, 200);
	m_statusBar.SetPaneText (0, m_strLanExecutionTime);
	m_statusBar.SetPaneText (1, m_strLanSourceImageSize);
	m_statusBar.SetPaneText (2, m_strLanDstImageSize);
	m_statusBar.SetPaneText (3, m_strLanPixelPos);
	m_statusBar.GetClientRect (&rectState);
	m_statusBar.MoveWindow (0, rect.bottom - rectState.Height (), rect.right, rectState.Height ());// 调整状态栏的位置和大小

	RepositionBars (AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	
	//滾輪縮放
		m_hScrollBar.ShowWindow (SW_HIDE);
	m_vScrollBar.ShowWindow (SW_HIDE);
	m_hScrollBar.SetScrollRange (0, 1);
	m_vScrollBar.SetScrollRange (0, 1);
	m_hScrollBar.SetScrollPos (0, 0);
	m_vScrollBar.SetScrollPos (0, 0);
	
	//滑鼠事件
	setMouseCallback ("SrcView", MouseCall, (void*)this);
	//視窗縮放
	m_font.CreatePointFont (FONT_SIZE, L"Times New Roman", NULL);
	m_listRect.AddTail (CRect (rect.left, rect.top, rect.Width (), rect.Height () - rectState.Height () * 2));
	CWnd *pWndChild = GetWindow (GW_CHILD);
	while (pWndChild)//依次獲得對話框上控件的坐標，並將全部的控件坐標存儲在鍊錶中
	{
		pWndChild->GetWindowRect (&rect);
		m_listRect.AddTail (&rect); //因為依次將控件坐標添加到鍊錶末尾，因此開頭的坐標是對話框的坐標
		pWndChild->SetFont (&m_font);
		pWndChild = pWndChild->GetNextWindow ();
	} 
	m_hScrollBar.GetWindowRect (&rect);
	m_listRect.AddTail (&rect);
	m_vScrollBar.GetWindowRect (&rect);
	m_listRect.AddTail (&rect);
	m_statusBar.GetWindowRect (&rect);
	m_listRect.AddTail (&rect);
	bInitial = TRUE;
	//字體
	m_staticMaxPos.SetFont (&m_font);
	//比對角度範圍設定
	GetDlgItem (IDC_EDIT_TOLERANCE1)->EnableWindow (FALSE);
	GetDlgItem (IDC_EDIT_TOLERANCE2)->EnableWindow (FALSE);
	GetDlgItem (IDC_EDIT_TOLERANCE3)->EnableWindow (FALSE);
	GetDlgItem (IDC_EDIT_TOLERANCE4)->EnableWindow (FALSE);

	//Language
	m_cbLanSelect.AddString (L"Chinese (Tranditional)");
	m_cbLanSelect.AddString (L"Chinese (Simplified)");
	m_cbLanSelect.AddString (L"English");
	m_cbLanSelect.SetCurSel (2);

	//語言
	ChangeLanguage (L"English");
	//GetDlgItem (IDC_STATIC_MAX_POS)->SetFont (&font);
	
	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。



// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CELCVMatchToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CELCVMatchToolDlg::OnLoadSrc ()
{
	CString szFilter = L"|Bitmap Files(*.bmp)|*.bmpJPG Files (*.jpg)|*.jpg|All Files(*.*)|*.*||";

	//以Dialog方式開啟檔案
	CFileDialog fd (true, NULL, NULL, OFN_HIDEREADONLY, szFilter, NULL);

	if (fd.DoModal () == IDOK)  //按下OK
	{
		//開啟檔案成功
		CString szFileName = fd.GetPathName (); //取得開啟檔案的全名(包含路徑)
		
		USES_CONVERSION;

		m_matSrc = imread (String (T2A (szFileName.GetBuffer ())), IMREAD_GRAYSCALE);
		szFileName.ReleaseBuffer ();

		LoadSrc ();
	}
}
void CELCVMatchToolDlg::RefreshSrcView ()
{
	HWND hWnd = (HWND)cvGetWindowHandle ("SrcView");
	if (!hWnd || m_matSrc.empty ())
		return;
	CWnd* pWnd = CWnd::FromHandle (hWnd);
	CDC* dc = pWnd->GetDC ();
	CRect rect;
	//將背景繪製上白色網格
	CBrush brush (HS_DIAGCROSS, RGB (200, 200, 200));
	::GetWindowRect (GetDlgItem (IDC_STATIC_SRC_VIEW)->m_hWnd, rect);
	ScreenToClient (rect);
	//dc->FillRect (CRect (0, 0, rect.Width (), rect.Height ()), &brush);

	Mat matResize, matColorSrc;
	Size size (int (m_dNewScale * m_matSrc.cols), int (m_dNewScale * m_matSrc.rows));
	cvtColor (m_matSrc, matColorSrc, COLOR_GRAY2BGR);
	resize (matColorSrc, matResize, size);
	//縮放
	int iPosX = m_hScrollBar.GetScrollPos ();
	int iPosY = m_vScrollBar.GetScrollPos ();

	int iW = int (m_matSrc.cols * m_dSrcScale), iH = int (m_matSrc.rows * m_dSrcScale);

	Rect rectShow (Point (iPosX, iPosY), Size (iW, iH));
	//縮放
	int iSize = (int)m_vecSingleTargetData.size ();

	

	if (m_bShowResult)
	{
		for (int i = 0; i < iSize; i++)
		{
			Point ptLT (m_vecSingleTargetData[i].ptLT * m_dNewScale);
			Point ptLB (m_vecSingleTargetData[i].ptLB * m_dNewScale);
			Point ptRB (m_vecSingleTargetData[i].ptRB * m_dNewScale);
			Point ptRT (m_vecSingleTargetData[i].ptRT * m_dNewScale);
			Point ptC (m_vecSingleTargetData[i].ptCenter * m_dNewScale);
			DrawDashLine (matResize, ptLT, ptLB);
			DrawDashLine (matResize, ptLB, ptRB);
			DrawDashLine (matResize, ptRB, ptRT);
			DrawDashLine (matResize, ptRT, ptLT);

			//左上及角落邊框
			Point ptDis1, ptDis2;
			if (m_matDst.cols > m_matDst.rows)
			{
				ptDis1 = (ptLB - ptLT) / 3;
				ptDis2 = (ptRT - ptLT) / 3 * (m_matDst.rows / (float)m_matDst.cols);
			}
			else
			{
				ptDis1 = (ptLB - ptLT) / 3 * (m_matDst.cols / (float)m_matDst.rows);
				ptDis2 = (ptRT - ptLT) / 3;
			}
			line (matResize, ptLT, ptLT + ptDis1 / 2, colorGreen, 1, CV_AA);
			line (matResize, ptLT, ptLT + ptDis2 / 2, colorGreen, 1, CV_AA);
			line (matResize, ptRT, ptRT + ptDis1 / 2, colorGreen, 1, CV_AA);
			line (matResize, ptRT, ptRT - ptDis2 / 2, colorGreen, 1, CV_AA);
			line (matResize, ptRB, ptRB - ptDis1 / 2, colorGreen, 1, CV_AA);
			line (matResize, ptRB, ptRB - ptDis2 / 2, colorGreen, 1, CV_AA);
			line (matResize, ptLB, ptLB - ptDis1 / 2, colorGreen, 1, CV_AA);
			line (matResize, ptLB, ptLB + ptDis2 / 2, colorGreen, 1, CV_AA);
			//

			DrawDashLine (matResize, ptLT + ptDis1, ptLT + ptDis2);
			DrawMarkCross (matResize, ptC.x, ptC.y, 5, colorGreen, 1);
			string str = format ("%d", i);
			putText (matResize, str, (ptLT + ptRT) / 2, FONT_HERSHEY_PLAIN, 1, colorGreen);
		}
	}

	resizeWindow ("SrcView", size.width, size.height);
	//if (rectShow.width + rectShow.tl ().x > m_matSrc.cols || rectShow.height + rectShow.tl ().y > m_matSrc.rows)
		//AfxMessageBox (L"1");
	imshow ("SrcView", matResize (rectShow));
	//::ShowWindow ((HWND)cvGetWindowHandle ("SrcView"), SW_SHOW);


	
	
	pWnd->ReleaseDC (dc);


}
void CELCVMatchToolDlg::RefreshDstView ()
{
	HWND hWnd = (HWND)cvGetWindowHandle ("DstView");
	if (!hWnd || m_matDst.empty ())
		return;
	CWnd* pWnd = CWnd::FromHandle (hWnd);
	CDC* dc = pWnd->GetDC ();
	CRect rect;
	//將背景繪製上白色網格
	CBrush brush (HS_DIAGCROSS, RGB (200, 200, 200));
	::GetWindowRect (GetDlgItem (IDC_STATIC_DST_VIEW)->m_hWnd, rect);
	ScreenToClient (rect);
	dc->FillRect (CRect (0, 0, rect.Width (), rect.Height ()), &brush);
	

	Mat matResize;
	Size size (int (m_dDstScale * m_matDst.cols), int (m_dDstScale * m_matDst.rows));
	resize (m_matDst, matResize, size);
	imshow ("DstView", matResize);
	resizeWindow ("DstView", size.width, size.height);

	::ShowWindow (hWnd, SW_SHOW);
	LearnPattern ();
	pWnd->ReleaseDC (dc);
}
void CELCVMatchToolDlg::DrawDashLine (Mat& matDraw, Point ptStart, Point ptEnd, Scalar color1, Scalar color2)
{
	LineIterator itLine (matDraw, ptStart, ptEnd, 8, 0);
	int iCount = itLine.count;
	bool bOdd = false;
	for (int i = 0; i < iCount; i+=1, itLine++)
	{
		if (i % 3 == 0)
		{
			//白色BGR
			(*itLine)[0] = (uchar)color2.val[0];
			(*itLine)[1] = (uchar)color2.val[1];
			(*itLine)[2] = (uchar)color2.val[2];
		}
		else
		{
			//紅色BGR
			(*itLine)[0] = (uchar)color1.val[0];
			(*itLine)[1] = (uchar)color1.val[1];
			(*itLine)[2] = (uchar)color1.val[2];
		}

	}
}
void CELCVMatchToolDlg::DrawMarkCross (Mat& matDraw, int iX, int iY, int iLength, Scalar color, int iThickness)
{
	if (matDraw.empty ())
		return;
	Point ptC (iX, iY);
	line (matDraw, ptC - Point (iLength, 0), ptC + Point (iLength, 0), color, iThickness);
	line (matDraw, ptC - Point (0, iLength), ptC + Point (0, iLength), color, iThickness);
}
void CELCVMatchToolDlg::LearnPattern ()
{
	UpdateData (1);;
	m_TemplData.clear ();

	int iTopLayer = GetTopLayer (&m_matDst, (int)sqrt ((double)m_iMinReduceArea));
	buildPyramid (m_matDst, m_TemplData.vecPyramid, iTopLayer);
	s_TemplData* templData = &m_TemplData;

	int iSize = templData->vecPyramid.size ();
	templData->resize (iSize);

	for (int i = 0; i < iSize; i++)
	{
		double invArea = 1. / ((double)templData->vecPyramid[i].rows * templData->vecPyramid[i].cols);
		Scalar templMean, templSdv;
		double templNorm = 0, templSum2 = 0;

		meanStdDev (templData->vecPyramid[i], templMean, templSdv);
		templNorm = templSdv[0] * templSdv[0] + templSdv[1] * templSdv[1] + templSdv[2] * templSdv[2] + templSdv[3] * templSdv[3];

		if (templNorm < DBL_EPSILON)
		{
			templData->vecResultEqual1[i] = TRUE;
		}
		templSum2 = templNorm + templMean[0] * templMean[0] + templMean[1] * templMean[1] + templMean[2] * templMean[2] + templMean[3] * templMean[3];


		templSum2 /= invArea;
		templNorm = std::sqrt (templNorm);
		templNorm /= std::sqrt (invArea); // care of accuracy here


		templData->vecInvArea[i] = invArea;
		templData->vecTemplMean[i] = templMean;
		templData->vecTemplNorm[i] = templNorm;
	}
	templData->bIsPatternLearned = TRUE;
}

int CELCVMatchToolDlg::GetTopLayer (Mat* matTempl, int iMinDstLength)
{
	int iTopLayer = 0;
	int iMinReduceArea = iMinDstLength * iMinDstLength;
	int iArea = matTempl->cols * matTempl->rows;
	while (iArea > iMinReduceArea)
	{
		iArea /= 4;
		iTopLayer++;
	}
	return iTopLayer;
}

void CELCVMatchToolDlg::OnLoadDst ()
{
	CString szFilter = L"|Bitmap Files(*.bmp)|*.bmpJPG Files (*.jpg)|*.jpg|All Files(*.*)|*.*||";

	//以Dialog方式開啟檔案
	CFileDialog fd (true, NULL, NULL, OFN_HIDEREADONLY, szFilter, NULL);

	if (fd.DoModal () == IDOK)  //按下OK
	{
		//開啟檔案成功
		CString szFileName = fd.GetPathName (); //取得開啟檔案的全名(包含路徑)

		USES_CONVERSION;

		m_matDst = imread (String (T2A (szFileName.GetBuffer ())), IMREAD_GRAYSCALE);
		szFileName.ReleaseBuffer ();

		LoadDst ();
	}
}


void CELCVMatchToolDlg::OnDropFiles (HDROP hDropInfo)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	USES_CONVERSION;
	int iCount_droppedfile = DragQueryFile (hDropInfo, 0xFFFFFFFF, NULL, 0);
	CRect rectSrc, rectDst;
	::GetWindowRect (GetDlgItem (IDC_STATIC_SRC_VIEW)->m_hWnd, rectSrc);
	::GetWindowRect (GetDlgItem (IDC_STATIC_DST_VIEW)->m_hWnd, rectDst);
	ScreenToClient (rectSrc);
	ScreenToClient (rectDst);
	POINT pointCursor;
	GetCursorPos (&pointCursor);
	ScreenToClient (&pointCursor);
	CPoint ptCursor (pointCursor);

	for (int i = 0; i < iCount_droppedfile; i++)
	{
		wchar_t filepath[MAX_PATH] = { 0 };
		if (DragQueryFile (hDropInfo, i, filepath, MAX_PATH) > 0)
		{
			CString cstrFile = filepath;
			String strFile (T2A (cstrFile));

			_TCHAR szPath[MAX_PATH];
			_TCHAR szDrv[_MAX_DRIVE] = _T ("");//C: or D:
			_TCHAR szDir[_MAX_DIR] = _T ("");//資料夾
			_TCHAR szName[_MAX_FNAME] = _T ("");//檔名
			_TCHAR szExt[_MAX_EXT] = _T ("");//副檔名
			//swprintf (szPath, _MAX_PATH, L"%hs", cstrFile);
			_tsplitpath_s (filepath, szDrv, _MAX_DRIVE, szDir, _MAX_DIR, szName, _MAX_FNAME, szExt, _MAX_EXT);
			_stprintf_s (szPath, MAX_PATH, _T ("%s%s%s"), szDrv, szDir, szName);
			CString cstrFileName (szPath);
			String strFileName (T2A (cstrFileName));

			CString strSize;

			if (rectSrc.PtInRect (pointCursor))
			{
				m_matSrc = imread (strFile, IMREAD_GRAYSCALE);
				LoadSrc ();
			}
			else if (rectDst.PtInRect (pointCursor))
			{
				m_matDst = imread (strFile, IMREAD_GRAYSCALE);
				LoadDst ();
			}
		}
	}
	CDialogEx::OnDropFiles (hDropInfo);
}


void CELCVMatchToolDlg::OnMouseMove (UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	POINT pointCursor;
	GetCursorPos (&pointCursor);
	ScreenToClient (&pointCursor);
	CPoint ptCursor (pointCursor);

	CDialogEx::OnMouseMove (nFlags, point);
}

void CELCVMatchToolDlg::PumpMessages ()
{
	MSG msg;

	if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (!IsDialogMessage (&msg))
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}
}
bool GetExeDir (_TCHAR* psz)
{
	if (!psz)
		return false;

	_TCHAR sz[MAX_PATH] = _T ("");
	GetModuleFileName (NULL, sz, MAX_PATH);

	_TCHAR szDrv[_MAX_DRIVE] = _T ("");
	_TCHAR szDir[_MAX_DIR] = _T ("");
	_TCHAR szName[_MAX_FNAME] = _T ("");
	_TCHAR szExt[_MAX_EXT] = _T ("");
	_tsplitpath_s (sz, szDrv, _MAX_DRIVE, szDir, _MAX_DIR, szName, _MAX_FNAME, szExt, _MAX_EXT);

	_stprintf_s (psz, MAX_PATH, _T ("%s%s"), szDrv, szDir);

	return true;
}
void CELCVMatchToolDlg::ChangeLanguage (CString strLan)
{
	if (strLan.CompareNoCase (L"Chinese (Tranditional)") == 0)
		strLan = L"Chinese_Traditional";
	else if (strLan.CompareNoCase (L"Chinese (Simplified)") == 0)
		strLan = L"Chinese_Simplified";
	else
		strLan = L"English";

	_TCHAR szExe[MAX_PATH] = _T ("");
	GetExeDir (szExe);
	CString strLanPath (szExe);
	strLanPath += L"MatchTool.lang";

	_TCHAR szBuf[MAX_PATH];

	GetPrivateProfileString (strLan, L"ImageMatchTool", L"Image Match Tool", szBuf, _MAX_PATH, strLanPath);
	SetWindowText (szBuf);

	GetPrivateProfileString (strLan, L"File", L"File", szBuf, _MAX_PATH, strLanPath);
	m_Menu.ModifyMenuW (0, MF_BYPOSITION | MF_STRING, 0, szBuf);
	CMenu *pSubMenu = m_Menu.GetSubMenu (0);
	int iMenuItemCount = pSubMenu->GetMenuItemCount ();
	for (int i = 0; i < iMenuItemCount; i++)
		pSubMenu->DeleteMenu (0, MF_BYPOSITION);
	GetPrivateProfileString (strLan, L"ReadSourceImage", L"Read Source Image", szBuf, _MAX_PATH, strLanPath);
	pSubMenu->InsertMenuW (0, MF_BYPOSITION | MF_STRING, ID_LOAD_SRC, szBuf);
	GetPrivateProfileString (strLan, L"ReadDstImage", L"Read Dst Image", szBuf, _MAX_PATH, strLanPath);
	pSubMenu->InsertMenuW (1, MF_BYPOSITION | MF_STRING, ID_LOAD_DST, szBuf);


	GetPrivateProfileString (strLan, L"ParameterSetting", L"Parameter Setting", szBuf, _MAX_PATH, strLanPath);
	GetDlgItem (IDC_GROUP_PARAMS)->SetWindowText (szBuf);
	GetPrivateProfileString (strLan, L"TargetNumber", L"Target Number:", szBuf, _MAX_PATH, strLanPath);
	GetDlgItem (IDC_STATIC_MAX_POS)->SetWindowText (szBuf);

	GetPrivateProfileString (strLan, L"MaxOverLapRatio", L"Max OverLap Ratio:", szBuf, _MAX_PATH, strLanPath);
	GetDlgItem (IDC_STATIC_MAX_OVERLAP)->SetWindowText (szBuf);
	GetPrivateProfileString (strLan, L"Score(Similarity)", L"Score (Similarity):", szBuf, _MAX_PATH, strLanPath);
	GetDlgItem (IDC_STATIC_SCORE)->SetWindowText (szBuf);
	GetPrivateProfileString (strLan, L"ToleranceAngle", L"Tolerance Angle:", szBuf, _MAX_PATH, strLanPath);
	GetDlgItem (IDC_STATIC_TOLERANCE_ANGLE)->SetWindowText (szBuf);
	GetPrivateProfileString (strLan, L"MinReducedArea", L"Min Reduced Area", szBuf, _MAX_PATH, strLanPath);
	GetDlgItem (IDC_STATIC_MIN_REDUCE_AREA)->SetWindowText (szBuf);
	GetPrivateProfileString (strLan, L"Execute", L"Execute", szBuf, _MAX_PATH, strLanPath);
	GetDlgItem (IDC_BUTTON_EXECUTE)->SetWindowText (szBuf);
	GetPrivateProfileString (strLan, L"Language", L"Language:", szBuf, _MAX_PATH, strLanPath);
	GetDlgItem (IDC_STATIC_LANGUAGE)->SetWindowText (szBuf);

	GetPrivateProfileString (strLan, L"Index", L"Index", szBuf, _MAX_PATH, strLanPath);
	m_strLanIndex = szBuf;
	GetPrivateProfileString (strLan, L"Score", L"Score", szBuf, _MAX_PATH, strLanPath);
	m_strLanScore = szBuf;
	GetPrivateProfileString (strLan, L"Angle(deg)", L"Angle (deg)", szBuf, _MAX_PATH, strLanPath);
	m_strLanAngle = szBuf;
	GetPrivateProfileString (strLan, L"PosX", L"PosX", szBuf, _MAX_PATH, strLanPath);
	m_strLanPosX = szBuf;
	GetPrivateProfileString (strLan, L"PosY", L"PosY", szBuf, _MAX_PATH, strLanPath);
	m_strLanPosY = szBuf;
	GetPrivateProfileString (strLan, L"ExecutionTime", L"Execution Time", szBuf, _MAX_PATH, strLanPath);
	m_strLanExecutionTime = szBuf;
	GetPrivateProfileString (strLan, L"SourceImageSize", L"Source Image Size", szBuf, _MAX_PATH, strLanPath);
	m_strLanSourceImageSize = szBuf;
	GetPrivateProfileString (strLan, L"DstImageSize", L"Dst Image Size", szBuf, _MAX_PATH, strLanPath);
	m_strLanDstImageSize = szBuf;
	GetPrivateProfileString (strLan, L"PixelPos", L"Pixel Pos", szBuf, _MAX_PATH, strLanPath);
	m_strLanPixelPos = szBuf;

	m_statusBar.SetPaneText (0, m_strLanExecutionTime);
	m_statusBar.SetPaneText (1, m_strLanSourceImageSize);
	m_statusBar.SetPaneText (2, m_strLanDstImageSize);
	m_statusBar.SetPaneText (3, m_strLanPixelPos);

	for (int i = 0; i < 5; i++)
	{
		LVCOLUMN lvCol;
		::ZeroMemory ((void *)&lvCol, sizeof (LVCOLUMN));
		lvCol.mask = LVCF_TEXT;
		m_listMsg.GetColumn (i, &lvCol);
		 CString str = (i == 0) ? m_strLanIndex :
			i == 1 ? m_strLanScore : 
			i == 2 ? m_strLanAngle :
			i == 3 ? m_strLanPosX : m_strLanPosY;
		 lvCol.pszText = (LPWSTR)(LPCWSTR)str;
		m_listMsg.SetColumn (i, &lvCol);
	}
	
}
//OCR
bool comparePosWithY (const pair<Point2d, char>& lhs, const pair<Point2d, char>& rhs) { return lhs.first.y < rhs.first.y; }
bool comparePosWithX (const pair<Point2d, char>& lhs, const pair<Point2d, char>& rhs){return lhs.first.x < rhs.first.x;}
//OCR
void CELCVMatchToolDlg::OnBnClickedButtonExecute ()
{
	Match ();
	return;
	//DDD OCR
	vector<pair<Point2d, char>> vecPos;
	char chLetters[36] = { '0', '1','2', '3', '4', '5', '6', '7', '8', '9',
		'A','B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 
		'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
	
	for (int i = 0; i < 36 ; i++)
	{
		String str;
		str = format ("C:\\Users\\user\\Pictures\\testImage\\OCR\\M12\\%c.jpg", chLetters[i]);
		m_matDst = imread (str, IMREAD_GRAYSCALE);
		LoadDst ();
		double d1 = clock ();
		while (clock () - d1 < 50)
			PumpMessages ();
		
		Match ();
		
		double d2 = clock ();
		while (clock () - d2 < 10)
			PumpMessages ();
		int iSize = (int)m_vecSingleTargetData.size ();
		for (size_t j = 0; j < m_vecSingleTargetData.size (); j++)
		{
			s_SingleTargetMatch sstm = m_vecSingleTargetData[j];
			vecPos.push_back (make_pair (m_vecSingleTargetData[j].ptCenter, chLetters[i]));
		}
	}
	sort (vecPos.begin (), vecPos.end (), comparePosWithY);
	int iStart = 0, iEnd = 0;
	double dTol = 10;

	for (size_t i = 0; i < vecPos.size () - 1 ; i++)
	{
		if (fabs (vecPos[i + 1].first.y - vecPos[i].first.y) < dTol)
			continue;
		iEnd = i + 1;
		sort (vecPos.begin () + iStart, vecPos.begin () + iEnd, comparePosWithX);
		iStart = i + 1;
	}
	sort (vecPos.begin () + iStart, vecPos.end (), comparePosWithX);
	Point2d ptPre = vecPos[0].first;
	CString strRet;
	for (size_t i = 0; i < vecPos.size () ; i++)
	{
		Point2d ptCurPos = vecPos[i].first;
		if (fabs (ptCurPos.y - ptPre.y) > dTol)
			strRet += "\n";
		ptPre = ptCurPos;
		strRet += vecPos[i].second;
	}
	AfxMessageBox (strRet);
	//DDD
}
BOOL CELCVMatchToolDlg::Match ()
{
	if (m_matSrc.empty () || m_matDst.empty ())
		return FALSE;
	if ((m_matDst.cols < m_matSrc.cols && m_matDst.rows > m_matSrc.rows) || (m_matDst.cols > m_matSrc.cols && m_matDst.rows < m_matSrc.rows))
		return FALSE;
	if (m_matDst.size ().area () > m_matSrc.size ().area ())
		return FALSE;
	if (!m_TemplData.bIsPatternLearned)
		return FALSE;
	UpdateData (1);
	double d1 = clock ();
	//決定金字塔層數 總共為1 + iLayer層
	int iTopLayer = GetTopLayer (&m_matDst, (int)sqrt ((double)m_iMinReduceArea));
	//建立金字塔
	vector<Mat> vecMatSrcPyr;
	if (m_ckBitwiseNot.GetCheck ())
	{
		Mat matNewSrc = 255 - m_matSrc;
		buildPyramid (matNewSrc, vecMatSrcPyr, iTopLayer);
		imshow ("1", matNewSrc);
		moveWindow ("1", 0, 0);
	}
	else
		buildPyramid (m_matSrc, vecMatSrcPyr, iTopLayer);

	s_TemplData* pTemplData = &m_TemplData;

	//第一階段以最頂層找出大致角度與ROI
	double dAngleStep = atan (2.0 / max (pTemplData->vecPyramid[iTopLayer].cols, pTemplData->vecPyramid[iTopLayer].rows)) * R2D;

	vector<double> vecAngles;
	
		if (m_bToleranceRange)
		{
			if (m_dTolerance1 >= m_dTolerance2 || m_dTolerance3 >= m_dTolerance4)
			{
				AfxMessageBox (L"角度範圍設定異常，左值須小於右值");
				return FALSE;
			}
			for (double dAngle = m_dTolerance1 ; dAngle < m_dTolerance2 + dAngleStep; dAngle += dAngleStep)
				vecAngles.push_back (dAngle);
			for (double dAngle = m_dTolerance3 ; dAngle < m_dTolerance4 + dAngleStep; dAngle += dAngleStep)
				vecAngles.push_back (dAngle);
		}
		else
		{
			if (m_dToleranceAngle < VISION_TOLERANCE)
				vecAngles.push_back (0.0);
			else
			{
				for (double dAngle = 0; dAngle < m_dToleranceAngle + dAngleStep; dAngle += dAngleStep)
					vecAngles.push_back (dAngle);
				for (double dAngle = -dAngleStep; dAngle > -m_dToleranceAngle - dAngleStep; dAngle -= dAngleStep)
					vecAngles.push_back (dAngle);
			}
		}

	int iTopSrcW = vecMatSrcPyr[iTopLayer].cols, iTopSrcH = vecMatSrcPyr[iTopLayer].rows;
	Point2f ptCenter ((iTopSrcW - 1) / 2.0f, (iTopSrcH - 1) / 2.0f);

	int iSize = (int)vecAngles.size ();
	vector<s_MatchParameter> vecMatchParameter (iSize * (m_iMaxPos + MATCH_CANDIDATE_NUM));

	for (int i = 0; i < iSize; i++)
	{
		Mat matRotatedSrc, matR = getRotationMatrix2D (ptCenter, vecAngles[i], 1);
		Mat matResult;
		Point ptMaxLoc;
		double dValue, dMaxVal;
		double dRotate = clock ();
		Size sizeBest = GetBestRotationSize (vecMatSrcPyr[iTopLayer].size (), pTemplData->vecPyramid[iTopLayer].size (), vecAngles[i]);
		float fTranslationX = (sizeBest.width - 1) / 2.0f - ptCenter.x;
		float fTranslationY = (sizeBest.height - 1) / 2.0f - ptCenter.y;
		matR.at<double> (0, 2) += fTranslationX;
		matR.at<double> (1, 2) += fTranslationY;
		warpAffine (vecMatSrcPyr[iTopLayer], matRotatedSrc, matR, sizeBest);


		MatchTemplate (matRotatedSrc, pTemplData, matResult, iTopLayer);

		minMaxLoc (matResult, 0, &dMaxVal, 0, &ptMaxLoc);

		vecMatchParameter[i * (m_iMaxPos + MATCH_CANDIDATE_NUM)] = s_MatchParameter (Point2f (ptMaxLoc.x - fTranslationX, ptMaxLoc.y - fTranslationY), dMaxVal, vecAngles[i]);

		for (int j = 0; j < m_iMaxPos + MATCH_CANDIDATE_NUM - 1; j++)
		{
			ptMaxLoc = GetNextMaxLoc (matResult, ptMaxLoc, -1, pTemplData->vecPyramid[iTopLayer].cols, pTemplData->vecPyramid[iTopLayer].rows, dValue, m_dMaxOverlap);
			vecMatchParameter[i * (m_iMaxPos + MATCH_CANDIDATE_NUM) + j + 1] = s_MatchParameter (Point2f (ptMaxLoc.x - fTranslationX, ptMaxLoc.y - fTranslationY), dValue, vecAngles[i]);
		}
	}
	FilterWithScore (&vecMatchParameter, m_dScore-0.05*iTopLayer);

	//紀錄旋轉矩形、ROI與角度
	int iMatchSize = (int)vecMatchParameter.size ();
	int iDstW = pTemplData->vecPyramid[iTopLayer].cols, iDstH = pTemplData->vecPyramid[iTopLayer].rows;
	if (m_bDebugMode)
	{
		for (int i = 0; i < iMatchSize; i++)
		{
			Point2f ptLT, ptRT, ptRB, ptLB;
			double dRAngle = -vecMatchParameter[i].dMatchAngle * D2R;
			ptLT = ptRotatePt2f (vecMatchParameter[i].pt, ptCenter, dRAngle);
			ptRT = Point2f (ptLT.x + iDstW * (float)cos (dRAngle), ptLT.y - iDstW * (float)sin (dRAngle));
			ptLB = Point2f (ptLT.x + iDstH * (float)sin (dRAngle), ptLT.y + iDstH * (float)cos (dRAngle));
			ptRB = Point2f (ptRT.x + iDstH * (float)sin (dRAngle), ptRT.y + iDstH * (float)cos (dRAngle));
			//紀錄旋轉矩形
			Point2f ptRectCenter = Point2f ((ptLT.x + ptRT.x + ptLB.x + ptRB.x) / 4.0f, (ptLT.y + ptRT.y + ptLB.y + ptRB.y) / 4.0f);
			vecMatchParameter[i].rectR = RotatedRect (ptRectCenter, pTemplData->vecPyramid[iTopLayer].size (), (float)vecMatchParameter[i].dMatchAngle);

		}
		//紀錄旋轉矩形
	}
	
	//FilterWithRotatedRect (&vecMatchParameter, CV_TM_CCORR_NORMED, m_dMaxOverlap);

	//顯示第一層結果
	if (m_bDebugMode)
	{
		Mat matShow;

		cvtColor (vecMatSrcPyr[iTopLayer], matShow, CV_GRAY2BGR);
		iMatchSize = (int)vecMatchParameter.size ();
		string str = format ("Toplayer, Candidate:%d", iMatchSize);

		for (int i = 0; i < iMatchSize; i++)
		{
			Point2f ptLT, ptRT, ptRB, ptLB;
			double dRAngle = -vecMatchParameter[i].dMatchAngle * D2R;
			ptLT = ptRotatePt2f (vecMatchParameter[i].pt, ptCenter, dRAngle);
			ptRT = Point2f (ptLT.x + iDstW * (float)cos (dRAngle), ptLT.y - iDstW * (float)sin (dRAngle));
			ptLB = Point2f (ptLT.x + iDstH * (float)sin (dRAngle), ptLT.y + iDstH * (float)cos (dRAngle));
			ptRB = Point2f (ptRT.x + iDstH * (float)sin (dRAngle), ptRT.y + iDstH * (float)cos (dRAngle));
			line (matShow, ptLT, ptLB, Scalar (0, 255, 0));
			line (matShow, ptLB, ptRB, Scalar (0, 255, 0));
			line (matShow, ptRB, ptRT, Scalar (0, 255, 0));
			line (matShow, ptRT, ptLT, Scalar (0, 255, 0));
			circle (matShow, ptLT, 1, Scalar (0, 0, 255));

			string strText = format ("%d", i);
			putText (matShow, strText, ptLT, FONT_HERSHEY_PLAIN, 1, Scalar (0, 255, 0));
			imshow (str, matShow);
			
		}
		imshow (str, matShow);
		moveWindow (str, 0, 0);
	}

	//顯示第一層結果

	//第一階段結束
	BOOL bSubPixelEstimation = m_bSubPixel.GetCheck ();
	int iStopLayer = 0;
	//int iSearchSize = min (m_iMaxPos + MATCH_CANDIDATE_NUM, (int)vecMatchParameter.size ());//可能不需要搜尋到全部 太浪費時間
	vector<s_MatchParameter> vecAllResult;
	for (int i = 0; i < (int)vecMatchParameter.size (); i++)
	//for (int i = 0; i < iSearchSize; i++)
	{
		double dRAngle = -vecMatchParameter[i].dMatchAngle * D2R;
		Point2f ptLT = ptRotatePt2f (vecMatchParameter[i].pt, ptCenter, dRAngle);

		double dAngleStep = atan (2.0 / max (iDstW, iDstH)) * R2D;//min改為max
		vecMatchParameter[i].dAngleStart = vecMatchParameter[i].dMatchAngle - dAngleStep;
		vecMatchParameter[i].dAngleEnd = vecMatchParameter[i].dMatchAngle + dAngleStep;

		if (iTopLayer <= iStopLayer)
		{
			vecMatchParameter[i].pt = Point2d (ptLT * ((iTopLayer == 0) ? 1 : 2));
			vecAllResult.push_back (vecMatchParameter[i]);
		}
		else
		{
			for (int iLayer = iTopLayer - 1; iLayer >= iStopLayer; iLayer--)
			{
				//搜尋角度
				dAngleStep = atan (2.0 / max (pTemplData->vecPyramid[iLayer].cols, pTemplData->vecPyramid[iLayer].rows)) * R2D;//min改為max
				vector<double> vecAngles;
				//double dAngleS = vecMatchParameter[i].dAngleStart, dAngleE = vecMatchParameter[i].dAngleEnd;
				/*if (m_dToleranceAngle < VISION_TOLERANCE)
					vecAngles.push_back (0.0);
				else*/
				{
					double dMatchedAngle = vecMatchParameter[i].dMatchAngle;

					for (int i = -1; i <= 1; i++)
						vecAngles.push_back (dMatchedAngle + dAngleStep * i);
				}
				Point2f ptSrcCenter ((vecMatSrcPyr[iLayer].cols - 1) / 2.0f, (vecMatSrcPyr[iLayer].rows - 1) / 2.0f);
				iSize = (int)vecAngles.size ();
				vector<s_MatchParameter> vecNewMatchParameter (iSize);
				int iMaxScoreIndex = 0;
				double dBigValue = -1;
				for (int j = 0; j < iSize; j++)
				{
					Mat rMat, matResult, matRotatedSrc;
					double dMaxValue = 0;
					Point ptMaxLoc;
					if (iLayer == 0)
						int a = 0;
					GetRotatedROI (vecMatSrcPyr[iLayer], pTemplData->vecPyramid[iLayer].size (), ptLT * 2, vecAngles[j], matRotatedSrc);
					
					MatchTemplate (matRotatedSrc, pTemplData, matResult, iLayer);
					minMaxLoc (matResult, 0, &dMaxValue, 0, &ptMaxLoc);
					vecNewMatchParameter[j] = s_MatchParameter (ptMaxLoc, dMaxValue, vecAngles[j]);
					
					if (vecNewMatchParameter[j].dMatchScore > dBigValue)
					{
						iMaxScoreIndex = j;
						dBigValue = vecNewMatchParameter[j].dMatchScore;
					}
					//次像素估計
					if (ptMaxLoc.x == 0 || ptMaxLoc.y == 0 || ptMaxLoc.y == matResult.cols - 1 || ptMaxLoc.x == matResult.rows - 1)
						vecNewMatchParameter[j].bPosOnBorder = TRUE;
					if (!vecNewMatchParameter[j].bPosOnBorder)
					{
						for (int y = -1; y <= 1; y++)
							for (int x = -1; x <= 1; x++)
								vecNewMatchParameter[j].vecResult[x + 1][y + 1] = matResult.at<float> (ptMaxLoc + Point (x, y));
					}
					//次像素估計
				}
				//sort (vecNewMatchParameter.begin (), vecNewMatchParameter.end (), compareScoreBig2Small);
				if (vecNewMatchParameter[iMaxScoreIndex].dMatchScore < m_dScore - 0.05 * iLayer)
					break;

				//次像素估計
				if (bSubPixelEstimation 
					&& iLayer == 0 
					&& (!vecNewMatchParameter[iMaxScoreIndex].bPosOnBorder) 
					&& iMaxScoreIndex != 0 
					&& iMaxScoreIndex != 4)
				{
					double dNewX = 0, dNewY = 0, dNewAngle = 0;
					SubPixEsimation ( &vecNewMatchParameter, &dNewX, &dNewY, &dNewAngle, dAngleStep, iMaxScoreIndex);
					vecNewMatchParameter[iMaxScoreIndex].pt = Point2d (dNewX, dNewY);
					vecNewMatchParameter[iMaxScoreIndex].dMatchAngle = dNewAngle;
				}
				//次像素估計

				double dNewMatchAngle = vecNewMatchParameter[iMaxScoreIndex].dMatchAngle;

				//讓坐標系回到旋轉時(GetRotatedROI)的(0, 0)
				Point2f ptPaddingLT = ptRotatePt2f (ptLT * 2, ptSrcCenter, dNewMatchAngle * D2R) - Point2f (3, 3);
				Point2f pt (vecNewMatchParameter[iMaxScoreIndex].pt.x + ptPaddingLT.x, vecNewMatchParameter[iMaxScoreIndex].pt.y + ptPaddingLT.y);
				//再旋轉
				pt = ptRotatePt2f (pt, ptSrcCenter, -dNewMatchAngle * D2R);

				if (iLayer == iStopLayer)
				{
					vecNewMatchParameter[iMaxScoreIndex].pt = pt * (iStopLayer == 0 ? 1 : 2);
					vecAllResult.push_back (vecNewMatchParameter[iMaxScoreIndex]);
				}
				else
				{
					//更新MatchAngle ptLT
					vecMatchParameter[i].dMatchAngle = dNewMatchAngle;
					vecMatchParameter[i].dAngleStart = vecMatchParameter[i].dMatchAngle - dAngleStep / 2;
					vecMatchParameter[i].dAngleEnd = vecMatchParameter[i].dMatchAngle + dAngleStep / 2;
					ptLT = pt;
				}
			}
		}
	}
	FilterWithScore (&vecAllResult, m_dScore);

	//最後再次濾掉重疊
	iDstW = pTemplData->vecPyramid[iStopLayer].cols, iDstH = pTemplData->vecPyramid[iStopLayer].rows;

	for (int i = 0; i < (int)vecAllResult.size (); i++)
	{
		Point2f ptLT, ptRT, ptRB, ptLB;
		double dRAngle = -vecAllResult[i].dMatchAngle * D2R;
		ptLT = vecAllResult[i].pt;
		ptRT = Point2f (ptLT.x + iDstW * (float)cos (dRAngle), ptLT.y - iDstW * (float)sin (dRAngle));
		ptLB = Point2f (ptLT.x + iDstH * (float)sin (dRAngle), ptLT.y + iDstH * (float)cos (dRAngle));
		ptRB = Point2f (ptRT.x + iDstH * (float)sin (dRAngle), ptRT.y + iDstH * (float)cos (dRAngle));
		//紀錄旋轉矩形
		Point2f ptRectCenter = Point2f ((ptLT.x + ptRT.x + ptLB.x + ptRB.x) / 4.0f, (ptLT.y + ptRT.y + ptLB.y + ptRB.y) / 4.0f);
		vecAllResult[i].rectR = RotatedRect (ptRectCenter, pTemplData->vecPyramid[iStopLayer].size (), (float)vecAllResult[i].dMatchAngle);
	}
	FilterWithRotatedRect (&vecAllResult, CV_TM_CCOEFF_NORMED, m_dMaxOverlap);
	//最後再次濾掉重疊

	//根據分數排序
	sort (vecAllResult.begin (), vecAllResult.end (), compareScoreBig2Small);
	m_strExecureTime.Format (L"%s : %d ms", m_strLanExecutionTime, int (clock () - d1));
	m_statusBar.SetPaneText (0, m_strExecureTime);
	
	m_vecSingleTargetData.clear ();
	m_listMsg.DeleteAllItems ();
	iMatchSize = (int)vecAllResult.size ();
	if (vecAllResult.size () == 0)
		return FALSE;
	int iW = pTemplData->vecPyramid[0].cols, iH = pTemplData->vecPyramid[0].rows;

	for (int i = 0; i < iMatchSize; i++)
	{
		s_SingleTargetMatch sstm;
		double dRAngle = -vecAllResult[i].dMatchAngle * D2R;

		sstm.ptLT = vecAllResult[i].pt;

		sstm.ptRT = Point2d (sstm.ptLT.x + iW * cos (dRAngle), sstm.ptLT.y - iW * sin (dRAngle));
		sstm.ptLB = Point2d (sstm.ptLT.x + iH * sin (dRAngle), sstm.ptLT.y + iH * cos (dRAngle));
		sstm.ptRB = Point2d (sstm.ptRT.x + iH * sin (dRAngle), sstm.ptRT.y + iH * cos (dRAngle));
		sstm.ptCenter = Point2d ((sstm.ptLT.x + sstm.ptRT.x + sstm.ptRB.x + sstm.ptLB.x) / 4, (sstm.ptLT.y + sstm.ptRT.y + sstm.ptRB.y + sstm.ptLB.y) / 4);
		sstm.dMatchedAngle = -vecAllResult[i].dMatchAngle;
		sstm.dMatchScore = vecAllResult[i].dMatchScore;

		if (sstm.dMatchedAngle < -180)
			sstm.dMatchedAngle += 360;
		if (sstm.dMatchedAngle > 180)
			sstm.dMatchedAngle -= 360;
		m_vecSingleTargetData.push_back (sstm);

		

		//Test Subpixel
		/*Point2d ptLT = vecAllResult[i].ptSubPixel;
		Point2d ptRT = Point2d (sstm.ptLT.x + iW * cos (dRAngle), sstm.ptLT.y - iW * sin (dRAngle));
		Point2d ptLB = Point2d (sstm.ptLT.x + iH * sin (dRAngle), sstm.ptLT.y + iH * cos (dRAngle));
		Point2d ptRB = Point2d (sstm.ptRT.x + iH * sin (dRAngle), sstm.ptRT.y + iH * cos (dRAngle));
		Point2d ptCenter = Point2d ((sstm.ptLT.x + sstm.ptRT.x + sstm.ptRB.x + sstm.ptLB.x) / 4, (sstm.ptLT.y + sstm.ptRT.y + sstm.ptRB.y + sstm.ptLB.y) / 4);
		CString strDiff;strDiff.Format (L"Diff(x, y):%.3f, %.3f", ptCenter.x - sstm.ptCenter.x, ptCenter.y - sstm.ptCenter.y);
		AfxMessageBox (strDiff);*/
		//Test Subpixel
		//存出MATCH ROI
		OutputRoi (sstm);
		if (i + 1 == m_iMaxPos)
			break;
	}
	//sort (m_vecSingleTargetData.begin (), m_vecSingleTargetData.end (), compareMatchResultByPosX);
	m_listMsg.DeleteAllItems ();

	for (int i = 0 ; i < (int)m_vecSingleTargetData.size (); i++)
	{
		s_SingleTargetMatch sstm = m_vecSingleTargetData[i];
		//Msg
		CString str (L"");
		m_listMsg.InsertItem (i, str);
		m_listMsg.SetCheck (i);
		str.Format (L"%d", i);
		m_listMsg.SetItemText (i, SUBITEM_INDEX, str);
		str.Format (L"%.3f", sstm.dMatchScore);
		m_listMsg.SetItemText (i, SUBITEM_SCORE, str);
		str.Format (L"%.3f", sstm.dMatchedAngle);
		m_listMsg.SetItemText (i, SUBITEM_ANGLE, str);
		str.Format (L"%.3f", sstm.ptCenter.x);
		m_listMsg.SetItemText (i, SUBITEM_POS_X, str);
		str.Format (L"%.3f", sstm.ptCenter.y);
		m_listMsg.SetItemText (i, SUBITEM_POS_Y, str);
		//Msg
	}

	//sort (m_vecSingleTargetData.begin (), m_vecSingleTargetData.end (), compareMatchResultByPos);
	m_bShowResult = TRUE;

	RefreshSrcView ();
	

	return (int)m_vecSingleTargetData.size ();
}
BOOL CELCVMatchToolDlg::SubPixEsimation (vector<s_MatchParameter>* vec, double* dNewX, double* dNewY, double* dNewAngle, double dAngleStep, int iMaxScoreIndex)
{
	//Az=S, (A.T)Az=(A.T)s, z = ((A.T)A).inv (A.T)s

	Mat matA (27, 10, CV_64F);
	Mat matZ (10, 1, CV_64F);
	Mat matS (27, 1, CV_64F);

	double dX_maxScore = (*vec)[iMaxScoreIndex].pt.x;
	double dY_maxScore = (*vec)[iMaxScoreIndex].pt.y;
	double dTheata_maxScore = (*vec)[iMaxScoreIndex].dMatchAngle;
	int iRow = 0;
	/*for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			for (int theta = 0; theta <= 2; theta++)
			{*/
	for (int theta = 0; theta <= 2; theta++)
	{
		for (int y = -1; y <= 1; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
				//xx yy tt xy xt yt x y t 1
				//0  1  2  3  4  5  6 7 8 9
				double dX = dX_maxScore + x;
				double dY = dY_maxScore + y;
				//double dT = (*vec)[theta].dMatchAngle + (theta - 1) * dAngleStep;
				double dT = (dTheata_maxScore + (theta - 1) * dAngleStep) * D2R;
				matA.at<double> (iRow, 0) = dX * dX;
				matA.at<double> (iRow, 1) = dY * dY;
				matA.at<double> (iRow, 2) = dT * dT;
				matA.at<double> (iRow, 3) = dX * dY;
				matA.at<double> (iRow, 4) = dX * dT;
				matA.at<double> (iRow, 5) = dY * dT;
				matA.at<double> (iRow, 6) = dX;
				matA.at<double> (iRow, 7) = dY;
				matA.at<double> (iRow, 8) = dT;
				matA.at<double> (iRow, 9) = 1.0;
				matS.at<double> (iRow, 0) = (*vec)[iMaxScoreIndex + (theta - 1)].vecResult[x + 1][y + 1];
				iRow++;
#ifdef _DEBUG
				/*string str = format ("%.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f", dValueA[0], dValueA[1], dValueA[2], dValueA[3], dValueA[4], dValueA[5], dValueA[6], dValueA[7], dValueA[8], dValueA[9]);
				fileA <<  str << endl;
				str = format ("%.6f", dValueS[iRow]);
				fileS << str << endl;*/
#endif
			}
		}
	}
	//求解Z矩陣，得到k0~k9
	//[ x* ] = [ 2k0 k3 k4 ]-1 [ -k6 ]
	//| y* | = | k3 2k1 k5 |   | -k7 |
	//[ t* ] = [ k4 k5 2k2 ]   [ -k8 ]
	
	//solve (matA, matS, matZ, DECOMP_SVD);
	matZ = (matA.t () * matA).inv () * matA.t ()* matS;
	Mat matZ_t;
	transpose (matZ, matZ_t);
	double* dZ = matZ_t.ptr<double> (0);
	Mat matK1 = (Mat_<double> (3, 3) << 
		(2 * dZ[0]), dZ[3], dZ[4], 
		dZ[3], (2 * dZ[1]), dZ[5], 
		dZ[4], dZ[5], (2 * dZ[2]));
	Mat matK2 = (Mat_<double> (3, 1) << -dZ[6], -dZ[7], -dZ[8]);
	Mat matDelta = matK1.inv () * matK2;

	*dNewX = matDelta.at<double> (0, 0);
	*dNewY = matDelta.at<double> (1, 0);
	*dNewAngle = matDelta.at<double> (2, 0) * R2D;
	return TRUE;
}

void CELCVMatchToolDlg::OutputRoi (s_SingleTargetMatch sstm)
{
	if (!m_ckOutputROI.GetCheck ())
		return;
	Rect rect (sstm.ptLT, sstm.ptRB);
	for (int i = 1 ; i < 50 ; i++)
	{
		String strName = format ("C:\\Users\\Dennis\\Desktop\\testImage\\MatchFail\\workSpace\\roi%d.bmp", i);
		if (::PathFileExists (CString (strName.c_str ())))
			continue;
		imwrite (strName, m_matSrc (rect));
		break;
	}
}
void CELCVMatchToolDlg::MatchTemplate (cv::Mat& matSrc, s_TemplData* pTemplData, cv::Mat& matResult, int iLayer)
{
	matchTemplate (matSrc, pTemplData->vecPyramid[iLayer], matResult, CV_TM_CCORR);
	CCOEFF_Denominator (matSrc, pTemplData, matResult, iLayer);
}
void CELCVMatchToolDlg::GetRotatedROI (Mat& matSrc, Size size, Point2f ptLT, double dAngle, Mat& matROI)
{
	double dAngle_radian = dAngle * D2R;
	Point2f ptC ((matSrc.cols - 1) / 2.0f, (matSrc.rows - 1) / 2.0f);
	Point2f ptLT_rotate = ptRotatePt2f (ptLT, ptC, dAngle_radian);
	Size sizePadding (size.width + 6, size.height + 6);


	Mat rMat = getRotationMatrix2D (ptC, dAngle, 1);
	rMat.at<double> (0, 2) -= ptLT_rotate.x - 3;
	rMat.at<double> (1, 2) -= ptLT_rotate.y - 3;
	//平移旋轉矩陣(0, 2) (1, 2)的減，為旋轉後的圖形偏移，-= ptLT_rotate.x - 3 代表旋轉後的圖形往-X方向移動ptLT_rotate.x - 3
	//Debug
	
	//Debug
	warpAffine (matSrc, matROI, rMat, sizePadding);
}
void CELCVMatchToolDlg::CCOEFF_Denominator (cv::Mat& matSrc, s_TemplData* pTemplData, cv::Mat& matResult, int iLayer)
{
	if (pTemplData->vecResultEqual1[iLayer])
	{
		matResult = Scalar::all (1);
		return;
	}
	double *q0 = 0, *q1 = 0, *q2 = 0, *q3 = 0;

	Mat sum, sqsum;
	integral (matSrc, sum, sqsum, CV_32F, CV_64F);

	double d2 = clock ();

	q0 = (double*)sqsum.data;
	q1 = q0 + pTemplData->vecPyramid[iLayer].cols;
	q2 = (double*)(sqsum.data + pTemplData->vecPyramid[iLayer].rows * sqsum.step);
	q3 = q2 + pTemplData->vecPyramid[iLayer].cols;

	float* p0 = (float*)sum.data;
	float* p1 = p0 + pTemplData->vecPyramid[iLayer].cols;
	float* p2 = (float*)(sum.data + pTemplData->vecPyramid[iLayer].rows*sum.step);
	float* p3 = p2 + pTemplData->vecPyramid[iLayer].cols;

	int sumstep = sum.data ? (int)(sum.step / sizeof (float)) : 0;
	int sqstep = sqsum.data ? (int)(sqsum.step / sizeof (double)) : 0;

	//
	double dTemplMean0 = pTemplData->vecTemplMean[iLayer][0];
	double dTemplNorm = pTemplData->vecTemplNorm[iLayer];
	double dInvArea = pTemplData->vecInvArea[iLayer];
	//

	int i, j;
	for (i = 0; i < matResult.rows; i++)
	{
		float* rrow = matResult.ptr<float> (i);
		int idx = i * sumstep;
		int idx2 = i * sqstep;

		for (j = 0; j < matResult.cols; j += 1, idx += 1, idx2 += 1)
		{
			double num = rrow[j], t;
			double wndMean2 = 0, wndSum2 = 0;

			t = p0[idx] - p1[idx] - p2[idx] + p3[idx];
			wndMean2 += t * t;
			num -= t * dTemplMean0;
			wndMean2 *= dInvArea;


			t = q0[idx2] - q1[idx2] - q2[idx2] + q3[idx2];
			wndSum2 += t;


			t = std::sqrt (MAX (wndSum2 - wndMean2, 0)) * dTemplNorm;

			if (fabs (num) < t)
				num /= t;
			else if (fabs (num) < t * 1.125)
				num = num > 0 ? 1 : -1;
			else
				num = 0;

			rrow[j] = (float)num;
		}
	}
}
Size CELCVMatchToolDlg::GetBestRotationSize (Size sizeSrc, Size sizeDst, double dRAngle)
{
	double dRAngle_radian = dRAngle * D2R;
	Point ptLT (0, 0), ptLB (0, sizeSrc.height - 1), ptRB (sizeSrc.width - 1, sizeSrc.height - 1), ptRT (sizeSrc.width - 1, 0);
	Point2f ptCenter ((sizeSrc.width - 1) / 2.0f, (sizeSrc.height - 1) / 2.0f);
	Point2f ptLT_R = ptRotatePt2f (Point2f (ptLT), ptCenter, dRAngle_radian);
	Point2f ptLB_R = ptRotatePt2f (Point2f (ptLB), ptCenter, dRAngle_radian);
	Point2f ptRB_R = ptRotatePt2f (Point2f (ptRB), ptCenter, dRAngle_radian);
	Point2f ptRT_R = ptRotatePt2f (Point2f (ptRT), ptCenter, dRAngle_radian);

	float fTopY = max (max (ptLT_R.y, ptLB_R.y), max (ptRB_R.y, ptRT_R.y));
	float fRightestX = max (max (ptLT_R.x, ptLB_R.x), max (ptRB_R.x, ptRT_R.x));

	if (fabs (fabs (dRAngle) - 90) < VISION_TOLERANCE || fabs (fabs (dRAngle) - 270) < VISION_TOLERANCE)
	{
		return Size (sizeSrc.height, sizeSrc.width);
	}
	else if (fabs (dRAngle) < VISION_TOLERANCE || fabs (fabs (dRAngle) - 180) < VISION_TOLERANCE)
	{
		return sizeSrc;
	}
	if (dRAngle > 360)
		dRAngle -= 360;
	else if (dRAngle < 0)
		dRAngle += 360;
	double dAngle = dRAngle;

	if (dAngle > 0 && dAngle < 90)
	{
		;
	}
	else if (dAngle > 90 && dAngle < 180)
	{
		dAngle -= 90;
	}
	else if (dAngle > 180 && dAngle < 270)
	{
		dAngle -= 180;
	}
	else if (dAngle > 270 && dAngle < 360)
	{
		dAngle -= 270;
	}
	else//Debug
	{
		AfxMessageBox (L"Unkown");
	}

	float fH1 = sizeDst.width * sin (dAngle * D2R) * cos (dAngle * D2R);
	float fH2 = sizeDst.height * sin (dAngle * D2R) * cos (dAngle * D2R);

	int iHalfHeight = (int)ceil (fTopY - ptCenter.y - fH1);
	int iHalfWidth = (int)ceil (fRightestX - ptCenter.x - fH2);

	return Size (iHalfWidth * 2, iHalfHeight * 2);
}
Point2f CELCVMatchToolDlg::ptRotatePt2f (Point2f ptInput, Point2f ptOrg, double dAngle)
{
	double dWidth = ptOrg.x * 2;
	double dHeight = ptOrg.y * 2;
	double dY1 = dHeight - ptInput.y, dY2 = dHeight - ptOrg.y;

	double dX = (ptInput.x - ptOrg.x) * cos (dAngle) - (dY1 - ptOrg.y) * sin (dAngle) + ptOrg.x;
	double dY = (ptInput.x - ptOrg.x) * sin (dAngle) + (dY1 - ptOrg.y) * cos (dAngle) + dY2;

	dY = -dY + dHeight;
	return Point2f ((float)dX, (float)dY);
}
void CELCVMatchToolDlg::FilterWithScore (vector<s_MatchParameter>* vec, double dScore)
{
	sort (vec->begin (), vec->end (), compareScoreBig2Small);
	int iSize = vec->size (), iIndexDelete = iSize + 1;
	for (int i = 0; i < iSize; i++)
	{
		if ((*vec)[i].dMatchScore < dScore)
		{
			iIndexDelete = i;
			break;
		}
	}
	if (iIndexDelete == iSize + 1)//沒有任何元素小於dScore
		return;
	vec->erase (vec->begin () + iIndexDelete, vec->end ());
	return;
	//刪除小於比對分數的元素
	vector<s_MatchParameter>::iterator it;
	for (it = vec->begin (); it != vec->end ();)
	{
		if (((*it).dMatchScore < dScore))
			it = vec->erase (it);
		else
			++it;
	}
}
void CELCVMatchToolDlg::FilterWithRotatedRect (vector<s_MatchParameter>* vec, int iMethod, double dMaxOverLap)
{
	int iMatchSize = (int)vec->size ();
	RotatedRect rect1, rect2;
	for (int i = 0; i < iMatchSize - 1; i++)
	{
		if (vec->at (i).bDelete)
			continue;
		for (int j = i + 1; j < iMatchSize; j++)
		{
			if (vec->at (j).bDelete)
				continue;
			rect1 = vec->at (i).rectR;
			rect2 = vec->at (j).rectR;
			vector<Point2f> vecInterSec;
			int iInterSecType = rotatedRectangleIntersection (rect1, rect2, vecInterSec);
			if (iInterSecType == INTERSECT_NONE)//無交集
				continue;
			else if (iInterSecType == INTERSECT_FULL) //一個矩形包覆另一個
			{
				int iDeleteIndex;
				if (iMethod == CV_TM_SQDIFF)
					iDeleteIndex = (vec->at (i).dMatchScore <= vec->at (j).dMatchScore) ? j : i;
				else
					iDeleteIndex = (vec->at (i).dMatchScore >= vec->at (j).dMatchScore) ? j : i;
				vec->at (iDeleteIndex).bDelete = TRUE;
			}
			else//交點 > 0
			{
				if (vecInterSec.size () < 3)//一個或兩個交點
					continue;
				else
				{
					int iDeleteIndex;
					//求面積與交疊比例
					SortPtWithCenter (vecInterSec);
					double dArea = contourArea (vecInterSec);
					double dRatio = dArea / rect1.size.area ();
					//若大於最大交疊比例，選分數高的
					if (dRatio > dMaxOverLap)
					{
						if (iMethod == CV_TM_SQDIFF)
							iDeleteIndex = (vec->at (i).dMatchScore <= vec->at (j).dMatchScore) ? j : i;
						else
							iDeleteIndex = (vec->at (i).dMatchScore >= vec->at (j).dMatchScore) ? j : i;
						vec->at (iDeleteIndex).bDelete = TRUE;
					}
				}
			}
		}
	}
	vector<s_MatchParameter>::iterator it;
	for (it = vec->begin (); it != vec->end ();)
	{
		if ((*it).bDelete)
			it = vec->erase (it);
		else
			++it;
	}
}
Point CELCVMatchToolDlg::GetNextMaxLoc (Mat & matResult, Point ptMaxLoc, double dMinValue, int iTemplateW, int iTemplateH, double& dMaxValue, double dMaxOverlap)
{
	//比對到的區域完全不重疊 : +-一個樣板寬高
	//int iStartX = ptMaxLoc.x - iTemplateW;
	//int iStartY = ptMaxLoc.y - iTemplateH;
	//int iEndX = ptMaxLoc.x + iTemplateW;

	//int iEndY = ptMaxLoc.y + iTemplateH;
	////塗黑
	//rectangle (matResult, Rect (iStartX, iStartY, 2 * iTemplateW * (1-dMaxOverlap * 2), 2 * iTemplateH * (1-dMaxOverlap * 2)), Scalar (dMinValue), CV_FILLED);
	////得到下一個最大值
	//Point ptNewMaxLoc;
	//minMaxLoc (matResult, 0, &dMaxValue, 0, &ptNewMaxLoc);
	//return ptNewMaxLoc;

	//比對到的區域需考慮重疊比例
	int iStartX = ptMaxLoc.x - iTemplateW * (1 - dMaxOverlap);
	int iStartY = ptMaxLoc.y - iTemplateH * (1 - dMaxOverlap);
	int iEndX = ptMaxLoc.x + iTemplateW * (1 - dMaxOverlap);

	int iEndY = ptMaxLoc.y + iTemplateH * (1 - dMaxOverlap);
	//塗黑
	rectangle (matResult, Rect (iStartX, iStartY, 2 * iTemplateW * (1-dMaxOverlap), 2 * iTemplateH * (1-dMaxOverlap)), Scalar (dMinValue), CV_FILLED);
	//得到下一個最大值
	Point ptNewMaxLoc;
	minMaxLoc (matResult, 0, &dMaxValue, 0, &ptNewMaxLoc);
	return ptNewMaxLoc;
}
void CELCVMatchToolDlg::SortPtWithCenter (vector<Point2f>& vecSort)
{
	int iSize = (int)vecSort.size ();
	Point2f ptCenter;
	for (int i = 0; i < iSize; i++)
		ptCenter += vecSort[i];
	ptCenter /= iSize;

	Point2f vecX (1, 0);

	vector<pair<Point2f, double>> vecPtAngle (iSize);
	for (int i = 0; i < iSize; i++)
	{
		vecPtAngle[i].first = vecSort[i];//pt
		Point2f vec1 (vecSort[i].x - ptCenter.x, vecSort[i].y - ptCenter.y);
		float fNormVec1 = vec1.x * vec1.x + vec1.y * vec1.y;
		float fDot = vec1.x;

		if (vec1.y < 0)//若點在中心的上方
		{
			vecPtAngle[i].second = acos (fDot / fNormVec1) * R2D;
		}
		else if (vec1.y > 0)//下方
		{
			vecPtAngle[i].second = 360 - acos (fDot / fNormVec1) * R2D;
		}
		else//點與中心在相同Y
		{
			if (vec1.x - ptCenter.x > 0)
				vecPtAngle[i].second = 0;
			else
				vecPtAngle[i].second = 180;
		}

	}
	sort (vecPtAngle.begin (), vecPtAngle.end (), comparePtWithAngle);
	for (int i = 0; i < iSize; i++)
		vecSort[i] = vecPtAngle[i].first;
}
LRESULT CELCVMatchToolDlg::OnShowMSG (WPARAM wMSGPointer, LPARAM lIsShowTime)
{
	CString* pMSG = (CString*)wMSGPointer;
	CString strNum;
	CString strTime;
	time_t timep;
	struct tm p;
	CRect rc;
	int item;
	int index = pMSG->Find (_T ("\n"));

	time (&timep);
	localtime_s (&p, &timep);
	strTime.Format (_T ("%02d/%02d %02d:%02d:%02d"), p.tm_mon + 1, p.tm_mday, p.tm_hour, p.tm_min, p.tm_sec);

	int iRowCount = m_listMsg.GetItemCount ();
	m_iMessageCount++;
	strNum.Format (_T ("%d"), m_iMessageCount);

	m_listMsg.InsertItem (iRowCount, strNum);
	if (index != -1)
	{
		m_listMsg.SetItemText (iRowCount, 1, pMSG->Left (index));
		if (lIsShowTime)
			m_listMsg.SetItemText (iRowCount, 2, strTime);

		m_listMsg.InsertItem (iRowCount + 1, _T (""));
		m_listMsg.SetItemText (iRowCount + 1, 1, pMSG->Right (pMSG->GetLength () - index - 1));
	}
	else
	{
		m_listMsg.SetItemText (iRowCount, 1, *pMSG);
		if (lIsShowTime)
			m_listMsg.SetItemText (iRowCount, 2, strTime);
	}

	//滾輪自動移動最下方
	item = m_listMsg.GetTopIndex ();
	m_listMsg.GetItemRect (item, rc, LVIR_BOUNDS);
	CSize sz (0, (m_iMessageCount - item) * rc.Height ());
	m_listMsg.Scroll (sz);



	return 0;
}

void CELCVMatchToolDlg::LoadSrc ()
{
	CRect rectSrc;
	::GetWindowRect (GetDlgItem (IDC_STATIC_SRC_VIEW)->m_hWnd, rectSrc);
	double dScaleX = rectSrc.Width () / (double)m_matSrc.cols;
	double dScaleY = rectSrc.Height () / (double)m_matSrc.rows;
	m_dSrcScale = min (dScaleX, dScaleY);
	m_dNewScale = m_dSrcScale;
	m_bShowResult = FALSE;
	m_iScaleTimes = 0;
	//防止顯示不同比例圖片時DC殘留
	CWnd* pWnd = GetDlgItem (IDC_STATIC_SRC_VIEW);
	pWnd->GetClientRect (&rectSrc);//得到控件客户端区域坐标
	pWnd->ClientToScreen (rectSrc);//将区域坐标由 控件客户区转成对话框区
	this->ScreenToClient (rectSrc); //将区域坐标由 对话框区转成对话框客户区坐标
	InvalidateRect (rectSrc);
	//防止顯示不同比例圖片時DC殘留
	RefreshSrcView ();
	CString strSize;
	strSize.Format (L"%s : %d X %d", m_strLanSourceImageSize, m_matSrc.cols, m_matSrc.rows);
	m_statusBar.SetPaneText (1, strSize);
	
}

void CELCVMatchToolDlg::LoadDst ()
{
	CRect rectDst;
	::GetWindowRect (GetDlgItem (IDC_STATIC_DST_VIEW)->m_hWnd, rectDst);
	double dScaleX = rectDst.Width () / (double)m_matDst.cols;
	double dScaleY = rectDst.Height () / (double)m_matDst.rows;
	m_dDstScale = min (dScaleX, dScaleY);
	RefreshDstView ();
	CString strSize;
	strSize.Format (L"%s : %d X %d", m_strLanDstImageSize, m_matDst.cols, m_matDst.rows);
	m_statusBar.SetPaneText (2, strSize);
}
double g_dCompensationX = 0;//補償ScrollBar取整數造成的誤差
double g_dCompensationY = 0;
#define BAR_SIZE 100
BOOL CELCVMatchToolDlg::OnMouseWheel (UINT nFlags, short zDelta, CPoint pt)
{
	POINT pointCursor;
	GetCursorPos (&pointCursor);
	ScreenToClient (&pointCursor);
	//check the wheel is forward or backward
	if (zDelta > 0)
	{
		if (m_iScaleTimes == MAX_SCALE_TIMES)
			return TRUE;
		else
			m_iScaleTimes++;
	}
	if (zDelta < 0)
	{
		if (m_iScaleTimes == MIN_SCALE_TIMES)
			return TRUE;
		else
			m_iScaleTimes--;
	}
	CRect rect;
	//GetWindowRect (rect);
	GetDlgItem (IDC_STATIC_SRC_VIEW)->GetWindowRect (rect);//重要

	if (m_iScaleTimes == 0)
		g_dCompensationX = g_dCompensationY = 0;

	int iMouseOffsetX = pt.x - (rect.left + 1);
	int iMouseOffsetY = pt.y - (rect.top + 1);

	//compensationXY is value for slight error in every calculating process
	double dPixelX = (m_hScrollBar.GetScrollPos () + iMouseOffsetX + g_dCompensationX) / m_dNewScale;
	double dPixelY = (m_vScrollBar.GetScrollPos () + iMouseOffsetY + g_dCompensationY) / m_dNewScale;


	m_dNewScale = m_dSrcScale * pow (SCALE_RATIO, m_iScaleTimes);

	if (m_iScaleTimes != 0)
	{
		int iWidth = m_matSrc.cols;
		int iHeight = m_matSrc.rows;

		//every time calculating a new scale, change the horizontal&vertical bars' range
		m_hScrollBar.SetScrollRange (0, int (m_dNewScale * iWidth - m_dSrcScale * iWidth) - 1 + BAR_SIZE);
		m_vScrollBar.SetScrollRange (0, int (m_dNewScale * iHeight - m_dSrcScale * iHeight) - 1 + BAR_SIZE);
		int iBarPosX = int (dPixelX * m_dNewScale - iMouseOffsetX + 0.5);
		m_hScrollBar.SetScrollPos (iBarPosX);
		m_hScrollBar.ShowWindow (SW_SHOW);
		//recorde the slight error, and compensate it next time
		g_dCompensationX = -iBarPosX + (dPixelX * m_dNewScale - iMouseOffsetX);

		int iBarPosY = int (dPixelY * m_dNewScale - iMouseOffsetY + 0.5);
		m_vScrollBar.SetScrollPos (iBarPosY);
		m_vScrollBar.ShowWindow (SW_SHOW);
		g_dCompensationY = -iBarPosY + (dPixelY * m_dNewScale - iMouseOffsetY);

		//scroll bar size
		SCROLLINFO infoH;
		infoH.cbSize = sizeof (SCROLLINFO);
		infoH.fMask = SIF_PAGE;
		infoH.nPage = BAR_SIZE;
		m_hScrollBar.SetScrollInfo (&infoH);

		SCROLLINFO infoV;
		infoV.cbSize = sizeof (SCROLLINFO);
		infoV.fMask = SIF_PAGE;
		infoV.nPage = BAR_SIZE;
		m_vScrollBar.SetScrollInfo (&infoV);
		//scroll bar size

	}
	else
	{
		m_hScrollBar.SetScrollPos (0);
		m_hScrollBar.ShowWindow (SW_HIDE);
		m_vScrollBar.SetScrollPos (0);
		m_vScrollBar.ShowWindow (SW_HIDE);
	}
	//because scale is changed, refresh image
	RefreshSrcView ();
	return CDialogEx::OnMouseWheel (nFlags, zDelta, pt);
}


void CELCVMatchToolDlg::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if (pScrollBar == (CScrollBar*)GetDlgItem (IDC_SCROLLBAR_H) && !m_matSrc.empty ())
	{
		
		BOOL bEnd = FALSE;
		int iWidth = m_matSrc.cols, iHeight = m_matSrc.rows;

		int iCurPos = pScrollBar->GetScrollPos ();
		int iPageLeftRightStep = int (m_dSrcScale * iWidth);
		switch (nSBCode)
		{
		case SB_THUMBPOSITION:
			iCurPos = nPos;
			break;
		case SB_LINEUP:
			iCurPos--;
			break;
		case SB_LINEDOWN:
			iCurPos++;
			break;
		case SB_TOP:
			iCurPos = 0;
			break;
		case SB_BOTTOM:
			iCurPos = pScrollBar->GetScrollLimit ();
			break;
		case SB_THUMBTRACK:
			iCurPos = nPos;
			break;
		case SB_PAGELEFT:
			iCurPos -= iPageLeftRightStep;
			if (iCurPos < 0)
				iCurPos = 0;
			break;
		case SB_PAGERIGHT:
			iCurPos += iPageLeftRightStep;
			if (iCurPos > pScrollBar->GetScrollLimit ())
				iCurPos = pScrollBar->GetScrollLimit ();
			break;
		case SB_ENDSCROLL:
			bEnd = 1;
			break;
		}
		pScrollBar->SetScrollPos (iCurPos);
		//if (bEnd)
			RefreshSrcView ();

	}
	CDialogEx::OnHScroll (nSBCode, nPos, pScrollBar);
}


void CELCVMatchToolDlg::OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if (pScrollBar == (CScrollBar*)GetDlgItem (IDC_SCROLLBAR_V) && !m_matSrc.empty ())
	{
		BOOL bEnd = FALSE;
		int iWidth = m_matSrc.cols, iHeight = m_matSrc.rows;

		int iCurPos = pScrollBar->GetScrollPos ();
		int iPageUpDownStep = int (m_dSrcScale * iHeight);
		switch (nSBCode)
		{
		case SB_THUMBPOSITION:
			iCurPos = nPos;
			break;
		case SB_LINEUP:
			iCurPos--;
			break;
		case SB_LINEDOWN:
			iCurPos++;
			break;
		case SB_TOP:
			iCurPos = 0;
			break;
		case SB_BOTTOM:
			iCurPos = pScrollBar->GetScrollLimit ();
			break;
		case SB_THUMBTRACK:
			iCurPos = nPos;
			break;
		case SB_PAGEDOWN:
			iCurPos += iPageUpDownStep;
			if (iCurPos > pScrollBar->GetScrollLimit ())
				iCurPos = pScrollBar->GetScrollLimit ();
			break;
		case SB_PAGEUP:
			iCurPos -= iPageUpDownStep;
			if (iCurPos < 0)
				iCurPos = 0;
			break;
		case SB_ENDSCROLL:
			bEnd = 1;;
			break;
		}
		pScrollBar->SetScrollPos (iCurPos);
		if (bEnd)
			RefreshSrcView ();
	}
	
	CDialogEx::OnVScroll (nSBCode, nPos, pScrollBar);
}


void CELCVMatchToolDlg::OnSize (UINT nType, int cx, int cy)
{
	CDialogEx::OnSize (nType, cx, cy);

	// TODO: 在此加入您的訊息處理常式程式碼

	if (m_listRect.GetCount () > 0)
	{
		CRect rectDlgNow;
		GetWindowRect (&rectDlgNow);//獲得當前對話框的坐標
		//
		CRect rectState;
		m_statusBar.GetWindowRect (rectState);
		rectDlgNow = CRect (rectDlgNow.TopLeft (), CSize (rectDlgNow.Width (), rectDlgNow.Height () - rectState.Height () * 2));
		//
		POSITION mp = m_listRect.GetHeadPosition ();//取得存儲在鍊錶中的頭元素，其實就是前邊的對話框坐標
		CRect rectDlgSaved;
		rectDlgSaved = m_listRect.GetNext (mp);
		ScreenToClient (rectDlgNow);
		float fRateScaleX = (float)(rectDlgNow.right - rectDlgNow.left) / (rectDlgSaved.right - rectDlgSaved.left);//拖拉後的窗口大小與原來窗口大小的比例
		float fRateScaleY = (float)(rectDlgNow.bottom - rectDlgNow.top) / (rectDlgSaved.bottom - rectDlgSaved.top);
		ClientToScreen (rectDlgNow);
		CRect rectChildSaved;
		CWnd *pWndChild = GetWindow (GW_CHILD);

		while (pWndChild)
		{
			rectChildSaved = m_listRect.GetNext (mp);
			rectChildSaved.left = rectDlgNow.left + (int)((rectChildSaved.left - rectDlgSaved.left) * fRateScaleX);
			rectChildSaved.top = rectDlgNow.top + (int)((rectChildSaved.top - rectDlgSaved.top) * fRateScaleY);


			rectChildSaved.right = rectDlgNow.right + (int)((rectChildSaved.right - rectDlgSaved.right) * fRateScaleX);
			rectChildSaved.bottom = rectDlgNow.bottom + (int)((rectChildSaved.bottom - rectDlgSaved.bottom) * fRateScaleY);
			ScreenToClient (rectChildSaved);
			pWndChild->MoveWindow (rectChildSaved);
			pWndChild = pWndChild->GetNextWindow ();
		}
		//
		for (int i = 0 ; i < 3 ; i++)
		{
			HWND hWnd = i == 0 ? m_hScrollBar.m_hWnd :
				i == 1 ? m_vScrollBar.m_hWnd : m_statusBar.m_hWnd;
			CWnd* pWnd = CWnd::FromHandle (hWnd);
						 
			rectChildSaved = m_listRect.GetNext (mp);
			rectChildSaved.left = rectDlgNow.left + (int)((rectChildSaved.left - rectDlgSaved.left) * fRateScaleX);
			rectChildSaved.top = rectDlgNow.top + (int)((rectChildSaved.top - rectDlgSaved.top) * fRateScaleY);
			rectChildSaved.right = rectDlgNow.right + (int)((rectChildSaved.right - rectDlgSaved.right) * fRateScaleX);
			rectChildSaved.bottom = rectDlgNow.bottom + (int)((rectChildSaved.bottom - rectDlgSaved.bottom) * fRateScaleY);
			ScreenToClient (rectChildSaved);
			pWnd->MoveWindow (rectChildSaved);
			if (i == 2)
			{
				m_statusBar.GetClientRect (&rectState);
				GetClientRect (rectDlgNow);
				m_statusBar.MoveWindow (0, rectDlgNow.bottom - rectState.Height (), rectDlgNow.right, rectState.Height ());
			}
		}
		

	}

	Invalidate (); //強制重繪窗口

}



HBRUSH CELCVMatchToolDlg::OnCtlColor (CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor (pDC, pWnd, nCtlColor);

	// TODO:  在此變更 DC 的任何屬性
	if (pWnd->GetDlgCtrlID () == IDC_STATIC_MAX_POS)
	{
		
	}
	// TODO:  如果預設值並非想要的，則傳回不同的筆刷
	return hbr;
}


void CELCVMatchToolDlg::OnLvnKeydownListMsg (NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	//Ctrl+C+V
	if ((::GetKeyState (VK_CONTROL) & 0x8000) && (pLVKeyDow->wVKey == 'C' || pLVKeyDow->wVKey == 'c'))
	{
		int iItemCount = m_listMsg.GetItemCount ();
		CString strAllText, strText;
		for (int i = 0 ; i < iItemCount ; i++)
		{
			for (int j = 0 ; j < 5 ; j++)
			{
				strText = m_listMsg.GetItemText (i, j);
				strAllText.Append (strText + L"\t");
			}
			strAllText += L"\n";
		}
		if (OpenClipboard ())
		{
			EmptyClipboard ();
			TCHAR* pszData;
			HGLOBAL hClipboardData = GlobalAlloc (GMEM_DDESHARE, (strAllText.GetLength () + 1) * sizeof (TCHAR));
			if (hClipboardData)
			{
				pszData = (TCHAR*)GlobalLock (hClipboardData);
				_tcscpy (pszData, strAllText);
				GlobalUnlock (hClipboardData);
				SetClipboardData (CF_UNICODETEXT, hClipboardData);//根据相应的数据选择第一个参数，（CF_TEXT）  
			}
			CloseClipboard ();
		}
	}
	*pResult = 0;
}
void CELCVMatchToolDlg::OnPaint ()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CDialogEx::OnPaint();
	}

	//呼叫CDialogEx::OnPaint()在視窗拖出畫面時會閃爍 & OpenCV的window會被蓋掉
	//螢幕尺寸
	int iW = GetSystemMetrics (SM_CXFULLSCREEN);
	int iH = GetSystemMetrics (SM_CYFULLSCREEN);

	CRect rect;
	::GetWindowRect (GetDlgItem (IDC_STATIC_SRC_VIEW)->m_hWnd, rect);
	ScreenToClient (rect);
	//將背景繪製上白色網格
	CBrush brush (HS_DIAGCROSS, RGB (200, 200, 200));
	CPaintDC dc (this); // 繪製的裝置內容
	dc.FillRect (CRect (rect.left + 1, rect.top + 1, rect.right, rect.bottom), &brush);


	//Tracker
	//RefreshSrcView ();

	
}
void MouseCall (int event, int x, int y, int flag, void* pUserData)
{
	CELCVMatchToolDlg* pDlg = (CELCVMatchToolDlg*)pUserData;

	if (event == CV_EVENT_MOUSEMOVE)
	{
		int iX = int (x / pDlg->m_dNewScale);
		int iY = int (y / pDlg->m_dNewScale);
		CString strPos;
		strPos.Format (L"%s : %d, %d", pDlg->m_strLanPixelPos, iX, iY);
		pDlg->m_statusBar.SetPaneText (3, strPos);
	}
	//Tracker
	HWND hWnd = (HWND)cvGetWindowHandle ("SrcView");
	CWnd* pWndView = CWnd::FromHandle (hWnd);
	CDC* pDC = pWndView->GetDC ();
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		
	}
	pWndView->ReleaseDC (pDC);
}


void CELCVMatchToolDlg::OnBnClickedButtonChangeToleranceMode ()
{
	m_bToleranceRange = !m_bToleranceRange;

	if (m_bToleranceRange)
	{
		GetDlgItem (IDC_BUTTON_CHANGE_TOLERANCE_MODE)->SetWindowText (L"↑");
		GetDlgItem (IDC_EDIT_TOLERANCE_ANGLE)->EnableWindow (FALSE);
		GetDlgItem (IDC_EDIT_TOLERANCE1)->EnableWindow (TRUE);
		GetDlgItem (IDC_EDIT_TOLERANCE2)->EnableWindow (TRUE);
		GetDlgItem (IDC_EDIT_TOLERANCE3)->EnableWindow (TRUE);
		GetDlgItem (IDC_EDIT_TOLERANCE4)->EnableWindow (TRUE);
	}
	else
	{
		GetDlgItem (IDC_BUTTON_CHANGE_TOLERANCE_MODE)->SetWindowText (L"↓");
		GetDlgItem (IDC_EDIT_TOLERANCE1)->EnableWindow (FALSE);
		GetDlgItem (IDC_EDIT_TOLERANCE2)->EnableWindow (FALSE);
		GetDlgItem (IDC_EDIT_TOLERANCE3)->EnableWindow (FALSE);
		GetDlgItem (IDC_EDIT_TOLERANCE4)->EnableWindow (FALSE);
		GetDlgItem (IDC_EDIT_TOLERANCE_ANGLE)->EnableWindow (TRUE);

	}
	
}




void CELCVMatchToolDlg::OnCbnSelchangeComboLan ()
{
	int iSel = m_cbLanSelect.GetCurSel ();
	CString strLan;
	m_cbLanSelect.GetLBText (iSel, strLan);
	ChangeLanguage (strLan);
}
