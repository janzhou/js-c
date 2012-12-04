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

function text_replace(text){
	text = text.replace(/;/ig, ';;');
	text = text.replace(/ /ig, '; ');
	return text;
}

$(document).ready(function(){
	$( "#tabs" ).tabs();
});

interval = 2000;