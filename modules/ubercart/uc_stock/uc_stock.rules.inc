<?php
// $Id: uc_stock.rules.inc,v 1.1 2010/07/16 12:57:36 islandusurper Exp $

/**
 * @file
 * Rules hooks for uc_stock.module.
 */

/**
 * Implement hook_rules_action_info().
 */
function uc_stock_rules_action_info() {
  $actions['uc_stock_action_decrement_stock'] = array(
    'label' => t('Decrement stock of products on the order with tracking activated.'),
    'group' => t('Stock'),
    'base' => 'uc_stock_action_decrement_stock',
    'parameter' => array(
      'order' => array(
        'type' => 'uc_order',
        'label' => t('Order'),
      ),
    ),
  );

  return $actions;
}

/**
 * Decrease the stock of ordered products.
 */
function uc_stock_action_decrement_stock($order) {
  if (is_array($order->products)) {
    array_walk($order->products, 'uc_stock_adjust_product_stock', $order);
  }
}
