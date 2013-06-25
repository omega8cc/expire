<?php

/**
 * @file
 * Provides internal API for page cache flushes.
 */

class ExpireAPI {

  /**
   * Executes internal or external cache expiration.
   *
   * @param $urls
   *   List of internal urls that should be expired.
   *
   * @param $object_type
   *  Name of object type ('node', 'comment', 'user', etc).
   *
   * @param $object
   *   Object (node, comment, user, etc) for which expiration is executes.
   */
  public static function executeExpiration($urls, $object_type = '', $object = NULL) {

    // Allow other modules to modify the list prior to expiring
    drupal_alter('expire_cache', $urls, $object_type, $object);

    $status = variable_get('expire_status', EXPIRE_STATUS_DISABLED);
    if ($status == EXPIRE_STATUS_ENABLED_INTERNAL) {
      self::executeInternalExpiration($urls);
    }
    elseif ($status == EXPIRE_STATUS_ENABLED_EXTERNAL) {
      self::executeExternalExpiration($urls, $object_type, $object);
    }
  }

  /**
   * Execute internal urls expiration.
   * Calls cache_clear_all().
   *
   * @param $urls
   *   List of internal urls that should be flushed.
   */
  protected static function executeInternalExpiration($urls) {
    foreach ($urls as $url) {

      // Every URL may contain |wildcard suffix, so we should check this.
      $url_parts = explode('|', $url);
      if (count($url_parts) > 1) {
        $wildcard = TRUE;
        array_pop($url_parts);
        $url = implode('|', $url_parts);
      }
      else {
        $wildcard = FALSE;
      }

      $url = url($url, array('absolute' => TRUE));

      // Clear cached page data.
      cache_clear_all($url, 'cache_page', $wildcard);
    }
  }

  /**
   * Executes hook_cache_expire().
   *
   * It allows other modules to implement custom login for cache expiration.
   *
   * @param $urls
   *   List of internal urls that should be flushed.
   *
   * @param $object_type
   *  Name of object type ('node', 'comment', 'user', etc).
   *
   * @param $object
   *   Object (node, comment, user, etc) for which expiration is executes.
   */
  protected static function executeExternalExpiration($urls, $object_type, $object) {
    $modules = module_implements('expire_cache');
    foreach ($modules as $module) {
      module_invoke($module, 'expire_cache', $urls, $object_type, $object);
    }
  }

  /**
   * Find all taxonomy terms in node fields and build urls for them.
   *
   * @param $node
   *   Node object.
   *
   * @return array
   *   Term urls that should be flushed.
   */
  public static function expireNodeTermPages($node) {

    $terms = array();
    $field_info = field_info_fields();
    $field_instances = field_info_instances('node', $node->type);

    foreach ($field_instances as $name => $instance) {
      if ($field_info[$name]['type'] == 'taxonomy_term_reference') {
        $new_terms = field_get_items('node', $node, $name);
        if (is_array($new_terms) && !empty($new_terms)) {
          $terms = array_merge($new_terms, $terms);
        }
        $old_terms = isset($node->original) && !empty($node->original) ? field_get_items('node', $node->original, $name) : array();
        if (is_array($old_terms) && !empty($old_terms)) {
          $terms = array_merge($old_terms, $terms);
        }
      }
    }

    $urls = array();
    foreach ($terms as $term) {
      $urls['term-' . $term['tid']] = 'taxonomy/term/' . $term['tid'];
    }

    return $urls;
  }

  /**
   * Find all node references in node fields and build urls for them.
   *
   * @param $node
   *   Node object.
   *
   * @return array
   *   Node urls that should be flushed.
   */
  public static function expireNodeReferences($node) {

    $references = array();
    $field_info = field_info_fields();
    $field_instances = field_info_instances('node', $node->type);

    foreach ($field_instances as $name => $instance) {
      if ($field_info[$name]['type'] == 'node_reference') {
        $new_references = field_get_items('node', $node, $name);
        if (is_array($new_references) && !empty($new_references)) {
          $references = array_merge($new_references, $references);
        }
        $old_references = isset($node->original) && !empty($node->original) ? field_get_items('node', $node->original, $name) : array();
        if (is_array($old_references) && !empty($old_references)) {
          $references = array_merge($old_references, $references);
        }
      }
    }

    $urls = array();
    foreach ($references as $reference) {
      $urls['reference-' . $reference['nid']] = 'node/' . $reference['nid'];
    }

    return $urls;
  }

  /**
   * Create expiration urls for custom pages.
   *
   * @param $pages
   *   Unformated string from user input raw.
   *
   * @param $token_options
   *   Options for token replacements.
   *
   * @return array
   *   List of custom urls that should be flushed.
   */
  public static function expireCustomPages($pages, $token_options) {

    $urls = array();

    $pages = explode("\n", $pages);
    foreach ($pages as $index => $page) {
      $page = trim($page);
      if (!empty($page)) {

        // Replace only urls with tokens.
        if (token_scan($page)) {
          $page = token_replace($page, $token_options);
        }

        $urls['custom-' . $index] = $page;
      }
    }

    return $urls;
  }

}
