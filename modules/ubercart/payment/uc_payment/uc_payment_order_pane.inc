<?php
// $Id: uc_payment_order_pane.inc,v 1.17 2010/12/17 14:06:17 islandusurper Exp $

/**
 * @file
 * This file contains the callbacks for the payment order pane supplied with
 * Ubercart and their corresponding helper functions.
 *
 * Order panes are defined using hook_uc_order_pane() and use a callback to
 * handle the different processes involved in order viewing/editing. The
 * payment order pane is defined in uc_payment_uc_order_pane() in
 * uc_payment.module.
 */

/**
 * Handle the Payment order pane.
 */
function uc_order_pane_payment($op, $order, &$form = NULL, &$form_state = NULL) {
  switch ($op) {
    case 'view':
      if (variable_get('uc_payment_tracking', TRUE)) {
        $build['balance'] = array('#markup' => t('Balance: @balance', array('@balance' => uc_currency_format(uc_payment_balance($order)))));
        $build['view_payments'] = array(
          '#markup' => ' (' . l(t('View'), 'admin/store/orders/' . $order->order_id . '/payments') . ')',
          '#suffix' => '<br />',
        );
      }
      $method_name = _uc_payment_method_data($order->payment_method, 'review');
      if (empty($method_name)) {
        $method_name = _uc_payment_method_data($order->payment_method, 'name');
      }
      $build['method'] = array('#markup' => t('Method: @payment_method', array('@payment_method' => $method_name)));
      $func = _uc_payment_method_data($order->payment_method, 'callback');
      if (function_exists($func)) {
        $method_output = $func('order-view', $order);
        if (!empty($method_output)) {
          $build['output'] = $method_output + array(
            '#prefix' => '<br />',
          );
        }
      }

      return $build;

    case 'customer':
      $method_name = _uc_payment_method_data($order->payment_method, 'review');
      if (empty($method_name)) {
        $method_name = _uc_payment_method_data($order->payment_method, 'name');
      }
      $build['method'] = array('#markup' => t('Method: @payment_method', array('@payment_method' => $method_name)));
      $func = _uc_payment_method_data($order->payment_method, 'callback');
      if (function_exists($func)) {
        $method_output = $func('customer-view', $order);
        if (!empty($method_output)) {
          $build['output'] = $method_output + array(
            '#prefix' => '<br />',
          );
        }
      }

      return $build;

    case 'edit-form':
      drupal_add_library('system', 'drupal.progress');
      drupal_add_js(array('defPaymentMsg' => ''), 'setting');

      $form['payment'] = array();

      $methods = _uc_payment_method_list();
      foreach ($methods as $method) {
        $options[$method['id']] = $method['name'];
      }
      $form['payment']['payment_method'] = array(
        '#type' => 'select',
        '#title' => t('Payment method'),
        '#default_value' => $order->payment_method,
        '#options' => (is_array($options)) ? $options : array(t('None available')),
        '#attributes' => array('onchange' => "add_order_save_hold(); get_payment_details(Drupal.settings.ucURL.adminOrders + '" . $order->order_id . "/payment_details/' + this.value);"),
        '#disabled' => (is_array($options)) ? FALSE : TRUE,
      );
      return $form;

    case 'edit-theme':
      drupal_add_js(drupal_get_path('module', 'uc_payment') . '/uc_payment.js');

      // build a JS array of our payment methods
      $methods = _uc_payment_method_list();
      $method_array_js = array();
      foreach ($methods as $method) {
        $method_array_js[$method['name']] = $method['id'];
      }
      drupal_add_js(array('methods' => $method_array_js), 'setting');

      // begin the order lock, also get payment details
      drupal_add_js(array('ucOrderInitiate' => TRUE), 'setting');

      $output = '<table class="order-edit-table">';
      foreach (element_children($form['payment']) as $field) {
        $title = $form['payment'][$field]['#title'];
        $form['payment'][$field]['#title'] = NULL;
        $output .= '<tr><td class="oet-label">' . $title . ':</td><td>'
                 . drupal_render($form['payment'][$field]) . '</td></tr>';
      }
      $output .= '</table><div id="payment_details"></div>';
      return $output;

    case 'edit-process':
      $changes['payment_method'] = $form_state['values']['payment_method'];
      $func = _uc_payment_method_data($form_state['values']['payment_method'], 'callback');
      if (function_exists($func) && ($return = $func('edit-process', $form_state['values'])) != NULL && is_array($return)) {
        $changes = array_merge($changes, $return);
      }
      return $changes;
  }
}
