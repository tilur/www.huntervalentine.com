<?php
// $Id: uc_product_handler_field_addtocart.inc,v 1.5 2010/03/17 17:51:57 islandusurper Exp $

/**
 * @file
 * Views handler: "Add to cart" form as a field.
 */

/**
 * Display the "Add to cart" form like the product page.
 */
class uc_product_handler_field_addtocart extends views_handler_field {
  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function element_type() {
    if (isset($this->definition['element type'])) {
      return $this->definition['element type'];
    }
    return 'div';
  }

  function render($values) {
    if (uc_product_is_product($values->{$this->aliases['type']})) {
      $type = node_type_get_type($values->{$this->aliases['type']});
      $base = $type->base;
      $product = node_load($values->{$this->aliases['nid']});
      if (function_exists('theme_' . $base . '_add_to_cart')) {
        return theme($base . '_add_to_cart', array('node' => $product));
      }
      elseif (function_exists('theme_uc_product_add_to_cart')) {
        return theme('uc_product_add_to_cart', array('node' => $product));
      }
    }
  }
}
