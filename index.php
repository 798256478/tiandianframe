<?php
define('DRUPAL_ROOT', dirname(__FILE__));//定义根目录绝对路径，如：D:\wampstack\apache2\htdocs\tdframe
require_once "inc.php";//定义系统常量
define('PLUM_ENV', PLUM_DEVELOPMENT_ENV);
require_once PLUM_DIR_BOOTSTRAP . '/bootload.inc';//引导文件

$query        = plum_parse_request_uri();
$prefix       = array_shift($query);
$admin_prefix = array('admin', 'user', 'js');//管理端路径前缀
$front_prefix = array('manage', 'member'); //前端路径前缀

if(in_array($prefix, $admin_prefix)) {
	require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
	drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
	menu_execute_active_handler();
	drupal_load('module', 'common');
} else {
	require_once PLUM_DIR_BOOTSTRAP . '/distribute.inc';
	$session_cfg = plum_get_config('session');
	$session_type = isset($session_cfg[$prefix]) ? $prefix : PLUM_WEBAPP_MODULE;

	$module_type = in_array($prefix, $front_prefix) ? null : PLUM_WEBAPP_MODULE;

	plum_open_session($session_type);
	plum_distribute($module_type);
}