#pragma once
#include <afxwin.h>

class CRegisterDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CRegisterDlg)

public:
    CRegisterDlg(CWnd* pParent = nullptr);   
    virtual ~CRegisterDlg();

    // Данные диалогового окна
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_REGISTER_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);   
    virtual BOOL OnInitDialog();                        

    DECLARE_MESSAGE_MAP()

public:
    // Обработчик нажатия кнопки
    afx_msg void OnRegisterButton();

private:
    // Вспомогательная функция для хеширования пароля
    CString HashPassword(const CString& password);

    // Указатели на динамически создаваемые элементы интерфейса
    CEdit* m_pEditName;
    CEdit* m_pEditEmail;
    CEdit* m_pEditLogin;
    CEdit* m_pEditPassword;

    CButton* m_pButtonRegister;

    // Указатели на текстовые метки 
    CStatic* m_pStaticName;
    CStatic* m_pStaticEmail;
    CStatic* m_pStaticLogin;
    CStatic* m_pStaticPassword;

    // Объект шрифта
    CFont m_fontCustom;
};