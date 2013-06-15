<?php

/**
 * @file
 * Provides class that expires voted objects' pages.
 */

class ExpireVotingapi implements ExpireInterface {

  /**
   * Common expiry logic for VotingAPI votes.
   */
  public function expire($votes, $action) {

    if (empty($votes)) {
      return;
    }

    // Some vote actions may be executed one after another. To avoid
    // duplicate flushes we should check whether cache was already flushed.
    static $cache_flushed = FALSE;
    if ($cache_flushed) {
      return;
    }

    $enabled_actions = variable_get('expire_votingapi_actions', array());
    $enabled_actions = array_filter($enabled_actions);

    // Do not process current action if it is not selected by administrator.
    if (!in_array($action, $enabled_actions)) {
      return;
    }

    $expire_urls = array();

    foreach ($votes as $vote) {

      $entity_type = $vote['entity_type'];
      $entity_id = $vote['entity_id'];

      // Expire voted entity url.
      $expire_entity_page = variable_get('expire_votingapi_entity', EXPIRE_VOTINGAPI_ENTITY);
      if ($expire_entity_page) {

        $entities = entity_load($entity_type, array($entity_id));

        // Check whether entity is loaded.
        if (isset($entities[$entity_id])) {
          $entity = $entities[$entity_id];

          // Check whether entity has uri.
          $uri = entity_uri($entity_type, $entity);
          if (!empty($uri['path'])) {
            $expire_urls['vote-' . $entity_type . '-' . $entity_id] = $uri['path'];
          }
        }
      }

      // Expire custom pages.
      $expire_custom = variable_get('expire_votingapi_custom', EXPIRE_VOTINGAPI_CUSTOM);
      if ($expire_custom) {
        $pages = variable_get('expire_votingapi_custom_pages');

        // Load voted entity.
        $entities = entity_load($entity_type, array($entity_id));

        // Check whether entity is loaded.
        if (isset($entities[$entity_id])) {
          $entity = $entities[$entity_id];

          $urls = ExpireAPI::expireCustomPages($pages, array($entity_type => $entity));
          $expire_urls = array_merge($expire_urls, $urls);
        }
      }

    }

    // Flush page cache for expired urls.
    if (!empty($expire_urls)) {
      ExpireAPI::flushCache($expire_urls, 'votingapi', $votes);
    }

    $cache_flushed = TRUE;
  }

}
