var imageTag = false;
var theSelection = false;
var clientPC = navigator.userAgent.toLowerCase();
var clientVer = parseInt(navigator.appVersion);
var is_ie = ((clientPC.indexOf("msie") != -1) && (clientPC.indexOf("opera") == -1));
var is_nav = ((clientPC.indexOf('mozilla')!=-1) && (clientPC.indexOf('spoofer')==-1)
		&& (clientPC.indexOf('compatible') == -1) && (clientPC.indexOf('opera')==-1)
		&& (clientPC.indexOf('webtv')==-1) && (clientPC.indexOf('hotjava')==-1));
var is_moz = 0;
var is_win = ((clientPC.indexOf("win")!=-1) || (clientPC.indexOf("16bit") != -1));
var is_mac = (clientPC.indexOf("mac")!=-1);

bbcode = new Array();
bbtags = new Array('[b]','[/b]','[i]','[/i]','[u]','[/u]','[quote]','[/quote]','[code]','[/code]','[list]','[/list]','[list=]','[/list]','[img]','[/img]','[url]','[/url]','[stream]','[/stream]','[fade]','[/fade]','[scroll]','[/scroll]','[swf width= height=]','[/swf]','[center]','[/center]','[hide]','[/hide]','[youtube]','[/youtube]');
imageTag = false;

function helpline(help)
{
	document.post.helpbox.value = eval(help + "_help");
}

function getarraysize(thearray)
{
	for (i = 0; i < thearray.length; i++)
	{
		if ((thearray[i] == "undefined") || (thearray[i] == "") || (thearray[i] == null))
			return i;
	}
	return thearray.length;
}

function arraypush(thearray,value)
{
	thearray[ getarraysize(thearray) ] = value;
}

function arraypop(thearray)
{
	thearraysize = getarraysize(thearray);
	retval = thearray[thearraysize - 1];
	delete thearray[thearraysize - 1];
	return retval;
}

function bbfontstyle(bbopen, bbclose)
{
	var txtarea = document.post.message;
	if ((clientVer >= 4) && is_ie && is_win)
	{
		theSelection = document.selection.createRange().text;
		if (!theSelection)
		{
			txtarea.value += bbopen + bbclose;
			txtarea.focus();
			return;
		}
		document.selection.createRange().text = bbopen + theSelection + bbclose;
		txtarea.focus();
		return;
	}
	else if (txtarea.selectionEnd && (txtarea.selectionEnd - txtarea.selectionStart > 0))
	{
		mozWrap(txtarea, bbopen, bbclose);
		return;
	}
	else
	{
		txtarea.value += bbopen + bbclose;
		txtarea.focus();
	}
	storeCaret(txtarea);
}

function bbstyle(bbnumber)
{
	var txtarea = document.post.message;

	txtarea.focus();
	donotinsert = false;
	theSelection = false;
	bblast = 0;

	if (bbnumber == -1)
	{
		while (bbcode[0])
		{
			butnumber = arraypop(bbcode) - 1;
			txtarea.value += bbtags[butnumber + 1];
			buttext = eval('document.post.addbbcode' + butnumber + '.value');
			eval('document.post.addbbcode' + butnumber + '.value ="' + buttext.substr(0,(buttext.length - 1)) + '"');
		}
		imageTag = false;
		txtarea.focus();
		return;
	}

	if ((clientVer >= 4) && is_ie && is_win)
	{
		theSelection = document.selection.createRange().text;
		if (theSelection)
		{
			document.selection.createRange().text = bbtags[bbnumber] + theSelection + bbtags[bbnumber+1];
			txtarea.focus();
			theSelection = '';
			return;
		}
	}
	else if (txtarea.selectionEnd && (txtarea.selectionEnd - txtarea.selectionStart > 0))
	{
		mozWrap(txtarea, bbtags[bbnumber], bbtags[bbnumber+1]);
		return;
	}
	
	for (i = 0; i < bbcode.length; i++)
	{
		if (bbcode[i] == bbnumber+1)
		{
			bblast = i;
			donotinsert = true;
		}
	}

	if (donotinsert)
	{
		while (bbcode[bblast])
		{
				butnumber = arraypop(bbcode) - 1;
				txtarea.value += bbtags[butnumber + 1];
				buttext = eval('document.post.addbbcode' + butnumber + '.value');
				eval('document.post.addbbcode' + butnumber + '.value ="' + buttext.substr(0,(buttext.length - 1)) + '"');
				imageTag = false;
			}
			txtarea.focus();
			return;
	}
	else
	{
		if (imageTag && (bbnumber != 14))
		{
			txtarea.value += bbtags[15];
			lastValue = arraypop(bbcode) - 1;
			document.post.addbbcode14.value = "Img";
			imageTag = false;
		}
		
		txtarea.value += bbtags[bbnumber];
		if ((bbnumber == 14) && (imageTag == false)) imageTag = 1;
		arraypush(bbcode,bbnumber+1);
		eval('document.post.addbbcode'+bbnumber+'.value += "*"');
		txtarea.focus();
		return;
	}
	storeCaret(txtarea);
}

function mozWrap(txtarea, open, close)
{
	var selLength = txtarea.textLength;
	var selStart = txtarea.selectionStart;
	var selEnd = txtarea.selectionEnd;
	if (selEnd == 1 || selEnd == 2) 
		selEnd = selLength;

	var s1 = (txtarea.value).substring(0,selStart);
	var s2 = (txtarea.value).substring(selStart, selEnd)
	var s3 = (txtarea.value).substring(selEnd, selLength);
	txtarea.value = s1 + open + s2 + close + s3;
	return;
}

function mpFoto(img)
{
	foto1= new Image();
	foto1.src=(img);
	mpControl(img);
}

function mpControl(img)
{
	if ( (foto1.width != 0) && (foto1.height != 0) )
	{
		viewFoto(img);
	}
	else
	{
		mpFunc = "mpControl('"+img+"')";
		intervallo = setTimeout(mpFunc,20);
	}
}

function viewFoto(img)
{
	largh = foto1.width + 20;
	altez = foto1.height + 20;
	string = "width="+largh+",height="+altez;
	finestra = window.open(img, "", string);
}

function setCheckboxes(theForm, elementName, isChecked)
{
	var chkboxes = document.forms[theForm].elements[elementName];
	var count = chkboxes.length;
	if ( count )
	{
		for (var i = 0; i < count; i++)
		{
			chkboxes[i].checked = isChecked;
		}
	}
	else
	{
		chkboxes.checked = isChecked;
	}
	return true;
}

var selectedText = quoteAuthor = '';

function quoteSelection()
{
	theSelection = false;
	if (selectedText) theSelection = selectedText;
	else if (document.selection && document.selection.createRange) theSelection = document.selection.createRange().text;

	if (theSelection)
	{
		emoticon( (document.post.message.value ? '' : '') + '[quote' + (quoteAuthor ? '="' + quoteAuthor + '"' : '') + ']' + theSelection + '[/quote]');
		document.post.message.focus();
		selectedText = quoteAuthor = theSelection = '';
		return;
	}
	else
	{
		alert(no_text_selected);
	}
}

function displayWindow(url, width, height)
{
	var Win = window.open(url,"displayWindow",'width=' + width + ',height=' + height + ',resizable=1,scrollbars=no,menubar=no' );
}

function hideLoadingPage()
{
	if (document.getElementById)
	{
		 document.getElementById('hidepage').style.visibility = 'hidden';
	}
	else
	{
		if (document.layers)
		{
			document.hidepage.visibility = 'hidden';
		}
		else
		{
			document.all.hidepage.style.visibility = 'hidden';
		}
	}
}

function Active(what)
{
	what.style.backgroundColor=factive_color;
}

function NotActive(what)
{
	what.style.backgroundColor='';
}

function storeCaret(textEl)
{
	if (textEl.createTextRange) textEl.caretPos = document.selection.createRange().duplicate();
}

function emoticon(text)
{
	if (document.post.message.createTextRange && document.post.message.caretPos)
	{
		var caretPos = document.post.message.caretPos;
		caretPos.text = caretPos.text.charAt(caretPos.text.length - 1) == ' ' ? text + ' ' : text;
		document.post.message.focus();
	}
	else if (document.post.message.selectionStart != undefined)
	{
		document.post.message.value = document.post.message.value.substring(0, document.post.message.selectionStart) + text + document.post.message.value.substring(document.post.message.selectionStart);
		document.post.message.focus();
	}
	else
	{
		document.post.message.value += text;
		document.post.message.focus();
	}
}

function em(text)
{
	return emoticon(text);
}

function checkForm()
{
	formErrors = false;
	if (document.post.message.value.length < 2)
	{
		formErrors = l_empty_message;
	}

	if (formErrors)
	{
		alert(formErrors);
		return false;
	}
	else
	{
		bbstyle(-1);
		//formObj.preview.disabled = true;
		//formObj.submit.disabled = true;
		return true;
	}
}

function wrapSelection(h, strFore, strAft)
{
	h.focus();
	if (h.setSelectionRange)
	{
		var selStart = h.selectionStart, selEnd = h.selectionEnd;
		h.value = h.value.substring(0, selStart) + strFore + h.value.substring(selStart, selEnd) + strAft + h.value.substring(selEnd);
		h.setSelectionRange(selStart + strFore.length, selEnd + strFore.length);
	}
	else if (document.selection)
	{
		var oRange = document.selection.createRange();
		var numLen = oRange.text.length;
		oRange.text = strFore + oRange.text + strAft;
		//moveSelectionRange(oRange, -numLen, -strAft.length);
	}
	else
	{
		h.value += strFore + strAft;
	}
}

function imgcode(theform,imgcode,prompttext)
{
	tag_prompt = img_addr;
	inserttext = prompt(tag_prompt+"\n["+imgcode+"]xxx[/"+imgcode+"]",prompttext);
	if ((inserttext != null) && (inserttext != ""))
	theform.message.value += "["+imgcode+"]"+inserttext+"[/"+imgcode+"] ";
	theform.message.focus();
}

function namedlink(theform,thetype)
{
	linktext = prompt(link_text_prompt,"");
	var prompttext;
	if (thetype == "URL")
	{
		prompt_text = link_url_prompt;
		prompt_contents = "http://";
	}
	else
	{
		prompt_text = link_email_prompt;
		prompt_contents = "";
	}
	linkurl = prompt(prompt_text,prompt_contents);
	if ((linkurl != null) && (linkurl != ""))
	{
		if ((linktext != null) && (linktext != "")) theform.message.value += "["+thetype+"="+linkurl+"]"+linktext+"[/"+thetype+"] ";
		else theform.message.value += "["+thetype+"]"+linkurl+"[/"+thetype+"] ";
	}
	theform.message.focus();
}

function filter_freak()
{
	theSelection = document.selection.createRange().text;
	if (theSelection != '')
	{
		var caretPos = document.post.message.caretPos;
		var export_text = '';
		var current_char = '';
		for (i = 0; i <= caretPos.text.length; i++)
		{
			current_char = caretPos.text.charAt(i);
			if ( (i % 2) == 0 )
			{
				export_text += current_char.toUpperCase();
			}
			else
			{
				export_text += current_char.toLowerCase();
			}
		}
		caretPos.text = export_text;document.post.message.focus();
		return;
	}
	alert(no_text_selected);
	return;
}
			
function filter_l33t()
{
	theSelection = document.selection.createRange().text;
	if (theSelection != '')
	{
		var caretPos = document.post.message.caretPos;
		var export_text = '';
		var current_char = '';
		for (i = 0; i <= caretPos.text.length; i++)
		{
			current_char = caretPos.text.charAt(i);
			if ( (current_char == 'a') || (current_char == 'A') )
			{
				export_text += '4';
			}
			else if ( (current_char == 'e') || (current_char == 'E') )
			{
				export_text += '3';
			}
			else if ( (current_char == 'i') || (current_char == 'I') )
			{
				export_text += '1';
			}
			else if ( (current_char == 'o') || (current_char == 'O') )
			{
				export_text += '0';
			}
			else
			{
				export_text += current_char;
			}
		}
		caretPos.text = export_text;document.post.message.focus();
		return;
	}
	alert(no_text_selected);
	return;
}

function onv(element)
{
	element.style.backgroundColor=faonmouse_color;
}

function onv2(element)
{
	element.style.backgroundColor=faonmouse2_color;
}

function ont(element)
{
	element.style.backgroundColor='';
}

function focus_field(def_field)
{
	if (document.getElementById(def_field))
	{
		document.getElementById(def_field).focus();
	}
}

function show_pagina(e)
{
	var sTop = document.body.scrollTop;
	var sLeft = document.body.scrollLeft;
	document.getElementById('s_pagina').style.display='block';
	document.getElementById('s_pagina').style.left=e.clientX-35+sLeft;
	document.getElementById('s_pagina').style.top=e.clientY+sTop-20;
	return;
}

function qc()
{
	if (document.post && document.post.message)
	{
		quoteSelection();
		return false;
	}
}

function qo()
{
	selectedText = document.selection? document.selection.createRange().text : document.getSelection();
}

function qu(username)
{
	if (window.getSelection && window.getSelection() || document.selection && document.selection.createRange && document.selection.createRange().text)
	{
		return true;
	}
	else
	{
		return false;
	}
}

function cp(what)
{
	what.style.cursor='pointer';
}

var PreloadFlag = false;
var expDays = 90;
var exp = new Date(); 
var tmp = '';
var tmp_counter = 0;
var tmp_open = 0;

exp.setTime(exp.getTime() + (expDays*24*60*60*1000));

function SetCookie(name, value) 
{
	var argv = SetCookie.arguments;
	var argc = SetCookie.arguments.length;
	var expires = (argc > 2) ? argv[2] : null;
	document.cookie = cname + name + "=" + escape(value) +
		((expires == null) ? "" : ("; expires=" + expires.toGMTString())) +
		((cpath == null) ? "" : ("; path=" + cpath)) +
		((cdomain == null) ? "" : ("; domain=" + cdomain)) +
		((csecure == 1) ? "; secure" : "");
}

function getCookieVal(offset) 
{
	var endstr = document.cookie.indexOf(";",offset);
	if (endstr == -1)
	{
		endstr = document.cookie.length;
	}
	return unescape(document.cookie.substring(offset, endstr));
}

function GetCookie(name) 
{
	var arg = cname + name + "=";
	var alen = arg.length;
	var clen = document.cookie.length;
	var i = 0;
	while (i < clen) 
	{
		var j = i + alen;
		if (document.cookie.substring(i, j) == arg)
			return getCookieVal(j);
		i = document.cookie.indexOf(" ", i) + 1;
		if (i == 0)
			break;
	} 
	return null;
}

function ShowHide(id1, id2, id3) 
{
	// System to show/hide page elements, cookie based
	// Take from Morpheus style Created by Vjacheslav Trushkin (aka CyberAlien)
	var res = expMenu(id1);
	if (id2 != '') expMenu(id2);
	if (id3 != '') SetCookie(id3, res, exp);
}
	
function expMenu(id) 
{
	var itm = null;
	if (document.getElementById) 
	{
		itm = document.getElementById(id);
	}
	else if (document.all)
	{
		itm = document.all[id];
	} 
	else if (document.layers)
	{
		itm = document.layers[id];
	}
	if (!itm) 
	{
		// do nothing
	}
	else if (itm.style) 
	{
		if (itm.style.display == "none")
		{ 
			itm.style.display = "inline"; 
			return 1;
		}
		else
		{
			itm.style.display = "none"; 
			return 2;
		}
	}
	else 
	{
		itm.visibility = "show"; 
		return 1;
	}
}

function showMenu(id)
{
	var itm = null;
	if (document.getElementById) 
	{
		itm = document.getElementById(id);
	}
	else if (document.all)
	{
		itm = document.all[id];
	} 
	else if (document.layers)
	{
		itm = document.layers[id];
	}
	if (!itm) 
	{
		// do nothing
	}
	else if (itm.style) 
	{
		if (itm.style.display == "none")
		{ 
			itm.style.display = ""; 
			return true;
		}
		else
		{
//			itm.style.display = "none"; 
			return true;
		}
	}
	else 
	{
		itm.visibility = "show"; 
		return true;
	}
}

function hideMenu(id)
{
	var itm = null;
	if (document.getElementById) 
	{
		itm = document.getElementById(id);
	}
	else if (document.all)
	{
		itm = document.all[id];
	} 
	else if (document.layers)
	{
		itm = document.layers[id];
	}
	if (!itm) 
	{
		// do nothing
	}
	else if (itm.style) 
	{
		if (itm.style.display == "none")
		{ 
//			itm.style.display = ""; 
			return true;
		}
		else
		{
			itm.style.display = "none"; 
			return true;
		}
	}
	else 
	{
		itm.visibility = "hide"; 
		return true;
	}
}

function change_size(area_object, mode)
{
	var msg_cur_size = GetCookie('msg_size');
	var new_size = msg_cur_size;

	if ( mode == 1 )
	{
		new_size++;
		new_size2++;
		if ( new_size > 8 ) new_size = 8;
		if ( new_size2 > 8 ) new_size2 = 8;
		SetCookie('msg_size', new_size);
	}
	else if ( mode == -1 )
	{
		new_size = 0;
		new_size2 = 0;
		if ( new_size < 0 ) new_size = 0;
		if ( new_size2 < 0 ) new_size2 = 0;
		SetCookie('msg_size', new_size);
	}
	if (document.cookie.length) call_size = new_size;
	else call_size = new_size2;

	set_size(area_object, call_size);
}

function set_size(area_object, new_size)
{
	var default_width = 550;
	var default_height = 200;

	if ( new_size == "undefined" ) new_size = 0;

	area_object.style.width = (default_width + (30 * new_size)) + "px";
	area_object.style.height = (default_height + (100 * new_size)) + "px";
}

function chng(val)
{
    var nval = '#' + val.value;
    val.style.color = nval;
}
