<?php

/**
* 
*/
class App_Controller_Site_HeaderController extends Libs_Mvc_Controller_FrontBaseController
{
	
	public function __construct()
	{
		parent::__construct();
		$this->setLayout('default.tpl');
	}
}