<?php
/**
 * Created by PhpStorm.
 * User: thomas
 * Date: 16/1/6
 * Time: 下午7:35
 */
//非控制台调用直接退出
if (php_sapi_name() != 'cli') {
    die('请在控制台下执行脚本');
}
set_time_limit(0);
ini_set('memory_limit', '128M');
define('DRUPAL_ROOT',       realpath(dirname(__FILE__).'/../'));

require_once DRUPAL_ROOT."/inc.php";
define('PLUM_ENV',          PLUM_DEVELOPMENT_ENV);          //设置当前为开发环境，自动识别测试环境
require_once PLUM_DIR_BOOTSTRAP . '/bootload.inc';
/**
 * 应用分发
 * @author thomas<thomas@ikinvin.com>
 * @since 2014-03-29
 */
define('DEFAULT_CONTROLLER', 'index');
define('DEFAULT_ACTION', 'index');
define('DEFAULT_MODULE', 'console');
/**
 * @param null $module 获取默认模块
 */
$module     = DEFAULT_MODULE;
$controller = DEFAULT_CONTROLLER;
$action     = DEFAULT_ACTION;
if ($argc > 1) {
    $mca    = explode('/', $argv[1]);
    switch (count($mca)) {
        case 1 :
            $action     = strtolower(strtolower($mca[0]));
            break;
        case 2 :
            $controller = ucfirst(strtolower($mca[0]));
            $action     = strtolower($mca[1]);
            break;
        case 3 :
            $module     = ucfirst(strtolower($mca[0]));
            $controller = ucfirst(strtolower($mca[1]));
            $action     = strtolower($mca[2]);
            break;
        default :
            break;
    }
    for ($i = 2; $i < $argc; $i++) {
        $tmp = explode('=', $argv[$i]);
        $key = isset($tmp[0]) && trim($tmp[0]) ? trim($tmp[0]) : 'unknown';
        $val = isset($tmp[1]) && trim($tmp[1]) ? trim($tmp[1]) : '';
        $_REQUEST[$key] = $val;
    }
}
plum_console_distribute($module, $controller, $action);


function plum_console_distribute($module, $controller, $action) {
    $controller = 'App_Controller_'.ucfirst(strtolower($module)).'_'.ucfirst(strtolower($controller)).'Controller';
    $action = strtolower($action).'Action';
    //分发逻辑控制
    $flag = false;
    if (class_exists($controller)) {
        $ctClass = new $controller();
        if (method_exists($ctClass, $action)) {
            $flag = true;
            $ctClass->$action();
        }
    }
    if (!$flag) {
        die("controller={$controller};action={$action};404 not found\n");
    }
}

