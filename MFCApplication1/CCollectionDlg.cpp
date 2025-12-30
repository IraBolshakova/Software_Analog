#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "CCollectionDlg.h"
#include "CSelectSoftwareDlg.h"
#include "afxdialogex.h"
#include <sql.h>
#include <sqlext.h>

IMPLEMENT_DYNAMIC(CCollectionDlg, CDialogEx)

CCollectionDlg::CCollectionDlg(long id, CWnd* pParent)
    : CDialogEx(IDD_COLLECTION, pParent), m_collectionId(id)
{
    m_id = id;
}

CCollectionDlg::~CCollectionDlg()
{
}

void CCollectionDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_COLLECTION_CONTENT, m_listItems);
}

BEGIN_MESSAGE_MAP(CCollectionDlg, CDialogEx)
    ON_BN_CLICKED(1050, &CCollectionDlg::OnAddSoftware)
    ON_BN_CLICKED(IDC_BUTTON_DELETE, &CCollectionDlg::OnBnClickedButtonDelete)
END_MESSAGE_MAP()

BOOL CCollectionDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetWindowText(_T("Состав выбранной подборки"));

    m_fontCustom.DeleteObject();
    m_fontCustom.CreateFont(22, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Segoe UI"));

    m_listItems.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_listItems.InsertColumn(0, _T("Программное обеспечение"), LVCFMT_LEFT, 230);
    m_listItems.InsertColumn(1, _T("Разработчик"), LVCFMT_LEFT, 250);

    SetFont(&m_fontCustom);

    CWnd* pChild = GetWindow(GW_CHILD);
    while (pChild)
    {
        pChild->SendMessage(WM_SETFONT, (WPARAM)m_fontCustom.GetSafeHandle(), MAKELPARAM(TRUE, 0));
        pChild = pChild->GetWindow(GW_HWNDNEXT);
    }

    CHeaderCtrl* pHeader = m_listItems.GetHeaderCtrl();
    if (pHeader)
    {
        pHeader->SendMessage(WM_SETFONT, (WPARAM)m_fontCustom.GetSafeHandle(), MAKELPARAM(TRUE, 0));
    }

    LoadData();

    return TRUE;
}

void CCollectionDlg::LoadData()
{
    m_listItems.DeleteAllItems();

    SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;

    try {
        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

        CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

        if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
            SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

            CStringW query;
            query.Format(L"SELECT S.ID, S.NAME, D.NAME "
                L"FROM SOFTWARE S "
                L"JOIN COLLECTIONITEM CI ON S.ID = CI.SOFTWARE_ID "
                L"JOIN DEVELOPER D ON S.DEVELOPER_ID = D.ID "
                L"WHERE CI.COLLECTION_ID = %ld", m_collectionId);

            SQLRETURN ret = SQLExecDirectW(hStmt, (SQLWCHAR*)query.GetString(), SQL_NTS);

            if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
                int i = 0;
                while (SQLFetch(hStmt) == SQL_SUCCESS) {
                    long softId = 0;
                    WCHAR sName[256] = { 0 }, dName[256] = { 0 };
                    SQLLEN lID, l1, l2;

                    SQLGetData(hStmt, 1, SQL_C_LONG, &softId, 0, &lID);
                    SQLGetData(hStmt, 2, SQL_C_WCHAR, sName, sizeof(sName), &l1);
                    SQLGetData(hStmt, 3, SQL_C_WCHAR, dName, sizeof(dName), &l2);

                    int row = m_listItems.InsertItem(i, (l1 != SQL_NULL_DATA) ? sName : L"Без названия");
                    m_listItems.SetItemText(row, 1, (l2 != SQL_NULL_DATA) ? dName : L"Не указан");

                    m_listItems.SetItemData(row, (DWORD_PTR)softId);

                    i++;
                }
            }
            else {
                SQLWCHAR sqlState[6], msg[1024];
                SQLINTEGER nativeError; SQLSMALLINT msgLen;
                SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError, msg, 1024, &msgLen);
                AfxMessageBox(CString(L"Ошибка SQL: ") + msg);
            }
        }
    }
    catch (...) {
        AfxMessageBox(_T("Критическая ошибка при загрузке данных"));
    }

    if (m_listItems.GetItemCount() == 0) {
        m_listItems.InsertItem(0, _T("В этой подборке пусто"));
    }

    if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    if (hDbc) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
    if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

void CCollectionDlg::OnAddSoftware()
{
    CSelectSoftwareDlg selectDlg(this);
    if (selectDlg.DoModal() == IDOK) {
        long swId = selectDlg.m_selectedSoftwareId;
        if (swId <= 0) return;

        SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;
        try {
            SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
            SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
            SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

            CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

            SQLRETURN ret = SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);

            if (SQL_SUCCEEDED(ret)) {
                SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

                CStringW query;
                query.Format(L"INSERT INTO COLLECTIONITEM (COLLECTION_ID, SOFTWARE_ID) VALUES (%ld, %ld)", m_collectionId, swId);

                ret = SQLExecDirectW(hStmt, (SQLWCHAR*)query.GetString(), SQL_NTS);

                if (SQL_SUCCEEDED(ret)) {
                    AfxMessageBox(_T("Программа успешно добавлена!"));
                    LoadData(); 
                }
                else {
                    AfxMessageBox(_T("Эта программа уже добавлена в текущую подборку!"));
                }
            }
        }
        catch (...) {
            AfxMessageBox(_T("Критическая ошибка при связи с БД"));
        }

        if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        if (hDbc) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
        if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    }
}

void CCollectionDlg::OnBnClickedButtonDelete()
{
    int nItem = m_listItems.GetNextItem(-1, LVNI_SELECTED);
    if (nItem == -1) {
        AfxMessageBox(_T("Выберите программу для удаления."));
        return;
    }

    long softId = (long)m_listItems.GetItemData(nItem);
    long collId = this->m_id;

    if (AfxMessageBox(_T("Удалить программу из подборки?"), MB_YESNO | MB_ICONQUESTION) != IDYES) {
        return;
    }

    SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;
    try {
        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

        CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

        if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
            SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

            CStringW query;
            query.Format(L"EXECUTE PROCEDURE DELETE_ITEM_FROM_COLLECTION(%ld, %ld)", collId, softId);

            if (SQLExecDirectW(hStmt, (SQLWCHAR*)query.GetString(), SQL_NTS) >= 0) {
                // ВИЗУАЛЬНОЕ УДАЛЕНИЕ ИЗ СПИСКА
                m_listItems.DeleteItem(nItem);

                AfxMessageBox(_T("Программа удалена из подборки."));
            }
            else {
                SQLWCHAR sqlState[6], msg[1024];
                SQLINTEGER nativeError; SQLSMALLINT msgLen;
                SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError, msg, 1024, &msgLen);
                AfxMessageBox(CString(L"Ошибка БД: ") + msg);
            }
        }
    }
    catch (...) {
        AfxMessageBox(_T("Ошибка связи с БД."));
    }

    if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    if (hDbc) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
    if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    LoadData();
}

