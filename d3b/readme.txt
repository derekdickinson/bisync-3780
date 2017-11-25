This disk contains the complete source, object and executable for D3B.EXE the
main program of SyncTax (it file is renamed to SYNCTAXX.EXE on the SyncTax
release disk).

Borland C++ 2.0 is used for the ALL parts of the the development. The
assembly portions of the code are assembled with the TASM assembler which
is included in the Borland C++ package. The ASCII C standard conventions are
used and Borland's IDEAL mode is used for the assembly (this mode has a
somewhat simpler syntax than the microsoft standard). The package is compiled
for the Medium memory model (i.e. 1 data segment, multiple code segments).

The project file in "x:\D3B\D3B.PRJ" lists the files used. The compilation
should be done from the "x:\D3B" directory after naming D3B.PRJ as your
project file. All compiler options are stored in the project file so you
shouldn't need to set anything up to create a copy of the executable.

Note: The word "command" will generally be used for the function(s) which
	are used to create the script language command and the word "function"
	will be used for C functions (sometimes assembly).

Extensions:
	file.c       ; C source file
	file.h			 ; C header file
	file.asm		 ; Assembly language file
	file.inc		 ; Assembly include file
	D3B.PRJ			 ; The Borland C++ 2.0 project file

The directory structure used is :

COMMON			- Generic utility functions (C source files)
	INCLUDE		- Header files for C source in COMMON
	ASMS			- Generic assembly language files and includes (screen stuff)

D3B      	  - C source files
	INCLUDE		- Headers for C source in D3B
	ASMS			- Assembly language and include files (modem hardware interface)
	EXE				- The executable D3B.EXE
	OBJS			- The object files created from the C and assembly.

In general, the code in D3B is modem hardware involved and the code in
COMMON is implementing the script language of the emulator.

File List:
\COMMON
	\COMMON\BRANCMDS.C		; Script file branching commands
	\COMMON\COMNDATA.C		; Global Data used in \COMMON
	\COMMON\DIAGCMDS.C		; Diagnostic commands
	\COMMON\DYNACMDS.C		; Commands which perform dynamic memory allocation
	\COMMON\ED.C					; Line editor used when running D3B interactively
	\COMMON\ERRSTUF.C			; Error message support functions
	\COMMON\FILECMDS.C		; Commands used for file manipulation
	\COMMON\GENCMDS.C			; Hodge Podge of commands
	\COMMON\HELPCMD.C			; Help Command
	\COMMON\NUMBCMDS.C 		; Functions to perform Math functions
	\COMMON\PARPRAMS.C		; Parser for Command parameters
	\COMMON\PARSER.C			; Parser for Command itself
	\COMMON\SCRNCMDS.C		; Commands which control the video
	\COMMON\STRCMDS.C			; Commands for string manipulation
	\COMMON\TERMCMDS.C		; Commands which exit D3B
	\COMMON\VIDEO.C				; Video support functions

\COMMON\INCLUDE
	\COMMON\INCLUDE\GLOBALS.H ; Declaration for global variables
	\COMMON\INCLUDE\PROTOS.H  ; Function Prototypes

\COMMON\ASMS
	\COMMON\ASMS\SCRNLIB.ASM  ; Video write function
	\COMMON\ASMS\SCRNMACS.INC	; Macros used for direct write
	\COMMON\ASMS\SCRNMACB.INC ; Macros used for Bios write

\D3B
	\D3B\BUFCMDS.C  ; Commands used to store incoming data in buffers
	\D3B\BUFRCV.C   ; Functions concerned with protocol buffering for receive
	\D3B\BUFSEND.C	; Functions concerned with protocol buffering for send
	\D3B\CONNCMDS.C ; Commands concerned with establishing a connection
	\D3B\GETDTA.C		; Functions involved with getting boot up data from disk
	\D3B\GLOBDATA.C ; Global data used in D3B
	\D3B\LISEND.C		; High Level Send function
	\D3B\MAIN.C			; The C function Main and some support functions
	\D3B\MDMSTART.C ; Modem initialization functions
	\D3B\RCV.C			; High level receive functions
	\D3B\REMSTUF.C	; Code for implementing remote commands
	\D3B\SENDCMDS.C	; Commands involved with sending data to the other computer
	\D3B\STATSTUF.C	; Commands involved with compiling statistics
	\D3B\D3B.PRJ		; The project file

\D3B\INCLUDE
	\D3B\INCLUDE\TYPEDEFS.H  ; C type definitions
	\D3B\INCLUDE\PROTOS.H		 ; Function prototypes for D3B stuff
	\D3B\INCLUDE\TABLES.H		 ; ASCII/EBCDIC Conversion tables
	\D3B\INCLUDE\COMMANDS.H	 ; Script file command array
	\D3B\INCLUDE\RAGSTRS.H	 ; Ragged arrays, error and other messages
	\D3B\INCLUDE\ENUMS.H		 ; Enumerated data types
	\D3B\INCLUDE\D3.H				 ; Include for which has include statements in it
	\D3B\INCLUDE\DISPVARS.H	 ; Display (updated onscreen) variables structures
	\D3B\INCLUDE\GLOBALS.H	 ; Global variables in D3B
	\D3B\INCLUDE\SCRNS.H		 ; Screen built into D3B
	\D3B\INCLUDE\MAXES.H		 ; Miscellaneous constants used in the code
	\D3B\INCLUDE\USERVARS.H	 ; Variables the user can access in the scripts
	\D3B\INCLUDE\ALIASES.H	 ; C macros
		NOTE: Macros and Defines are generally capitalized to distinguish them
					from functions and variables without having to look them up.

\D3B\ASMS
	\D3B\ASMS\INTRUPTS.ASM ; Low level modem interface
	\D3B\ASMS\COMERRS.INC	 ; Communication error macros
	\D3B\ASMS\CTRLRCV.INC	 ; Receive for control information (remote commands)
	\D3B\ASMS\CTRLSEND.INC ; Send for control information (remote commands)
	\D3B\ASMS\DATA.INC		 ; Data used in drivers
	\D3B\ASMS\ENDINTS.INC	 ; Cleanup for the end of the interrupt
	\D3B\ASMS\EQUATES.INC	 ; Defined constants
	\D3B\ASMS\STANRCV.INC	 ; Standard receive macros
	\D3B\ASMS\STANSEND.INC ; Standard send macros
	\D3B\ASMS\STATINTS.INC ; Status interrupt macros
	\D3B\ASMS\TRANSEND.INC ; Transparent send macros
	\D3B\ASMS\TRANSRCV.INC ; Transparent receive macros
	\D3B\ASMS\GLOBALS.INC	 ; Global variables (used in C also)
	\D3B\ASMS\CRITMACS.INC ; Critical error macros
	\D3B\ASMS\STAND.INC	   ; Commonly used standard macros
	\D3B\ASMS\MDMRCV.INC	 ; Macros used to communicate with the modem itself
