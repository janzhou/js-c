<?php
	include_once('config.php');

	function connect_sock($server_ip, $server_port){
		$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);  
		if ($socket >= 0){
			$result = socket_connect($socket, $server_ip, $server_port);  
			if ($result >= 0)
				return $socket;
		}
	}

	function run_php_cmd($cmd){
		global $server_ip, $server_port;

		$sockfd = connect_sock($server_ip, $server_port);  
		$out = '';

		socket_write($sockfd, $cmd, strlen($cmd));  
		
		if($out = socket_read($sockfd, 2048)){
			echo $out;
		}
		socket_close($sockfd); 
	}
	
	header("content-type:text/html; charset=utf-8");
	
	if (isset($_GET['q'])) {
		$cmd = $_GET['q'];
	}
	else if (isset($_GET['cmd'])) {
		$cmd = $_GET['cmd'];
	}
	else $cmd = "ajax";

	if (isset($_GET['callback'])) {
		$callback = $_GET['callback'];
		$callback .= '(';
		echo($callback);
	}
	run_php_cmd($cmd);

	if (isset($_GET['callback'])) {
		echo(')');
	}
?>
