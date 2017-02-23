KindEditor.ready(function(K) {
    var uploadbutton = K.uploadbutton({
        button : K('#image-upl-btn'),
        fieldName : 'files[imgFile]',
        url : '/admin/image/upload/common/client',
        afterUpload : function(data) {
            if (data.error === 0) {
                K('#image-preview').attr('src', data.url);
                K('#img-url').val(data.filepath);
            } else {
                alert(data.message);
            }
        },
        afterError : function(str) {
            alert('自定义错误信息: ' + str);
        }
    });
    uploadbutton.fileBox.change(function(e) {
        uploadbutton.submit();
    });
});
