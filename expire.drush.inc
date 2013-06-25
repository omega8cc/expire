<?php

/**
 * @file
 * This is the drush integration for the expire module.
 */

/**
 * Implementats hook_drush_command().
 */
function expire_drush_command() {

  $items['expire-url'] = array(
    'description' => 'Expire fully qualified URLs.',
    'arguments' => array(
      'urls' => 'URLs to expire separated by spaces.',
    ),
    'examples' => array(
      'drush expire-url http://example.com/testpage.html' => 'Expire a single URL.',
      'drush xu http://example.com/ http://test.com/logo.jpg' => 'Expire multiple URLs.',
    ),
    'aliases' => array('xu'),
    'drupal dependencies' => array('expire'),
    'callback' => 'drush_expire_url',
  );

  $items['expire-path'] = array(
    'description' => 'Expire a drupal path.',
    'arguments' => array(
      'paths' => 'A list drupal paths to expire separated by spaces.',
    ),
    'examples' => array(
      'drush expire-path node/123' => 'Expire a single drupal path.',
      'drush expire-path FRONT' => 'Expire the front page.',
      'drush xp FRONT node/234 contact' => 'Expire multiple drupal paths.',
    ),
    'aliases' => array('xp'),
    'drupal dependencies' => array('expire'),
    'callback' => 'drush_expire_path',
  );

  // @todo: implement other commands for entity expiration.

  return $items;
}

/**
 * Callback for expire-url drush command.
 */
function drush_expire_url() {

  // Get drush params.
  $full_urls = drush_get_arguments();
  unset($full_urls[0]);

  // Expire urls.
  ExpireAPI::executeExpiration($full_urls);
}

/**
 * Callback for expire-path drush command.
 */
function drush_expire_path() {

  $paths = drush_get_arguments();
  unset($paths[0]);

  $full_urls = array();
  foreach ($paths as $path) {
    $url = $path === 'FRONT' ? '<front>' : $path;
    $full_urls[] = url($url, array('absolute' => TRUE));
  }

  // Expire urls.
  // @todo: flag about full or internal url?
  ExpireAPI::executeExpiration($full_urls);
}
