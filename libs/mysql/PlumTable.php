<?php

/**
* 
*/
class Libs_Mysql_PlumTable
{
	protected $_table;
	protected $_pk;
	protected $_df;
	protected static $_fields = array();
		
	public function __construct($para = array())
	{
		if(!empty($para)) {
			$this->_table = $para['table'];
			$this->_pk = $para['pk'];
			self::$_fields = $para['fields'];
		}
	}

	/**
	 * 获取表名
	 * @return 表名
	 */
	public function getTable() {
		return $this->_table;
	}

	/**
	 * 设置表名
	 * @param 表名
	 */
	public function setTable($name) {
		return $this->_table = $name;
	}

	/**
	 * 获取表中数据数量
	 * @return 数量
	 */
	public function count() {
		$count = (int) DB::result_first("SELECT count(*) FROM " . DB::table($this->_table));
		return $count;
	}

	    //根据主键val更新数据
    public function update($val, $data, $unbuffered = false, $low_priority = false) {
        if(isset($val) && !empty($data) && is_array($data)) {
            $this->checkpk();
            $ret = DB::update($this->_table, $data, DB::field($this->_pk, $val), $unbuffered, $low_priority);
            return $ret;
        }
        return !$unbuffered ? 0 : false;
    }
    //根据主键val删除数据 unbuffered是否缓存结果集，默认缓存
    public function delete($val, $unbuffered = false) {
        $ret = false;
        if(isset($val)) {
            $this->checkpk();
            $ret = DB::delete($this->_table, DB::field($this->_pk, $val), null, $unbuffered);
        }
        return $ret;
    }

    //返回最后插入生成的主键ID
    public function insert($data, $return_insert_id = false, $replace = false, $silent = false) {
        return DB::insert($this->_table, $data, $return_insert_id, $replace, $silent);
    }

    public function checkpk() {
        if(!$this->_pk) {
            DB::halt("table {$this->_table} has not PRIMARY KEY defined");
        }
    }
    //根据主键ID获取一条记录
    public function fetch($id){
        $data = array();
        if(!empty($id)) {
            $data = DB::fetch_first('SELECT * FROM '.DB::table($this->_table).' WHERE '.DB::field($this->_pk, $id));
        }
        return $data;
    }
    //根据主键IDs获取多条记录
    public function fetch_all(array $ids) {
        $data = array();
        if(!empty($ids)) {
            $query = DB::query('SELECT * FROM '.DB::table($this->_table).' WHERE '.DB::field($this->_pk, $ids));
            while($value = DB::fetch($query)) {
                $data[$value[$this->_pk]] = $value;
            }
            DB::free_result($query);
        }
        return $data;
    }

    //获取所有字段
    public function fetch_all_field(){
        $data = false;
        $query = DB::query('SHOW FIELDS FROM '.DB::table($this->_table), '', 'SILENT');
        if($query) {
            $data = array();
            while($value = DB::fetch($query)) {
                $data[$value['Field']] = $value;
            }
        }
        return $data;
    }

    //获取指定区间的记录，并可根据主键排序，根据主键形成索引
    public function range($start = 0, $limit = 0, $sort = '') {
        if($sort) {
            $this->checkpk();
        }
        return DB::fetch_all('SELECT * FROM '.DB::table($this->_table).($sort ? ' ORDER BY '.DB::order($this->_pk, $sort) : '').DB::limit($start, $limit), null, $this->_pk ? $this->_pk : '');
    }

    /**
     * 整理表，清理被删除数据仍占用的空间
     * @return [type] [description]
     */
    public function optimize() {
        DB::query('OPTIMIZE TABLE '.DB::table($this->_table), 'SILENT');
    }

    public function truncate() {
        DB::query("TRUNCATE ".DB::table($this->_table));
    }

        /**
     * 格式化形成查询SQL语句
     * @param array $fields e.g. array('id', 'count' => 'total', 'name')
     * @param array $where
     * @param array $sort e.g. array('count' => 'DESC', 'id' => 'ASC')
     * @param int $index
     * @param int $count
     * @param bool $isand
     * @return string
     */
    public function formatSelectSql($fields = '', $where = array(), $sort = array(), $index = 0, $count = 20, $isand = true) {
        $sql = "select ";
        $sql .= $this->getFieldString($fields);
        $sql .= " from `".DB::table($this->_table)."` ";
        $sql .= $this->formatWhereSql($where, $isand);
	    $sql .= $this->getSqlSort($sort);
	    $sql .= $this->formatLimitSql($index,$count);
        return $sql;
    }

    /**
     * 格式化生成自增字段sql
     * @param mixed $field 字段名，单个
     * @param mixed $inc 自增值，可为正负值
     * @param array $where 条件数组
     * @return string
     */
    public function formatIncrementSql($field, $inc = 1, $where = array()) {
        $sql = "UPDATE `".DB::table($this->_table)."` SET ";
        $increment      = array();
        if (is_string($field)) {
            $increment  = array($field => $inc);
        } else {
            foreach ($field as $key => $val) {
                $increment[$val] = $inc[$key];
            }
        }

        foreach ($increment as $key =>  $val) {
            $sql .= "`{$key}`=`{$key}`+{$val},";
        }
        $sql = rtrim($sql, ',');
        $sql .= $this->formatWhereSql($where);
        return $sql;
    }

    /**
     * 格式化查询总数的SQL语句
     * @param $where
     * @return string
     */
    public function formatCountSql($where) {
        $sql = "select count(*) as total from `".DB::table($this->_table)."` ";
        $sql .= $this->formatWhereSql($where);
        return $sql;
    }
    /**
     * 获取单条数据记录
     * @param array $where @see $this->formatWhereSql
     * @param array $fields @see $this->formatSelectSql
     * @return string
     */
    public function formatSelectOneSql($where, $fields = '') {
        $sql = "select ";
        $fields = $this->getFieldString($fields);
        $sql .= $fields;
        $sql .= " from `".DB::table($this->_table)."` ";
        $sql .= $this->formatWhereSql($where);
        return $sql;
    }

    /**
     * 格式化形成where语句
     * @param array $where e.g. array( array('name' => 'id', 'oper' => '=', 'value' => 2), array(...), ... )
     * @param bool $isand
     * @return string
     */
    public function formatWhereSql($where = array(), $isand = true,$need = '') {
        $cond = "";
        if (!empty($where)) {
            $cond .= " where ";
            $field = array();
            foreach($where as $val) {
                if(is_array($val)){
                    switch ($val['oper']) {
                        case 'in' :
                            //循环并转义
                            foreach ($val['value'] as $zkey => $zval) {
                                $val['value'][$zkey] = DB::quote($zval);
                            }
                            $field[] = "{$need} {$val['name']} {$val['oper']} (".implode(",", $val['value']).")";
                            break;
                        default :
                            //转义
                            $val['value'] = DB::quote($val['value']);
                            $field[] = "{$need} {$val['name']} {$val['oper']}  {$val['value']}";
                            break;
                    }
                }else{
                    $field[] = $val;
                }
//                switch ($val['oper']) {
//                    case 'in' :
//                        //循环并转义
//                        foreach ($val['value'] as $zkey => $zval) {
//                            $val['value'][$zkey] = DB::quote($zval);
//                        }
//                        $field[] = "{$need} {$val['name']} {$val['oper']} (".implode(",", $val['value']).")";
//                        break;
//                    default :
//                        //转义
//                        $val['value'] = DB::quote($val['value']);
//                        $field[] = "{$need} {$val['name']} {$val['oper']}  {$val['value']}";
//                        break;
//                }
            }
            if ($isand) {
                $cond .= implode(" and ", $field);
            } else {
                $cond .= implode(" or ", $field);
            }
        }
        return $cond;
    }

    /**
     * 格式化形成insert语句
     * @param array $values e.g. array(1, 'thomas')
     * @param array $fields e.g. array('id', 'name')
     * @return bool|string
     */
    public function formatInsertSql($values, $fields = array()) {
        //$fields = $this->formatFieldArray($fields);
        if (empty($values)) {
            return false;
        }
        if (!empty($fields) && count($fields) != count($values)) {
            return false;
        }
        $sql = "insert into `".DB::table($this->_table)."` ";
        if (!empty($fields)) {
            $sql .= " (`".implode('`,`', $fields)."`)";
        }
        //转义
        foreach ($values as $key => $val) {
            $values[$key] = DB::quote($val);
        }
        $sql .= " values (".implode(",", $values).")";
        return $sql;
    }

    /**
     * 生成ignore insert语句，防止unique index报错
     * @param array $values
     * @param array $fields
     * @return bool|string
     */
    public function formatIgnoreInsertSql($values, $fields = array()) {
        //$fields = $this->formatFieldArray($fields);
        if (empty($values)) {
            return false;
        }
        if (!empty($fields) && count($fields) != count($values)) {
            return false;
        }
        $sql = "insert ignore into `".DB::table($this->_table)."` ";
        if (!empty($fields)) {
            $sql .= " (`".implode('`,`', $fields)."`)";
        }
        //转义
        foreach ($values as $key => $val) {
            $values[$key] = DB::quote($val);
        }
        $sql .= " values (".implode(",", $values).")";
        return $sql;
    }

    public function formatReplaceSql($values, $fields = array()) {
        if (empty($values)) {
            return false;
        }
        if (!empty($fields) && count($fields) != count($values)) {
            return false;
        }
        $sql = "replace into `".DB::table($this->_table)."` ";
        if (!empty($fields)) {
            $sql .= " (`".implode('`,`', $fields)."`)";
        }
        //转义
        foreach ($values as $key => $val) {
            $values[$key] = DB::quote($val);
        }
        $sql .= " values (".implode(",", $values).")";
        return $sql;
    }

    /**
     * 格式化形成update语句
     * @param array $sets e.g. array('id' => 1, 'name' => 'thomas')
     * @param array $where
     * @return string
     */
    public function formatUpdateSql(array $sets, array $where) {
        if (empty($where)) {
            return false;
        }
        $sql = "update `".DB::table($this->_table)."` set ";
        $sql .= $this->formatUpdateField($sets);
        $sql .= $this->formatWhereSql($where);

        return $sql;
    }

    /**
     * 格式化update字段
     * @param array $sets
     * @return string
     */
    public function formatUpdateField(array $sets){
        $fields = array();
        foreach ($sets as $name => $value) {
            //转义
            $value = DB::quote($value);
            $fields[] = "`{$name}` = {$value}";
        }
        return implode(', ', $fields);
    }

    /**
     * 格式化形成delete语句
     * @param array $where @see $this->formatWhereSql()
     * @return bool|string
     */
    public function formatDeleteSql(array $where) {
        if (empty($where)) {
            return false;
        }
        $sql = "delete from `".DB::table($this->_table)."` ";
        $sql .= $this->formatWhereSql($where);
        return $sql;
    }

    /**
     * 格式化生成字段数组
     * @param array $fields
     * @return array
     */
    public function formatFieldArray(array $fields) {
        if (empty($fields)) {
            return array();
        }
        if (empty(self::$_fields)) {
            $all_fields = $this->fetch_all_field();
            foreach ($all_fields as $key => $val) {
                array_push(self::$_fields, strtolower($key));
            }
        }
        if (isset($fields['field_flag']) && $fields['field_flag'] == 'exclude') {
            unset($fields['field_flag']);
            return array_diff(self::$_fields, $fields);
        }
        return array_intersect($fields, self::$_fields);
    }

    public function __toString() {
        return $this->_table;
    }

    /**
     * @param array $field
     * return sting
     */
    public function getFieldString($fields,$im_str=','){
        if (!empty($fields)&&is_array($fields)) {
            $select_fields = array();
            foreach ($fields as $key => $val) {
                if (is_numeric($key)) {
                    $select_fields[] = "`".$val."`";
                } else {
                    $select_fields[] = "`".$key."` as ".$val."";
                }
            }
            $sql_field = implode($im_str, $select_fields);
        } elseif(!empty($fields)&&is_string($fields)){
            $sql_field = " {$fields} ";
        }else{
            $sql_field = " * ";
        }

        return $sql_field;
    }

    /**
     * @param $sort
     * @return string
     */
    public function getSqlSort($sort,$im_str=','){
        $sql_sort = " ";
        if (!empty($sort)) {
            $sql_sort .= " order by ";
            $select_sort = array();
            foreach ($sort as $key => $val) {
                $select_sort[] = "{$key} {$val}";
            }
            $sql_sort .= implode($im_str, $select_sort);
        }
        return $sql_sort;
    }

    /**
     * 格式化生成limit SQL语句，如果$count为0，则返回空语句
     * @param int $index
     * @param int $count
     * @return string
     */
    public function formatLimitSql($index = 0, $count = 20) {
        $index  = intval($index);
        $count  = intval($count);

        $sql    = '';
        if ($count != 0) {
            $sql    .= " LIMIT {$index}, {$count} ";
        }
        return $sql;
    }
}