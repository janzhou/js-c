//实现do_cmd和界面初始化
//interval表示查询间隔

function do_cmd(cmd, callbackfunction){
	tmp = Math.random();
	buf = "{ tmp:'" + tmp+ "'}";
	$.ajax({
		type:"GET",
		url:'ajax.php?q='+cmd,  //ajax.php?q=+cmd相对路径,//为了兼容，而修改,2010.10.09
		dataType:"json",
		data:buf,
		success:callbackfunction,
		error:function(XMLHttpRequest,textStatus,errorThrown){
			alert(textStatus+"  "+errorThrown);
		}
	});
}

//	\n	\r
//	n	r
//	[	]	{	}	;	:	'	"	,	<		>	/	?
//	o	p	O	P	k	K	l	L	b	B		N	m	M
//	`~@#%^&=
//	qQ235678
function text_replace(text){
//alert(text);
//return text;
	text = text.replace(/!/ig, '!!');
	text = text.replace(/ /ig, '! ');
	
	text = text.replace(/\n/ig, '!n');
	text = text.replace(/\r/ig, '!r');
	
	text = text.replace(/\[/ig, '!o');
	text = text.replace(/]/ig, '!p');
	text = text.replace(/{/ig, '!O');
	text = text.replace(/}/ig, '!P');
	
	text = text.replace(/;/ig, '!k');
	text = text.replace(/:/ig, '!K');
	text = text.replace(/'/ig, '!l');
	text = text.replace(/\"/ig, '!L');
	
	text = text.replace(/,/ig, '!b');
	text = text.replace(/</ig, '!B');
	text = text.replace(/>/ig, '!N');
	text = text.replace(/\//ig, '!m');
	text = text.replace(/\?/ig, '!M');
	
	text = text.replace(/`/ig, '!q');
	text = text.replace(/~/ig, '!Q');
	
	text = text.replace(/@/ig, '!2');
	text = text.replace(/#/ig, '!3');
	text = text.replace(/%/ig, '!5');
	text = text.replace(/\^/ig, '!6');
	text = text.replace(/&/ig, '!7');
	
	text = text.replace(/=/ig, '!8');
	text = text.replace(/\+/ig, '!*');
	return text;
}

$(document).ready(function(){
	$( "#tabs" ).tabs();
});

interval = 1000;