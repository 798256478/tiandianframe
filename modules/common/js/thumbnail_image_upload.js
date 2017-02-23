(function($){
    /**
     * 定义缩略图处理类
     */
    var PlumThumbnail = {
        /**
         * 提供默认数据
         */
        vars : {
            thumb_width     : 200,
            thumb_height    : 200,
            dialog_width    : 960,
            dialog_height   : 720,
            thumb_style     : {
                'default' : {'name' : '默认宽高样式', 'wh' : '200|200'},
                'slide'  : {'name' : '杰出豫商幻灯尺寸', 'wh' : '640|160'},
                'news'  : {'name' : '杰出豫商报道缩略图', 'wh' : '200|160'}
            },
            dialog          : null,
            subdirectory    : 'story',
            image_alt       : '',
            coord_xywh      : '',
            thumb_preview   : null,//缩略图预览区域
            image_upload_base_url   : '/admin/image/upload/common',//图片上传URL
            thumb_upload_base_url   : '/admin/image/upload/thumbnail'//缩略图截取URL
        },
        /**
         * 数据初始化
         * @param options
         */
        initiate : function(options) {
            var self = this;
            self.vars.thumb_width   = options.thumb_width;
            self.vars.thumb_height  = options.thumb_height;
            self.vars.dialog_width  = Math.max(960, (360 + 50 + options.thumb_width));
            self.vars.dialog_height = 580;
            self.vars.subdirectory  = options.subdirectory;
            self.vars.image_alt     = options.image_alt;
            self.vars.thumb_style.default.wh = options.thumb_width + '|' + options.thumb_height;
            //弹窗处理
            self.vars.dialog = self._createDialog();
        },
        /**
         * 创建上传弹窗
         */
        _createDialog : function() {
            var self = this,
                choose = ['<select id="wh-style-select">'];
            for(var style in self.vars.thumb_style) {
                choose.push('<option value ="'+self.vars.thumb_style[style]['wh']+'">'+self.vars.thumb_style[style]['name']+'</option>');
            }
            choose.push('</select>');
            choose = choose.join('');

            var html = [
                '<div id="image-upload-panel">',
                '<div id="file-upload-box" style="display: inline-block;">',
                '<input type="file" name="files[upload-image]" id="image-upload-file">',
                '</div>',
                '<span><input type="button" value="图片空间" id="open-image-server" /></span>',
                '<div id="file-cut-size" style="display: inline;">',
                '<span>宽(px)：<input type="text" size="5" id="thumb-width" value="'+self.vars.thumb_width+'"/></span>',
                '<span>高(px)：<input type="text" size="5" id="thumb-height" value="'+self.vars.thumb_height+'" /></span>',
                '<span><input type="button" value="自定义宽高" id="custom-set-size" /></span>',
                '<span>或选择宽高样式：'+choose+'</span>',
                '</div>',
                '<hr style="margin: 2px 0;"/>',
                '</div>'
            ];

            var dialog = KindEditor.dialog({
                title: '裁切选取图片',
                width: self.vars.dialog_width,
                height: self.vars.dialog_height,
                body : html.join(''),
                closeBtn : {
                    name : '关闭',
                    click : function(e) {
                        dialog.remove();
                    }
                },
                yesBtn : {
                    name : '保存',
                    click : self._dialogSaveOper
                },
                noBtn : {
                    name : '取消',
                    click : function(e) {
                        dialog.remove();
                    }
                }
            });
            //图片文件上传监听
            $('#image-upload-file').on('change', null, self, self._uploadImageFile);
            //自定义宽、高监听
            $('#custom-set-size').on('click', null, self, self._customSetSize);
            //选择宽高样式监听
            $('#wh-style-select').on('change', null, self, self._selectThumbStyle);
            //打开图片空间监听
            $('#open-image-server').on('click', null, self, self._openImageServer);
            return dialog;
        },
        /**
         * 弹窗保存操作
         * @private
         */
        _dialogSaveOper : function() {
            var self = PlumThumbnail,
                $xywh = self.vars.coord_xywh,
                $src = $("#image-target").attr('src'),
                upload_data = {'xywh' : $xywh, 'src' : $src, 'thumbwh' : self.vars.thumb_width + '|' + self.vars.thumb_height},
                upload_url = self.vars.thumb_upload_base_url;
            if ($xywh && $src) {
                $.post(upload_url, upload_data, function(data, status) {
                    $("#image-tbl-display").attr('src', data['url']);
                    $('#list-thumbnail-image').val(data['url']);
                    /**
                     * 注释掉往kindeditor中增加图片
                     if (typeof window.editor != "undefined") {
                        var com_url = data.url.replace(/-tbl/, '');
                        window.editor.exec('insertimage', com_url, self.vars.image_alt);
                     }
                     * ****/

                });
                self.vars.dialog.remove();
            } else {
                alert("请选择图片区域");
            }
        },
        /**
         * 图片文件上传
         * @private
         */
        _uploadImageFile : function(event) {
            var self = event.data,
                upload_url = self.vars.image_upload_base_url + '/' + self.vars.subdirectory,
                file_element_id = 'image-upload-file',
                upload_data = {name : "upload-image"};
            $.ajaxFileUpload({
                url : upload_url,
                secureuri : false,
                type : 'post',
                fileElementId : file_element_id,
                dataType : 'json',
                data : upload_data,
                success : function(data, status) {
                    if (data['ec'] == 200) {
                        var image_box = [
                            '<img style="width: 360px" src="'+data['filepath']+'" id="image-target">',
                            '<div id="preview-pane">',
                            '<div class="preview-container" style="width: '+self.vars.thumb_width+'px;height: '+self.vars.thumb_height+'px;">',
                            '<img src="'+data['filepath']+'" class="jcrop-preview" id="image-preview" />',
                            '</div>',
                            '</div>',
                            '<div style="clear: both;">',
                            '</div>'
                        ];
                        $('#image-upload-panel').append(image_box.join(''));
                        self.vars.thumb_preview = $('#preview-pane .preview-container');
                        self._initJcrop();
                    } else {
                        alert(data['em']);
                    }
                },
                error : function(data, status, e) {

                }
            });
        },
        /**
         * 自定义宽、高
         * @private
         */
        _customSetSize : function(event) {
            var self = event.data,
                thumb_width = parseInt($('#thumb-width').val()),
                thumb_height = parseInt($('#thumb-height').val());

            if (thumb_width <= 0 || thumb_height <= 0) {
                alert('自定义宽高不能小于0px');
            } else {
                $('#thumb-wh').val(thumb_width + '|' + thumb_height);
                self.vars.thumb_width = thumb_width;
                self.vars.thumb_height = thumb_height;
                //重新初始化缩略图预览
                self._initJcrop();
                alert("自定义宽："+thumb_width+"px；高："+thumb_height+"px；--设置成功");
            }
        },
        _openImageServer : function (event) {
            var self = event.data,
                editor = KindEditor.editor({
                    fileManagerJson : '/admin/gallery/server/manage'
                });
            editor.loadPlugin('filemanager', function() {
                editor.plugin.filemanagerDialog({
                    viewType : 'VIEW',
                    dirName : '',
                    clickFn : function(url, title) {
                        editor.hideDialog();
                        var image_box = [
                            '<img style="width: 360px" src="'+url+'" id="image-target">',
                            '<div id="preview-pane">',
                            '<div class="preview-container" style="width: '+self.vars.thumb_width+'px;height: '+self.vars.thumb_height+'px;">',
                            '<img src="'+url+'" class="jcrop-preview" id="image-preview" />',
                            '</div>',
                            '</div>',
                            '<div style="clear: both;">',
                            '</div>'
                        ];
                        $('#image-upload-panel').append(image_box.join(''));
                        self.vars.thumb_preview = $('#preview-pane .preview-container');
                        self._initJcrop();
                    }
                });
            });
        },
        /**
         * 选择宽、高样式
         * @private
         */
        _selectThumbStyle : function(event) {
            var self = event.data,
                wh_val = $('#wh-style-select').val(),
                wh_arr = wh_val.split('|'),
                wh_w = parseInt(wh_arr[0]),
                wh_h = parseInt(wh_arr[1]);
            $('#thumb-wh').val(wh_val);
            self.vars.thumb_width = wh_w;
            self.vars.thumb_height = wh_h;
            //重新初始化缩略图预览
            self._initJcrop();
            //调整input数值
            $('#thumb-width').val(wh_w);
            $('#thumb-height').val(wh_h);
        },
        /**
         * 初始化Jcrop工具
         */
        _initJcrop : function() {
            var self = this,
                xsize = self.vars.thumb_width,
                ysize = self.vars.thumb_height,
                thumb_preview = self.vars.thumb_preview;
            //缩略图预览区域存在时进行初始化
            if (thumb_preview) {
                thumb_preview.width(xsize).height(ysize);
                $('#image-target').Jcrop({
                    onChange: self._updatePreview,
                    onSelect: self._updatePreview,
                    aspectRatio: xsize / ysize
                });
            }
        },
        /**
         * 选择缩略图范围监听函数
         */
        _updatePreview : function(coords) {
            var self = PlumThumbnail,
                rx = self.vars.thumb_width / coords.w,
                ry = self.vars.thumb_height / coords.h,
                $imgtg = $("#image-target");

            $('#image-preview').css({
                width: Math.round(rx * $imgtg.width()) + 'px',
                height: Math.round(ry * $imgtg.height()) + 'px',
                marginLeft: '-' + Math.round(rx * coords.x) + 'px',
                marginTop: '-' + Math.round(ry * coords.y) + 'px'
            });
            self.vars.coord_xywh = "x="+coords.x+"|y="+coords.y+"|w="+coords.w+"|h="+coords.h;
        }
    };
    /**
     * 主函数，调用
     */
    $(document).ready(function(){
        $('div#add-image-panel').click(function(){
            //定义初始化需要的数据
            var thumb_arr       = $('#thumb-wh').val().split('|'),//定义截取的缩略图宽，高
                thumb_width     = parseInt(thumb_arr[0]),
                thumb_height    = parseInt(thumb_arr[1]),
                sub_directory   = $('#sub-dir').val(),//上传子目录
                image_title     = $('#img-title').val();//img标签title
            //初始化操作
            PlumThumbnail.initiate({
                thumb_width     : thumb_width,
                thumb_height    : thumb_height,
                subdirectory    : sub_directory,
                image_alt       : image_title
            });

        });
        var verify = $('#form-verify').val();

        var form_verify     = verify.split('|'),
            form_id         = form_verify[0],
            form_status     = form_verify[1];
        $('#'+form_id).submit(form_status, function(event) {
            var thumb_val = $('#list-thumbnail-image').val();
            if (event.data == '1' && !thumb_val) {
                alert("未上传缩略图");
                return false;
            }
            return true;
        });
    });
}(jQuery));