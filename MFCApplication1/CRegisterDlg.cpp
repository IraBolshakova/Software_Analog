#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "CRegisterDlg.h"
#include "afxdialogex.h"
#include <wincrypt.h>
#include <sql.h>
#include <sqlext.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "odbc32.lib")

IMPLEMENT_DYNAMIC(CRegisterDlg, CDialogEx)

CRegisterDlg::CRegisterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REGISTER_DIALOG, pParent)
{
	// Инициализация указателей в конструкторе
	m_pEditName = nullptr;
	m_pEditEmail = nullptr;
	m_pEditLogin = nullptr;
	m_pEditPassword = nullptr;
	m_pButtonRegister = nullptr;
	m_pStaticName = nullptr;
	m_pStaticEmail = nullptr;
	m_pStaticLogin = nullptr;
	m_pStaticPassword = nullptr;
}

CRegisterDlg::~CRegisterDlg()
{
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRegisterDlg, CDialogEx)
	ON_BN_CLICKED(1001, &CRegisterDlg::OnRegisterButton)
END_MESSAGE_MAP()

BOOL CRegisterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(_T("Регистрация нового пользователя"));
	SetWindowPos(nullptr, 0, 0, 400, 280, SWP_NOMOVE | SWP_NOZORDER); 

	m_fontCustom.DeleteObject();
	m_fontCustom.CreateFont(22, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Segoe UI"));

	m_pStaticName = new CStatic();
	m_pStaticName->Create(_T("Имя:"), WS_CHILD | WS_VISIBLE, CRect(20, 20, 100, 45), this);

	m_pEditName = new CEdit();
	m_pEditName->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(110, 20, 350, 45), this, 2001);

	m_pStaticEmail = new CStatic();
	m_pStaticEmail->Create(_T("Email:"), WS_CHILD | WS_VISIBLE, CRect(20, 60, 100, 85), this);

	m_pEditEmail = new CEdit();
	m_pEditEmail->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(110, 60, 350, 85), this, 2002);

	m_pStaticLogin = new CStatic();
	m_pStaticLogin->Create(_T("Логин:"), WS_CHILD | WS_VISIBLE, CRect(20, 100, 100, 125), this);

	m_pEditLogin = new CEdit();
	m_pEditLogin->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(110, 100, 350, 125), this, 2003);

	m_pStaticPassword = new CStatic();
	m_pStaticPassword->Create(_T("Пароль:"), WS_CHILD | WS_VISIBLE, CRect(20, 140, 100, 165), this);

	m_pEditPassword = new CEdit();
	m_pEditPassword->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD, CRect(110, 140, 350, 165), this, 2004);

	m_pButtonRegister = new CButton();
	m_pButtonRegister->Create(_T("Создать аккаунт"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(110, 190, 280, 225), this, 1001);

	SetFont(&m_fontCustom);

	CWnd* pChild = GetWindow(GW_CHILD);
	while (pChild)
	{
		pChild->SendMessage(WM_SETFONT, (WPARAM)m_fontCustom.GetSafeHandle(), MAKELPARAM(TRUE, 0));
		pChild = pChild->GetWindow(GW_HWNDNEXT);
	}

	return TRUE;
}

// Хеширование пароля (SHA-256)
CString CRegisterDlg::HashPassword(const CString& password)
{
	if (password.IsEmpty()) return _T("");
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	BYTE hash[32];
	DWORD hashLen = sizeof(hash);
	CString hashStr;

	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
		return _T("");

	if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
		CryptReleaseContext(hProv, 0);
		return _T("");
	}

	CStringA pwdAnsi(password);
	CryptHashData(hHash, (BYTE*)(LPCSTR)pwdAnsi, (DWORD)pwdAnsi.GetLength(), 0);

	if (CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashLen, 0)) {
		for (DWORD i = 0; i < hashLen; ++i) {
			CString hex;
			hex.Format(_T("%02X"), hash[i]);
			hashStr += hex;
		}
	}

	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	return hashStr;
}

void CRegisterDlg::OnRegisterButton()
{
    CString n, e, l, p;
    m_pEditName->GetWindowText(n);
    m_pEditEmail->GetWindowText(e);
    m_pEditLogin->GetWindowText(l);
    m_pEditPassword->GetWindowText(p);

    if (n.IsEmpty() || e.IsEmpty() || l.IsEmpty() || p.IsEmpty()) {
        AfxMessageBox(_T("Заполните все поля!")); return;
    }

    int atPos = e.Find(_T('@'));
    int dotPos = e.ReverseFind(_T('.'));

    if (atPos == -1 || dotPos == -1) {
        AfxMessageBox(_T("Email должен содержать символ @ и точку!"));
        return;
    }

    if (atPos == 0) {
        AfxMessageBox(_T("Введите имя пользователя перед символом @"));
        return;
    }

    if (dotPos <= atPos + 1) {
        AfxMessageBox(_T("После @ должен быть указан почтовый сервис (например, gmail или mail) перед точкой!"));
        m_pEditEmail->SetFocus();
        return;
    }

    for (int i = 0; i < e.GetLength(); i++) {
        TCHAR c = e[i];
        if ((c >= 0x0400 && c <= 0x04FF) || c == 0x0405 || c == 0x0451) {
            AfxMessageBox(_T("Ошибка: Email должен содержать только латинские буквы (a-z)!"));
            m_pEditEmail->SetFocus();
            return;
        }
    }

    if (dotPos == e.GetLength() - 1) {
        AfxMessageBox(_T("После точки должно быть расширение (например, .ru или .com)"));
        return;
    }

    if (p.GetLength() < 5) {
        AfxMessageBox(_T("Пароль слишком короткий! Минимум 5 символов."));
        m_pEditPassword->SetFocus();
        return;
    }

    bool hasLetter = false;
    bool hasDigit = false;

    for (int i = 0; i < p.GetLength(); i++) {
        TCHAR c = p[i];
        if (iswalpha(c)) hasLetter = true; // Проверка на букву (любого языка)
        if (iswdigit(c)) hasDigit = true; // Проверка на цифру
    }

    if (!hasLetter || !hasDigit) {
        AfxMessageBox(_T("Пароль должен содержать и буквы, и цифры!"));
        m_pEditPassword->SetFocus();
        return;
    }

    CString hPass = HashPassword(p);
    SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;
    bool bSuccess = false;

    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) >= 0) {
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) >= 0) {

            CString conn = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

            if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)conn.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {

                SQLSetConnectAttr(hDbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);

                if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) >= 0) {

                    CString sqlCheck;
                    sqlCheck.Format(_T("SELECT COUNT(*) FROM CREDENTIALS WHERE LOGIN = '%s'"), l);
                    long userCount = 0;
                    if (SQLExecDirect(hStmt, (SQLTCHAR*)sqlCheck.GetString(), SQL_NTS) >= 0) {
                        if (SQLFetch(hStmt) == SQL_SUCCESS) SQLGetData(hStmt, 1, SQL_C_LONG, &userCount, 0, NULL);
                    }
                    SQLCloseCursor(hStmt);

                    if (userCount > 0) {
                        AfxMessageBox(_T("Логин занят!"));
                    }
                    else {
                        long newId = 0;
                        if (SQLExecDirect(hStmt, (SQLTCHAR*)_T("SELECT NEXT VALUE FOR GEN_USER_ID FROM RDB$DATABASE"), SQL_NTS) >= 0) {
                            if (SQLFetch(hStmt) == SQL_SUCCESS) SQLGetData(hStmt, 1, SQL_C_LONG, &newId, 0, NULL);
                        }
                        SQLCloseCursor(hStmt);

                        if (newId > 0) {
                            CString sqlUser;
                            sqlUser.Format(_T("INSERT INTO \"User\" (ID, NAME, EMAIL, ROLE) VALUES (%ld, '%s', '%s', 'user')"), newId, n, e);

                            if (SQLExecDirect(hStmt, (SQLTCHAR*)sqlUser.GetString(), SQL_NTS) >= 0) {
                                SQLCloseCursor(hStmt);

                                CString sqlCred;
                                sqlCred.Format(_T("INSERT INTO CREDENTIALS (USER_ID, LOGIN, PASSWORD_HASH) VALUES (%ld, '%s', '%s')"), newId, l, hPass);

                                if (SQLExecDirect(hStmt, (SQLTCHAR*)sqlCred.GetString(), SQL_NTS) >= 0) {
                                    SQLEndTran(SQL_HANDLE_DBC, hDbc, SQL_COMMIT); 
                                    bSuccess = true;
                                }
                            }
                            else {
                                SQLWCHAR state[6], msg[512];
                                SQLINTEGER native; SQLSMALLINT len;
                                SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, state, &native, msg, 512, &len);

                                CString errorStr(msg);
                                if (errorStr.Find(_T("CHK_EMAIL_FORMAT")) != -1) {
                                    AfxMessageBox(_T("База данных отклонила Email! Убедитесь, что в адресе есть @ и точка (например, test@mail.ru)"));
                                }
                                else {
                                    AfxMessageBox(_T("Ошибка базы: ") + errorStr);
                                }
                                SQLEndTran(SQL_HANDLE_DBC, hDbc, SQL_ROLLBACK); 
                            }
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

    if (bSuccess) {
        AfxMessageBox(_T("Регистрация завершена!"));
        EndDialog(IDOK);
    }
}