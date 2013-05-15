<?php

/**
 * @file
 * Interface that should implements class to be added in Expiration group.
 */

interface ExpireInterface {

  /**
   * Expires urls for the object.
   *
   * @param $object
   *   Some object (user, node, comment, etc).
   *
   * @param $action
   *   Action that was executed for the object.
   */
  public static function expire($object, $action);

}
