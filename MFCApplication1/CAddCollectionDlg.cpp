#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "CAddCollectionDlg.h"
#include "afxdialogex.h"
#include <sql.h>
#include <sqlext.h>
#include "resource.h"

CAddCollectionDlg::CAddCollectionDlg(CWnd* pParent)
    : CDialogEx(IDD_ADD_COLLECTION, pParent)
{
}

void CAddCollectionDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_NEW_NAME, m_strName);
    DDX_Text(pDX, IDC_EDIT_NEW_DESC, m_strDesc);
}

BEGIN_MESSAGE_MAP(CAddCollectionDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CAddCollectionDlg::OnInitDialog()
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

    return TRUE; 
}

void CAddCollectionDlg::OnOK()
{
    UpdateData(TRUE);

    // Чистим пробелы для корректной проверки
    m_strName.Trim();

    if (m_strName.IsEmpty()) {
        AfxMessageBox(_T("Введите название подборки!"));
        return;
    }
    CDialogEx::OnOK();
}