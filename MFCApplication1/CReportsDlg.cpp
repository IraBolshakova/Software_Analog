#include "pch.h"
#include "MFCApplication1.h"
#include "CReportsDlg.h"
#include "afxdialogex.h"
#include <sql.h>
#include <sqlext.h>

IMPLEMENT_DYNAMIC(CReportsDlg, CDialogEx)

CReportsDlg::CReportsDlg(CWnd* pParent)
    : CDialogEx(IDD_REPORTS_DIALOG, pParent)
{
}

CReportsDlg::~CReportsDlg()
{
    m_fontCustom.DeleteObject();
}

void CReportsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_REPORTS, m_listReports);
    DDX_Control(pDX, IDC_COMBO_REPORT_TYPE, m_comboTypes);
}

BEGIN_MESSAGE_MAP(CReportsDlg, CDialogEx)
    ON_CBN_SELCHANGE(IDC_COMBO_REPORT_TYPE, &CReportsDlg::OnCbnSelchangeComboReportType)
END_MESSAGE_MAP()

BOOL CReportsDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_fontCustom.CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Segoe UI"));

    SetFont(&m_fontCustom);

    if (!m_imgList.GetSafeHandle()) {
        m_imgList.Create(1, 190, ILC_COLOR, 1, 1); 
        m_listReports.SetImageList(&m_imgList, LVSIL_SMALL);
    }

    m_listReports.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_listReports.InsertColumn(0, _T("Параметр"), LVCFMT_LEFT, 200);
    m_listReports.InsertColumn(1, _T("Список ПО"), LVCFMT_LEFT, 240);

    m_comboTypes.SetFont(&m_fontCustom);
    m_listReports.SetFont(&m_fontCustom);

    m_comboTypes.AddString(_T("Программы по категориям"));
    m_comboTypes.AddString(_T("Программы по разработчикам"));
    m_comboTypes.AddString(_T("Программы по типу лицензии"));
    m_comboTypes.SetCurSel(0);

    OnCbnSelchangeComboReportType();

    return TRUE;
}

void CReportsDlg::OnCbnSelchangeComboReportType()
{
    int sel = m_comboTypes.GetCurSel();
    CString query;

    CString linebreak = _T("ASCII_CHAR(13) || ASCII_CHAR(10)");

    switch (sel) {
    case 0: 
        query.Format(_T("SELECT C.NAME, LIST(S.NAME, %s) FROM SOFTWARE S ")
            _T("JOIN CATEGORY C ON S.CATEGORY_ID = C.ID GROUP BY C.NAME"), linebreak);
        break;

    case 1: 
        query.Format(_T("SELECT D.NAME, LIST(S.NAME, %s) FROM SOFTWARE S ")
            _T("JOIN DEVELOPER D ON S.DEVELOPER_ID = D.ID GROUP BY D.NAME"), linebreak);
        break;

    case 2: 
    query.Format(_T("SELECT DISTINCT TRIM(LICENSE_TYPE), LIST(NAME, %s) FROM SOFTWARE GROUP BY 1"), linebreak);
    break;

    default: return;
    }

    ExecuteReport(query);
}

void CReportsDlg::ExecuteReport(CString sql)
{
    m_listReports.DeleteAllItems();

    SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;
    try {
        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

        CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

        if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
            SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
            CStringW queryW(sql);

            if (SQLExecDirectW(hStmt, (SQLWCHAR*)queryW.GetString(), SQL_NTS) >= 0) {
                int i = 0;
                while (SQLFetch(hStmt) == SQL_SUCCESS) {
                    WCHAR attrName[256] = { 0 };
                    WCHAR softList[4096] = { 0 }; // Большой буфер для списка названий
                    SQLLEN lName = 0, lList = 0;

                    SQLGetData(hStmt, 1, SQL_C_WCHAR, attrName, sizeof(attrName), &lName);
                    SQLGetData(hStmt, 2, SQL_C_WCHAR, softList, sizeof(softList), &lList);

                    CString strName = (lName != SQL_NULL_DATA) ? attrName : _T("-");
                    CString strSoft = (lList != SQL_NULL_DATA) ? softList : _T("Данные отсутствуют");

                    int nIndex = m_listReports.InsertItem(i, strName);
                    m_listReports.SetItemText(nIndex, 1, strSoft);
                    i++;
                }
            }
        }
    }
    catch (...) {
        AfxMessageBox(_T("Критическая ошибка при работе с базой данных"));
    }

    if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    if (hDbc) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
    if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}