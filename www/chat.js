//聊天功能
//娱乐性质
//text_replace实现转义字符，不能用/做转义，只好选了;
//需要把;转成;; 把空格转成;+空格
//相应的，在c程序部分会解析这个功能
//这是为了处理包含空格的文本
//text_replace在functions.js中实现


$(document).ready(function(){
	$('#chat-input').keydown(function(event){
		if(event.which == 13){ 
			text = $('#chat-input').val();
			$('#chat-input').val('');
			do_cmd('c '+text_replace(text), function(json){});
		}
	});
	$('#chat-input').keyup(function(event){
		if(event.which == 13){
			$('#chat-input').val('');
		}
	});
});
