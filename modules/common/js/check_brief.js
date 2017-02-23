/**
 * Created by thomas on 14/11/27.
 */
(function($){
    $(document).ready(function(){
        //表单验证
        var brief_length    = $('#brief-length').val().split('|'),
            form_verify     = $('#brief-verify').val().split('|'),
            form_id         = form_verify[0],
            form_verify_type= parseInt(form_verify[1]);
        if (form_verify_type) {
            $('#'+form_id).submit(function() {
                var min = parseInt(brief_length[0]),
                    max = parseInt(brief_length[1]);
                var length = $('input.brief-check-field').first().val().trim().length;
                if (length < min || length > max) {
                    alert("摘要不符合规范，请完善");
                    return false;
                }
                return true;
            });
        }
        $('input.brief-check-field').keyup(function() {
            var self = this;
            setBriefMsg(self);
        }).change(function() {
            var self = this;
            setBriefMsg(self);
        }).keydown(function() {
            var self = this;
            setBriefMsg(self);
        }).focus(function() {
            var self = this;
            setBriefMsg(self);
        });

        function setBriefMsg(input) {
            var brief = input.value.trim();
            var length = brief.length;
            var max_length = brief_length[1];
            if (length > max_length || length != input.value.length) {
                brief = brief.substr(0, max_length).trim();
                $(input).val(brief);
                length = brief.length;
            }
            var contentMsg = $('#brief-msg');
            contentMsg.html("您已输入<font color='red'>"
                + length
                + "</font>字符，还可输入<font color='red'>"
                + (max_length - length)
                + "</font>字符。");
        }
    });
})(jQuery);