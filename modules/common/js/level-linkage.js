(function($){
    $(document).ready(function(){
        var currentShow = 0;
        $("#first-select").change(function(){
            $("#first-select option").each(function(i, o){
                if(this.selected) {
                    $('#first-level').val($(this).val());
                    $(".second-select").hide();
                    $(".second-select").eq(i).show();

                    $(".second-select").each(function(j, p){
                        if(i == j){
                            $('#second-level').val($('.second-select').eq(i).val());
                        }
                    });
                    currentShow = i;
                }
            });
        });
        $("#first-select").change();

        $('.second-select').change(function(i, o) {
            $('#second-level').val($(this).val());
        });

        //表单验证
        var form_verify         = $('#level-verify').val().split('|'),
            form_id             = form_verify[0],
            form_first_verify   = form_verify[1],
            form_second_verify  = form_verify[2];
        $('#'+form_id).submit(function() {
            if (form_first_verify==1 && $('#first-level').val() == '0') {
                alert('请选择主分类');
                return false;
            }
            if (form_second_verify==1 && $('#second-level').val() == '0') {
                alert('请选择子分类');
                return false;
            }
            return true;
        });
    });
})(jQuery);