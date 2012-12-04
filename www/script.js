//向测试机发送脚本
//脚本送达c后，c再把脚本送到每个测试机




$(document).ready(function(){
	$('#script-broadcast').click(function(){
		if($('#script-port').val() == ''){
			warning("没有端口号");
			return;
		}
		if($('#script-input').val() == ''){
			warning("空脚本");
			return;
		}
		do_cmd('wb '+$('#script-port').val()+' '+text_replace($('#script-input').val()), function(json){});
	});

	$('#script-submit').click(function(){
		if($('#script-input').val() == ''){
			warning("空脚本");
			return;
		}
		do_cmd('scriptsend '+text_replace($('#script-input').val()), function(json){});
	});

	$('#script-tcp').click(function(){
		if($('#script-port').val() == ''){
			warning("没有IP");
			return;
		}
		if($('#script-port').val() == ''){
			warning("没有端口号");
			return;
		}
		if($('#script-input').val() == ''){
			warning("空脚本");
			return;
		}
		do_cmd('wt '+$('#script-ip').val()+' '+$('#script-port').val()+' '+text_replace($('#script-input').val()), function(json){});
	});
	
	$('#script-udp').click(function(){
		if($('#script-port').val() == ''){
			warning("没有IP");
			return;
		}
		if($('#script-port').val() == ''){
			warning("没有端口号");
			return;
		}
		if($('#script-input').val() == ''){
			warning("空脚本");
			return;
		}
		do_cmd('wu '+$('#script-ip').val()+' '+$('#script-port').val()+' '+text_replace($('#script-input').val()), function(json){});
	});
});

