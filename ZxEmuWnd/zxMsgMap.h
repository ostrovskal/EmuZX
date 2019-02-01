#pragma once

typedef void (zxWnd::*SSH_PMSG)();

struct SSH_MSGMAP_ENTRY {
	UINT nMessage;   // windows message
	UINT nCode;      // control code or WM_NOTIFY code
	UINT nID;        // control ID (or 0 for windows messages)
	UINT nLastID;    // used for entries specifying a range of control id's
	UINT_PTR nSig;   // signature type (action) or pointer to message #
	SSH_PMSG pfn;    // routine to call (or special value)
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

enum ssh_sig {
	sshSig_vv
};

#define ssh_msg

#define ON_COMMAND()
#define ON_SIZE()
#define ON_PAINT()
#define ON_NOTIFY()
#define ON_CREATE()
#define ON_DRAWITEM()
#define ON_DESTROY()
#define ON_CLOSE()
#define ON_MOUSEMOVE()
#define ON_KEYUP()
#define ON_DOWN()
#define ON_LBUTTONDOWN()
#define ON_RBUTTONDOWN()
#define ON_LBUTTONUP()
#define ON_RBUTTONUP()
#define ON_LBUTTONDBLCLK()
#define ON_RBUTTONDBLCLK()
#define ON_VSCROLL()
#define ON_HSCROLL()
#define ON_CTLCOLORSTATIC()
#define ON_ERASEBKGND()
#define ON_ERASEBKGND()
#define ON_MOUSEWHEEL()
#define ON_SETFONT()
#define ON_INITDIALOG()
#define ON_NCCALCSIZE()
