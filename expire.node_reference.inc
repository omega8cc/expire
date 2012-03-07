<?php

function expire_get_node_references($node)  {
  $paths = array();
  $fields= field_info_instances('node', $node->type);
  $field_types = field_info_fields();
  foreach ($fields as $field) {
    // Add referenced node paths and expire referenced nodes
    // when the referencing node is updated.
    if ($field_types[$field['field_name']]['type'] == 'node_reference') {
      $node_field = isset($node->$field['field_name']) && !empty($node->$field['field_name']) ? $node->$field['field_name'] : array();
      foreach($node_field[LANGUAGE_NONE] as $reference) {
        $paths['reference' . $reference['nid']] = 'node/'. $reference['nid'];
      }
    }
  }
  return $paths;
}
