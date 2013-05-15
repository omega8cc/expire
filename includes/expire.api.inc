<?php

/**
 * @file
 * Provides internal API for page cache flushes.
 */

class ExpireAPI {

  /**
   * Flush page cache for a given urls.
   *
   * @param $urls
   *   List of urls that should be flushed.
   */
  public static function flushCache($urls) {
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
   * Find all taxonomy terms in node fields and build urls for them.
   *
   * @param $node
   *   Node object.
   *
   * @return array
   *   Term urls that should be flushed.
   */
  public  static function expireNodeTermPages($node) {

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
