<?php
	header("content-type:text/html; charset=utf-8");
	include_once('functions.php');
	
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
