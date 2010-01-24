
function s4upl() {
var l=200;
var s4u=new String();
if (screen)
 {if (screen.width && screen.colorDepth) s4u+='&s='+screen.width+'x'+screen.height+'x'+screen.colorDepth;}
s4u+="&r=";
if(typeof Error != 'undefined') {
eval("try {if (top && top.document && top.document.referrer) {s4u+=escape(top.document.referrer.substring(0, l))} } catch(exception) {}");
} else if(top.document.referrer) {s4u+=escape(top.document.referrer.substring(0, l));}
return s4u;
}
var pp_gemius_identifier = new String('ApWVTa7TSYfR7O1usANHQ8Q87xEyGoNK9tUgP8eIrkn.X7');
document.writeln('<scr'+'ipt type="text/javascript" src="http://idm.hit.gemius.pl/pp_gemius.js"></scr'+'ipt>');