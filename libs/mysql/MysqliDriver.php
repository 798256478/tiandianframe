<?php

/**
* 
*/
class Libs_Mysql_MysqliDriver
{
	public $config;
	public $table_prifix;
	public $cur_link;
	public $query_num;
	public $version;

	public function set_config($config) {
		if(!empty($config)) {
			$this->config = $config;
			$this->table_prifix = isset($this->config['tablepre']) ? $this->config['tablepre'] : 'pre_';
		} else {
			$this->halt("call undefined mysql configure");
		}
	}

	public function connect() {
		if(empty($this->config)) {
			$this->halt('mysql db config empty');
		}
		/*连接数据库*/
		$link = @new mysqli(
			$this->config['host'],
			$this->config['user'],
			$this->config['pass'],
			$this->config['dbname'],
			$this->config['port']
		);
		if($link->connect_errno) {
			$this->halt("mysqli connect failed. connect error:{$link->connect_error}. with config:" . json_encode($this->config));
		}
		$link->set_charset(isset($this->config['dbcharset']) ? $this->config['dbcharset'] : 'utf8');
		$this->cur_link = $link;
		unset($link);
	}

	/**
	 * 获取完整表名
	 * @param  [type] $tablename [description]
	 * @return [type]            [description]
	 */
	public function table_name($tablename) {
		return $this->table_prefix . $tablename;
	}

	/**
	 * 以关联数组形式获取第一行数据
	 * @param  [type] $sql [description]
	 * @return [type]      [description]
	 */
	public function fetch_first($sql) {
		return $this->fetch_array($this->query($sql));
	}

	/**
	 * 获取数据首行，收个字段数据
	 * @param  [type] $sql [description]
	 * @return [type]      [description]
	 */
	public function result_first($sql) {
		return $this->result($this->query($sql), 0);
	}

	public function halt($message = '', $code = 0, $sql = '') {
		trigger_error("errmsg = {$message};errcode={$code};sql={$sql}", E_USER_ERROR);
		exit;
	}

	public function query($sql, $silent = false, $unbuffered = false) {
		if('UNBUFFERED' === $silent) {
			$silent = false;
		} elseif ('SILENT' === $silent) {
			$silent = true;
			$unbuffered = false;
		}

		$resultmode = $unbuffered ? MYSQLI_USE_RESULT : MYSQLI_STORE_RESULT;
		if(!($query = $this->cur_link->query($sql, $resultmode))) {
			if(in_array($this->errno(), array(2006, 2013)) && substr($silent, 0, 5) != 'RETRY') {
				$this->connect();
				return $this->cur_link->query($sql, 'RETRY'.$silent);
			}
			if(!$silent) {
				$this->halt($this->error(), $this->errno(), $sql);
			}
		}
		$this->query_num ++ ;
		return $query;
	}

	/**
	 * 总行数， buffered数据集时可用
	 * @param  [type] $query [description]
	 * @return [type]        [description]
	 */
	public function num_rows($query) {
		return $query ? $query->num_rows : 0;
	}

	/*
	 * 结果集query中的字段数目
	 * @param  [type] $query [description]
	 * @return [type]        [description]
	 */
	 
	public function num_fields($query) {
		return $query ? $query->field_count : null;
	}

	/**
	 * 释放结果集
	 * @param  [type] $query [description]
	 * @return [type]        [description]
	 */
	public function free_result($query) {
		return $query ? $query->free() : false;
	}

	/**
	 * 默认以关联数组的形式获取结果集
	 * @param  [type] $query       [description]
	 * @param  [type] $result_type [description]
	 * @return [type]              [description]
	 */
	public function fetch_array($query, $result_type = MYSQLI_ASSOC) {
		if($result_type == "MYSQLI_ASSOC")
			$result_type = MYSQLI_ASSOC;
		return $query ? $query->fetch_array($result_type) : null;
	}

	/**
	 * 获取结果集中第row行数据中的第一个字段数据
	 * @param  [type]  $query [description]
	 * @param  integer $row   [description]
	 * @return [type]         [description]
	 */
	public function result($query, $row = 0) {
		if(!$query || $query->num_rows == 0) {
			return null;
		}
		$query->data_seek($row);//将结果集中的指针移动到第row行
		$assocs = $query->fetch_row();//获取第row行的数据
		return $assocs[0];
	}

	/**
	 * 获取一行数据
	 * @param  [type] $query [description]
	 * @return [type]        [description]
	 */
	public function fetch_row($query) {
		return $query ? $query->fetch_row() : null;
	}

	/**
	 * 获取查询字段
	 * @param  [type] $query [description]
	 * @return [type]        [description]
	 */
	public function fetch_fields($query) {
		return $query ? $query->fetch_fields() : null;
	}

	/**
	 * 转义 SQL 语句中使用的字符串中的特殊字符。
	 * @param  [type] $escapestr [description]
	 * @return [type]            [description]
	 */
	public function escape($escapestr) {
		return $this->cur_link->real_escape_string($escapestr);
	}

	/**
	 * 返回受影响的行数
	 * @return [type] [description]
	 */
	public function affected_rows() {
		return $this->cur_link->affected_rows;
	}

	/**
	 * 返回插入的id
	 * @return [type] [description]
	 */
	public function insert_id() {
		return ($id = $this->cur_link->insert_id) >= 0 ? $id : $this->result($this->query("SELECT last_insert_id()"), 0);
	}

	/**
	 * 获取错误代码
	 * @return [type] [description]
	 */
	public function errno() {
		return intval($this->cur_link->errno);
	}

	/**
	 * 获取错误内容
	 * @return [type] [description]
	 */
	public function error() {
		return $this->cur_link->error;
	}

	public function version() {
		if(empty($this->version)) {
			$this->version = $this->cur_link->server_info;
		}
		return $this->version;
	}

	public function close() {
		$this->cur_link->close();
	}

}