#pragma once

typedef void (zxWnd::*SSH_PMSG)();
typedef void (zxWnd::*SSH_PMSGW)();

struct SSH_NOTIFY {
	LRESULT* pResult;
	NMHDR* pNMHDR;
};

struct SSH_MSGMAP_ENTRY {
	UINT nMessage;   // сообщение
	UINT nCode;      // управл€ющий код или код уведомлени€
	UINT nID;        // идентификатор контрола(или 0 дл€ сообшени€)
	UINT nLastID;    // последний идентификатор контрола(дл€ диапазона идентификаторов)
	UINT_PTR nSig;   // сигнатура
	SSH_PMSG pfn;    // вызываема€ функци€
};

struct SSH_MSGMAP {
	const SSH_MSGMAP* (PASCAL* pfnGetBaseMap)();
	const SSH_MSGMAP_ENTRY* lpEntries;
};

#define DECL_MSG_MAP() \
protected: \
	static const SSH_MSGMAP* PASCAL GetThisMessageMap(); \
	virtual const SSH_MSGMAP* GetMessageMap() const;

#define BEGIN_MSG_MAP(theClass, baseClass) \
	__pragma(warning( push )) \
	__pragma(warning( disable : 4867 )) \
	const SSH_MSGMAP* theClass::GetMessageMap() const { return GetThisMessageMap(); } \
	const SSH_MSGMAP* PASCAL theClass::GetThisMessageMap() { \
		typedef theClass ThisClass;						    \
		typedef baseClass TheBaseClass;					    \
		__pragma(warning(push))							    \
		__pragma(warning(disable: 4640))					\
		static const SSH_MSGMAP_ENTRY _messageEntries[] =  {

#define END_MSG_MAP() \
		{0, 0, 0, 0, 0, (SSH_PMSG)0 } }; \
		__pragma(warning(pop))	\
		static const SSH_MSGMAP messageMap = { &TheBaseClass::GetThisMessageMap, &_messageEntries[0] }; \
		return &messageMap; }  \
	__pragma(warning( pop ))

#define ssh_msg

enum SshSig {
	sshSig_end = 0,				// [marks end of message map]
	sshSig_b_D_v,				// BOOL (HDC*)
	sshSig_b_b_v,				// BOOL (BOOL)
	sshSig_b_u_v,				// BOOL (UINT)
	sshSig_b_h_v,				// BOOL (HANDLE)
	sshSig_b_W_uu,				// BOOL (HWND, UINT, UINT)
	sshSig_i_u_W_u,				// int (UINT, HWND, UINT)  // ?TOITEM
	sshSig_i_uu_v,				// int (UINT, UINT)
	sshSig_i_W_uu,				// int (HWND, UINT, UINT)
	sshSig_i_v_s,				// int (LPTSTR)
	sshSig_l_w_l,				// LRESULT (WPARAM, LPARAM)
	sshSig_l_uu_M,				// LRESULT (UINT, UINT, HMENU)
	sshSig_v_b_h,				// void (BOOL, HANDLE)
	sshSig_v_h_v,				// void (HANDLE)
	sshSig_v_h_h,				// void (HANDLE, HANDLE)
	sshSig_v_v_v,				// void ()
	sshSig_v_u_v,				// void (UINT)
	sshSig_v_up_v,				// void (UINT_PTR)
	sshSig_v_u_u,				// void (UINT, UINT)
	sshSig_v_uu_v,				// void (UINT, UINT)
	sshSig_v_v_ii,				// void (int, int)
	sshSig_v_u_uu,				// void (UINT, UINT, UINT)
	sshSig_v_u_ii,				// void (UINT, int, int)
	sshSig_v_u_W,				// void (UINT, HWND)
	sshSig_i_u_v,				// int (UINT)
	sshSig_u_u_v,				// UINT (UINT)
	sshSig_b_v_v,				// BOOL ()
	sshSig_v_w_l,				// void (WPARAM, LPARAM)
	sshSig_v_D_v,				// void (HDC)
	sshSig_v_W_v,				// void (HWND)
	sshSig_v_v_W,				// void (HWND)
	sshSig_v_W_uu,				// void (HWND, UINT, UINT)
	sshSig_v_W_p,				// void (HWND, POINT)
	sshSig_v_W_h,				// void (HWND, HANDLE)
	sshSig_C_v_v,				// HCURSOR ()
	sshSig_ACTIVATE,			// void (UINT, HWND, BOOL)
	sshSig_SCROLL,				// void (UINT, UINT, HWND)
	sshSig_SCROLL_REFLECT,		// void (UINT, UINT)
	sshSig_v_v_s,				// void (LPTSTR)
	sshSig_v_u_cs,				// void (UINT, LPCTSTR)
	sshSig_OWNERDRAW,			// void (int, LPTSTR) force return TRUE
	sshSig_i_i_s,				// int (int, LPTSTR)
	sshSig_u_v_p,				// UINT (POINT)
	sshSig_u_v_v,				// UINT ()
	sshSig_v_b_NCCALCSIZEPARAMS,// void (BOOL, NCCALCSIZE_PARAMS*)
	sshSig_v_v_WINDOWPOS,		// void (WINDOWPOS*)
	sshSig_v_uu_M,				// void (UINT, UINT, HMENU)
	sshSig_v_u_p,				// void (UINT, POINT)
	sshSig_SIZING,				// void (UINT, LPRECT)
	sshSig_MOUSEWHEEL,			// BOOL (UINT, short, POINT)
	sshSig_MOUSEHWHEEL,			// void (UINT, short, POINT)
	sshSigNotify_v,				// void (NMHDR*, LRESULT*)
	sshSigNotify_b,				// BOOL (NMHDR*, LRESULT*)
	sshSigNotify_RANGE,			// void (UINT, NMHDR*, LRESULT*)
	sshSigNotify_EX,			// BOOL (UINT, NMHDR*, LRESULT*)
	sshSigCmd_v_pv,				// void (void*)
	sshSigCmd_b_pv,				// BOOL (void*)
	sshSig_l,					// LRESULT ()
	sshSig_l_p,					// LRESULT (POINT)	
	sshSig_u_W_u,				// UINT (HWND, UINT)
	sshSig_v_u_M,				// void (UINT, HMenu )
	sshSig_u_u_M,				// UINT (UINT, HMenu )
	sshSig_APPCOMMAND,			// void (HWND, UINT, UINT, UINT)
	sshSig_u_u_u,				// UINT (UINT, UINT)
	sshSig_MOUSE_XBUTTON,		// void (UINT, UINT, POINT)
	sshSig_MOUSE_NCXBUTTON,		// void (short, UINT, POINT)
	sshSig_l_D_u,				// LRESULT (HDC, UINT)
	sshSig_i_v_S,				// int (LPCTSTR)
	sshSig_v_F_b,				// void (CFont*, BOOL)
	sshSig_h_v_v,				// HANDLE ()
	sshSig_h_b_h,				// HANDLE (BOOL, HANDLE)
	sshSig_b_v_ii,				// BOOL (int, int)
	sshSig_h_h_h,				// HANDLE (HANDLE, HANDLE)
	sshSig_u_u_l,				// UINT (UINT, LPARAM)
	sshSigCmd_v,				// void ()
	sshSigCmd_b,				// BOOL ()
	sshSigCmdUI,				// void (CCmdUI*)
};

#define CN_COMMAND              0               // void ()
#define CN_UPDATE_COMMAND_UI    ((UINT)(-1))    // void (CCmdUI*)

#define ON_COMMAND(id, memberFxn) { WM_COMMAND, 0, (WORD)id, (WORD)id, sshSigCmd_v, static_cast<SSH_PMSG>(&ThisClass::memberFxn) },

#define ON_COMMAND_RANGE(id, idLast, memberFxn) { WM_COMMAND, 0, (WORD)id, (WORD)idLast, sshSigCmd_v, static_cast<SSH_PMSG>(&ThisClass::memberFxn) },

#define ON_NOTIFY(wNotifyCode, id, memberFxn) { WM_NOTIFY, wNotifyCode, (WORD)id, (WORD)id, sshSigNotify_v, (SSH_PMSG)(static_cast<void (zxWnd::*)(NMHDR*, LRESULT*)>(&ThisClass::memberFxn)) },

#define ON_NOTIFY_RANGE(wNotifyCode, id, lastId, memberFxn) { WM_NOTIFY, wNotifyCode, (WORD)id, (WORD)lastId, sshSigNotify_v, (SSH_PMSG)(static_cast<void (zxWnd::*)(NMHDR*, LRESULT*)>(&ThisClass::memberFxn)) },

#define ON_CONTROL(wNotifyCode, id, memberFxn) { WM_COMMAND, (WORD)wNotifyCode, (WORD)id, (WORD)id, sshSigCmd_v, (static_cast<SSH_PMSG>(&ThisClass::memberFxn)) },

#define ON_CONTROL_RANGE(wNotifyCode, id, lastId, memberFxn) { WM_COMMAND, (WORD)wNotifyCode, (WORD)id, (WORD)lastId, sshSigCmd_v, (static_cast<SSH_PMSG>(&ThisClass::memberFxn)) },

#define ON_WM_CREATE() { WM_CREATE, 0, 0, 0, sshSig_i_v_s, (SSH_PMSG) (SSH_PMSGW)(static_cast<int (zxWnd::*)(LPCREATESTRUCT)>(&ThisClass::onCreate)) },

#define ON_WM_DESTROY() { WM_DESTROY, 0, 0, 0, sshSig_v_v_v, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(void)>(&ThisClass::onDestroy)) },

#define ON_WM_MOVE() { WM_MOVE, 0, 0, 0, sshSig_v_u_ii, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(int, int)>(&ThisClass::onMove)) },

#define ON_WM_SIZE() { WM_SIZE, 0, 0, 0, sshSig_v_u_ii, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, int, int)>(&ThisClass::onSize)) },

#define ON_WM_SIZING() { WM_SIZING, 0, 0, 0, sshSig_SIZING, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, LPRECT)>(&ThisClass::onSizing)) },

#define ON_WM_ACTIVATE() { WM_ACTIVATE, 0, 0, 0, sshSig_ACTIVATE, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, HWND, BOOL)>(&ThisClass::onActivate)) },

#define ON_WM_SETFOCUS() { WM_SETFOCUS, 0, 0, 0, sshSig_v_W_v, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(HWND)>(&ThisClass::onSetFocus)) },

#define ON_WM_KILLFOCUS() { WM_KILLFOCUS, 0, 0, 0, sshSig_v_W_v, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(HWND)>(&ThisClass::onKillFocus)) },

#define ON_WM_ENABLE() { WM_ENABLE, 0, 0, 0, sshSig_v_u_v, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(BOOL)>(&ThisClass::onEnable)) },

#define ON_WM_PAINT() { WM_PAINT, 0, 0, 0, sshSig_v_v_v, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(void)>(&ThisClass::onPaint)) },

#define ON_WM_CLOSE() { WM_CLOSE, 0, 0, 0, sshSig_v_v_v, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(void)>(&ThisClass::onClose)) },

#define ON_WM_ERASEBKGND() { WM_ERASEBKGND, 0, 0, 0, sshSig_b_D_v, (SSH_PMSG)(SSH_PMSGW)(static_cast<BOOL (zxWnd::*)(HDC)>(&ThisClass::onEraseBkgnd)) },

#define ON_WM_SETFONT() { WM_SETFONT, 0, 0, 0, sshSig_v_F_b, (SSH_PMSG) (SSH_PMSGW)(static_cast<void (zxWnd::*)(HFONT, BOOL)>(&ThisClass::onSetFont)) },

#define ON_WM_SHOWWINDOW() { WM_SHOWWINDOW, 0, 0, 0, sshSig_v_u_u, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(BOOL, UINT)>(&ThisClass::onShowWindow)) },

#define ON_WM_ACTIVATEAPP() { WM_ACTIVATEAPP, 0, 0, 0, sshSig_v_u_u, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(BOOL, DWORD)>(&ThisClass::onActivateApp)) },

#define ON_WM_SETCURSOR() { WM_SETCURSOR, 0, 0, 0, sshSig_b_W_uu, (SSH_PMSG)(SSH_PMSGW)(static_cast<BOOL (zxWnd::*)(HWND, UINT, UINT)>(&ThisClass::onSetCursor)) },

#define ON_WM_DRAWITEM() { WM_DRAWITEM, 0, 0, 0, sshSig_OWNERDRAW, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(int, LPDRAWITEMSTRUCT)>(&ThisClass::onDrawItem)) },

#define ON_WM_NCCREATE() { WM_NCCREATE, 0, 0, 0, sshSig_i_v_s, (SSH_PMSG)(SSH_PMSGW)(static_cast<BOOL (zxWnd::*)(LPCREATESTRUCT)>(&ThisClass::onNcCreate)) },

#define ON_WM_NCDESTROY() { WM_NCDESTROY, 0, 0, 0, sshSig_v_v_v, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(void)>(&ThisClass::onNcDestroy)) },

#define ON_WM_NCCALCSIZE() { WM_NCCALCSIZE, 0, 0, 0, sshSig_v_b_NCCALCSIZEPARAMS, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(BOOL, NCCALCSIZE_PARAMS*)>(&ThisClass::onNcCalcSize)) },

#define ON_WM_NCPAINT() { WM_NCPAINT, 0, 0, 0, sshSig_v_v_v, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(void) > ( &ThisClass::onNcPaint)) },

#define ON_WM_KEYDOWN() { WM_KEYDOWN, 0, 0, 0, sshSig_v_u_uu, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, UINT, UINT) > ( &ThisClass::onKeyDown)) },

#define ON_WM_KEYUP() { WM_KEYUP, 0, 0, 0, sshSig_v_u_uu, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, UINT, UINT) > ( &ThisClass::onKeyUp)) },

#define ON_WM_CHAR() { WM_CHAR, 0, 0, 0, sshSig_v_u_uu, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, UINT, UINT) > ( &ThisClass::onChar)) },

#define ON_WM_TIMER() { WM_TIMER, 0, 0, 0, sshSig_v_up_v, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT_PTR) > ( &ThisClass::onTimer)) },

#define ON_WM_HSCROLL() { WM_HSCROLL, 0, 0, 0, sshSig_SCROLL, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, UINT, HWND)>(&ThisClass::onHScroll)) },

#define ON_WM_VSCROLL() { WM_VSCROLL, 0, 0, 0, sshSig_SCROLL, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, UINT, HWND)>(&ThisClass::onVScroll)) },

#define ON_WM_ENTERIDLE() { WM_ENTERIDLE, 0, 0, 0, sshSig_v_u_W, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, HWND) > ( &ThisClass::onEnterIdle)) },

#define ON_WM_MOUSEMOVE() { WM_MOUSEMOVE, 0, 0, 0, sshSig_v_u_p, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, POINT) > ( &ThisClass::onMouseMove)) },

#define ON_WM_MOUSEWHEEL() { WM_MOUSEWHEEL, 0, 0, 0, sshSig_MOUSEWHEEL, (SSH_PMSG)(SSH_PMSGW)(static_cast<BOOL (zxWnd::*)(UINT, short, POINT) > ( &ThisClass::onMouseWheel)) },

#define ON_WM_LBUTTONDOWN() { WM_LBUTTONDOWN, 0, 0, 0, sshSig_v_u_p, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, POINT) > ( &ThisClass::onLButtonDown)) },

#define ON_WM_LBUTTONUP() { WM_LBUTTONUP, 0, 0, 0, sshSig_v_u_p, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, POINT) > ( &ThisClass::onLButtonUp)) },

#define ON_WM_LBUTTONDBLCLK() { WM_LBUTTONDBLCLK, 0, 0, 0, sshSig_v_u_p, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, POINT) > ( &ThisClass::onLButtonDblClk)) },

#define ON_WM_RBUTTONDOWN() { WM_RBUTTONDOWN, 0, 0, 0, sshSig_v_u_p, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, POINT) > ( &ThisClass::onRButtonDown)) },

#define ON_WM_RBUTTONUP() { WM_RBUTTONUP, 0, 0, 0, sshSig_v_u_p, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, POINT) > ( &ThisClass::onRButtonUp)) },

#define ON_WM_RBUTTONDBLCLK() { WM_RBUTTONDBLCLK, 0, 0, 0, sshSig_v_u_p, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, POINT) > ( &ThisClass::onRButtonDblClk)) },

#define ON_WM_MBUTTONDOWN() { WM_MBUTTONDOWN, 0, 0, 0, sshSig_v_u_p, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, POINT) > ( &ThisClass::onMButtonDown)) },

#define ON_WM_MBUTTONUP() { WM_MBUTTONUP, 0, 0, 0, sshSig_v_u_p, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, POINT) > ( &ThisClass::onMButtonUp)) },

#define ON_WM_MBUTTONDBLCLK() { WM_MBUTTONDBLCLK, 0, 0, 0, sshSig_v_u_p, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(UINT, POINT) > ( &ThisClass::onMButtonDblClk)) },

#define ON_WM_CONTEXTMENU() { WM_CONTEXTMENU, 0, 0, 0, sshSig_v_u_p, (SSH_PMSG)(SSH_PMSGW)(static_cast<void (zxWnd::*)(HWND, POINT) > ( &ThisClass::onContextMenu)) },

#define ON_WM_INITDIALOG() { WM_INITDIALOG, 0, 0, 0, sshSig_l_w_l, (SSH_PMSG)(SSH_PMSGW)(static_cast<int (zxWnd::*)(WPARAM,  LPARAM) > ( &ThisClass::onInitDialog)) },

union MessageMapFunctions {
	SSH_PMSG pfn;
	BOOL	(zxWnd::*pfn_b_D)(HDC);
	BOOL	(zxWnd::*pfn_b_b)(BOOL);
	BOOL	(zxWnd::*pfn_b_u)(UINT);
	BOOL	(zxWnd::*pfn_b_h)(HANDLE);
	BOOL	(zxWnd::*pfn_b_W_u_u)(HWND, UINT, UINT);
	BOOL	(zxWnd::*pfn_b_W_COPYDATASTRUCT)(HWND, COPYDATASTRUCT*);
	BOOL	(zxWnd::*pfn_b_HELPINFO)(LPHELPINFO);
	HBRUSH	(zxWnd::*pfn_B_D_W_u)(HDC, HWND, UINT);
	HBRUSH	(zxWnd::*pfn_B_D_u)(HDC, UINT);
	int		(zxWnd::*pfn_i_u_W_u)(UINT, HWND, UINT);
	int		(zxWnd::*pfn_i_u_u)(UINT, UINT);
	int		(zxWnd::*pfn_i_W_u_u)(HWND, UINT, UINT);
	int		(zxWnd::*pfn_i_s)(LPTSTR);
	int		(zxWnd::*pfn_i_S)(LPCTSTR);
	LRESULT	(zxWnd::*pfn_l_w_l)(WPARAM, LPARAM);
	void	(zxWnd::*pfn_v_b_h)(BOOL, HANDLE);
	void	(zxWnd::*pfn_v_h)(HANDLE);
	void	(zxWnd::*pfn_v_h_h)(HANDLE, HANDLE);
	void	(zxWnd::*pfn_v_v)();
	int		(zxWnd::*pfn_i_u)(UINT);
	HCURSOR (zxWnd::*pfn_C_v)();
	UINT	(zxWnd::*pfn_u_u)(UINT);
	BOOL	(zxWnd::*pfn_b_v)();
	void	(zxWnd::*pfn_v_u)(UINT);
	void	(zxWnd::*pfn_v_up)(UINT_PTR);
	void	(zxWnd::*pfn_v_u_u)(UINT, UINT);
	void	(zxWnd::*pfn_v_i_i)(int, int);
	void	(zxWnd::*pfn_v_u_u_u)(UINT, UINT, UINT);
	void	(zxWnd::*pfn_v_u_i_i)(UINT, int, int);
	void	(zxWnd::*pfn_v_w_l)(WPARAM, LPARAM);
	void	(zxWnd::*pfn_v_b_W_W)(BOOL, HWND, HWND);
	void	(zxWnd::*pfn_v_D)(HDC);
	void	(zxWnd::*pfn_v_W)(HWND);
	void	(zxWnd::*pfn_v_W_u_u)(HWND, UINT, UINT);
	void	(zxWnd::*pfn_v_W_p)(HWND, POINT);
	void	(zxWnd::*pfn_v_W_h)(HWND, HANDLE);
	void	(zxWnd::*pfn_v_u_W)(UINT, HWND);
	void	(zxWnd::*pfn_v_u_W_b)(UINT, HWND, BOOL);
	void	(zxWnd::*pfn_v_u_u_W)(UINT, UINT, HWND);
	void	(zxWnd::*pfn_v_s)(LPTSTR);
	void	(zxWnd::*pfn_v_u_cs)(UINT, LPCTSTR);
	void	(zxWnd::*pfn_v_i_s)(int, LPTSTR);
	int		(zxWnd::*pfn_i_i_s)(int, LPTSTR);
	UINT	(zxWnd::*pfn_u_p)(POINT);
	LRESULT	(zxWnd::*pfn_l_p)(POINT);
	UINT	(zxWnd::*pfn_u_v)();
	void	(zxWnd::*pfn_v_b_NCCALCSIZEPARAMS)(BOOL, NCCALCSIZE_PARAMS*);
	void	(zxWnd::*pfn_v_v_WINDOWPOS)(WINDOWPOS*);
	void	(zxWnd::*pfn_v_u_u_M)(UINT, UINT, HMENU);
	void	(zxWnd::*pfn_v_u_p)(UINT, POINT);
	void	(zxWnd::*pfn_v_u_pr)(UINT, LPRECT);
	BOOL	(zxWnd::*pfn_b_u_s_p)(UINT, short, POINT);
	void	(zxWnd::*pfn_MOUSEHWHEEL)(UINT, short, POINT);
	LRESULT	(zxWnd::*pfn_l_v)();
	UINT	(zxWnd::*pfn_u_W_u)(HWND, UINT);
	UINT	(zxWnd::*pfn_u_v_MENUGETOBJECTINFO)(MENUGETOBJECTINFO*);
	void	(zxWnd::*pfn_v_u_LPMDINEXTMENU)(UINT, LPMDINEXTMENU);
	void	(zxWnd::*pfn_APPCOMMAND)(HWND, UINT, UINT, UINT);
	BOOL	(zxWnd::*pfn_RAWINPUT)(UINT, HRAWINPUT);
	UINT	(zxWnd::*pfn_u_u_u)(UINT, UINT);
	UINT	(zxWnd::*pfn_u_u_l)(UINT, LPARAM);
	void	(zxWnd::*pfn_MOUSE_XBUTTON)(UINT, UINT, POINT);
	void	(zxWnd::*pfn_MOUSE_NCXBUTTON)(short, UINT, POINT);
	void	(zxWnd::*pfn_INPUTLANGCHANGE)(UINT, UINT);
	BOOL	(zxWnd::*pfn_v_u_h)(UINT, HANDLE);
	void	(zxWnd::*pfn_INPUTDEVICECHANGE)(unsigned short, HANDLE);
	LRESULT	(zxWnd::*pfn_l_D_u)(HDC, UINT);
	void	(zxWnd::*pfn_v_F_b)(HFONT, BOOL);
	HANDLE	(zxWnd::*pfn_h_v)();
	HANDLE	(zxWnd::*pfn_h_b_h)(BOOL, HANDLE);
	BOOL	(zxWnd::*pfn_b_v_ii)(int, int);
	HANDLE	(zxWnd::*pfn_h_h_h)(HANDLE, HANDLE);
	void	(zxWnd::*pfn_v_W_b)(HWND, BOOL);
	void	(zxWnd::*pfnCmd_v_uu_W)(UINT, UINT, HWND);
	void	(zxWnd::*pfnCmd_v_v)();
	BOOL	(zxWnd::*pfnCmd_b_v)();
	void	(zxWnd::*pfnCmd_v_pv)(void*);
	BOOL	(zxWnd::*pfnCmd_b_pv)(void*);
	void	(zxWnd::*pfnNotify_v_NMHDR_pl)(NMHDR*, LRESULT*);
	BOOL	(zxWnd::*pfnNotify_b_NMHDR_pl)(NMHDR*, LRESULT*);
//	void	(zxWnd::*pfnCmdUI_v_C)(CCmdUI*);
//	void	(zxWnd::*pfnCmdUI_v_C_u)(CCmdUI*, UINT);
	void	(zxWnd::*pfn_vPOS)(WINDOWPOS*);
	void	(zxWnd::*pfn_vCALC)(BOOL, NCCALCSIZE_PARAMS*);
	void	(zxWnd::*pfn_vwp)(UINT, POINT);
	void	(zxWnd::*pfn_vwwh)(UINT, UINT, HANDLE);
	BOOL	(zxWnd::*pfn_bwsp)(UINT, short, POINT);
};
