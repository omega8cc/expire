<?php

/**
 * @file
 * API documentation for Cache Expiration module.
 */

/**
 * Provides possibility to flush pages for external cache storages.
 *
 * @param $urls
 *   List of internal urls that should be flushed.
 *
 * @param $object_type
 *  Name of object type ('node', 'comment', 'user', etc).
 *
 * @param $object
 *   Object (node, comment, user, etc) for which expiration is executes.
 *
 * @see expire.api.inc
 */
function hook_expire_cache($urls, $object_type, $object) {
  foreach ($urls as $url) {
    $full_path = url($url, array('absolute' => TRUE));
    purge_url($full_path);
  }
}

/**
 * Provides possibility to change urls before they are expired.
 *
 * @param $urls
 *   List of internal urls that should be flushed.
 *
 * @param $object_type
 *  Name of object type ('node', 'comment', 'user', etc).
 *
 * @param $object
 *   Object (node, comment, user, etc) for which expiration is executes.
 *
 * @see expire.api.inc
 */
function hook_expire_cache_alter($urls, $object_type, $object) {
  if ($object_type == 'node') {
    unset($urls['node-' . $object->nid]);
    $urls['example'] = 'custom_page/' . $object->uid . '/' . $object->nid;
  }
}
