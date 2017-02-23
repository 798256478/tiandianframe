(function() {
     if (! 
     /*@cc_on!@*/
     0) return;
     var e = "abbr, article, aside, audio, canvas, datalist, details, dialog, eventsource, figure, footer, header, hgroup, mark, menu, meter, nav, output, progress, section, time, video".split(', ');
     var i= e.length;
     while (i--){
         document.createElement(e[i])
     } 
})();


(function($) {
	$(function(){

		$(".col-left").delegate("#admin-sidebar-menu > li > a","click",function(event){
			if(event.preventDefault){
				event.preventDefault();
			}else{
				event.returnValue=false;
			}
			
			$(this).parent().toggleClass("expandable");
		});
		$(".col-left>ul>li.expandable>.dropdown li").mouseover(function(){
			$(this).find("ul:eq(0)").show();
			// $(this).find("ul:eq(0)").css("display","block");
		}).mouseout(function(){
			$(this).find("ul:eq(0)").hide();
			// $(this).find("ul:eq(0)").css("display","none");
		});

	});
})(jQuery);

