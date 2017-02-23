<?php
/**
 * 图片上传通用方法
 * @param string $dir 上传的子目录
 */
function image_upload_common($dir) {
    $path_info = _fetch_gallery_path($dir);
    $validators = array(
        array('png gif jpg jpeg'),
        'filename' => $path_info['file_name'],
        'filepath' => $path_info['absolute_path'],
    );
    $data = array(
        'ec' => 404,
        'em' => '上传失败',
        'error' => 404,//error，message配置段兼容kindeditor
        'message' => '请选择文件上传'
    );
    $name = $_FILES['files']['name'];
    if (isset($name) && count($name) == 1) {
        $source = key($name);;
        $file = file_save_upload($source, $validators)

        if ($file) {
            $file->status = FILE_STATUS_PERMANENT;
            file_save($file);
            $file_url = $path_info['relative_path'] . $file->filename;

            $data = array(
                'ec' => 200,
                'em' => '上传成功',
                'filepath' => $file_url,
                'url'   => $file_url,
                'error' => 0, //error，message配置段兼容kindeditor
                'message' => '上传成功',
            );
        }
    }
    drupal_json_output($data);
    exit;
}

function image_server_manage() {
    $gallery    = '/gallery/';                                  //根目录路径，可以指定绝对路径，比如 /var/www/attached/
    $root_path  = PLUM_DIR_UPLOAD . $gallery;                   //根目录URL，可以指定绝对路径，比如 http://www.yoursite.com/attached/
    $root_url   = PLUM_PATH_UPLOAD . $gallery;                  //图片扩展名
    $ext_arr    = array('gif', 'jpg', 'jpeg', 'png', 'bmp');    //目录名
    $dir_name   = plum_get_param('dir');
    if (preg_match('/\./', $dir_name)) {
        echo "Invalid Directory name.";
        exit;
    }
    if ($dir_name !== '') {
        $root_path .= $dir_name . "/";
        $root_url .= $dir_name . "/";
        if (!file_exists($root_path)) {
            mkdir($root_path);
        }
    }

    //根据path参数，设置各路径和URL
    if (!plum_get_param('path')) {
        $current_path = realpath($root_path) . '/';
        $current_url = $root_url;
        $current_dir_path = '';
        $moveup_dir_path = '';
    } else {
        $current_path = realpath($root_path) . '/' . $_GET['path'];
        $current_url = $root_url . $_GET['path'];
        $current_dir_path = $_GET['path'];
        $moveup_dir_path = preg_replace('/(.*?)[^\/]+\/$/', '$1', $current_dir_path);
    }
    //排序形式，name or size or type
    $order = plum_get_param('order', 'name');
    $order = strtolower($order);

    //不允许使用..移动到上一级目录
    if (preg_match('/\.\./', $current_path)) {
        echo 'Access is not allowed.';
        exit;
    }
    //最后一个字符不是/
    if (!preg_match('/\/$/', $current_path)) {
        echo 'Parameter is not valid.';
        exit;
    }
    //目录不存在或不是目录
    if (!file_exists($current_path) || !is_dir($current_path)) {
        echo 'Directory does not exist.';
        exit;
    }

    //遍历目录取得文件信息
    $file_list = array();
    if ($handle = opendir($current_path)) {
        $i = 0;
        while (false !== ($filename = readdir($handle))) {
            if ($filename{0} == '.') continue;
            $file = $current_path . $filename;
            if (is_dir($file)) {
                $file_list[$i]['is_dir'] = true; //是否文件夹
                $file_list[$i]['has_file'] = (count(scandir($file)) > 2); //文件夹是否包含文件
                $file_list[$i]['filesize'] = 0; //文件大小
                $file_list[$i]['is_photo'] = false; //是否图片
                $file_list[$i]['filetype'] = ''; //文件类别，用扩展名判断
            } else {
                $file_list[$i]['is_dir'] = false;
                $file_list[$i]['has_file'] = false;
                $file_list[$i]['filesize'] = filesize($file);
                $file_list[$i]['dir_path'] = '';
                $file_ext = strtolower(pathinfo($file, PATHINFO_EXTENSION));
                $file_list[$i]['is_photo'] = in_array($file_ext, $ext_arr);
                $file_list[$i]['filetype'] = $file_ext;
            }
            $file_list[$i]['filename'] = $filename; //文件名，包含扩展名
            $file_list[$i]['datetime'] = date('Y-m-d H:i:s', filemtime($file)); //文件最后修改时间
            $i++;
        }
        closedir($handle);
    }

    //排序
    function cmp_func($a, $b) {
        global $order;
        if ($a['is_dir'] && !$b['is_dir']) {
            return -1;
        } else if (!$a['is_dir'] && $b['is_dir']) {
            return 1;
        } else {
            if ($order == 'size') {
                if ($a['filesize'] > $b['filesize']) {
                    return 1;
                } else if ($a['filesize'] < $b['filesize']) {
                    return -1;
                } else {
                    return 0;
                }
            } else if ($order == 'type') {
                return strcmp($a['filetype'], $b['filetype']);
            } else {
                return strcmp($a['filename'], $b['filename']);
            }
        }
    }
    usort($file_list, 'cmp_func');

    $result = array();
    //相对于根目录的上一级目录
    $result['moveup_dir_path'] = $moveup_dir_path;
    //相对于根目录的当前目录
    $result['current_dir_path'] = $current_dir_path;
    //当前目录的URL
    $result['current_url'] = $current_url;
    //文件数
    $result['total_count'] = count($file_list);
    //文件列表数组
    $result['file_list'] = $file_list;

    drupal_json_output($result);
    exit;
}

/**
 * 截取图片缩略图
 */
function image_upload_thumbnail() {
    $xywh       = plum_get_param('xywh');
    $src        = plum_get_param('src');
    $thumbwh    = plum_get_param('thumbwh');

    $data = array('ec' => 404, 'em' => 'bad request');
    if ($xywh && $src && $thumbwh) {
        $xywh = explode('|', $xywh);
        $size = array();
        foreach($xywh as $each) {
            $one = explode('=', $each);
            $size[$one[0]] = ceil($one[1]);
        }
        $thumbwh = explode('|', $thumbwh);
        $targ = array('w' => intval($thumbwh[0]), 'h' => intval($thumbwh[1]));
        $ratio_width = 360;
        if ($tbl_path = _generate_thumbnail($src, $size, $targ, $ratio_width)) {
            $data['ec'] = 200;
            $data['em'] = '截取缩略图成功';
            $data['url'] = $tbl_path;
        } else {
            $data['em'] = '缩略图生成失败';
        }
    }
    drupal_json_output($data);
    exit;
}

/**
 * 根据图片类别生成缩略图
 * @param string $s_path 原始图片路径
 * @param array $size e.g. array('x' => 1, 'y' => 2, 'w' => 3, 'h' => 4)
 * @param array $targ 生成缩略图的宽，高 e.g. array('w' => 200, 'h' => 200)
 * @param int $ratio_w 缩放比例--宽度
 * @return bool
 */
function _generate_thumbnail($s_path, array $size, array $targ, $ratio_w = 360) {
    $filepath = PLUM_DIR_ROOT . $s_path;
    if (file_exists($filepath)) {
        $tmp        = explode('.', $s_path);
        $ext        = strtolower(array_pop($tmp));
        $fetch_thumb_path   = _fetch_thumbnail_path($s_path);
        $thumb_dir_path     = $fetch_thumb_path['thumb_dir_path'];
        $thumb_url_path     = $fetch_thumb_path['thumb_url_path'];
        $type               = $ext == 'jpg' ? 'jpeg' : $ext;
        if (in_array($type, array('jpeg', 'gif', 'png'))) {
            $create_func = "imagecreatefrom$type";
            $output_func = "image$type";
            if (function_exists($create_func) && function_exists($output_func)) {
                $targ_w = intval($targ['w']);
                $targ_h = intval($targ['h']);
                $dst_r = imagecreatetruecolor( $targ_w, $targ_h );
                $img_r = $create_func($filepath);
                $true_w = imagesx($img_r);
                $ratio = $true_w / $ratio_w;
                $size_x = floor($size['x'] * $ratio);
                $size_y = floor($size['y'] * $ratio);
                $size_w = floor($size['w'] * $ratio);
                $size_h = floor($size['h'] * $ratio);
                imagecopyresampled($dst_r, $img_r, 0, 0, $size_x, $size_y, $targ_w, $targ_h, $size_w, $size_h);
                if ('jpeg' == $type) {
                    $output_func($dst_r, $thumb_dir_path, 70);
                } else {
                    $output_func($dst_r, $thumb_dir_path);
                }
                return $thumb_url_path;
            }
        }
    }
    return false;
}

/**
 * 获取图库路径信息
 */
function _fetch_gallery_path($sub_dir) {
    $dir        = isset($sub_dir) && trim($sub_dir) ? trim($sub_dir) : 'default';
    $gallery    = '/gallery/';
    $filepath   = PLUM_PATH_UPLOAD . $gallery . $dir . '/';
    $fileroot   = PLUM_DIR_UPLOAD . $gallery . $dir . '/';
    if (!is_dir($fileroot)) {
        drupal_mkdir($fileroot, null, true);
    }
    list($usec, $sec) = explode(" ", microtime());
    $md5        = strtoupper(md5($usec.$sec));
    $filename   = substr($md5, 0, 8).'-'.substr($md5, 10, 4).'-'.mt_rand(1000, 9999).'-'.substr($md5, 20, 12);

    return array(
        'absolute_path'     => $fileroot,   //绝对路径
        'relative_path'     => $filepath,   //相对路径
        'file_name'         => $filename,   //文件名，不包括扩展名
    );
}

/**
 * 获取缩略图路径
 * @param $image_path
 * @return array
 */
function _fetch_thumbnail_path($image_path) {
    $dir        = 'thumbnail';
    $gallery    = '/gallery/';
    $filepath   = PLUM_PATH_UPLOAD . $gallery . $dir . '/';
    $fileroot   = PLUM_DIR_UPLOAD . $gallery . $dir . '/';
    if (!is_dir($fileroot)) {
        @drupal_mkdir($fileroot, null, true);
    }
    $temp       = explode('/', $image_path);
    $filename   = array_pop($temp);
    list($imagename, $extension)    = explode('.', $filename);
    $thumb_name         = $imagename."-tbl.".$extension;
    $thumb_dir_path     = $fileroot.$thumb_name;
    $thumb_url_path     = $filepath.$thumb_name;
    if (file_exists($thumb_dir_path)) {//已存在
        $new_thumb_path = _fetch_gallery_path($dir);
        $thumb_dir_path = $new_thumb_path['absolute_path'].$new_thumb_path['file_name'].'.'.$extension;
        $thumb_url_path = $new_thumb_path['relative_path'].$new_thumb_path['file_name'].'.'.$extension;
    }
    return array(
        'thumb_dir_path'    => $thumb_dir_path,
        'thumb_url_path'    => $thumb_url_path,
    );
}
