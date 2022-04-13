/* See LICENSE file for copyright and license details. */
#include "/usr/X11R6/include/X11/XF86keysym.h"

/* appearance */
static unsigned int borderpx  = 2;        /* border pixel of windows */
static unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
static char font[]            = "monospace:size=10";
static char dmenufont[]       = "monospace:size=10";
static const char *fonts[]          = { font };
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selbgcolor[]            = "#005577";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "St",          NULL,     NULL,           0,         0,          1,          0,        -1 },
	{ "qutebrowser", NULL,     NULL,           1 << 3,    0,          0,          0,        -1 },
	{ "Firefox",     NULL,     NULL,           1 << 3,    0,          0,          0,        -1 },
	{ "Thunar",      NULL,     NULL,           1 << 5,		  0,          0,          0,        -1 },
	{ "okular",      NULL,     NULL,           0,         0,          0,          0,        -1 },
	{ "MuPDF",       NULL,     NULL,           0,         0,          1,          0,        -1 },
	{ "St",			 NULL,     "neomutt",      1 << 4,         0,          0,          0,        -1 },
	{ "St",			 NULL,     "calcurse",     1 << 4,         0,          0,          0,        -1 },

	{ NULL,          NULL,     "Event Tester", 0,           0,          0,           1,        -1 }, /* xev */
};


/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1
#include "nrowgrid.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "###",      nrowgrid },
	{ "><>",      NULL },    /* no layout function means floating behavior */
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \
/*	{ MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \*/


/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34", NULL };
static const char *calcmd[] = { "st", "-e", "calcurse", NULL };
static const char *editorcmd[] = { "st", "-e", "nvim", NULL  };
static const char *firefoxcmd[]  = { "/usr/local/bin/firefox", NULL };
static const char *mailcmd[] = { "st", "-e", "neomutt", NULL };
static const char *lfcmd[] = { "st", "-e", "sfm", NULL };
static const char *thunarcmd[] = { "/usr/local/bin/thunar", NULL };
static const char *webcmd[]  = { "/usr/local/bin/qutebrowser", NULL };
static const char *zoterocmd[] = { "/usr/local/bin/zotero", NULL };
static const char *volupcmd[] = { "amixer", "-q", "set", "Master", "5%+", "unmute", NULL };
static const char *voldowncmd[] = { "amixer", "-q", "set", "Master", "5%-", "unmute",  NULL };
static const char *mutecmd[] = { "amixer", "-q", "set", "Master", "toggle", NULL };
static const char *brupcmd[] = { "sudo", "xbacklight", "-inc", "10", NULL  };
static const char *brdowncmd[] = { "sudo", "xbacklight", "-dec", "10", NULL  };
static const char *exbrupcmd[] = { "st", "ddcutil", "setvpc", "10", "-", "10", NULL  };
static const char *exbrdowncmd[] = { "st", "ddcutil", "setvpc", "10", "-", "10", NULL  };
static const char *mediacmd[] = { "st", "-e", "pavucontrol", NULL };
static const char *wificmd[] = { "st", "-e", "networkmanager_dmenu", NULL };
static const char *slockcmd[] = { "/usr/local/bin/slock", NULL };
static const char *printcmd[] = { "st", "-e", "maim", NULL };
/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "font",               STRING,  &font },
		{ "dmenufont",          STRING,  &dmenufont },
		{ "color0",             STRING,  &normbgcolor },
		{ "color8",             STRING,  &normbordercolor },
		{ "color18",            STRING,  &normfgcolor },
		{ "color17",            STRING,  &selbgcolor },
		{ "color7",             STRING,  &selbordercolor },
		{ "color18",            STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          	INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	FLOAT,   &mfact },
};

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY|ShiftMask,             XK_BackSpace, quit,         {0} },
	{ MODKEY,                       XK_q,       killclient,     {0} },
	{ MODKEY,                       XK_d,       spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return,  spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_s,       togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,                       XK_f,       togglefullscr,  {0} },
	{ MODKEY,						XK_w,	    spawn,			{.v = webcmd } },
	{ MODKEY|ShiftMask,				XK_w,	    spawn,			{.v = firefoxcmd } },
	{ MODKEY,						XK_c,	    spawn,		    {.v = calcmd } },
	{ MODKEY,						XK_o,	    spawn,			{.v = lfcmd } },
	{ MODKEY|ShiftMask,				XK_o,	    spawn,		    {.v = thunarcmd } },
	{ MODKEY,						XK_m,	    spawn,			{.v = mailcmd } },
	{ MODKEY,                       XK_e,       spawn,          {.v = editorcmd } },
	{ MODKEY,                       XK_t,       spawn,          {.v = slockcmd } },
	{ MODKEY,                       XK_space,   zoom,           {0} },
	{ MODKEY|ShiftMask,             XK_space,   togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_s,       togglesticky,   {0} },
	{ MODKEY,                       XK_Tab,     view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_t,       tag,            {.ui = ~0 } },
	{ MODKEY,					    XK_l,       setmfact,       {.f = +0.05} },
	{ MODKEY,		                XK_h,       setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_equal,   incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_minus,   incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_Left,    focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_Right,   focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_Left,    tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_Right,   tagmon,         {.i = +1 } },
	{ MODKEY|Mod1Mask,              XK_b,       togglebar,      {0} },
	{ MODKEY|Mod1Mask,              XK_comma,   layoutscroll,   {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_period,  layoutscroll,   {.i = +1 } },
	{ MODKEY|Mod1Mask,              XK_t,       setlayout,      {.v = &layouts[0]} },
	{ MODKEY|Mod1Mask,              XK_f,       setlayout,      {.v = &layouts[3]} },
	{ MODKEY|Mod1Mask,              XK_m,       setlayout,      {.v = &layouts[1]} },
	{ MODKEY|Mod1Mask,              XK_g,       setlayout,      {.v = &layouts[2]} },
	{ 0,							XK_Print,	spawn,			{.v = printcmd } },
/*	{ MODKEY,						XK_F1,		spawn,			{.v = mediacmd } },
	{ MODKEY,						XK_F10,		spawn,			{.v = wificmd } },
	{ 0,                            XF86XK_AudioMute,         spawn,          {.v = mutecmd }   },
	{ 0,                            XF86XK_AudioLowerVolume,  spawn,          {.v = voldowncmd} },
	{ 0,                            XF86XK_AudioRaiseVolume,  spawn,          {.v = volupcmd}   },
	{ 0,                            XF86XK_MonBrightnessUp,   spawn,          {.v = brupcmd}    },
	{ 0,                            XF86XK_MonBrightnessDown, spawn,          {.v = brdowncmd}  },*/
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

