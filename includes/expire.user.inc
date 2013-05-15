<?php

/**
 * @file
 * Provides class that expires users.
 */

class ExpireUser implements ExpireInterface {

  /**
   * Executes expiration actions for user.
   *
   * @param $user
   *   User object.
   *
   * @param $action
   *   Action that has been executed.
   */
  public static function expire($user, $action) {

    if (empty($user->uid)) {
      return;
    }

    $enabled_actions = variable_get('memcache_storage_pc_user_actions', array());
    $enabled_actions = array_filter($enabled_actions);

    // Do not process current action if it is not selected by administrator.
    if (!in_array($action, $enabled_actions)) {
      return;
    }

    $expire_urls = array();

    // Expire front page.
    $expire_front_page = variable_get('memcache_storage_pc_user_expire_front_page', MEMCACHE_STORAGE_PC_USER_EXPIRE_FRONT_PAGE);
    if ($expire_front_page) {
      $expire_urls['front'] = '<front>';
    }

    // Expire user page.
    $expire_user_page = variable_get('memcache_storage_pc_user_expire_user_page', MEMCACHE_STORAGE_PC_USER_EXPIRE_USER_PAGE);
    if ($expire_user_page) {
      $expire_urls['user-' . $user->uid] = 'user/' . $user->uid;
    }

    // Expire custom pages.
    $expire_custom_urls = variable_get('memcache_storage_pc_user_expire_custom', MEMCACHE_STORAGE_PC_USER_EXPIRE_CUSTOM);
    if ($expire_custom_urls) {
      $pages = variable_get('memcache_storage_pc_user_expire_custom_pages');
      $urls = ExpireAPI::expireCustomPages($pages, array('user' => $user));
      $expire_urls = array_merge($expire_urls, $urls);
    }

    // Flush page cache for expired urls.
    if (!empty($expire_urls)) {
      ExpireAPI::flushCache($expire_urls);
    }
  }

}
