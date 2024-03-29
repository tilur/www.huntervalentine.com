<?php
// $Id: uc_order.line_item.inc,v 1.11 2010/08/25 13:40:25 islandusurper Exp $

/**
 * @file
 * This file contains the callbacks for the default line items for orders and
 * the various functions that make line items work.
 *
 * Line items are defined using hook_uc_line_item() and use a callback to
 * handle the different processes involved in line item
 * viewing/editing/calculating. The default line items are defined in
 * uc_order_uc_line_item() in uc_order.module.
 */

/**
 * Handle the subtotal line item.
 */
function uc_line_item_subtotal($op, $order) {
  switch ($op) {
    case 'load':
      $lines[] = array(
        'id' => 'subtotal',
        'title' => t('Subtotal'),
        'amount' => uc_order_get_total($order, TRUE),
      );
      return $lines;
  }
}

/**
 * Handle the total line item.
 */
function uc_line_item_total($op, $order) {
  switch ($op) {
    case 'display':
      $lines[] = array(
        'id' => 'total',
        'title' => t('Total'),
        'amount' => uc_order_get_total($order),
      );
      return $lines;
  }
}

/**
 * Calculate the total value of line items of types that should be calculated.
 */
function uc_line_items_calculate($order) {
  $total = 0;

  if (is_array($order->line_items)) {
    foreach ($order->line_items as $item) {
      if (_uc_line_item_data($item['type'], 'calculated') == TRUE) {
        $total += $item['amount'];
      }
    }
  }

  return $total;
}

/**
 * Update a line item.
 */
function uc_order_update_line_item($id, $title, $amount, $data = NULL) {
  $fields = array(
    'title' => $title,
    'amount' => $amount,
  );

  if (!is_null($data)) {
    $fields['data'] = serialize($data);
  }

  db_update('uc_order_line_items')
    ->fields($fields)
    ->condition('line_item_id', $id)
    ->execute();

  return TRUE;
}

/**
 * Delete a line item, or pass $order as TRUE and $id as an order_id to delete
 * every line item attached to an order.
 */
function uc_order_delete_line_item($id, $order = FALSE) {
  if ($order === FALSE) {
    db_delete('uc_order_line_items')
      ->condition('line_item_id', $id)
      ->execute();
  }
  else {
    db_delete('uc_order_line_items')
      ->condition('order_id', $id)
      ->execute();
  }

  return TRUE;
}

/**
 * Add a line item to an order.
 */
function uc_order_line_item_add($order_id, $type, $title, $amount, $weight = NULL, $data = NULL) {
  if (is_null($weight)) {
    $weight = _uc_line_item_data($type, 'weight');
  }

  $line_item = array(
    'order_id' => $order_id,
    'type' => $type,
    'title' => $title,
    'amount' => $amount,
    'weight' => $weight,
    'data' => $data,
  );
  drupal_write_record('uc_order_line_items', $line_item);

  return TRUE;
}

/**
 * Build a list of line items defined in the enabled modules.
 */
function _uc_line_item_list($action = NULL) {
  static $items;

  if (count($items) > 0 && $action !== 'rebuild') {
    return $items;
  }

  $items = module_invoke_all('uc_line_item', NULL);
  foreach ($items as $i => $value) {
    $items[$i]['enabled'] = variable_get('uc_li_' . $items[$i]['id'] . '_enabled', (!isset($items[$i]['enabled']) ? TRUE : $items[$i]['enabled']));
    $items[$i]['weight'] = variable_get('uc_li_' . $items[$i]['id'] . '_weight', (!isset($items[$i]['weight']) ? 1 : $items[$i]['weight']));
  }

  drupal_alter('uc_line_item_data', $items);

  usort($items, 'uc_weight_sort');

  return $items;
}

/**
 * Return data from a line item by ID and the array key.
 */
function _uc_line_item_data($item_id, $key) {
  $items = _uc_line_item_list();
  foreach ($items as $item) {
    if ($item['id'] == $item_id) {
      return $item[$key];
    }
  }
}
