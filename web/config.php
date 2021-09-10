<?php

return [
    /**
     * --------------------------------------------------------------------------
     * Site base URL
     * --------------------------------------------------------------------------
     *
     * This base URL will be used to generate links for the @url blade
     * directive, it should always start with a forward slash.
     *
     * WARNING: There is no nedd to change the base_url
     * 
     * The base_url is being automatically set every build via .gitlab-ci.yml
     * Please see pull-check-compose.sh to see how,
     * 
     */

    'base_url' => '/placeholder',

    /**
     * --------------------------------------------------------------------------
     * Enabling the blog generator
     * --------------------------------------------------------------------------
     *
     * When the blog generator is enabled, blade views in "content/_blog" will
     * be parsed to get blog posts data and generate blog pagination views.
     *
     * Disable this option if you don't need  a blog for your website to
     * get a performance boost.
     *
     */

    'enableBlog' => false,

    /**
     * --------------------------------------------------------------------------
     * The number of posts in a page
     * --------------------------------------------------------------------------
     *
     * The number of posts to appear on every page in the blog pages.
     *
     */
    'postsPerPage' => 3,

    /**
     * --------------------------------------------------------------------------
     * The view used to display blog paginated pages
     * --------------------------------------------------------------------------
     *
     * This blade view will be used to generate the paginated list of
     * posts for the blog.
     *
     */

    'postsListView' => 'blog',

    /**
     * --------------------------------------------------------------------------
     * Custom variables
     * --------------------------------------------------------------------------
     *
     * Here you can set all the custom variables you'd like to be present in
     * the blade views.
     *
     */

    'siteName' => 'Blue Hawk',

    'siteDescription' => 'A Web browser',

    'footerDate' => date('Y'),

    'releaseDate' => date('l jS \of F Y h:i:s A'),

    'appArchive' => 'bhawk.zip',

    'appChecksum' => 'app-checksum',

];