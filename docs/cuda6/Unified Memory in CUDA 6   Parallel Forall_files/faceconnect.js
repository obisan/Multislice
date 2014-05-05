var fcCommentPopupShown = false;

window.fbAsyncInit = function() {
	FB.init({
		appId   : fc_app_id, // make sure this is output to the page before this script is included
		status  : true, // check login status
		cookie  : true, // enable cookies to allow the server to access the session
		xfbml   : true // parse XFBML
	});

	FB.Event.subscribe('auth.login', function() {
		window.location.reload();
	});

	FB.Event.subscribe('auth.logout', function() {
		window.location.reload();
	});

};

(function() {
	var e = document.createElement('script');
	e.src = document.location.protocol + '//connect.facebook.net/en_US/all.js';
	e.async = true;
	var root = document.getElementById('fb-root');
	if (root != null) {
		root.appendChild(e);
	}
}());

function fcPostCommentToWall(postTitle, postExcerpt, postPermalink) {
	FB.ui(
		{
			method: 'stream.publish',
			message: jQuery('#comment').val(),
			attachment: {
				name: postTitle,
				caption: "\n",
				description: postExcerpt,
				href: postPermalink
			},
			user_message_prompt: 'Share your comment on ' + postTitle
		},
		function(response) {
			fcCommentPopupShown = true;
			jQuery('#submit').click();
		}
	);
}