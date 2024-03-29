<?php
// $Id: uc_taxes.api.php,v 1.4 2010/07/16 12:57:37 islandusurper Exp $

/**
 * @file
 * Hooks provided by the Taxes module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Calculate tax line items for an order.
 *
 * @param $order
 *   An order object or an order id.
 * @return
 *   An array of tax line item objects keyed by a module-specific id.
 */
function hook_uc_calculate_tax($order) {
  if (!is_object($order)) {
    return array();
  }
  if (empty($order->delivery_postal_code)) {
    $order->delivery_postal_code = $order->billing_postal_code;
  }
  if (empty($order->delivery_zone)) {
    $order->delivery_zone = $order->billing_zone;
  }
  if (empty($order->delivery_country)) {
    $order->delivery_country = $order->billing_country;
  }

  $order->taxes = array();

  if (isset($order->order_status)) {
    $state = uc_order_status_data($order->order_status, 'state');
    $use_same_rates = in_array($state, array('payment_received', 'completed'));
  }
  else {
    $use_same_rates = FALSE;
  }

  $use_rules = module_exists('rules');

  foreach (uc_taxes_rate_load() as $tax) {
    if ($use_same_rates) {
      foreach ((array)$order->line_items as $old_line) {
        if ($old_line['type'] == 'tax' && $old_line['data']['tax_id'] == $tax->id) {
          $tax->rate = $old_line['data']['tax_rate'];
          break;
        }
      }
    }

    if ($use_rules) {
      $set = rules_config_load('uc_taxes_' . $tax->id);
      $apply = $set->execute($order);
    }
    else {
      $apply = TRUE;
    }

    if ($apply) {
      $line_item = uc_taxes_apply_tax($order, $tax);
      if ($line_item) {
        $order->taxes[$line_item->id] = $line_item;
      }
    }
  }

  return $order->taxes;
}

/**
 * @} End of "addtogroup hooks".
 */

