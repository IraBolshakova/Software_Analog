#include "pch.h" 
#include "MFCApplication1.h"
#include "CEditReviewDlg.h"
#include "afxdialogex.h"
#include <sql.h>
#include <sqlext.h>

IMPLEMENT_DYNAMIC(CEditReviewDlg, CDialogEx)

CEditReviewDlg::CEditReviewDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_EDIT_REVIEW_DIALOG, pParent)
{
    m_reviewId = 0;
    m_nRating = 5;
    m_strSoftName = _T("");
    m_strReviewText = _T("");
    m_strUserId = _T("");
}

CEditReviewDlg::~CEditReviewDlg()
{
}

void CEditReviewDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_REVIEW_EDIT, m_comboRating);
    DDX_Control(pDX, IDC_EDIT_TEXT_REVIEW, m_editReview);
}

BEGIN_MESSAGE_MAP(CEditReviewDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CEditReviewDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CEditReviewDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_fontCustom.DeleteObject();
    m_fontCustom.CreateFont(22, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Segoe UI"));

    m_comboRating.ResetContent();
    for (int i = 1; i <= 5; i++) {
        CString str;
        str.Format(_T("%d"), i);
        m_comboRating.AddString(str);
    }

    // Установка текущих значений
    m_comboRating.SetCurSel(m_nRating - 1);     // Выбор текущей оценки
    m_editReview.SetWindowText(m_strReviewText); // Текст отзыва

    SetFont(&m_fontCustom);

    CWnd* pChild = GetWindow(GW_CHILD);
    while (pChild)
    {
        pChild->SendMessage(WM_SETFONT, (WPARAM)m_fontCustom.GetSafeHandle(), MAKELPARAM(TRUE, 0));
        pChild = pChild->GetWindow(GW_HWNDNEXT);
    }

    return TRUE;
}

void CEditReviewDlg::OnBnClickedOk()
{
    m_editReview.GetWindowText(m_strReviewText);
    m_nRating = m_comboRating.GetCurSel() + 1;
    m_strReviewText.Replace(_T("'"), _T("''"));

    SQLHENV hEnv = SQL_NULL_HENV;
    SQLHDBC hDbc = SQL_NULL_HDBC;
    SQLHSTMT hStmt = SQL_NULL_HSTMT;

    CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) == SQL_SUCCESS) {
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

        if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) == SQL_SUCCESS) {
            if (SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {

                if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {

                    CStringW query;
                    query.Format(L"EXECUTE PROCEDURE UPDATE_REVIEW(%ld, %d, %d, '%s')",
                        m_reviewId,
                        _ttoi(m_strUserId),
                        m_nRating,
                        (const wchar_t*)m_strReviewText);

                    if (SQLExecDirectW(hStmt, (SQLWCHAR*)query.GetString(), SQL_NTS) == SQL_SUCCESS) {
                        SQLEndTran(SQL_HANDLE_DBC, hDbc, SQL_COMMIT); 
                        AfxMessageBox(_T("Отзыв успешно обновлен!"));
                        CDialogEx::OnOK(); 
                    }
                    else {
                        SQLWCHAR sqlState[6], msg[512];
                        SQLINTEGER nativeError;
                        SQLSMALLINT msgLen;
                        SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError, msg, 512, &msgLen);

                        CString errorReport;
                        errorReport.Format(_T("Ошибка обновления в БД: %s"), (CString)msg);
                        AfxMessageBox(errorReport);
                    }

                    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                }
                SQLDisconnect(hDbc);
            }
            else {
                AfxMessageBox(_T("Ошибка соединения с базой данных для редактирования!"));
            }
            SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        }
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    }
}