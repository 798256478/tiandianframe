<?php

/**
* 
*/
class App_Controller_Site_IndexController extends App_Controller_Site_HeaderController
{	
	public function __construct()
	{
		parent::__construct();
	}

	public function indexAction() {
		$this->setLayout('default.tpl');
		$this->displaySmarty('site/index.tpl');
	}
}