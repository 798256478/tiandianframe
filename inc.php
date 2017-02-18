<?php
//ini_set用来设置php.ini的值，在函数执行的时候生效，脚本结束后，设置失效
//无需打开php.ini文件，就能修改配置
ini_set('date.timezone', 'Asia/Shanghai');                                    //设置时区为东八区

define('PLUM_VERSION', '0.0.1');                                              //定义版本号
define('PLUM_RELEASE', '20160314');                                           //定义发布时间

define('PLUM_DIR_ROOT',      str_replace("\\", "/", dirname(__FILE__)));      //将根目录所有\替换成/
define('PLUM_DIR_BOOTSTRAP', PLUM_DIR_ROOT."/bootstrap");                     //设置引导文件目录
define('PLUM_DIR_APP',       PLUM_DIR_ROOT."/sites/app");                     //设置app所在目录
define('PLUM_DIR_CONFIG',    PLUM_DIR_APP.'/config');                         //app配置文件所在目录
define('PLUM_APP_PLUGIN',    PLUM_DIR_APP.'/plugin');                         //app插件所在目录
define('PLUM_DIR_LIB',       PLUM_DIR_ROOT.'/libs');                          //库文件所在目录
define('PLUM_DIR_UPLOAD',    PLUM_DIR_ROOT.'/upload');                        //上传文件所在目录
define('PLUM_DIR_SESSION',   PLUM_DIR_APP.'/storage/session');                //session保存目录
define('PLUM_DIR_CACHE',     PLUM_DIR_APP.'/storage/cache');                  //cache保存目录
define('PLUM_DIR_TEMP',      PLUM_DIR_APP.'/storage/tmp');                    //临时文件目录
define('PLUM_APP_LOG',       PLUM_DIR_APP.'/storage/log');                    //log日志保存目录
define('PLUM_APP_BUILD',     PLUM_DIR_ROOT.'/public/build');                  //生成文件目录
define('PLUM_PATH_UPLOAD',   '/upload');                                      //上传文件根目录
define('PLUM_PATH_PUBLIC',   '/public');                                      //公共文件根目录
define('PLUM_WEBAPP_MODULE', 'site');                                         //默认的app模块

define('PLUM_TESTING_ENV',     'test');                                       //当前测试环境
define('PLUM_DEVELOPMENT_ENV', 'dev');                                        //当前测试环境
define('PLUM_PRODUCTION_ENV',  'pro');                                        //当前测试环境