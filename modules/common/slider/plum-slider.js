jQuery(function($) {
    var weight_config   = $('#plum-weight-config-value').val().split('|'),
        cfg_min         = parseInt(weight_config[0]),
        cfg_max         = parseInt(weight_config[1]),
        cfg_step        = parseInt(weight_config[2]),
        cfg_value       = parseInt(weight_config[3]);

    $(".plum-slider").slider({
        animate: "fast",
        min: cfg_min,
        max: cfg_max,
        value: cfg_value,
        step: cfg_step,
        slide: function (event, ui) {
            $("#plum-edit-number-weight").val(ui.value);
        }
    }).slider("pips").slider("float");
});