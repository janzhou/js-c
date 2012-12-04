<?php
function run_php_cmd($cmd){

	$sockfd = connect_sock();  
	$out = '';

	socket_write($sockfd, $cmd, strlen($cmd));  
	
	if($out = socket_read($sockfd, 2048)){
		echo $out;
	}
	socket_close($sockfd); 
}

function connect_sock(){
	$service_port = 2012;
	$address = "127.0.0.1";
  	$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);   
	if ($socket >= 0)   
	{
		$result = socket_connect($socket, $address, $service_port);   
		if ($result >= 0)   
			return $socket;
	}
}
?>


