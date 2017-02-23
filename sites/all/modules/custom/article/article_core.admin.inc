<?php
function article_core_admin_list() {
	//列表
	$header = array(
		array('data' => '文章标题', 'width' => '23%'),
		array('data' => '文章类型'),
		array('data' => '文章权重'),
		array('data' => '是否置顶'),
		array('data' => '文章更新时间'),
		array('data' => '操作'),
	);

	$table = theme('table', array('header' => $header, 'rows' => []));
	return $table;
}

function article_core_admin_edit($form, &$form_state, $ar_id = 0) {
	$ar_id = intval($ar_id);
	/*$form['#attached']['css'] = array(
		drupal_get_path('module', 'common') . "/css/search_form.css",
	);//载入css，js*/
	$form['basic_set'] = array(
		'#type'        => 'fieldset',  //表单标题信息
		'#title'       => t('文章信息'),
		'#collapsible' => true,        //表单是否可折叠
		'#collapsed'   => false,       //默认是否折叠
	);
	$form['basic_set']['id'] = array(
		'#type'          => 'textfield', //文本输入框
		'#title'         => t('文章id'),
		'#default_value' => $ar_id,
		'#required'      => true,
	);
	$is_top = array(0 => '置顶', 1 => '不置顶');
	$form['basic_set']['ar_istop'] = array(
		'#type'          => 'select',  //下拉选项框
		'#title'         => t('是否置顶'),
		'#default_value' => 0,
		'#options'       => $is_top,
		'#required'      => true,
	);
	$config = array(
        'label_title'       => '排序权重',
        'field_name'        => 'weight',
        'default_value'     => 0,
        'min'               => 0,
        'max'               => 100,
        'step'              => 1,
    );
	//数字滑动块
    $form['basic_set']['ar_sort'] = plum_number_weight_field($config);
	
	$form['basic_content'] = array(
		'#type'        => 'fieldset',
		'#title'       => t("文章内容"),
		'#collapsible' => TRUE,
		'#collapsed'   => false,
	);
	$textarea_config = array(
        'field_name'        => 'ar_content',
        'default_value'     => '',
        'sub_dir'           => 'default',  //图片上传的子目录
    );
	//富文本编辑器
	$form['basic_content']['ar_content'] = plum_textarea_content_field($textarea_config);
	
	$default_config = array(
        'label_title'           => '请选择',
        'field_name'            => 'plum_level_field',
        'level_number'          => 3,
        'request_url'           => plum_parse_menu_link('member', 'level linkage'),//获取省市联动的数据
        'field_title'           => array('请选择', '请选择', '请选择'),
        'default_value'         => array(0, 0, 0),
        'form_id'               => 'form',
        'form_verify'           => 0,
    );
    //多级联动（省市三级联动）
	$form['basic_content']['ar_level'] = plum_select_level_field($default_config);
	
	$check_config = array(
        'title'             => '',
        'default_value'     => '',
        'max_length'        => 100,
        'min_length'        => 10,
        'placeholder'       => '',
        'form_id'           => 'form',
        'form_verify'       => 0,
    );
	//字数检查，提示已输入多少字符，还可输入多少字符
    $form['basic_content']['ar_check'] = plum_brief_check_field($check_config);
	
	$upload_config = array(
        'request_uri'       => '/index/uploadFile',
        'field_name'        => 'file_path',
        'default_value'     => '',
        'file_type'         => 'png|gif|jpg|jpeg',
        'upload_multi'      => false,
        'is_image'          => false,
    );
	//文件上传，带进度条提示
	$form['basic_content']['ar_upload'] = plum_file_upload_field($upload_config);
	
	$mimage_config = array(
        'field_name'        => 'file_path',
        'default_values'    => array(),
        'file_type'         => 'png|gif|jpg|jpeg',
        'upload_multi'      => true,
        'is_image'          => true,
    );
	//多图片上传
	$form['basic_content']['ar_image'] = plum_multi_image_field($mimage_config);

	$form['#submit'][] = 'article_core_admin_edit_submit';//提交表单
	$form              = system_settings_form($form);
	
	return $form;
}

function article_core_admin_edit_submit($form, &$form_state) {

}