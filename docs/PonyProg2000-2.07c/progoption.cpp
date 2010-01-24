//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id: progoption.cpp,v 1.3 2008/01/06 17:34:42 lancos Exp $
//-------------------------------------------------------------------------//
//                                                                         //
// This program is free software; you can redistribute it and/or           //
// modify it under the terms of the GNU  General Public License            //
// as published by the Free Software Foundation; either version2 of        //
// the License, or (at your option) any later version.                     //
//                                                                         //
// This program is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of          //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
// General Public License for more details.                                //
//                                                                         //
// You should have received a copy of the GNU  General Public License      //
// along with this program (see COPYING);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//-------------------------------------------------------------------------//
//=========================================================================//

//=======================================================================
//@V@:Note: This file generated by vgen V1.07 (12:35:57 03 Oct 1999).
//	fusemdlg.cpp:	Source for progOptionDialog class
//=======================================================================

#include <stdio.h>

#include "progoption.h"
#include <v/vnotice.h>

#include "eeptypes.h"
#include "e2app.h"
#include "modaldlg_utilities.h"

//@V@:BeginIDs
enum {
	lblMainMsg = 5000,

	frmFuses,

	chk0,
	chk1,
	chk2,
	chk3,
	chk4,
	chk5,
	chk6,
	chk7,
	chk8,
	chk9,
	chk10,
	chk11,
	chk12,
	chk13,
	chk14,
	chk15,

	blk1,
	blk2
};
//@V@:EndIds

#define	N_OPTIONS	10

#define	idRELOAD	(chk0+RELOAD_idx)
#define	idREADFLASH	(chk0+READFLASH_idx)
#define	idREADEEP	(chk0+READEEP_idx)
#define	idBYTESWAP	(chk0+BYTESWAP_idx)
#define	idSETID		(chk0+SETID_idx)
#define idREADOSCCAL (chk0+READOSCCAL_idx)
#define	idERASE		(chk0+ERASE_idx)
#define	idFLASH		(chk0+FLASH_idx)
#define	idEEPROM	(chk0+EEPROM_idx)
#define	idLOCK		(chk0+LOCK_idx)

//@V@:BeginDialogCmd DefaultCmds
static DialogCmd ProgDialogCmds[] =
{
	{C_Label, lblMainMsg, 0,"X",NoList,CA_MainMsg,isSens,NoFrame, 0, 0},

	{C_Frame, frmFuses,0,"Fuses",NoList,CA_None,isSens,NoFrame,0,lblMainMsg},

	{C_CheckBox, idRELOAD, 0,STR_MSGRELOAD,NoList,CA_None,isSens,frmFuses,0,0},
	{C_CheckBox, idREADFLASH, 0,STR_MSGREADPROG,NoList,CA_None,isSens,frmFuses,0,idRELOAD},
	{C_CheckBox, idREADEEP, 0,STR_MSGREADDATA,NoList,CA_None,isSens,frmFuses,0,idREADFLASH},
	{C_CheckBox, idBYTESWAP, 0,STR_MSGBYTESWAP,NoList,CA_None,isSens,frmFuses,0,idREADEEP},
	{C_CheckBox, idSETID, 0,STR_MSGSERNUM,NoList,CA_None,isSens,frmFuses,0,idBYTESWAP},
	{C_CheckBox, idREADOSCCAL, 0,STR_READOSCCALIB,NoList,CA_None,isSens,frmFuses,0,idSETID},
	{C_CheckBox, idERASE, 0,STR_MSGERASE,NoList,CA_None,isSens,frmFuses,0,idREADOSCCAL},
	{C_CheckBox, idFLASH, 0,STR_MSGWRITEPROG,NoList,CA_None,isSens,frmFuses,0,idERASE},
	{C_CheckBox, idEEPROM, 0,STR_MSGWRITEDATA,NoList,CA_None,isSens,frmFuses,0,idFLASH},
	{C_CheckBox, idLOCK, 0,STR_MSGWRITESEC,NoList,CA_None,isSens,frmFuses,0,idEEPROM},

	{C_Blank, blk1, 0, " ",NoList,CA_None,isSens,NoFrame,0,frmFuses},

	{C_Button,M_Cancel,0, STR_BTNCANC, NoList,CA_None,isSens,NoFrame,0,blk1},
	{C_Button,M_OK,0, STR_BTNOK, NoList,CA_DefaultButton,isSens,NoFrame,M_Cancel,blk1},

	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
};
//@V@:EndDialogCmd


//======================>>> progOptionDialog::progOptionDialog <<<==================
progOptionDialog::progOptionDialog(vBaseWindow* bw, char* title) :
				vModalDialog(bw, title)
{
	UserDebug(Constructor,"progOptionDialog::progOptionDialog()\n")

	cmdw = (e2CmdWindow *)bw;
}

//===================>>> progOptionDialog::~progOptionDialog <<<====================
progOptionDialog::~progOptionDialog()
{
	UserDebug(Destructor,"progOptionDialog::~progOptionDialog() destructor\n")
}

//====================>>> progOptionDialog::progAction <<<====================
int progOptionDialog::progAction(char* msg, long type, int &reload, int &reep, int &erase, int &flash, int &eeprom, int &lock)
{
	int prog_opt = THEAPP->GetProgramOptions();
	int k;

	//Checked
	for (k = 0; k < N_OPTIONS; k++)
	{
		SetCommandObject(chk0+k, (prog_opt & (1<<k)) ? 1 : 0, ProgDialogCmds);
	}

	AddDialogCmds(ProgDialogCmds);		//add commands to dialog

	ItemVal ans,rval;

	ans = ShowModalDialog(msg,rval);
	if (ans == M_Cancel)
		return 0;

	// *** Add code to process dialog values here
	prog_opt = 0;
	for (k = 0; k < N_OPTIONS; k++)
	{
		prog_opt |= GetValue(chk0+k) ? (1<<k) : 0;
	}

	if (ans == M_OK)
	{
		THEAPP->SetProgramOptions(prog_opt);
		return 1;
	}
	else
		return 0;
}

//====================>>> progOptionDialog::DialogCommand <<<====================
void progOptionDialog::DialogCommand(ItemVal id, ItemVal retval, CmdType ctype)
{
	UserDebug2(CmdEvents,"progOptionDialog::DialogCommand(id:%d, val:%d)\n",id, retval)

	vModalDialog::DialogCommand(id,retval,ctype);
}
