jQuery(function($) {

	// Requires jquery.placeholder.min.js
	// Add support for placeholder attribute in older browsers.
	$('.search-input').placeholder();

	$('a.sub-email').click(function(event){
		$('.email-box').show();
		return false;
	});

	$('a.close').click(function(event){
		$('.email-box').hide();
		return false;
	});

});
