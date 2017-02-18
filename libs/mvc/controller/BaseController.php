<?php

/**
* 
*/
class Libs_Mvc_Controller_BaseController
{
	const JSON_SUCCESS_STATUS = 200;
	const JSON_FAILURE_STATUS = 400;

	public $request  = null;
	public $response = null;
	public $smarty;
	public $output = array();
	
	public function __construct()
	{
		$this->request = Libs_Mvc_Helper_RequestHelper::getInstance();
		$this->response = Libs_Mvc_Helper_ResponseHelper::getInstance();
		//构造smarty模板类
		$this->smarty = new Libs_View_Smarty_SmartyClass();
	}

	public function setLayout($layout) {
		$this->smarty->setLayout($layout);
	}

	public function displaySmarty($template, $cache_id = null, $compile_id = null, $use_layout = null) {
		foreach ($this->output as $name => $value) {
			$this->smarty->assign($name, $value);
		}
		$this->smarty->display($template, $cache_id, $compile_id, $use_layout);
	}
}