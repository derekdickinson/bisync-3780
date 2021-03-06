uint version=1000;
char verlet='B';

USERV uv[]=
{
	{ "A~", argvg,						0,		PARASN,					POINTER,	PARTYP	},
	{	"A0",	&argvg[0],				0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"A1",	&argvg[1],				0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"A2",	&argvg[2],				0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"A3",	&argvg[3],				0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"A4",	&argvg[4],				0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"A5",	&argvg[5],				0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"A6",	&argvg[6],				0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"A7",	&argvg[7],				0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"A8",	&argvg[8],				0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"A9",	&argvg[9],				0,		STRASN|POIASN,	POINTER,	POITYP	},
	{ "AA", abuf,							200,	STRASN,					POINTER,	STRTYP	},
	{	"AC",	&argcg,				 		2,		INTASN,					SCALAR,		UINTYP	},
	{	"AE",	sentable,					128,	STRASN,					POINTER,	STRTYP	},
	{	"AS",	&as,							1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"BA",	&base_address,		2,		INTASN,					SCALAR,		UINTYP	},
	{	"BK",	&baklmt,					1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"BL",	&blocksiz,				2,		INTASN,					SCALAR,		UINTYP	},
	{	"BR",	&rblkcnt,					2,		NOASN,					SCALAR,		UINTYP	},
	{	"BS",	&sblkcnt,					2,		NOASN,					SCALAR,		UINTYP	},
	{	"CA",	&cha_attr,				1,		CHARASN,				SCALAR,		CHARTYP	},
	{ "CB", cmdbuf,						200,	STRASN,					POINTER,	STRTYP	},
	{	"CC",	&curcol,					1,		NOASN,					SCALAR,		CHARTYP	},
	{	"CE",	&cmend,						1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"CF",	cfgfilname,				65,		STRASN,					POINTER,	STRTYP	},
	{	"CO",	&co,							1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"CL",	&cmdline,					2,		NOASN,					SCALAR,		UINTYP	},
	{	"CR",	&curro,						1,		NOASN,					SCALAR,		CHARTYP	},
	{	"DA",	&def_attr,				1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"DB",	&db,							0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"DC",	&dce,							1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"DD",	&thedate.da_day,	1,		NOASN,					SCALAR,		CHARTYP	},
	{ "DF", dispforms,				0,		PARASN,					POINTER,	PARTYP	},
	{	"DI",	&dosgo,						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"DM",	&thedate.da_mon,	1,		NOASN,					SCALAR,		CHARTYP	},
	{	"DO",	&dosreturn,				0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"DP",	&usrmode,					1,		INTASN,					SCALAR,		UINTYP	},
	{	"DR",	curdir,						65,		NOASN,					POINTER,	STRTYP	},
	{	"DS",	NULL,							4,		NOASN,					SCALAR,		DSTYP		},
	{	"DT",	&dostype,					1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"DY",	&thedate.da_year,	1,		NOASN,					SCALAR,		UINTYP	},
	{	"DV",	&thedel,					2,		INTASN,					SCALAR,		UINTYP	},
	{	"EA",	rcvtable,					256,	STRASN,					POINTER,	STRTYP	},
	{	"ED",	emudir,						65,		NOASN,					POINTER,	STRTYP	},
	{	"EF",	&exfile,					0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"EI",	&ei,							1,		NOASN,					SCALAR,		UINTYP	},
	{	"EL",	&el,							2,		INTASN,					SCALAR,		UINTYP	},
	{ "EM", errmessages,			0,		PARASN,					POINTER,	PARTYP	},
	{	"EN", &instdata[8],			13,		NOASN,					POINTER,	STRTYP	},
	{	"EP",	&ep,							2,		INTASN,					SCALAR,		UINTYP	},
	{	"EQ",	&enqlmt,					1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"ER",	&renqcnt,					2,		NOASN,					SCALAR,		UINTYP	},
	{	"ES",	&senqcnt,					2,		NOASN,					SCALAR,		UINTYP	},
	{ "EV", NULL,							0,		PARASN,					POINTER,	PARTYP	},
	{	"FB",	&fixedblanks,			1,		CHARASN,				SCALAR,		CHARTYP	},
	{ "G~", &gptr,						0,		STRASN,					POINTER,	IARTYP	},
	{	"G0",	&g[0],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"G1",	&g[1],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"G2",	&g[2],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"G3",	&g[3],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"G4",	&g[4],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"G5",	&g[5],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"G6",	&g[6],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"G7",	&g[7],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"G8",	&g[8],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"G9",	&g[9],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"GA",	&grph_attr,				1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"GE",	&graphinit,				1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"HA",	&high_attr,				1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"HC",	&hidecur,					1,		CHARASN,				SCALAR,		CHARTYP	},
	{ "I~", &iptr,						0,		STRASN,					POINTER,	IARTYP	},
	{	"I0",	&i[0],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"I1",	&i[1],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"I2",	&i[2],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"I3",	&i[3],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"I4",	&i[4],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"I5",	&i[5],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"I6",	&i[6],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"I7",	&i[7],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"I8",	&i[8],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"I9",	&i[9],						2,		INTASN,					SCALAR,		UINTYP	},
	{	"ID",	&idllmt,					2,		INTASN,					SCALAR,		UINTYP	},
	{	"IO",	&ioc,							1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"IT",	&intrupt,					1,		CHARASN,				SCALAR,		CHARTYP	},
	{ "K0", &messptrs[0],			0,		PARASN,					POINTER,	PARTYP	},
	{ "K1", &messptrs[1],			0,		PARASN,					POINTER,	PARTYP	},
	{ "K2", &messptrs[2],			0,		PARASN,					POINTER,	PARTYP	},
	{ "K3", &messptrs[3],			0,		PARASN,					POINTER,	PARTYP	},
	{ "K4", &messptrs[4],			0,		PARASN,					POINTER,	PARTYP	},
	{ "K5", &messptrs[5],			0,		PARASN,					POINTER,	PARTYP	},
	{ "K6", &messptrs[6],			0,		PARASN,					POINTER,	PARTYP	},
	{ "K7", &messptrs[7],			0,		PARASN,					POINTER,	PARTYP	},
	{ "K8", &messptrs[8],			0,		PARASN,					POINTER,	PARTYP	},
	{ "K9", &messptrs[9],			0,		PARASN,					POINTER,	PARTYP	},
	{	"KA",	&key_attr,				1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"KR",	&rnakcnt,					2,		NOASN,					SCALAR,		UINTYP	},
	{	"KS",	&snakcnt,					2,		NOASN,					SCALAR,		UINTYP	},
	{ "L0", &messptrs[10],		0,		PARASN,					POINTER,	PARTYP	},
	{ "L1", &messptrs[11],		0,		PARASN,					POINTER,	PARTYP	},
	{ "L2", &messptrs[12],		0,		PARASN,					POINTER,	PARTYP	},
	{ "L3", &messptrs[13],		0,		PARASN,					POINTER,	PARTYP	},
	{ "L4", &messptrs[14],		0,		PARASN,					POINTER,	PARTYP	},
	{ "L5", &messptrs[15],		0,		PARASN,					POINTER,	PARTYP	},
	{ "L6", &messptrs[16],		0,		PARASN,					POINTER,	PARTYP	},
	{ "L7", &messptrs[17],		0,		PARASN,					POINTER,	PARTYP	},
	{ "L8", &messptrs[18],		0,		PARASN,					POINTER,	PARTYP	},
	{ "L9", &messptrs[19],		0,		PARASN,					POINTER,	PARTYP	},
	{	"LE",	errmessbuf,				100,	STRASN,					POINTER,	STRTYP	},
	{	"LF",	lfname,						65,		NOASN,					POINTER,	STRTYP	},
	{ "M0", &messptrs[20],		0,		PARASN,					POINTER,	PARTYP	},
	{ "M1", &messptrs[21],		0,		PARASN,					POINTER,	PARTYP	},
	{ "M2", &messptrs[22],		0,		PARASN,					POINTER,	PARTYP	},
	{ "M3", &messptrs[23],		0,		PARASN,					POINTER,	PARTYP	},
	{ "M4", &messptrs[24],		0,		PARASN,					POINTER,	PARTYP	},
	{ "M5", &messptrs[25],		0,		PARASN,					POINTER,	PARTYP	},
	{ "M6", &messptrs[26],		0,		PARASN,					POINTER,	PARTYP	},
	{ "M7", &messptrs[27],		0,		PARASN,					POINTER,	PARTYP	},
	{ "M8", &messptrs[28],		0,		PARASN,					POINTER,	PARTYP	},
	{ "M9", &messptrs[29],		0,		PARASN,					POINTER,	PARTYP	},
	{	"MB",	&mb,							1,		CHARASN,				SCALAR,		CHARTYP	},
	{ "ME",	&mdmerr,					1,		NOASN,					SCALAR,		CHARTYP	},
	{	"ML",	&memleft,			 		2,		INTASN,					SCALAR,		UINTYP	},
	{ "MN", sndmodename,			0,		PARASN,					POINTER,	PARTYP	},
	{	"MV",	&themodem.version,1,		NOASN,					SCALAR,		CHARTYP	},
	{ "MS", statmess,					0,		PARASN,					POINTER,	PARTYP	},
	{ "MT", sts,							0,		PARASN,					POINTER,	PARTYP	},
	{	"N#",	&pun.index,				2,		NOASN,					SCALAR,		UINTYP	},
	{ "N0", &n[0],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "N1", &n[1],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "N2", &n[2],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "N3", &n[3],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "N4", &n[4],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "N5", &n[5],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "N6", &n[6],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "N7", &n[7],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "N8", &n[8],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "N9", &n[9],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{	"NA",	&pun.autoname,		1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"NK",	&naklmt,					1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"NN",	pun.filename,			65,		STRASN,					POINTER,	STRTYP	},
	{	"NS",	&pun.stdcnv,			1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"NT",	&pun.trancnv,			1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"NU",	NULL,							0,		NOASN,					POINTER,	POITYP	},
	{ "NY", noyes,						0,		PARASN,					POINTER,	PARTYP	},
	{ "O0", &n[10],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "O1", &n[11],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "O2", &n[12],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "O3", &n[13],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "O4", &n[14],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "O5", &n[15],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "O6", &n[16],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "O7", &n[17],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "O8", &n[18],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "O9", &n[19],						0,		STRASN|POIASN,	POINTER,	IARTYP	},
	{ "OC", &over_char,				1,		NOASN,					SCALAR,		UINTYP	},
	{ "OG", &over_chars,			6,		STRASN|POIASN,	POINTER,	IARTYP	},
	{	"P#",	&ptr.index,				2,		NOASN,					SCALAR,		UINTYP	},
	{ "P~", p,								0,		PARASN,					POINTER,	PARTYP	},
	{	"P0",	&p[0],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"P1",	&p[1],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"P2",	&p[2],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"P3",	&p[3],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"P4",	&p[4],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"P5",	&p[5],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"P6",	&p[6],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"P7",	&p[7],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"P8",	&p[8],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"P9",	&p[9],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"PA",	&ptr.autoname,		1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"PB",	&pb,							1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"PK",	&packname,				13,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"PN",	ptr.filename,			65,		STRASN,					POINTER,	STRTYP	},
	{ "PP", prinpunch,				0,		PARASN,					POINTER,	PARTYP	},
	{	"PS",	&ptr.stdcnv,			1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"PT",	&ptr.trancnv,			1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"RA",	&rev_attr,				1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"RC",	&remcmds,					1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"RE",	&crlf,						3,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"RL",	&preclen,					2,		INTASN,					SCALAR,		UINTYP	},
	{ "RN", rcvcnv,						0,		PARASN,					POINTER,	PARTYP	},
	{ "RM", srs,							0,		PARASN,					POINTER,	PARTYP	},
	{	"RS",	&datain,					4,		NOASN,					SCALAR,		UINTYP	},
	{	"RT",	&rcvtype,					1,		NOASN,					SCALAR,		CHARTYP	},
	{ "S~", s,								0,		PARASN,					POINTER,	PARTYP	},
	{	"S0",	&s[0],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"S1",	&s[1],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"S2",	&s[2],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"S3",	&s[3],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"S4",	&s[4],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"S5",	&s[5],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"S6",	&s[6],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"S7",	&s[7],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"S8",	&s[8],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"S9",	&s[9],						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"SC",	&bscrn,						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"SD",	&prindest,				1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"SF",	scrname,					65,		STRASN,					POINTER,	STRTYP	},
	{	"SL",	&stepline,				0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"SM",	&buftype,					1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"SN",	senfilname,				65,		NOASN,					POINTER,	STRTYP	},
	{	"SR",	&rreclen,					2,		INTASN,					SCALAR,		UINTYP	},
	{	"SS",	&dataout,					4,		NOASN,					SCALAR,		UINTYP	},
	{	"ST",	&step,						1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"SV",	&mydat,						0,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"TB",	&tb,							1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"TH",	&hour,						1,		NOASN,					SCALAR,		CHARTYP	},
	{	"TI",	&timeout,					2,		INTASN,					SCALAR,		UINTYP	},
	{	"TM",	&minute,					1,		NOASN,					SCALAR,		CHARTYP	},
	{	"TS",	&second,					1,		NOASN,					SCALAR,		CHARTYP	},
	{	"TW",	&tabwid,					1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"UA",	&und_attr,				1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"VA",	&_version,				1,		NOASN,					SCALAR,		CHARTYP	},
	{	"VB",	&version,					1,		NOASN,					SCALAR,		UINTYP	},
	{	"VE",	&eschar,					6,		STRASN|POIASN,	POINTER,	POITYP	},
	{	"VI",	(char	*)&_version+1,1,	NOASN,					SCALAR,		CHARTYP	},
	{	"VL",	&verlet,					1,		NOASN,					SCALAR,		CHARTYP	},
	{	"VM",	&vidmode,					1,		NOASN,					SCALAR,		CHARTYP	},
	{	"VS",	escstr,						0,		PARASN,					POINTER,	PARTYP	},
	{	"WA",	&win_attr,				1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"WB",	&winbottom,				1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"WR",	&winright,				1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"WL",	&winleft,					1,		CHARASN,				SCALAR,		CHARTYP	},
	{	"WT",	&wintop,					1,		CHARASN,				SCALAR,		CHARTYP	},
	{	{	0xff,	0xff	}	}
};