<?php

/**
* 
*/
class Libs_Mvc_Helper_RequestHelper
{
	private static $_instance;
	
	private function __construct()
	{

	}

	public static function getInstance() 
	{
		if(is_null(self::$_instance)) {
			self::$_instance = new self();
		}
		return self::$_instance;
	}
}