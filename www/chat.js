//���칦��
//��������
//text_replaceʵ��ת���ַ���������/��ת�壬ֻ��ѡ��;
//��Ҫ��;ת��;; �ѿո�ת��;+�ո�
//��Ӧ�ģ���c���򲿷ֻ�����������
//����Ϊ�˴�������ո���ı�
//text_replace��functions.js��ʵ��


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
