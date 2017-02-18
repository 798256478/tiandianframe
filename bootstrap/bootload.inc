<?php
require_once "functions.inc"; //全局函数

/**
 * 类的自动加载
 */
function _plum_auto_loader($classname) 
{
	$arr = explode('_', $classname);
	if('libs' == strtolower($arr[0])) {
		$class = array_pop($arr);
		array_shift($arr);//移除前缀
		$path = '/'.strtolower(implode('/', $arr)).'/';
		if(!class_exists($classname)) {
			@include_once PLUM_DIR_LIB . $path . $class . '.php';
		}
	} else if('app' == strtolower($arr[0])) {
		$class = array_pop($arr);
		array_shift($arr);
		$path = '/'.strtolower(implode('/', $arr)).'/';
		if(!class_exists($classname)) {
			$data = @include_once PLUM_DIR_APP . $path . $class . '.php';
		}
	}
}
spl_autoload_register('_plum_auto_loader');

/**
 * 映射api调用
 * @return json 
 */
function plum_api_invoke() 
{
	$map   = isset($_REQUEST['map']) ? trim($_REQUEST['map']) : '';
	$table = plum_parse_config('api', 'rewrite');
	$flag  = true;
	if(isset($table[$map])) {
		try {
			$class = new $table[$map]['controller'];
			$method = $table[$map]['action'];
			$class->$method();
		} catch (Exception $e) {
			$flag = false;
			trigger_error("api rewrite mapping undifined controller:
			{$table['$map']['controller']}, action: {$method}", E_USER_ERROR);
		}
	} else {
		$flag = false;
		trigger_error("call undifined api rewrite mapping: {$map}", E_USER_WARNING);
	}
	if(!$flag) {
		$json = array(
			'ec' => 500,
			'em' => 'api rewrite error'
		);
		echo json_encode($json);
	}
}

/**
 * 开启自定义session
 */
function plum_api_session_init() 
{
	$session_id = plum_get_param('session_id');
	if($session_id) {
		session_id($session_id);
	}
	$sessionHandler = new Libs_Session_SessionHandler();
	$sessionHandler->run();	
}

/**
 * 加密形式开启session
 * @param  string $type 配置名
 */
function plum_auth_session($type = 'auth') 
{
	$session_config = plum_parse_config($type, 'session');
	global $APP_USER;
	$APP_USER = 0;
	$session = plum_get_param($session_config['cookie_name']);
	$session = rawurldecode(urlencode($session));//先转码再解码;
	if($session) {
		//获取session，开始解密
		$code = unserialize(plum_authcode($session, 'DECODE', $session['auth_token']));

		if(is_array($code) && isset($code[$session_config['uid_key']])) {
			$uid = intval($code[$session_config['uid_key']]);
			if($uid) {
				$APP_USER = $uid;
			}
		}
	}
}

//API调用，开启session，保存类型为文件
function plum_open_session($type = PLUM_WEBAPP_MODULE) 
{
    //获取session配置
    $session_config     = plum_parse_config($type, 'session');
    $is_set_config      = $session_config ? true : false;
    $session_config     = $is_set_config ? $session_config : plum_parse_config(PLUM_WEBAPP_MODULE, 'session');

    $save_path          = PLUM_DIR_SESSION . '/' . $type;
    if (!file_exists($save_path)) {
        $save_path = mkdir($save_path) ? $save_path : $session_config['save_path'];
    }
    $cookie_name = $is_set_config ? $session_config['cookie_name'] : 'plum_session_' . $type;
    $cookie_path = $is_set_config ? $session_config['path'] : '/' . $type;

    ini_set('session.use_cookies',      plum_check_array_key('use_cookies', $session_config, 1));
    ini_set('session.use_only_cookies', plum_check_array_key('use_only_cookies', $session_config, 1));
    $lottery    = plum_check_array_key('lottery', $session_config, array(2, 100));
    ini_set('session.gc_probability',   $lottery[1]);
    ini_set('session.gc_divisor',       $lottery[0]);
    ini_set('session.name',             $cookie_name);
    $lifetime   = plum_check_array_key('lifetime', $session_config, 120) * 60;
    ini_set('session.gc_maxlifetime',   $lifetime);
    ini_set('session.save_path',        $save_path);
    $expire     = plum_check_array_key('expire_on_close', $session_config, true);
    $expire     = $expire ? 0 : $lifetime;
    ini_set('session.cookie_lifetime',  $expire);
    ini_set('session.cookie_path',      $cookie_path);
    $domain     = plum_check_array_key('domain', $session_config, null);
    if ($domain) ini_set('session.cookie_domain',   $domain);
    ini_set('session.cookie_secure',    plum_check_array_key('secure', $session_config, 0));
    //@session_start();

    //$session_handler = new Libs_Session_FileHandler();
    //$session_handler->run();

    plum_app_user_init();
}

function plum_web_session_init() 
{
    $sessionHandler = new Libs_Session_SessionHandler();
    $sessionHandler->run();
}

//---------------------------App User逻辑处理-----------------------------------
/**
 * app user 初始化
 */
function plum_app_user_init() 
{
	global $APP_USER;
	$APP_USER = isset($_SESSION['uid']) && $_SESSION['uid'] ? intval($_SESSION['uid']) : 0;
}

/**
 * app user登录并设置$_SESSION
 * @param  int $uid 用户id
 * @return int
 */
function plum_app_user_login($uid) 
{
	global $APP_USER;
	$uid = intval($uid);
	if($uid > 0) {
		$APP_USER = $uid;
		$_SESSION['uid'] = $uid;
		return $uid;
	}
	return 0;
}

/**
 * app user 是否登录并返回登录用户id
 * @return int
 */
function plum_app_user_islogin() 
{
	global $APP_USER;
	return $APP_USER > 0 ? $APP_USER : 0;
}

/**
 * app user 退出并清空session
 */
function plum_app_user_logout() 
{
	global $APP_USER;
	$APP_USER        = 0;
	$_SESSION['uid'] = 0;
	$_SESSION        = array();

	session_destroy();
}

//注册错误处理函数
new Libs_Log_Error();

class DB extends Libs_Mysql_PlumDatabase {}

$db_config = plum_parse_config('default', 'mysql');

DB::init($db_config);

