<?php

/**
* 
*/
class Libs_Mvc_Controller_FrontBaseController extends Libs_Mvc_Controller_BaseController
{
	
	public function __construct()
	{
		parent::__construct();
		global $base_url;
		$this->output['baseurl'] = $base_url;
	}
}