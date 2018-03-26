#pragma once

//-------------------------------------------------------------------------------------
class CMy002Doc : public CDocument
{
public:
    FCObjImage   m_img ;
    FCImageProperty   m_property ;

    ~CMy002Doc() ;

private:
    std::deque<FCObjImage*>   m_undo ;
    std::deque<FCObjImage*>   m_redo ;

    enum TARGET_STATUS
    {
        TARGET_FILE_OK,
        TARGET_FILE_VIOLATION,
        TARGET_FILE_DENY,
    };
    static TARGET_STATUS QuerySaveTargetFileStatus (LPCTSTR strDestFile) ;

    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

    void SaveCurrentToUndo() ;
    void SaveCurrentToRedo() ;
    void ClearRedo() ;

    afx_msg void OnEditUndo();
    afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
    afx_msg void OnEditRedo();
    afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);

    afx_msg void OnFileSaveAs();
    afx_msg void OnToolExportAscii();
    afx_msg void OnImageEffect(UINT nID);
    afx_msg void OnEditCopy();
	DECLARE_MESSAGE_MAP()

    DECLARE_DYNCREATE(CMy002Doc)
};
