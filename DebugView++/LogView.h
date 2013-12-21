// (C) Copyright Gert-Jan de Vos and Jan Wilmans 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// Repository at: https://github.com/djeedjay/DebugViewPP/

#pragma once

#include <vector>
#include "OffscreenPaint.h"
#include "LogFile.h"
#include "FilterDlg.h"
#include "DisplayInfo.h"
#include "ProcessInfo.h"

namespace fusion {

typedef CWinTraitsOR<LVS_REPORT | LVS_OWNERDATA | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS> CListViewTraits;

class CMainFrame;

struct SelectionInfo
{
	SelectionInfo();
	SelectionInfo(int beginLine, int endLine, int count);

	int beginLine;
	int endLine;
	int count;
};

struct TextColor
{
	TextColor(COLORREF back, COLORREF fore) :
		back(back), fore(fore)
	{
	}

	COLORREF back;
	COLORREF fore;
};

struct Highlight
{
	Highlight(int begin, int end, COLORREF bkColor, COLORREF fgColor) :
		begin(begin), end(end), bkColor(bkColor), fgColor(fgColor)
	{
	}

	int begin;
	int end;
	COLORREF bkColor;
	COLORREF fgColor;
};

struct LogLine
{
	LogLine(int line, TextColor color);

	int line;
	TextColor color;
	std::vector<Highlight> highlights;
};

class CLogView :
	public CWindowImpl<CLogView, CListViewCtrl, CListViewTraits>,
	public COffscreenPaint<CLogView>
{
public:
	typedef boost::signals2::signal<void (const std::wstring&)> SaitUpdateSignal;

	CLogView(CMainFrame& mainFrame, LogFile& logFile, LogFilter logFilter = LogFilter());

	DECLARE_WND_SUPERCLASS(nullptr, CListViewCtrl::GetWndClassName())

	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);
	void ExceptionHandler();
	BOOL PreTranslateMessage(MSG* pMsg);

	void DoPaint(CDCHandle dc, const RECT& rcClip);

	bool GetScroll() const;
	void SetScroll(bool enable);
	void Clear();
	void Add(int line, const Message& msg);
	void BeginUpdate();
	void EndUpdate();
	void ClearSelection();

	void StopScrolling();
	void ScrollToIndex(int index, bool center);
	void ScrollDown();
	bool IsLastLineSelected();

	bool GetClockTime() const;
	void SetClockTime(bool clockTime);
	void SelectAll();
	void Copy();

	void SetHighlightText(const std::wstring& text = std::wstring());
	bool FindNext(const std::wstring& text);
	bool FindPrevious(const std::wstring& text);

	LogFilter GetFilters() const;
	void SetFilters(const LogFilter& filter);

	using CListViewCtrl::GetItemText;
	std::string GetItemText(int item, int subItem) const;
	std::string GetItemText(int item) const;
	std::wstring GetItemWText(int item, int subItem) const;

	void LoadSettings(CRegKey& reg);
	void SaveSettings(CRegKey& reg);
	void Save(const std::wstring& fileName) const;
	
	std::vector<int> GetSelectedIndices() const;
	SelectionInfo GetViewRange() const;
	SelectionInfo GetSelectedRange() const;

private:
	LRESULT OnCreate(const CREATESTRUCT* pCreate);
	void OnContextMenu(HWND /*hWnd*/, CPoint pt);
	LRESULT OnGetDispInfo(NMHDR* pnmh);
	LRESULT OnDblClick(NMHDR* pnmh);
	LRESULT OnItemChanged(NMHDR* pnmh);
	LRESULT OnCustomDraw(NMHDR* pnmh);
	LRESULT OnOdStateChanged(NMHDR* pnmh);
	LRESULT OnIncrementalSearch(NMHDR* pnmh);
	void OnViewClear(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/);
	void OnViewSelectAll(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/);
	void OnViewCopy(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/);
	void OnViewHideHighlight(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/);
	void OnViewFindNext(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/);
	void OnViewFindPrevious(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/);

	std::string GetSubItemText(int iItem, int iSubItem) const;
	RECT GetItemRect(int iItem, unsigned code) const;
	RECT GetSubItemRect(int iItem, int iSubItem, unsigned code) const;
	void DrawItem(CDCHandle dc, int iItem, unsigned iItemState) const;
	std::vector<Highlight> GetHighlights(const std::string& text) const;
	void DrawSubItem(CDCHandle dc, int iItem, int iSubItem) const;

	bool Find(const std::string& text, int direction);
	void ApplyFilters();
	bool IsIncluded(const std::string& text) const;
	bool IsStop(const std::string& text) const;
	bool IsTrack(const std::string& text) const;
	TextColor GetTextColor(const std::string& text) const;

	CMainFrame& m_mainFrame;
	LogFile& m_logFile;
	LogFilter m_filter;
	std::vector<LogLine> m_logLines;
	bool m_clockTime;
	bool m_autoScrollDown;
	DisplayInfo m_displayInfo;
	ProcessInfo m_processInfo;
	bool m_dirty;
	std::function<void ()> m_stop;
	std::function<void ()> m_track;
	bool m_insidePaint;
	std::wstring m_highlightText;
};

} // namespace fusion
