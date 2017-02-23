jQuery(function($) {
    $(document).ready(function() {
        var request_level   = $('#plum-request-level').val().split('|'),
            request_url     = request_level[0],
            level_number    = parseInt(request_level[1]);

        var higher_level    = 0;
        $('.plum-level-value').each(function(index, element) {
            var current_value = parseInt($(element).val());

            if (index == 0 || current_value != 0) {
                $.ajax({
                    type    : 'POST',
                    url     : request_url,
                    data    : {level: index, extra: higher_level},
                    dataType: 'json',
                    success : function(result) {
                        if (result.ec == 200) {
                            var ret         = result.data,
                                current_dom = $('#plum-level-select'+index);
                            for (var i = 0; i < ret.length; i++ ) {
                                current_dom.append('<option value="'+ret[i]['value']+'">'+ret[i]['label']+'</option>');
                            }
                            current_dom.val(current_value);
                        }
                    }
                });
            }
            higher_level = current_value;
        });

        $('.plum-level-select').change(function(event) {
            var level   = parseInt($(event.target).attr('data-value')) + 1,
                extra   = parseInt($(event.target).val()),
                level_dom   = $('#plum-level-select'+level);

            for (var j = level - 1; j < level_number; j++) {
                if (j == (level - 1)) {
                    $('#plum-level-value'+j).val(extra);
                } else {
                    $('#plum-level-value'+j).val(0);
                    $('#plum-level-select'+j).find('option').each(function(index, element) {
                        if (index != 0) {
                            $(element).remove();
                        }
                    });
                }
            }

            if (level < level_number) {
                if (extra == 0) {
                    return false;
                }
                $.ajax({
                    type    : 'POST',
                    url     : request_url,
                    data    : {level: level, extra: extra},
                    dataType: 'json',
                    success : function(result) {
                        if (result.ec == 200) {
                            var ret         = result.data;
                            for (var i = 0; i < ret.length; i++ ) {
                                level_dom.append('<option value="'+ret[i]['value']+'">'+ret[i]['label']+'</option>');
                            }
                        }
                    }
                });
            }
        });
    });
});