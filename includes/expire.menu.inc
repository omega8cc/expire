<?php

/**
 * @file
 * Provides class that expires menu items' pages.
 */

class ExpireMenu implements ExpireInterface {

  // @todo: implement.
  function expire($menu, $action) {

  }

  /**
   * Finds parent, siblings and children of the menu item. UGLY CODE...
   *
   * @param array $menu
   *  Output from menu_tree_all_data().
   *
   * @param bool $found
   *  Signal for when the needle was found in the menu array.
   *  Set TRUE to get entire menu.
   *
   * @param string $needle
   *  Name of menu link. Example 'node/21'.
   *
   * @param bool $first
   *  Keep track of the first call; this is a recursive function.
   *
   * @param bool &$found_global
   *  Used to signal the parent item was found in one of it's children.
   *
   * @param bool &$menu_out
   *  Output array of parent, siblings and children menu links.
   *
   * @return array
   */
  private function getMenuStructure($menu, $found, $needle, $first, &$found_global, &$menu_out) {
    // Set Defaults
    $found = !is_null($found) ? $found : TRUE;
    $needle = !is_null($needle) ? $needle : '';
    $first = !is_null($first) ? $first : TRUE;
    $found_global = FALSE;
    $menu_out = !is_null($menu_out) ? $menu_out : array();

    // Get Siblings
    foreach ($menu as $item) {
      if ($item['link']['hidden'] == 0 && $item['link']['page_callback'] != '' && ($item['link']['link_path'] == $needle || $found)) {
        $menu_out[] = $item['link']['link_path'];
        $found = TRUE;
      }
    }
    // Get Children
    foreach ($menu as $item) {
      if ($item['link']['hidden'] != 0) {
        continue;
      }
      if ($item['link']['page_callback'] != '' && ($item['link']['link_path'] == $needle || $found)) {
        $menu_out[] = $item['link']['link_path'];
        $found = TRUE;
      }
      // Get Grandkids
      if (!empty($item['below'])) {
        $sub_menu = array();
        foreach ($item['below'] as $below) {
          if ($below['link']['hidden'] == 0) {
            $sub_menu[] = $below;
          }
        }
        expire_get_menu_structure($sub_menu, $needle, $found, FALSE, $found_global, $menu_out);
        $structure[$item['link']['link_path']][] = $sub_menu;
        if ($item['link']['page_callback'] != '' && $found_global) {
          // Get Parent of kid
          $menu_out[] = $item['link']['link_path'];
        }
      }
      else {
        $structure[$item['link']['link_path']] = '';
      }
    }

    // Clean up
    if (isset($structure)) {
      if (is_array($structure)) {
        $structure = array_unique($structure);
      }
    }
    else {
      $structure = array();
    }
    $found_global = $found;
    if ($first) {
      if (isset($menu_out) && is_array($menu_out)) {
        $menu_out = array_unique($menu_out);
        sort($menu_out);
        return $menu_out;
      }
      else {
        return array();
      }
    }
    else {
      return $structure;
    }
  }

}
