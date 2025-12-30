#include "pch.h"
#include "CSelectSoftwareDlg.h"
#include <sql.h>
#include <sqlext.h>

IMPLEMENT_DYNAMIC(CSelectSoftwareDlg, CDialogEx)

CSelectSoftwareDlg::CSelectSoftwareDlg(CWnd* pParent)
    : CDialogEx(IDD_SELECT_SOFTWARE, pParent), m_selectedSoftwareId(-1) {
}

CSelectSoftwareDlg::~CSelectSoftwareDlg() {}

void CSelectSoftwareDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_SELECT_SW, m_listSelect);
}

BEGIN_MESSAGE_MAP(CSelectSoftwareDlg, CDialogEx)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_SELECT_SW, &CSelectSoftwareDlg::OnDblClkList)
END_MESSAGE_MAP()

BOOL CSelectSoftwareDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_fontCustom.DeleteObject();
    m_fontCustom.CreateFont(22, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Segoe UI"));

    SetFont(&m_fontCustom);

    CWnd* pChild = GetWindow(GW_CHILD);
    while (pChild)
    {
        pChild->SendMessage(WM_SETFONT, (WPARAM)m_fontCustom.GetSafeHandle(), MAKELPARAM(TRUE, 0));
        pChild = pChild->GetWindow(GW_HWNDNEXT);
    }

    m_listSelect.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_listSelect.InsertColumn(0, _T("Название"), LVCFMT_LEFT, 250);

    CHeaderCtrl* pHeader = m_listSelect.GetHeaderCtrl();
    if (pHeader) {
        pHeader->SendMessage(WM_SETFONT, (WPARAM)m_fontCustom.GetSafeHandle(), MAKELPARAM(TRUE, 0));
    }

    SQLHENV hEnv = SQL_NULL_HENV;
    SQLHDBC hDbc = SQL_NULL_HDBC;
    SQLHSTMT hStmt = SQL_NULL_HSTMT;

    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) == SQL_SUCCESS) {
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

        if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) == SQL_SUCCESS) {
            CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

            if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {

                if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {
                    if (SQLExecDirectW(hStmt, L"SELECT ID, NAME FROM SOFTWARE", SQL_NTS) == SQL_SUCCESS) {
                        int i = 0;
                        while (SQLFetch(hStmt) == SQL_SUCCESS) {
                            long id;
                            WCHAR name[255];
                            SQLLEN cbName;

                            SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
                            SQLGetData(hStmt, 2, SQL_C_WCHAR, name, sizeof(name), &cbName);

                            int idx = m_listSelect.InsertItem(i, name);
                            m_listSelect.SetItemData(idx, (DWORD_PTR)id);
                            i++;
                        }
                    }
                    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                }
                SQLDisconnect(hDbc);
            }
            SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        }
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    }

    m_listSelect.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

    return TRUE;
}

void CSelectSoftwareDlg::OnDblClkList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pItem = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    if (pItem->iItem != -1) {
        m_selectedSoftwareId = (long)m_listSelect.GetItemData(pItem->iItem);
        EndDialog(IDOK); 
    }
    *pResult = 0;
}