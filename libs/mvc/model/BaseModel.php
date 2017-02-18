<?php
/**
* 
*/
class Libs_Mvc_Model_BaseModel extends Lib_Mysql_PlumTable
{
	public $_shopId;
	public function __construct()
	{
		parent::__construct();
	}

	    public function getRow($where) {
        if($this->_df){
            $where[] = array('name' => $this->_df, 'oper' => '=', 'value' => 0);
        }
        $sql = $this->formatSelectOneSql($where);
        $row = DB::fetch_first($sql);
        if ($row === false) {
            trigger_error("query mysql failed.", E_USER_ERROR);
            return false;
        }
        return $row;
    }

    /**
     * @param array $where
     * @param int $index
     * @param int $count
     * @param array $sort
     * @param array $field
     * @param bool $primary 是否使用主键作为结果数组的索引
     * @return array|bool
     */
    public function getList($where = array(), $index = 0, $count = 20, $sort = array(), $field = array(), $primary = false) {
        if($this->_df){
            $where[] = array('name' => $this->_df, 'oper' => '=', 'value' => 0);
        }
        $sql = $this->formatSelectSql($field, $where, $sort, $index, $count);
        $keyfield = '';
        if ($primary) {
            $keyfield   = $this->_pk;
        }
        $ret = DB::fetch_all($sql, array(), $keyfield);
        if ($ret === false) {
            trigger_error("query mysql failed.", E_USER_ERROR);
            return false;
        }
        return $ret;
    }


    public function getCount($where = array()) {
        if($this->_df){
            $where[] = array('name' => $this->_df, 'oper' => '=', 'value' => 0);
        }
        $sql = $this->formatCountSql($where);
        $ret = DB::result_first($sql);

        if ($ret === false) {
            trigger_error("query mysql failed.", E_USER_ERROR);
            return false;
        }
        return $ret;
    }

    public function insertValue(array $data) {
        $ret = $this->insert($data, true);
        if ($ret === false) {
            trigger_error("query mysql failed.", E_USER_ERROR);
            return false;
        }
        return $ret;
    }

    public function updateValue($set, $where){
        if(empty($where)){
            trigger_error("Update the mysql  where conditions cannot be empty");
            return false;
        }

        $sql = $this->formatUpdateSql($set,$where);
        $ret = DB::query($sql);

        if ($ret === false) {
            trigger_error("query mysql failed.", E_USER_ERROR);
            return false;
        }
        return $ret;
    }

    public function deleteValue($where) {
        if(empty($where)){
            trigger_error("Delete the mysql  where conditions cannot be empty");
            return false;
        }
        $sql = $this->formatDeleteSql($where);
        $ret = DB::query($sql);

        if ($ret === false) {
            trigger_error("query mysql failed.", E_USER_ERROR);
            return false;
        }
        return $ret;
    }

    public function getRowById($id){
        $where   = array();
        $where[] = array('name' => $this->_pk,'oper' => '=','value' =>$id);
        return $this->getRow($where);
    }

    public function updateById($set,$id){
        if($id){
            $where   = array();
            $where[] = array('name' => $this->_pk,'oper' => '=','value' =>$id);
            return $this->updateValue($set,$where);
        }
    }
    public function getRowByIdSid($id,$sh_id){
        $where   = array();
        $where[] = array('name' => $this->_pk,'oper' => '=','value' =>$id);
        $where[] = array('name' => $this->_shopId,'oper' => '=','value' =>$sh_id);
        return $this->getRow($where);
    }

        /**
     * @param $id
     * @param $sh_id
     * @param array $data
     * @return array|bool
     * 获取或修改单行数据
     */
    public function getRowUpdateByIdSid($id,$sh_id,$data=array()){
        $where   = array();
        $where[] = array('name' => $this->_pk,'oper' => '=','value' =>$id);
        $where[] = array('name' => $this->_shopId,'oper' => '=','value' =>$sh_id);
        if(!empty($data)){
            return $this->updateValue($data,$where);
        }else{
            return $this->getRow($where);
        }
    }


    public function deleteById($id){
        if($id){
            $where   = array();
            $where[] = array('name' => $this->_pk,'oper' => '=','value' =>$id);
            return $this->deleteValue($where);
        }
        return false;
    }
    
    public function deleteBySidId($id,$sh_id){
        if($id && $sh_id){
            $where   = array();
            $where[] = array('name' => $this->_pk,'oper' => '=','value' =>$id);
            $where[] = array('name' => $this->_shopId,'oper' => '=','value' =>$sh_id);
            return $this->deleteValue($where);
        }
        return false;
    }
}