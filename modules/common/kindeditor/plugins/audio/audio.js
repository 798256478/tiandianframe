
KindEditor.plugin('audio', function(K) {
    var self = this, name = 'audio';

    self.plugin.audioDialog = function(options) {
        var uploadJson = K.undef(options.uploadJson, '/admin/audio/upload/common/story'),
            filePostName = 'files['+K.undef(options.showLocal, 'audioFile')+']',
            tabIndex = K.undef(options.tabIndex, 0);
        var target = 'kindeditor_upload_iframe_' + new Date().getTime();
        var html = [
            '<div style="padding:20px;">',
            //tabs
            '<div class="tabs"></div>',
            //remote image - start
            '<div class="tab1" style="display:none;">',
            //url
            '<div class="ke-dialog-row">',
            '<label for="remoteUrl" style="width:60px;">音频地址</label>',
            '<input type="text" id="remoteUrl" class="ke-input-text" name="url" value="" style="width:200px;" /> &nbsp;',
            '</div>',
            '</div>',
            //remote image - end
            //local upload - start
            '<div class="tab2" style="display:none;">',
            '<iframe name="' + target + '" style="display:none;"></iframe>',
            '<form class="ke-upload-area ke-form" method="post" enctype="multipart/form-data" target="' + target + '" action="' + K.addParam(uploadJson, 'dir=image') + '">',
            //file
            '<div class="ke-dialog-row">',
            '<label style="width:60px;">上传文件</label>',
            '<input type="text" name="localUrl" class="ke-input-text" tabindex="-1" style="width:200px;" readonly="true" /> &nbsp;',
            '<input type="button" class="ke-upload-button" value="浏览文件" />',
            '</div>',
            '</form>',
            '</div>',
            //local upload - end
            '</div>'
        ].join('');

        var dialog = self.createDialog({
            width : 500,
            height: 200,
            title : '添加音频',
            body : html,
            yesBtn : {
                name : '确定',
                click : function(e) {
                    if (tabs.selectedIndex == 0) {//第一个标签
                        var audio_src = K('#remoteUrl').val();
                    } else if (tabs.selectedIndex == 1) {
                        var audio_src = K('input[name=localUrl]').val();
                    }
                    if (!audio_src) {
                        alert('请添加音频文件');
                    } else {
                        var $audio = '<div class="raw-upload-audio" style="width: 90%;"><audio src="'+audio_src+'" controls="controls">您的设备不支持播放</audio></div>';
                        self.insertHtml($audio);
                        self.hideDialog();
                    }
                }
            }
        });
        var div = dialog.div;
        var localUrlBox = K('[name="localUrl"]', div);
        var tabs;
        tabs = K.tabs({
            src : K('.tabs', div),
            afterSelect : function(i) {}
        });
        tabs.add({
            title : '网络音频',
            panel : K('.tab1', div)
        });
        tabs.add({
            title : '本地上传',
            panel : K('.tab2', div)
        });
        tabs.select(tabIndex);

        var uploadbutton = K.uploadbutton({
            button : K('.ke-upload-button', div)[0],
            fieldName : filePostName,
            form : K('.ke-form', div),
            target : target,
            width: 60,
            afterUpload : function(data) {
                dialog.hideLoading();
                if (data.ec == 200) {
                    localUrlBox.val(data.filepath);
                } else {
                    alert(data.em);
                }
            },
            afterError : function(html) {
                dialog.hideLoading();
                self.errorDialog(html);
            }
        });
        uploadbutton.fileBox.change(function(e) {
            if (uploadbutton.fileBox.val() == '') {
                alert('请选择文件上传');
                return;
            }
            dialog.showLoading('上传中');
            uploadbutton.submit();
        });
        return dialog;
    };
});