KindEditor.ready(function(K) {
    var field_name  = K('#field-name').val(),
        sub_dir     = K('#sub-dir').val(),
        qiniu_bucket    = K('#qiniu-bucket').val();
    var editor = K.editor({
        allowFileManager : false,
        uploadJson : '/admin/common/image/upload/'+sub_dir+'/'+qiniu_bucket,
        imageUploadLimit : 10,
        filePostName : 'imgFile',
        imageSizeLimit : '2MB'
    });
    K('#multi-image-upload-button').click(function() {
        editor.loadPlugin('multiimage', function() {
            editor.plugin.multiImageDialog({
                clickFn : function(urlList) {
                    var div = K('#image-display-box');
                    K.each(urlList, function(i, data) {
                        var img = '<span><img src="' + data.url + '" class="img-thumbnail" width="160px">'+
                                    '<input type="hidden" name="'+field_name+'[]" value="' + data.url + '"/>'+
                                    '<span class="close-img">X</span><span class="check-img"></span></span>';
                        div.append(img);
                    });
                    editor.hideDialog();
                }
            });
        });
    });
});

(function($){
    $(document).ready(function(){
        $("#image-display-box").on("mouseover",".img-thumbnail,span.close-img",function(){
            $(this).parent().find("span.close-img").css("display","inline-block");
        });
        $("#image-display-box").on("mouseout",".img-thumbnail,span.close-img",function(){
            $(this).parent().find("span.close-img").css("display","none");
        });

        $("#image-display-box").on("click","span.close-img",function(){
            $(this).parent().remove();
        });
    });
}(jQuery));

