<?php

/**
 * @file
 * Provides class that expires voted objects' pages.
 */

class ExpireVotingapi implements ExpireInterface {

  /**
   * Common expiry logic for votingapi.
   */
  // @todo: implement.
  public function expire($votes, $action) {
    foreach ($votes as $vote) {
      if ($vote['entity_type'] == 'comment') {
        $nid = db_query("SELECT nid FROM {comment} WHERE cid = :cid", array(':cid' => $vote['entity_id']))->fetchField();
        if (is_numeric($nid)) {
          $node = node_load($nid);
          if ($node) {
            expire_node($node);
          }
        }
      }
      if ($vote['entity_type'] == 'node') {
        $node = node_load($vote['entity_id']);
        if ($node) {
          expire_node($node);
        }
      }
    }
  }

}
