#pragma once
#include "afxdialogex.h"
#include <vector>

class CSoftwareDetailDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CSoftwareDetailDlg)

public:
    CSoftwareDetailDlg(long softwareId, CWnd* pParent = nullptr);
    virtual ~CSoftwareDetailDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SOFTWARE_DETAIL };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
private:
    long m_softwareId;
    HICON m_hIcon;

    CString m_name, m_description, m_license, m_category, m_functions;
    CString m_developer, m_website, m_size;
    CString m_sysreq_os, m_sysreq_ram, m_sysreq_cpu, m_sysreq_gpu, m_sysreq_disk;

    CStatic* m_pImgAnchor;
    CFont m_fontCustom;
    std::vector<BYTE> m_imgBuffer;

    // Вектор для хранения динамических элементов, чтобы они не вызывали Assert при закрытии/открытии
    std::vector<CWnd*> m_dynamicControls;

    void LoadSoftwareDetails();
    void LoadScreenshot();

    DECLARE_MESSAGE_MAP()
};