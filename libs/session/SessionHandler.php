<?php
class Libs_Session_SessionHandler 
{
	private $redis = null;
	private $session_name = 'ikinvin_session';
	private $user_life_time = 864000;
	private $visitor_life_time = 3600;

	public function __construct() 
	{
		$this->redis = (new Lib_Redis_RedisClient('session'))->getRedis();
		$this->_init_session();
	}

	private function _init_session() {
		session_name($this->session_name);
		session_set_save_handler(
			array(&$this, "open"), 
			array(&$this, "close"), 
			array(&$this, "read"), 
			array(&$this, "write"), 
			array(&$this, "destroy"), 
			array(&$this, "gc")
		);
	}

	public function run() {
		$session_data = isset($_SESSION) ? $_SESSION : null;
		session_start();
		if(!empty($session_data)) {
			$_SESSION += $session_data;
		}

		global $APP_USER;

		$APP_USER = isset($_SESSION['uid']) && $_SESSION['uid'] ?  intval($_SESSION['uid']) : 0;
		$life_time = $APP_USER ? $this->user_life_time : $this->visitor_life_time;
		$params = session_get_cookie_params();
		$request_time = (int) $_SERVER['REQUEST_TIME'];
		setcookie($this->session_name, session_id(), $request_time + $life_time,
			$params['path'], $params['domain'], $params['secure'], $params['httponly']);
	}

}
