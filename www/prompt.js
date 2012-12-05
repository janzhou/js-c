promptId = 0;

$(document).ready(function(){
	$('body').append('<div id="prompt"></div>');
});

function delprompt(id){
	$(id).fadeOut(3000);
}

function prompt(html){
	id = promptId++;
	$('#prompt').append('<li id="prompt-'+id+'">'+html+'</li>');
	setTimeout("delprompt('#prompt-'+"+id+")", 3000);
}

function warning(html){
	id = promptId++;
	$('#prompt').append('<li id="prompt-'+id+'" class="warning">'+html+'</li>');
	setTimeout("delprompt('#prompt-'+"+id+")", 5000);
}