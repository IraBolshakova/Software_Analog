#include "pch.h"
#include "framework.h"
#include <vector>
#include "MFCApplication1.h"
#include "CSoftwareDetailDlg.h"
#include "afxdialogex.h"
#include <sql.h>
#include <sqlext.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "odbc32.lib")

#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

IMPLEMENT_DYNAMIC(CSoftwareDetailDlg, CDialogEx)

CSoftwareDetailDlg::CSoftwareDetailDlg(long softwareId, CWnd* pParent)
	: CDialogEx(IDD_SOFTWARE_DETAIL, pParent), m_softwareId(softwareId)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pImgAnchor = nullptr;
}

CSoftwareDetailDlg::~CSoftwareDetailDlg()
{
	if (m_pImgAnchor) {
		m_pImgAnchor->DestroyWindow();
		delete m_pImgAnchor;
		m_pImgAnchor = nullptr;
	}

	if (gdiplusToken) GdiplusShutdown(gdiplusToken);
}

void CSoftwareDetailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSoftwareDetailDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

BOOL CSoftwareDetailDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 1. Инициализация графики (GDI+)
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	SetWindowText(_T("Подробная информация о программном обеспечении"));

	m_fontCustom.CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Segoe UI"));

	// 2. Загрузка данных
	LoadSoftwareDetails();
	LoadScreenshot();

	// 3. Создаем элемент-якорь
	if (m_pImgAnchor == nullptr) {
		m_pImgAnchor = new CStatic();
	}

	if (!::IsWindow(m_pImgAnchor->m_hWnd)) {
		m_pImgAnchor->Create(_T(""), WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
			CRect(20, 20, 300, 220), this, 2001);
	}

	int yRight = 20;
	int yBottom = 240;

	auto CreateRow = [&](const CString& labelText, const CString& valueText, bool isRightSide) {
		if (valueText.IsEmpty() || valueText == _T("0") || valueText == _T("0 МБ") || valueText == _T("0 ГБ")) return;

		int currentY = isRightSide ? yRight : yBottom;
		int labelX = isRightSide ? 320 : 20;
		int editX = isRightSide ? 450 : 190;
		int editWidth = isRightSide ? 750 : 750;

		CStatic* pLabel = new CStatic();
		pLabel->Create(labelText + _T(":"), WS_CHILD | WS_VISIBLE,
			CRect(labelX, currentY, labelX + 120, currentY + 25), this);
		pLabel->SetFont(&m_fontCustom);

		CEdit* pEdit = new CEdit();
		pEdit->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL,
			CRect(editX, currentY, editWidth, currentY + 25), this, 0);
		pEdit->SetFont(&m_fontCustom);
		pEdit->SetWindowText(valueText);

		if (isRightSide) yRight += 35;
		else yBottom += 35;
		};

	CreateRow(_T("Название"), m_name, true);
	CreateRow(_T("Категория"), m_category, true);
	CreateRow(_T("Разработчик"), m_developer, true);
	CreateRow(_T("Лицензия"), m_license, true);
	CreateRow(_T("Объём"), m_size, true);
	CreateRow(_T("Описание"), m_description, true);

	if (yRight > yBottom) yBottom = yRight + 10;

	CreateRow(_T("Сайт"), m_website, false);
	CreateRow(_T("ОС"), m_sysreq_os, false);
	CreateRow(_T("ОЗУ"), m_sysreq_ram, false);
	CreateRow(_T("Процессор"), m_sysreq_cpu, false);
	CreateRow(_T("Видеокарта"), m_sysreq_gpu, false);
	CreateRow(_T("Место на диске"), m_sysreq_disk, false);

	Invalidate(); 
	UpdateWindow();

	SetWindowPos(nullptr, 0, 0, 800, yBottom + 60, SWP_NOMOVE | SWP_NOZORDER);

	return TRUE;
}

void CSoftwareDetailDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CSoftwareDetailDlg::LoadScreenshot()
{
	m_imgBuffer.clear();
	SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;
	try {
		SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
		SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
		SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
		CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");
		if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
			SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
			CStringW query;
			query.Format(L"SELECT IMAGE FROM SCREENSHOT WHERE SOFTWARE_ID = %ld", m_softwareId);
			if (SQLExecDirectW(hStmt, (SQLWCHAR*)query.GetString(), SQL_NTS) == SQL_SUCCESS) {
				if (SQLFetch(hStmt) == SQL_SUCCESS) {
					BYTE chunk[4096];
					SQLLEN cbData;
					while (true) {
						SQLRETURN ret = SQLGetData(hStmt, 1, SQL_C_BINARY, chunk, sizeof(chunk), &cbData);
						if (ret == SQL_NO_DATA || cbData == SQL_NULL_DATA) break;
						SQLLEN readSize = (cbData == -4 || cbData > (SQLLEN)sizeof(chunk)) ? (SQLLEN)sizeof(chunk) : cbData;
						if (readSize > 0) m_imgBuffer.insert(m_imgBuffer.end(), chunk, chunk + readSize);
						else break;
					}
				}
			}
		}
	}
	catch (...) {}
	if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	if (hDbc) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
	if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

void CSoftwareDetailDlg::LoadSoftwareDetails()
{
	SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;
	try {
		SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
		SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
		SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
		CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");
		if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
			SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
			CString query;
			query.Format(_T("SELECT * FROM GET_SOFTWARE_DETAILS(%ld)"), m_softwareId);
			if (SQLExecDirect(hStmt, (SQLTCHAR*)query.GetString(), SQL_NTS) >= 0) {
				if (SQLFetch(hStmt) == SQL_SUCCESS) {
					WCHAR name[256], desc[1024], lic[100], cat[100], dev[100], web[256], os[100], cpu[100], gpu[100];
					SQLINTEGER size = 0, ram = 0, disk = 0;
					SQLLEN lName, lDesc, lLic, lSize, lCat, lDev, lWeb, lOs, lRam, lCpu, lGpu, lDisk;

					SQLGetData(hStmt, 1, SQL_C_WCHAR, name, sizeof(name), &lName);
					SQLGetData(hStmt, 2, SQL_C_WCHAR, desc, sizeof(desc), &lDesc);
					SQLGetData(hStmt, 3, SQL_C_WCHAR, lic, sizeof(lic), &lLic);
					SQLGetData(hStmt, 4, SQL_C_LONG, &size, 0, &lSize);
					SQLGetData(hStmt, 5, SQL_C_WCHAR, cat, sizeof(cat), &lCat);
					SQLGetData(hStmt, 6, SQL_C_WCHAR, dev, sizeof(dev), &lDev);
					SQLGetData(hStmt, 7, SQL_C_WCHAR, web, sizeof(web), &lWeb);
					SQLGetData(hStmt, 8, SQL_C_WCHAR, os, sizeof(os), &lOs);
					SQLGetData(hStmt, 9, SQL_C_LONG, &ram, 0, &lRam);
					SQLGetData(hStmt, 10, SQL_C_WCHAR, cpu, sizeof(cpu), &lCpu);
					SQLGetData(hStmt, 11, SQL_C_WCHAR, gpu, sizeof(gpu), &lGpu);
					SQLGetData(hStmt, 12, SQL_C_LONG, &disk, 0, &lDisk);

					m_name = (lName != SQL_NULL_DATA) ? name : _T("");
					m_description = (lDesc != SQL_NULL_DATA) ? desc : _T("");
					m_license = (lLic != SQL_NULL_DATA) ? lic : _T("");
					m_size.Format(_T("%d МБ"), size);
					m_category = (lCat != SQL_NULL_DATA) ? cat : _T("");
					m_developer = (lDev != SQL_NULL_DATA) ? dev : _T("");
					m_website = (lWeb != SQL_NULL_DATA) ? web : _T("");
					m_sysreq_os = (lOs != SQL_NULL_DATA) ? os : _T("");
					m_sysreq_ram.Format(_T("%d МБ"), ram);
					m_sysreq_cpu = (lCpu != SQL_NULL_DATA) ? cpu : _T("");
					m_sysreq_gpu = (lGpu != SQL_NULL_DATA) ? gpu : _T("");
					m_sysreq_disk.Format(_T("%d ГБ"), disk);
				}
			}
		}
	}
	catch (...) {}
	if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	if (hDbc) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
	if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

HCURSOR CSoftwareDetailDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSoftwareDetailDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == 2001 && !m_imgBuffer.empty())
	{
		Graphics graphics(lpDrawItemStruct->hDC);
		CRect rect = lpDrawItemStruct->rcItem;

		SolidBrush backBrush(Color(255, 240, 240, 240));
		graphics.FillRectangle(&backBrush, 0, 0, rect.Width(), rect.Height());

		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, m_imgBuffer.size());
		if (hGlobal)
		{
			void* pData = GlobalLock(hGlobal);
			memcpy(pData, m_imgBuffer.data(), m_imgBuffer.size());
			GlobalUnlock(hGlobal);

			IStream* pStream = NULL;
			if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
			{
				Image* pImg = Image::FromStream(pStream);
				if (pImg && pImg->GetLastStatus() == Ok)
				{
					float imgW = (float)pImg->GetWidth();
					float imgH = (float)pImg->GetHeight();
					float boxW = (float)rect.Width();
					float boxH = (float)rect.Height();

					float scale = min(boxW / imgW, boxH / imgH);

					float newW = imgW * scale;
					float newH = imgH * scale;

					float posX = (boxW - newW) / 2.0f;
					float posY = (boxH - newH) / 2.0f;

					graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
					graphics.DrawImage(pImg, posX, posY, newW, newH);

					delete pImg;
				}
				pStream->Release();
			}
		}
		return;
	}
	CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}