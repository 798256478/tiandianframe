(function($){
    $(document).ready(function(){
        KindEditor.ready(function(K) {
            var width = $('fieldset').first().width() - 30,
                sub_dir = $('#sub-dir').val(),
                ke_textarea_name = $("input[name=ke_textarea_name]");
            if (ke_textarea_name.length > 1) {
                ke_textarea_name.each(function(index, element) {
                    var name = $(element).val();
                    K.create('textarea[name="'+name+'"]', {
                        width : width,
                        uploadJson : '/admin/image/upload/common/' + sub_dir,
                        filePostName : 'files[imageFile]',
                        filterMode : false,
                        basePath : '/modules/common/kindeditor/',
                        items : [
                            'source', '|',
                            'undo', 'redo', '|',
                            'plainpaste', 'quickformat', 'preview', '|',
                            'print', 'template', 'shcode', 'wordpaste', '|',
                            'image', 'multiimage', 'hr', 'table', 'emoticons', 'pagebreak', 'anchor', 'link', 'unlink', '/',
                            'formatblock', 'fontname', 'fontsize', '|',
                            'forecolor', 'hilitecolor', 'bold', 'italic', 'underline', 'strikethrough', 'lineheight', 'removeformat','|',
                            'justifyleft', 'justifycenter', 'justifyright', 'justifyfull', 'insertorderedlist', 'insertunorderedlist', 'indent', 'outdent', 'subscript', 'superscript', 'clearhtml', 'selectall', '|',
                            'about'
                        ]
                    });
                });
            } else {
                window.editor = K.create('#ke-txt-content', {
                    width : width,
                    height : 500,
                    uploadJson : '/admin/image/upload/common/' + sub_dir,
                    filePostName : 'files[imageFile]',
                    filterMode : false,
                    basePath : '/modules/common/kindeditor/',
                    items : [
                        'source', '|',
                        'undo', 'redo', '|',
                        'plainpaste', 'quickformat', 'preview', '|',
                        'print', 'template', 'shcode', 'wordpaste', '|',
                        'image', 'multiimage', 'hr', 'table', 'emoticons', 'pagebreak', 'anchor', 'link', 'unlink', '/',
                        'formatblock', 'fontname', 'fontsize', '|',
                        'forecolor', 'hilitecolor', 'bold', 'italic', 'underline', 'strikethrough', 'lineheight', 'removeformat','|',
                        'justifyleft', 'justifycenter', 'justifyright', 'justifyfull', 'insertorderedlist', 'insertunorderedlist', 'indent', 'outdent', 'subscript', 'superscript', 'clearhtml', 'selectall', '|',
                        'about'
                    ]
                });
            }
        });
    });
}(jQuery));
