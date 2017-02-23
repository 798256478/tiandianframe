(function($){
    $(document).ready(function(){
        var request_uri     = $('#request-uri').val(),
            file_type       = $('#file-type').val(),
            upload_multi    = $('#upload-multi').val();
        $('#file-upload').uploadify({
            'formData'     : {
                'timestamp' : '222'
            },
            'fileObjName'   : 'uploadFile',
            'fileTypeExts'  : file_type,
            'buttonClass'   : 'btn btn-primary',
            'buttonText'    : '选择文件上传',
            'height'        : 40,
            'multi'         : upload_multi,
            'swf'           : '/modules/common/uploadify/uploadify.swf',
            'uploader'      : request_uri,
            'onUploadSuccess' : function(file, data, response) {
                var ret = $.parseJSON(data);
                if (ret.ec == 200) {
                    var image_field = $('#file-image');
                    if (image_field) {
                        image_field.attr('src', ret.url);
                    }
                    $('#file-path').val(ret.url);
                } else {
                    alert(ret.em);
                }
            }
        });
    });
})(jQuery);