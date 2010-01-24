/*

   "Resize Posted Images Based on Max Width" 2.4.5
   A phpBB MOD originally created by Christian Fecteau.

   This MOD is copyright (c) Christian Fecteau 2004-2005

   This MOD is released under the Creative Commons licence:
   http://creativecommons.org/licenses/by-nc-sa/2.0/
   Read carefully this licence before making any use of my code.

   Credits must be given with my full name (Christian Fecteau)
   and a link to my portfolio: http://portfolio.christianfecteau.com/

   Removal or alteration of this notice is strongly prohibited.

*/

// don't change anything below

function rmw_go()
{
	var rmw_img_array = document.getElementsByTagName("IMG");
	for (var i = 0; i < rmw_img_array.length; i++)
	{
		var rmw_img = rmw_img_array[i];
		if (String(rmw_img.getAttribute('resizemod')) == 'on')
		{
			if (rmw_wait_for_width && rmw_img.width && !isNaN(rmw_img.width))
			{
				if (rmw_img.width > Number(rmw_max_width))
				{
					rmw_img.setAttribute('resizemod','off');
					rmw_img.onload = null;
					rmw_img.removeAttribute('onload');
					var rmw_clone = rmw_img.cloneNode(false);
					var rmw_parent = rmw_img.parentNode;
					rmw_clone.setAttribute('width',String(rmw_max_width));
					rmw_parent.replaceChild(rmw_clone,rmw_img);
					rmw_make_pop(rmw_clone);
				}
			}
			else if (!rmw_wait_for_width)
			{
				rmw_img.setAttribute('resizemod','off');
				var rmw_clone = rmw_img.cloneNode(false);
				rmw_img.onload = null;
				rmw_img.removeAttribute('onload');
				var rmw_parent = rmw_img.parentNode;
				var rmw_ind = rmw_count++;
				rmw_clone.setAttribute('resizemod',String(rmw_ind));
				rmw_preload[rmw_ind] = new Image();
				rmw_preload[rmw_ind].src = rmw_img.src;
				if (window.showModelessDialog)
				{
					rmw_clone.style.margin = '2px';
				}
				rmw_clone.style.border = rmw_border_1;
				rmw_clone.style.width = '28px';
				rmw_parent.replaceChild(rmw_clone,rmw_img);
			}
		}
	}
	if (!rmw_over && document.getElementById('resizemod'))
	{
		rmw_over = true;
		rmw_go();
	}
	else if (!rmw_over)
	{
		window.setTimeout('rmw_go()',2000);
	}
}
function rmw_img_loaded(rmw_obj)
{
	if (!document.getElementsByTagName || !document.createElement) {return;}
	var rmw_att = String(rmw_obj.getAttribute('resizemod'));
	var rmw_real_width = false;
	if ((rmw_att != 'on') && (rmw_att != 'off'))
	{
		var rmw_index = Number(rmw_att);
		if (rmw_preload[rmw_index].width)
		{
			rmw_real_width = rmw_preload[rmw_index].width;
		}
	}
	else
	{
		rmw_obj.setAttribute('resizemod','off');
		if (rmw_obj.width)
		{
			rmw_real_width = rmw_obj.width;
		}
	}
	if (!rmw_real_width || isNaN(rmw_real_width) || (rmw_real_width <= 0))
	{
		var rmw_rand1 = String(rmw_count++);
		eval("rmw_retry" + rmw_rand1 + " = rmw_obj;");
		eval("window.setTimeout('rmw_img_loaded(rmw_retry" + rmw_rand1 + ")',2000);");
		return;
	}
	if (rmw_real_width > Number(rmw_max_width))
	{
		if (window.showModelessDialog)
		{
			rmw_obj.style.margin = '2px';
		}
		rmw_make_pop(rmw_obj);
	}
	else if (!rmw_wait_for_width)
	{
		rmw_obj.style.width = String(rmw_real_width) + 'px';
		rmw_obj.style.border = '0';
		if (window.showModelessDialog)
		{
			rmw_obj.style.margin = '0px';
		}
	}
	if (window.ActiveXObject) // IE on Mac and Windows
	{
		window.clearTimeout(rmw_timer1);
		rmw_timer1 = window.setTimeout('rmw_refresh_tables()',10000);
	}
}
function rmw_refresh_tables()
{
	var rmw_tables = document.getElementsByTagName("TABLE");
	for (var j = 0; j < rmw_tables.length; j++)
	{
		rmw_tables[j].refresh();
	}
}
function rmw_make_pop(rmw_ref)
{
	rmw_ref.style.border = rmw_border_2;
	rmw_ref.style.width = String(rmw_max_width) + 'px';
	if (!window.opera)
	{
		rmw_ref.onclick = function()
		{
			if (!rmw_pop.closed)
			{
				rmw_pop.close();
			}
			rmw_pop = window.open('about:blank','christianfecteaudotcom',rmw_pop_features);
			rmw_pop.resizeTo(window.screen.availWidth,window.screen.availHeight);
			rmw_pop.moveTo(0,0);
			rmw_pop.focus();
			rmw_pop.location.href = this.src;
		}
	}
	else
	{
		var rmw_rand2 = String(rmw_count++);
		eval("rmw_pop" + rmw_rand2 + " = new Function(\"rmw_pop = window.open('" + rmw_ref.src + "','christianfecteaudotcom','" + rmw_pop_features + "'); if (rmw_pop) {rmw_pop.focus();}\")");
		eval("rmw_ref.onclick = rmw_pop" + rmw_rand2 + ";");
	}
	document.all ? rmw_ref.style.cursor = 'hand' : rmw_ref.style.cursor = 'pointer';
	rmw_ref.title = rmw_image_title;
	if (window.showModelessDialog)
	{
		rmw_ref.style.margin = '0px';
	}
}
if (document.getElementsByTagName && document.createElement) // W3C DOM browsers
{
	rmw_preload = new Array();
	if (window.GeckoActiveXObject || window.showModelessDialog) // Firefox, NN7.1+, and IE5+ for Win
	{
		rmw_wait_for_width = false;
	}
	else
	{
		rmw_wait_for_width = true;
	}
	rmw_pop_features = 'top=0,left=0,width=' + String(window.screen.width-80) + ',height=' + String(window.screen.height-190) + ',scrollbars=1,resizable=1';
	rmw_over = false;
	rmw_count = 1;
	rmw_timer1 = null;
	if (!window.opera)
	{
		rmw_pop = new Object();
		rmw_pop.closed = true;
		rmw_old_onunload = window.onunload;
		window.onunload = function()
		{
			if (rmw_old_onunload)
			{
				rmw_old_onunload();
				rmw_old_onunload = null;
			}
			if (!rmw_pop.closed)
			{
				rmw_pop.close();
			}
		}
	}
	window.setTimeout('rmw_go()',2000);
}
