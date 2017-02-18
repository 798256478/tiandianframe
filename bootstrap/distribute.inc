<?php
define('DEFAULT_CONTROLLER', 'index');
define('DEFAULT_ACTION', 'index');
define('DEFAULT_MODULE', 'site');

function plum_distribute($module = null) 
{
	global $base_url;
	$query = plum_parse_request_uri();
	$module = isset($module) && $module ? $module : (empty($query) ? DEFAULT_MODULE : trim(array_shift($query)));
	//设置基url
	$base_url = isset($_SERVER['PHP_SELF']) && trim($_SERVER['PHP_SELF'], '/') != 'index.php' ? '/' : '/' . $module . '/';

	$controller = empty($query) ? DEFAULT_CONTROLLER : trim(array_shift($query));
	$action = empty($query) ? DEFAULT_ACTION : trim(array_shift($query));
	while(!empty($query)) {
		$key = array_shift($query);
		$key = trim($key) ? trim($key) : 'unknown';
		$val = array_shift($query);
		$val = trim($val) ? trim($val) : '';
		$_REQUEST[$key] = $val;
	}

	$controller = 'App_Controller_'.ucfirst(strtolower($module)). '_' . ucfirst(strtolower($controller)).'Controller';
	$action = strtolower($action).'Action';
	$flag = false;
	if(class_exists($controller)) {
		$ctClass = new $controller();
		if(method_exists($ctClass, $action)) {
			$flag = true;
			$ctClass->$action();
		}
	}

	if(!$flag) {
		header('Location:/error/404.html');
	}
}