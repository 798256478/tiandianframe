(function($) {
    $(function () {
        //表单验证
        var form_verify     = $('#auto-form-verify').val().split('|'),
            form_id         = form_verify[0],
            form_verify_type= parseInt(form_verify[1]);
        if (form_verify_type) {
            $('#'+form_id).submit(function() {
                var desc = $('#nope-auto-label').html(),
                    item = $('#nope-auto-completer').val();
                if (!item) {
                    alert(desc + "不能为空");
                    return false;
                }
                return true;
            });
        }
        var path_limit      = $('#auto-request-limit').val().split('|'),
            request_path    = path_limit[0],
            list_limit      = parseInt(path_limit[1]);
        $('#nope-auto-completer').autocompleter({
            source: request_path,
            limit: list_limit,
            delay: 5,
            combine: function () {
                if ($('.auto-combine-type')) {
                    return {extra: $('.auto-combine-type').val()};
                }
                return {};
            },
            callback: function (value, index, selected) {
                if (selected) {
                    $('.nope-auto-icon').html(selected.label);
                }
            }
        });
    });
})(jQuery);
