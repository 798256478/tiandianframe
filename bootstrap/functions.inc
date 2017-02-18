<?php
define('DEFAULT_DIR_MODE', 0775); //定义默认文件权限
define('PLUM_AUTH_TOKEN', 'plum'); //默认的认证token

//----------------------------获取变量----------------------------------
/**
 * 根据变量名获取变量
 * @param  string $name    变量名
 * @param  string $default 默认值
 * @return string          返回变量的值
 */
function plum_get_param($name, $default = '') 
{
	return isset($_REQUEST[$name]) ? trim($_REQUEST[$name]) : $default;
}

/**
 * 根据变量名获取整型变量
 * @param  string $name    变量名
 * @param  int $default    默认值
 * @return int             返回变量的值
 */
function plum_get_int_param($name, $default = 0)
{
	return isset($_REQUEST[$neme]) ? intval($_REQUEST[$name]) : $default;
}

/**
 * 根据变量名获取浮点型变量
 * @param  string $name    变量名
 * @param  int    $default 默认值
 * @return float | int     返回变量的值
 */
function plum_get_float_param($name, $default = 0)
{
	return isset($_REQUEST[$name]) ? floatval($_REQUEST[$name]) : $default;	
}

/**
 * 根据变量名判断变量是否存在
 * @param  string $name 变量名
 * @return bool       true or false
 */
function plum_get_isset_param($name)
{
	return isset($_REQUEST[$name]) ? true : false;
}

/**
 * 根据变量名获取数组变量
 * @param  string $name    变量名
 * @param  array  $default 默认值
 * @return array           返回变量的值
 */
function plum_get_arr_param($name, $default = array())
{
	return isset($_REQUEST[$name]) ? $_REQUEST[$name] : $default;
}

//---------------------------获取服务器变量-----------------------------------
/**
 * 获取服务器变量
 * @param  string   $name    变量名
 * @param  string   $default 默认值
 * @return string            返回变量的值
 */
function plum_get_server($name, $default = '')
{
	$name = strtoupper($name);
	return isset($_SERVER[$name]) ? trim($_SERVER[$name]) : $default;
}

/**
 * 检测当前是否为测试环境
 * @param  mixed $test_host 测试环境的host名称
 * @return bool            true or false
 */
function plum_check_test_env($test_host = array())
{
	$curr_host = plum_get_server('HTTP_HOST'); //获取当前请求的host(主机名)
	if(empty($test_host)) {
		$test_host = plum_parse_config('test_host', 'app', array());
	} else {
		if(is_string($test_host)) {
			$test_host = array($test_host);
		}
	}

	if(in_array($curr_host, $test_host)) {
		return true;
	}
	return false;
}
//--------------------------获取配置----------------------------------
/**
 * 获取配置文件
 * @param  string $type 文件名
 * @return array       配置文件的内容
 */
function plum_get_config($type = 'app')
{
	if(!$type) {
		return array();
	}
	static $config = array();
	if(isset($config[$type])) {
		return $config[$type];
	}
	$config_file = PLUM_DIR_CONFIG . '/' . $type . '.php';
	$data = array();
	if(file_exists($config_file)) {
		$data = @include_once $config_file;
	}
	$config[$type] = $data;
	return $data;
}

/**
 * 解析配置文件(获取某个配置项的配置信息)
 * @param  string $name    配置项名称
 * @param  string $type    配置文件的名称
 * @param  string $default 默认值
 * @return mixed           配置项的值
 */
function plum_parse_config($name, $type = 'app', $default = '')
{
	static $env = null;
	if(!$env) {
		$curr_host = plum_get_server('HTTP_HOST');
		$app_cfg   = plum_get_config('app');
		$test_host = isset($app_cfg['test_host']) ? $app_cfg['test_host'] : array();
		
		if(in_array($curr_host, $test_host)) {
			$env = PLUM_TESTING_ENV;
		} else {
			$env = PLUM_ENV;
		}
	}

	$config = plum_get_config($type);
	if(empty($config) || !$name) {
		return $default;
	}
	//先获取当前环境变量下的配置字符，若无，则获取默认的配置字段
	return isset($config[$env.':'.$name]) ? $config[$env.':'.$name] : (isset($config[$name]) ? $config[$name] : $default);
}

/**
 * 检查数组的键值是否存在
 * @param  string | int $key     键名
 * @param  array        $array   数组
 * @param  string       $default 默认值
 * @return string                返回值
 */
function plum_check_array_key($key, $array = array(), $default = '') 
{
	if(is_array($array) && isset($array[$key])) {
		return $array[$key];
	}
	return $default;
}

function plum_redirect($url, $absolute = false)
{
	$pattern = '/^http[s]?:\/\//';
	if(preg_match($pattern, $url)) {
		$absolute = true;
	}

	if(!$absolute) {
		$http_type = plum_get_server('https') ? 'https://' : 'http://';
		$http_host = plum_get_server('http_host');
		$server_port = plum_get_server('server_port', 80);

		$url = $http_type . $http_host . ($server_port == 80 ? '' : ':'.$server_port) . '/' . ltrim($url, '/');
	}

	if(!headers_sent()) {
		header("Location: {$url}");
	} else {
		echo '<script type="text/javascript">';
		echo "window.location.href = '{$url}'";
		echo '</script>';
	}
	exit;
}

/**
 * 自定义cookie设置方法，默认可以用来处理session cookie
 * @param  int $lifetime 存活时间，单位为秒，默认为0，即关闭浏览器即失效
 * @param  null $name    cookie名称，默认为session name
 * @param  null $value   cookie对应值，默认为session id
 * @param  string $path  cookie存储路径，默认为整站
 * @return bool       true or false
 */
function plum_set_cookie($lifetime = 0, $name = null, $value = null, $path = '/')
{
	$lifetime = $lifetime > 0 ? intval($lifetime) : 0;
	//默认用来处理session
	$name = $name ? $name : ini_get('session.name');
	$value = $value ? $value : session_id();
	$reuqest_time = (int) plum_get_server('REQUEST_TIME', time());
	$expire = $lifetime > 0 ? $request_time + $lifetime : 0;

	if(!setrawcookie($name, $value, $expire, $path)) {
		trigger_error('set cookie name:{$name},value:{$value} failed!',E_USER_WARNING);
		return false;
	}
	return true;
}

/**
 * 通过设置cookie失效时间，达到记住用户功能
 * @param  int $lifetime 存活时间
 * @param  string $path  存储路径，默认整站
 */
function plum_remember_login_user($lifetime, $path = '/') 
{
	plum_set_cookie($lifetime, null, null, $path);
}

/**
 * 为密码加盐
 * @param  string $pass 密码
 * @return string       加密后的密码
 */
function plum_salt_password($pass) 
{
	return md5('@&!#plum'.md5($pass).'plum$^%*');
}

/**
 * 对字符串前后加盐加密
 * @param  string $str  需要加密的字符串
 * @param  mixed  $salt 盐
 * @return string       加盐加密后的字符串
 */
function plum_md5_with_salt($str, $salt) 
{
	if(!$str || !$salt) {
		return false;
	}
	$prefix_salt = $suffix_salt = $salt;
	if(is_array($salt) && count($salt) == 2) {
		list($prefix_salt, $suffix_salt) = $salt;
	}
	return md5($prefix_salt.md5($str).$suffix_salt);
}

/**
 * url跳转加错误提醒
 * @param  string  $msg       提示信息
 * @param  string  $url       跳转url,url为空时将后退
 * @param  int     $countdown 等待时间
 * @param  int     $aboulute  url是否为绝对路径
 */
function plum_redirect_with_msg($msg, $url = '', $countdown = 3, $aboulute = false)
{
	$pattern = '/^http[s]?:\/\//';
	if(preg_match($pattern, $url)) {
		$absolute = true;
	}
    if (!$absolute && $url) {//非绝对路径
        $http_type      = plum_get_server('https') ? 'https://' : 'http://';
        $http_host      = plum_get_server('http_host');
        $server_port    = plum_get_server('server_port', 80);

        $url = $http_type . $http_host . ($server_port == 80 ? '' : ':'.$server_port) . '/' . ltrim($url, '/');
    }
	
	$msg = $msg ? trim($msg) : '跳转提示';
	$countdown = intval($countdown);

	include 'tpl/redirect.tpl';
	exit;
}

/**
 * 验证是否为邮箱
 * @param $email
 * @return bool
 */
function plum_is_email($email) 
{
    $pattern = '/^[A-Za-z0-9]+([-_\.][A-Za-z0-9]+)*@([A-Za-z0-9]+[-\.])+[A-Za-z0-9]{2,5}$/i';
    if (preg_match($pattern, $email)) {
        return true;
    }
    return false;
}

/**
 * 验证手机号正确
 * @param $phone
 * @return bool
 */
function plum_is_phone($phone) 
{
    $pattern = '/^(0|86|17951)?(13[0-9]|15[012356789]|17[012356789]|18[0-9]|14[57])[0-9]{8}$/';
    if (preg_match($pattern, $phone)) {
        return true;
    }
    return false;
}

/**
 * 手机号验证，通用版
 * @param $mobile
 * @return bool
 */
function plum_is_mobile($mobile) 
{
    $pattern = '/^1(\d{10})$/';
    if (preg_match($pattern, $mobile)) {
        return true;
    }
    return false;
}

/**
 * 密码长度验证
 * @param $pass
 * @return bool
 */
function plum_passlen_check($pass) 
{
    $length = strlen($pass);
    if ($length < 6 || $length > 50) {
        return false;
    }
    return true;
}

#pragma mark - UTF-8字符处理
/**
 * 检验字符是否为UTF-8编码
 * @param $text
 * @return bool
 */
function plum_validate_utf8($text) 
{
    if (strlen($text) == 0) {
        return TRUE;
    }
    // With the PCRE_UTF8 modifier 'u', preg_match() fails silently on strings
    // containing invalid UTF-8 byte sequences. It does not reject character
    // codes above U+10FFFF (represented by 4 or more octets), though.
    return (preg_match('/^./us', $text) == 1);
}

/**
 * 获取UTF-8编码的字符长度
 * @param $text
 * @return int
 */
function plum_utf8_strlen($text) 
{
    if (plum_validate_utf8($text)) {
        return mb_strlen($text, 'UTF-8');
    }
    return 0;
}

/**
 * 截取指定长度的字符
 * @param $str
 * @param int $length
 * @param string $replace
 * @return string
 */
function plum_utf8_str_truncate($str, $length = 10, $replace = '……') 
{
    if (plum_utf8_strlen($str) <= $length) {
        return $str;
    } else {
        return mb_substr($str, 0, $length) . $replace;
    }
}

/**
 * 用户名格式：只能是数字，字母，下划线组成
 * @param $username
 * @return bool
 */
function plum_is_username($username)
{
    if(preg_match('/^[0-9a-zA-Z_\x{4e00}-\x{9fa5}]+$/u',$username)){
        return true;
    }else{
        return false;
    }
}

/**
 * 以清晰的格式打印数组或者对象
 * @param  array | json  $msg  要打印的数组或对象
 * @param  bool $exit 是否退出
 */
function plum_msg_dump($msg, $exit = false) 
{
	header("Content-Type: text/html; charset = UTF-8");
	echo '<pre>';
	var_dump($msg);
	echo '</pre>';
	if($exit) {
		exit('this is end');
	}	
}

function plum_url_location($msg = '', $url = '')
{
	header("Content-Type: text/html; charset = UTF-8");
	echo "<script>";
	if(!empty($msg))
		echo "alert('$msg')";
	if($url) {
		echo "window.location.href = '$url'";
	} else {
		echo "window.history.back(-1);";
	}
	echo "</script>";
	exit();
}

/**
 * 去掉字符串中的html标签
 * @param  string $str 需要剔除标签的字符串
 * @return string      剔除标签后的字符串
 */
function plum_strip_html_tags($str) 
{
	$str = htmlspecialchars_decode($str);
	$str = strip_tags($str);
	$str = trim($str);
	$str = preg_replace('/\s(?=\s)/', '', $str);
	$str = preg_replace('/[\n\r\t]/', '', $str);
	$str = preg_replace('/<img.*?src="(.*?)".*>/is', '', $str);
	return $str;
}

/**
 * 解析request_url，并返回数组形式的请求参数列表
 * '/'分隔的请求参数只能包含字母、数字
 * 只解析形如“域名/参数?更多”单引号之间的内容
 * @return array
 */
function plum_parse_request_uri() 
{
	$uri = plum_get_server('REQUEST_URI', '/');
	$pattern = '/^\/+([a-zA-Z0-9\/]+)(\?|&)?.*/i';
	$query = array();

	if(preg_match($pattern, $uri, $match)) {
		$rep_pat = '/\/+/';
		$rep_str = '/';
		$tmp = $match[1];
		$tmp = trim($tmp, '/');//去掉两端无用的'/';
		$tmp = preg_replace($rep_pat, $rep_str, $tmp);//替换多余的'/'
		$query = explode('/', $tmp);
	}
	return $query;
}

/**
 * 判断目录是否存在，创建并修改权限
 * @param  string $uri 目录地址
 * @return bool      ]
 */
function plum_setmod_dir($uri) 
{
	//检查目录是否存在
	if(!file_exists($uri)) {
		//递归创建目录，并赋予755权限，拥有者/用户组可写
		if(!mkdir($uri, DEFAULT_DIR_MODE, true)) {
			trigger_error($uri.'目录创建失败', E_USER_ERROR);
			return false;
		}
	}

	if(!is_writable($uri)) {
		//更改目录的权限
		if(!chmod($uri, DEFAULT_DIR_MODE)) {
			//更改权限失败，触发错误并退出
			trigger_error($uri.'目录不可写', E_USER_ERROR);
			return false;
		}
	}
	return false;
}


